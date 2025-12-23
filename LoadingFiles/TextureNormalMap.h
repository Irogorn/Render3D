#pragma once
#include <string>
#include "../Tools/jpeg.hpp"
#include "../LoadingFiles/Mesh.hpp"
#include <atomic>
#include <mutex>

using namespace std;
namespace Render3D
{
	class TextureNormalMap {
	private:
		int _width;
		int _height;
		unsigned char* _image;
		std::atomic<bool> _isLoaded;
		vec2 UV1;
		vec2 UV2;
		vec3 edge1;
		vec3 edge2;
		std::mutex _loadMutex;

	protected:
		vec3 _normal;
		vec3 normal;
		string _pathTextureBump;
		mat3x3 _TBN;

	public:
		TextureNormalMap();
		TextureNormalMap(string pathTexture);
		~TextureNormalMap();
		void loadTexture();
		void setPixel(float u, float v);
		vec3 GetPixelNormal(Face& f, vec3 a, vec3 b, vec3 c,  vector<vec2>& uvs, vec3 weight, mat3x3 w, bool parallax = false);
		mat3x3 getTBN(Face& f, vec3 a, vec3 b, vec3 c,  vector<vec2>& uvs, vec3 weight, mat3x3 w);
		bool getLoaded();
		mat3x3 preCompute(Face& f, vec3 a, vec3 b, vec3 c,  vector<vec2>& uvs, vec3 normal, mat3x3 w);
		void computeTangent(Face& f, float coef);
		void computeBiTangent(Face& f, float coef);
		vec3 getNormal() const;

	};
}