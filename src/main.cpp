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

class MyApp : public core::Application
{
private:
	util::Camera<util::ProjectionType::PERSPECTIVE> mCamera;
	std::vector<float> mFPS;
	MyApp(MyApp &app) = delete;
	MyApp &operator=(MyApp &app) = delete;
	MyApp() : Application() {}
	~MyApp() {}

public:
	static MyApp &Launch()
	{
		static MyApp app;
		return app;
	}

	virtual bool OnInitialize()
	{
		mCamera.SetViewState(glm::vec3(0.f, 0.08f, 0.2f), glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, -1.f));

		printf("MyApp::OnInitialize\n");
		util::ResourceManager::OnInitialize();

		gl::Pipeline::SetBlendFunc(gl::ComputOption::SRC_ALPHA, gl::ComputOption::ONE_MINUS_SRC_ALPHA);

		gl::Pipeline::EnableDepthTest();
		gl::Render::SetClearColor(0.5f, 0.4f, 0.0f, 1.f);
		return true;
	};
	virtual bool OnGui()
	{
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
		}

		// printf("MyApp::OnGui\n");
		ImGui::ShowDemoWindow(nullptr);

		{
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

		return true;
	}
	virtual bool OnRender()
	{
		rsrc::Shaders *shaders = util::ResourceManager::GetShaders();

		gl::Render::Clear(gl::BufferBit::COLOR, gl::BufferBit::DEPTH);
#ifndef __EMSCRIPTEN__
		util::ResourceManager::GetCubeMap()->Draw(shaders->GetCubeMapProg(), mCamera.GetViewMat(), mCamera.GetProjectionMat());
		gl::Program *program = shaders->GetPBRProg();
		program->Use();
		program->SetFloat3(program->Uniform("uViewPos"), mCamera.GetPosition());
		program->SetFloat3(program->Uniform("uLightPos"), mCamera.GetPosition());
		program->SetMatrix4(program->Uniform("view"), mCamera.GetViewMat());
		program->SetMatrix4(program->Uniform("projection"), mCamera.GetProjectionMat());
		util::ResourceManager::GetModel()->Draw(program, glm::mat4(1.f));

		util::ResourceManager::GetController()->Draw(mCamera.GetViewMat(), mCamera.GetProjectionMat(), glm::vec3(0.f, 0.f, 0.f), glm::quat(1.f, 0.f, 0.f, 0.f));

#else
		const WebXRRigidTransform &headPose = WebXR::GetHeadPose();

		WebXRView *viewArray;
		uint32_t viewCount;
		WebXR::GetViews(&viewArray, &viewCount);
		for (uint32_t i = 0; i < viewCount; i++)
		{
			WebXRView &view = viewArray[i];

			gl::Render::SetViewport(view.viewport.x, view.viewport.y, view.viewport.width, view.viewport.height);
			if (shaders->IsReady())
			{
				util::ResourceManager::GetCubeMap()->Draw(shaders->GetCubeMapProg(), view.viewPose.matrix, view.projectionMatrix);

				gl::Program *program = shaders->GetPBRProg();
				program->Use();
				program->SetFloat3(program->Uniform("uViewPos"), headPose.position);
				program->SetFloat3(program->Uniform("uLightPos"), headPose.position);
				program->SetMatrix4(program->Uniform("view"), view.viewPose.matrix);
				program->SetMatrix4(program->Uniform("projection"), view.projectionMatrix);
				util::ResourceManager::GetModel()->Draw(program, glm::mat4(1.f));

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

					util::ResourceManager::GetController()->Draw(view.viewPose.matrix, view.projectionMatrix, transform.position, transform.orientation);
				}
			}
		}
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
