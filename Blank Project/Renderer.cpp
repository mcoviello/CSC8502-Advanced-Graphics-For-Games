#include "Renderer.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Light.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Frustum.h"
#include <chrono>
const int LIGHT_NUM = 1;
const int MAX_HEIGHT = 500;

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	time = new GameTimer();
	sphere = Mesh::LoadFromMeshFile("Sphere.msh");
	quad = Mesh::GenerateQuad();
	heightMap = new HeightMap(TEXTUREDIR"Coursework/Heightmap.png");
	root = new SceneNode();
	SceneNode* hmNode = new SceneNode(heightMap);

	//Initialise Textures
	earthTex = SOIL_load_OGL_texture(
		TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	earthBump = SOIL_load_OGL_texture(
		TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	grassMap = SOIL_load_OGL_texture(
		TEXTUREDIR"Coursework/GrassMap.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg",
		TEXTUREDIR"rusted_up.jpg", TEXTUREDIR"rusted_down.jpg",
		TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	SetTextureRepeating(earthTex, true);
	SetTextureRepeating(earthBump, true);
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
	l.SetPosition(Vector3(4000, 2000.0f, heightMapSize.z / 2));
	l.SetColour(Vector4(0.95f, 0.9f, 0.85f, 1));
	l.SetRadius(5000.0f);

	sceneShader = new Shader("BumpVertex.glsl", "bufferFragment.glsl");
	pointLightShader = new Shader("pointlightvert.glsl", "pointlightfrag.glsl");
	combineShader = new Shader("combineVert.glsl", "combineFrag.glsl");
	grassShader = new Shader("GrassVert.glsl", "GrassFrag.glsl", "GrassGeom.glsl", "GrassTessControl.glsl", "GrassTessEval.glsl");
	skyboxShader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");

	if (!sceneShader->LoadSuccess() || !pointLightShader->LoadSuccess() || !combineShader->LoadSuccess())
		return;


	hmNode->AddTexture(earthTex);
	hmNode->AddTexture(earthBump);
	hmNode->SetShader(sceneShader);
	hmNode->GetShader()->AddUniform("diffuseTex", new UniformValue(0));
	hmNode->GetShader()->AddUniform("bumpTex", new UniformValue(1));

	nodeList.emplace_back(hmNode);

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

void Renderer::SortNodeLists() {
	std::sort(transparentNodeList.rbegin(), transparentNodeList.rend(), SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);

}

void Renderer::DrawNodes() {
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	for (const auto& i : nodeList) {
		DrawNode(i);
	}
	for (const auto& i : transparentNodeList) {
		DrawNode(i);
	}
}

void Renderer::DrawNode(SceneNode* n) {
	if (n->GetMesh() && n->GetShader()) {
		Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		Shader* nodeShader = n->GetShader();
		//UpdateShaderMatrices();
		nodeShader->ChangeUniform("modelMatrix", modelMatrix);
		nodeShader->ChangeUniform("projMatrix", projMatrix);
		nodeShader->ChangeUniform("viewMatrix", viewMatrix);
		nodeShader->SetUniforms();
		n->SetShaderTextures();
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
	UpdateShaderMatrices();

	DrawSkybox();
	//FillBuffers();
	DrawNodes();
	DrawGrass();
	DrawPointLights();
	CombineBuffers();
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

void Renderer::FillBuffers() {
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);

	BindShader(sceneShader);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "bumpTex"), 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthTex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, earthBump);

	UpdateShaderMatrices();

	heightMap->Draw();
}

void Renderer::DrawPointLights() {
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
	glEnable(GL_BLEND);
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

void Renderer::DrawGrass() {
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glDisable(GL_CULL_FACE);
	BindShader(grassShader);
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(grassShader->GetProgram(), "grassMap"), 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grassMap);

	glUniform1f(glGetUniformLocation(grassShader->GetProgram(), "time"),time->GetTotalTimeMSec());

	glUniform1i(glGetUniformLocation(grassShader->GetProgram(), "tessAmount"), 2);

	glUniform1i(glGetUniformLocation(grassShader->GetProgram(), "mapScale"), 32);

	glPatchParameteri(GL_PATCH_VERTICES, 3);
	heightMap->DrawType(GL_PATCHES);
	glEnable(GL_CULL_FACE);
}
