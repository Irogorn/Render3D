#include "MatrixTools.h"
#include <iostream>

#define PI 3.14159f

void MatrixInitZero(mat4x4& m)
{
    m[0][0] = 0.0f;
    m[0][1] = 0.0f;
    m[0][2] = 0.0f;
    m[0][3] = 0.0f;
    m[1][0] = 0.0f;
    m[1][1] = 0.0f;
    m[1][2] = 0.0f;
    m[1][3] = 0.0f;
    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    m[2][2] = 0.0f;
    m[2][3] = 0.0f;
    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;
    m[3][3] = 0.0f;
}; 

void IdentityMatrix(mat4x4& m)
{
    MatrixInitZero(m);
    m[0][0] = 1.0f;
    m[1][1] = 1.0f;
    m[2][2] = 1.0f;
    m[3][3] = 1.0f;
};

void TransformVectorByMatrix3x3(vec3 v, mat3x3 m, vec3& out)
{
    /*Column-major order*/
    out.x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0];
    out.y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1];
    out.z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2];

};

float TransformVectorByMatrix4x4(vec3& v, const mat4x4& m, vec3& out)
{

    /*Column-major order*/
    out.x = v.x*m[0][0] + v.y*m[1][0] + v.z*m[2][0] + m[3][0];
    out.y = v.x*m[0][1] + v.y*m[1][1] + v.z*m[2][1] + m[3][1];
    out.z = v.x*m[0][2] + v.y*m[1][2] + v.z*m[2][2] + m[3][2];
    float w = v.x*m[0][3] + v.y*m[1][3] + v.z*m[2][3] + m[3][3];

    if(w != 1.0f )
    {
        float invW = 1.0f / w;
        out.x *= invW;
        out.y *= invW;
        out.z *= invW;
    }

    return w;
};

void TransformVectorByMatrix4x4_without_w(const vec3& v, const mat4x4& m, vec3& out)
{
    // En Column-Major (OpenGL/GLM), m[i] est la i-ème COLONNE.
    out.x = m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z;
    out.y = m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z;
    out.z = m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z;
};


void transpose(mat4x4 m, mat4x4 t)
{
    t[0][0] = m[0][0];
    t[0][1] = m[1][0];
    t[0][2] = m[2][0];
    t[0][3] = m[3][0];
    
    t[1][0] = m[0][1];
    t[1][1] = m[1][1];
    t[1][2] = m[2][1];
    t[1][3] = m[3][1];
    
    t[2][0] = m[0][2];
    t[2][1] = m[1][2];
    t[2][2] = m[2][2];
    t[2][3] = m[3][2];
    
    t[3][0] = m[0][3];
    t[3][1] = m[1][3];
    t[3][2] = m[2][3];
    t[3][3] = m[3][3];
};

void printMat4x4(mat4x4 m)
{
    std::cout << m[0][0] << " " << m[1][0] << " " << m[2][0] << " " << m[3][0] << std::endl;
    std::cout << m[0][1] << " " << m[1][1] << " " << m[2][1] << " " << m[3][1] << std::endl;
    std::cout << m[0][2] << " " << m[1][2] << " " << m[2][2] << " " << m[3][2] << std::endl;
    std::cout << m[0][3] << " " << m[1][3] << " " << m[2][3] << " " << m[3][3] << std::endl;
 }

void BuildViewMatrix(vec3 eye, vec3 to, vec3 unityVector, mat4x4& out)
{
    //Foward computation
    vec3 forward = eye - to;
    forward = normalize(forward);

    if (abs(dot(unityVector, forward)) > 0.999f)
    {
        unityVector = vec3(0.0f, 1.0f, -0.1f);
    }

    //Right computation
    vec3 right = cross(unityVector,forward);
    right = normalize(right);

    //Up computation
    vec3 up = cross(forward,right);
    up = normalize(up);

    //Here we fill the projection matrix.
    out[0][0] = right.x;
    out[1][0] = right.y;
    out[2][0] = right.z;
    out[3][0] = -dot(right,eye);
    
    out[0][1] = up.x;
    out[1][1] = up.y;
    out[2][1] = up.z;
    out[3][1] = -dot(up,eye);

    out[0][2] = forward.x;
    out[1][2] = forward.y;
    out[2][2] = forward.z;
    out[3][2] = -dot(forward,eye);

    out[0][3] = 0.0f;
    out[1][3] = 0.0f;
    out[2][3] = 0.0f;
    out[3][3] = 1.0f;
};

void BuildPerspectiveMatrix(float fovInDegrees, float aspectRatio, float near, float far, mat4x4& mFov)
{

    MatrixInitZero(mFov);
    float fovRad = fovInDegrees * (PI / 180.0f);
    float tanHalfFov = std::tan(fovRad / 2.0f);
    mFov[1][1] = 1.0f / tanHalfFov;
    mFov[0][0] = mFov[1][1] / aspectRatio;
    mFov[2][2] = -(far + near) / (far - near);
    mFov[3][2] = -(2.0f * far * near) / (far - near);
    mFov[2][3] = -1.0f;
}

void Ortho(float left, float right, float bottom, float top, float near, float far, mat4x4& mOrtho)
{
    MatrixInitZero(mOrtho);


    mOrtho[0][0] = 2.0f / (right - left);
    mOrtho[1][1] = 2.0f / (top - bottom);
    mOrtho[2][2] = -2.0f / (far - near);
    mOrtho[3][0] = -(right + left) / (right - left);
    mOrtho[3][1] = -(top + bottom) / (top - bottom);
    mOrtho[3][2] = -(far + near) / (far - near);
    mOrtho[3][3] = 1.0f;
}

void BuildTranslationMatrix(vec3 v, mat4x4& m)
{
  MatrixInitZero(m);
  IdentityMatrix(m);
  m[3][0] = v.x;
  m[3][1] = v.y;
  m[3][2] = v.z;
};

void Rotation_X_Pitch(mat4x4& m, const float angle)
{
  MatrixInitZero(m);
  IdentityMatrix(m);
  m[1][1] = cos(angle);
  m[1][2] = sin(angle);
  m[2][1] = -sin(angle);
  m[2][2] = cos(angle);
};

void Rotation_Y_Yaw(mat4x4& m, const float angle)
{
  MatrixInitZero(m);
  IdentityMatrix(m);
  m[0][0] = cos(angle);
  m[0][2] = -sin(angle);
  m[2][0] = sin(angle);
  m[2][2] = cos(angle);
};

void Rotation_Z_Roll(mat4x4& m, const float angle)
{
  MatrixInitZero(m);
  IdentityMatrix(m);
  m[0][0] = cos(angle);
  m[0][1] = sin(angle);
  m[1][0] = -sin(angle);
  m[1][1] = cos(angle);
};

void Rotation_XYZ_PitchYawRoll(mat4x4& mP, mat4x4& mY, mat4x4& mR , mat4x4& m)
{
  m = mR*mY*mP;
};

void Scale(float multiplicateur, mat4x4& m)
{
  MatrixInitZero(m);
  IdentityMatrix(m);
    if(multiplicateur == 0.0f || multiplicateur == 1.0f)
  {
    m[0][0] = 1.0f;
    m[1][1] = 1.0f;
    m[2][2] = 1.0f;
    m[3][3] = 1.0f;
  }
  else
  {
    m[0][0] = multiplicateur;
    m[1][1] = multiplicateur;
    m[2][2] = multiplicateur;
  } 
};
