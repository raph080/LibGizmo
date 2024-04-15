#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "engine.h"
#include "gizmo.h"
#include "scene.h"
#include "shape.h"

Engine *engine;

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        engine->OnMouseDown(vec2(xpos, ypos));

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        engine->OnMouseUp(vec2(xpos, ypos));
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    engine->OnMouseMove(vec2(xpos, ypos));
}

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods)
{
    if (glfwGetKey(window, GLFW_KEY_A) && action == GLFW_PRESS)
        engine->SetGizmoMove();

    if (glfwGetKey(window, GLFW_KEY_Z) && action == GLFW_PRESS)
        engine->SetGizmoRotate();

    if (glfwGetKey(window, GLFW_KEY_E) && action == GLFW_PRESS)
        engine->SetGizmoScale();
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    engine->SetViewportSize(width, height);
}

int main(void)
{
    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // start GLEW extension handler
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    engine = new Engine();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        engine->Render();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
