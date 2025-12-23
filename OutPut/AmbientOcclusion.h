#pragma once
#include <vector>
#include "../LoadingFiles/Mesh.hpp"
#include "../LoadingFiles/LoadObj.hpp"
using namespace std;

namespace Render3D
{
	class AmbientOcclusion {
	public:
		AmbientOcclusion();
		vector<vec3> computeOcclusion(vector<vec3> vertices, vector<Face>& faces);

	private:
		void computeArea(vector<vec3>& vertices, vector<Face>& faces);
		float formFactor(Vertices emetrice, Vertices receptrice);
		float clamp(float occlusion);
	};
}
