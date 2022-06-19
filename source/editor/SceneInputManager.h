#pragma once
#include "function/inputSystem/InputSystem.h"
#include "function/render/RenderSystem.h"

namespace tiny
{
	enum class EDITORCOMMNAD : uint32_t
	{
		CAMERA_LEFT = 1 << 0,		// A
		CAMERA_BACK = 1 << 1,		// S
		CAMERA_FORWARD = 1 << 2,	// W
		CAMERA_RIGHT = 1 << 3,		// D
		CAMERA_UP = 1 << 4,			// E
		CAMERA_DOWN = 1 << 5,		// Q
	};

	class SceneInputManager : public tiny::inputSystem
	{
	public:
		void initialize();
		void tick(float deltaTime);

	private:
		virtual void onKey(int key, int scancode, int action, int mods);
		virtual void onMouseButton(int button, int action, int mods);
		virtual void onCursorPos(float xpos, float ypos);

		void processKeyInput();
	private:
		std::shared_ptr<tiny::RenderSystem> mRenderSystem;
		uint32_t mEditorCommand;

		bool mEditorMouseButtonRight;
		glm::vec2 mLastMousePos;
	};
}


