#pragma once
#include "TextureNormalMap.h"

using namespace std;
namespace Render3D
{
	class TextureParallaxMapping{
	private:
		float _height_scale;
		string _pathTextureDisp;
		int _width;
		int _height;
		std::atomic<bool> _isLoaded;
		unsigned char* _image;
		vec3 _pHeigth;
		std::mutex _loadMutex;

	public:
		TextureParallaxMapping();
		TextureParallaxMapping(const TextureParallaxMapping& other) = delete;
		TextureParallaxMapping(string pathTextureDisp, float height_scale);
		~TextureParallaxMapping();
		void loadTexture();
		void setPixel(float u, float v);
		bool getLoaded() const;
		vec2 getParallaxMapping(vec2 texCoords, vec3 viewDir) const;
	};
}
