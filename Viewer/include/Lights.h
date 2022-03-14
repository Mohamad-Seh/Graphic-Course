#pragma once

#include <glm/glm.hpp>

class Lights
{
public:
	Lights();
	Lights(const glm::vec3& color);
	virtual ~Lights();
	
	const glm::vec3& GetColor()  const;
	void SetColor(const glm::vec3& color);
	glm::vec3 ambient= glm::vec3(1.0f);
	glm::vec3 diffuse = glm::vec3(1.0f);;
	glm::vec3 specular = glm::vec3(1.0f);;
	float alpha=1;
	glm::vec3 location=glm::vec3(0.0f);
protected:
};
