    #pragma once
    
    struct ConstantLight
    {
        std::string pathTexture = "";
        std::string pathTextureBump=  "";
        std::string pathTextureDisp = "";
        float Ns = 0.0f;
        vec3 Ka = { 0.0f,0.0f,0.0f };
        vec3 Kd = { 1.0f,1.0f,1.0f };
        vec3 Ks = { 0.0f,0.0f,0.0f };
        vec3 Ke = { 0.0f,0.0f,0.0f };
    };