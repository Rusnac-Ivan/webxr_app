#ifndef _RESOURCES_MANAGER_H_
#define _RESOURCES_MANAGER_H_

#include <core/Platform.h>
#include <resources/Shaders.h>
#include <resources/CubeMap.h>
#include <resources/Model.h>
#include <utilities/Controller/Controller.h>
#include <widgets3d/Menu.h>

namespace util
{
    class ResourceManager
    {


        static std::unique_ptr<rsrc::Shaders> mShaders;
        static std::unique_ptr<rsrc::CubeMap> mCubeMap;
        static std::unique_ptr<rsrc::Model> mModel;
        static std::unique_ptr<util::Controller> mController;
        static std::unique_ptr<w3d::Menu> mMenu;

        ResourceManager();
        ~ResourceManager();

    public:
        static void OnInitialize();
        static void OnFinalize();

        static rsrc::CubeMap *GetCubeMap();
        static rsrc::Model* GetModel();
        static rsrc::Shaders *GetShaders();
        static util::Controller *GetController();
        static w3d::Menu* GetW3DMenu();

        static float GetProgress();
    };
}

#endif