#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h> // Header File For Standard Input/Output

#include "utils.h"
#include <IGizmo.h>
#include <string>
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

std::string vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec4 col;\n"
    "layout (location = 1) in vec2 uv;\n"
    "layout (location = 2) in vec3 pos;\n"
    "uniform mat4 proj;\n"
    "uniform mat4 view;\n"
    "uniform mat4 model;\n"
    "out vec4 color;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = proj * view * model * vec4(pos.xyz, 1.0);\n"
    "   color = col;\n"
    "}\0";
std::string fragmentShaderSource = "#version 330 core\n"
                                   "in vec4 color;\n"
                                   "out vec4 FragColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   FragColor = color;\n"
                                   "}\n\0";

Shader shader;

unsigned int VBO, VAO;
unsigned int EBO;

int screenWidth, screenHeight;

glm::mat4 model = glm::mat4(1);
glm::mat4 view =
    glm::lookAt(glm::vec3(5, 5, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

IGizmo *gizmo = NULL;

IGizmo *gizmoMove, *gizmoRotate, *gizmoScale;

int InitGizmo() // Load Bitmaps And Convert To Textures
{
    gizmoMove = CreateMoveGizmo();
    gizmoRotate = CreateRotateGizmo();
    gizmoScale = CreateScaleGizmo();

    gizmo = gizmoMove;

    gizmo->SetLocation(IGizmo::LOCATE_LOCAL);
    gizmo->SetEditMatrix(glm::value_ptr(model));
    gizmo->SetScreenDimension(screenWidth, screenHeight);
    gizmoMove->SetDisplayScale(2.f);
    gizmoRotate->SetDisplayScale(2.f);
    gizmoScale->SetDisplayScale(2.f);
    return true; // Return The Status
}

GLvoid ReSizeGLScene(GLsizei width,
                     GLsizei height) // Resize And Initialize The GL Window
{
    if (height == 0) // Prevent A Divide By Zero By
    {
        height = 1; // Making Height Equal One
    }

    glViewport(0, 0, width, height); // Reset The Current Viewport

    screenWidth = width;
    screenHeight = height;

    if (gizmo)
        gizmo->SetScreenDimension(screenWidth, screenHeight);
}

void InitGL(GLvoid) // All Setup For OpenGL Goes Here
{
    glEnable(GL_TEXTURE_2D);              // Enable Texture Mapping ( NEW )
    glShadeModel(GL_SMOOTH);              // Enable Smooth Shading
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f); // Black Background
    glClearDepth(1.0f);                   // Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);              // Enables Depth Testing
    glDepthFunc(GL_LEQUAL);               // The Type Of Depth Testing To Do
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,
           GL_NICEST); // Really Nice Perspective Calculations
    glDepthMask(1);
}

void InitCube()
{
    float vertices[] = {
        0.6f, 0.6f, 0.6f, 1.f, 0.0f, 0.0f, -1.0f, -1.0f, 1.0f, 0.6f, 0.6f, 0.6f,
        1.f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.6f, 0.6f, 0.6f, 1.f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 0.6f, 0.6f, 0.6f, 1.f, 0.0f, 1.0f, -1.0f, 1.0f, 1.0f,
        // Back Face
        0.5f, 0.5f, 0.5f, 1.f, 1.0f, 0.0f, -1.0f, -1.0f, -1.0f, 0.5f, 0.5f,
        0.5f, 1.f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 0.5f, 0.5f, 0.5f, 1.f, 0.0f,
        1.0f, 1.0f, 1.0f, -1.0f, 0.5f, 0.5f, 0.5f, 1.f, 0.0f, 0.0f, 1.0f, -1.0f,
        -1.0f,
        // Top Face
        0.4f, 0.4f, 0.4f, 1.f, 0.0f, 1.0f, -1.0f, 1.0f, -1.0f, 0.4f, 0.4f, 0.4f,
        1.f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, 0.4f, 0.4f, 0.4f, 1.f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 0.4f, 0.4f, 0.4f, 1.f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
        // Bottom Face
        0.6f, 0.6f, 0.6f, 1.f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 0.6f, 0.6f,
        0.6f, 1.f, 0.0f, 1.0f, 1.0f, -1.0f, -1.0f, 0.6f, 0.6f, 0.6f, 1.f, 0.0f,
        0.0f, 1.0f, -1.0f, 1.0f, 0.6f, 0.6f, 0.6f, 1.f, 1.0f, 0.0f, -1.0f,
        -1.0f, 1.0f,
        // Right face
        0.5f, 0.5f, 0.5f, 1.f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.5f, 0.5f, 0.5f,
        1.f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 0.5f, 0.5f, 0.5f, 1.f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 0.5f, 0.5f, 0.5f, 1.f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f,
        // Left Face
        0.4f, 0.4f, 0.4f, 1.f, 0.0f, 0.0f, -1.0f, -1.0f, -1.0f, 0.4f, 0.4f,
        0.4f, 1.f, 1.0f, 0.0f, -1.0f, -1.0f, 1.0f, 0.4f, 0.4f, 0.4f, 1.f, 1.0f,
        1.0f, -1.0f, 1.0f, 1.0f, 0.4f, 0.4f, 0.4f, 1.f, 0.0f, 1.0f, -1.0f, 1.0f,
        -1.0f};

    unsigned int indices[] = {           // note that we start from 0!
                              0,  1,  2, // first triangle
                              0,  2,  3, // second triangle
                              4,  5,  6,  4,  6,  7,  8,  9,  10, 8,
                              10, 11, 12, 13, 14, 12, 14, 15, 16, 17,
                              18, 16, 18, 19, 20, 21, 22, 20, 22, 23};

    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s),
    // and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
                          (void *)(sizeof(float) * 4));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
                          (void *)(sizeof(float) * 6));
    glEnableVertexAttribArray(2);

    // note that this is allowed, the call to glVertexAttribPointer registered
    // VBO as the vertex attribute's bound vertex buffer object so afterwards we
    // can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally
    // modify this VAO, but this rarely happens. Modifying other VAOs requires a
    // call to glBindVertexArray anyways so we generally don't unbind VAOs (nor
    // VBOs) when it's not directly necessary.
    glBindVertexArray(0);
}

int DrawGLScene(GLvoid) // Here's Where We Do All The Drawing
{

    shader.use();

    glm::mat4 proj = glm::perspective(45.0f, screenWidth / (float)screenHeight,
                                      0.001f, 100000.0f);

    shader.setMat4("proj", proj);
    shader.setMat4("view", view);
    shader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    if (gizmo)
    {
        gizmo->SetCameraMatrix(&view[0][0], &proj[0][0]);

        Flag depthTestFlag = Flag::disable(GL_DEPTH_TEST);
        Flag lightingFlag = Flag::disable(GL_LIGHTING);
        Flag blendFlag = Flag::enable(GL_BLEND);
        Flag cullFaceFlag = Flag::disable(GL_CULL_FACE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        gizmo->Draw();

        depthTestFlag.restore();
        lightingFlag.restore();
        blendFlag.restore();
        cullFaceFlag.restore();
    }

    return true; // Keep Going
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        gizmo->OnMouseDown(xpos, ypos);
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        gizmo->OnMouseUp(xpos, ypos);
    }
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    gizmo->OnMouseMove(xposIn, yposIn);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods)
{
    if (glfwGetKey(window, GLFW_KEY_A) && action == GLFW_PRESS)
    {
        gizmo = gizmoMove;
        gizmo->SetLocation(IGizmo::LOCATE_LOCAL);
    }
    if (glfwGetKey(window, GLFW_KEY_Z) && action == GLFW_PRESS)
    {
        gizmo = gizmoRotate;
        gizmo->SetLocation(IGizmo::LOCATE_LOCAL);
    }
    if (glfwGetKey(window, GLFW_KEY_E) && action == GLFW_PRESS)
    {
        gizmo = gizmoScale;
        gizmo->SetLocation(IGizmo::LOCATE_LOCAL);
    }
    gizmo->SetEditMatrix(glm::value_ptr(model));
    gizmo->SetScreenDimension(screenWidth, screenHeight);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width
    // and height will be significantly larger than specified on retina
    // displays.
    ReSizeGLScene(width, height);
}

int main(void)
{
    GLFWwindow *window;

    /* Initialize the library */
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
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

    InitGL();
    InitGizmo();

    ReSizeGLScene(640, 480);

    shader.loadFromStrings(vertexShaderSource, fragmentShaderSource);

    glEnable(GL_DEPTH_TEST);

    InitCube();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        DrawGLScene();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}