//
//  Device.hpp
//  Rasterization
//
//  Created by Alexandre STEIN on 17/07/2020.
//  Copyright © 2020 Alexandre STEIN. All rights reserved.
//

#ifndef Device_hpp
#define Device_hpp

#include <stdio.h>
#include "../Tools/MatrixTools.h"
#include "Camera.hpp"
#include "Light.hpp"
#include "../LoadingFiles/Mesh.hpp"
#include <algorithm>
#include <vector>
#include <mutex>
#include "../LoadingFiles/Texture.h"
#include "../LoadingFiles/TextureNormalMap.h"
#include "../LoadingFiles/TextureParallaxMapping.h"

using namespace std;
using namespace glm;

namespace Render3D
{
    struct Plane {
        vec3 normal;
        float distance;
        
        // Calcule la distance signée d'un point au plan
        float distanceToPoint(const vec3& point) const {
            return normal.x * point.x + normal.y * point.y + normal.z * point.z + distance;
        }
    };

    struct Frustum {
        Plane planes[6]; // Left, Right, Bottom, Top, Near, Far
        
        // Extrait les 6 plans du frustum depuis la matrice proj*view
        void extractFromMatrix(const mat4x4& projView) {
            // Left plane
            planes[0].normal.x = projView[0][3] + projView[0][0];
            planes[0].normal.y = projView[1][3] + projView[1][0];
            planes[0].normal.z = projView[2][3] + projView[2][0];
            planes[0].distance = projView[3][3] + projView[3][0];
            normalizePlane(planes[0]);
            
            // Right plane
            planes[1].normal.x = projView[0][3] - projView[0][0];
            planes[1].normal.y = projView[1][3] - projView[1][0];
            planes[1].normal.z = projView[2][3] - projView[2][0];
            planes[1].distance = projView[3][3] - projView[3][0];
            normalizePlane(planes[1]);
            
            // Bottom plane
            planes[2].normal.x = projView[0][3] + projView[0][1];
            planes[2].normal.y = projView[1][3] + projView[1][1];
            planes[2].normal.z = projView[2][3] + projView[2][1];
            planes[2].distance = projView[3][3] + projView[3][1];
            normalizePlane(planes[2]);
            
            // Top plane
            planes[3].normal.x = projView[0][3] - projView[0][1];
            planes[3].normal.y = projView[1][3] - projView[1][1];
            planes[3].normal.z = projView[2][3] - projView[2][1];
            planes[3].distance = projView[3][3] - projView[3][1];
            normalizePlane(planes[3]);
            
            // Near plane
            planes[4].normal.x = projView[0][3] + projView[0][2];
            planes[4].normal.y = projView[1][3] + projView[1][2];
            planes[4].normal.z = projView[2][3] + projView[2][2];
            planes[4].distance = projView[3][3] + projView[3][2];
            normalizePlane(planes[4]);
            
            // Far plane
            planes[5].normal.x = projView[0][3] - projView[0][2];
            planes[5].normal.y = projView[1][3] - projView[1][2];
            planes[5].normal.z = projView[2][3] - projView[2][2];
            planes[5].distance = projView[3][3] - projView[3][2];
            normalizePlane(planes[5]);
        }
        
        // Teste si un triangle est complètement hors du frustum
        bool isTriangleOutside(const vec3& a, const vec3& b, const vec3& c) const {
            // Pour chaque plan, si les 3 sommets sont du mauvais côté, le triangle est rejeté
            for (int i = 0; i < 6; i++) {
                if (planes[i].distanceToPoint(a) < -0.1f &&
                    planes[i].distanceToPoint(b) < -0.1f &&
                    planes[i].distanceToPoint(c) < -0.1f) {
                    return true; // Triangle complètement hors du frustum
                }
            }
            return false; // Au moins partiellement visible
        }
        
    private:
        void normalizePlane(Plane& plane) {
            float length = sqrt(plane.normal.x * plane.normal.x + 
                            plane.normal.y * plane.normal.y + 
                            plane.normal.z * plane.normal.z);
            if (length > 0.0f) {
                plane.normal.x /= length;
                plane.normal.y /= length;
                plane.normal.z /= length;
                plane.distance /= length;
            }
        }
    };

    class Device
    {
        private:
            unsigned char* _imagePPM;
            unsigned char* _imageZbuffer;
            unsigned char* _imageNormal;
            int _width;
            int _height;
            atomic<float>* _depthbuffer;
            std::mutex _mutex;
            vec3* _normalBuffer; 

        public:
            Device(int Width, int Height);
            ~Device();

            //Display
            void SetPixelColor(int x, int y, float r, float g, float b);
            void RasterizeTriangle(const mat4x4& proj, const mat4x4& world, const mat3x3& normalMatrix, Face f, Mesh mesh, Lights l, std::shared_ptr<Camera> camera, vector<dvec3> projected_coordinates, vector<vec3> world_coordinates, vector<float> Z_correction, Textures& tex, TextureNormalMap& nTex, TextureParallaxMapping& pTex);

            //Matrix
            float Projection_3D_to_2D(vec3& coordinate, const mat4x4& projection, vec3& out);

            //Picture
            void RenderScene(std::shared_ptr<Camera> camera, Mesh meshes, Lights& l);
            void ApplyScreenSpaceReflections(const std::shared_ptr<Camera>& camera,  const mat4x4& view , const mat4x4& proj);

            //getter
            int GetWidth();
            int GetHeight();

            //buffers
            vec3 GetPixelAlbedo(int x, int y) const;
            float GetPixelDepth(int x, int y) const;
            vec3 GetPixelNormal(int x, int y) const;
    };
};
#endif /* Device_hpp */
