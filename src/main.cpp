#include <iostream>
#include <chrono>

#include <imgui.h>

#include <core/Application.h>
#include <opengl/Render.h>

class MyApp : public core::Application
{
private:
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
		return true;
	};
	virtual bool OnRender()
	{
		gl::Render::SetClearColor(0.f, 0.3f, 0.4f, 1.f);
		gl::Render::Clear(gl::BufferBit::COLOR);
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
