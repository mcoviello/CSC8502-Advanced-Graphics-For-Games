#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Frustum.h"
#include <Vector>
class Camera;
class Mesh;
class HeightMap;
class SceneNode;
class MeshMaterial;

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
	void BuildNodeTree();
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
	void DrawPointLights();
	void CombineBuffers();

	void GenerateScreenTexture(GLuint& into, bool depth = false);

	SceneNode* root;

	Shader* sceneShader;
	Shader* pointLightShader;
	Shader* combineShader;
	Shader* grassShader;
	Shader* skyboxShader;
	Shader* waterShader;
	Shader* texturedShader;

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
	Mesh* invQuad;
	Mesh* palmTree;
	Mesh* fire;
	Camera* camera;

	MeshMaterial* palmTreeMat;
	MeshMaterial* fireMat;

	GLuint cubeMap;
	GLuint sandTex;
	GLuint sandBump;
	GLuint rockTex;
	GLuint rockBump;
	GLuint grassTex;
	GLuint grassBump;
	GLuint waterBump;
	GLuint grassMap;
	GLuint islandHeightMap;
	GLuint treeMap;
	GLuint textureMap;
	GLuint treeTex;

	std::vector<GLuint> treeTextures;
	std::vector<GLuint> fireTextures;

	GameTimer* time;

	bool freeCam = true;
	std::vector<CameraTrackPos> cameraTrack;
	int curCamTrackPos;

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;
	Frustum frameFrustum;
};

