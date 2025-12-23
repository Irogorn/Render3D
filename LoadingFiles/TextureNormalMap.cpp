#include "TextureNormalMap.h"
//#include <opencv2/imgcodecs.hpp>
#include <iostream>

using namespace Render3D;

TextureNormalMap::TextureNormalMap()
{
	_image = nullptr;
	_width = 0;
	_height = 0;
	_isLoaded.store(false);
	UV1 = {};
	UV2 = {};
	edge1 = {};
	edge2 = {};
	_TBN = {};
	_normal = {};
	normal = {};
}

TextureNormalMap::TextureNormalMap(string pathTexture) {
	_pathTextureBump = pathTexture;
	_image = nullptr;
	_width = 0;
	_height = 0;
	_isLoaded = false;
	UV1 = {};
	UV2 = {};
	edge1 = {};
	edge2 = {};
	_TBN = {};
	_normal = {};
	normal = {};
	if (pathTexture.empty() == false)
	{
		loadTexture();
	}
}

TextureNormalMap::~TextureNormalMap() {
	delete[] _image;
}

void TextureNormalMap::loadTexture()
{
	_isLoaded.store(readJPEG(_pathTextureBump.c_str(), _image, _width, _height));
}

void TextureNormalMap::setPixel(float u, float v) {
	
	int x = static_cast<int>(u * (_width-1));
	int y = static_cast<int>(v * (_height-1));
	int index = 3 * (y * _height + x);

	_normal.x = _image[index];
	_normal.y = _image[index + 1];
	_normal.z = _image[index + 2];
}

vec3 TextureNormalMap::GetPixelNormal(Face& f, vec3 a, vec3 b, vec3 c, vector<vec2>& uvs, vec3 weight, mat3x3 w, bool parallax) {

	normal.x = ((_normal.x / 255.0f) * 2.0f) -1.0f;
	normal.y = ((_normal.y / 255.0f) * 2.0f) -1.0f;
	normal.z = ((_normal.z / 255.0f) * 2.0f) -1.0f;

	normal = normalize(normal);
	normal = w * normal;
	normal = normalize(normal);

	if(parallax){
		normal = normalize(getTBN(f, a,  b, c, uvs, weight, w) * normal);
	}

	return normal;
}

mat3x3 TextureNormalMap::getTBN(Face& f, vec3 a, vec3 b, vec3 c, vector<vec2>& uvs, vec3 weight, mat3x3 w) {
	return preCompute(f, a,  b, c, uvs, weight, w);
}

bool TextureNormalMap::getLoaded() {
	return _isLoaded.load();
}

mat3x3 TextureNormalMap::preCompute(Face& f, vec3 a, vec3 b, vec3 c, vector<vec2>& uvs, vec3 normal, mat3x3 w) {
	UV1 = uvs[f.B.IndiceTexCoords - 1] - uvs[f.A.IndiceTexCoords - 1];
	UV2 = uvs[f.C.IndiceTexCoords - 1] - uvs[f.A.IndiceTexCoords - 1];

	edge1 = b - a;
	edge2 = c - a;

	float coef = 1.0f / ((UV1.x * UV2.y) - (UV2.x * UV1.y));

	computeTangent(f, coef);
	computeBiTangent(f, coef);

	vec3 T;
	vec3 B;
	vec3 N;

	N = normalize(cross(edge1, edge2));

	//TransformVectorByMatrix4x4(f.A.Tangent, w, T);
	T = w * f.A.Tangent ;
	T = normalize(T);

	//TransformVectorByMatrix4x4(f.A.Bitangent, w, B);
	B = w * f.A.Bitangent;
	B = normalize(B);

	return mat3(T, B, N);
}
void TextureNormalMap::computeTangent(Face& f, float coef) {
	f.A.Tangent.x = coef * ((UV2.y * edge1.x) - (UV1.y * edge2.x));
	f.B.Tangent.x = coef * ((UV2.y * edge1.x) - (UV1.y * edge2.x));
	f.C.Tangent.x = coef * ((UV2.y * edge1.x) - (UV1.y * edge2.x));

	f.A.Tangent.y = coef * ((UV2.y * edge1.y) - (UV1.y * edge2.y));
	f.B.Tangent.y = coef * ((UV2.y * edge1.y) - (UV1.y * edge2.y));
	f.C.Tangent.y = coef * ((UV2.y * edge1.y) - (UV1.y * edge2.y));


	f.A.Tangent.z = coef * ((UV2.y * edge1.z) - (UV1.y * edge2.z));
	f.B.Tangent.z = coef * ((UV2.y * edge1.z) - (UV1.y * edge2.z));
	f.C.Tangent.z = coef * ((UV2.y * edge1.z) - (UV1.y * edge2.z));
}

void TextureNormalMap::computeBiTangent(Face& f, float coef) {
	f.A.Bitangent.x = coef * (-(UV2.x * edge1.x) + (UV1.x * edge2.x));
	f.B.Bitangent.x = coef * (-(UV2.x * edge1.x) + (UV1.x * edge2.x));
	f.C.Bitangent.x = coef * (-(UV2.x * edge1.x) + (UV1.x * edge2.x));

	f.A.Bitangent.y = coef * (-(UV2.x * edge1.y) + (UV1.x * edge2.y));
	f.B.Bitangent.y = coef * (-(UV2.x * edge1.y) + (UV1.x * edge2.y));
	f.C.Bitangent.y = coef * (-(UV2.x * edge1.y) + (UV1.x * edge2.y));

	f.A.Bitangent.z = coef * (-(UV2.x * edge1.z) + (UV1.x * edge2.z));
	f.B.Bitangent.z = coef * (-(UV2.x * edge1.z) + (UV1.x * edge2.z));
	f.C.Bitangent.z = coef * (-(UV2.x * edge1.z) + (UV1.x * edge2.z));
}

vec3 TextureNormalMap::getNormal() const{
	return _normal;
}