#include "ResourceManager.h"

namespace util
{

    std::unique_ptr<rsrc::Shaders> ResourceManager::mShaders;
    std::unique_ptr<rsrc::CubeMap> ResourceManager::mCubeMap;
    std::unique_ptr<util::Controller> ResourceManager::mController;
    std::unique_ptr<rsrc::Model> ResourceManager::mModel;
    std::unique_ptr<rsrc::Model> ResourceManager::mModel1;
    std::unique_ptr<w3d::Menu> ResourceManager::mMenu;
    std::unique_ptr<w3d::Video2D> ResourceManager::mVideo2D;
    std::unique_ptr<w3d::Video3D> ResourceManager::mVideo3D;

    ResourceManager::ResourceManager()
    {
    }
    ResourceManager::~ResourceManager() {}

    rsrc::CubeMap *ResourceManager::GetCubeMap() { return mCubeMap.get(); }
    rsrc::Model *ResourceManager::GetModel() { return mModel.get(); }
    rsrc::Model *ResourceManager::GetModel1() { return mModel1.get(); }
    rsrc::Shaders *ResourceManager::GetShaders() { return mShaders.get(); }
    w3d::Menu *ResourceManager::GetW3DMenu() { return mMenu.get(); }
    w3d::Video2D *ResourceManager::GetW3DVideo2D() { return mVideo2D.get(); }
    w3d::Video3D *ResourceManager::GetW3DVideo3D() { return mVideo3D.get(); }

    util::Controller *ResourceManager::GetController()
    {
        return mController.get();
    }

    void ResourceManager::OnInitialize()
    {
#ifdef __EMSCRIPTEN__
        std::string path = "./resources";
#else
        std::string path = "D:/CPP/webxr_app/resources";
#endif

        // loads shaders
        mShaders = std::make_unique<rsrc::Shaders>();
        mShaders->Load(std::string(path + "/shaders/").c_str());
        // load cube map
        mCubeMap = std::make_unique<rsrc::CubeMap>();
        mCubeMap->Load(std::string(path + "/cube_maps/yokohama/").c_str());

        // loads models
        // mModel = std::make_unique<rsrc::Model>();
        // mModel->Load(std::string(path + "/models/Range_Rover.glb").c_str());

        // mModel1 = std::make_unique<rsrc::Model>();
        // mModel1->Load(std::string(path + "/models/mini.glb").c_str());

        mController = std::make_unique<util::Controller>();
        mController->Create(path.c_str(), 1.5f, 0.01f);
        // mController->Load(std::string(path + "/models/DamagedHelmet.glb").c_str());
        // mController->Load(std::string(path + "/models/watch.glb").c_str());
        // mController->Load(std::string(path + "/models/space_war_c.glb").c_str());

        mMenu = std::make_unique<w3d::Menu>();
        mMenu->Create(500, 700);

        mVideo2D = std::make_unique<w3d::Video2D>();
        mVideo2D->Create("./resources/video/2d/test_2d_vid.mp4");

        // mVideo3D = std::make_unique<w3d::Video3D>();
        //  mVideo3D->Create(10.f, "./resources/video/3d/test_3d_vid.mp4");
        //  mVideo3D->Create("./resources/video/3d/test_3d_vid.mp4");
        //  mVideo3D->Create("https://vimeo.com/215984159");
    }
    void ResourceManager::OnFinalize()
    {
        mShaders.release();
        mCubeMap.release();
        mModel.release();
        mModel1.release();
        mController.release();
        mMenu.release();
        mVideo2D.release();
        mVideo3D.release();
    }

    float ResourceManager::GetProgress()
    {
        float progress = 0;
        float count = 0;

        if (mShaders)
        {
            progress += mShaders->GetProgress();
            count++;
        }

        if (mCubeMap)
        {
            progress += mCubeMap->GetProgress();
            count++;
        }

        if (mModel)
        {
            progress += mModel->GetProgress();
            count++;
        }

        if (mModel1)
        {
            progress += mModel1->GetProgress();
            count++;
        }

        if (mController)
        {
            progress += mController->GetProgress();
            count++;
        }

        return progress / count;
    }
}