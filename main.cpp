#include "Canvas.h"
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

unsigned int width = 512;
unsigned int height = 512;
bool leftButtonPressed = false;
bool rightButtonPressed = false;
float mouse_pos_x = 0;
float mouse_pos_y = 0;
float mouse_speed = 0.008;

float radius =5;
// horizontal angle : toward -Z
float horizontal_angle = 0.0f;
// vertical angle : 0, look at the horizon
float vertical_angle = 0.0f;

Canvas canvas;
vec3 camera;
vec3 center(0, 0, 0);
vec3 up_vector(0, 1, 0);

vec3 lightPos(1.2f, 1.0f, 2.0f);


//vertext shader is in chanrge of moving points
//fragment shader - pixel color
const char * vshader_square = """\
#version 330 core \n\
      uniform mat4 mvp;\
      in vec3 vpoint; \
      \
      void main() {\
        gl_Position =  mvp*vec4(vpoint,1);\
      }""";

      const char * fshader_square = "\
#version 330 core \n\
      out vec3 color;\
      uniform vec3 objectColor;\
      uniform vec3 lightColor;\
      void main() { float ambientStrength = 0.1f;\
        vec3 ambient = ambientStrength * lightColor;\
        color = ambient*objectColor; }";

      const GLfloat vpoint [] =
{
  -0.5,-0.5,-0.5, // triangle 1 : begin
  -0.5,-0.5, 0.5,
  -0.5, 0.5, 0.5, // triangle 1 : end
  0.5, 0.5,-0.5, // triangle 2 : begin
  -0.5,-0.5,-0.5,
  -0.5, 0.5,-0.5, // triangle 2 : end
  0.5,-0.5, 0.5,
  -0.5,-0.5,-0.5,
  0.5,-0.5,-0.5,
  0.5, 0.5,-0.5,
  0.5,-0.5,-0.5,
  -0.5,-0.5,-0.5,
  -0.5,-0.5,-0.5,
  -0.5, 0.5, 0.5,
  -0.5, 0.5,-0.5,
  0.5,-0.5, 0.5,
  -0.5,-0.5, 0.5,
  -0.5,-0.5,-0.5,
  -0.5, 0.5, 0.5,
  -0.5,-0.5, 0.5,
  0.5,-0.5, 0.5,
  0.5, 0.5, 0.5,
  0.5,-0.5,-0.5,
  0.5, 0.5,-0.5,
  0.5,-0.5,-0.5,
  0.5, 0.5, 0.5,
  0.5,-0.5, 0.5,
  0.5, 0.5, 0.5,
  0.5, 0.5,-0.5,
  -0.5, 0.5,-0.5,
  0.5, 0.5, 0.5,
  -0.5, 0.5,-0.5,
  -0.5, 0.5, 0.5,
  0.5, 0.5, 0.5,
  -0.5, 0.5, 0.5,
  0.5,-0.5, 0.5
};

GLuint VertexArrayID = 0;
GLuint ProgramID = 0;
GLuint RotBindingID = 0;
GLuint MatrixID = 0;

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
  //GlBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
  glVertexAttribPointer(vpoint_id,3, GL_FLOAT, false, 0,0);
  MatrixID = glGetUniformLocation(ProgramID, "mvp");
  RotBindingID = glGetUniformLocation(ProgramID, "rotation");

  GLuint lightVAO;
  glGenVertexArrays(1, &lightVAO);
  glBindVertexArray(lightVAO);
  // We only need to bind to the VBO, the container's VBO's data already contains the correct data.
  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);  /////
  // Set the vertex attributes (only position data for our lamp)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);
  GLint objectColorLoc = glGetUniformLocation(ProgramID, "objectColor");
  GLint lightColorLoc  = glGetUniformLocation(ProgramID, "lightColor");
  glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
  glUniform3f(lightColorLoc,  1.0f, 1.0f, 1.0f); // Also set light's color (white)

}

void MouseMove(double x, double y)
{
    if (leftButtonPressed == true) {
      //check in which direction the mouse is moving
      float x_diff = x-mouse_pos_x;
      float y_diff = y-mouse_pos_y;

      horizontal_angle += mouse_speed * x_diff;
      vertical_angle += mouse_speed * y_diff;
    }
    else if (rightButtonPressed == true) {
      float y_diff = y-mouse_pos_y;
      radius += y_diff*mouse_speed;
    }

    //update current mouse position
    mouse_pos_x = x;
    mouse_pos_y = y;

}

void MouseButton(MouseButtons mouseButton, bool press)
{
    //check for the left button
    if (mouseButton == LeftButton)
    {
      if (press == true) leftButtonPressed = true;
      else leftButtonPressed = false;
    }
    //check for right button
    if (mouseButton == RightButton)
    {
      if (press == true) rightButtonPressed = true;
      else rightButtonPressed = false;
    }

}

void OnPaint()
{
  glClear(GL_COLOR_BUFFER_BIT);
  //contex
  glUseProgram(ProgramID);
  glBindVertexArray(VertexArrayID);

  //compute Mcamera
  vec3 new_camera (
      radius*sin(horizontal_angle)*cos(vertical_angle),
      radius*sin(horizontal_angle)*sin(vertical_angle),
      radius*cos(horizontal_angle));
  camera = new_camera;

  mat4 Projection = perspective(glm::radians(45.0f), (float) width / (float)height, 0.1f, 100.0f);
  mat4 View = lookAt(
      camera, // Camera is at (4,3,3), in World Space
      center, // and looks at the origin
      up_vector  // Head is up (set to 0,-1,0 to look upside-down)
      );
  mat4 Model = mat4(1.0f);
  mat4 mvp = Projection * View * Model;
  glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

  //draw
  glDrawArrays(GL_TRIANGLES, 0, 45);
  //clean up
  glUseProgram(0);
  glBindVertexArray(0);
}


int main(int, char **){

  //Link the call backs
  canvas.SetMouseMove(MouseMove);
  canvas.SetMouseButton(MouseButton);
  canvas.SetOnPaint(OnPaint);
  //Show Window
  canvas.Initialize(width, height, "OpenGL Intro Demo");
  //Do our initialization
  InitializeGL();
  canvas.Show();

  return 0;
}



