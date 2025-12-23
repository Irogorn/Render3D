#pragma once
#include <string>
#include <atomic>
#include <mutex>

using namespace std;
namespace Render3D
{
	class Textures {
	private:
		string _pathTexture;
		int _width;
		int _height;
		std::atomic<bool> _isLoaded;
		unsigned char* _image;
		float _u;
		float _v;
		std::mutex _loadMutex;

	public:
		Textures(string pathTexture);
		~Textures();
		void loadTexture();
		void setPixel(float u, float v);
		int getRed();
		int getGreen();
		int getBlue();
		bool getLoaded();

	};
}