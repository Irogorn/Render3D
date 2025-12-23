#include "../Tools/jpeg.hpp"
#include "TextureParallaxMapping.h"
#include <iostream>
#include <atomic>
#include <mutex>

using namespace Render3D;

TextureParallaxMapping::TextureParallaxMapping() {
	_pathTextureDisp = "";
	_height_scale = 0;
	_width = 0;
	_height = 0;
	_isLoaded.store(false);
	_image = nullptr;
	_pHeigth = {};
}

TextureParallaxMapping::TextureParallaxMapping(string pathTextureDisp, float height_scale) {
	_pathTextureDisp = pathTextureDisp;
	_height_scale = height_scale;
	_width = 0;
	_height = 0;
	_isLoaded.store(false);
	_image = nullptr;
	_pHeigth = {};
	if (pathTextureDisp.empty() == false)
	{
		loadTexture();
	}
}

Render3D::TextureParallaxMapping::~TextureParallaxMapping()
{
	delete[] _image;
}

void TextureParallaxMapping::loadTexture()
{
	_isLoaded.store(readJPEG(_pathTextureDisp.c_str(), _image, _width, _height));
}

void TextureParallaxMapping::setPixel(float u, float v) {
	int x = static_cast<int>(u * (_width-1));
	int y = static_cast<int>(v * (_height-1));
	int index = 3 * (y * _height + x);
	_pHeigth.x = _image[index];
	_pHeigth.y = _image[index+1];
	_pHeigth.z = _image[index+2];
}

bool TextureParallaxMapping::getLoaded() const {
	return _isLoaded.load();
}

vec2 TextureParallaxMapping::getParallaxMapping(vec2 texCoords, vec3 viewDir) const
{
	float height = (_pHeigth.x * 0.30f + _pHeigth.y * 0.59f + _pHeigth.z * 0.11f) / 255.0f;
	vec2 p = {};
	p.x = (viewDir.x / viewDir.z) * (height * _height_scale);
	p.y = (viewDir.y / viewDir.z) * (height * _height_scale);

	return texCoords - p;
}