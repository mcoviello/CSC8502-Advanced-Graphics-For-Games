#include "Renderer.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Light.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Frustum.h"
#include "../nclgl/MeshMaterial.h"
#include <chrono>
const int LIGHT_NUM = 2;
const int MAX_HEIGHT = 500;
const int NO_OF_TREES = 10;

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	time = new GameTimer();
	sphere = Mesh::LoadFromMeshFile("Sphere.msh");
	quad = Mesh::GenerateQuad();
	invQuad = Mesh::GenerateInvertedQuad();
	heightMap = new HeightMap(TEXTUREDIR"Coursework/Heightmap.png");
	root = new SceneNode();
	palmTree = Mesh::LoadFromMeshFile("MY_PALM.msh");
	palmTreeMat = new MeshMaterial("MY_PALM.mat");
	fire = Mesh::LoadFromMeshFile("campfire.msh");
	fireMat = new MeshMaterial("campfire.mat");

	
	for (int i = 0; i < palmTree->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry = palmTreeMat->GetMaterialForLayer(i);
		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		treeTextures.emplace_back(texID);
	}

	for (int i = 0; i < fire->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry = fireMat->GetMaterialForLayer(i);
		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		fireTextures.emplace_back(texID);
	}

	//Initialise Textures
	sandTex = SOIL_load_OGL_texture(
		TEXTUREDIR"Coursework/sand.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	sandBump = SOIL_load_OGL_texture(
		TEXTUREDIR"Coursework/sandnormal.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	rockTex = SOIL_load_OGL_texture(
		TEXTUREDIR"Coursework/cliff.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	rockBump = SOIL_load_OGL_texture(
		TEXTUREDIR"Coursework/cliffnormal.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	grassTex = SOIL_load_OGL_texture(
		TEXTUREDIR"Coursework/grass.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	grassBump = SOIL_load_OGL_texture(
		TEXTUREDIR"Coursework/grassnormal.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	waterBump = SOIL_load_OGL_texture(
		TEXTUREDIR"Coursework/waternormal.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	grassMap = SOIL_load_OGL_texture(
		TEXTUREDIR"Coursework/GrassMap.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	textureMap = SOIL_load_OGL_texture(
		TEXTUREDIR"Coursework/Texturemap.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	islandHeightMap = SOIL_load_OGL_texture(
		TEXTUREDIR"Coursework/Heightmap.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR"/Coursework/cubemap_left.jpg", TEXTUREDIR"/Coursework/cubemap_right.jpg",
		TEXTUREDIR"/Coursework/cubemap_up.jpg", TEXTUREDIR"/Coursework/cubemap_down.jpg",
		TEXTUREDIR"/Coursework/cubemap_forward.jpg", TEXTUREDIR"/Coursework/cubemap_back.jpg",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	SetTextureRepeating(sandTex, true);
	SetTextureRepeating(sandBump, true);
	SetTextureRepeating(grassTex, true);
	SetTextureRepeating(grassBump, true);
	SetTextureRepeating(rockTex, true);
	SetTextureRepeating(rockBump, true);
	SetTextureRepeating(waterBump, true);
	SetTextureRepeating(grassMap, true);


	Vector3 heightMapSize = heightMap->GetHeightmapSize();

	//Camera Movement Track Positions
	curCamTrackPos = 0;
	cameraTrack.push_back(CameraTrackPos{ Vector3(3053.0f, 1260.0f, 770.0f), -180.0f, 0.0f });
	cameraTrack.push_back(CameraTrackPos{ Vector3(3238.0f, 674.0f, 1776.0f), -180.0f, 0.0f });
	cameraTrack.push_back(CameraTrackPos{ Vector3(3900.0f, 674.0f, 4629.0f), -180.0f, 0.0f });

	camera = new Camera(cameraTrack[0].pitch, cameraTrack[0].yaw, cameraTrack[0].position);

	pointLights = new Light[LIGHT_NUM];

	Light& l = pointLights[0];
	l.SetPosition(Vector3(heightMapSize.x / 2, 3000.0f, heightMapSize.z / 2));
	l.SetColour(Vector4(0.95f, 0.9f, 0.85f, 1));
	l.SetRadius(7000.0f);
	
	pointLights[1] = Light();
	pointLights[1].SetPosition(Vector3(2750, 550, 2050));
	pointLights[1].SetColour(Vector4(1.0f, 0.1f, 0.1f, 1));
	pointLights[1].SetRadius(400.0f);

	sceneShader = new Shader("BumpVertex.glsl", "bufferFragment.glsl");
	pointLightShader = new Shader("pointlightvert.glsl", "pointlightfrag.glsl");
	combineShader = new Shader("combineVert.glsl", "combineFrag.glsl");
	grassShader = new Shader("GrassVert.glsl", "GrassFrag.glsl", "GrassGeom.glsl", "GrassTessControl.glsl", "GrassTessEval.glsl");
	skyboxShader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	waterShader = new Shader("waterVertex.glsl", "waterFragment.glsl");
	texturedShader = new Shader("TexturedVertex.glsl", "texturedFragment.glsl");

	if (!sceneShader->LoadSuccess() || !pointLightShader->LoadSuccess() || !combineShader->LoadSuccess() ||
		!grassShader->LoadSuccess() || !skyboxShader->LoadSuccess() || !waterShader->LoadSuccess())
		return;

	BuildNodeTree();

	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &pointLightFBO);

	GLenum buffers[2] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1
	};

	GenerateScreenTexture(bufferDepthTex, true);
	GenerateScreenTexture(bufferColourTex);
	GenerateScreenTexture(bufferNormalTex);
	GenerateScreenTexture(lightDiffuseTex);
	GenerateScreenTexture(lightSpecularTex);

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glDrawBuffers(2, buffers);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return;
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightDiffuseTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lightSpecularTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	init = true;
}

Renderer ::~Renderer(void) {
	delete			sceneShader;
	delete			pointLightShader;
	delete			combineShader;
	delete			grassShader;
	delete			skyboxShader;

	delete        heightMap;
	delete        camera;
	delete        sphere;
	delete        quad;
	delete[]	pointLights;
	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &lightDiffuseTex);
	glDeleteTextures(1, &lightSpecularTex);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &pointLightFBO);
}

void Renderer::BuildNodeTree() {
	Vector3 hmSize = heightMap->GetHeightmapSize();
	SceneNode* hm = new SceneNode(heightMap);
	hm->SetShader(sceneShader);
	hm->AddTexture(sandTex);
	hm->AddTexture(sandBump);
	hm->AddTexture(rockTex);
	hm->AddTexture(rockBump);
	hm->AddTexture(grassTex);
	hm->AddTexture(grassBump);
	hm->AddTexture(textureMap);
	root->AddChild(hm);

	SceneNode* grass = new SceneNode(heightMap);
	grass->AddTexture(grassMap);
	grass->SetShader(grassShader);
	grass->SetPrimitiveType(GL_PATCHES);
	grass->SetCullFaces(false);
	grass->AddUniformToShader("time", new UniformValue((float)0.0f));
	grass->AddUniformToShader("mapScale", new UniformValue(32));
	hm->AddChild(grass);

	SceneNode* water = new SceneNode(invQuad);
	water->SetShader(waterShader);
	water->SetModelScale(Vector3(hmSize.x/2, hmSize.z/2,1));
	water->SetPrimitiveType(GL_TRIANGLE_STRIP);
	water->SetTransform(Matrix4::Translation(Vector3(hmSize.x / 2, 250, hmSize.z / 2)) * Matrix4::Rotation(90, Vector3(1, 0, 0)) * Matrix4::Rotation(180, Vector3(0, 0, 1)));
	water->AddUniformToShader("cameraPos", new UniformValue(camera->GetPosition()));
	water->AddUniformToShader("time", new UniformValue((float)0.0f));
	water->AddTexture(waterBump);
	water->AddTexture(islandHeightMap);
	hm->AddChild(water);

	SceneNode* fireNode = new SceneNode(fire);
	fireNode->SetShader(texturedShader);
	fireNode->SetMatTexture(fireTextures);
	fireNode->SetTransform(Matrix4::Translation(Vector3(2700, 510, 2000)));
	fireNode->SetModelScale(Vector3(1, 1, 1) * 1000);
	hm->AddChild(fireNode);

	SceneNode* forest = new SceneNode();
	hm->AddChild(forest);
	for (int i = 0; i < NO_OF_TREES; i++) {
		SceneNode* tree = new SceneNode(palmTree);
		tree->SetShader(texturedShader);
		//tree->SetModelScale(Vector3(10, 10, 10));
		tree->SetTransform(Matrix4::Translation(Vector3(i*1000, 0, i*1000)) * Matrix4::Scale(Vector3(1, 1, 1) * 100));
		tree->SetMatTexture(treeTextures);
		forest->AddChild(tree);
	}

}

void Renderer::GenerateScreenTexture(GLuint& into, bool depth) {
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLuint format = depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8;
	GLuint type = depth ? GL_DEPTH_COMPONENT : GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, type, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::UpdateScene(float dt) {
	grassShader->ChangeUniform("time", UniformValue{ (float)time->GetTotalTimeMSec() });
	waterShader->ChangeUniform("time", UniformValue{ (float)time->GetTotalTimeMSec() });
	waterShader->ChangeUniform("cameraPos", UniformValue{ camera->GetPosition() });
	frameFrustum.FromMatrix(projMatrix * viewMatrix);
	root->Update(dt);
	if (freeCam) {
		camera->UpdateCamera(dt);
	}
	else {
		if (Vector3::Distance(camera->GetPosition(), cameraTrack[curCamTrackPos].position) < 10.0f) {
			curCamTrackPos = (curCamTrackPos + 1) % cameraTrack.size();
		}
		else {
			camera->SetPosition(camera->GetPosition() + (cameraTrack[curCamTrackPos].position - camera->GetPosition()).Normalised() * 5);
			//TODO: Add pitch and yaw automation
		}
	}
}

void Renderer::BuildNodeLists(SceneNode* from) {
	if (frameFrustum.InsideFrustum(*from)) {
		Vector3 dir = from->GetWorldTransform().GetPositionVector() = camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));

		if (from->GetColour().w < 1.0f) {
			transparentNodeList.push_back(from);
		}
		else {
			nodeList.push_back(from);
		}
	}

	for (vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); i++) {
		BuildNodeLists((*i));
	}
}

void Renderer::ClearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear();
}

void Renderer::SortNodeLists() {
	std::sort(transparentNodeList.rbegin(), transparentNodeList.rend(), SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);

}

void Renderer::DrawNodes() {
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	for (const auto& i : nodeList) {
		DrawNode(i);
	}
	for (const auto& i : transparentNodeList) {
		DrawNode(i);
	}
}

void Renderer::DrawNode(SceneNode* n) {
	if (n->GetMesh() != nullptr && n->GetShader()) {
		Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		Shader* nodeShader = n->GetShader();
		BindShader(nodeShader);
		nodeShader->SetUniforms();
		UpdateShaderMatrices();
		glUniformMatrix4fv(glGetUniformLocation(nodeShader->GetProgram(),"modelMatrix"), 1, false, model.values);
		n->SetShaderTextures();
		//Set the primitive type to draw
		n->GetMesh()->SetPrimitive(n->GetPrimitiveType());
		//Set the cull faces value
		n->GetCullFaces() ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
		n->Draw(*this);
	}
}

void Renderer::ToggleFreecam() {
	freeCam = !freeCam;
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	modelMatrix.ToIdentity();
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
		(float)width / (float)height, 45.0f);
	BuildNodeLists(root);
	DrawSkybox();
	DrawNodes();
	DrawPointLights();
	CombineBuffers();
	ClearNodeLists();
}

void Renderer::DrawSkybox() {
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glDepthMask(GL_FALSE);

	BindShader(skyboxShader);
	UpdateShaderMatrices();

	quad->Draw();
	glDepthMask(GL_TRUE);
}

void Renderer::DrawPointLights() {
	glEnable(GL_BLEND);
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	BindShader(pointLightShader);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glBlendFunc(GL_ONE, GL_ONE);
	glCullFace(GL_FRONT);
	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_FALSE);

	glUniform1i(glGetUniformLocation(pointLightShader->GetProgram(), "depthTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glUniform1i(glGetUniformLocation(pointLightShader->GetProgram(), "normTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	glUniform3fv(glGetUniformLocation(pointLightShader->GetProgram(),"cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform2f(glGetUniformLocation(pointLightShader->GetProgram(),"pixelSize"), 1.0f / width, 1.0f / height);

	Matrix4 invViewProj = (projMatrix * viewMatrix).Inverse();
	glUniformMatrix4fv(glGetUniformLocation(pointLightShader->GetProgram(), "inverseProjView"), 1, false, invViewProj.values);

	UpdateShaderMatrices();
	for (int i = 0; i < LIGHT_NUM; i++) {
		Light& l = pointLights[i];
		SetShaderLight(l);
		sphere->Draw();
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LEQUAL);

	glDepthMask(GL_TRUE);

	glClearColor(0.2f, 0.2f, 0.2f, 1);
}

void Renderer::CombineBuffers() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	BindShader(combineShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);
	
	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "diffuseLight"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, lightDiffuseTex);

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "specularLight"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);

	quad->Draw();
	glDisable(GL_BLEND);
}
