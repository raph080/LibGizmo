#ifndef ENGINE_H
#define ENGINE_H

#include <glm/glm.hpp>
#include <vector>

#include "camera.h"
#include "gizmo.h"
#include "renderer.h"
#include "scene.h"
#include "shape.h"

using namespace std;
using namespace glm;

class Engine
{
public:
    Engine();
    void OnMouseDown(vec2 pos);
    void OnMouseUp(vec2 pos);
    void OnMouseMove(vec2 pos);
    void SetGizmoMove();
    void SetGizmoRotate();
    void SetGizmoScale();
    void Render();
    void SetViewportSize(int width, int height);

    Scene scene;
    Gizmo gizmo;
    Renderer renderer;
    int width, height;
};

#endif