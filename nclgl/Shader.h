/******************************************************************************
Class:Shader
Implements:
Author:Rich Davison	 <richard-gordon.davison@newcastle.ac.uk>
Description:VERY simple class to encapsulate GLSL shader loading, linking,
and binding. Useful additions to this class would be overloaded functions to
replace the glUniformxx functions in external code, and possibly a map to store
uniform names and their resulting bindings. 

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////

#pragma once
#include "OGLRenderer.h"
#include <map>
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix3.h"
#include "Matrix4.h"

union UniformValue
{
	int i;
	float f;
	Vector2 vec2;
	Vector3 vec3;
	Vector4 vec4;
	Matrix3 mat3;
	Matrix4 mat4;

	UniformValue(int in) { i = in; }
	UniformValue(float fl){ f = fl; }
	UniformValue(Vector2 v2) { vec2 = v2; }
	UniformValue(Vector3 v3) { vec3 = v3; }
	UniformValue(Vector4 v4) { vec4 = v4; }
	UniformValue(Matrix3 m3) { mat3 = m3; }
	UniformValue(Matrix4 m4) { mat4 = m4; }
	~UniformValue() {};
};

enum UniformType {
	UNIFORMINT,
	UNIFORMFLOAT,
	UNIFORMVEC2,
	UNIFORMVEC3,
	UNIFORMVEC4,
	UNIFORMMAT3,
	UNIFORMMAT4,
	UNIFORMSAMPLER2D
};

enum ShaderStage {
	SHADER_VERTEX,
	SHADER_FRAGMENT,
	SHADER_GEOMETRY,
	SHADER_DOMAIN,
	SHADER_HULL,
	SHADER_MAX
};

class Shader	{
public:
	Shader(const std::string& vertex, const std::string& fragment, const std::string& geometry = "", const std::string& domain = "", const std::string& hull = "");
	~Shader(void);

	GLuint  GetProgram() { return programID;}
	
	void	Reload(bool deleteOld = true);

	bool	LoadSuccess() {
		return shaderValid[0] == GL_TRUE && programValid == GL_TRUE;
	}

	void		SetUniforms();
	void		AddUniform(std::string s, union UniformValue* v) { customUniforms.insert(std::pair<std::string, union UniformValue*>(s,v)); }
	void		ChangeUniform(std::string, UniformValue);

	static void ReloadAllShaders();
	static void	PrintCompileLog(GLuint object);
	static void	PrintLinkLog(GLuint program);

protected:
	void	DeleteIDs();

	bool	LoadShaderFile(const  std::string& from, std::string &into);
	void	GenerateShaderObject(unsigned int i);
	void	SetDefaultAttributes();
	void	LinkProgram();

	GLuint	programID;
	GLuint	objectIDs[SHADER_MAX];
	GLint	programValid;
	GLint	shaderValid[SHADER_MAX];

	std::string  shaderFiles[SHADER_MAX];

	static std::vector<Shader*> allShaders;
	std::map<std::string, UniformValue*> customUniforms;
	std::map<std::string, UniformType> shaderUniforms;
};

