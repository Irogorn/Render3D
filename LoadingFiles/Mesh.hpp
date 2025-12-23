//
//  Mesh.hpp
//  Rasterization
//
//  Created by Alexandre STEIN on 17/07/2020.
//  Copyright © 2020 Alexandre STEIN. All rights reserved.
//

#ifndef Mesh_hpp
#define Mesh_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include "../Tools/MatrixTools.h"
#include "../OutPut/ConstantLight.hpp"
#include <iostream>

using namespace std;
using namespace glm;

namespace Render3D
{

    /**
     * @struct Vertices
     * @brief Structure complète d'un vertex avec toutes ses propriétés géométriques
     *
     * Cette structure contient toutes les informations nécessaires pour le rendu
     * d'un vertex, incluant sa position, ses normales, et les données pour l'occlusion.
     */
    struct Vertices {
        vec3 Position;
        vec3 Normal;
        vec3 Tangent;
        vec3 Bitangent;
        vec2 TexCoords;
        float area = 0.0f;
        float occlusion = 0.0f;
        float prevOcclusion = 0.0f;
    };

    /**
     * @struct Info
     * @brief Informations d'un sommet de face avec indices et propriétés géométriques
     *
     * Cette structure stocke les indices pointant vers les données globales du mesh
     * ainsi que les propriétés calculées pour ce sommet spécifique.
     */
    struct Info
    {
        int IndiceVertices;
        int IndiceTexCoords;
        int IndiceNormals;
        vec3 Tangent;
        vec3 Bitangent;
        float area = 0.0f;
        float occlusion = 0.0f;
        float prevOcclusion = 0.0f;

    };

    /**
     * @struct Face
     * @brief Représentation d'une face triangulaire
     *
     * Une face est composée de trois sommets (A, B, C) dans l'ordre anti-horaire
     * pour définir la normale frontale (règle de la main droite).
     */
    struct Face
    {
        Info A;
        Info B;
        Info C;
    };

    /**
     * @struct MaterialProperty
     * @brief Propriétés d'un matériau pour l'éclairage
     *
     * Cette structure regroupe les propriétés d'éclairage d'un matériau.
     * Elle est utilisée pour stocker les coefficients de réflexion
     * ambiante, diffuse, spéculaire, et émissive ainsi que le facteur
     * de brillance.
     * 
     */
    struct MaterialProperty{
        string useMtl;
        string pathTexture;
        string pathTextureBump;
        string pathTextureDisp;
        float ns;
        vec3 ka;
        vec3 kd;
        vec3 ks;
        vec3 ke;
    };

    /**
     * @struct MeshData
     * @brief Données complètes d'un sous-mesh incluant géométrie et matériau
     *
     * Cette structure regroupe toutes les informations d'un objet 3D :
     * - Identification (noms)
     * - Géométrie (faces)
     * - Matériau (propriétés d'éclairage et textures)
     * - Transformation (position, rotation)
     */
    struct MeshData{
        string nameMesh;
        vector<Face> faces;
        vector<MaterialProperty> material;
        vec3 position;
        vec3 rotation;
    };

    /**
     * @class Mesh
     * @brief Classe principale représentant un modèle 3D complet
     *
     * Cette classe encapsule toutes les données géométriques d'un modèle 3D :
     * - Vertices (positions)
     * - Normales
     * - Coordonnées de texture (UV)
     * - Faces triangulaires
     * - Sous-meshes avec leurs matériaux
     * - Chemins des textures
     *
     * Un Mesh peut contenir plusieurs sous-meshes (MeshData), chacun avec
     * son propre matériau et ses propres faces.
     */
    class Mesh
    {
        private:
            string _name;
            vector<int> _indiceVertices;
            vector<float> __normals;
            vector<Face> _faces;
            int _count;
            string _pathTexture;
            string _pathTextureBump;
            string _pathTextureDisp;
            vector<vec3> _vertices;
            vector<vec2> _uv;
            vector<vec3> _normal;
            vector<MeshData> _meshData;
        
        public:

            string get_name();
            void set_name(string _name);

            /**
             * @brief Définit la liste des positions de vertices
             * @param vertices Vecteur de positions 3D
             */
            void set_vertices(vector<vec3> vertices);

            vector<vec3> get_vertices();
            vec3 get_position(int i);
            void set_position(int i, float x, float y, float z);
            vec3 get_rotation( int i );
            void set_rotation(int i, float x, float y, float z);
            int get_count();
            void set_faces(vector<Face> faces);
            vector<Face> get_faces();
            Mesh();
            Mesh(string name, int verticesCount, int facesCount);
            ~Mesh();

            // ===== Getters/Setters pour les textures =====
        
            /**
             * @brief Définit le chemin de la texture diffuse
             * @param pathtexture Chemin relatif ou absolu vers le fichier texture
             */
            void set_pathtexture(string pathtexture);

            /**
             * @brief Récupère le chemin de la texture diffuse
             * @return Chemin de la texture
             */
            string get_pathtexture();

            /**
             * @brief Définit le chemin de la texture de bump mapping
             * @param pathtexture Chemin vers la texture de bump
             */
            void set_pathtexture_bump(string pathtexture);

            /**
             * @brief Récupère le chemin de la texture de bump
             * @return Chemin de la texture de bump
             */
            string get_pathtexture_bump();

            /**
             * @brief Définit le chemin de la texture de displacement
             * @param pathtexture Chemin vers la texture de displacement
             */
            void set_pathtexture_disp(string pathtexture);

            /**
             * @brief Récupère le chemin de la texture de displacement
             * @return Chemin de la texture de displacement
             */
            string get_pathtexture_disp();

            // ===== Setters pour les indices et coordonnées =====
        
            /**
             * @brief Définit les indices des vertices
             * @param indiceVertices Référence vers le vecteur d'indices
             * @note Utilise une référence pour éviter la copie
             */
            void set_indice_vertices(vector<int>& indiceVertices);

            /**
             * @brief Définit les coordonnées de texture UV
             * @param v Vecteur de coordonnées UV (vec2)
             */
            void set_uvs(vector<vec2> v);

            /**
             * @brief Définit les normales
             * @param v Vecteur de normales (vec3)
             */
            void set_normals(vector<vec3> v);

            // ===== Getters pour les coordonnées =====
        
            /**
             * @brief Récupère les coordonnées UV
             * @return Vecteur de coordonnées UV
             */
            vector<vec2> get_uvs();

            /**
             * @brief Récupère les normales
             * @return Vecteur de normales
             */
            vector<vec3> get_normals();

            // ===== Getters pour les mesh data =====
        
            /**
             * @brief Récupère la liste des sous-meshes
             * @return Référence vers le vecteur de MeshData
             * @note Retourne une référence pour permettre la modification directe
             */
            vector<MeshData>& get_meshData();

            // ===== Utilitaire pour l'éclairage =====
        
            /**
             * @brief Extrait les propriétés d'éclairage d'un sous-mesh
             * @param i Indice du sous-mesh dans _meshData
             * @param j Indice du matériel dans _meshData
             * @return Structure ConstantLight contenant Ns, Ka, Kd, Ks, Ke
             * 
             * Cette méthode convertit les propriétés de matériau MTL en structure
             * utilisable par le pipeline de rendu pour calculer l'éclairage.
             */
            ConstantLight get_ConstantLight(int i, int j);    
    };
};
#endif /* Mesh_hpp */
