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

//visible area on the screen
float n = -1.0f;
float f = -10.0f;
float r = 1.0f;
float t = 1.0f;
float b = -1.0f;
float l = -1.0f;

float scale = 1.0f;

std::vector<vec4> points;
mat4 perspective_matrix;
mat4 camera_matrix;
mat4 final_matrix;

//glm::mat4 mvp;


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
      void main() {color = vec3(1,0,0); }";

      //front



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
       /* -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         0.0f,  1.0f, 0.0f,*/
};

float Rotation =0;
float RotationSpeed = 0.02;

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
  MatrixID = glGetUniformLocation(ProgramID, "mvp");   ///////added


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

/*
  //glm::mat4 ViewMatrix = glm::translate(-3.0f, 0.0f ,0.0f);
  //mat4 ViewMatrix = mat4 MVPmatrix = projection * view * model;(-3.0f, 0.0f ,0.0f);
  mat4 CameraMatrix = glm::lookAt(
       camera, // the position of your camera, in world space
       center,   // where you want to look at, in world space
       up_vector        // probably glm::vec3(0,1,0), but (0,-1,0) would make you looking upside-down, which can be great too
  );

  mat4 projectionMatrix = glm::perspective(
       horizontal_angle,         // The horizontal Field of View, in degrees : the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
       4.0f / 3.0f, // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
       0.1f,        // Near clipping plane. Keep as big as possible, or you'll get precision issues.
       100.0f       // Far clipping plane. Keep as little as possible.
   );

  //mat4 MVPmatrix = projection * view * model;*/



  glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float) width / (float)height, 0.1f, 100.0f);
  glm::mat4 View = glm::lookAt(
                  glm::vec3(4,3,3), // Camera is at (4,3,3), in World Space
                  glm::vec3(0,0,0), // and looks at the origin
                  glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                  );
  glm::mat4 Model = glm::mat4(1.0f);
  glm::mat4 mvp = Projection * View * Model;
  glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

  glUniform1f(RotBindingID, Rotation);
    //draw
     glDrawArrays(GL_TRIANGLES, 0, 45);
     //clean up
     glUseProgram(0);
     glBindVertexArray(0);


}

void OnTimer()
{
  ///Rotation+=RotationSpeed;

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

