//
//  Light.hpp
//  Rasterization
//
//  Created by Alexandre STEIN on 30/07/2020.
//  Copyright © 2020 Alexandre STEIN. All rights reserved.
//

#ifndef Light_hpp
#define Light_hpp

#include <stdio.h>
#include <glm.hpp>
#include <vector>
#include<map>
#include "../LoadingFiles/Mesh.hpp"
#include <iostream>
using namespace glm;
using namespace std;

namespace Render3D
{
    enum class LightType {NoLight,DirectionLight, PointLight, SpotLight};

    struct Light
    {
        vec3 _positions = { 0.0f,0.0f,0.0f };
        vec3 _direction = { 0.0f,0.0f,0.0f };
        vec3 _intensity = { 0.0f,0.0f,0.0f };
        vec3 _color = { 1.0f,1.0f,1.0f };
        float spec = 0.0f;
        float dot = 0.0f;
        float attenuation = 1.0f;
        float spot = 0.0f;
        LightType _typeOfLight;

        Light() : _typeOfLight(LightType::NoLight) {}

        Light(LightType lt)
        {
            _typeOfLight = lt;
        };
    };

    class Lights
    {
        map<string, Light> _mLights;
        vector<Light> _lights;
        ConstantLight _constantLight;
        vec3 point3D_position{ 0.0f };
        vec3 N{ 0.0f };

    public:
        Lights();
        Lights(const Lights& other);
        void ComputeLightFlat(vec3 a, vec3 b, vec3 c, vec3 normal_a, vec3 normal_b, vec3 normal_c/*, mat4x4 w*/);
        void ComputeLightGouraud(vec3 weight, vec3 a, vec3 b, vec3 c, vec3 normal_a, vec3 normal_b, vec3 normal_c);
        void ComputeLightPhong(mat3x3 TBN = {}, bool parallax = false);
        void ComputeSpecular(vec3 camera_position, float s, mat3x3 TBN={}, bool parallax=false);
        void ComputeAttenuation(string name, float C1 = 0.0f, float C2 = 0.0f, float radius = 0.0f);
        void ComputeSpotLight(string name, float cutoff, float outerCutoff);
        vec3 getIntensity(vec3 weight, Face f);
        vec3 getPosition(string name);
        vec3 getConstantKd();
        string getPathTexture() const;
        string getPathTextureBump() const;
        string getPathTextureDisp() const;
        void setLight(string name, LightType type, vec3 position, vec3 color, vec3 direction = {});
        map <string, Light > getLight();
        void setConstantLight(ConstantLight constantLight);
        void preCompute(vec3 weight, vec3 a, vec3 b, vec3 c, vec3 normal_a, vec3 normal_b, vec3 normal_c);
    private:
        void setDot(string name, float dot);
        void setSpecular(string name, float spec);
        void setAttenuation(string name, float att);
        void setSpot(string name, const float spot);
    };
}

#endif /* Light_hpp */
