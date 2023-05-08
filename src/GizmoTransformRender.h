///////////////////////////////////////////////////////////////////////////////////////////////////
// LibGizmo
// File Name :
// Creation : 10/01/2012
// Author : Cedric Guillemet
// Description : LibGizmo
//
/// Copyright (C) 2012 Cedric Guillemet
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do
/// so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#ifndef GIZMOTRANSFORMRENDER_H__
#define GIZMOTRANSFORMRENDER_H__

#define GLFW_INCLUDE_NONE
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "LibBase.h"

typedef tvector4 tplane;

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

class CGizmoTransformRender
{
public:
    CGizmoTransformRender() {}
    virtual ~CGizmoTransformRender() {}

    static glm::mat4 proj;
    static glm::mat4 view;

    static void PrepareCircle(const tvector3 &orig, const tvector3 &vtx,
                              const tvector3 &vty, tvector4 col);
    static void PrepareCircleHalf(const tvector3 &orig, const tvector3 &vtx,
                                  const tvector3 &vty, tplane &camPlan,
                                  tvector4 col);
    static void PrepareCubeAxis(const tvector3 &orig, const tvector3 &axis,
                                const tvector3 &vtx, const tvector3 &vty,
                                float fct, float fct2, const tvector4 &col);
    static void PrepareConeAxis(const tvector3 &orig, const tvector3 &axis,
                                const tvector3 &vtx, const tvector3 &vty,
                                float fct, float fct2, const tvector4 &col);
    static void PrepareCamem(const tvector3 &orig, const tvector3 &vtx,
                             const tvector3 &vty, float ng, tvector4 innerCol,
                             tvector4 borderCol);
    static void PrepareQuad(const tvector3 &orig, float size,
                            const tvector3 &axisU, const tvector3 &axisV,
                            tvector4 innerCol, tvector4 borderCol);

    static void Draw();

private:
    static unsigned int SHADER_PROG_ID, VBO, VAO;
    static std::vector<float> lineData;
    static std::vector<float> triangleData;

    static void Init();
    static void AddTriangleToDraw(tvector3 pt1, tvector3 pt2, tvector3 pt3,
                                  tvector4 col);
    static void AddLineToDraw(tvector3 pt1, tvector3 pt2, tvector4 col);
};

#endif
