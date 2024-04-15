#ifndef GIZMO_H
#define GIZMO_H

#include <GL/glew.h>

#include <IGizmo.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "shape.h"

using namespace std;
using namespace glm;

class Gizmo
{
public:
    Gizmo()
    {
        gizmoMove = CreateMoveGizmo();
        gizmoRotate = CreateRotateGizmo();
        gizmoScale = CreateScaleGizmo();

        gizmoMove->SetDisplayScale(2.f);
        gizmoRotate->SetDisplayScale(2.f);
        gizmoScale->SetDisplayScale(2.f);

        gizmo = gizmoRotate;

        curLocation = IGizmo::LOCATE_WORLD;

        for (IGizmo *g : {gizmoMove, gizmoRotate, gizmoScale})
            g->SetLocation(curLocation);
    }

    void SetMove() { gizmo = gizmoMove; }

    void SetRotate() { gizmo = gizmoRotate; }

    void SetScale() { gizmo = gizmoScale; }

    void SetShape(Shape *shape)
    {
        for (IGizmo *g : {gizmoMove, gizmoRotate, gizmoScale})
        {
            g->SetScreenDimension(640, 480);
            g->SetEditMatrix(glm::value_ptr(shape->modelMatrix));
        }
    }

    void Render(mat4 view, mat4 proj)
    {
        if (gizmo)
        {
            gizmo->SetCameraMatrix(&view[0][0], &proj[0][0]);
            gizmo->Draw();
        }
    }

    IGizmo *gizmo, *gizmoMove, *gizmoRotate, *gizmoScale;
    IGizmo::LOCATION curLocation;
};

#endif