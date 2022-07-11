#ifndef _RESOURCES_MANAGER_H_
#define _RESOURCES_MANAGER_H_

#include <core/Platform.h>
#include <resources/Shaders.h>
#include <resources/CubeMap.h>
#include <resources/Model.h>
#include <utilities/Controller/Controller.h>
#include <widgets3d/Menu.h>
#include <widgets3d/Video2D.h>
#include <widgets3d/Video3D.h>
#include <resources/Image.h>

namespace util
{
    class ResourceManager
    {

        static std::unique_ptr<rsrc::Shaders> mShaders;
        static std::unique_ptr<rsrc::CubeMap> mCubeMap;
        static std::unique_ptr<rsrc::Model> mModel;
        static std::unique_ptr<rsrc::Model> mModel1;
        static std::unique_ptr<util::Controller> mController;
        static std::unique_ptr<w3d::Menu> mMenu;
        static std::unique_ptr<w3d::Menu> mMenu1;
        static std::unique_ptr<w3d::Video2D> mVideo2D;
        static std::unique_ptr<w3d::Video3D> mVideo3D;
        static std::unique_ptr<rsrc::Image> mImage;

        ResourceManager();
        ~ResourceManager();

    public:
        static void OnInitialize();
        static void OnFinalize();

        static void LoadModel1();

        static rsrc::CubeMap *GetCubeMap();
        static rsrc::Model *GetModel();
        static rsrc::Model *GetModel1();
        static rsrc::Shaders *GetShaders();
        static util::Controller *GetController();
        static w3d::Menu *GetW3DMenu();
        static w3d::Menu *GetW3DMenu1();
        static w3d::Video2D *GetW3DVideo2D();
        static w3d::Video3D *GetW3DVideo3D();
        static rsrc::Image *GetImage();

        static float GetProgress();
    };
}

#endif