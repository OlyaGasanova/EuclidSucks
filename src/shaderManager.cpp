#include "shaderManager.h"
#include <fstream>
#include <sstream>

// =========== temporary ============

std::map<std::string, std::string> shaders_names = { { "vertexShader.glsl", "pixelShader.glsl"} };

// ==================================

ShaderManager::ShaderManager():m_shaders()
{
	load_shaders();
}

void ShaderManager::load_shaders() {
	std::string key("vertex");
	for (auto it = std::cbegin(shaders_names); it != std::cend(shaders_names); ++it)
	{
		GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

		std::string VertexShaderCode;
		std::ifstream VertexShaderStream(it->first, std::ios::in);
		if (VertexShaderStream.is_open())
		{
			std::stringstream sstr;
			sstr << VertexShaderStream.rdbuf();
			VertexShaderCode = sstr.str();
			VertexShaderStream.close();
		}

		std::string FragmentShaderCode;
		std::ifstream FragmentShaderStream(it->second, std::ios::in);
		if (FragmentShaderStream.is_open()) {
			std::stringstream sstr;
			sstr << FragmentShaderStream.rdbuf();
			FragmentShaderCode = sstr.str();
			FragmentShaderStream.close();
		}

		GLint Result = GL_FALSE;
		int InfoLogLength;

		char const* VertexSourcePointer = VertexShaderCode.c_str();
		glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
		glCompileShader(VertexShaderID);

		glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);

		char const* FragmentSourcePointer = FragmentShaderCode.c_str();
		glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
		glCompileShader(FragmentShaderID);

		glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
			fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
		}

		GLuint ProgramID = glCreateProgram();
		glAttachShader(ProgramID, VertexShaderID);
		glAttachShader(ProgramID, FragmentShaderID);
		glLinkProgram(ProgramID);

		glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
		glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
			glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
		}

		glDeleteShader(VertexShaderID);
		glDeleteShader(FragmentShaderID);

		std::size_t found = it->first.find(key);
		if (found != std::string::npos) {
			const std::string common_name(it->first, found+ key.length(), it->first.length() - key.length());
			m_shaders.insert(m_shaders.begin(), std::pair<const std::string, const GLuint>(common_name, ProgramID));
		}

	}

}


ShaderManager::~ShaderManager()
{
}

ShaderManager& ShaderManager::get_instance()
{
	static ShaderManager instance;
	return instance;
}

GLuint ShaderManager::get_shader_handle(const std::string shader_name) const
{
	auto got = m_shaders.find(shader_name);

	if (got == m_shaders.end())
		return 0;
	else
		return got->second;

}
