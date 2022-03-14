#pragma once
#include "Scene.h"
#include "ShaderProgram.h"
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <memory>
#include <Utils.h>
#include "Texture2D.h"

class Renderer
{
public:
	Renderer(int viewportWidth, int viewportHeight);
	Renderer();
	void Render(const std::shared_ptr<Scene>& scene);
	void env_mapping(const std::shared_ptr<Scene>& scene);
	virtual ~Renderer();
	void LoadShaders();
	void LoadTextures();
	void SwapBuffers();
	void ClearColorBuffer(const glm::vec3& color);
	int GetViewportWidth() const;
	int GetViewportHeight() const;
	void  DrawMesh(MeshModel mesh, std::vector<glm::vec3> vertices, std::vector<Face> faces,Scene scene,int width,int height,int index);//true world false local  
	void Renderer::SetViewportWidth(int ViewportWidth);
	void Renderer::SetViewportHeight(int ViewportHeight);
private:
	void PutPixel(const int i, const int j, const glm::vec3& color);
	void DrawLine(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color);
	void Renderer::Drawcircle(const glm::ivec2& p1, const float radius, const glm::vec3& color);
	void Drawphone();
	void CreateBuffers(int w, int h);
	void CreateOpenglBuffer();
	void InitOpenglRendering();
	float* color_buffer; 
	float* Z_buffer;
	int viewport_width;
	int viewport_height;
	GLuint gl_screen_tex;
	GLuint gl_screen_vtc;
	float is_orth = 1;
	float maxz=-1*INFINITY;
	float minz=INFINITY;
	ShaderProgram lightShader;
	ShaderProgram colorShader;
	ShaderProgram shader;
	ShaderProgram skyboxShader;
	GLuint cubeMapTexture;
	GLuint cubeVAO, cubeVBO;
	GLuint skyboxVAO, skyboxVBO;
	Texture2D texture1;
	Texture2D texture2;
	Texture2D texture3;
	int lightcount;
};
