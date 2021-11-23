#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Frustum.h"
#include <Vector>
class Camera;
class Mesh;
class HeightMap;
class SceneNode;

struct CameraTrackPos {
	Vector3 position;
	float pitch;
	float yaw;
};

class Renderer : public OGLRenderer
{
public:
	Renderer(Window& parent);
	~Renderer(void);

	void RenderScene() override;
	void UpdateScene(float dt) override;
	void ToggleFreecam();
protected:
	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode* n);

	void DrawSkybox();
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
	Shader* skyboxShader;

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

	GLuint cubeMap;
	GLuint earthTex;
	GLuint earthBump;
	GLuint grassMap;

	GameTimer* time;

	bool freeCam = true;
	std::vector<CameraTrackPos> cameraTrack;
	int curCamTrackPos;

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;
	Frustum frameFrustum;
};

