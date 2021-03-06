#ifndef _CORE_APPLICATION_H_
#define _CORE_APPLICATION_H_

#include <cstdlib>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Platform.h"

struct GLFWwindow;
struct ImGuiContext;

namespace core
{

	class Application
	{
	private:
		std::string mGLSLVersion;
		GLFWwindow *mGLFWWindow;
		uint32_t mWidth;
		uint32_t mHeight;

		// ImGuiContext* mImGuiContext;

		static float mFPS;

		bool mIsSync;

		double Time;

		Application(Application &app) = delete;
		Application &operator=(Application &app) = delete;

	protected:
		bool mIsInitialized;

		glm::vec3 _headPos;
		glm::quat _headRot;
		glm::mat4 _headMat;

		int _controllerCount;

		int _viewCount;

		glm::vec4 _viewports[2];
		glm::mat4 _viewMatrices[2];
		glm::mat4 _projectionMatrices[2];

		glm::vec3 _controllersPos[2];
		glm::quat _controllersRot[2];

		glm::vec3 _rightContPos;
		glm::quat _rightContRot;

		Application();
		virtual ~Application();

		uint32_t GetWidth() const { return mWidth; }
		uint32_t GetHeight() const { return mHeight; }

		virtual void OnMouseLeftDown(double x, double y) {}
		virtual void OnMouseLeftUp(double x, double y) {}
		virtual void OnMouseRightDown(double x, double y) {}
		virtual void OnMouseRightUp(double x, double y) {}
		virtual void OnMouseMiddleDown(double x, double y) {}
		virtual void OnMouseMiddleUp(double x, double y) {}
		virtual void OnMouseMove(double x, double y) {}
		virtual void OnMouseWhell(double offset) {}
		virtual void OnKeyPress(int key) {}
		virtual void OnKeyRepeat(int key) {}
		virtual void OnKeyRelease(int key) {}
		virtual void OnResize(int width, int height) {}

	public:
		int Run(int argc, char **argv, const char *version, uint32_t width, uint32_t height, const char *title);

		static float GetFPS();

		// ImGuiContext* GetMainImGuiContext() { return mImGuiContext; }

		virtual bool OnInitialize() { return true; }
		virtual bool OnGUICompose() { return true; }
		virtual bool OnRender(int x, int y, int width, int height, const glm::mat4 &view, const glm::mat4 &proj) { return true; }
		virtual bool OnGUI(int x, int y, int width, int height, const glm::mat4 &view, const glm::mat4 &proj) { return true; }
		virtual bool OnFinalize() { return true; }

		static void OnUpdate(void *_arg);
#ifdef __EMSCRIPTEN__
		int ResizeGLFWWindow();
#endif

#ifdef __EMSCRIPTEN__
		static EM_BOOL emscripten_window_resized_callback(int eventType, const void *reserved, void *userData);
#endif
	private:
		static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
		static void MouseMoveCallback(GLFWwindow *window, double xpos, double ypos);
		static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
		static void MouseScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
		static void FramebufferSizeCallback(GLFWwindow *window, int width, int height);
		static void WindowSizeCallback(GLFWwindow *window, int width, int height);
	};

}

#endif