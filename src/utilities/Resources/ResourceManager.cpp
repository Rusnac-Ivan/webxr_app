#include "ResourceManager.h"

namespace util
{

    std::unique_ptr<rsrc::Shaders> ResourceManager::mShaders;
    std::unique_ptr<rsrc::CubeMap> ResourceManager::mCubeMap;
    std::unique_ptr<util::Controller> ResourceManager::mController;
    std::unique_ptr<rsrc::Model> ResourceManager::mModel;

    ResourceManager::ResourceManager()
    {
    }
    ResourceManager::~ResourceManager() {}

    rsrc::CubeMap *ResourceManager::GetCubeMap() { return mCubeMap.get(); }
    rsrc::Model* ResourceManager::GetModel() { return mModel.get(); }
    rsrc::Shaders *ResourceManager::GetShaders() { return mShaders.get(); }

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
        mModel = std::make_unique<rsrc::Model>();
        mModel->Load(std::string(path + "/models/watch.glb").c_str());

        mController = std::make_unique<util::Controller>();
        mController->Create(path.c_str(), 1.5f, 0.01f);
        // mController->Load(std::string(path + "/models/DamagedHelmet.glb").c_str());
        // mController->Load(std::string(path + "/models/watch.glb").c_str());
        // mController->Load(std::string(path + "/models/space_war_c.glb").c_str());
    }
    void ResourceManager::OnFinalize()
    {
        mShaders.release();
        mCubeMap.release();
        mModel.release();
        mController.release();
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

        if (mController)
        {
            progress += mController->GetProgress();
            count++;
        }

        return progress / count;
    }
}