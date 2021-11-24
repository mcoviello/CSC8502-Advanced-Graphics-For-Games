#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Mesh.h"
#include <map>
#include <vector>

class SceneNode
{
public:
	SceneNode(Mesh* m = NULL, Vector4 colour = Vector4(1, 1, 1, 1));
	~SceneNode(void);

	void SetTransform(const Matrix4& matrix)		{ transform = matrix; }
	const Matrix4& GetTransform()			const	{ return transform; }
	Matrix4 GetWorldTransform()				const	{ return worldTransform; }

	Vector4 GetColour()						const	{ return colour; }
	void SetColour(Vector4 c)						{ colour = c; }

	GLint GetPrimitiveType()						const { return primitiveType; }
	void SetPrimitiveType(GLint t)					{ primitiveType = t; }

	bool GetCullFaces()								const { return cullFaces; }
	void SetCullFaces(bool c)						{ cullFaces = c; }

	Vector3 GetModelScale()					const	{ return modelScale; }
	void SetModelScale(Vector3 s)					{ modelScale = s; }

	Shader* GetShader()						const	{ return shader; }
	void SetShader(Shader* s)						{ shader = s; }
	void AddUniformToShader(std::string s, UniformValue* v) { shader->AddUniform(s, v); }

	Mesh* GetMesh()							const	{ return mesh; }
	void SetMesh(Mesh* m)							{ mesh = m; }

	void AddChild(SceneNode* s);

	float GetBoundingRadius() const { return boundingRadius; }
	void SetBoundingRadius(float f) { boundingRadius = f; }

	float GetCameraDistance() const { return distanceFromCamera; }
	void SetCameraDistance(float f) { distanceFromCamera = f; }

	std::vector<GLint> GetTextures() const { return textures; }
	void AddTexture(GLuint tex) { textures.emplace_back(tex); }
	void SetShaderTextures();

	static bool CompareByCameraDistance(SceneNode* a, SceneNode* b) {
		return (a->distanceFromCamera < b->distanceFromCamera) ? true : false;
	}


	virtual void Update(float dt);
	virtual void Draw(const OGLRenderer &r);

	std::vector<SceneNode*>::const_iterator GetChildIteratorStart() {
		return children.begin();
	}

	std::vector<SceneNode*>::const_iterator GetChildIteratorEnd() {
		return children.end();
	}

protected:
	SceneNode*					parent;
	Mesh*						mesh;
	Shader*						shader;
	Matrix4						worldTransform;
	Matrix4						transform;
	Vector3						modelScale;
	Vector4						colour;
	std::vector<SceneNode*>		children;
	std::vector<GLint>			textures;
	GLint						primitiveType = GL_TRIANGLES;
	bool						cullFaces = true;

	float distanceFromCamera;
	float boundingRadius;
};

