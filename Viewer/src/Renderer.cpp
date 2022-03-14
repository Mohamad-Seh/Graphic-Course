#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include <iostream>
#include "Renderer.h"
#include "InitShader.h"

#define INDEX(width,x,y,c) ((x)+(y)*(width))*3+(c)
#define Z_INDEX(width,x,y) ((x)+(y)*(width))

Renderer::Renderer(int viewport_width, int viewport_height) :
	viewport_width(viewport_width),
	viewport_height(viewport_height)
{
	InitOpenglRendering();
	CreateBuffers(viewport_width, viewport_height);
}

Renderer::~Renderer()
{

}
Renderer::Renderer()
{

}
void Renderer::PutPixel(int i, int j, const glm::vec3& color)
{
	if (i < 0) return; if (i >= viewport_width) return;
	if (j < 0) return; if (j >= viewport_height) return;

	color_buffer[INDEX(viewport_width, i, j, 0)] = color.x;
	color_buffer[INDEX(viewport_width, i, j, 1)] = color.y;
	color_buffer[INDEX(viewport_width, i, j, 2)] = color.z;
}

void Renderer::DrawLine(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color)
{
	//// TODO: Implement bresenham algorithm
	//// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
	//SWITCH AND REFLECT FLAG
	bool switch_flag = false, reflect_flag = false;
	//-------------------------------------
	int x1 = p1.x, x2 = p2.x, y1 = p1.y, y2 = p2.y;
	if (x1 == x2)//straight line case 
	{
		while (y1 != y2)
		{
			PutPixel(x1, y1, color);
			if (y1 > y2)
			{
				y1 -= 1;
			}
			else
			{
				y1 += 1;
			}
		}
		return;
	}
	if (x1 > x2)//we need to swap the cordinates of the starting pint and the ending point of the line
	{
		//swaping the x coordinates
		int temp = x1;
		x1 = x2;
		x2 = temp;
		//------------------------
		//swaping the y coordinates
		temp = y1;
		y1 = y2;
		y2 = temp;
	}
	float dy = y2 - y1, dx = x2 - x1;
	float slope = dy / dx;
	int e = -dx;
	if (slope > 1) {
		switch_flag = true;
		e = -dy;
	}
	if (0 > slope&&slope >= -1) {
		reflect_flag = true;
		e = -dx;
	}
	if (slope < -1)
	{
		reflect_flag = true;
		switch_flag = true;
		e = dy;
	}
	int x = x1, y = y1, goal = x2;
	if (switch_flag)
	{
		while (y <= y2 && !reflect_flag || reflect_flag && y >= y2)
		{
			if (e > 0)
			{
				if (reflect_flag) {//m<-1
					x = x + 1;
					e = e + 2 * dy;
				}
				else if (reflect_flag == false)//m>1
				{
					x = x + 1;
					e = e - 2 * dy;
				}
			}
			PutPixel(x, y, color);
			if (reflect_flag) {//m<-1
				y = y - 1;
				e = e + 2 * dx;
			}
			else//m>1
			{
				y = y + 1;
				e = e + 2 * dx;
			}
		}
	}
	else
	{
		while (x <= x2)
		{
			if (e > 0)
			{
				if (reflect_flag) {//-1<m<0
					y = y - 1;
					e = e - 2 * dx;
				}
				else//0<m<1
				{
					y = y + 1;
					e = e - 2 * dx;
				}
			}
			PutPixel(x, y, color);
			if (reflect_flag) {//-1<m<0
				x = x + 1;
				e = e - 2 * dy;
			}
			else//0<m<1
			{
				x = x + 1;
				e = e + 2 * dy;
			}
		}
	}
}

void Renderer::Drawcircle(const glm::ivec2& p1, const float radius, const glm::vec3& color)
{
	int x1 = p1.x;
	int y1 = p1.y;
	int a = 60;
	for (int i = 0; i < a; i++)
	{
		int x2 = int(x1 + radius * sin(2 * M_PI * i / a));
		int y2 = int(y1 + radius * cos(2 * M_PI * i / a));
		glm::vec2 p2 = glm::vec2(x2, y2);
		DrawLine(p1, p2, color);
	}
	return;
}

void Renderer::Drawphone()
{
	DrawLine(glm::vec2(370, 600), glm::vec2(850, 600), glm::vec3(0, 0, 0));
	DrawLine(glm::vec2(370, 20), glm::vec2(850, 20), glm::vec3(0, 0, 0));
	DrawLine(glm::vec2(370, 600), glm::vec2(370, 20), glm::vec3(0, 0, 0));
	DrawLine(glm::vec2(850, 600), glm::vec2(850, 20), glm::vec3(0, 0, 0));
	for (int i = 1; i < 20; i++)
	{
		DrawLine(glm::vec2(370 + i, 600), glm::vec2(370 + i, 20), glm::vec3(0, 0, 0));
		DrawLine(glm::vec2(850 - i, 600), glm::vec2(850 - i, 20), glm::vec3(0, 0, 0));
	}
	for (int i = 1; i < 40; i++)
	{
		DrawLine(glm::vec2(370, 600 - i), glm::vec2(850, 600 - i), glm::vec3(0, 0, 0));
		DrawLine(glm::vec2(370, 20 + i), glm::vec2(850, 20 + i), glm::vec3(0, 0, 0));
	}
	Drawcircle(glm::vec2(600, 40), 10, glm::vec3(1, 1, 1));
	Drawcircle(glm::vec2(600, 580), 5, glm::vec3(1, 0, 0));
	// writing ZOOM
	// Z 
	DrawLine(glm::vec2(430, 540), glm::vec2(490, 540), glm::vec3(0, 0, 1));
	DrawLine(glm::vec2(430, 480), glm::vec2(490, 480), glm::vec3(0, 0, 1));
	DrawLine(glm::vec2(490, 540), glm::vec2(430, 480), glm::vec3(0, 0, 1));
	// O O 
	Drawcircle(glm::vec2(550, 510), 30, glm::vec3(0, 0, 1));
	Drawcircle(glm::vec2(550, 510), 12, glm::vec3(1, 1, 1));
	Drawcircle(glm::vec2(640, 510), 30, glm::vec3(0, 0, 1));
	Drawcircle(glm::vec2(640, 510), 12, glm::vec3(1, 1, 1));
	// M
	DrawLine(glm::vec2(710, 480), glm::vec2(710, 540), glm::vec3(0, 0, 1));
	DrawLine(glm::vec2(790, 480), glm::vec2(790, 540), glm::vec3(0, 0, 1));
	DrawLine(glm::vec2(710, 540), glm::vec2(750, 510), glm::vec3(0, 0, 1));
	DrawLine(glm::vec2(750, 510), glm::vec2(790, 540), glm::vec3(0, 0, 1));
	//
	for (int i = 1; i < 5; i++)
	{
		DrawLine(glm::vec2(430, 400 - i), glm::vec2(790, 400 - i), glm::vec3(0, 0, 1));
		DrawLine(glm::vec2(430, 200 + i), glm::vec2(790, 200 + i), glm::vec3(0, 0, 1));
		DrawLine(glm::vec2(430, 100 + i), glm::vec2(790, 100 + i), glm::vec3(0, 0, 1));
		DrawLine(glm::vec2(430, 400 - i), glm::vec2(790, 400 - i), glm::vec3(0, 0, 1));
		DrawLine(glm::vec2(429 + i, 399), glm::vec2(429 + i, 100), glm::vec3(0, 0, 1));
		DrawLine(glm::vec2(549 + i, 199), glm::vec2(549 + i, 100), glm::vec3(0, 0, 1));
		DrawLine(glm::vec2(669 + i, 199), glm::vec2(669 + i, 100), glm::vec3(0, 0, 1));
		DrawLine(glm::vec2(786 + i, 399), glm::vec2(786 + i, 100), glm::vec3(0, 0, 1));
	}
	Drawcircle(glm::vec2(610, 300), 70, glm::vec3(0.7, 0.5, 0.3));
	Drawcircle(glm::vec2(580, 320), 10, glm::vec3(1, 1, 1));
	Drawcircle(glm::vec2(640, 320), 10, glm::vec3(1, 1, 1));
	return;
}

void Renderer::CreateBuffers(int w, int h)
{
	CreateOpenglBuffer(); //Do not remove this line.
	color_buffer = new float[3 * w * h];
	ClearColorBuffer(glm::vec3(0.0f, 0.0f, 0.0f));
	Z_buffer = new float[w*h];

}

//##############################
//##OpenGL stuff. Don't touch.##
//##############################

// Basic tutorial on how opengl works:
// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/
// don't linger here for now, we will have a few tutorials about opengl later.
void Renderer::InitOpenglRendering()
{
	// Creates a unique identifier for an opengl texture.
	glGenTextures(1, &gl_screen_tex);

	// Same for vertex array object (VAO). VAO is a set of buffers that describe a renderable object.
	glGenVertexArrays(1, &gl_screen_vtc);

	GLuint buffer;

	// Makes this VAO the current one.
	glBindVertexArray(gl_screen_vtc);

	// Creates a unique identifier for a buffer.
	glGenBuffers(1, &buffer);

	// (-1, 1)____(1, 1)
	//	     |\  |
	//	     | \ | <--- The exture is drawn over two triangles that stretch over the screen.
	//	     |__\|
	// (-1,-1)    (1,-1)
	const GLfloat vtc[] = {
		-1, -1,
		 1, -1,
		-1,  1,
		-1,  1,
		 1, -1,
		 1,  1
	};

	const GLfloat tex[] = {
		0,0,
		1,0,
		0,1,
		0,1,
		1,0,
		1,1 };

	// Makes this buffer the current one.
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	// This is the opengl way for doing malloc on the gpu. 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtc) + sizeof(tex), NULL, GL_STATIC_DRAW);

	// memcopy vtc to buffer[0,sizeof(vtc)-1]
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vtc), vtc);

	// memcopy tex to buffer[sizeof(vtc),sizeof(vtc)+sizeof(tex)]
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vtc), sizeof(tex), tex);

	// Loads and compiles a sheder.
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");

	// Make this program the current one.
	glUseProgram(program);

	// Tells the shader where to look for the vertex position data, and the data dimensions.
	GLint  vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// Same for texture coordinates data.
	GLint  vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)sizeof(vtc));

	//glProgramUniform1i( program, glGetUniformLocation(program, "texture"), 0 );

	// Tells the shader to use GL_TEXTURE0 as the texture id.
	glUniform1i(glGetUniformLocation(program, "texture"), 0);
}

void Renderer::CreateOpenglBuffer()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, gl_screen_tex);

	// malloc for a texture on the gpu.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, viewport_width, viewport_height, 0, GL_RGB, GL_FLOAT, NULL);
	glViewport(0, 0, viewport_width, viewport_height);
}

void Renderer::SwapBuffers()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, gl_screen_tex);

	// memcopy's colorBuffer into the gpu.
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, viewport_width, viewport_height, GL_RGB, GL_FLOAT, color_buffer);

	// Tells opengl to use mipmapping
	glGenerateMipmap(GL_TEXTURE_2D);

	// Make glScreenVtc current VAO
	glBindVertexArray(gl_screen_vtc);

	// Finally renders the data.
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::ClearColorBuffer(const glm::vec3& color)
{
	for (int i = 0; i < viewport_width; i++)
	{
		for (int j = 0; j < viewport_height; j++)
		{
			PutPixel(i, j, color);
		}
	}
}

void Renderer::Render(const std::shared_ptr<Scene>& scene)
{
	// TODO: Replace this code with real scene rendering code
	int half_width = viewport_width / 2;
	int half_height = viewport_height / 2;
	int camera_count = scene->GetCameraCount();
	if (camera_count > 0)
	{
		const Camera& active_camera = scene->GetActiveCamera();
		int model_count = scene->GetModelCount();
		if (model_count > 0)
		{
			for (int model_index = 0; model_index < model_count; model_index++)
			{
				std::shared_ptr<MeshModel> currentModel = scene->GetModel(model_index);
				// Activate the 'colorShader' program (vertex and fragment shaders)
				
				shader.use();
				// ... set view and projection matrix
				skyboxShader.setUniform("view", active_camera.GetViewTransformation());
				skyboxShader.setUniform("projection", active_camera.GetProjectionTransformation());
				glBindVertexArray(cubeVAO);
				glBindTexture(GL_TEXTURE_2D, cubeMapTexture);
				glDrawArrays(GL_TRIANGLES, 0, 36);
				glBindVertexArray(0);
				colorShader.use();
				// Set the uniform variables
				colorShader.setUniform("model", currentModel->GetWorldTransformation()*currentModel->GetModelTransformation());
				colorShader.setUniform("view", active_camera.GetViewTransformation());
				colorShader.setUniform("projection", active_camera.GetProjectionTransformation());
				colorShader.setUniform("is_mesh", 1);
				colorShader.setUniform("mapping_type", scene->GetActiveModel()->GetTextureMappingType());
				colorShader.setUniform("center_cor", scene->GetActiveModel()->GetCenter());
				colorShader.setUniform("max_z", scene->GetActiveModel()->max_z);
				colorShader.setUniform("material.textureMap", 0);
				colorShader.setUniform("enviro_map", 1);
				colorShader.setUniform("material.k_i", scene->GetActiveModel()->get_ambient());
				colorShader.setUniform("material.k_d", scene->GetActiveModel()->get_diffuse());
				colorShader.setUniform("material.k_s", scene->GetActiveModel()->get_diffuse());
				colorShader.setUniform("camera_pos", scene->GetActiveCamera().GetEye());
				colorShader.setUniform("type", scene->GetMappingType());
				colorShader.setUniform("enviro_map", 2);
				//----Texture----
				texture1.bind(0);
				
				//drawing the model
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glBindVertexArray(currentModel->GetVAO());
				glDrawArrays(GL_TRIANGLES, 0, currentModel->GetModelVertices().size());
				glBindVertexArray(0);
				//------------------
				texture1.unbind(0);
				////------------------
				//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				//glBindVertexArray(currentModel->GetVAO());
				//glDrawArrays(GL_TRIANGLES, 0, currentModel->GetModelVertices().size());
				//glBindVertexArray(0);
			}
		}
		lightcount = scene->GetLightsNumber();
		if (lightcount)
		{
			const Camera& active_camera = scene->GetActiveCamera();
			for (int i = 0; i < scene->GetLightsNumber(); i++)
			{
				lightShader.use();
				Lights activelight = scene->GetLight(i);
				// Activate the 'colorShader' program (vertex and fragment shaders)
				// Set the uniform variables
				//colorShader.setUniform("lightposition", activelight.getlightvert());
				colorShader.setUniform("view", active_camera.GetViewTransformation());
				colorShader.setUniform("projection", active_camera.GetProjectionTransformation());
				colorShader.setUniform("is_mesh", 0);
				if (scene->GetLightsNumber())
				{
					colorShader.setUniform("light.alpha", activelight.alpha);
					colorShader.setUniform("light.l_i", activelight.ambient);
					colorShader.setUniform("light.l_d", activelight.diffuse);
					colorShader.setUniform("light.l_s", activelight.specular);
					colorShader.setUniform("light.pos", activelight.location);
				}

			}
		}
		//Draw skybox as last
		glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.use();
		glm::mat4 view = glm::mat4(glm::mat3(active_camera.GetViewTransformation()));	// Remove any translation component of the view matrix

		skyboxShader.setUniform("view", view);
		skyboxShader.setUniform("projection", active_camera.GetProjectionTransformation());

		// skybox cube
		texture3.bind(cubeMapTexture);
		glBindVertexArray(skyboxVAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // Set depth function back to default
		texture3.unbind(cubeMapTexture);
	}

}
void Renderer::LoadShaders()//loaded at the main
{
	colorShader.loadShaders("vshader.glsl", "fshader.glsl");
	shader.loadShaders("vcubemap.glsl", "fcubemap.glsl");
	skyboxShader.loadShaders("vskybox.glsl", "fskybox.glsl");
}

void Renderer::LoadTextures()//loaded at the main
{
	if (!texture1.loadTexture("bin\\debug\\texture.png", true))
	{
		texture1.loadTexture("bin\\release\\texture.png", true);
	}
	GLfloat cubeVertices[] =
	{
		// Positions          // Texture Coords
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	GLfloat skyboxVertices[] = {
		// Positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
	//shader.use();
	// Setup cube VAO
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);

	//skyboxShader.use();
	// Setup skybox VAO
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);
	std::vector<std::string> faces
	{
		"bin\\debug\\posx.PNG",
		"bin\\debug\\negx.PNG",
		"bin\\debug\\posy.PNG",
		"bin\\debug\\negy.PNG",
		"bin\\debug\\posz.PNG",
		"bin\\debug\\negz.PNG"
	};
	cubeMapTexture = texture3.loadCubemap(faces);
	
}

int Renderer::GetViewportWidth() const
{
	return viewport_width;
}

int Renderer::GetViewportHeight() const
{
	return viewport_height;
}	



