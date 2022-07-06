#include "Application.h"

#include "Platform.h"
#include <utilities/Emsc/webxr.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <widgets3d/ImGui_Impl_2d_to_3d.h>
#include "helvetica_font.inl"
//#include "roboto_medium.inl"

namespace core
{
	float Application::mFPS = 0.f;
	static Application *thiz = nullptr;

	float Application::GetFPS() { return mFPS; }

	void Application::KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (action == GLFW_PRESS)
			thiz->OnKeyPress(key);
		if (action == GLFW_REPEAT)
			thiz->OnKeyRepeat(key);
		if (action == GLFW_RELEASE)
			thiz->OnKeyRelease(key);
	}

	void Application::MouseMoveCallback(GLFWwindow *window, double xpos, double ypos)
	{
		thiz->OnMouseMove(xpos, ypos);
	}

	void Application::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		if (action == GLFW_PRESS)
		{
			if (button == GLFW_MOUSE_BUTTON_LEFT)
			{
				thiz->OnMouseLeftDown(xpos, ypos);
			}
			else if (button == GLFW_MOUSE_BUTTON_RIGHT)
			{
				thiz->OnMouseRightDown(xpos, ypos);
			}
			else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
			{
				thiz->OnMouseMiddleDown(xpos, ypos);
			}
		}
		else if (action == GLFW_RELEASE)
		{
			if (button == GLFW_MOUSE_BUTTON_LEFT)
			{
				thiz->OnMouseLeftUp(xpos, ypos);
			}
			else if (button == GLFW_MOUSE_BUTTON_RIGHT)
			{
				thiz->OnMouseRightUp(xpos, ypos);
			}
			else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
			{
				thiz->OnMouseMiddleUp(xpos, ypos);
			}
		}
	}

	void Application::MouseScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
	{
		thiz->OnMouseWhell(yoffset);
	}

	void Application::FramebufferSizeCallback(GLFWwindow *window, int width, int height)
	{
	}

	void Application::WindowSizeCallback(GLFWwindow *window, int width, int height)
	{
		thiz->mWidth = width;
		thiz->mHeight = height;
		thiz->OnResize(width, height);
	}

	Application::Application() : mIsSync(false)
	{
		thiz = this;
		mFPS = 0.f;
		Time = 0.f;
	}

	Application::~Application()
	{
		// Cleanup imgui
		ImGui_ImplOpenGL3_Shutdown();
		// ImGui_ImplGlfw_Shutdown();
		ImGui_Impl_2d_to_3d_Shutdown();
		ImGui::DestroyContext();

		// Destroy glfw
		glfwDestroyWindow(mGLFWWindow);
		glfwTerminate();
	}

	int Application::Run(int argc, char **argv, const char *version, uint32_t width, uint32_t height, const char *title)
	{
		mWidth = width;
		mHeight = height;

		if (!glfwInit())
			return EXIT_FAILURE;

			// Decide GL+GLSL versions
#ifdef __APPLE__
		// GL 3.2 + GLSL 150
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);		   // Required on Mac
#elif __EMSCRIPTEN__
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#else
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

		// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_SAMPLES, 8);

		glfwWindowHint(GLFW_RESIZABLE, 1);

#ifdef __EMSCRIPTEN__
		int canv_width, canv_height;
		if (emscripten_get_canvas_element_size(".emscripten", &canv_width, &canv_height) != EMSCRIPTEN_RESULT_SUCCESS)
			return EXIT_FAILURE;

		mWidth = canv_width;
		mHeight = canv_height;
#endif // __EMSCRIPTEN__

		// Create window with graphics context
		mGLFWWindow = glfwCreateWindow(mWidth, mHeight, title, NULL, NULL);
		if (mGLFWWindow == NULL)
			return EXIT_FAILURE;

		glfwMakeContextCurrent(mGLFWWindow);

#ifndef __EMSCRIPTEN__
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			fprintf(stderr, "Failed to initialize GLAD!\n");
			exit(1);
		}
#endif

#ifndef __EMSCRIPTEN__
		if (glfwRawMouseMotionSupported())
			glfwSetInputMode(mGLFWWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
#endif

		glfwSetKeyCallback(mGLFWWindow, Application::KeyCallback);
		glfwSetCursorPosCallback(mGLFWWindow, Application::MouseMoveCallback);
		glfwSetMouseButtonCallback(mGLFWWindow, Application::MouseButtonCallback);
		glfwSetScrollCallback(mGLFWWindow, Application::MouseScrollCallback);
#ifndef __EMSCRIPTEN__
		glfwSetFramebufferSizeCallback(mGLFWWindow, Application::FramebufferSizeCallback);
		glfwSetWindowSizeCallback(mGLFWWindow, Application::WindowSizeCallback);
#endif

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();
		(void)io;
		// io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		// ImGui::StyleColorsClassic();

		ImFontConfig imFontConf = {};
		imFontConf.FontDataOwnedByAtlas = false;

		// ImFont* font = io.Fonts->AddFontFromMemoryTTF(__roboto_medium_ttf, __roboto_medium_ttf_len, 17.f, &imFontConf);
		ImFont *font = io.Fonts->AddFontFromMemoryTTF(__Helvetica_ttf, __Helvetica_ttf_len, 32.f, &imFontConf);

		// ImGui::MergeIconsWithLatestFont(17.f, false);
		// io.Fonts->AddFontDefault(&imFontConf);
		// io.Fonts->AddFontDefault(&imFontConf);

		// Setup Platform/Renderer backends
		// ImGui_ImplGlfw_InitForOpenGL(mGLFWWindow, true);
		ImGui_Impl_2d_to_3d_InitForOpenGL();
		ImGui_ImplOpenGL3_Init(GLSL_VERSION);

		ImGui_ImplOpenGL3_NewFrame();

#ifdef __EMSCRIPTEN__
		if (WebXR::IsWebXRSupported())
		{
			printf("WebXR Supported\n");
			mIsSync = true; // from webxr
			WebXR::Start(this);
		}
		else
		{
			printf("WebXR Not Supported\n");
			// Initialize sample.
			bool success = OnInitialize();
			if (!success)
				return EXIT_FAILURE;

			if (ResizeGLFWWindow() == EXIT_FAILURE)
				return EXIT_FAILURE;

			emscripten_set_main_loop_arg(Application::OnUpdate, (void *)this, 0, 1);

			success = OnFinalize();
			if (!success)
				return EXIT_FAILURE;
		}
#else
		// Initialize sample.
		bool success = OnInitialize();
		if (!success)
			return EXIT_FAILURE;
		OnResize(mWidth, mHeight);
		while (!glfwWindowShouldClose(mGLFWWindow))
		{
			OnUpdate(this);
		}
		success = OnFinalize();
		if (!success)
			return EXIT_FAILURE;
#endif
		return EXIT_SUCCESS;
	}

	void Application::OnUpdate(void *_arg)
	{
		Application *app = reinterpret_cast<Application *>(_arg);

		static double curr = 0.f;
		static double prev = curr;
		curr = glfwGetTime();
		Application::mFPS = 1.0 / (curr - prev);
		prev = curr;

		if (!app->mIsSync)
		{
			glfwSwapInterval(1); // Enable vsync
			app->mIsSync = true;
		}

#ifdef __EMSCRIPTEN__
		int width, height;
		if (emscripten_get_canvas_element_size(".emscripten", &width, &height) != EMSCRIPTEN_RESULT_SUCCESS)
		{
			fprintf(stderr, "unsupported emscripten_get_canvas_element_size!\n");
			return;
		}

		if (width != app->mWidth || (height != app->mHeight && height != 0))
		{
			app->mWidth = width;
			app->mHeight = height;
			glfwSetWindowSize(app->mGLFWWindow, width, height);
			app->OnResize(width, height);
		}
#endif

		{ // render scene
			app->OnRender();
		}

		{ // render menu

			// Setup time step
			ImGuiIO &io = ImGui::GetIO();
			double current_time = glfwGetTime();
			io.DeltaTime = app->Time > 0.0 ? (float)(current_time - app->Time) : (float)(1.0f / 60.0f);
			app->Time = current_time;

			app->OnGui();
		}

		glfwSwapBuffers(app->mGLFWWindow);
		glfwPollEvents();
	}

#ifdef __EMSCRIPTEN__
	int Application::ResizeGLFWWindow()
	{

		int canv_width, canv_height;
		if (emscripten_get_canvas_element_size(".emscripten", &canv_width, &canv_height) != EMSCRIPTEN_RESULT_SUCCESS)
			return EXIT_FAILURE;

		mWidth = canv_width;
		mHeight = canv_height;

		printf("canvas_element_size %dx%d\n", canv_width, canv_height);

		glfwSetWindowSize(mGLFWWindow, canv_width, canv_height);
		OnResize(canv_width, canv_height);

		return EXIT_SUCCESS;
	}
#endif
}