//
//  LoadObj.hpp
//  Rasterization
//
//  Created by Alexandre STEIN on 06/08/2020.
//  Copyright © 2020 Alexandre STEIN. All rights reserved.
//

#ifndef LoadObj_hpp
#define LoadObj_hpp
#include <string>
#include <stdio.h>
#include <string>
#include <vector>
#include <fstream>
#include "Mesh.hpp"


namespace Render3D
{

    /**
     * @struct VerticesLoading
     * @brief Structure temporaire pour le chargement des données de vertices
     *
     * Cette structure stocke toutes les informations associées à un vertex
     * lors du processus de chargement d'un fichier OBJ.
     */
    struct VerticesLoading {
        vec3 Position;
        vec3 Normal;
        vec3 Tangent;
        vec3 Bitangent;
        vec2 TexCoords;
        bool visited = false;
    };

    /**
     * @struct TmpMesh
     * @brief Structure temporaire pour stocker les données d'un mesh pendant le chargement
     *
     * Cette structure contient les informations d'un objet 3D avant sa conversion
     * finale en structure Mesh. Elle inclut les propriétés du matériau (MTL).
     */
    struct TmpMesh{
        string nameMesh;
        string nameMtl;
        vector<string> useMtl;
        vector<Face> faces;
        float ns;
        vec3 ka;
        vec3 kd;
        vec3 ks;
        vec3 ke;

        /**
         * @brief Constructeur par défaut initialisant toutes les propriétés à zéro
         */
        TmpMesh() : ns(0.0f), ka({ 0.0f, 0.0f, 0.0f }), kd({ 0.0f, 0.0f, 0.0f }), ks({ 0.0f, 0.0f, 0.0f }), ke({ 0.0f, 0.0f, 0.0f }) {}
    };

    class LoadObj
    {
        string _path;
        string _nameMesh;
        string _nameMtl;
        string _useMtl;
        vector<vec3> _vertices;
        vector<vec2> _uvs;
        vector<vec3> _normals;
        vector<Face> _faces;
        vector<TmpMesh> _tmpMeshes;
        int verticesCount;
        int facesCount;
        float _ns;
        vec3 _ka;
        vec3 _kd;
        vec3 _ks;
        vec3 _ke;

        vector<VerticesLoading> vertices;

        vector<vec2> uv;

        vector<vec3> normal;

        
        
        public :
        LoadObj(string path);

        /**
         * @brief Remplit un objet Mesh avec les données chargées
         * @param meshes Référence vers l'objet Mesh à remplir
         *
         * Convertit les structures temporaires (TmpMesh) en structures MeshData
         * et charge les propriétés des matériaux depuis le fichier MTL.
         * Remplit également les vertices, normales, UV et faces du mesh.
         *
         * @throw out_of_range si l'accès aux données dépasse les limites des vecteurs
         */
        void get_Mesh(Mesh& meshes);

        /**
         * @brief Charge les propriétés d'un matériau depuis un fichier MTL
         * @param useMtl Nom du matériau à charger (doit correspondre à 'newmtl' dans le MTL)
         * @param meshData Référence vers la structure MeshData à remplir
         *
         * Parse le fichier MTL et extrait les propriétés suivantes :
         * - Ns : exposant spéculaire (shininess)
         * - Ka : couleur ambiante (RGB)
         * - Kd : couleur diffuse (RGB)
         * - Ks : couleur spéculaire (RGB)
         * - Ke : couleur émissive (RGB)
         * - map_Kd : chemin de la texture diffuse
         * - map_Bump : chemin de la texture de bump mapping
         * - disp : chemin de la texture de displacement
         *
         * @throw exception si le fichier MTL ne peut pas être ouvert
         */
        void loadMtl(const string useMtl, MeshData& meshData);

        /**
         * @brief Définit les coordonnées de texture UV
         * @param v Vecteur contenant les coordonnées UV (vec2) à stocker
         */
        void setUvs(vector<vec2> v);

        /**
         * @brief Définit les vecteurs normaux
         * @param v Vecteur contenant les normales (vec3) à stocker
         */
        void setNormals(vector<vec3> v);
    };
};
#endif /* LoadObj_hpp */
