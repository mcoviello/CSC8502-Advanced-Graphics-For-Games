#pragma once
#include "../nclgl/OGLRenderer.h"

class Camera;
class Mesh;
class HeightMap;
class SceneNode;

class Renderer : public OGLRenderer
{
public:
	Renderer(Window& parent);
	~Renderer(void);

	void RenderScene() override;
	void UpdateScene(float dt) override;
protected:
	void FillBuffers();
	void DrawPointLights();
	void CombineBuffers();
	void DrawGrass();

	void GenerateScreenTexture(GLuint& into, bool depth = false);

	SceneNode* root;

	Shader* sceneShader;
	Shader* pointLightShader;
	Shader* combineShader;
	Shader* grassShader;

	GLuint bufferFBO;
	GLuint bufferColourTex;
	GLuint bufferNormalTex;
	GLuint bufferDepthTex;

	GLuint pointLightFBO;
	GLuint lightDiffuseTex;
	GLuint lightSpecularTex;

	HeightMap* heightMap;
	Light* pointLights;
	Mesh* sphere;
	Mesh* quad;
	Camera* camera;

	GLuint earthTex;
	GLuint earthBump;
	GLuint grassMap;
	GLuint seabedTex;
	GLuint seabedBump;

	GameTimer* time;
};

