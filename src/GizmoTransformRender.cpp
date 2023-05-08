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

#include "GizmoTransformRender.h"
#include "LibBase.h"
#include <vector>

unsigned int CGizmoTransformRender::SHADER_PROG_ID = 0;
unsigned int CGizmoTransformRender::VBO = 0;
unsigned int CGizmoTransformRender::VAO = 0;

glm::mat4 CGizmoTransformRender::proj = glm::mat4();
glm::mat4 CGizmoTransformRender::view = glm::mat4();
std::vector<float> CGizmoTransformRender::lineData = std::vector<float>();
std::vector<float> CGizmoTransformRender::triangleData = std::vector<float>();

void CGizmoTransformRender::Init()
{
    // vertex shader
    const char *vShaderCode =
        "#version 330 core\n"
        "layout (location = 0) in vec3 pos;\n"
        "layout (location = 1) in vec4 col;\n"
        "uniform mat4 proj;\n"
        "uniform mat4 view;\n"
        "out vec4 color;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = proj * view * vec4(pos.xyz, 1.0);\n"
        "   color = col;\n"
        "}\0";
    // fragment shader
    const char *fShaderCode = "#version 330 core\n"
                              "in vec4 color;\n"
                              "out vec4 FragColor;\n"
                              "void main()\n"
                              "{\n"
                              "   FragColor = color;\n"
                              "}\n\0";

    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    // shader Program
    SHADER_PROG_ID = glCreateProgram();
    glAttachShader(SHADER_PROG_ID, vertex);
    glAttachShader(SHADER_PROG_ID, fragment);
    glLinkProgram(SHADER_PROG_ID);
    // delete the shaders as they're linked into our program now and no
    // longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float),
                          (void *)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered
    // VBO as the vertex attribute's bound vertex buffer object so
    // afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CGizmoTransformRender::AddTriangleToDraw(tvector3 pt1, tvector3 pt2,
                                              tvector3 pt3, tvector4 col)
{
    float *buffer = &col[0];
    for (tvector3 pt : {pt1, pt2, pt3})
    {
        triangleData.insert(triangleData.end(), {pt.x, pt.y, pt.z});
        triangleData.insert(triangleData.end(), buffer, buffer + 4);
    }
}
void CGizmoTransformRender::AddLineToDraw(tvector3 pt1, tvector3 pt2,
                                          tvector4 col)
{
    float *buffer = &col[0];
    for (tvector3 pt : {pt1, pt2})
    {
        lineData.insert(lineData.end(), {pt.x, pt.y, pt.z});
        lineData.insert(lineData.end(), buffer, buffer + 4);
    }
}

void CGizmoTransformRender::Draw()
{
    if (SHADER_PROG_ID == 0)
        Init();

    glUseProgram(SHADER_PROG_ID);
    glUniformMatrix4fv(glGetUniformLocation(SHADER_PROG_ID, "proj"), 1,
                       GL_FALSE, &CGizmoTransformRender::proj[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(SHADER_PROG_ID, "view"), 1,
                       GL_FALSE, &CGizmoTransformRender::view[0][0]);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, lineData.size() * sizeof(lineData),
                 &lineData[0], GL_DYNAMIC_DRAW);

    glDrawArrays(GL_LINES, 0, lineData.size() / 7);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, triangleData.size() * sizeof(triangleData),
                 &triangleData[0], GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, triangleData.size() / 7);

    lineData.clear();
    triangleData.clear();
}

void CGizmoTransformRender::PrepareCircle(const tvector3 &orig,
                                          const tvector3 &vtx,
                                          const tvector3 &vty, tvector4 color)
{
    tvector3 prevVt;
    for (int i = 0; i <= 50; i++)
    {
        tvector3 vt;
        vt = vtx * cos((2 * ZPI / 50) * i);
        vt += vty * sin((2 * ZPI / 50) * i);
        vt += orig;

        if (i > 0)
            AddLineToDraw(prevVt, vt, color);

        prevVt = vt;
    }
}

void CGizmoTransformRender::PrepareCircleHalf(const tvector3 &orig,
                                              const tvector3 &vtx,
                                              const tvector3 &vty,
                                              tplane &camPlan, tvector4 color)
{

    bool isFirstVt = true;
    tvector3 prevVt;
    for (int i = 0; i <= 30; i++)
    {
        tvector3 vt;
        vt = vtx * cos((ZPI / 30) * i);
        vt += vty * sin((ZPI / 30) * i);
        vt += orig;
        if (camPlan.DotNormal(vt))
        {
            if (!isFirstVt)
                AddLineToDraw(prevVt, vt, color);

            isFirstVt = false;
            prevVt = vt;
        }
    }
}

void CGizmoTransformRender::PrepareCubeAxis(const tvector3 &orig,
                                            const tvector3 &axis,
                                            const tvector3 &vtx,
                                            const tvector3 &vty, float fct,
                                            float fct2, const tvector4 &col)
{
    AddLineToDraw(orig, orig + axis, col);

    tvector3 cCntr = orig + axis; // cube center
    float cSize = fct * 2;        // cube size

    tvector3 pts[8];
    pts[0] = orig + axis * (1 - cSize) - (vtx + vty) * cSize / 2.0f;
    pts[1] = pts[0] + (axis * cSize);
    pts[2] = pts[0] + (axis + vtx) * cSize;
    pts[3] = pts[0] + (vtx * cSize);
    pts[4] = pts[0] + vty * cSize;
    pts[5] = pts[1] + vty * cSize;
    pts[6] = pts[2] + vty * cSize;
    pts[7] = pts[3] + vty * cSize;

    unsigned int index[] = {
        0, 1, 2, 0, 2, 3, // bottom
        4, 5, 6, 4, 6, 7, // top
        1, 2, 6, 1, 6, 5, // front
        0, 3, 7, 0, 7, 4, // back
        0, 1, 5, 0, 5, 4, // left
        2, 3, 7, 2, 7, 6  // right
    };

    for (int i = 0; i < 36; i += 3)
        AddTriangleToDraw(pts[index[i]], pts[index[i + 1]], pts[index[i + 2]],
                          col);
}

void CGizmoTransformRender::PrepareConeAxis(const tvector3 &orig,
                                            const tvector3 &axis,
                                            const tvector3 &vtx,
                                            const tvector3 &vty, float fct,
                                            float fct2, const tvector4 &col)
{
    AddLineToDraw(orig, orig + axis, col);

    tvector3 prevPt;
    for (int i = 0; i <= 30; i++)
    {
        tvector3 pt;
        pt = vtx * cos(((2 * ZPI) / 30.0f) * i) * fct;
        pt += vty * sin(((2 * ZPI) / 30.0f) * i) * fct;
        pt += axis * fct2;
        pt += orig;

        if (i > 0)
            AddTriangleToDraw(prevPt, pt, orig + axis, col);

        prevPt = pt;
    }
}

void CGizmoTransformRender::PrepareCamem(const tvector3 &orig,
                                         const tvector3 &vtx,
                                         const tvector3 &vty, float ng,
                                         tvector4 innerCol, tvector4 borderCol)
{
    tvector3 prevVt;
    tvector3 vt;
    for (int i = 0; i <= 50; i++)
    {
        vt = vtx * cos(((ng) / 50) * i);
        vt += vty * sin(((ng) / 50) * i);
        vt += orig;

        if (i == 0)
            AddLineToDraw(orig, vt, borderCol);

        if (i > 0)
        {
            AddLineToDraw(prevVt, vt, borderCol);
            AddTriangleToDraw(orig, prevVt, vt, innerCol);
        }
        prevVt = vt;
    }

    AddLineToDraw(orig, vt, borderCol);
}

void CGizmoTransformRender::PrepareQuad(const tvector3 &orig, float size,
                                        const tvector3 &axisU,
                                        const tvector3 &axisV,
                                        tvector4 innerCol, tvector4 borderCol)
{

    tvector3 pts[4];
    pts[0] = orig;
    pts[1] = orig + (axisU * size);
    pts[2] = orig + (axisU + axisV) * size;
    pts[3] = orig + (axisV * size);

    AddTriangleToDraw(pts[0], pts[1], pts[2], innerCol);
    AddTriangleToDraw(pts[0], pts[2], pts[3], innerCol);

    AddLineToDraw(pts[0], pts[1], borderCol);
    AddLineToDraw(pts[1], pts[2], borderCol);
    AddLineToDraw(pts[2], pts[3], borderCol);
    AddLineToDraw(pts[3], pts[0], borderCol);
}
