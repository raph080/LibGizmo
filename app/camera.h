#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

class Camera
{
public:
    mat4 viewMatrix;
    mat4 projMatrix;

    Camera()
    {
        viewMatrix = lookAt(vec3(5, 5, 5), vec3(0, 0, 0), vec3(0, 1, 0));
        projMatrix = perspective(45.0f, 640 / (float)480, 0.001f, 100000.0f);
    }
};

#endif