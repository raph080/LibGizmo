#include <iostream>

#include "renderer.h"
#include "shape.h"

Renderer::Renderer()
{
    std::string vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 pos;\n"
        "uniform mat4 proj;\n"
        "uniform mat4 view;\n"
        "uniform mat4 model;\n"
        "out vec4 color;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = proj * view * model * vec4(pos.xyz, 1.0);\n"
        "   color = vec4(1,0,0,1);\n"
        "}\0";
    std::string fragmentShaderSource = "#version 330 core\n"
                                       "in vec4 color;\n"
                                       "out vec4 FragColor;\n"
                                       "void main()\n"
                                       "{\n"
                                       "   FragColor = color;\n"
                                       "}\n\0";

    shader.loadFromStrings(vertexShaderSource, fragmentShaderSource);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)(sizeof(float) * 0));
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer::Render(Scene *scene, int width, int height)
{
    glEnable(GL_DEPTH_TEST);
    shader.use();
    shader.setMat4("proj", scene->activeCamera->projMatrix);
    shader.setMat4("view", scene->activeCamera->viewMatrix);

    for (Shape *shape : scene->shapes)
    {
        int arrSize = shape->faces.size() * 3 * 3;
        float data[arrSize];

        for (int i = 0; i < shape->faces.size(); i++)
        {
            Face face = shape->faces[i];

            for (int j = 0; j < face.points.size(); j++)
            {
                vec3 pt = *(face.points[j]);
                data[i * 3 * 3 + j * 3 + 0] = pt[0];
                data[i * 3 * 3 + j * 3 + 1] = pt[1];
                data[i * 3 * 3 + j * 3 + 2] = pt[2];
            }
        }

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

        shader.setMat4("model", shape->modelMatrix);

        glDrawArrays(GL_TRIANGLES, 0, shape->faces.size() * 3);
    }
}

void Renderer::RenderGizmo(Gizmo *gizmo, Scene *scene)
{
    Flag depthTestFlag = Flag::disable(GL_DEPTH_TEST);
    Flag lightingFlag = Flag::disable(GL_LIGHTING);
    Flag blendFlag = Flag::enable(GL_BLEND);
    Flag cullFaceFlag = Flag::disable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    gizmo->Render(scene->activeCamera->viewMatrix,
                  scene->activeCamera->projMatrix);

    depthTestFlag.restore();
    lightingFlag.restore();
    blendFlag.restore();
    cullFaceFlag.restore();
}