#include "CubeRobot.h"

CubeRobot::CubeRobot(Mesh* cube) {
	SceneNode* body = new SceneNode(cube, Vector4(1, 0, 0, 1));
	body->SetModelScale(Vector3(10, 15, 5));
	body->SetTransfom(Matrix4::Translation(Vector3(0, 35, 0)));
	AddChild(body);

	head = new SceneNode(cube, Vector4(0, 1, 0, 1));
	body->SetModelScale(Vector3(5, 5, 5));
	body->SetTransfom(Matrix4::Translation(Vector3(0, 35, 0)));
	body->AddChild(head);

	leftArm = new SceneNode(cube, Vector4(0, 0, 1, 1));
	body->SetModelScale(Vector3(3, -18, 3));
	body->SetTransfom(Matrix4::Translation(Vector3(-12,30,-1)));
	body->AddChild(leftArm);

	rightArm = new SceneNode(cube, Vector4(0, 0, 1, 1));
	body->SetModelScale(Vector3(3, -18, 3));
	body->SetTransfom(Matrix4::Translation(Vector3(12,30,-1)));
	body->AddChild(rightArm);

	SceneNode* leftLeg = new SceneNode(cube, Vector4(0, 0, 1, 1));
	body->SetModelScale(Vector3(3,-17,5.3f));
	body->SetTransfom(Matrix4::Translation(Vector3(-8,0,0)));
	body->AddChild(leftLeg);

	SceneNode* rightLeg = new SceneNode(cube, Vector4(0, 0, 1, 1));
	body->SetModelScale(Vector3(3,-17,5.3f));
	body->SetTransfom(Matrix4::Translation(Vector3(8,0,0)));
	body->AddChild(rightLeg);
}

void CubeRobot::Update(float dt) {
	transform = transform * Matrix4::Rotation(30.0f * dt, Vector3(0, 1, 0));
	head->SetTransfom(head->GetTransform() * Matrix4::Rotation(-30.0f * dt, Vector3(0, 1, 0)));
	leftArm->SetTransfom(leftArm->GetTransform() * Matrix4::Rotation(-30.0f * dt, Vector3(1, 0, 0)));
	rightArm->SetTransfom(rightArm->GetTransform() * Matrix4::Rotation(-30.0f * dt, Vector3(1, 0, 0)));
	SceneNode::Update(dt);
}
