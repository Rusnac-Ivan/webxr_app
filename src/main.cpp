#include <iostream>
#include <chrono>

#include <imgui.h>
#include <imgui_internal.h>

#include <core/Application.h>
#include <opengl/Render.h>
#include <utilities/Emsc/webxr.h>

class MyApp : public core::Application
{
private:
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
		printf("MyApp::OnInitialize\n");
		return true;
	};
	virtual bool OnGui()
	{
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

				const WebXRRigidTransform &headPose = WebXR::GetHeadPose();
				const WebXRRigidTransform &leftIn = WebXR::GetLeftInputSource().rigidTransform;
				const WebXRRigidTransform &rightIn = WebXR::GetRightInputSource().rigidTransform;

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

				ImGui::Text("%s", "left");
				sprintf(text, "pos:[ %.3f, %.3f, %.3f]", leftIn.position.x, leftIn.position.y, leftIn.position.z);
				ImGui::Text("%-64s", text);
				sprintf(text, "ori:[ %.3f, %.3f, %.3f, %.3f]", leftIn.orientation.x, leftIn.orientation.y, leftIn.orientation.z, leftIn.orientation.w);
				ImGui::Text("%-64s", text);

				ImGui::Text("%s", "right");
				sprintf(text, "pos:[ %.3f, %.3f, %.3f]", rightIn.position.x, rightIn.position.y, rightIn.position.z);
				ImGui::Text("%-64s", text);
				sprintf(text, "ori:[ %.3f, %.3f, %.3f, %.3f]", rightIn.orientation.x, rightIn.orientation.y, rightIn.orientation.z, rightIn.orientation.w);
				ImGui::Text("%-64s", text);

				const WebXRView &leftView = WebXR::GetLeftEyeView();
				const WebXRView &rightView = WebXR::GetRightEyeView();
				ImGui::Text("%s", "left view0");
				sprintf(text, "port:[ %.3d, %.3d, %.3d, %.3d]", leftView.viewport.x, leftView.viewport.y, leftView.viewport.width, leftView.viewport.height);
				ImGui::Text("%-64s", text);
				sprintf(text, "pos:[ %.3f, %.3f, %.3f]", leftView.viewPose.position.x, leftView.viewPose.position.y, leftView.viewPose.position.z);
				ImGui::Text("%-64s", text);
				sprintf(text, "ori:[ %.3f, %.3f, %.3f, %.3f]", leftView.viewPose.orientation.x, leftView.viewPose.orientation.y, leftView.viewPose.orientation.z, leftView.viewPose.orientation.w);
				ImGui::Text("%-64s", text);
				ImGui::Text("%s", "right view0");
				sprintf(text, "port:[ %.3d, %.3d, %.3d, %.3d]", rightView.viewport.x, rightView.viewport.y, rightView.viewport.width, rightView.viewport.height);
				ImGui::Text("%-64s", text);
				sprintf(text, "pos:[ %.3f, %.3f, %.3f]", rightView.viewPose.position.x, rightView.viewPose.position.y, rightView.viewPose.position.z);
				ImGui::Text("%-64s", text);
				sprintf(text, "ori:[ %.3f, %.3f, %.3f, %.3f]", rightView.viewPose.orientation.x, rightView.viewPose.orientation.y, rightView.viewPose.orientation.z, rightView.viewPose.orientation.w);
				ImGui::Text("%-64s", text);

				for (uint32_t i = 0; i < 4; i++)
				{
					for (uint32_t j = 0; j < 4; j++)
					{
						if (j == 3)
							sprintf(text + (j + i * 4) * 7, " %5.2f\n", rightView.projectionMatrix[i][j]);
						else
							sprintf(text + (j + i * 4) * 7, " %5.2f ", rightView.projectionMatrix[i][j]);
					}
				}
				ImGui::Text("%-128s", text);
			}
			ImGui::End();

			ImGui::PopStyleColor();
		}

		return true;
	}
	virtual bool OnRender()
	{
		static bool is_left = true;
		const WebXRView &leftView = WebXR::GetLeftEyeView();
		const WebXRView &rightView = WebXR::GetRightEyeView();

		// render left eye
		// if (is_left)
		{
			gl::Render::SetViewport(leftView.viewport.x, leftView.viewport.y, leftView.viewport.width, leftView.viewport.height);
			gl::Render::SetClearColor(0.5f, 0.4f, 0.0f, 1.f);
			gl::Render::Clear(gl::BufferBit::COLOR);
		}

		// render right eye
		// if (!is_left)
		{
			gl::Render::SetViewport(rightView.viewport.x, rightView.viewport.y, rightView.viewport.width, rightView.viewport.height);
			gl::Render::SetClearColor(0.f, 0.5f, 0.4f, 1.f);
			gl::Render::Clear(gl::BufferBit::COLOR);
		}

		is_left = !is_left;

		return true;
	};
	virtual bool OnFinalize()
	{
		printf("MyApp::OnFinalize\n");
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
		printf("MyApp::OnResize\n");
	}

private:
};

int main(int argc, char **argv)
{
	MyApp::Launch().Run(argc, argv, "v0.0", 1280, 720, "Opengl Window");
	return 0;
}
