#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>
#include "MeshModel.h"
#include "Lights.h"
#include "Camera.h"
using namespace std;

class Scene {
private:
	std::vector<std::shared_ptr<MeshModel>> models;
	std::vector<Camera> cameras;
	std::vector<std::shared_ptr<Lights>> lights;
	int activeCameraIndex;
	int activeModelIndex;
	int lights_num = 0;
	int mapping_type = -1;
public:
	Scene();
	void Scene::AddLight();
	void AddModel(const std::shared_ptr<MeshModel>& model);
	int GetModelCount() const;
	std::shared_ptr<MeshModel> GetModel(int index) const;
	int GetLightsNumber()const;
	void SetMappingType(int type);
	int GetMappingType();

	void AddCamera(const Camera& camera);
	int GetCameraCount() const;
	Camera& GetCamera(int index);
	const Camera& GetCamera(int index) const;

	

	const Camera& GetActiveCamera() const;
	Camera& GetActiveCamera();

	void SetActiveCameraIndex(int index);
	const int GetActiveCameraIndex() const;

	const std::shared_ptr<MeshModel>& GetActiveModel() const;

	void SetActiveModelIndex(int index);
	const int GetActiveModelIndex() const;
	Lights& Scene::GetLight(int index) const;
};