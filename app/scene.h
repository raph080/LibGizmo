#ifndef SCENE_H
#define SCENE_H

#include <glm/glm.hpp>
#include <vector>

#include "camera.h"
#include "gizmo.h"
#include "shape.h"

using namespace std;
using namespace glm;

class Scene
{
public:
    vector<Shape *> shapes;
    Camera *activeCamera;
    Gizmo gizmo;
};

#endif