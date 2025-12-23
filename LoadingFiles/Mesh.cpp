//
//  Mesh.cpp
//  Rasterization
//
//  Created by Alexandre STEIN on 17/07/2020.
//  Copyright © 2020 Alexandre STEIN. All rights reserved.
//

#include "Mesh.hpp"
using namespace Render3D;

string Mesh::get_name()
{
    return _name;
};

void Mesh::set_name(string name)
{
    _name = name;
};

vector<vec3> Mesh::get_vertices() {
    return _vertices;
}

void Mesh::set_vertices(vector<vec3> v)
{
    _vertices = v;
}

vec3 Mesh::get_position(int i)
{
    return _meshData[i].position;
};

void Mesh::set_position(int i,float x, float y, float z)
{
   _meshData[i].position.x = x;
   _meshData[i].position.y = y;
   _meshData[i].position.z = z;
};

vec3 Mesh::get_rotation(int i)
{
    return _meshData[i].rotation;
};

void Mesh::set_rotation(int i, float x, float y, float z)
{
    _meshData[i].rotation.x = x;
    _meshData[i].rotation.y = y;
    _meshData[i].rotation.z = z;
};

int Mesh::get_count()
{
    return _count;
};

Mesh::Mesh()
{
    _name = "";
    _count = 0;
};

Mesh::Mesh(string name, int verticesCount, int facesCount)
{
    _name = name;
    _count = verticesCount;
};

Mesh::~Mesh()
{

};

void Mesh::set_faces(vector<Face> faces)
{
    _faces = faces;
}

vector<Face> Mesh::get_faces()
{
    return _faces;
};

void Mesh::set_pathtexture(string pathTexture)
{
    _pathTexture = pathTexture;
}

string Mesh::get_pathtexture()
{
    return _pathTexture;
}

void Mesh::set_pathtexture_bump(string pathTexture)
{
    _pathTextureBump = pathTexture;
}

string Mesh::get_pathtexture_bump()
{
    return _pathTextureBump;
}

void Render3D::Mesh::set_pathtexture_disp(string pathtexture)
{
    _pathTextureDisp = pathtexture;
}

string Render3D::Mesh::get_pathtexture_disp()
{
    return _pathTextureDisp;
}

void Mesh::set_indice_vertices(vector<int>& indiceVertices) {
    _indiceVertices = indiceVertices;
}

void Mesh::set_uvs(vector<vec2> v) {
    _uv = v;
}
void Mesh::set_normals(vector<vec3> v) {
    _normal = v;
}

vector<vec2> Mesh::get_uvs() {
    return _uv;
}

vector<vec3> Mesh::get_normals() {
    return _normal;
}

vector<MeshData>& Mesh::get_meshData() {
    return _meshData;
}

ConstantLight Mesh::get_ConstantLight(int i, int j)
{
    ConstantLight constantLight;
    
    if(get_meshData()[i].material.size() > 0){
        constantLight.Ns = get_meshData()[i].material[j].ns;
        constantLight.Ka = get_meshData()[i].material[j].ka;
        constantLight.Kd = get_meshData()[i].material[j].kd;
        constantLight.Ks = get_meshData()[i].material[j].ks;
        constantLight.Ke = get_meshData()[i].material[j].ke;
        constantLight.pathTexture = get_meshData()[i].material[j].pathTexture;
        constantLight.pathTextureBump = get_meshData()[i].material[j].pathTextureBump;
        constantLight.pathTextureDisp = get_meshData()[i].material[j].pathTextureDisp;
    }
    
    return constantLight;
}