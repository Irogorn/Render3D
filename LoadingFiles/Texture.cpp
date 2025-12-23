#include "Texture.h"
#include "../Tools/jpeg.hpp"
#include <iostream>
using namespace Render3D;

Textures::Textures(string pathTexture) {
	_pathTexture = pathTexture;
	_isLoaded.store(false);
	_height = 0;
	_width = 0;
	_image = nullptr;
	_u = 0.0f;
	_v = 0.0f;
	if (pathTexture.empty() == false)
	{
		loadTexture();
	}
}

Textures::~Textures() {
	delete[] _image;
}

void Textures::loadTexture()
{

	bool toto = readJPEG(_pathTexture.c_str(), _image, _width, _height);
	_isLoaded.store(toto);
}

void Textures::setPixel(float u, float v) {
	_u = u;
	_v = v;
}

int Textures::getRed() {
	int x = static_cast<int>(_u * (_width-1));
	int y = static_cast<int>(_v * (_height-1));
	int index = 3 * (y * _height + x);
	return  _image[index];
}

int Textures::getGreen() {
	int x = static_cast<int>(_u * (_width-1));
	int y = static_cast<int>(_v * (_height-1));
	int index = 3 * (y * _height + x);
	return _image[index + 1];
}

int Textures::getBlue() {
	int x = static_cast<int>(_u * (_width-1));
	int y = static_cast<int>(_v * (_height-1));
	int index = 3 * (y * _height + x);
	return _image[index + 2];
}

bool Textures::getLoaded() {
	return _isLoaded.load();
}