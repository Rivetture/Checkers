#include <chrono>
#include <thread>
#include <vector>
#include <math.h>
#include <cstdio>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#define GLEW_STATIC
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#define PI 3.14159265358979323846


//Camera Position
GLfloat Tx = 0; GLfloat Ty = 0; GLfloat Tz = -90;
//Camera Rotation
GLfloat rot = 90; GLfloat Rx = 1; GLfloat Ry = 0; GLfloat Rz = 0;

//If error on input happens this func is called
void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void controls(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
        if (key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(window, GL_TRUE);
}

GLFWwindow* initWindow(const int resX, const int resY)
{
    //Failed to load glfw
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return NULL;
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing

    // Open a window and create its OpenGL context
    GLFWwindow* window = glfwCreateWindow(resX, resY, "Checkers", NULL, NULL);

    //Always check, may be driver error
    if (window == NULL)
    {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return NULL;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, controls);

    // Get info of GPU and supported OpenGL version
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("OpenGL version supported %s\n", glGetString(GL_VERSION));

    glEnable(GL_DEPTH_TEST); // Depth Testing
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    return window;
}

void drawCube(Shader &ourShader)
{

    float vertices[] =
    {
        //Top
        
       -0.5f,1.0f,0.5f,   0.25f, 0.25f, 0.25f, //0
        0.5f,1.0f,0.5f,   0.5f, 1.0f, 1.0f, //1
        0.5f,1.0f,-0.5f,  1.0f, 1.0f, 0.5f, //2
       -0.5f,1.0f,-0.5f,  1.0f, 0.25f, 0.0f, //3 
       -0.5f,-1.0f,0.5f,  0.0f, 0.0f, 1.0f, //4
        0.5f,-1.0f,0.5f,  0.0f, 0.5f, 1.0f, //5
        0.5f,-1.0f,-0.5f, 0.25f, 0.5f, 1.0f, //6
       -0.5f,-1.0f,-0.5f, 0.0f, 0.75f, 1.0f  //7
  
    };

    unsigned int indices[] = {
    0,1,3, // Top
    1,2,3,  
    
    4,5,6, // Bottom
    6,7,4,
  
    2,1,6,
    1,5,6,
  
    7,4,3,
    4,0,3,
   
    0,4,1,
    1,4,5,
   
    7,3,6,
    3,2,6
    };
    
    
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) , vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

  
    ourShader.use();
    glm::mat4 model = glm::translate(glm::mat4(1.0f),glm::vec3(0,0,0));

    model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1,0,0));
    glm::mat4 mvp = model;
    ourShader.setMat4("mvp", mvp);
    glBindVertexArray(VAO);
   
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1,&VAO);
    glBindVertexArray(0);
    
}

void drawChecker(const unsigned int row = 0, const unsigned int column = 0)
{
    /*
        Function drw_polygon:
        Arguments:
            n - number of sides
            arg - starting angle (not so important at all)
            mult - multiplying sides to incrase their length
            v - cylinder height
    */

    //Circle Precision
    int n = 100;
    int arg = 0;
    //radius
    float mult = 4;
    //height
    float v = 2;

    glBegin(GL_POLYGON);
    glColor4f(1.0, 0.0, 0.0, 1.0);
    for (int i = arg; i <= (360 + arg); i += (360 / n)) {
        float a = i * PI / 140; // degrees to radians
        glVertex3f(mult * cos(a), mult * sin(a), 0.0);

    }
    glEnd();

    // glRotatef(45, 1, 0, 0);

     // Cylinder Top
    glBegin(GL_POLYGON);
    glColor4f(0.0, 0.0, 1.0, 1.0);
    for (int i = arg; i <= (360 + arg); i += (360 / n)) {
        float a = i * PI / 140; // degrees to radians
        glVertex3f(mult * cos(a), mult * sin(a), v);

    }
    glEnd();

    // glTranslatef(0, 5, 0);
    // glRotatef(45, 1, 0, 0);

     // Cylinder "Cover"
    glBegin(GL_QUAD_STRIP);
    glColor4f(1.0, 1.0, 0.0, 1.0);

    for (int i = arg; i < 440; i += (360 / n)) {
        float a = i * PI / 140; // degrees to radians
        glVertex3f(mult * cos(a), mult * sin(a), 0.0);
        glVertex3f(mult * cos(a), mult * sin(a), v);

    }
    glEnd();

}


void display(GLFWwindow* window)
{
    Shader ourShader("src/board.vs", "src/board.fs");


    while (!glfwWindowShouldClose(window))
    {
        glFlush();
        // Scale to window size
        GLint windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight);


        // Draw stuff
        glClearColor(0.0, 0.8, 0.3, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      //  glMatrixMode(GL_PROJECTION);
       // glLoadIdentity();
        //gluPerspective(60, (double)windowWidth / (double)windowHeight, 0.1, 100);

        glMatrixMode(GL_MODELVIEW);

        //glPopMatrix();
        //Move world
        //glTranslatef(Tx, Ty, Tz);
        //glRotatef(rot, Rx, Ry, Rz);
      
        //glm::mat4 Projection = glm::perspective(60, (double)windowWidth / (double)windowHeight, 0.1, 100);
        
        
        drawCube(ourShader);
       
        /*
        for (GLfloat i = -35; i < 40.0; i += 10.0) {
            for (GLfloat j = -35; j < 40.0; j += 10.0)
            {
                glPushMatrix();
                glTranslatef(i, 1, j);
                glRotatef(-90, 1, 0, 0);
                drawChecker();
                glPopMatrix();
            }
        }
        */
        //Reset
        //glRotatef(-rot, Rx, Ry, Rz);
        //glTranslatef(-Tx, -Ty, -Tz);
        
        // Update Screen
        glfwSwapBuffers(window);

        // Check for any input, or window movement
        glfwPollEvents();
    
    }
}

int main(int argc, char** argv)
{
    
    glfwInit();

    GLFWwindow* window = initWindow(1024, 620);
    glfwSetErrorCallback(error_callback);

    //Enter Main display loop
  
    if (NULL != window)
    {
        display(window);
    }

    //Must call these when terminating GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

//Cube
/*
-1, -1, -1,   -1, -1,  1,   -1,  1,  1,   -1,  1, -1,
 1, -1, -1,    1, -1,  1,    1,  1,  1,    1,  1, -1,
 -1, -1, -1,   -1, -1,  1,    1, -1,  1,    1, -1, -1,
 -1,  1, -1,   -1,  1,  1,    1,  1,  1,    1,  1, -1,
 -1, -1, -1,   -1,  1, -1,    1,  1, -1,    1, -1, -1,
 -1, -1,  1,   -1,  1,  1,    1,  1,  1,    1, -1,  1
 */
