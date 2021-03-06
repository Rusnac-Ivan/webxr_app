#include "Application.h"

#include <utilities/Emsc/webxr.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <widgets3d/ImGui_Impl_2d_to_3d.h>
#include "helvetica_font.inl"
#include <utilities/Emsc/webxr.h>
#include <opengl/Render.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <opengl/Program.h>
#include <webxr.h>
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

#ifdef __EMSCRIPTEN__
		webxr_request_session(WEBXR_SESSION_MODE_IMMERSIVE_VR, WEBXR_SESSION_FEATURE_LOCAL_FLOOR, WEBXR_SESSION_FEATURE_HIT_TEST);
#endif
	}

	EM_BOOL Application::emscripten_window_resized_callback(int eventType, const void *reserved, void *userData)
	{
		int width, height;
		if (emscripten_get_canvas_element_size(".emscripten", &width, &height) != EMSCRIPTEN_RESULT_SUCCESS)
		{
			fprintf(stderr, "unsupported emscripten_get_canvas_element_size!\n");
		}

		std::string msg("emscripten_window_resized_callback: " + std::to_string(width) + "x" + std::to_string(height));
		emscripten::val::global("console").call<void>("log", msg);

		// resize window
		GLFWwindow *window = (GLFWwindow *)userData;
		glfwSetWindowSize(window, width, height);

		Application::WindowSizeCallback(window, width, height);

		return true;
	}

	Application::Application() : mIsSync(false), mIsInitialized(false)
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

	static void StyleColors()
	{
		ImGuiStyle &style = ImGui::GetStyle();

		style.Alpha = 1.0;
		style.FrameBorderSize = 1.f;
		style.WindowRounding = 10.f;
		style.GrabRounding = 1.f;
		style.GrabMinSize = 20.f;
		style.FrameRounding = 10.f;

		ImVec4 defaultColor = ImVec4(0.96f, 0.96f, 0.96f, 0.7f);
		ImVec4 hoverColor = ImVec4(0.96f, 0.96f, 0.96f, 0.8f);
		ImVec4 activeColor = ImVec4(0.96f, 0.96f, 0.96f, 0.9f);

		ImVec4 bgColor = ImVec4(0.96f, 0.96f, 0.96f, 0.5f);
		ImVec4 textColor = ImVec4(0.f, 0.f, 0.f, 1.f);
		ImVec4 titleBgColor = ImVec4(0.96f, 0.96f, 0.96f, 0.7f);
		ImVec4 titleBgCollapsedColor = ImVec4(0.96f, 0.96f, 0.96f, 0.5f);
		ImVec4 titleBgActiveColor = ImVec4(0.96f, 0.96f, 0.96f, 0.8f);

		style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.00f, 0.40f, 0.41f, 1.00f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.1f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 1.00f, 1.00f, 0.65f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_ChildBg] = ImVec4(0.44f, 0.80f, 0.80f, 0.5f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.44f, 0.80f, 0.80f, 0.5f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.44f, 0.80f, 0.80f, 0.27f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.44f, 0.81f, 0.86f, 0.66f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.18f, 0.21f, 0.73f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.27f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.22f, 0.29f, 0.30f, 0.71f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.00f, 1.00f, 1.00f, 0.44f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 1.00f, 1.00f, 0.68f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 1.00f, 1.00f, 0.36f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.76f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.00f, 0.65f, 0.65f, 0.50f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.01f, 1.00f, 1.00f, 0.43f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.70f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.00f, 1.00f, 1.00f, 0.33f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.42f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 1.00f, 1.00f, 0.22f);
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

		webxr_init(
			/* Frame callback */
			[](void *userData, int time, WebXRRigidTransform *headPose, WebXRView views[2], int viewCount)
			{
				Application *thiz = reinterpret_cast<Application *>(userData);

				if (headPose)
				{
					thiz->_headPos = glm::vec3(headPose->position[0], headPose->position[1], headPose->position[2]);
					thiz->_headRot = glm::quat(headPose->orientation[3], headPose->orientation[0], headPose->orientation[1], headPose->orientation[2]);
				}

				thiz->_viewCount = viewCount;

				int viewIndex = 0;
				for (WebXRView view : {views[0], views[1]})
				{
					thiz->_viewports[viewIndex] = {view.viewport[0], view.viewport[1], view.viewport[2], view.viewport[3]};

					glm::mat4 translate = glm::translate(glm::mat4(1.), glm::vec3(view.viewPose.position[0], view.viewPose.position[1], view.viewPose.position[2]));
					glm::mat4 rotation = glm::toMat4(glm::quat(view.viewPose.orientation[3], view.viewPose.orientation[0], view.viewPose.orientation[1], view.viewPose.orientation[2]));

					thiz->_viewMatrices[viewIndex] = glm::inverse(translate * rotation);

					thiz->_projectionMatrices[viewIndex] = (glm::make_mat4(view.projectionMatrix));
					++viewIndex;
				}

				constexpr int maxInputCount = 2;
				WebXRInputSource sources[maxInputCount];
				WebXRRigidTransform controllersPose[maxInputCount];

				webxr_get_input_sources(sources, maxInputCount, &thiz->_controllerCount);

				for (int i = 0; i < thiz->_controllerCount; ++i)
				{
					webxr_get_input_pose(sources + i, controllersPose + i, WEBXR_INPUT_POSE_GRIP);

					switch (sources[i].targetRayMode)
					{
					case WEBXR_TARGET_RAY_MODE_GAZE:
						printf("WEBXR_TARGET_RAY_MODE_GAZE\n");
						break;
					case WEBXR_TARGET_RAY_MODE_TRACKED_POINTER:
						printf("WEBXR_TARGET_RAY_MODE_TRACKED_POINTER\n");
						break;
					case WEBXR_TARGET_RAY_MODE_SCREEN:
						printf("WEBXR_TARGET_RAY_MODE_SCREEN\n");
						break;
					default:
						break;
					}

					thiz->_controllersPos[i] = glm::vec3(controllersPose[i].position[0], controllersPose[i].position[1], controllersPose[i].position[2]);

					thiz->_controllersRot[i] = glm::quat(controllersPose[i].orientation[3], controllersPose[i].orientation[0], controllersPose[i].orientation[1], controllersPose[i].orientation[2]);

					if (sources[i].handedness == WEBXR_HANDEDNESS_RIGHT)
					{
						thiz->_rightContPos = thiz->_controllersPos[i];
						thiz->_rightContRot = thiz->_controllersRot[i];
					}
				}

				Application::OnUpdate(thiz);
			},
			/* Session start callback */
			[](void *userData, int mode)
			{
				Application *thiz = reinterpret_cast<Application *>(userData);

				webxr_set_projection_params(0.01f, 100.f);

				// Scene Initialize
				thiz->OnInitialize();

				printf("webxr_init: Session start callback\n");

				webxr_set_select_start_callback([](WebXRInputSource *inputSource, void *userData)
												{ 
								printf("select_start_callback\n"); 
								ImGui_Impl_2d_to_3d_MouseButtonCallback(MOUSEBUTTON_LEFT, PRESS, 0); },
												userData);

				webxr_set_select_end_callback([](WebXRInputSource *inputSource, void *userData)
											  { 
								printf("select_end_callback\n");
								ImGui_Impl_2d_to_3d_MouseButtonCallback(MOUSEBUTTON_LEFT, RELEASE, 0); },
											  userData);
			},
			/* Session end callback */
			[](void *userData, int mode)
			{
				Application *thiz = reinterpret_cast<Application *>(userData);
				thiz->OnFinalize();
				printf("webxr_init: Session end callback\n");
			},
			/* Error callback */
			[](void *userData, int error)
			{
				printf("webxr_init: Errord callback\n");
			},
			/* userData */
			this);

#ifdef __EMSCRIPTEN__
		int canv_width, canv_height;
		if (emscripten_get_canvas_element_size(".emscripten", &canv_width, &canv_height) != EMSCRIPTEN_RESULT_SUCCESS)
			return EXIT_FAILURE;

		mWidth = canv_width;
		mHeight = canv_height;
#endif // __EMSCRIPTEN__

		webxr_is_session_supported(WEBXR_SESSION_MODE_IMMERSIVE_VR, [](int mode, int supported)
								   {
		printf("mode: %d, supported: %d\n", mode, supported);
		if ((mode == WEBXR_SESSION_MODE_IMMERSIVE_VR) && (supported))
		{
			
							// webxr_request_session(WEBXR_SESSION_MODE_IMMERSIVE_VR, WEBXR_SESSION_FEATURE_LOCAL, WEBXR_SESSION_FEATURE_LOCAL);
		} });

		// Create window with graphics context
		mGLFWWindow = glfwCreateWindow(mWidth, mHeight, title, NULL, NULL);
		if (mGLFWWindow == NULL)
			return EXIT_FAILURE;

		EmscriptenFullscreenStrategy strategy;
		strategy.scaleMode = EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_STDDEF;
		strategy.filteringMode = EMSCRIPTEN_FULLSCREEN_FILTERING_DEFAULT;
		strategy.canvasResizedCallback = Application::emscripten_window_resized_callback;
		strategy.canvasResizedCallbackUserData = mGLFWWindow; // pointer to user data
		emscripten_enter_soft_fullscreen("canvas", &strategy);

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

		// ImGui::StyleColorsDark();
		ImGui::StyleColorsClassic();
		// StyleColors();

		ImFontConfig imFontConf = {};
		imFontConf.FontDataOwnedByAtlas = false;
		ImFont *font = io.Fonts->AddFontFromMemoryTTF(__Helvetica_ttf, __Helvetica_ttf_len, 17.f, &imFontConf);

		// ImGui::MergeIconsWithLatestFont(17.f, false);
		// io.Fonts->AddFontDefault(&imFontConf);
		// io.Fonts->AddFontDefault(&imFontConf);

		// Setup Platform/Renderer backends
		// ImGui_ImplGlfw_InitForOpenGL(mGLFWWindow, true);
		ImGui_Impl_2d_to_3d_InitForOpenGL();
		ImGui_ImplOpenGL3_Init(GLSL_VERSION);

		ImGui_ImplOpenGL3_NewFrame();

#ifdef __EMSCRIPTEN__
		mIsSync = true;
		/*if (WebXR::IsWebXRSupported())
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
		}*/
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

		if (!app->mIsInitialized)
			return;

		printf("Application::OnUpdate\n");

		{ // calc FPS
			static double curr = 0.f;
			static double prev = curr;
			curr = glfwGetTime();
			Application::mFPS = 1.0 / (curr - prev);
			prev = curr;
		}

		if (!app->mIsSync)
		{
			glfwSwapInterval(1); // Enable vsync
			app->mIsSync = true;
		}

#ifdef __EMSCRIPTEN__
		/*int width, height;
		if (emscripten_get_canvas_element_size(".emscripten", &width, &height) != EMSCRIPTEN_RESULT_SUCCESS)
		{
			fprintf(stderr, "unsupported emscripten_get_canvas_element_size!\n");
			// return;
		}

		if (width != app->mWidth || (height != app->mHeight && height != 0))
		{
			app->mWidth = width;
			app->mHeight = height;
			glfwSetWindowSize(app->mGLFWWindow, width, height);
			app->OnResize(width, height);
		}*/
#endif

		/*{ // compose gui menus

			// Setup time step
			ImGuiIO &io = ImGui::GetIO();
			double current_time = glfwGetTime();
			io.DeltaTime = app->Time > 0.0 ? (float)(current_time - app->Time) : (float)(1.0f / 60.0f);
			app->Time = current_time;

			app->OnGUICompose();
		}*/

		{ // render webxr viewes
			gl::Render::SetClearColor(0.f, 0.f, 0.4f, 1.f);
			gl::Render::Clear(gl::BufferBit::COLOR, gl::BufferBit::DEPTH);

			// WebXRRigidTransform &headPose = WebXR::GetHeadPose();

			{ // new webxr not work on oculus
			  //  WebXRView *viewArray;
			  //  uint32_t viewCount;
			  //  if (!WebXR::GetViews(&viewArray, &viewCount))
			  //  return;
			}

			/*static glm::vec4 view_ports[2];
			view_ports[0] = glm::vec4(0.f, 0.f, app->GetWidth() / 2.f, app->GetHeight());
			view_ports[1] = glm::vec4(app->GetWidth() / 2.f, 0.f, app->GetWidth() / 2.f, app->GetHeight());
			static glm::mat4 proj_mats[2];
			proj_mats[0] = glm::perspective(glm::radians(60.f), view_ports[0].z / view_ports[0].w, 0.01f, 100.f);
			proj_mats[1] = glm::perspective(glm::radians(60.f), view_ports[1].z / view_ports[1].w, 0.01f, 100.f);*/

			for (uint32_t i = 0; i < app->_viewCount; i++)
			{
				// WebXRView &view = viewArray[i];

				{ // render scene
					// app->OnRender(view.viewport.x, view.viewport.y, view.viewport.width, view.viewport.height, view.viewPose.matrix, view.projectionMatrix);
					app->OnRender(app->_viewports[i].x, app->_viewports[i].y, app->_viewports[i].z, app->_viewports[i].w, app->_viewMatrices[i], app->_projectionMatrices[i]);
					// app->OnRender(view_ports[i].x, view_ports[i].y, view_ports[i].z, view_ports[i].w, glm::mat4(1.f), proj_mats[i]);
				}

				/*{ // render surface gui
					ImGui_Impl_2d_to_3d_NewFrame(ImVec2(width, height), ImVec2(-1.f, -1.f));
					ImGui::NewFrame();

					ImGuiContext &g = *GImGui;

					ImGuiViewportP *main_viewport = g.Viewports[0];
					main_viewport->Flags = ImGuiViewportFlags_IsPlatformWindow | ImGuiViewportFlags_OwnedByApp;
					main_viewport->Pos = ImVec2(view.viewport.x, view.viewport.y);
					main_viewport->Size = ImVec2(view.viewport.width, view.viewport.height);
					for (int n = 0; n < g.Viewports.Size; n++)
					{
						ImGuiViewportP *viewport = g.Viewports[n];
						// Lock down space taken by menu bars and status bars, reset the offset for fucntions like BeginMainMenuBar() to alter them again.
						viewport->WorkOffsetMin = ImVec2(0.f, 0.f);
						viewport->WorkOffsetMax = ImVec2(0.f, 0.f);
						viewport->BuildWorkOffsetMin = viewport->BuildWorkOffsetMax = ImVec2(0.0f, 0.0f);
						viewport->UpdateWorkRect();
					}

					g.ActiveIdNoClearOnFocusLoss = true;
					ImGuizmo::BeginFrame();

					char win_name[8] = {};
					sprintf(win_name, "gui_%d", i);

					ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
					ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
					ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);

					ImGui::SetNextWindowPos(ImVec2(view.viewport.x, view.viewport.y), ImGuiCond_Always);
					ImGui::SetNextWindowSize(ImVec2(view.viewport.width, view.viewport.height), ImGuiCond_Always);

					ImGui::Begin(win_name, nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
					{
						app->OnGUI(view.viewport.x, view.viewport.y, view.viewport.width, view.viewport.height, view.viewPose.matrix, view.projectionMatrix);
						// app->OnGUI(view_ports[i].x, view_ports[i].y, view_ports[i].z, view_ports[i].w, glm::mat4(1.f), proj_mats[i]);
					}
					ImGui::End();

					ImGui::PopStyleVar(3);

					ImGui::Render();
					ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
				}*/
			}
			gl::Program::StopUsing();
			gl::Render::Flush();
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