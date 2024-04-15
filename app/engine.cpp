#include "engine.h"

Engine::Engine()
{
    Shape *cube = new Cube();
    scene.shapes.push_back(cube);
    Camera *camera = new Camera();

    scene.activeCamera = camera;
    gizmo.SetShape(cube);
}

void Engine::OnMouseDown(vec2 pos) { gizmo.gizmo->OnMouseDown(pos.x, pos.y); }

void Engine::OnMouseUp(vec2 pos) { gizmo.gizmo->OnMouseUp(pos.x, pos.y); }

void Engine::OnMouseMove(vec2 pos) { gizmo.gizmo->OnMouseMove(pos.x, pos.y); }

void Engine::SetGizmoMove() { gizmo.SetMove(); }
void Engine::SetGizmoRotate() { gizmo.SetRotate(); }
void Engine::SetGizmoScale() { gizmo.SetScale(); }

void Engine::Render()
{
    renderer.Render(&scene, width, height);
    renderer.RenderGizmo(&gizmo, &scene);
}

void Engine::SetViewportSize(int width, int height)
{
    this->width = width;
    this->height = height;
}
