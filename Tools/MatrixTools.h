#include <glm.hpp>

using namespace glm;

void MatrixInitZero(mat4x4& m); 

void IdentityMatrix(mat4x4& m);

void TransformVectorByMatrix3x3(vec3 v, mat3x3 m, vec3& out);

float TransformVectorByMatrix4x4(vec3& v, const mat4x4& m, vec3& out);

void TransformVectorByMatrix4x4_without_w(const vec3& v, const mat4x4& m, vec3& out);

void transpose(mat4x4 m, mat4x4 t);

void printMat4x4(mat4x4 m);

void BuildViewMatrix(vec3 eye, vec3 to, vec3 unityVector, mat4x4& out);

void BuildPerspectiveMatrix(float fovInDegrees, float aspectRatio, float near, float far, mat4x4& mFov);

void Ortho(float left, float right, float bottom, float top, float near, float far, mat4x4& mOrtho);

void BuildTranslationMatrix(vec3 v,mat4x4& out);

void Rotation_X_Pitch(mat4x4& m, float angle);

void Rotation_Y_Yaw(mat4x4& m, float angle);

void Rotation_Z_Roll(mat4x4& m, float angle);

void Rotation_XYZ_PitchYawRoll(mat4x4& mP, mat4x4& mY, mat4x4& mR , mat4x4& m);

void Scale(float multiplicateur, mat4x4& mat4x4);
