#include <iostream>
#include <chrono>

#include <imgui.h>
#include <imgui_internal.h>

#include <core/Application.h>
#include <opengl/Render.h>
#include <opengl/Pipeline.h>
#include <utilities/Emsc/webxr.h>
#include <utilities/Resources/ResourceManager.h>
#include <utilities/Camera/Camera.h>
#include <utilities/Controller/Controller.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <widgets3d/ImGui_Impl_2d_to_3d.h>

class MyApp : public core::Application
{
private:
	struct UniformLocations
	{
		int32_t pbr_prog_proj_view = -1;
		int32_t cubemap_prog_proj_view = -1;
		int32_t ray_prog_proj_view = -1;
		int32_t menu_prog_proj_view = -1;
		int32_t light_map_prog_proj_view = -1;

		int32_t pbr_prog_ligh_pos = -1;
		int32_t pbr_prog_view_pos = -1;

		
	};
	bool is_init_uniforms = false;
	UniformLocations mUniformLocations;

	glm::mat4 mMenuModel;

	WebXRInputSource *mInputSource;
	util::Camera<util::ProjectionType::PERSPECTIVE> mCamera;
	std::vector<float> mFPS;
	MyApp(MyApp &app) = delete;
	MyApp &operator=(MyApp &app) = delete;
	MyApp() : mInputSource(nullptr), Application() {}
	~MyApp() {}

public:
	static MyApp &Launch()
	{
		static MyApp app;
		return app;
	}

	virtual bool OnInitialize()
	{
		mCamera.SetViewState(glm::vec3(0.f, 0.5f, 1.f), glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, -1.f));

		printf("MyApp::OnInitialize\n");
		util::ResourceManager::OnInitialize();

		gl::Pipeline::SetBlendFunc(gl::ComputOption::SRC_ALPHA, gl::ComputOption::ONE_MINUS_SRC_ALPHA);

		gl::Pipeline::EnableDepthTest();
		gl::Render::SetClearColor(0.5f, 0.4f, 0.0f, 1.f);

		return true;
	};
	virtual bool OnGui()
	{
		util::ResourceManager::GetW3DVideo2D()->Compose(mInputSource, glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.3f, -1.f)));

		/*ImGui_Impl_2d_to_3d_NewFrame(ImVec2(GetWidth(), GetHeight()), ImVec2(-1.f, -1.f));
		ImGui::NewFrame();

		ImGui::SetNextWindowPos(ImVec2(0.f, 0.f), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(300.f, 300.f), ImGuiCond_Always);
		ImGui::Begin("Other Context", nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration);
		{
			// ImGui::PushFont(mDefaultFont);
			static float FPS[100] = {};

			float fps = GetFPS();

			float average_fps = 0.f;
			for (uint32_t i = 1; i < 100; i++)
			{
				FPS[i - 1] = FPS[i];
				FPS[99] = fps;
				average_fps += FPS[i] / 100.f;
			}

			if (average_fps > 50.f)
			{
				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.7f, 0.f, 0.5f));
			}
			else if (average_fps > 20.f)
			{
				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.7f, 0.7f, 0.f, 0.5f));
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.7f, 0.f, 0.f, 0.5f));
			}

			char text[128] = {};

			sprintf(text, "FPS: %.3f", average_fps);
			ImGui::Text("%-64s", text);

			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 30.f);
			ImGui::PlotLines("##Frame Times", &FPS[0], 100);

			ImGui::PopStyleColor();
		}
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());*/

		mMenuModel = glm::translate(glm::mat4(1.f), glm::vec3(1.f, 1.6f, 0.f));
		mMenuModel = glm::rotate(mMenuModel, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
		util::ResourceManager::GetW3DMenu()->Compose(mInputSource, mMenuModel, "3DMenu",
			[]()
			{
				if (ImGui::Button("Ok", ImVec2(90.f, 30.f)))
				{
					printf("Button Ok\n");
				}
				if (ImGui::Button("Save", ImVec2(90.f, 30.f)))
				{
					printf("Button Save\n");
				}
				if (ImGui::Button("Cancel", ImVec2(90.f, 30.f)))
				{
					printf("Button Cancel\n");
				}
				static float slider = 0.5f;
				ImGui::SliderFloat("Speed", &slider, 0.f, 1.f, "%.3f");

				static char buff[64] = {};
				ImGui::InputText("in text", buff, IM_ARRAYSIZE(buff));

				static bool animate = true;
				ImGui::Checkbox("Animate", &animate);

				static float arr[] = {0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f};
				ImGui::PlotLines("Line", arr, IM_ARRAYSIZE(arr));

				static float values[90] = {};
				static int values_offset = 0;
				static double refresh_time = 0.0;
				if (!animate || refresh_time == 0.0)
					refresh_time = ImGui::GetTime();
				while (refresh_time < ImGui::GetTime()) // Create data at fixed 60 Hz rate for the demo
				{
					static float phase = 0.0f;
					values[values_offset] = cosf(phase);
					values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
					phase += 0.10f * values_offset;
					refresh_time += 1.0f / 60.0f;
				}

				{
					float average = 0.0f;
					for (int n = 0; n < IM_ARRAYSIZE(values); n++)
						average += values[n];
					average /= (float)IM_ARRAYSIZE(values);
					char overlay[32];
					sprintf(overlay, "avg %f", average);
					ImGui::PlotLines("Lines", values, IM_ARRAYSIZE(values), values_offset, overlay, -1.0f, 1.0f, ImVec2(0, 80.0f));
				}
				ImGui::PlotHistogram("Histogram", arr, IM_ARRAYSIZE(arr), 0, NULL, 0.0f, 1.0f, ImVec2(0, 80.0f));
			}
		);

		glm::mat4 model = glm::translate(glm::mat4(1.f), glm::vec3(-1.f, 1.6f, 0.f));
		model = glm::rotate(model, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
		util::ResourceManager::GetW3DMenu1()->Compose(mInputSource, model, "Second 3DMenu",
			[]()
			{
				if (ImGui::Button("Ok", ImVec2(90.f, 30.f)))
				{
					printf("Button Ok\n");
				}
				if (ImGui::Button("Save", ImVec2(90.f, 30.f)))
				{
					printf("Button Save\n");
				}
				if (ImGui::Button("Cancel", ImVec2(90.f, 30.f)))
				{
					printf("Button Cancel\n");
				}
				static float slider = 0.5f;
				ImGui::SliderFloat("Speed", &slider, 0.f, 1.f, "%.3f");

				static char buff[64] = {};
				ImGui::InputText("in text", buff, IM_ARRAYSIZE(buff));

				static bool animate = true;
				ImGui::Checkbox("Animate", &animate);

				static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
				ImGui::PlotLines("Line", arr, IM_ARRAYSIZE(arr));

				static float values[90] = {};
				static int values_offset = 0;
				static double refresh_time = 0.0;
				if (!animate || refresh_time == 0.0)
					refresh_time = ImGui::GetTime();
				while (refresh_time < ImGui::GetTime()) // Create data at fixed 60 Hz rate for the demo
				{
					static float phase = 0.0f;
					values[values_offset] = cosf(phase);
					values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
					phase += 0.10f * values_offset;
					refresh_time += 1.0f / 60.0f;
				}

				{
					float average = 0.0f;
					for (int n = 0; n < IM_ARRAYSIZE(values); n++)
						average += values[n];
					average /= (float)IM_ARRAYSIZE(values);
					char overlay[32];
					sprintf(overlay, "avg %f", average);
					ImGui::PlotLines("Lines", values, IM_ARRAYSIZE(values), values_offset, overlay, -1.0f, 1.0f, ImVec2(0, 80.0f));
				}
				ImGui::PlotHistogram("Histogram", arr, IM_ARRAYSIZE(arr), 0, NULL, 0.0f, 1.0f, ImVec2(0, 80.0f));
			}
		);

		float progress = util::ResourceManager::GetProgress();
		// if (progress < 99.999f)
		if (false)
		{
			ImGui::NewFrame();

			ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
			ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
			// ImGui::SetNextWindowSize(ImVec2(GetWidth(), GetHeight()));
			ImGui::Begin("Loading", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
			{
				ImVec2 bar_size = ImVec2(500.f, 30.f);
				ImGui::SetCursorPos(ImVec2((ImGui::GetIO().DisplaySize.x - bar_size.x) / 2.f, (ImGui::GetIO().DisplaySize.y - bar_size.y) / 2.f));
				ImGui::ProgressBar(util::ResourceManager::GetProgress() / 100.f, bar_size);
			}
			ImGui::End();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			return true;
		}

		/*//ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		float progress = util::ResourceManager::GetProgress();
		if (progress < 99.999f)
		{
			ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
			ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
			ImGui::Begin("Loading", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
			{
				ImVec2 bar_size = ImVec2(500.f, 30.f);
				ImGui::SetCursorPos(ImVec2((ImGui::GetIO().DisplaySize.x - bar_size.x) / 2.f, (ImGui::GetIO().DisplaySize.y - bar_size.y) / 2.f));
				ImGui::ProgressBar(util::ResourceManager::GetProgress() / 100.f, bar_size);
			}
			ImGui::End();

			return true;
		}*/

		/*{
			float fps = (*GImGui).IO.Framerate;

			if (mFPS.size() > 100) // Max seconds to show
			{
				for (size_t i = 1; i < mFPS.size(); i++)
				{
					mFPS[i - 1] = mFPS[i];
				}
				mFPS[mFPS.size() - 1] = fps;
			}
			else
			{
				mFPS.push_back(fps);
			}
			float average_fps = 0.f;
			for (float val : mFPS)
			{
				average_fps += val / mFPS.size();
			}

			if (average_fps > 50.f)
			{
				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.7f, 0.f, 0.5f));
			}
			else if (average_fps > 20.f)
			{
				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.7f, 0.7f, 0.f, 0.5f));
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.7f, 0.f, 0.f, 0.5f));
			}

			ImGui::SetNextWindowSize(ImVec2(300.f, 700.f));
			ImGui::SetNextWindowPos(ImVec2(10.f, 10.f));
			ImGui::Begin("FPS Graph", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoNav);
			{
				char text[128] = {};

				sprintf(text, "FPS: %.3f", average_fps);
				ImGui::Text("%-64s", text);

				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 30.f);
				ImGui::PlotLines("##Frame Times", &mFPS[0], mFPS.size());

#ifdef __EMSCRIPTEN__
				const WebXRRigidTransform &headPose = WebXR::GetHeadPose();

				ImGui::Text("%s", "head");
				sprintf(text, "pos:[ %.3f, %.3f, %.3f]", headPose.position.x, headPose.position.y, headPose.position.z);
				ImGui::Text("%-64s", text);
				sprintf(text, "ori:[ %.3f, %.3f, %.3f, %.3f]", headPose.orientation.x, headPose.orientation.y, headPose.orientation.z, headPose.orientation.w);
				ImGui::Text("%-64s", text);
				for (uint32_t i = 0; i < 4; i++)
				{
					for (uint32_t j = 0; j < 4; j++)
					{
						if (j == 3)
							sprintf(text + (j + i * 4) * 7, " %5.2f\n", headPose.matrix[i][j]);
						else
							sprintf(text + (j + i * 4) * 7, " %5.2f ", headPose.matrix[i][j]);
					}
				}
				ImGui::Text("%-128s", text);

				WebXRInputSource *inputSourceArray = nullptr;
				uint32_t inputCount = 0;
				WebXR::GetInputSources(&inputSourceArray, &inputCount);
				for (uint32_t i = 0; i < inputCount; i++)
				{
					WebXRInputSource &inputSource = inputSourceArray[i];
					WebXRRigidTransform &transform = inputSource.rigidTransform;
					if (inputSource.handedness == XRHandedness::RIGHT)
						ImGui::Text("%s", "right");
					if (inputSource.handedness == XRHandedness::LEFT)
						ImGui::Text("%s", "left");
					sprintf(text, "pos:[ %.3f, %.3f, %.3f]", transform.position.x, transform.position.y, transform.position.z);
					ImGui::Text("%-64s", text);
					sprintf(text, "ori:[ %.3f, %.3f, %.3f, %.3f]", transform.orientation.x, transform.orientation.y, transform.orientation.z, transform.orientation.w);
					ImGui::Text("%-64s", text);
				}

				WebXRView *viewArray;
				uint32_t viewCount;
				WebXR::GetViews(&viewArray, &viewCount);
				for (uint32_t i = 0; i < viewCount; i++)
				{
					WebXRView &view = viewArray[i];

					if (view.eye == XREye::LEFT)
						ImGui::Text("%s", "left view0");
					else if (view.eye == XREye::RIGHT)
						ImGui::Text("%s", "right view0");

					sprintf(text, "port:[ %.3d, %.3d, %.3d, %.3d]", view.viewport.x, view.viewport.y, view.viewport.width, view.viewport.height);
					ImGui::Text("%-64s", text);
					sprintf(text, "pos:[ %.3f, %.3f, %.3f]", view.viewPose.position.x, view.viewPose.position.y, view.viewPose.position.z);
					ImGui::Text("%-64s", text);
					sprintf(text, "ori:[ %.3f, %.3f, %.3f, %.3f]", view.viewPose.orientation.x, view.viewPose.orientation.y, view.viewPose.orientation.z, view.viewPose.orientation.w);
					ImGui::Text("%-64s", text);

					for (uint32_t i = 0; i < 4; i++)
					{
						for (uint32_t j = 0; j < 4; j++)
						{
							if (j == 3)
								sprintf(text + (j + i * 4) * 7, " %5.2f\n", view.projectionMatrix[i][j]);
							else
								sprintf(text + (j + i * 4) * 7, " %5.2f ", view.projectionMatrix[i][j]);
						}
					}
					ImGui::Text("%-128s", text);
				}
#endif
			}
			ImGui::End();

			ImGui::PopStyleColor();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());*/
		return true;
	}

	void InitPrograms(const glm::vec3 &light_pos, const glm::vec3 &view_pos, const glm::mat4 &proj_view)
	{
		rsrc::Shaders *shaders = util::ResourceManager::GetShaders();

		gl::Program *pbr_prog = shaders->GetPBRProg();
		gl::Program *cubemap_prog = shaders->GetCubeMapProg();
		gl::Program *ray_prog = shaders->GetRayProg();
		gl::Program *menu_prog = shaders->GetMenuProg();
		gl::Program* light_map_prog = shaders->GetLightMapProg();

		pbr_prog->Use();
		if (!is_init_uniforms)
		{
			mUniformLocations.pbr_prog_proj_view = pbr_prog->Uniform("proj_view");
			mUniformLocations.pbr_prog_ligh_pos = pbr_prog->Uniform("uLightPos");
			mUniformLocations.pbr_prog_view_pos = pbr_prog->Uniform("uViewPos");
			
		}
		pbr_prog->SetMatrix4(mUniformLocations.pbr_prog_proj_view, proj_view);
		pbr_prog->SetFloat3(mUniformLocations.pbr_prog_view_pos, view_pos);
		pbr_prog->SetFloat3(mUniformLocations.pbr_prog_ligh_pos, light_pos);

		cubemap_prog->Use();
		if (!is_init_uniforms)
			mUniformLocations.cubemap_prog_proj_view = cubemap_prog->Uniform("proj_view");
		cubemap_prog->SetMatrix4(mUniformLocations.cubemap_prog_proj_view, proj_view);

		ray_prog->Use();
		if (!is_init_uniforms)
			mUniformLocations.ray_prog_proj_view = ray_prog->Uniform("proj_view");
		ray_prog->SetMatrix4(mUniformLocations.ray_prog_proj_view, proj_view);

		menu_prog->Use();
		if (!is_init_uniforms)
			mUniformLocations.menu_prog_proj_view = menu_prog->Uniform("proj_view");
		menu_prog->SetMatrix4(mUniformLocations.menu_prog_proj_view, proj_view);

		light_map_prog->Use();
		if (!is_init_uniforms)
			mUniformLocations.light_map_prog_proj_view = light_map_prog->Uniform("proj_view");
		light_map_prog->SetMatrix4(mUniformLocations.light_map_prog_proj_view, proj_view);

		is_init_uniforms = true;
	}

	virtual bool OnRender()
	{
#ifndef __EMSCRIPTEN__
		if (util::ResourceManager::GetProgress() < 99.999f)
			return true;
#endif

		rsrc::Shaders *shaders = util::ResourceManager::GetShaders();

		if (!shaders->IsReady())
			return true;

		InitPrograms(mCamera.GetPosition(), mCamera.GetPosition(), mCamera.GetProjectionMat() * mCamera.GetViewMat());
		gl::Render::SetClearColor(0.f, 0.7f, 0.f, 1.f);
		gl::Render::Clear(gl::BufferBit::COLOR, gl::BufferBit::DEPTH);
#ifndef __EMSCRIPTEN__
		util::ResourceManager::GetCubeMap()->Draw(shaders->GetCubeMapProg(), mCamera.GetViewMat(), mCamera.GetProjectionMat());
		glm::mat4 model = glm::scale(glm::mat4(1.f), glm::vec3(0.25f, 0.25f, 0.25f));
		util::ResourceManager::GetModel()->Draw(glm::translate(model, glm::vec3(0.f, -0.5f, 0.f)));

		util::ResourceManager::GetController()->Draw(glm::vec3(0.f, 0.f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f));
		util::ResourceManager::GetW3DMenu()->Draw();
		util::ResourceManager::GetW3DMenu1()->Draw();
		util::ResourceManager::GetW3DVideo2D()->Draw();
#else
		const WebXRRigidTransform &headPose = WebXR::GetHeadPose();

		WebXRView *viewArray;
		uint32_t viewCount;
		WebXR::GetViews(&viewArray, &viewCount);
		//printf("viewCount: %d\n", viewCount);
		for (uint32_t i = 0; i < viewCount; i++)
		{
			//if(i == 1)
				//continue;

			WebXRView &view = viewArray[i];
			//printf("view%d, viewPort[%d, %d, %d, %d]\n", i, view.viewport.x, view.viewport.y, view.viewport.width, view.viewport.width);
			gl::Render::SetViewport(view.viewport.x, view.viewport.y, view.viewport.width, view.viewport.height);
			if (shaders->IsReady())
			{
				InitPrograms(headPose.position, headPose.position, view.projectionMatrix * view.viewPose.matrix);

				/*char text[128] = {};
				for (uint32_t r = 0; r < 4; r++)
				{
					for (uint32_t c = 0; c < 4; c++)
					{
						if (c == 3)
							sprintf(text + (c + r * 4) * 7, " %5.2f\n", view.projectionMatrix[r][c]);
						else
							sprintf(text + (c + r * 4) * 7, " %5.2f ", view.projectionMatrix[r][c]);
					}
				}
				printf("projM%d:%s\n", i, text);*/


				util::ResourceManager::GetCubeMap()->Draw(shaders->GetCubeMapProg(), view.viewPose.matrix, view.projectionMatrix);

				util::ResourceManager::GetModel()->Draw(glm::translate(glm::mat4(1.f), glm::vec3(0.f, -1.f, -3.f)));
				//  util::ResourceManager::GetModel1()->Draw(glm::translate(glm::mat4(1.f), glm::vec3(2.f, 0.f, 1.f)));

				util::ResourceManager::GetW3DMenu()->Draw();
				util::ResourceManager::GetW3DMenu1()->Draw();
				//  util::ResourceManager::GetW3DMenu()->Draw(glm::translate(glm::mat4(1.f), glm::vec3(0.f, 1.6f, -1.5f)));
				// util::ResourceManager::GetW3DVideo3D()->Draw(glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f)));
				// util::ResourceManager::GetW3DVideo3D()->Draw(glm::mat4(1.f));
				util::ResourceManager::GetW3DVideo2D()->Draw();

				WebXRInputSource *inputSourceArray = nullptr;
				uint32_t inputCount = 0;
				WebXR::GetInputSources(&inputSourceArray, &inputCount);
				
				for (uint32_t j = 0; j < inputCount; j++)
				{
					WebXRInputSource &inputSource = inputSourceArray[j];
					WebXRRigidTransform &transform = inputSource.rigidTransform;
					/*if (inputSource.handedness == XRHandedness::RIGHT)
						ImGui::Text("%s", "right");
					if (inputSource.handedness == XRHandedness::LEFT)
						ImGui::Text("%s", "left");*/
					if (inputSource.handedness == XRHandedness::RIGHT)
						mInputSource = &inputSource;

					util::ResourceManager::GetController()->Draw(transform.position, transform.orientation);
				}
			}
		}
		GL(Flush());
#endif
		return true;
	};
	virtual bool OnFinalize()
	{
		printf("MyApp::OnFinalize\n");
		util::ResourceManager::OnFinalize();
		return true;
	};

	virtual void OnMouseLeftDown(double x, double y)
	{
		printf("MyApp::OnMouseLeftDown\n");
	}
	virtual void OnMouseLeftUp(double x, double y)
	{
		printf("MyApp::OnMouseLeftUp\n");
	}
	virtual void OnMouseRightDown(double x, double y)
	{
		printf("MyApp::OnMouseRightDown\n");
	}
	virtual void OnMouseRightUp(double x, double y)
	{
		printf("MyApp::OnMouseRightUp\n");
	}
	virtual void OnMouseMiddleDown(double x, double y)
	{
		printf("MyApp::OnMouseMiddleDown\n");
	}
	virtual void OnMouseMiddleUp(double x, double y)
	{
		printf("MyApp::OnMouseMiddleUp\n");
	}
	virtual void OnMouseMove(double x, double y)
	{
		printf("MyApp::OnMouseMove\n");
	}
	virtual void OnMouseWhell(double offset)
	{
		printf("MyApp::OnMouseWhell\n");
	}
	virtual void OnKeyPress(int key)
	{
		printf("MyApp::OnKeyPress\n");
	}
	virtual void OnKeyRepeat(int key)
	{
		printf("MyApp::OnKeyRepeat\n");
	}
	virtual void OnKeyRelease(int key)
	{
		printf("MyApp::OnKeyRelease\n");
	}
	virtual void OnResize(int width, int height)
	{
		if (height > 0)
		{
			gl::Render::SetViewport(width, height);
			mCamera.SetState(0.01f, 100.f, (float)width / (float)height, 60.f);
		}

		printf("MyApp::OnResize\n");
	}

private:
};

int main(int argc, char **argv)
{
	MyApp::Launch().Run(argc, argv, "v0.0", 1280, 720, "Opengl Window");
	return 0;
}
