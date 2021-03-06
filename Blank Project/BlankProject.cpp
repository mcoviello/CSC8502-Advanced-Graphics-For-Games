#include "../NCLGL/window.h"
#include "Renderer.h"

int main()	{
	Window w("Eventide Island Scene", 1280, 720, false);

	if(!w.HasInitialised()) {
		return -1;
	}
	
	Renderer renderer(w);
	if(!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while(w.UpdateWindow()  && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		w.SetTitle(std::to_string(1 / w.GetTimer()->GetTimeDeltaSeconds()));
		renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds());
		renderer.RenderScene();
		renderer.SwapBuffers();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5))
			Shader::ReloadAllShaders();
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_C))
			renderer.ToggleFreecam();
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_F))
			renderer.ToggleCelShading();

	}
	return 0;
}