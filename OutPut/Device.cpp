/**
 * @file Device.cpp
 * @brief Implémentation de la classe Device pour le rendu rasterisé 3D.
 *
 * Ce fichier contient les méthodes principales de la classe Device, responsable du rendu,
 * de la gestion des buffers, et de l'application des effets (textures, normal maps, parallax, etc).
 *
 * @author Alexandre STEIN
 * @date 17/07/2020
 */

#include "Device.hpp"
#include <iostream>
#include "../OutPut/AmbientOcclusion.h"
#include <thread>
#include <mutex>
#include "../Tools/ThreadPool.hpp"

using namespace Render3D;

/**
 * @brief Constructeur de la classe Device
 * @param Width Largeur de l'image de rendu
 * @param Height Hauteur de l'image de rendu
 */
Device::Device(int Width, int Height)
{
    _width = Width;
    _height = Height;
    int length = GetWidth() * GetHeight();
    _depthbuffer = new std::atomic<float>[GetWidth() * GetHeight()]; // float[GetWidth()*GetHeight()];
    _normalBuffer = new vec3[GetWidth() * GetHeight()];
    _imageZbuffer = new unsigned char[GetWidth() * GetHeight()];
    _imagePPM = new unsigned char[GetWidth() * GetHeight() * 3];
    _imageNormal = new unsigned char[GetWidth() * GetHeight() * 3];

    for (int i = 0; i < (length * 3); i++)
    {
        *(_imagePPM + i) = 0;
        *(_imageNormal + i) = 255;
    }

    for (int i = 0; i < length; i++)
    {
        // *(_depthbuffer + i) = 800.0f;//numeric_limits<float>::max();
        _depthbuffer[i].store(500.0f);
        *(_imageZbuffer + i) = 255;
        _normalBuffer[i] = vec3(0.0f);
    }
};

/**
 * @brief Dessine un point dans l'image de rendu
 * @param x Coordonnée x du pixel
 * @param y Coordonnée y du pixel
 * @param r Valeur rouge (0-255)
 * @param g Valeur verte (0-255)
 * @param b Valeur bleue (0-255)
 */
void Device::SetPixelColor(int x, int y, float r, float g, float b)
{
    _imagePPM[(y * GetWidth() + x) * 3] = (unsigned char)r;
    _imagePPM[((y * GetWidth() + x) * 3) + 1] = (unsigned char)g;
    _imagePPM[((y * GetWidth() + x) * 3) + 2] = (unsigned char)b;
};

/**
 * @brief Projette une coordonnée 3D en 2D (version vec2)
 * @param coordinate Coordonnée 3D à projeter
 * @param projection Matrice de projection
 * @param out Coordonnée projetée (en sortie)
 */
float Device::Projection_3D_to_2D(vec3 &coordinate, const mat4x4 &projection, vec3 &out)
{
    vec3 tmp;
    float w = TransformVectorByMatrix4x4(coordinate, projection, tmp);
    out.x = GetWidth() * (tmp.x + 1.0f) * 0.5f;
    out.y = GetHeight() * (-tmp.y + 1.0f) * 0.5f;
    out.z = tmp.z;
    return w;
};

/**
 * @brief Retourne la largeur de l'image de rendu
 * @return Largeur
 */
int Device::GetWidth()
{
    return _width;
}

/**
 * @brief Retourne la hauteur de l'image de rendu
 * @return Hauteur
 */
int Device::GetHeight()
{
    return _height;
}

// Computing the minimum among the three points.
double MinOfThree(double &a, const double &b, const double &c)
{
    return std::min(a, std::min(b, c));
}

// Computing the maximum among the three points.
double MaxOfThree(const double &a, const double &b, const double &c)
{
    return std::max(a, std::max(b, c));
}

// Computing the minimum among the three points.
float MinOfThree(float &a, const float &b, const float &c)
{
    return std::min(a, std::min(b, c));
}

// Computing the maximum among the three points.
float MaxOfThree(const float &a, const float &b, const float &c)
{
    return std::max(a, std::max(b, c));
}

float ComputeEdgeFunction(vec3 &a, vec3 &b, vec3 &c)
{
    return ((c.x - a.x) * (b.y - a.y)) - ((c.y - a.y) * (b.x - a.x));
}

/**
 * @brief Remplit un triangle à l'écran avec gestion du Z-buffer, textures, normales, etc.
 * @param proj Matrice de projection
 * @param world Matrice de transformation monde
 * @param f Face à rasteriser
 * @param mesh Maillage
 * @param l Lumières
 * @param camera Caméra
 * @param projected_coordinates Coordonnées projetées des sommets
 * @param world_coordinates Coordonnées monde des sommets
 * @param Z_correction Correction de profondeur
 * @param tex Texture couleur
 * @param nTex Texture de normales
 * @param pTex Texture de parallax mapping
 */
void Device::RasterizeTriangle(const mat4x4 &proj, const mat4x4 &world, const mat3x3 &normalMatrix, Face f, Mesh mesh, Lights l, std::shared_ptr<Camera> camera, vector<dvec3> projected_coordinates, vector<vec3> world_coordinates, vector<float> Z_correction, Textures &tex, TextureNormalMap &nTex, TextureParallaxMapping &pTex)
{
    float Wrgb = 0.0f;
    float Wr = 0.0f;
    float Wg = 0.0f;
    float Wb = 0.0f;
    float Weight_RED = 0.0f;
    float Weight_GREEN = 0.0f;
    float Weight_BLUE = 0.0f;
    bool write = true;

    vector<vec3> vertices = mesh.get_vertices();
    vector<vec3> normals = mesh.get_normals();
    vector<vec2> uv = mesh.get_uvs();

    vec3 a = projected_coordinates[0];
    vec3 b = projected_coordinates[1];
    vec3 c = projected_coordinates[2];

    float wa = Z_correction[0];
    float wb = Z_correction[1];
    float wc = Z_correction[2];

    Wrgb = ComputeEdgeFunction(a, b, c);

    float x_max = MaxOfThree(a.x, b.x, c.x);
    float x_min = MinOfThree(a.x, b.x, c.x);

    float y_max = MaxOfThree(a.y, b.y, c.y);
    float y_min = MinOfThree(a.y, b.y, c.y);

    int x0 = static_cast<int>(std::max(0.0f, x_min));
    int x1 = static_cast<int>(std::min(GetWidth() - 1.0f, x_max));
    int y0 = static_cast<int>(std::max(0.0f, y_min));
    int y1 = static_cast<int>(std::min(GetHeight() - 1.0f, y_max));

    // To transform coordinate by rotation, translation and/or scaling from polygon.
    vec3 a_world = world_coordinates[0];
    vec3 b_world = world_coordinates[1];
    vec3 c_world = world_coordinates[2];

    // Here, it's possible to enhance with the multithreading
    for (int y = y0; y <= y1; y++)
    {

        vec3 p(x0 + 0.5f, y + 0.5f, 0.0);

        // Computing weights.
        Weight_RED = ComputeEdgeFunction(b, c, p);
        Weight_GREEN = ComputeEdgeFunction(c, a, p);
        Weight_BLUE = ComputeEdgeFunction(a, b, p);

        for (int x = x0; x <= x1; x++)
        {
            // The point is in the triangle.
            if (Weight_RED >= 0.0f && Weight_GREEN >= 0.0f && Weight_BLUE >= 0.0f)
            {
                Wr = Weight_RED / Wrgb;
                Wg = Weight_GREEN / Wrgb;
                Wb = Weight_BLUE / Wrgb;

                float invW_interp = (Wr / wa) + (Wg / wb) + (Wb / wc);

                float Z = a.z * Wr + b.z * Wg + c.z * Wb;

                // Test of Z-buffer.
                if (Z > _depthbuffer[y * GetWidth() + x].load())
                {
                    write = false;
                }
                else
                {
                    _depthbuffer[y * GetWidth() + x].store(Z);
                    float contrast = std::pow(Z, 10.0f);
                    _imageZbuffer[y * GetWidth() + x] = static_cast<unsigned char>(contrast * 255.0f);
                    write = true;
                }

                // Set variable "weight" by the weight computed previously.
                vec3 weight{};
                weight.x = Wr;
                weight.y = Wg;
                weight.z = Wb;

                // Draw the pixel
                if (write)
                {
                    vec3 normalsA = normalize(normalMatrix * normals[f.A.IndiceNormals - 1]);
                    vec3 normalsB = normalize(normalMatrix * normals[f.B.IndiceNormals - 1]);
                    vec3 normalsC = normalize(normalMatrix * normals[f.C.IndiceNormals - 1]);

                    _normalBuffer[y * GetWidth() + x] = normalsA * Wr + normalsB * Wg + normalsC * Wb;

                    _imageNormal[(y * GetWidth() + x) * 3] = (unsigned char)((_normalBuffer[y * GetWidth() + x].x * 0.5f + 0.5f) * 255.0f);
                    _imageNormal[((y * GetWidth() + x) * 3) + 1] = (unsigned char)((_normalBuffer[y * GetWidth() + x].y * 0.5f + 0.5f) * 255.0f);
                    _imageNormal[((y * GetWidth() + x) * 3) + 2] = (unsigned char)((_normalBuffer[y * GetWidth() + x].z * 0.5f + 0.5f) * 255.0f);

                    // Interpolation de la texture.
                    float u = (uv[f.A.IndiceTexCoords - 1].x / wa * weight.x + uv[f.B.IndiceTexCoords - 1].x / wb * weight.y + uv[f.C.IndiceTexCoords - 1].x / wc * weight.z);
                    float v = (uv[f.A.IndiceTexCoords - 1].y / wa * weight.x + uv[f.B.IndiceTexCoords - 1].y / wb * weight.y + uv[f.C.IndiceTexCoords - 1].y / wc * weight.z);

#pragma region Parallax Mapping
                    mat3x3 TBN{};
                    if (pTex.getLoaded() == true)
                    {
                        TBN = nTex.getTBN(f, a_world, b_world, c_world, uv, weight, normalMatrix);
                        vec3 tangentialCamPos{};
                        TransformVectorByMatrix3x3(camera->get_position(), transpose(TBN), tangentialCamPos);
                        vec3 tangentialFragPos{};
                        vec3 point3D_position = a_world * weight.x + b_world * weight.y + c_world * weight.z;
                        TransformVectorByMatrix3x3(point3D_position, transpose(TBN), tangentialFragPos);

                        vec3 viewDirection = tangentialCamPos - tangentialFragPos;

                        pTex.setPixel(std::min(1.0f, u / (invW_interp)), std::min(1.0f, v / (invW_interp)));
                        u = std::min(1.0f, std::max(0.0f, u / (invW_interp)));
                        v = std::min(1.0f, std::max(0.0f, v / (invW_interp)));
                        vec2 uv = {u, v};
                        vec2 dis{};
                        dis = pTex.getParallaxMapping(uv, normalize(viewDirection));

                        if (dis.x <= 1.0f && dis.x >= 0.0f && dis.y <= 1.0f && dis.y >= 0.0f)
                        {
                            u = dis.x;
                            v = dis.y;
                        }
                    }
#pragma endregion Parallax Mapping

                    if (tex.getLoaded() == true)
                    {
                        if (pTex.getLoaded() == true)
                        {
                            tex.setPixel(u, v);
                        }
                        else
                        {
                            tex.setPixel(std::min(1.0f, u / (invW_interp)), std::min(1.0f, v / (invW_interp)));
                        }
                    }

                    vec3 normalMap{};
                    if (nTex.getLoaded() == true)
                    {

                        if (pTex.getLoaded() == true)
                        {
                            nTex.setPixel(u, v);
                            normalMap = nTex.GetPixelNormal(f, a_world, b_world, c_world, uv, weight, normalMatrix, true);
                            normalMap = transpose(TBN) * normalMap;
                        }
                        else
                        {
                            nTex.setPixel(std::min(1.0f, u / (invW_interp)), std::min(1.0f, v / (invW_interp)));
                            normalMap = nTex.GetPixelNormal(f, a_world, b_world, c_world, uv, weight, normalMatrix);
                        }
                        _normalBuffer[y * GetWidth() + x] = normalMap;
                        _imageNormal[(y * GetWidth() + x) * 3] = (unsigned char)((normalMap.x * 0.5f + 0.5f) * 255.0f);
                        _imageNormal[((y * GetWidth() + x) * 3) + 1] = (unsigned char)((normalMap.y * 0.5f + 0.5f) * 255.0f);
                        _imageNormal[((y * GetWidth() + x) * 3) + 2] = (unsigned char)((normalMap.z * 0.5f + 0.5f) * 255.0f);
                    }

                    if (nTex.getLoaded() == true)
                    {
                        l.preCompute(weight, a_world, b_world, c_world, normalMap, normalMap, normalMap);
                        l.ComputeLightPhong(TBN, pTex.getLoaded());
                        l.ComputeSpecular(camera->get_position(), 64, TBN, pTex.getLoaded());
                    }
                    else
                    {
                        l.preCompute(weight, a_world, b_world, c_world, normalsA, normalsB, normalsC);
                        l.ComputeLightPhong();
                        l.ComputeSpecular(camera->get_position(), 64);
                    }

                    //   l.ComputeAttenuation("pointlight", 0.014f, 0.000007f,20.0f);
                    //    l.ComputeAttenuation("pointlight", 0.22f,  0.20f,   20.0f);
                    //    l.ComputeAttenuation("pointlight",0.09f,  0.032f,  10.0f);

                    float innerAngle = 12.5f;
                    float outerAngle = 17.5f;

                    float cutoff = cos(glm::radians(innerAngle));
                    float outerCutoff = cos(glm::radians(outerAngle));
                    l.ComputeSpotLight("spot", cutoff, outerCutoff);

                    vec3 I = l.getIntensity(weight, f);

                    if (tex.getLoaded() == true)
                    {
                        SetPixelColor(x, y, I.x * tex.getRed(), I.y * tex.getGreen(), I.z * tex.getBlue());
                    }
                    else
                    {
                        if (l.getConstantKd().x == 0 && l.getConstantKd().y == 0 && l.getConstantKd().z == 0)
                        {
                            SetPixelColor(x, y, I.x * 127.0f, I.y * 127.0f, I.z * 127.0f);
                        }
                        else
                        {
                            SetPixelColor(x, y, I.x * l.getConstantKd().x, I.y * l.getConstantKd().y, I.z * l.getConstantKd().z);
                        }
                    }
                }
            }

            Weight_RED = Weight_RED + c.y - b.y;
            Weight_GREEN = Weight_GREEN + a.y - c.y;
            Weight_BLUE = Weight_BLUE + b.y - a.y;
        }
    }
}

/**
 * @brief Lance le rendu de la scène complète
 * @param camera Caméra
 * @param meshes Maillage
 * @param l Lumières
 */
void Device::RenderScene(std::shared_ptr<Camera> camera, Mesh meshes, Lights &l)
{
    mat4x4 proj, view;
    vec3 unitY{};
    unitY.x = 0.0f;
    unitY.y = 1.0f;
    unitY.z = 0.0f;

    BuildViewMatrix(camera->get_position(), camera->get_target(), unitY, view);

    float scale = (static_cast<float>(GetWidth()) / static_cast<float>(GetHeight()));

    BuildPerspectiveMatrix(45.0f, scale, 1.0f, 100.0f, proj);

    mat4x4 T, Rx, Ry, Rz, Rm, S;

    auto t_start = std::chrono::high_resolution_clock::now();

    vector<MeshData> md = meshes.get_meshData();

    int i = 0;
    for (MeshData me : md)
    {
        Rotation_X_Pitch(Rx, meshes.get_rotation(i).x);
        Rotation_Y_Yaw(Ry, meshes.get_rotation(i).y);
        Rotation_Z_Roll(Rz, meshes.get_rotation(i).z);
        Rotation_XYZ_PitchYawRoll(Rx, Ry, Rz, Rm);
        BuildTranslationMatrix(meshes.get_position(i), T);
        Scale(1.0f, S);

        const mat4x4 WorldMatrix = T * Rm * S;
        const mat4x4 transformMatrix = proj * view * WorldMatrix;

        Frustum frustum;
        frustum.extractFromMatrix(transformMatrix);

        // Extraire la matrice 3x3 (rotation + échelle) de WorldMatrix
        mat3x3 normalMatrix = mat3x3(WorldMatrix);

        // Calculer l'inverse-transpose
        normalMatrix = transpose(inverse(normalMatrix));

        vector<dvec3> projected_coordinates{};
        vector<vec3> world_coordinates{};
        vector<float> Z_correction{};
        vector<float> xy_min{};
        vector<float> xy_max{};

        int j = 0;
        ThreadPool threadPool(4);

        for (const Face &face : me.faces)
        {
            l.setConstantLight(meshes.get_ConstantLight(i, j));

            vector<vec3> vertices = meshes.get_vertices();

            // 1. Récupérer les 3 sommets du triangle en espace monde
            vec3 a_world, b_world, c_world;
            TransformVectorByMatrix4x4(vertices[face.A.IndiceVertices - 1], WorldMatrix, a_world);
            TransformVectorByMatrix4x4(vertices[face.B.IndiceVertices - 1], WorldMatrix, b_world);
            TransformVectorByMatrix4x4(vertices[face.C.IndiceVertices - 1], WorldMatrix, c_world);

            // ========== FRUSTUM CULLING (NOUVEAU) ==========
            if (frustum.isTriangleOutside(a_world, b_world, c_world))
            {
                j++;
                continue; // Triangle hors du frustum
            }
            // ================================================

            // ========== BACKFACE CULLING ==========

            // 2. Calculer la normale de la face en espace monde
            vec3 edge1 = {b_world.x - a_world.x, b_world.y - a_world.y, b_world.z - a_world.z};
            vec3 edge2 = {c_world.x - a_world.x, c_world.y - a_world.y, c_world.z - a_world.z};

            // Produit vectoriel pour obtenir la normale
            vec3 faceNormal = {
                edge1.y * edge2.z - edge1.z * edge2.y,
                edge1.z * edge2.x - edge1.x * edge2.z,
                edge1.x * edge2.y - edge1.y * edge2.x};

            // Normaliser
            float length = sqrt(faceNormal.x * faceNormal.x +
                                faceNormal.y * faceNormal.y +
                                faceNormal.z * faceNormal.z);

            if (length > 0.0f)
            {
                faceNormal.x /= length;
                faceNormal.y /= length;
                faceNormal.z /= length;
            }

            // 3. Calculer le vecteur vers la caméra
            vec3 centroid = {
                (a_world.x + b_world.x + c_world.x) / 3.0f,
                (a_world.y + b_world.y + c_world.y) / 3.0f,
                (a_world.z + b_world.z + c_world.z) / 3.0f};

            vec3 cameraPos = camera->get_position();
            vec3 toCamera = {
                cameraPos.x - centroid.x,
                cameraPos.y - centroid.y,
                cameraPos.z - centroid.z};

            // Normaliser toCamera
            length = sqrt(toCamera.x * toCamera.x +
                          toCamera.y * toCamera.y +
                          toCamera.z * toCamera.z);

            if (length > 0.0f)
            {
                toCamera.x /= length;
                toCamera.y /= length;
                toCamera.z /= length;
            }

            // 4. Produit scalaire pour le test de visibilité
            float dotProduct = faceNormal.x * toCamera.x +
                               faceNormal.y * toCamera.y +
                               faceNormal.z * toCamera.z;

            // 5. Si la face est orientée vers l'arrière, on la skip
            if (dotProduct < 0.0f)
            {
                j++;
                continue; // Face arrière, on ne la rasterise pas
            }

            // ========== FIN BACKFACE CULLING ==========

            vec3 a, b, c;
            Z_correction.push_back(Projection_3D_to_2D(vertices[face.A.IndiceVertices - 1], transformMatrix, a));
            projected_coordinates.push_back(a);
            Z_correction.push_back(Projection_3D_to_2D(vertices[face.B.IndiceVertices - 1], transformMatrix, b));
            projected_coordinates.push_back(b);
            Z_correction.push_back(Projection_3D_to_2D(vertices[face.C.IndiceVertices - 1], transformMatrix, c));
            projected_coordinates.push_back(c);

            xy_max.push_back(MaxOfThree(a.x, b.x, c.x));
            xy_min.push_back(MinOfThree(a.x, b.x, c.x));
            xy_max.push_back(MaxOfThree(a.y, b.y, c.y));
            xy_min.push_back(MinOfThree(a.y, b.y, c.y));

            if (xy_min[0] > GetWidth() - 1.0f || xy_max[0] < 0.0f ||
                xy_min[1] > GetHeight() - 1.0f || xy_max[1] < 0.0f)
            {
                projected_coordinates.clear();
                Z_correction.clear();
                xy_min.clear();
                xy_max.clear();
                j++;
                continue;
            }

            world_coordinates.push_back(a_world);
            world_coordinates.push_back(b_world);
            world_coordinates.push_back(c_world);

            Lights localLight = Lights(l);
            localLight.setConstantLight(meshes.get_ConstantLight(i, j));

            threadPool.enqueue([this, transformMatrix, WorldMatrix, normalMatrix, face, meshes, localLight, camera,
                                projected_coordinates, world_coordinates, Z_correction]()
                               {

                Textures tex = Textures(localLight.getPathTexture());
                TextureNormalMap nTex = TextureNormalMap(localLight.getPathTextureBump());
                TextureParallaxMapping pTex = TextureParallaxMapping(localLight.getPathTextureDisp(),0.15f);
                
                RasterizeTriangle(transformMatrix, WorldMatrix, normalMatrix, face, meshes, localLight, camera, 
                                projected_coordinates, world_coordinates, Z_correction, tex, nTex, pTex); });

            projected_coordinates.clear();
            world_coordinates.clear();
            Z_correction.clear();
            xy_min.clear();
            xy_max.clear();
            j++;
        }

        i++;
    }

    auto t_end = std::chrono::high_resolution_clock::now();
    auto renderingTime = std::chrono::duration<double, std::milli>(t_end - t_start).count();
    std::cout << std::endl;
    std::cout << "Total rendering time : " << (renderingTime / 1000) << " s" << std::endl;

    std::filesystem::create_directories("./RenderedImages");

    std::ofstream ofsz("./RenderedImages/output_zbuffer.pgm", std::ios::binary);
    ofsz << "P5\n"
         << GetWidth() << " " << GetHeight() << "\n255\n";
    ofsz.write((char *)_imageZbuffer, GetWidth() * GetHeight());
    ofsz.close();

    std::ofstream ofsn("./RenderedImages/output_normal.ppm", std::ios::binary);
    ofsn << "P6\n"
         << GetWidth() << " " << GetHeight() << "\n255\n";
    ofsn.write((char *)_imageNormal, GetWidth() * GetHeight() * 3);
    ofsn.close();

    std::ofstream ofs("./RenderedImages/output_rendered.ppm", std::ios::binary);
    ofs << "P6\n"
        << GetWidth() << " " << GetHeight() << "\n255\n";
    ofs.write((char *)_imagePPM, GetWidth() * GetHeight() * 3);
    ofs.close();

    auto t_end_render = std::chrono::high_resolution_clock::now();

    std::cout << "Applying Screen-Space Reflections..." << std::endl;
    auto t_ssr_start = std::chrono::high_resolution_clock::now();
    ApplyScreenSpaceReflections(camera, view, proj);

    std::ofstream ofssr("./RenderedImages/output_screen_space_reflections.ppm", std::ios::binary);
    ofssr << "P6\n"
          << GetWidth() << " " << GetHeight() << "\n255\n";
    ofssr.write((char *)_imagePPM, GetWidth() * GetHeight() * 3);
    ofssr.close();

    auto t_ssr_end = std::chrono::high_resolution_clock::now();
    auto ssrTime = std::chrono::duration<double, std::milli>(t_ssr_end - t_ssr_start).count();
    std::cout << "SSR pass took: " << (ssrTime / 1000) << " s" << std::endl;
}

/**
 * @brief Destructeur de la classe Device
 */
Device::~Device()
{
    delete[] _depthbuffer;
    delete[] _imagePPM;
    delete[] _imageZbuffer;
    delete[] _imageNormal;
}

/**
 * @brief Applique le Screen-Space Reflection (SSR) à l'image de rendu
 * @param camera Caméra
 * @param view Matrice de vue
 * @param proj Matrice de projection
 */
using namespace std;
float LinearizeDepth(float depth, float near, float far)
{
    float z = depth * 2.0f - 1.0f; // Retour en NDC [-1,1]
    return (2.0f * near * far) / (far + near - z * (far - near));
}

void Device::ApplyScreenSpaceReflections(const std::shared_ptr<Camera> &camera, const mat4x4 &view, const mat4x4 &proj)
{
    std::cout << "Starting SSR..." << std::endl;
    mat4x4 invProjView = inverse(proj * view);

    for (int y = 0; y < _height; y++)
    {
        for (int x = 0; x < _width; x++)
        {

            float z = GetPixelDepth(x, y);

            if (z < 0.0f || z > 1.0f)
                continue;

            vec4 positionNdc{
                2.0f * (static_cast<float>(x) / GetWidth()) - 1.0f,
                -2.0f * (static_cast<float>(y) / GetHeight()) + 1.0f,
                z * 2.0f - 1.0f,
                1.0f};

            vec4 positionWorld4 = invProjView * positionNdc;
            positionWorld4 /= positionWorld4.w;

            vec3 normal = GetPixelNormal(x, y);
            vec3 positionWorld{positionWorld4};
            vec3 viewDirWorld = normalize(positionWorld - camera->get_position());
            vec3 reflection = reflect(viewDirWorld, normal);

            // ⭐ FRESNEL - Calculer l'intensité de réflexion selon l'angle
            float NdotV = std::max(dot(normal, -viewDirWorld), 0.0f);
            float F0 = 0.5f; // 0.04f; // Réflectance à incidence normale (matériau diélectrique standard)
            float fresnel = F0 + (1.0f - F0) * pow(1.0f - NdotV, 5.0f);

            vec3 startRay = positionWorld + normal * 0.01f; // 0.0005f;

            int maxStep = 1000;
            vec3 curPos = startRay;

            vec4 startNdc = proj * view * vec4{startRay, 1.0f};
            startNdc /= startNdc.w;

            bool hit = false;
            vec3 hitColor{};
            float prevPosZ = startNdc.z * 0.5f + 0.5f;

            for (int i = 0; i < maxStep; i++)
            {

                vec4 curPos4{curPos, 1.0f};
                vec4 curPosNdc = proj * view * curPos4;
                curPosNdc /= curPosNdc.w;

                if (curPosNdc.x < -1.0f || curPosNdc.x > 1.0f ||
                    curPosNdc.y < -1.0f || curPosNdc.y > 1.0f ||
                    curPosNdc.z < -1.0f || curPosNdc.z > 1.0f)
                    break;

                int screenX = GetWidth() * (curPosNdc.x + 1.0f) * 0.5f;
                int screenY = GetHeight() * (-curPosNdc.y + 1.0f) * 0.5f;

                if (screenX < 0 || screenX >= GetWidth() || screenY < 0 || screenY >= GetHeight())
                    break;

                float pixelDepth = GetPixelDepth(screenX, screenY);
                float curPosZ = curPosNdc.z * 0.5f + 0.5f;

                float linearPixelDepth = LinearizeDepth(pixelDepth, 1.0f, 100.0f);
                float linearCurPosZ = LinearizeDepth(curPosZ, 1.0f, 100.0f);
                float linearPrevPosZ = LinearizeDepth(prevPosZ, 1.0f, 100.0f);

                // ⭐ Calculer la distance à la surface
                float distanceToSurface = abs(linearCurPosZ - linearPixelDepth);

                bool crossing = (linearPrevPosZ < linearPixelDepth && linearCurPosZ >= linearPixelDepth) ||
                                (linearPrevPosZ > linearPixelDepth && linearCurPosZ <= linearPixelDepth);

                if (crossing && distanceToSurface < 0.5f)
                {
                    hit = true;
                    hitColor = GetPixelAlbedo(screenX, screenY);
                    break;
                }

                // ⭐ Pas adaptatif selon la distance
                float adaptiveStep;
                if (distanceToSurface < 1.0f)
                {
                    adaptiveStep = 0.005f; // Petit step près de la surface
                }
                else if (distanceToSurface < 5.0f)
                {
                    adaptiveStep = 0.02f; // Step moyen
                }
                else
                {
                    adaptiveStep = 0.05f; // Grand step loin des surfaces
                }

                curPos += reflection * adaptiveStep;
                prevPosZ = curPosZ;
            }

            // ⭐ BLENDING - Mélanger couleur originale et réflexion selon Fresnel
            vec3 baseColor = GetPixelAlbedo(x, y); // Couleur originale du pixel

            vec3 finalColor;
            if (hit)
            {
                // Blender avec Fresnel : plus l'angle est rasant, plus la réflexion domine
                finalColor = baseColor * (1.0f - fresnel) + hitColor * fresnel;
            }
            else
            {
                // Pas de hit : garder la couleur de base
                finalColor = baseColor;
            }

            _imagePPM[(y * _width + x) * 3] = finalColor.x;
            _imagePPM[(y * _width + x) * 3 + 1] = finalColor.y;
            _imagePPM[(y * _width + x) * 3 + 2] = finalColor.z;
        }
    }

    std::cout << "SSR completed." << std::endl;
}

vec3 Device::GetPixelAlbedo(int x, int y) const
{
    if (x >= _width || x < 0 || y >= _height || y < 0)
        throw std::out_of_range("Pixel coordinates out of range A");
    vec3 al{0.0f};
    al.x = _imagePPM[(y * _width + x) * 3];
    al.y = _imagePPM[(y * _width + x) * 3 + 1];
    al.z = _imagePPM[(y * _width + x) * 3 + 2];
    return al;
}

float Device::GetPixelDepth(int x, int y) const
{
    if (x >= _width || x < 0 || y >= _height || y < 0)
    {
        throw std::out_of_range("Pixel coordinates out of range Depth");
    }
    return _depthbuffer[y * _width + x].load();
}

vec3 Device::GetPixelNormal(int x, int y) const
{
    if (x >= _width || x < 0 || y >= _height || y < 0)
        throw std::out_of_range("Pixel coordinates out of range N");
    vec3 n{0.0f};
    n.x = ((_imageNormal[(y * _width + x) * 3] / 255.0f) * 2.0f) - 1.0f;
    n.y = ((_imageNormal[(y * _width + x) * 3 + 1] / 255.0f) * 2.0f) - 1.0f;
    n.z = ((_imageNormal[(y * _width + x) * 3 + 2] / 255.0f) * 2.0f) - 1.0f;
    return n;
}