#version 400            
uniform mat4 PVM;

in vec3 pos_attrib;
in vec2 tex_coord_attrib;
in vec3 normal_attrib;
uniform float fact;
out vec2 tex_coord;  
out vec3 normal;
out vec3 FragPos;
out vec3 Normal;
out vec3 nw;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(void)
{
   
   FragPos = vec3(model * vec4(pos_attrib,1.0));
   //Normal = mat3(transpose(inverse(model)))*normal_attrib;   
   nw = mat3(transpose(inverse(model)))*normal_attrib; 
   gl_Position = projection * view *model*vec4(pos_attrib,1.0);
   tex_coord = tex_coord_attrib;
   normal = normal_attrib;

}