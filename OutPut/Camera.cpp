//
//  Camera.cpp
//  Rasterization
//
//  Created by Alexandre STEIN on 17/07/2020.
//  Copyright © 2020 Alexandre STEIN. All rights reserved.
//

#include "Camera.hpp"
using namespace Render3D;

vec3 Camera::get_position()
{
    return _position;
};


void Camera::set_position(float x, float y, float z)
{
    _position.x = x;
    _position.y = y;
    _position.z= z;
};


vec3 Camera::get_target()
{
    return _target;
};

void Camera::set_target(float x, float y, float z)
{
    _target.x = x;
    _target.y = y;
    _target.z = z;

};
