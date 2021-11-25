#include "SceneNode.h"
#include "../nclgl/MeshMaterial.h"

SceneNode::SceneNode(Mesh* mesh, Vector4 colour) {
	this->mesh = mesh;
	this->colour = colour;
	shader = nullptr;
	parent = NULL;
	modelScale = Vector3(1, 1, 1);
	boundingRadius = 5000.0f;
	distanceFromCamera = 0.0f;
}

SceneNode::~SceneNode(void) {
	for (unsigned int i = 0; i < children.size(); ++i) {
		delete children[i];
	}
}

void SceneNode::AddChild(SceneNode* s) {
	children.push_back(s);
	s->parent = this;
}

void SceneNode::Draw(const OGLRenderer& r) {
	if (mesh && shader) {
		mesh->Draw(); 
	}
}

void SceneNode::SetShaderTextures() {
	if (mesh && shader) {
		if (matTextures.size() > 0) {
			for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, matTextures[i]);
				//mesh->DrawSubMesh(i);
			}
		}
		else {
			for (int i = 0; i < textures.size(); i++) {
				std::string name = "tex" + std::to_string((i));
				glUniform1i(glGetUniformLocation(shader->GetProgram(), name.c_str()), i);

				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, textures[i]);
			}
		}
	}
}


void SceneNode::Update(float dt) {
	if (parent) {
		worldTransform = parent->worldTransform * transform;
	}
	else {
		worldTransform = transform;
	}
	for (vector<SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) {
		(*i)->Update(dt);
	}
}
