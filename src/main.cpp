// STD LIBRARY
#include "Texture.h"
#include "detail/type_mat.hpp"
#include "detail/type_vec.hpp"
#include "stb_image.h"
#include <Debug.h>
#include <fstream>
#include <iostream>
#include <sstream>

// OPENGL STUFF
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// MY INCLUDES
#include "IndexBufferObject.h"
#include "ShaderProgram.h"
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
int                addZ = 0;
int                addX = 0;

int main()
{
    // Initialize GLFW and set opengl version
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;
    std::cout << glGetString(GL_VERSION) << std::endl;

    // DATA
    float vertices[] = {
        // positions          // texture coords
        0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // top right
        0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f,  0.0f, 0.0f, 1.0f  // top left
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    // VAO GENERATION
    const VertexArrayObject VAO1;
    VAO1.Bind();

    const VertexBufferObject VBO1(vertices, sizeof(vertices));
    const IndexBufferObject  IBO1(indices, sizeof(indices));

    // COORDS
    const int numCoords = 3;
    const int numTextCoords = 2;
    const int stride = (numCoords + numTextCoords) * sizeof(float);

    VAO1.LinkAttrib(VBO1, 0, numCoords, GL_FLOAT, stride, (void*)0);

    VAO1.LinkAttrib(VBO1, 1, numTextCoords, GL_FLOAT, stride, (void*)(numCoords * sizeof(float)));

    VBO1.Unbind();
    VAO1.Unbind();
    IBO1.Unbind();

    // TEXTURE GENERATION

    const char* filePath = "res/texture/cheltoni.jpg";

    Texture texture1(filePath);

    // SHADER GENERATION
    const ShaderProgram shader1("res/shaders/basic/vertex.shader", "res/shaders/basic/fragment.shader");
    const ShaderProgram shader2("res/shaders/basic/vertex.shader", "res/shaders/basic/fragmentUniform.shader");

    shader2.Activate();

    texture1.Bind();

    // transformations

    shader2.Set1i("ourTexture", 0);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    // note that we're translating the scene in the reverse direction of where we want to move
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    shader2.SetMatrix4f("model", 1, false, model);
    shader2.SetMatrix4f("view", 1, false, view);
    shader2.SetMatrix4f("projection", 1, false, projection);

    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader2.Activate();
        view = glm::translate(view, glm::vec3(addX * 0.1f, 0.0f, addZ * 0.1f));
        shader2.SetMatrix4f("view", 1, false, view);
        VAO1.Bind();
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
        addZ = 0;
        addX = 0;
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        addZ = 1;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        addZ = -1;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        addX = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        addX = -1;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    std::cout << "Framebuffer resized" << std::endl;
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
