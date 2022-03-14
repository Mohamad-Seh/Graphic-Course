#include "MeshModel.h"
#include"Renderer.h"
#include <iostream>
#include <random>
#include <glm/gtc/matrix_transform.hpp>
MeshModel::MeshModel::MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<glm::vec2> textureCoords, const std::string& modelName) :
	faces(faces),
	vertices(vertices),
	normals(normals),
	model_tranform(1),
	world_tranform(1),
	model_name(model_name)
{
	int x_cen=0, y_cen=0, z_cen=0;
	modelVertices.reserve(3 * faces.size());
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<double> dist(0, 1);
	color = glm::vec3(dist(mt), dist(mt), dist(mt));
	float min_x = vertices[0].x;
	float min_y = vertices[0].y;
	float min_z = vertices[0].z;
	float max_x = vertices[0].x;
	float max_y = vertices[0].y;
	float max_z = vertices[0].z;
	for (int i = 0; i < faces.size(); i++)
	{
		Face currentFace = faces.at(i);
		for (int j = 0; j < 3; j++)
		{
			int vertexIndex = currentFace.GetVertexIndex(j) - 1;
			Vertex vertex;
			vertex.position = vertices[vertexIndex];
			vertex.normal = normals[vertexIndex];
			vertex.color = color;
			if (textureCoords.size() > 0)
			{
				int textureCoordsIndex = currentFace.GetTextureIndex(j) - 1;
				vertex.textureCoords = textureCoords[textureCoordsIndex];
			}
			
			modelVertices.push_back(vertex);
		}
	}
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, modelVertices.size() * sizeof(Vertex), &modelVertices[0], GL_STATIC_DRAW);

	// Vertex Positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Normals attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Vertex Texture Coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// unbind to make sure other code does not change it somewhere else
	glBindVertexArray(0);
	//----scaling matrix local------
	scaling_mat_loc[0][0] = 1;
	scaling_mat_loc[1][1] = 1;
	scaling_mat_loc[2][2] = 1;
	scaling_mat_loc[3][3] = 1;
	//-------translation matrix local----
	translate_mat_loc[0][0] = 1;
	translate_mat_loc[1][1] = 1;
	translate_mat_loc[2][2] = 1;
	translate_mat_loc[3][3] = 1;
	translate_mat_loc[3][0] = 0;
	translate_mat_loc[3][1] = 0;
	translate_mat_loc[3][2] = 0;
	//------rotation matrix local-------
	rotation_mat_locX[0][0] = 1;
	rotation_mat_locX[1][1] = cos(0);
	rotation_mat_locX[2][1] = -sin(0);
	rotation_mat_locX[1][2] = sin(0);
	rotation_mat_locX[2][2] = cos(0);
	rotation_mat_locX[3][3] = 1;
	//___________________________
	rotation_mat_locY[1][1] = 1;
	rotation_mat_locY[0][0] = cos(0);
	rotation_mat_locY[2][0] = -sin(0);
	rotation_mat_locY[0][2] = sin(0);
	rotation_mat_locY[2][2] = cos(0);
	rotation_mat_locY[3][3] = 1;
	//__________________________
	rotation_mat_locZ[3][3] = 1;
	rotation_mat_locZ[0][0] = cos(0);
	rotation_mat_locZ[1][0] = -sin(0);
	rotation_mat_locZ[0][1] = sin(0);
	rotation_mat_locZ[1][1] = cos(0);
	rotation_mat_locZ[3][3] = 1;
	//----scaling matrix world------
	scaling_mat_wor[0][0] = 1;
	scaling_mat_wor[1][1] = 1;
	scaling_mat_wor[2][2] = 1;
	scaling_mat_wor[3][3] = 1;
	////-------translation matrix world----
	translate_mat_wor[0][0] = 1;
	translate_mat_wor[1][1] = 1;
	translate_mat_wor[2][2] = 1;
	translate_mat_wor[3][3] = 1;
	translate_mat_wor[3][0] = 0;
	translate_mat_wor[3][1] = 0;
	translate_mat_wor[3][2] = 0;
	//------rotation matrix world-------
	rotation_mat_worX[0][0] = 1;
	rotation_mat_worX[1][1] = cos(0);
	rotation_mat_worX[2][1] = -sin(0);
	rotation_mat_worX[1][2] = sin(0);
	rotation_mat_worX[2][2] = cos(0);
	rotation_mat_worX[3][3] = 1;
	//----------------------------------
	rotation_mat_worY[1][1] = 1;
	rotation_mat_worY[0][0] = cos(0);
	rotation_mat_worY[2][0] = -sin(0);
	rotation_mat_worY[0][2] = sin(0);
	rotation_mat_worY[2][2] = cos(0);
	rotation_mat_worY[3][3] = 1;
	//---------------------------------
	rotation_mat_worZ[3][3] = 1;
	rotation_mat_worZ[0][0] = cos(0);
	rotation_mat_worZ[1][0] = -sin(0);
	rotation_mat_worZ[0][1] = sin(0);
	rotation_mat_worZ[1][1] = cos(0);
	rotation_mat_worZ[3][3] = 1;
	for (int i = 1; i < vertices.size(); i++)
	{
		if (vertices[i].x > max_x)
		{
			max_x = vertices[i].x;
		}
		if (vertices[i].x < min_x)
		{
			min_x = vertices[i].x;
		}
		if (vertices[i].y > max_y)
		{
			max_y = vertices[i].y;
		}
		if (vertices[i].y < min_y)
		{
			min_y = vertices[i].y;
		}
		if (vertices[i].z > max_z)
		{
			max_z = vertices[i].z;
		}
		if (vertices[i].z < min_z)
		{
			min_z = vertices[i].z;
		}
	}
	this->max_z = max_z;
	center_cordinate = glm::vec4((max_x + min_x) / 2, (max_y + min_y) / 2, (max_z + min_z) / 2,1);

}

MeshModel::~MeshModel()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}
const Face& MeshModel::GetFace(int index) const
{
	return faces[index];
}
int MeshModel::GetFacesCount() const
{
	return faces.size();
}
const std::string& MeshModel::GetModelName() const
{
	return model_name;
}
const glm::vec3&  MeshModel::GetVertix(int index)const
{
	return vertices[index];
}
GLuint MeshModel::GetVAO() const
{
	return vao;
}
const std::vector<Vertex>& MeshModel::GetModelVertices()
{
	return modelVertices;
}
const glm::vec3& MeshModel::GetColor() const
{
	return color;
}
void MeshModel::SetColor(const glm::vec3& color)
{
	this->color = color;
}
void MeshModel::TranslateModel(const glm::vec3& translationVector)
{
	glm::mat4x4 translationMatrix;
	translationMatrix[0] = glm::vec4(1, 0, 0, 0);
	translationMatrix[1] = glm::vec4(0, 1, 0, 0);
	translationMatrix[2] = glm::vec4(0, 0, 1, 0);
	translationMatrix[3] = glm::vec4(translationVector, 1);
	model_tranform = translationMatrix * model_tranform;
}
void MeshModel::TranslateWorld(const glm::vec3& translationVector)
{
	glm::mat4x4 translationMatrix;
	translationMatrix[0] = glm::vec4(1, 0, 0, 0);
	translationMatrix[1] = glm::vec4(0, 1, 0, 0);
	translationMatrix[2] = glm::vec4(0, 0, 1, 0);
	translationMatrix[3] = glm::vec4(translationVector, 1);
	world_tranform = translationMatrix * world_tranform;
}
void MeshModel::RotateXModel(double angle)
{
	glm::mat4x4 rotationMatrix;
	rotationMatrix[0] = glm::vec4(1, 0, 0, 0);
	rotationMatrix[1] = glm::vec4(0, glm::cos(angle), -glm::sin(angle), 0);
	rotationMatrix[2] = glm::vec4(0, glm::sin(angle), glm::cos(angle), 0);
	rotationMatrix[3] = glm::vec4(0, 0, 0, 1);
	model_tranform = rotationMatrix * model_tranform;
}
void MeshModel::RotateYModel(double angle)
{
	glm::mat4x4 rotationMatrix;
	rotationMatrix[0] = glm::vec4(glm::cos(angle), 0, glm::sin(angle), 0);
	rotationMatrix[1] = glm::vec4(0, 1, 0, 0);
	rotationMatrix[2] = glm::vec4(-glm::sin(angle), 0, glm::cos(angle), 0);
	rotationMatrix[3] = glm::vec4(0, 0, 0, 1);
	model_tranform = rotationMatrix * model_tranform;
}
void MeshModel::RotateZModel(double angle)
{
	glm::mat4x4 rotationMatrix;
	rotationMatrix[0] = glm::vec4(glm::cos(angle), -glm::sin(angle), 0, 0);
	rotationMatrix[1] = glm::vec4(glm::sin(angle), glm::cos(angle), 0, 0);
	rotationMatrix[2] = glm::vec4(0, 0, 1, 0);
	rotationMatrix[3] = glm::vec4(0, 0, 0, 1);
	model_tranform = rotationMatrix * model_tranform;
}
void MeshModel::ScaleXModel(double factor)
{
	glm::mat4x4 scalingMatrix;
	scalingMatrix[0] = glm::vec4(factor, 0, 0, 0);
	scalingMatrix[1] = glm::vec4(0, 1, 0, 0);
	scalingMatrix[2] = glm::vec4(0, 0, 1, 0);
	scalingMatrix[3] = glm::vec4(0, 0, 0, 1);
	model_tranform = scalingMatrix * model_tranform;
}
void MeshModel::ScaleYModel(double factor)
{
	glm::mat4x4 scalingMatrix;
	scalingMatrix[0] = glm::vec4(1, 0, 0, 0);
	scalingMatrix[1] = glm::vec4(0, factor, 0, 0);
	scalingMatrix[2] = glm::vec4(0, 0, 1, 0);
	scalingMatrix[3] = glm::vec4(0, 0, 0, 1);
	model_tranform = scalingMatrix * model_tranform;
}
void MeshModel::ScaleZModel(double factor)
{
	glm::mat4x4 scalingMatrix;
	scalingMatrix[0] = glm::vec4(1, 0, 0, 0);
	scalingMatrix[1] = glm::vec4(0, 1, 0, 0);
	scalingMatrix[2] = glm::vec4(0, 0, factor, 0);
	scalingMatrix[3] = glm::vec4(0, 0, 0, 1);
	model_tranform = scalingMatrix * model_tranform;
}
void MeshModel::ScaleModel(double factor)
{
	model_tranform = glm::scale(glm::mat4(1), glm::vec3(factor, factor, factor)) * model_tranform;
}
void MeshModel::RotateXWorld(double angle)
{
	glm::mat4x4 rotationMatrix;
	rotationMatrix[0] = glm::vec4(1, 0, 0, 0);
	rotationMatrix[1] = glm::vec4(0, glm::cos(angle), -glm::sin(angle), 0);
	rotationMatrix[2] = glm::vec4(0, glm::sin(angle), glm::cos(angle), 0);
	rotationMatrix[3] = glm::vec4(0, 0, 0, 1);
	world_tranform = rotationMatrix * world_tranform;
}
void MeshModel::RotateYWorld(double angle)
{
	glm::mat4x4 rotationMatrix;
	rotationMatrix[0] = glm::vec4(glm::cos(angle), 0, glm::sin(angle), 0);
	rotationMatrix[1] = glm::vec4(0, 1, 0, 0);
	rotationMatrix[2] = glm::vec4(-glm::sin(angle), 0, glm::cos(angle), 0);
	rotationMatrix[3] = glm::vec4(0, 0, 0, 1);
	world_tranform = rotationMatrix * world_tranform;
}
void MeshModel::RotateZWorld(double angle)
{
	glm::mat4x4 rotationMatrix;
	rotationMatrix[0] = glm::vec4(glm::cos(angle), -glm::sin(angle), 0, 0);
	rotationMatrix[1] = glm::vec4(glm::sin(angle), glm::cos(angle), 0, 0);
	rotationMatrix[2] = glm::vec4(0, 0, 1, 0);
	rotationMatrix[3] = glm::vec4(0, 0, 0, 1);
	world_tranform = rotationMatrix * world_tranform;
}
void MeshModel::ScaleXWorld(double factor)
{
	glm::mat4x4 scalingMatrix;
	scalingMatrix[0] = glm::vec4(factor, 0, 0, 0);
	scalingMatrix[1] = glm::vec4(0, 1, 0, 0);
	scalingMatrix[2] = glm::vec4(0, 0, 1, 0);
	scalingMatrix[3] = glm::vec4(0, 0, 0, 1);
	world_tranform = scalingMatrix * world_tranform;
}
void MeshModel::ScaleYWorld(double factor)
{
	glm::mat4x4 scalingMatrix;
	scalingMatrix[0] = glm::vec4(1, 0, 0, 0);
	scalingMatrix[1] = glm::vec4(0, factor, 0, 0);
	scalingMatrix[2] = glm::vec4(0, 0, 1, 0);
	scalingMatrix[3] = glm::vec4(0, 0, 0, 1);
	world_tranform = scalingMatrix * world_tranform;
}
void MeshModel::ScaleZWorld(double factor)
{
	glm::mat4x4 scalingMatrix;
	scalingMatrix[0] = glm::vec4(1, 0, 0, 0);
	scalingMatrix[1] = glm::vec4(0, 1, 0, 0);
	scalingMatrix[2] = glm::vec4(0, 0, factor, 0);
	scalingMatrix[3] = glm::vec4(0, 0, 0, 1);
	world_tranform = scalingMatrix * world_tranform;
}
void MeshModel::ScaleWorld(double factor)
{
	world_tranform = glm::scale(glm::mat4(1), glm::vec3(factor, factor, factor)) * world_tranform;
}
void MeshModel::SetWorldTransformation(const glm::mat4x4& worldTransform)
{
	this->world_tranform = worldTransform;
}
const glm::mat4x4& MeshModel::GetWorldTransformation() const
{
	return world_tranform;
}
void MeshModel::SetModelTransformation(const glm::mat4x4& worldTransform)
{
	this->model_tranform = worldTransform;
}
const glm::mat4x4& MeshModel::GetModelTransformation() const
{
	return model_tranform;
}
void MeshModel::SetLocalScaleMatrix(float scale_x, float scale_y, float scale_z)
{
	
	scale_loc_x = scale_x;
	scale_loc_y = scale_y;
	scale_loc_z = scale_z;
	scaling_mat_loc[0][0] = scale_x;
	scaling_mat_loc[1][1] = scale_y;
	scaling_mat_loc[2][2] = scale_z;
	UpdateModelTransform();
}
void MeshModel::SetLocalTranslateMatrix(float translate_x, float  translate_y, float  translate_z)
{
	translate_loc_x = translate_x;
	translate_loc_y = translate_y;
	translate_loc_z = translate_z;
	translate_mat_loc[3][0] = translate_x;
	translate_mat_loc[3][1] = translate_y;
	translate_mat_loc[3][2] = translate_z;
	UpdateModelTransform();
}
void MeshModel::SetLocalRotateMatrix(float rotate_x, float  rotate_y, float  rotate_z)
{
	rotate_loc_x = rotate_x;
	rotate_loc_y = rotate_y;
	rotate_loc_z = rotate_z;
	float radians = glm::radians(rotate_x);
	rotation_mat_locX[1][1] = cos(radians);
	rotation_mat_locX[2][1] = -sin(radians);
	rotation_mat_locX[1][2] = sin(radians);
	rotation_mat_locX[2][2] = cos(radians);
	radians = glm::radians(rotate_y);
	rotation_mat_locY[0][0] = cos(radians);
	rotation_mat_locY[2][0] = -sin(radians);
	rotation_mat_locY[0][2] = sin(radians);
	rotation_mat_locY[2][2] = cos(radians);
	radians = glm::radians(rotate_z);
	rotation_mat_locZ[0][0] = cos(radians);
	rotation_mat_locZ[1][0] = -sin(radians);
	rotation_mat_locZ[0][1] = sin(radians);
	rotation_mat_locZ[1][1] = cos(radians);
	UpdateModelTransform();
}
void MeshModel::SetWorldScaleMatrix(float scale_x, float scale_y, float scale_z)
{
	scale_wor_x = scale_x;
	scale_wor_y = scale_y;
	scale_wor_z = scale_z;
	scaling_mat_wor[0][0] = scale_x;
	scaling_mat_wor[1][1] = scale_y;
	scaling_mat_wor[2][2] = scale_z;
	UpdateModelTransform();
}
void MeshModel::SetWorldTranslateMatrix(float translate_x, float  translate_y, float  translate_z)
{
	translate_wor_x = translate_x;
	translate_wor_y = translate_y;
	translate_wor_z = translate_z;
	translate_mat_wor[3][0] = translate_x;
	translate_mat_wor[3][1] = translate_y;
	translate_mat_wor[3][2] = translate_z;
	UpdateModelTransform();
}
void MeshModel::SetWorldRotateMatrix(float rotate_x, float  rotate_y, float  rotate_z)
{
	rotate_wor_x = rotate_x;
	rotate_wor_y = rotate_y;
	rotate_wor_z = rotate_z;
	float radians = glm::radians(rotate_x);
	rotation_mat_worX[1][1] = cos(radians);
	rotation_mat_worX[2][1] = -sin(radians);
	rotation_mat_worX[1][2] = sin(radians);
	rotation_mat_worX[2][2] = cos(radians);
	radians = glm::radians(rotate_y);
	rotation_mat_worY[0][0] = cos(radians);
	rotation_mat_worY[2][0] = -sin(radians);
	rotation_mat_worY[0][2] = sin(radians);
	rotation_mat_worY[2][2] = cos(radians);
	radians = glm::radians(rotate_z);
	rotation_mat_worZ[0][0] = cos(radians);
	rotation_mat_worZ[1][0] = -sin(radians);
	rotation_mat_worZ[0][1] = sin(radians);
	rotation_mat_worZ[1][1] = cos(radians);
	UpdateModelTransform();
}
void MeshModel::UpdateModelTransform()
{
	model_tranform = translate_mat_wor * scaling_mat_wor*rotation_mat_worX*rotation_mat_worY*rotation_mat_worZ* rotation_mat_locZ * rotation_mat_locY*rotation_mat_locX*translate_mat_loc*scaling_mat_loc;
}
float MeshModel::GetScaleLocx()//returning the local x scale  value
{
	return this->scale_loc_x;
}
float MeshModel::GetScaleLocy()//returning the local y scale  value
{
	return this->scale_loc_y;
}
float MeshModel::GetScaleLocz()//returning the local z scale  value
{
	return this->scale_loc_z;
}
float MeshModel::GetScaleWorx()//returning the World x scale  value
{
	return this->scale_wor_x;
}
float MeshModel::GetScaleWory()//returning the World y scale  value
{
	return this->scale_wor_y;
}
float MeshModel::GetScaleWorz()//returning the World z scale  value
{
	return this->scale_wor_z;
}
float MeshModel::GetTranslateLocx()//returning the local x translation  value
{
	return this->translate_loc_x;
}
float MeshModel::GetTranslateLocy()//returning the local y translation  value
{
	return this->translate_loc_y;
}
float MeshModel::GetTranslateLocz()//returning the local z translation  value
{
	return this->translate_loc_z;
}
float MeshModel::GetTranslateWorx()//returning the world x translation  value
{
	return this->translate_wor_x;
}
float MeshModel::GetTranslateWory()//returning the world y translation  value
{
	return this->translate_wor_y;
}
float MeshModel::GetTranslateWorz()//returning the world z translation  value
{
	return this->translate_wor_z;
}
float MeshModel::GetRotateLocx()//returning the local x rotation  value
{
	return this->rotate_loc_x;
}
float MeshModel::GetRotateLocy()//returning the local y rotation  value
{
	return this->rotate_loc_y;
}
float MeshModel::GetRotateLocz()//returning the local z rotation  value
{
	return this->rotate_loc_z;
}
float MeshModel::GetRotateWorx()//returning the world x rotation  value
{
	return this->rotate_wor_x;
}
float MeshModel::GetRotateWory()//returning the world y rotation  value
{
	return this->rotate_wor_y;
}
float MeshModel::GetRotateWorz()//returning the world z rotation  value
{
	return this->rotate_wor_z;
}
void MeshModel::Set_R_ambient(float val)
{
	this->ambient_color.x = val;
}
void MeshModel::Set_G_ambient(float val)
{

	this->ambient_color.y = val;
}
void MeshModel::Set_B_ambient(float val)
{

	this->ambient_color.z = val;
}
void MeshModel::Set_R_specular(float val)
{

	this->specular_color.x = val;
}
void MeshModel::Set_G_specular(float val)
{

	this->specular_color.y = val;
}
void MeshModel::Set_B_specular(float val)
{

	this->specular_color.z = val;
}
void MeshModel::Set_R_diffuse(float val)
{

	this->diffuse_color.x = val;
}
void MeshModel::Set_G_diffuse(float val)
{
	this->diffuse_color.y = val;
}
void MeshModel::Set_B_diffuse(float val)
{
	this->diffuse_color.z = val;
}
glm::vec3 MeshModel::get_ambient() {
	return this->ambient_color;
}
glm::vec3 MeshModel::get_diffuse() {
	return this->diffuse_color;
}
glm::vec3 MeshModel::get_specular() {
	return this->specular_color;
}
int MeshModel::GetTextureMappingType()
{
	return this->texture_mapping;
}
void MeshModel::SetTextureMappingType(int type)
{
	this->texture_mapping = type;
}
glm::vec4 MeshModel::GetCenter()
{
	return (this->center_cordinate);
}