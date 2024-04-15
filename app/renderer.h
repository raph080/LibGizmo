#ifndef RENDERER_H
#define RENDERER_H

#include <glm/glm.hpp>
#include <vector>

#include "gizmo.h"
#include "scene.h"
#include "shader.h"

using namespace std;
using namespace glm;

class Flag
{
public:
    Flag(GLenum flag)
    {
        id = flag;
        isEnabled = glIsEnabled(id);
    }
    static Flag enable(GLenum flag)
    {
        Flag obj = Flag(flag);
        glEnable(flag);
        return obj;
    }
    static Flag disable(GLenum flag)
    {
        Flag obj = Flag(flag);
        glDisable(flag);
        return obj;
    }
    void restore()
    {
        if (isEnabled)
            glEnable(id);
        else
            glDisable(id);
    }

private:
    GLenum id;
    bool isEnabled;
};

class Renderer
{
public:
    Renderer();
    void Render(Scene *scene, int width, int height);
    void RenderGizmo(Gizmo *gizmo, Scene *scene);

private:
    unsigned int VBO, VAO;
    Shader shader;
};

#endif