#include "AmbientOcclusion.h"
#include <map>
#include <cmath>
using namespace std;
using namespace Render3D;

AmbientOcclusion::AmbientOcclusion() {}

void AmbientOcclusion::computeArea(vector<vec3>& vertices, vector<Face>& faces)
{
	map<int, float> occlusions;

	for (Face f : faces) {
		float normAB = (float)sqrt(	pow(vertices[f.B.IndiceVertices-1].x - vertices[f.A.IndiceVertices-1].x, 2.0) +
								pow(vertices[f.B.IndiceVertices-1].y - vertices[f.A.IndiceVertices-1].y, 2.0) + 
								pow(vertices[f.B.IndiceVertices-1].z - vertices[f.A.IndiceVertices-1].z, 2.0));

		float normBC =(float) sqrt(	pow(vertices[f.C.IndiceVertices-1].x - vertices[f.B.IndiceVertices-1].x, 2.0) +
								pow(vertices[f.C.IndiceVertices-1].y - vertices[f.B.IndiceVertices-1].y, 2.0) +
								pow(vertices[f.C.IndiceVertices-1].z - vertices[f.B.IndiceVertices-1].z, 2.0));

		float normCA = (float)sqrt(pow(vertices[f.A.IndiceVertices-1].x - vertices[f.C.IndiceVertices-1].x, 2.0) +
								pow(vertices[f.A.IndiceVertices-1].y - vertices[f.C.IndiceVertices-1].y, 2.0) +
								pow(vertices[f.A.IndiceVertices-1].z - vertices[f.C.IndiceVertices-1].z, 2.0));

		float demiSphere = (normAB + normBC + normCA) / 2.0f;

		float Area = sqrt(demiSphere * (demiSphere - normAB) * (demiSphere - normBC) * (demiSphere - normCA));

		f.A.area += Area / 3.0f;
		f.B.area += Area / 3.0f;
		f.C.area += Area / 3.0f;
	}
}

float AmbientOcclusion::formFactor(Vertices emetrice, Vertices receptrice) {

	vec3 RecpToEmet = emetrice.Position - receptrice.Position;
	double distance = sqrt(pow(RecpToEmet.x,2.0f) + pow(RecpToEmet.y,2.0f) + pow(RecpToEmet.z,2.0f));
	RecpToEmet =  normalize(RecpToEmet);
	if (distance > 100.0f * emetrice.area)
	{
		return 0.0f;
	}

	if (distance == 0.0f)
	{
		return 0.0f;
	}
		
	float cosR = dot(RecpToEmet, normalize(receptrice.Normal));
	float cosE = dot(-RecpToEmet, normalize(emetrice.Normal));

	if (cosR < 0.0f || cosE < 0.0f)
	{
		return 0.0f;
	}

 	return (float)((cosE * cosR * emetrice.area) / ((3.14159f * distance * distance) + emetrice.area));

}

vector<vec3>  AmbientOcclusion::computeOcclusion(vector<vec3> vertices, vector<Face>& faces) {

	computeArea(vertices, faces);
	int cursor = 0;
	
	for (int i = 0; i < vertices.size(); i++)
	{
		for (int j = 0; j < vertices.size(); j++)
		{
			if (i == j)
			{
				continue;
			}
//			vertices[j].occlusion += formFactor(vertices[i], vertices[j])* vertices[i].prevOcclusion;
		}
	}

	for (int i = 0; i < vertices.size(); i++)
	{
//		vertices[i].occlusion = clamp(vertices[i].occlusion);
///		vertices[i].prevOcclusion = vertices[i].occlusion;
	}

	return vertices;
}

float AmbientOcclusion::clamp(float occlusion) {
	float occClamped = exp(-occlusion);
	if (occClamped < 0.0)
	{
		occClamped = 0.0;
	}
	else if (occClamped > 1.0)
	{
		occClamped = 1.0;
	}

	return occClamped;
}