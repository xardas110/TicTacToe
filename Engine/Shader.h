#pragma once
#include <string>
#include "../include/glm/glm.hpp"
class Shader
{
private:
	void CreateShaders(const char* vertexPath, const char* fragPath);
public:
	unsigned short ID;
	void Use();
	void Delete();
	void BindMat4(const std::string &name, glm::mat4 mat);
	void BindVec3(const std::string& name, glm::vec3 vec);
	void BindVec2(const std::string& name, glm::vec3 vec);

	Shader(const char* vertexPath, const char* fragPath);
};

