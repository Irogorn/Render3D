//
//  Light.cpp
//  Rasterization
//
//  Created by Alexandre STEIN on 30/07/2020.
//  Copyright © 2020 Alexandre STEIN. All rights reserved.
//

#include "Light.hpp"
#include <fstream>
using namespace Render3D;

Lights::Lights() {}

Lights::Lights(const Lights &other)
{
    for (const auto &pair : other._mLights)
    {
        Light newLight(pair.second);
        _mLights[pair.first] = newLight;
    }
    _constantLight = other._constantLight;
}

void Lights::setLight(string name, LightType type, vec3 position, vec3 color, vec3 direction)
{
    Light l(type);
    l._positions = position;
    l._direction = direction;
    l._color = color;
    _mLights.insert(std::make_pair(name, l));
}

void Lights::setDot(string name, float dot)
{
    try
    {
        _mLights.at(name).dot = dot; // Lance std::out_of_range si n'existe pas
    }
    catch (const std::out_of_range &e)
    {
        std::cerr << "Light '" << name << "' not found!" << std::endl;
    }
}

void Lights::setSpecular(string name, float spec)
{
    try
    {
        _mLights.at(name).spec = spec;
    }
    catch (const std::out_of_range &e)
    {
        std::cerr << "Light '" << name << "' not found!" << std::endl;
    }
}

void Lights::setAttenuation(string name, float att)
{
    try
    {
        _mLights.at(name).attenuation = att;
    }
    catch (const std::out_of_range &e)
    {
        std::cerr << "Light '" << name << "' not found!" << std::endl;
    }
}

void Lights::setSpot(string name, const float spot)
{
    try
    {
        _mLights.at(name).spot = spot;
    }
    catch (const std::out_of_range &e)
    {
        std::cerr << "Light '" << name << "' not found!" << std::endl;
    }
}

map<string, Light> Lights::getLight()
{
    return _mLights;
}

void Lights::ComputeLightFlat(vec3 a, vec3 b, vec3 c, vec3 normal_a, vec3 normal_b, vec3 normal_c)
{
    vec3 lightDirection{}, N{};
    vec3 face_centroid = a + b + c;
    face_centroid /= 3.0;

    for (std::map<string, Light>::iterator it = _mLights.begin(); it != _mLights.end(); ++it)
    {
        if (it->second._typeOfLight == LightType::DirectionLight)
        {
            lightDirection = -normalize(it->second._direction);
        }
        else
        {
            lightDirection = normalize(it->second._positions - face_centroid);
        }

        N = normal_a + normal_b + normal_c;
        N /= 3.0f;
        N = normalize(N);

        setDot(it->first, std::max(0.0f, glm::dot(N, lightDirection)));
    }
}

void Lights::ComputeLightGouraud(vec3 weight, vec3 a, vec3 b, vec3 c, vec3 normal_a, vec3 normal_b, vec3 normal_c)
{
    vec3 lightDirection_a, lightDirection_b, lightDirection_c;
    for (std::map<string, Light>::iterator it = _mLights.begin(); it != _mLights.end(); ++it)
    {
        if (it->second._typeOfLight == LightType::DirectionLight)
        {
            lightDirection_a = -normalize(it->second._direction);
            lightDirection_b = -normalize(it->second._direction);
            lightDirection_c = -normalize(it->second._direction);
        }
        else
        {
            vec3 tp = it->second._positions;
            lightDirection_a = it->second._positions - a;
            lightDirection_a = normalize(lightDirection_a);
            lightDirection_b = it->second._positions - b;
            lightDirection_b = normalize(lightDirection_b);
            lightDirection_c = it->second._positions - c;
            lightDirection_c = normalize(lightDirection_c);
        }

        normal_a = normalize(normal_a);
        float tmp = glm::dot(normal_a, lightDirection_a);
        float dot_a = std::max(0.0f, std::min(1.0f, tmp));

        normal_b = normalize(normal_b);
        tmp = glm::dot(normal_b, lightDirection_b);
        float dot_b = std::max(0.0f, std::min(1.0f, tmp));

        normal_c = normalize(normal_c);
        tmp = glm::dot(normal_c, lightDirection_c);
        float dot_c = std::max(0.0f, std::min(1.0f, tmp));

        setDot(it->first, dot_a * weight.x + dot_b * weight.y + dot_c * weight.z);
    }
}

void Lights::ComputeLightPhong(mat3x3 TBN, bool Parallax)
{
    for (const auto &light : _mLights)
    {
        vec3 lightDirection;

        if (light.second._typeOfLight == LightType::DirectionLight)
        {
            lightDirection = -normalize(light.second._direction);
            if (Parallax)
            {
                TransformVectorByMatrix3x3(light.second._direction, transpose(TBN), lightDirection);
                TransformVectorByMatrix3x3(N, transpose(TBN), N);
                lightDirection = normalize(lightDirection);
            }
        }
        else
        {
            lightDirection = normalize(light.second._positions - point3D_position);

            if (Parallax)
            {
                vec3 tangentialLightPosistion{};
                TransformVectorByMatrix3x3(light.second._positions, transpose(TBN), tangentialLightPosistion);
                TransformVectorByMatrix3x3(N, transpose(TBN), N);
                vec3 tangentialFrogPosition{};
                TransformVectorByMatrix3x3(point3D_position, transpose(TBN), tangentialFrogPosition);
                lightDirection = tangentialLightPosistion - tangentialFrogPosition;
                lightDirection = normalize(lightDirection);
            }
        }

        setDot(light.first, std::max(0.0f, glm::dot(N, lightDirection)));
    }
}

void Lights::ComputeSpecular(vec3 camera_position, float s, mat3x3 TBN, bool parallax)
{
    vec3 lightDirection, N;
    vec3 camera_direction = camera_position - point3D_position;
    camera_direction = normalize(camera_direction);

    for (std::map<string, Light>::iterator it = _mLights.begin(); it != _mLights.end(); ++it)
    {
        if (it->second._typeOfLight == LightType::DirectionLight)
        {

            lightDirection = normalize(-it->second._direction);
            if (parallax)
            {
                TransformVectorByMatrix3x3(it->second._direction, transpose(TBN), lightDirection);
                lightDirection = normalize(lightDirection);
            }
        }
        else
        {
            lightDirection = normalize(-it->second._positions - point3D_position);

            if (parallax)
            {
                vec3 tangentialLightPosistion{};
                TransformVectorByMatrix3x3(it->second._positions, transpose(TBN), tangentialLightPosistion);
                vec3 tangentialFrogPosition{};
                TransformVectorByMatrix3x3(point3D_position, transpose(TBN), tangentialFrogPosition);
                lightDirection = tangentialLightPosistion - tangentialFrogPosition;
                lightDirection = normalize(lightDirection);
            }
        }

        float dot = std::max(glm::dot(N, lightDirection), 0.0f);
        vec3 reflexion = -lightDirection + (2.0f * dot * N); // reflect(-lightDirection, N);
        setSpecular(it->first, pow(std::max(glm::dot(camera_direction, reflexion), 0.0f), _constantLight.Ns));
    }
}

vec3 Lights::getIntensity(vec3 weight, Face f)
{
    float ao = 0.7f; // f.A.occlusion * weight.x + f.B.occlusion * weight.y + f.C.occlusion * weight.z;

    __m128 Ke_simd = vec3_to_m128(_constantLight.Ke);
    __m128 Ka_simd = vec3_to_m128(_constantLight.Ka);
    __m128 Kd_simd = vec3_to_m128(_constantLight.Kd);
    __m128 Ks_simd = vec3_to_m128(_constantLight.Ks);

    __m128 ao_simd = _mm_set1_ps(ao);

    __m128 i_simd = _mm_fmadd_ps(Ka_simd, ao_simd, Ke_simd);

    __m128 directLight_simd = _mm_setzero_ps();
    __m128 pointLight_simd = _mm_setzero_ps();
    __m128 spotLight_simd = _mm_setzero_ps();

    for (std::map<string, Light>::iterator it = _mLights.begin(); it != _mLights.end(); ++it)
    {
        if (it->second._typeOfLight == LightType::DirectionLight)
        {
            __m128 color_simd = vec3_to_m128(it->second._color);
            __m128 dot_simd = _mm_set1_ps(it->second.dot);
            __m128 spec_simd = _mm_set1_ps(it->second.spec);

            __m128 DirectDot_simd = _mm_mul_ps(dot_simd, color_simd);
            directLight_simd = _mm_fmadd_ps(Kd_simd, DirectDot_simd, directLight_simd);
        }
        else
        {
            if (it->second._typeOfLight == LightType::PointLight)
            {
                __m128 color_simd = vec3_to_m128(it->second._color);
                __m128 dot_simd = _mm_set1_ps(it->second.dot);
                __m128 spec_simd = _mm_set1_ps(it->second.spec);
                __m128 att_simd = _mm_set1_ps(it->second.attenuation);

                __m128 PointDot_simd = _mm_mul_ps(dot_simd, color_simd);
                __m128 PointSpec_simd = _mm_mul_ps(spec_simd, color_simd);

                __m128 temp = _mm_mul_ps(Kd_simd, PointDot_simd);

                temp = _mm_fmadd_ps(Ks_simd, PointSpec_simd, temp);

                pointLight_simd = _mm_fmadd_ps(temp, att_simd, pointLight_simd);
            }
            else if (it->second._typeOfLight == LightType::SpotLight)
            {
                __m128 color_simd = vec3_to_m128(it->second._color);
                __m128 dot_simd = _mm_set1_ps(it->second.dot);
                __m128 spec_simd = _mm_set1_ps(it->second.spec);
                __m128 spot_simd = _mm_set1_ps(it->second.spot);

                __m128 SpotDot_simd = _mm_mul_ps(dot_simd, color_simd);
                __m128 SpotSpec_simd = _mm_mul_ps(spec_simd, color_simd);

                __m128 temp = _mm_mul_ps(Kd_simd, SpotDot_simd);

                temp = _mm_fmadd_ps(Ks_simd, SpotSpec_simd, temp);

                spotLight_simd = _mm_fmadd_ps(temp, spot_simd, spotLight_simd);
            }
        }
    }

    i_simd = _mm_add_ps(i_simd, directLight_simd);
    i_simd = _mm_add_ps(i_simd, pointLight_simd);
    i_simd = _mm_add_ps(i_simd, spotLight_simd);

    // Clamping [0, 1]
    __m128 zero = _mm_setzero_ps();
    __m128 one = _mm_set1_ps(1.0f);
    i_simd = clamp_m128(i_simd, zero, one);

    return m128_to_vec3(i_simd);
}

vec3 Lights::getPosition(string name)
{
    return _mLights.at(name)._positions;
}

vec3 Lights::getConstantKd()
{
    vec3 out = {};
    out.x = 255 * _constantLight.Kd.x;
    out.y = 255 * _constantLight.Kd.y;
    out.z = 255 * _constantLight.Kd.z;
    return out;
}

string Lights::getPathTexture() const
{
    return _constantLight.pathTexture;
}
string Lights::getPathTextureBump() const
{
    return _constantLight.pathTextureBump;
}
string Lights::getPathTextureDisp() const
{
    return _constantLight.pathTextureDisp;
}

void Lights::ComputeAttenuation(string name, float C1, float C2, float radius)
{
    if (_mLights.find(name) == _mLights.end())
        return;

    // ========================================================================
    // ÉTAPE 1: Calculer la distance du point au CENTRE de la sphère
    // ========================================================================
    vec3 lightDirection = (_mLights[name]._positions - point3D_position);
    float lightDistance = lightDirection.length();

    // ========================================================================
    // ÉTAPE 2: Test d'appartenance à la sphère - CUT-OFF COMPLET
    // ========================================================================
    // Si le point est HORS de la sphère → pas de lumière du tout
    if (lightDistance > radius)
    {
        setAttenuation(name, 0.0f);
        return; // Optimisation: sortir immédiatement
    }

    // ========================================================================
    // ÉTAPE 3: Atténuation basée sur la distance (formule quadratique)
    // ========================================================================
    // Plus on s'éloigne du centre, moins la lumière est intense
    float attenuation = 1.0f /
                        (1.0f + C1 * lightDistance + C2 * lightDistance * lightDistance);

    // ========================================================================
    // ÉTAPE 4: SMOOTH FALLOFF près des bords de la sphère
    // ========================================================================
    // Zone de transition: entre 75% et 100% du radius
    // Évite une coupure brutale aux bords de la sphère
    float transitionStart = radius * 0.55f; // 75% du radius

    if (lightDistance > transitionStart)
    {
        // Calculer le facteur de falloff (1.0 à 0.0)
        float transitionRange = radius - transitionStart; // 25% du radius
        float distanceInTransition = lightDistance - transitionStart;
        float falloff = 1.0f - (distanceInTransition / transitionRange);

        // Sécurité: s'assurer que falloff reste entre 0 et 1
        falloff = std::max(0.0f, std::min(1.0f, falloff));

        // Appliquer le falloff à l'atténuation
        attenuation *= falloff;
    }

    setAttenuation(name, attenuation);
}

void Lights::ComputeSpotLight(string name, float cutoff, float outerCutoff)
{
    if (_mLights.find(name) == _mLights.end())
        return;

    vec3 l = point3D_position - _mLights[name]._positions;

    float theta = glm::dot(normalize(l), normalize(_mLights[name]._direction));
    float epsilon = cutoff - outerCutoff; // 0.9978f - 0.90f;

    setSpot(name, std::max(0.0f, std::min(1.0f, (theta - outerCutoff) / epsilon)));
}

void Lights::setConstantLight(ConstantLight constantLight)
{
    _constantLight = constantLight;
}

void Lights::preCompute(vec3 weight, vec3 a, vec3 b, vec3 c, vec3 normal_a, vec3 normal_b, vec3 normal_c)
{
    point3D_position = a * weight.x + b * weight.y + c * weight.z;

    N = normal_a * weight.x + normal_b * weight.y + normal_c * weight.z;

    N = normalize(N);
}
