//STD LIBRARY
#include <fstream>
#include <iostream>
#include <sstream>
#include <Debug.h>
#include "Texture.h"
#include "detail/type_mat.hpp"
#include "detail/type_vec.hpp"
#include "stb_image.h"

//OPENGL STUFF
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//MY INCLUDES
#include "IndexBufferObject.h"
#include "ShaderProgram.h"
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    //Initialize GLFW and set opengl version
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
    std::cout << glGetString(GL_VERSION) << std:: endl;

    //DATA
    float vertices[] =
    {
    /*
       |     COORDINATES     |      COLOR       |  TEX COORD  |
          x       y      z      r     g     b                       */
        -0.5f,  -0.5f,  0.0f,  1.0f, 0.0f, 0.0f,   0.0f, 0.0f,  //  bottom  left
         0.5f,  -0.5f,  0.0f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,  //  bottom  right
        -0.5f,   0.5f,  0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 1.0f,  //  top     left
         0.5f,   0.5f,  0.0f,  1.0f, 0.0f, 0.0f,   1.0f, 1.0f   //  top     right
    };

    unsigned int indices[] =
    {
        0, 1, 2,
        1, 2, 3
    };


    //VAO GENERATION
    const VertexArrayObject VAO1;
    VAO1.Bind();

    const VertexBufferObject VBO1(vertices, sizeof(vertices));
    const IndexBufferObject IBO1(indices, sizeof(indices));

    //COORDS
    const int numCoords = 3;
    const int numColors = 3;
    const int numTextCoords = 2;
    const int stride = (numCoords + numColors + numTextCoords) * sizeof(float);

    VAO1.LinkAttrib(VBO1, 0, numCoords, GL_FLOAT, stride, (void*)0);

    VAO1.LinkAttrib(VBO1, 1, numColors, GL_FLOAT, stride, (void*)(numCoords * sizeof(float)));

    VAO1.LinkAttrib(VBO1, 2, numTextCoords, GL_FLOAT, stride, (void*)((numCoords + numColors) * sizeof(float)));

    VBO1.Unbind();
    VAO1.Unbind();
    IBO1.Unbind();

    //TEXTURE GENERATION


    const char* filePath ="res/texture/obamna2.png";

    Texture texture1(filePath);

    //SHADER GENERATION
    const ShaderProgram shader1("res/shaders/basic/vertex.shader", "res/shaders/basic/fragment.shader");
    const ShaderProgram shader2("res/shaders/basic/vertex.shader", "res/shaders/basic/fragmentUniform.shader");

    shader2.Activate();

    texture1.Bind();
    
    //transformations

    glm::mat4 trans = glm::mat4(1.0f); //identity

    //note we apply the rotation first then we scale!
    trans = glm::rotate(trans, glm::radians(135.0f), glm::vec3(0.0, 0.0, 1.0)); //rotate around z
    trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5)); // scale by 0.5f

    shader2.SetMatrix4f("transform", 1, false, trans);
    shader2.Set1i("ourTexture", 0);



    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float rot = 0.0f;
        int counter = 0;

        if (counter % 100 == 0)
        {
            rot--;
        }

        trans = glm::rotate(trans, glm::radians(rot), glm::vec3(0.0, 0.0, 1.0)); //rotate around z

        shader2.SetMatrix4f("transform", 1, false, trans);
        shader2.Activate();
        VAO1.Bind();
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
        counter++;

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
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
