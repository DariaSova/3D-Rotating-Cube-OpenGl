#include "Canvas.h"
#include <math.h>
#include "Eigen/Dense"
using namespace Eigen;

unsigned int width = 512;
unsigned int height = 512;

Canvas canvas;

//vertext shader is in chanrge of moving points
//fragment shader - pixel color

const char * vshader_square = "\
#version 330 core \n\
in vec3 vpoint; \
uniform float rotation;\
\
mat4 RotationMatrix(float rot) {\
    mat3 R = mat3(1);\
    R[0][0] = cos(rot);\
    R[0][1] = sin(rot);\
    R[1][0] = -sin(rot);\
    R[1][1] = cos(rot);\
return mat4(R);\
}\
void main() {\
    gl_Position = RotationMatrix(rotation)*vec4(vpoint, 1);\
}";

const char * fshader_square = "\
#version 330 core \n\
out vec3 color;\
void main() {color = vec3(1,0,0); }";

const GLfloat vpoint [] =
{
    -.5f, -.5f, 0,
    0.5f, -.5f, 0,
    -.5f, 0.5f, 0, //upper triangle
    0.5f, -.5f, 0,
    0.5f, 0.5f, 0,
    -.5f, 0.5f, 0,
        0.5f, -.5f, -.5f,
        0.5f, 0.5f, -.5f,
        -.5f, 0.5f, 0 //lower triangle
};

float Rotation =0;
float RotationSpeed = 0.02;

GLuint VertexArrayID = 0;
GLuint ProgramID = 0;
GLuint RotBindingID = 0;

void InitializeGL()
{
    //vertex array
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    //vertex buffer
    GLuint vertexBufferID;
    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vpoint), vpoint, GL_STATIC_DRAW);

    ProgramID = compile_shaders(vshader_square, fshader_square);
    glUseProgram(ProgramID);

    GLuint vpoint_id = glGetAttribLocation(ProgramID, "vpoint");
    glEnableVertexAttribArray(vpoint_id);
   // GlBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glVertexAttribPointer(vpoint_id,3, GL_FLOAT, false, 0,0);

    RotBindingID = glGetUniformLocation(ProgramID, "rotation");

}

void MouseMove(double x, double y)
{

}

void MouseButton(MouseButtons mouseButton, bool press)
{

}

void KeyPress(char keychar)
{

}

void OnPaint()
{
    glClear(GL_COLOR_BUFFER_BIT);
    //contex
    glUseProgram(ProgramID);
    glBindVertexArray(VertexArrayID);


    glUniform1f(RotBindingID, Rotation);
    //draw
    glDrawArrays(GL_TRIANGLES, 0, 6);
    //clean up
    glUseProgram(0);
    glBindVertexArray(0);

}

void OnTimer()
{
    Rotation+=RotationSpeed;

}

int main(int, char **){
    //Link the call backs
    canvas.SetMouseMove(MouseMove);
    canvas.SetMouseButton(MouseButton);
    canvas.SetKeyPress(KeyPress);
    canvas.SetOnPaint(OnPaint);
    canvas.SetTimer(0.05, OnTimer);
    //Show Window
    canvas.Initialize(width, height, "OpenGL Intro Demo");
    //Do our initialization
    InitializeGL();
    canvas.Show();

    return 0;
}

