//
//  Camera.hpp
//  Rasterization
//
//  Created by Alexandre STEIN on 17/07/2020.
//  Copyright © 2020 Alexandre STEIN. All rights reserved.
//

#ifndef Camera_hpp
#define Camera_hpp

#include <stdio.h>
#include "../Tools/MatrixTools.h"
#include <gtc/matrix_transform.hpp>

namespace Render3D
{
    class Camera
    {
        private:
            vec3 _position;
            vec3 _target;

        public:
            //Getters and Setters.
            vec3 get_position();
            void set_position(float x, float y, float z);
            vec3 get_target();
            void set_target(float x, float y, float z);
    };
};
#endif /* Camera_hpp */
