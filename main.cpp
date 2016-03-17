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

vec3 lightPos(2.0f, 2.0f, 2.0f);
//vec3 Normal(-1,0,0);


//vertext shader is in chanrge of moving points
//fragment shader - pixel color
const char * vshader_square = """\
#version 330 core \n\
      out vec3 Normal;\
      out vec3 FragPos;\
      uniform mat4 mvp;\
      uniform mat4 model;\
      uniform mat4 view;\
      in vec3 vpoint;\
      in vec3 normals;\
      \
      void main() {\
        gl_Position = mvp*vec4(vpoint,1);\
        FragPos = vpoint;\
        Normal = normals;\
      }""";

      const char * fshader_square = "\
#version 330 core \n\
      out vec4 color;\
      in vec3 Normal; \
      in vec3 FragPos;\
      uniform vec3 lightPos;\
      uniform vec3 objectColor;\
      uniform vec3 lightColor;\
      uniform vec3 viewPos;\
      void main() { \
        float ambientStrength = 0.1f;\
        vec3 ambient = ambientStrength * lightColor;\
        vec3 norm = normalize(Normal);\
        vec3 lightDir = normalize(lightPos - FragPos);\
        float diff = max(dot(norm, lightDir), 0.0);\
        vec3 diffuse = diff * lightColor;\
        vec3 eye_dir = normalize(viewPos-FragPos);\
        vec3 H = normalize(lightDir+eye_dir);\
        float specular_intensity = pow(max(dot(norm, H), 0.0), 16);\
        vec3 specular = specular_intensity * lightColor;\
        vec3 result = (ambient+diffuse+specular)*objectColor;\
        color =vec4(result, 1.0f); }";

      const GLfloat vpoint [] =
{

  -0.5f, -0.5f,  0.5f, //front face
  0.5f, -0.5f,  0.5f,
  0.5f,  0.5f,  0.5f,
  0.5f,  0.5f,  0.5f,
  -0.5f,  0.5f,  0.5f,
  -0.5f, -0.5f,  0.5f,

  -0.5f, -0.5f, -0.5f, //back face
  -0.5f,  0.5f, -0.5f,
  0.5f,  0.5f, -0.5f,
  0.5f,  0.5f, -0.5f,
  0.5f, -0.5f, -0.5f,
  -0.5f, -0.5f, -0.5f,


  -0.5f,  0.5f,  0.5f,
  -0.5f,  0.5f, -0.5f,
  -0.5f, -0.5f, -0.5f,
  -0.5f, -0.5f, -0.5f,
  -0.5f, -0.5f,  0.5f,
  -0.5f,  0.5f,  0.5f,

  0.5f,  0.5f,  0.5f,
  0.5f, -0.5f,  0.5f,
  0.5f, -0.5f, -0.5f,
  0.5f, -0.5f, -0.5f,
  0.5f,  0.5f, -0.5f,
  0.5f,  0.5f,  0.5f,

  -0.5f, -0.5f, -0.5f,
  -0.5f, -0.5f,  0.5f,
  0.5f, -0.5f,  0.5f,
  0.5f, -0.5f,  0.5f,
   0.5f, -0.5f, -0.5f,
  -0.5f, -0.5f, -0.5f,


  -0.5f,  0.5f, -0.5f,
  -0.5f,  0.5f,  0.5f,
  0.5f,  0.5f,  0.5f,
  0.5f,  0.5f,  0.5f,
  0.5f,  0.5f, -0.5f,
  -0.5f,  0.5f, -0.5f,

  //second
        0.6f, 0.6f,  0.9f, //front face
        0.9f, 0.6f,  0.9f,
        0.9f,  0.9f,  0.9f,
        0.9f,  0.9f,  0.9f,
        0.6f,  0.9f,  0.9f,
        0.6f, 0.6f,  0.9f,

        0.6f, 0.6f, 0.6f, //back face
        0.6f,  0.9f, 0.6f,
        0.9f,  0.9f, 0.6f,
        0.9f,  0.9f, 0.6f,
        0.9f, 0.6f, 0.6f,
        0.6f, 0.6f, 0.6f,


        0.6f,  0.9f,  0.9f,
        0.6f,  0.9f, 0.6f,
        0.6f, 0.6f, 0.6f,
        0.6f, 0.6f, 0.6f,
        0.6f, 0.6f,  0.9f,
        0.6f,  0.9f,  0.9f,

        0.9f,  0.9f,  0.9f,
        0.9f, 0.6f,  0.9f,
        0.9f, 0.6f, 0.6f,
        0.9f, 0.6f, 0.6f,
        0.9f,  0.9f, 0.6f,
        0.9f,  0.9f,  0.9f,

        0.6f, 0.6f, 0.6f,
        0.6f, 0.6f,  0.9f,
        0.9f, 0.6f,  0.9f,
        0.9f, 0.6f,  0.9f,
         0.9f, 0.6f, 0.6f,
        0.6f, 0.6f, 0.6f,


        0.6f,  0.9f, 0.6f,
        0.6f,  0.9f,  0.9f,
        0.9f,  0.9f,  0.9f,
        0.9f,  0.9f,  0.9f,
        0.9f,  0.9f, 0.6f,
        0.6f,  0.9f, 0.6f,

};


const GLfloat normals [] =
{
    0.0f,  0.0f, 1.0f, //front face
    0.0f,  0.0f, 1.0f,
    0.0f,  0.0f, 1.0f,
    0.0f,  0.0f, 1.0f,
    0.0f,  0.0f, 1.0f,
    0.0f,  0.0f, 1.0f,

  0.0f,  0.0f, -1.0f, //back face
  0.0f,  0.0f, -1.0f,
  0.0f,  0.0f, -1.0f,
  0.0f,  0.0f, -1.0f,
  0.0f,  0.0f, -1.0f,
  0.0f,  0.0f, -1.0f,



  -1.0f,  0.0f,  0.0f,
  -1.0f,  0.0f,  0.0f,
  -1.0f,  0.0f,  0.0f,
  -1.0f,  0.0f,  0.0f,
  -1.0f,  0.0f,  0.0f,
  -1.0f,  0.0f,  0.0f,

  1.0f,  0.0f,  0.0f,
  1.0f,  0.0f,  0.0f,
  1.0f,  0.0f,  0.0f,
  1.0f,  0.0f,  0.0f,
  1.0f,  0.0f,  0.0f,
  1.0f,  0.0f,  0.0f,

  0.0f, -1.0f,  0.0f,
  0.0f, -1.0f,  0.0f,
  0.0f, -1.0f,  0.0f,
  0.0f, -1.0f,  0.0f,
  0.0f, -1.0f,  0.0f,
  0.0f, -1.0f,  0.0f,

  0.0f,  1.0f,  0.0f,
  0.0f,  1.0f,  0.0f,
  0.0f,  1.0f,  0.0f,
  0.0f,  1.0f,  0.0f,
  0.0f,  1.0f,  0.0f,
  0.0f,  1.0f,  0.0f,

    0.0f,  0.0f, 1.0f, //front face
    0.0f,  0.0f, 1.0f,
    0.0f,  0.0f, 1.0f,
    0.0f,  0.0f, 1.0f,
    0.0f,  0.0f, 1.0f,
    0.0f,  0.0f, 1.0f,

  0.0f,  0.0f, -1.0f, //back face
  0.0f,  0.0f, -1.0f,
  0.0f,  0.0f, -1.0f,
  0.0f,  0.0f, -1.0f,
  0.0f,  0.0f, -1.0f,
  0.0f,  0.0f, -1.0f,



  -1.0f,  0.0f,  0.0f,
  -1.0f,  0.0f,  0.0f,
  -1.0f,  0.0f,  0.0f,
  -1.0f,  0.0f,  0.0f,
  -1.0f,  0.0f,  0.0f,
  -1.0f,  0.0f,  0.0f,

  1.0f,  0.0f,  0.0f,
  1.0f,  0.0f,  0.0f,
  1.0f,  0.0f,  0.0f,
  1.0f,  0.0f,  0.0f,
  1.0f,  0.0f,  0.0f,
  1.0f,  0.0f,  0.0f,

  0.0f, -1.0f,  0.0f,
  0.0f, -1.0f,  0.0f,
  0.0f, -1.0f,  0.0f,
  0.0f, -1.0f,  0.0f,
  0.0f, -1.0f,  0.0f,
  0.0f, -1.0f,  0.0f,

  0.0f,  1.0f,  0.0f,
  0.0f,  1.0f,  0.0f,
  0.0f,  1.0f,  0.0f,
  0.0f,  1.0f,  0.0f,
  0.0f,  1.0f,  0.0f,
  0.0f,  1.0f,  0.0f,

};

GLuint VertexArrayID = 0;
GLuint ProgramID = 0;
GLuint RotBindingID = 0;
GLuint MatrixID = 0;
GLint modelLoc = 0;
GLint viewLoc  = 0;

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

  modelLoc = glGetUniformLocation(ProgramID, "model");
  viewLoc  = glGetUniformLocation(ProgramID,  "view");

  //normals buffer
  GLuint normalbuffer;
  glGenBuffers(1, &normalbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
  GLuint normals_id = glGetAttribLocation(ProgramID, "normals");
  fprintf(stderr, "normals_id=%d", normals_id);
  glEnableVertexAttribArray(normals_id);
  glVertexAttribPointer(normals_id,3, GL_FLOAT, false, 0,0);


  //lightning
  //GLuint lightVAO;
  //glGenVertexArrays(1, &lightVAO);
  //glBindVertexArray(lightVAO);
  // We only need to bind to the VBO, the container's VBO's data already contains the correct data.
  //glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);  /////
  // Set the vertex attributes (only position data for our lamp)
  //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
  //glEnableVertexAttribArray(0);
  //glBindVertexArray(0);
  GLint objectColorLoc = glGetUniformLocation(ProgramID, "objectColor");
  GLint lightColorLoc  = glGetUniformLocation(ProgramID, "lightColor");
  GLint viewPosLoc = glGetUniformLocation(ProgramID, "viewPos");
  glUniform3f(viewPosLoc, 1.5f, -0.5f, -0.9f);
  //glUniform3f(viewPosLoc, camera.x, camera.y, camera.z);
  glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
  glUniform3f(lightColorLoc,  1.0f, 1.0f, 1.0f); // Also set light's color (white)
  GLint lightPosLoc = glGetUniformLocation(ProgramID, "lightPos");
  glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);

 glEnable(GL_DEPTH_TEST);

  // 3rd attribute buffer : normals
  //glEnableVertexAttribArray(2);
  //glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
  /*glVertexAttribPointer(
      2,                                // attribute
      3,                                // size
      GL_FLOAT,                         // type
      GL_FALSE,                         // normalized?
      0,                                // stride
      (void*)0                          // array buffer offset
      );*/

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
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &Model[0][0]);
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &View[0][0]);

  //draw
  glDrawArrays(GL_TRIANGLES, 0, 72);


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



