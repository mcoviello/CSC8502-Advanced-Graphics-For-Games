#include "../nclgl/window.h"
#include "Renderer.h"

int main() {
	Window w("Vertex Transformation!", 800, 600, false);
	if (!w.HasInitialised()) {
		return -1;
	}

	Renderer renderer(w);
	if (!renderer.HasInitialised()) {
		return -1;
	}

	float scale = 100.0f;
	float rotation = 0.0f;
	Vector3 position(0, 0, -1500.0f);

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_1))
			renderer.SwitchToOrthographic();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_2))
			renderer.SwitchToPerspective();

		renderer.SetRotation(rotation);
		renderer.SetScale(scale);
		renderer.SetPosition(position);
		renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds());
		renderer.RenderScene();
		renderer.SwapBuffers();
	}

	return 0;
}