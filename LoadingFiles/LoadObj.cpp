/**
 * @file LoadObj.cpp
 * @brief Implémentation du chargement de fichiers OBJ et MTL pour la rasterisation 3D.
 *
 * Ce fichier contient la définition de la classe LoadObj qui permet de charger des objets 3D
 * au format Wavefront OBJ ainsi que leurs matériaux associés (MTL).
 *
 * @author Alexandre STEIN
 * @date 06/08/2020
 */


#include "LoadObj.hpp"
#include "string.h"
#include <stdlib.h>
#include <iostream>
#include <sstream>
using namespace std;
using namespace Render3D;

/**
 * @brief Constructeur de la classe LoadObj
 * @param path Chemin du fichier OBJ à charger
 */
LoadObj::LoadObj(string path)
{
    _path = path;
    string line;
    _tmpMeshes.push_back(TmpMesh());
    verticesCount = 0;
    facesCount = 0;
    _ns = 0.0f;
    _ka = {};
    _kd = {};
    _ks = {};
    _ke = {};
    int nbObjects = 0, iTmp = 0;

    try {
        ifstream file(strdup(path.c_str()));
        string prefix, tmp;
        while (getline(file, line))
        {
            istringstream iss(line);
            iss >> prefix;
            if (prefix == "mtllib")
            {
                iss >> _nameMtl;
                _tmpMeshes[iTmp].nameMtl = _nameMtl;

            }
            else if (prefix == "usemtl")
            {
                iss >> _useMtl;

            }
            else if (prefix == "o")
            {
                _useMtl = "";
                iss >> _nameMesh;
                if(nbObjects > 0)
                {
                    iTmp++;
                    _tmpMeshes.push_back(TmpMesh());
                }
                
                _tmpMeshes[iTmp].nameMesh = _nameMesh;
                
                nbObjects++;
            }
            else if (prefix == "v")
            {
                vec3 v{};
                iss >> v.x;
                iss >> v.y;
                iss >> v.z;
                _vertices.push_back(v);
                verticesCount++;
            }
            else if (prefix == "vt")
            {
                vec2 v{};
                iss >> v.x;
                iss >> v.y;
                uv.push_back(v);

            }
            else if (prefix == "vn")
            {
                vec3 v{};
                iss >> v.x;
                iss >> v.y;
                iss >> v.z;
                normal.push_back(v);
            }
            else if (prefix == "f")
            {
                int numberOfCoordinates = 0;
                vector<int> v;
                Face face{};
                string faceA;
                string faceB;
                string faceC;
                string noMore;
                string extracted;
                iss >> faceA >> faceB >> faceC >> noMore;

                if (noMore.empty() == false) {
                    throw std::runtime_error("Only files containing triangular faces are accepted!");
                }
        
                istringstream ss(faceA);
                while (getline(ss, extracted, '/')) {
                    v.push_back(stoi(extracted));
                    numberOfCoordinates++;
                }

                face.A.IndiceVertices = v[0];
                face.A.IndiceTexCoords = v[1];
                face.A.IndiceNormals = v[2];

                istringstream ss1(faceB);
                v.clear();
                while (getline(ss1, extracted, '/')) {
                    v.push_back(stoi(extracted));
                }

                face.B.IndiceVertices = v[0];
                face.B.IndiceTexCoords = v[1];
                face.B.IndiceNormals = v[2];

                istringstream ss2(faceC);
                v.clear();
                while (getline(ss2, extracted, '/')) {
                    v.push_back(stoi(extracted));
                }

                face.C.IndiceVertices = v[0];
                face.C.IndiceTexCoords = v[1];
                face.C.IndiceNormals = v[2];

                _faces.push_back(face);
                _tmpMeshes[iTmp].faces.push_back(face);
                if(_useMtl.empty() == false)
                    _tmpMeshes[iTmp].useMtl.push_back(_useMtl);

                facesCount++;
            }
        }

        setUvs(uv);
        setNormals(normal);

    }
    catch (exception& e)
    {
        cerr << "Loading file OBJ falure; " << e.what() << endl;
        std::exit(EXIT_FAILURE);
    }

}

/**
 * @brief Remplit un objet Mesh à partir des données chargées
 * @param meshes Référence vers l'objet Mesh à remplir
 */
void LoadObj::get_Mesh(Mesh& meshes)
{
    try {
        for(auto m :_tmpMeshes){
            MeshData md;
            md.nameMesh = m.nameMesh;
            md.faces = m.faces;
  
            for(auto prop : m.useMtl){
                loadMtl(prop, md);
            }
           
            meshes.get_meshData().push_back(md);
        }

        meshes.set_faces(_faces);

        meshes.set_normals(normal);

        meshes.set_uvs(uv);

        meshes.set_vertices(_vertices);
    }
    catch (const out_of_range& oor)
    {
        cerr << "Out of range exception:" << oor.what() << endl;
    }


}

/**
 * @brief Charge les propriétés du matériau à partir d'un fichier MTL
 * @param useMtl Nom du matériau à utiliser
 * @param meshData Structure MeshData à remplir avec les propriétés du matériau
 */
void LoadObj::loadMtl(const string useMtl, MeshData& meshData) {
    try {
        MaterialProperty matprop;
        matprop.useMtl = useMtl;
        string pathTexture, pathTextureBump, pathTextureDisp;
        if (_nameMtl.empty() == false) {
            ifstream file(strdup(("./" + _nameMtl).c_str()));
            string line;
            string prefix, tmp;
            string mtl = "";

            while (getline(file, line))
            {
                istringstream iss(line);
                iss >> prefix;
                if(prefix == "newmtl")
                {
                   iss >> mtl;
                }
                if (prefix == "Ns")
                {
                    iss >> _ns;
                    if(mtl == useMtl){
                        matprop.ns = _ns;
                    }
                    
                }
                if (prefix == "Ka")
                {
                    iss >> _ka.x;
                    iss >> _ka.y;
                    iss >> _ka.z;

                    if(mtl == useMtl){
                        matprop.ka = _ka;
                    }
                }
                if (prefix == "Kd")
                {
                    iss >> _kd.x;
                    iss >> _kd.y;
                    iss >> _kd.z;

                    if(mtl == useMtl){
                        matprop.kd = _kd;
                    }
                }
                if (prefix == "Ks")
                {
                    iss >> tmp;
                    _ks.x = stof(tmp);
                    iss >> tmp;
                    _ks.y = stof(tmp);
                    iss >> tmp;
                    _ks.z = stof(tmp);

                    if(mtl == useMtl){
                        matprop.ks = _ks;
                    }
                }
                if (prefix == "Ke")
                {
                    iss >> tmp;
                    _ke.x = stof(tmp);
                    iss >> tmp;
                    _ke.y = stof(tmp);
                    iss >> tmp;
                    _ke.z = stof(tmp);

                    if(mtl == useMtl){
                        matprop.ke = _ke;
                    }
                }
                /*if (prefix == "Ni"){}
                if (prefix == "d"){}
                if (prefix == "illum"){}*/
                if (prefix == "map_Kd")
                {
                    iss >> pathTexture;
                    if(mtl == useMtl){
                        matprop.pathTexture = pathTexture;
                    }
                }
                if (prefix == "map_Bump")
                {
                    iss >> pathTextureBump;
                    if(mtl == useMtl){
                        matprop.pathTextureBump = pathTextureBump;
                    }
                }
                if (prefix == "disp")
                {
                    iss >> pathTextureDisp;
                    if(mtl == useMtl){
                        matprop.pathTextureDisp = pathTextureDisp;
                    }
                }
            }
            meshData.material.push_back(matprop);
        }
    }
    catch (exception& e)
    {
        cerr << "Loading file MTL falure: " << e.what() << endl;
    }


}

/**
 * @brief Définit les coordonnées de texture UV
 * @param v Vecteur de coordonnées UV
 */
void LoadObj::setUvs(vector<vec2> v) {
    _uvs = v;
}

/**
 * @brief Définit les normales
 * @param v Vecteur de normales
 */
void LoadObj::setNormals(vector<vec3> v) {
    _normals = v;
}
