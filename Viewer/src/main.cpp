#define _USE_MATH_DEFINES
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>

#include <nfd.h>

#include <stdio.h>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <sstream>
#include <stdlib.h>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Renderer.h"
#include "Scene.h"
#include "Camera.h"
#include "Utils.h"
static int slected_item_frame = 0;
/**
 * Fields
 */
double zoomFactor = 1;
int windowWidth = 1280;
int windowHeight = 720;
char* windowTitle = "OpenGL";
glm::vec4 clearColor = glm::vec4(0.8f, 0.8f, 0.8f, 1.00f);
bool zoomChanged = false;
std::shared_ptr<Scene> scene;
ImGuiIO* imgui;
GLFWwindow* window;

GLFWwindow* SetupGlfwWindow(int w, int h, const char* window_name);
ImGuiIO& SetupImgui(GLFWwindow* window);
bool Setup(int windowWidth, int windowHeight, const char* windowName);
void Cleanup();

static void GlfwErrorCallback(int error, const char* description);
void RenderFrame(GLFWwindow* window, std::shared_ptr<Scene> scene, Renderer& renderer, ImGuiIO& io);

void glfw_OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset);
void glfw_OnFramebufferSize(GLFWwindow* window, int width, int height);

float GetAspectRatio();
void DrawImguiMenus();
void HandleImguiInput();

/**
 * Function implementation
 */
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
	// TODO: Handle mouse scroll here
}

int main(int argc, char **argv)
{


	if (!Setup(windowWidth, windowHeight, windowTitle))
	{
		std::cerr << "Setup failed" << std::endl;
		return -1;
	}

	scene = std::make_shared<Scene>();	glm::vec3 eye = glm::vec3(0, 0, 10);
	glm::vec3 at = glm::vec3(0, 0, 0);
	glm::vec3 up = glm::vec3(0, 1, 0);
	scene->AddCamera(Camera(eye,at,up, GetAspectRatio()));

	/*scene->AddLight(std::make_shared<PointLight>(glm::vec3(0, 0, 15), glm::vec3(1, 1, 1)));
	scene->AddLight(std::make_shared<PointLight>(glm::vec3(0, 5, 5), glm::vec3(0, 0, 0)));
	scene->AddLight(std::make_shared<PointLight>(glm::vec3(-5, 0, 0), glm::vec3(0, 0, 0)));*/

	Renderer renderer;
	renderer.LoadShaders();
	renderer.LoadTextures();

	while (!glfwWindowShouldClose(window))
	{
		// Poll and process events
		glfwPollEvents();

		// Imgui stuff
		//start frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//-------------------
		DrawImguiMenus();
		ImGui::Render();
		HandleImguiInput();

		// Clear the screen and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render scene
		renderer.Render(scene);

		// Imgui stuff
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap front and back buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

static void GlfwErrorCallback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

bool Setup(int windowWidth, int windowHeight, const char* windowName)
{
	GLFWwindow* window = SetupGlfwWindow(windowWidth, windowHeight, windowName);
	if (!window)
	{
		std::cerr << "Window setup failed" << std::endl;
		return false;
	}

	imgui = &SetupImgui(window);

	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	glEnable(GL_DEPTH_TEST);

	return true;
}

GLFWwindow* SetupGlfwWindow(int windowWidth, int windowHeight, const char* windowName)
{
	// Intialize GLFW
	if (!glfwInit())
	{
		// An error occured
		std::cerr << "GLFW initialization failed" << std::endl;
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// forward compatible with newer versions of OpenGL as they become available but not backward compatible (it will not run on devices that do not support OpenGL 3.3
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Create an OpenGL 3.3 core, forward compatible context window
	window = glfwCreateWindow(windowWidth, windowHeight, windowName, NULL, NULL);
	if (window == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	// Make the window's context the current one
	glfwMakeContextCurrent(window);

	// Setup window events callbacks
	glfwSetFramebufferSizeCallback(window, glfw_OnFramebufferSize);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		// An error occured
		std::cerr << "GLAD initialization failed" << std::endl;
		return false;
	}

	return window;
}

ImGuiIO& SetupImgui(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	// Setup style
	ImGui::StyleColorsDark();

	glfwSetScrollCallback(window, glfw_OnMouseScroll);

	return io;
}

void Cleanup()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}

//-----------------------------------------------------------------------------
// Is called when the window is resized
//-----------------------------------------------------------------------------
void glfw_OnFramebufferSize(GLFWwindow* window, int width, int height)
{
	windowWidth = width;
	windowHeight = height;
	glViewport(0, 0, windowWidth, windowHeight);
	scene->GetActiveCamera().SetAspectRatio(GetAspectRatio());
}

void glfw_OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
	zoomFactor = glm::pow(1.1, -yoffset);
	zoomChanged = true;
}

float GetAspectRatio()
{
	return static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
}
void DrawImguiMenus()
{
	/**
	 * MeshViewer menu
	 */
	ImGui::Begin("MeshViewer Menu");

	// Menu Bar
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open", "CTRL+O"))
			{
				nfdchar_t* outPath = NULL;
				nfdresult_t result = NFD_OpenDialog("obj;", NULL, &outPath);
				if (result == NFD_OKAY)
				{
					scene->AddModel(Utils::LoadMeshModel(outPath));

					free(outPath);

				}
				else if (result == NFD_CANCEL)
				{
				}
				else
				{
				}

			}
			ImGui::EndMenu();
		}

		// TODO: Add more menubar items (if you want to)
		ImGui::EndMainMenuBar();
	}

	// Controls
	if (ImGui::ColorEdit3("Clear Color", (float*)&clearColor))
	{
		glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	}

	// TODO: Add more controls as needed
	if (ImGui::CollapsingHeader("Camera"))
	{
		if (ImGui::Button("Add Camera"))//adding new camera
		{

			std::random_device rd;
			std::mt19937 mt(rd());
			std::uniform_real_distribution<double> dist(0, 2 * M_PI);
			std::uniform_real_distribution<double> dist2(2, 10);
			double angle = dist(mt);
			double radius = dist2(mt);

			glm::vec4 eye = glm::vec4(radius * glm::cos(angle), 0, radius * glm::sin(angle), 1);
			glm::vec4 at = glm::vec4(0, 0, 0, 1);
			glm::vec4 up = glm::vec4(0, 1, 0, 1);
			scene->AddCamera(Camera(-scene->GetActiveCamera().GetEye(), at, up, GetAspectRatio()));
		}
		const char** items;
		std::vector<std::string> cameraStrings;
		items = new const char*[scene->GetCameraCount()];
		for (int i = 0; i < scene->GetCameraCount(); i++)
		{
			std::ostringstream s;
			s << "Camera " << i;
			std::string mystring = s.str();
			cameraStrings.push_back(mystring);
		}

		for (int i = 0; i < scene->GetCameraCount(); i++)
		{
			items[i] = cameraStrings[i].c_str();
		}

		int currentCamera = scene->GetActiveCameraIndex();
		ImGui::Combo("Active Camera", &currentCamera, items, scene->GetCameraCount());

		if (currentCamera != scene->GetActiveCameraIndex())
		{
			scene->SetActiveCameraIndex(currentCamera);
			scene->GetActiveCamera().SetAspectRatio(GetAspectRatio());
		}

		delete items;
		int static newProjectionType = scene->GetActiveCamera().IsPrespective() ? 0 : 1;
		ImGui::RadioButton("Prespective", &newProjectionType, 0);
		ImGui::RadioButton("Orthographic", &newProjectionType, 1);
		if (newProjectionType == 0)//perspective projection
		{
			static float fovy;
			static float zNear;
			static float zFar;

			scene->GetActiveCamera().SwitchToPrespective();

			if (ImGui::InputFloat("Fovy", &fovy, 0.1f, 0.3f))
			{
				scene->GetActiveCamera().SetFovy(fovy);
			}

			if (ImGui::InputFloat("Near", &zNear, 1.0f, 4.0f))
			{
				scene->GetActiveCamera().SetNear(zNear);
			}

			if (ImGui::InputFloat("Far", &zFar, 1.0f, 4.0f))
			{
				scene->GetActiveCamera().SetFar(zFar);
			}
		}
		else if (newProjectionType == 1)
		{
			static float height;
			static float zNear;
			static float zFar;

			scene->GetActiveCamera().SwitchToOrthographic();

			if (ImGui::InputFloat("Height", &height, 1.0f,3.0f))
			{
				scene->GetActiveCamera().SetHeight(height);
			}

			if (ImGui::InputFloat("Near", &zNear, 0.1f,0.5f))
			{
				scene->GetActiveCamera().SetNear(zNear);
			}

			if (ImGui::InputFloat("Far", &zFar, 0.1f, 0.5f))
			{
				scene->GetActiveCamera().SetFar(zFar);
			}
		}
	}
	
	if (ImGui::CollapsingHeader("Models"))
	{
		if (scene->GetModelCount() > 0)
		{
			static float translatex = 0;
			static float translatey = 0;
			static float translatez = 0;
			static float scalex = 1;
			static float scaley = 1;
			static float scalez = 1;
			static float rotatex = 0;
			static float rotatey = 0;
			static float rotatez = 0;
			static bool world = false;
			static int texture_mapping = scene->GetActiveModel()->GetTextureMappingType();
			static int scene_mapping = scene->GetMappingType();
			const char** items;
			std::vector<std::string> modelStrings;
			items = new const char*[scene->GetModelCount()];
			for (int i = 0; i < scene->GetModelCount(); i++)
			{
				std::ostringstream s;
				s << scene->GetModel(i)->GetModelName();
				std::string mystring = s.str();
				modelStrings.push_back(mystring);
			}
			for (int i = 0; i < scene->GetModelCount(); i++)
			{
				items[i] = modelStrings[i].c_str();
			}
			int currentModelIndex = scene->GetActiveModelIndex();
			ImGui::Combo("Active Model", &currentModelIndex, items, scene->GetModelCount());

			if (currentModelIndex != scene->GetActiveModelIndex())
			{
				scene->SetActiveModelIndex(currentModelIndex);
			}
			if (scene->GetModelCount())
			{
				if (ImGui::RadioButton("none", &texture_mapping, -1))
				{
					scene->GetActiveModel()->SetTextureMappingType(texture_mapping);
				} ImGui::SameLine();
				if (ImGui::RadioButton("Planer", &texture_mapping, 0))
				{
					scene->GetActiveModel()->SetTextureMappingType(texture_mapping);
				} ImGui::SameLine();
				if(ImGui::RadioButton("Cylindrical", &texture_mapping, 1) )
				{
					scene->GetActiveModel()->SetTextureMappingType(texture_mapping);
				} ImGui::SameLine();
				if (ImGui::RadioButton("Spherical", &texture_mapping, 2))
				{
					scene->GetActiveModel()->SetTextureMappingType(texture_mapping);
				}
				ImGui::ColorEdit3("ambient", (float*)&scene->GetActiveModel()->ambient_color);
				ImGui::ColorEdit3("diffuse", (float*)&scene->GetActiveModel()->diffuse_color); 
				ImGui::ColorEdit3("specular", (float*)&scene->GetActiveModel()->specular_color);
				if (ImGui::RadioButton("None", &scene_mapping, -1))
				{
					scene->SetMappingType(scene_mapping);
				} ImGui::SameLine();
				if (ImGui::RadioButton("Toon Shading", &scene_mapping, 0))
				{
					scene->SetMappingType(scene_mapping);
				} ImGui::SameLine();
				if (ImGui::RadioButton("Normal Mapping", &scene_mapping, 1))
				{
					scene->SetMappingType(scene_mapping);
				} ImGui::SameLine();
				if (ImGui::RadioButton("Environment mapping", &scene_mapping, 2))
				{
					scene->SetMappingType(scene_mapping);
				}

			}
			delete items;
			ImGui::Checkbox("World", &world); 
			if (world)
			{
				 translatex = scene->GetActiveModel()->GetTranslateWorx();
				 translatey = scene->GetActiveModel()->GetTranslateWory();
				 translatez = scene->GetActiveModel()->GetTranslateWorz();
				 scalex = scene->GetActiveModel()->GetScaleWorx();
				 scaley = scene->GetActiveModel()->GetScaleWory();
				 scalez = scene->GetActiveModel()->GetScaleWorz();
				 rotatex = scene->GetActiveModel()->GetRotateWorx();
				 rotatey = scene->GetActiveModel()->GetRotateWory();
				 rotatez = scene->GetActiveModel()->GetRotateWorz();
			}
			else
			{
				translatex = scene->GetActiveModel()->GetTranslateLocx();
				translatey = scene->GetActiveModel()->GetTranslateLocy();
				translatez = scene->GetActiveModel()->GetTranslateLocz();
				scalex = scene->GetActiveModel()->GetScaleLocx();
				scaley = scene->GetActiveModel()->GetScaleLocy();
				scalez = scene->GetActiveModel()->GetScaleLocz();
				rotatex = scene->GetActiveModel()->GetRotateLocx();
				rotatey = scene->GetActiveModel()->GetRotateLocy();
				rotatez = scene->GetActiveModel()->GetRotateLocz();
			}
			if (ImGui::InputFloat("TranslateX", &translatex, 0.1f, 1.0f, "%.3f"))
			{
				if (world)
				{
					scene->GetActiveModel()->SetWorldTranslateMatrix(translatex, translatey, translatez);
				}
				else
				{
				scene->GetActiveModel()->SetLocalTranslateMatrix(translatex, translatey, translatez);
				}
			}
			if (ImGui::InputFloat("TranslateY ", &translatey, 0.1f, 1.0f, "%.3f"))
			{
				if (world)
				{
					scene->GetActiveModel()->SetWorldTranslateMatrix(translatex, translatey, translatez);
				}
				else
				{
					scene->GetActiveModel()->SetLocalTranslateMatrix(translatex, translatey, translatez);
				}
			}
			if (ImGui::InputFloat("TranslateZ", &translatez, 0.1f, 1.0f, "%.3f"))
			{
				if (world)
				{
					scene->GetActiveModel()->SetWorldTranslateMatrix(translatex, translatey, translatez);
				}
				else
				{
					scene->GetActiveModel()->SetLocalTranslateMatrix(translatex, translatey, translatez);
				}
			}
			if (ImGui::InputFloat("ScaleX", &scalex, 0.1f, 1.0f, "%.3f"))
			{
				if (world)
				{
					scene->GetActiveModel()->SetWorldScaleMatrix(scalex, scaley, scalez);
				}
				else
				{
					scene->GetActiveModel()->SetLocalScaleMatrix(scalex, scaley, scalez);
				}
			}
			if (ImGui::InputFloat("ScaleY ", &scaley, 0.1f, 1.0f, "%.3f"))
			{
				if (world)
				{
					scene->GetActiveModel()->SetWorldScaleMatrix(scalex, scaley, scalez);
				}
				else
				{
					scene->GetActiveModel()->SetLocalScaleMatrix(scalex, scaley, scalez);
				}
			}
			if (ImGui::InputFloat("ScaleZ", &scalez, 0.1f, 1.0f, "%.3f"))
			{
				if (world)
				{
					scene->GetActiveModel()->SetWorldScaleMatrix(scalex, scaley, scalez);
				}
				else
				{
					scene->GetActiveModel()->SetLocalScaleMatrix(scalex, scaley, scalez);
				}
			}
			if (ImGui::InputFloat("RotateX ", &rotatex, 1.0f, 10.0f, "%.3f"))
			{
				if (world)
				{
					scene->GetActiveModel()->SetWorldRotateMatrix(rotatex, rotatey, rotatez);
				}
				else
				{
					scene->GetActiveModel()->SetLocalRotateMatrix(rotatex, rotatey, rotatez);
				}
			}
			if (ImGui::InputFloat("RotateY ", &rotatey, 1.0f, 10.0f, "%.3f"))
			{
				if (world)
				{
					scene->GetActiveModel()->SetWorldRotateMatrix(rotatex, rotatey, rotatez);
				}
				else
				{
					scene->GetActiveModel()->SetLocalRotateMatrix(rotatex, rotatey, rotatez);
				}
			}
			if (ImGui::InputFloat("RotateZ", &rotatez, 1.0f, 10.0f, "%.3f"))
			{
				if (world)
				{
					scene->GetActiveModel()->SetWorldRotateMatrix(rotatex, rotatey, rotatez);
				}
				else
				{
					scene->GetActiveModel()->SetLocalRotateMatrix(rotatex, rotatey, rotatez);
				}
			}
		}
	} 
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
	ImGui::Begin("lights Control Window");
	if (ImGui::Button("Add light"))
	{
		scene->AddLight();
	}
	static int Light_index = 1;
	if (scene->GetLightsNumber() > 1)
	{
		static int sliderint = 1;
		ImGui::SliderInt("Light Selction", &Light_index, 1, scene->GetLightsNumber());
	}
	if (scene->GetLightsNumber() > 0)
	{

		Lights light = scene->GetLight(Light_index - 1);

		static bool flag = false;
		ImGui::ColorEdit3("ambient", (float*)&scene->GetLight(Light_index - 1).ambient);
		ImGui::ColorEdit3("diffuse", (float*)&scene->GetLight(Light_index - 1).diffuse);
		ImGui::ColorEdit3("specular", (float*)&scene->GetLight(Light_index - 1).specular);
		ImGui::InputFloat("move x", (float*)&scene->GetLight(Light_index - 1).location.x, 1.0f, 1.0f, "%.3f");
		ImGui::InputFloat("move y", (float*)&scene->GetLight(Light_index - 1).location.y, 1.0f, 1.0f, "%.3f");
		ImGui::InputFloat("move z", (float*)&scene->GetLight(Light_index - 1).location.z, 1.0f, 1.0f, "%.3f");
		ImGui::InputFloat("Alpha", (float*)&scene->GetLight(Light_index - 1).alpha, 1.0f, 1.0f, "%.3f");
	}
	ImGui::End(); 
}
void HandleImguiInput()
{
	if (!imgui->WantCaptureKeyboard)
	{
		if (imgui->KeysDown[49]) // 1
		{
			scene->GetActiveModel()->RotateXModel(M_PI / 200);
		}

		if (imgui->KeysDown[50]) // 2
		{
			scene->GetActiveModel()->RotateXModel(-M_PI / 200);
		}

		if (imgui->KeysDown[51]) // 3
		{
			scene->GetActiveModel()->RotateYModel(M_PI / 200);
		}

		if (imgui->KeysDown[52]) // 4
		{
			scene->GetActiveModel()->RotateYModel(-M_PI / 200);
		}

		if (imgui->KeysDown[53]) // 5
		{
			scene->GetActiveModel()->RotateZModel(M_PI / 200);
		}

		if (imgui->KeysDown[54]) // 6
		{
			scene->GetActiveModel()->RotateZModel(-M_PI / 200);
		}

		if (imgui->KeysDown[45]) // -
		{
			scene->GetActiveModel()->ScaleModel(1 / 1.01);
		}

		if (imgui->KeysDown[61]) // +
		{
			scene->GetActiveModel()->ScaleModel(1.01);
		}

		if (imgui->KeysDown[65]) // a
		{
			scene->GetActiveModel()->TranslateModel(glm::vec3(-0.02, 0, 0));
		}

		if (imgui->KeysDown[68]) // d
		{
			scene->GetActiveModel()->TranslateModel(glm::vec3(0.02, 0, 0));
		}

		if (imgui->KeysDown[83]) // s
		{
			scene->GetActiveModel()->TranslateModel(glm::vec3(0, 0, 0.02));
		}

		if (imgui->KeysDown[87]) // w
		{
			scene->GetActiveModel()->TranslateModel(glm::vec3(0, 0, -0.02));
		}
	}

	//if (!imgui->WantCaptureMouse)
	//{
	//	if (zoomChanged)
	//	{
	//		scene->GetActiveCamera().Zoom(zoomFactor);
	//		zoomChanged = false;
	//	}
	//}
}
