#pragma once
#include <unordered_map>
#include <map>
#include <openGlEntities.h>

#define Shader_Manager ShaderManager::get_instance()
class ShaderManager
{
private:
	ShaderManager();
	~ShaderManager();
	ShaderManager(ShaderManager const&) = delete;
	ShaderManager& operator= (ShaderManager const&) = delete;
	std::map<const std::string, const GLuint> m_shaders;
	void load_shaders();
public:
	static ShaderManager& get_instance();
	GLuint get_shader_handle(const std::string) const;
};

class shader
{

};
