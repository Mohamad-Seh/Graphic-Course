#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;


// The model/view/projection matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int mapping_type;
uniform vec4 center_cor;
uniform int max_z;
uniform vec3 camera_pos;
uniform sampler2D texture_cordinates;

// These outputs will be available in the fragment shader as inputs
out vec3 color;
out vec2 frag_tex_coords;
out vec3 fragPos;
out vec3 fragNormal;
out vec3 reflected_vector;
//------consts----------
const float PI = 3.14159;
void main()
{
	gl_Position = projection * view *  model * vec4(pos, 1.0f);
	fragPos = vec3(model * vec4(pos, 1.0f));
	fragNormal = mat3(model) * normal;
	if(mapping_type==0)//planer
	{
		frag_tex_coords.x = pos.x;
		frag_tex_coords.y = pos.y;
	}
	else if(mapping_type==1)//Cylindrical
	{
		float theta=atan(pos.z,pos.x) +PI;
		float h=pos.y;
		frag_tex_coords=vec2(theta/2*PI,h/max_z);
	}
	else if(mapping_type==2)//Spherical
	{
		vec3 n=pos-vec3(center_cor);
		n=normalize(n);
		frag_tex_coords.x=0.5+atan(n.x,n.z)/(2*PI);
		frag_tex_coords.y=0.5-asin(n.y)/(PI);
	}
	else
	{
	    frag_tex_coords=texCoords;
	}
	vec3 view_vector=normalize(gl_Position.xyz-camera_pos);
	vec3 uint_normal=normalize(fragNormal);
	reflected_vector=reflect(view_vector,uint_normal);
	
}