#version 400

uniform sampler2D textures;
uniform float time;
out vec4 fragcolor;           
in vec2 tex_coord;
in vec3 normal;
uniform float L_x = 1.0f;
uniform float factor = 1.0f;

//vec3 light = vec3(L_x,1.0,1.0);

uniform vec3 M_ambient;
uniform vec3 M_diffuse;
uniform vec3 M_specular;
//uniform vec3 ambientColor;


uniform vec3 viewPos;
uniform int change;
uniform float lightPos = 1.0f;

vec3 AL_color = vec3(0.5,0.5,0.5);
vec3 DL_color = vec3(0.5,0.5,0.5);
vec3 SL_color = vec3(0.5,0.5,0.5);

uniform float La = 1.0f;
uniform float Ld = 1.0f;
uniform float Ls = 1.0f;

uniform float shininess = 10.0f;
uniform vec3 cameraPos;

in vec3 FragPos;
in vec3 Normal;
in vec3 nw;

vec3 LightP = vec3(0,0,lightPos);

void main(void)
{   
   vec3 nw = normalize(nw);
   float d = length(LightP-FragPos);
   vec3 textureColor = vec3(texture2D(textures,tex_coord));
   vec3 lw = normalize(LightP - FragPos);
   vec3 vw = normalize(cameraPos - FragPos);
   vec3 rw = reflect(-lw, nw);

   if(change == 1)
   {
     float specularAmount = pow(max(dot(rw, vw),0),shininess);
     vec3 specular = specularAmount * SL_color * textureColor * Ls;
     vec3 diffuse = max(dot(lw , nw),0) * DL_color * textureColor * Ld/pow(d,2);
     fragcolor = vec4( textureColor * AL_color * La + diffuse + specular,1.0);
   }
   else
   {
	   float specularAmount = pow(max(dot(rw, vw),0),shininess);
       vec3 specular = specularAmount * SL_color * M_specular * Ls;
	   vec3 diffuse = max(dot(lw , nw),0) * DL_color * M_diffuse * Ld/pow(d,2);
       fragcolor = vec4( M_ambient * AL_color * La + diffuse + specular,1.0);
   }
}




















