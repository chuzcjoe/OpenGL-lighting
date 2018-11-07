#include <windows.h>
#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glext.h>

#include <glm/glm.hpp> //This header defines basic glm types (vec3, mat4, etc)

//These headers define matrix transformations
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>                                                                                 

#include <glm/gtc/type_ptr.hpp> //This header defines helper functions (e.g. value_ptr(...)) for passing vectors and matrices to shaders

#include "InitShader.h"    //Functions for loading shaders from text files
#include "LoadMesh.h"      //Functions for creating OpenGL buffers from mesh files
#include "LoadTexture.h"   //Functions for creating OpenGL textures from image files
#include "imgui_impl_glut.h" //Functions for using imgui UI in glut programs

static const std::string vertex_shader("gui_demo_vs.glsl");
static const std::string fragment_shader("gui_demo_fs.glsl");

GLuint shader_program = -1;
GLuint texture_id = -1; //Texture map for fish
GLuint texture_id2 = -1;

static const std::string mesh_name = "earth.obj";
static const std::string texture_name = "earth.jpg";

static const std::string mesh_name2 = "Amago0.obj";
static const std::string texture_name2 = "AmagoT.bmp";


MeshData mesh_data;
MeshData mesh_data2;



                                                                                                                                                                                                                                                                          
bool clear = true;
bool depth = true;
float angle = 0.0f;
float scale = 2.0f;
glm::vec3 pos = glm::vec3(0.0f,0.0f,0.0f) ;
glm::vec3 camera_pos = glm::vec3(0.0f, 0.0f, 0.0f);
float fov = 90.0f;
float near_clip = 1.0f;
float far_clip = 20.0f;
float fact = 1.0f;
float L_x = 1.0f;
bool order = true;

float La = 1.0f;
float Ld = 1.0f;
float Ls = 1.0f;
float shininess = 10.0f;

float Light_y = 1.0f;

//glm::mat4 P = glm::perspective(fov,1.0f,near_clip,far_clip);

float factor = 1.0f;
//Draw the ImGui user interface
void draw_gui()
{
   ImGui_ImplGlut_NewFrame();

   ImGui::Begin("Debug window");

   ImGui::End();

   ImGui::Begin("Zongcheng Chu");
   //uncomment the following line to create a slider which changes the viewing angle
   ImGui::SliderFloat("View angle", &angle, -180.0f, +180.0f);
   ImGui::SliderFloat("Scale", &scale, -2.0f, 2.0f);
   ImGui::SliderFloat("FOV", &fov, 0.0f, 150.0f);
   ImGui::SliderFloat("change mesh 1",&fact,0.0f,20.0f);
   
   ImGui::SliderFloat("Near_clip", &near_clip, 0.0f, 10.0f);
   ImGui::SliderFloat("Far_clip", &far_clip, 0.0f, 20.0f);
   ImGui::SliderFloat3("value of pos",(float*)&pos,-1.0f,1.0f);
   ImGui::SliderFloat3("camera pos", (float*)&camera_pos, -1.0f, 1.0f);
   ImGui::Checkbox("clear the screen",&clear);
   ImGui::Checkbox("change the orders",&order);


   //glm:mat4 T = glm::translate();
   

   static bool depth = true;
   ImGui::Checkbox("depth testing",&depth);
   if (depth)
   {
	   glEnable(GL_DEPTH_TEST);
   }
   else
   {
	   glDisable(GL_DEPTH_TEST);
   }

   static float color[] = {0.0,0.0,0.0,0.0};
   if (ImGui::ColorEdit4("color change", color))
   {
	   //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	   glClearColor(color[0],color[1],color[2],color[3]);
   }

   if (ImGui::Button("reset angle to zero"))
   {
	   angle = 0;
   }

   if (ImGui::SliderFloat("darken\lighten", &factor, 0.0f, 2.0f))
   {
	   int factor_loc = glGetUniformLocation(shader_program, "factor");
	   glUniform1f(factor_loc, factor);
   }

   if (ImGui::SliderFloat("change mesh 2", &L_x, -50.0f, 50.0f))
   {
	   int L_loc = glGetUniformLocation(shader_program, "L_x");
	   glUniform1f(L_loc, L_x);
   }

   //int slider_loc = glGetUniformLocation(shader_program,"slider");
   //static float slider[5] = {0.0,f, 0.0f, 0.0f ,0.0f,0.0f};
   
   // for (int i =0;i<5;i++)
   //{//std::string name = "slider";
   // name = name + std::to_string(i);
   //if (ImGui::SliderFloat(name, &slider[i], 0.0f, 0.0f))
   //{glUniform1fv(slider_loc,5,slider)}
   //}

   ImGui::SliderFloat("lightPos", &Light_y,0.5f,5.0f);
   glUniform1f(glGetUniformLocation(shader_program, "lightPos"), Light_y);

 
   static bool material = false;
   ImGui::Checkbox("enable texture color", &material);

   int change = 1;
   if (material)
   {
	   glUniform1i(glGetUniformLocation(shader_program,"change"),1);
   }
   else
   {
	   glUniform1i(glGetUniformLocation(shader_program, "change"), 0);
   }

   static float A_color[] = { 0.8f,0.3f,0.5f };
   ImGui::ColorEdit3("material ambient color", A_color);
   glUniform3f(glGetUniformLocation(shader_program,"M_ambient"),A_color[0],A_color[1],A_color[2]);

   static float D_color[] = { 0.8f,0.3f,0.5f };
   ImGui::ColorEdit3("material diffuse color", D_color);
   glUniform3f(glGetUniformLocation(shader_program, "M_diffuse"), D_color[0], D_color[1], D_color[2]);

   static float S_color[] = { 0.8f,0.3f,0.5f };
   ImGui::ColorEdit3("material specular color", S_color);
   glUniform3f(glGetUniformLocation(shader_program, "M_specular"), S_color[0], S_color[1], S_color[2]);

   //static float AL_color[] = { 0.8f,0.3f,0.5f };
   //ImGui::ColorEdit3(" ambient light color", AL_color);
   //glUniform3f(glGetUniformLocation(shader_program, "ambientColor"), AL_color[0], AL_color[1], AL_color[2]);

   if (ImGui::SliderFloat("La", &La, 0.0f, 1.0f))
   {
	   glUniform1f(glGetUniformLocation(shader_program, "La"), La);
   }

   if (ImGui::SliderFloat("Ld", &Ld, 0.0f, 1.0f))
   {
	   glUniform1f(glGetUniformLocation(shader_program, "Ld"), Ld);
   }

   if (ImGui::SliderFloat("Ls", &Ls, 0.0f, 1.0f))
   {
	   glUniform1f(glGetUniformLocation(shader_program, "Ls"), Ls);
   }
   

   if (ImGui::SliderFloat("shininess", &shininess, 0.0f, 100.0f))
   {
	   glUniform1f(glGetUniformLocation(shader_program, "shininess"), shininess);
   }

   //static float D_color[] = { 0.0,0.0,0.0 };
   //ImGui::ColorEdit3("Diffuse color", D_color);
   //glUniform3f(glGetUniformLocation(shader_program, "objColor"), A_color[0], A_color[1], A_color[2]);

   //static float S_color[] = { 0.0,0.0,0.0 };
   //ImGui::ColorEdit3("Specular color", S_color);
   //glUniform3f(glGetUniformLocation(shader_program, "objColor"), A_color[0], A_color[1], A_color[2]);

   ImGui::End();

   static bool show_test = false;
   ImGui::ShowTestWindow(&show_test);

   ImGui::Render();
 }

// glut display callback function.
// This function gets called every time the scene gets redisplayed 
void display()
{
	//glm::mat4 T = glm::translate(pos);
	glm::mat4 camera = glm::translate(-camera_pos);

	if (clear) {
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); //Clear the back buffer
	}
   
   //glm::scale(scale*glm::vec3(mesh_data.mScaleFactor))
   //glm::mat4 M = glm::rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f))*glm::scale(glm::vec3(mesh_data.mScaleFactor));
   //glm::mat4 V = glm::lookAt(glm::vec3(0.0f, 1.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
   //glm::mat4 P = glm::perspective(40.0f, 1.0f, 0.1f, 100.0f);
	

	glm::mat4 R = glm::rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 T = glm::translate(pos);
	glm::mat4 S = glm::scale(scale*glm::vec3(mesh_data.mScaleFactor));
	glm::mat4 M = T*R*S;
	if (order)
	{
		M = R*T*S;
	}
   //glm::mat4 M = T* glm::rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f))*glm::scale(scale*glm::vec3(mesh_data.mScaleFactor));
   //glm::mat4 M =  glm::rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f))*T*glm::scale(scale*glm::vec3(mesh_data.mScaleFactor));

	float radius = 3.0f;
	float camX = sin(glutGet(GLUT_ELAPSED_TIME)*0.001) * radius;
	float camZ = cos(glutGet(GLUT_ELAPSED_TIME)*0.001) * radius;

   glm::mat4 V = camera  * glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
   
   glm::mat4 P = glm::perspective(fov, 1.0f, near_clip, far_clip);


   glUniformMatrix4fv(glGetUniformLocation(shader_program,"model"), 1, false, glm::value_ptr(M));
   glUniformMatrix4fv(glGetUniformLocation(shader_program, "view"), 1, false, glm::value_ptr(V));
   glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, false, glm::value_ptr(P));

   glUniform3f(glGetUniformLocation(shader_program,"cameraPos"), 0.0f, 1.0f, 3.0f);


   glUniform3f(glGetUniformLocation(shader_program, "dirction"),-0.2f,-0.1f,-0.3f );





   glUseProgram(shader_program);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, texture_id);

  
   
   int fact_loc = glGetUniformLocation(shader_program, "fact");
   glUniform1f(fact_loc, fact);


   int PVM_loc = glGetUniformLocation(shader_program, "PVM");
   if (PVM_loc != -1)
   {
      glm::mat4 PVM = P*V*M;
      glUniformMatrix4fv(PVM_loc, 1, false, glm::value_ptr(PVM));
   }

   int tex_loc = glGetUniformLocation(shader_program, "textures");
   if (tex_loc != -1)
   {
      glUniform1i(tex_loc, 0); // we bound our texture to texture unit 0
   }

   glBindVertexArray(mesh_data.mVao);
   //glDrawElements(GL_TRIANGLES, mesh_data.mSubmesh[0].mNumIndices, GL_UNSIGNED_INT, 0);
   mesh_data.DrawMesh(); 


   glBindVertexArray(0);

   R = glm::rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f));
   T = glm::translate(glm::vec3(0, 0, 2));
   S = glm::scale(scale*glm::vec3(mesh_data.mScaleFactor));
   M = T * R*S;
   if (order)
   {
	   M = R * T*S;
   }
   //glm::mat4 M = T* glm::rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f))*glm::scale(scale*glm::vec3(mesh_data.mScaleFactor));
   //glm::mat4 M =  glm::rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f))*T*glm::scale(scale*glm::vec3(mesh_data.mScaleFactor));

   radius = 3.0f;
   camX = sin(glutGet(GLUT_ELAPSED_TIME)*0.001) * radius;
   camZ = cos(glutGet(GLUT_ELAPSED_TIME)*0.001) * radius;

   V = camera * glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
   P = glm::perspective(fov, 1.0f, near_clip, far_clip);


   glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, false, glm::value_ptr(M));
   glUniformMatrix4fv(glGetUniformLocation(shader_program, "view"), 1, false, glm::value_ptr(V));
   glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, false, glm::value_ptr(P));

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, texture_id2);

   glBindVertexArray(mesh_data2.mVao);
   //glDrawElements(GL_TRIANGLES, mesh_data2.mSubmesh[0].mNumIndices, GL_UNSIGNED_INT, 0);
   mesh_data2.DrawMesh(); 


   draw_gui();
   


   glutSwapBuffers();
}

//void reshape(int w, int h)
//{
//    P = glm::perspective(90.0f, float(w/h), 0.1f, 100.0f);
	//glViewport(0,0,w,h);
//}

void idle()
{
	glutPostRedisplay();

   const int time_ms = glutGet(GLUT_ELAPSED_TIME);
   float time_sec = 0.001f*time_ms;

   //Pass time_sec value to the shaders
   int time_loc = glGetUniformLocation(shader_program, "time");
   if (time_loc != -1)
   {
      glUniform1f(time_loc, time_sec);
   }
}

void  reload_shader()
{
   GLuint new_shader = InitShader(vertex_shader.c_str(), fragment_shader.c_str());

   if (new_shader == -1) // loading failed
   {
      glClearColor(1.0f, 0.0f, 1.0f, 0.0f); //change clear color if shader can't be compiled
   }
   else
   {
      glClearColor(0.35f, 0.35f, 0.35f, 0.0f);

      if (shader_program != -1)
      {
         glDeleteProgram(shader_program);
      }
      shader_program = new_shader;
   }
}

void printGlInfo()
{
   std::cout << "Vendor: "       << glGetString(GL_VENDOR)                    << std::endl;
   std::cout << "Renderer: "     << glGetString(GL_RENDERER)                  << std::endl;
   std::cout << "Version: "      << glGetString(GL_VERSION)                   << std::endl;
   std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION)  << std::endl;
}

void initOpenGl()
{
   glewInit();

   glEnable(GL_DEPTH_TEST);

   reload_shader();

   //mesh and texture to be rendered
   mesh_data = LoadMesh(mesh_name2);
   texture_id = LoadTexture(texture_name);

   mesh_data2 = LoadMesh(mesh_name2);
   texture_id2 = LoadTexture(texture_name2);

}

// glut callbacks need to send keyboard and mouse events to imgui
void keyboard(unsigned char key, int x, int y)
{
   ImGui_ImplGlut_KeyCallback(key);

   switch(key)
   {
      case 'r':
      case 'R':
         reload_shader();     
      break;
   }
}

void keyboard_up(unsigned char key, int x, int y)
{
   ImGui_ImplGlut_KeyUpCallback(key);
}

void special_up(int key, int x, int y)
{
   ImGui_ImplGlut_SpecialUpCallback(key);
}

void passive(int x, int y)
{
   ImGui_ImplGlut_PassiveMouseMotionCallback(x,y);
}

void special(int key, int x, int y)
{
   ImGui_ImplGlut_SpecialCallback(key);
}

void motion(int x, int y)
{
   ImGui_ImplGlut_MouseMotionCallback(x, y);
}

void mouse(int button, int state, int x, int y)
{
   ImGui_ImplGlut_MouseButtonCallback(button, state);
}


int main (int argc, char **argv)
{
   //Configure initial window state
   glutInit(&argc, argv); 
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
   glutInitWindowPosition (5, 5);
   glutInitWindowSize (640, 640);
   int win = glutCreateWindow ("GUI demo");

   printGlInfo();

   //Register callback functions with glut. 
   glutDisplayFunc(display); 
   glutKeyboardFunc(keyboard);
   glutSpecialFunc(special);
   glutKeyboardUpFunc(keyboard_up);
   glutSpecialUpFunc(special_up);
   glutMouseFunc(mouse);
   glutMotionFunc(motion);
   glutPassiveMotionFunc(motion);
   glutIdleFunc(idle);
   //glutReshapeFunc(reshape);

   initOpenGl();
   ImGui_ImplGlut_Init(); // initialize the imgui system

   //Enter the glut event loop.
   glutMainLoop();
   glutDestroyWindow(win);
   return 0;		
}