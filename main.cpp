#include "Canvas.h"
#include <math.h>
#include <glm/glm.hpp>
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


//vertext shader is in chanrge of moving points
//fragment shader - pixel color

/*Vector4f p = final_matrix*points[i];
Vector4f q = final_matrix*points[i+1];
canvas.AddLine(p[0]/p[2], p[1]/p[2], q[0]/q[2], q[1]/q[2]);
gl_Position = final_matrix*vec4(vpoint, 1); */

const char * vshader_square = """\
#version 330 core \n\
      in vec3 vpoint; \
      in mat4 final_matrix;\
      \
      void main() {\
        vec4 temp =final_matrix*vec4(vpoint, 1);\
        gl_Position = vec4(temp[0]/temp[2], temp[1]/temp[2], 0, 1);\
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


  //compute perspective_matrixspective=Morth*perspective_matrix
  /*perspective_matrix <<
    2*n/(r-l), 0, (l+r)/(l-r), 0,
    0, 2*n/(t-b), (b+t)/(b-t), 0,
    0, 0, (n+f)/(n-f), 2*f*n/(f-n),
    0, 0, 1, 0;*/

  perspective_matrix[0][0] = 2*n/(r-l);
  perspective_matrix[0][1] = 0;
  perspective_matrix[0][2] = (l+r)/(l-r);
  perspective_matrix[0][3] = 0;
  perspective_matrix[1][0] = 0;
  perspective_matrix[1][1] = 2*n/(t-b);
  perspective_matrix[1][2] = (b+t)/(b-t);
  perspective_matrix[1][3] = 0;
  perspective_matrix[2][0] = 0;
  perspective_matrix[2][1] = 0;
  perspective_matrix[2][2] = (n+f)/(n-f);
  perspective_matrix[2][3] = 2*f*n/(f-n);
  perspective_matrix[3][0] = 0;
  perspective_matrix[3][1] = 0;
  perspective_matrix[3][2] = 1;
  perspective_matrix[3][3] = 0;

  //compute Mcamera
  vec3 new_camera (
      radius*sin(horizontal_angle)*cos(vertical_angle),
      radius*sin(horizontal_angle)*sin(vertical_angle),
      radius*cos(horizontal_angle));
  camera = new_camera;

  vec3 gaze_dir= center - camera; //center-eye_pos

  //update radius
  radius = length(gaze_dir);
  vec3 w = -(gaze_dir/length(gaze_dir));
  vec3 u = cross(up_vector, w)/length(cross(up_vector, w));
  vec3 v = cross(w, u);

  mat4 MV;
 /* MV <<
    u[0], u[1], u[2], 0,
    v[0], v[1], v[2], 0,
    w[0], w[1], w[2], 0,
    0, 0, 0, 1;*/

  MV[0][0] = u[0];
  MV[0][1] = u[1];
  MV[0][2] = u[2];
  MV[0][3] = 0;
  MV[1][0] = v[0];
  MV[1][1] = v[1];
  MV[1][2] = v[2];
  MV[1][3] = 0;
  MV[2][0] = w[0];
  MV[2][1] = w[1];
  MV[2][2] = w[2];
  MV[2][3] = 0;
  MV[3][0] = 0;
  MV[3][1] = 0;
  MV[3][2] = 0;
  MV[3][3] = 1;

  mat4 temp;
 /* temp <<
    1, 0, 0, -camera[0],
    0, 1, 0, -camera[1],
    0, 0, 1, -camera[2],
    0, 0, 0, 1;*/

  temp[0][0] = 1;
  temp[0][1] = 0;
  temp[0][2] = 0;
  temp[0][3] = -camera[0];
  temp[1][0] = 0;
  temp[1][1] = 1;
  temp[1][2] = 0;
  temp[1][3] = -camera[1];
  temp[2][0] = 0;
  temp[2][1] = 0;
  temp[2][2] = 1;
  temp[2][3] = -camera[2];
  temp[3][0] = 0;
  temp[3][1] = 0;
  temp[3][2] = 0;
  temp[3][3] = 1;

    camera_matrix = MV*temp;
    final_matrix= perspective_matrix*camera_matrix;




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

