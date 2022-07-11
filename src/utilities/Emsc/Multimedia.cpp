#ifdef __EMSCRIPTEN__
#include "Multimedia.h"
#include <string>

namespace em
{

    EMSCRIPTEN_BINDINGS(Multimedia)
    {
        emscripten::function("JSMediaOnCanPlay", &Multimedia::OnCanPlay);
        emscripten::function("JSMediaOnCanNotPlay", &Multimedia::OnCanNotPlay);
        emscripten::function("JSMediaOnLoadedData", &Multimedia::OnLoadedData);

        emscripten::function("JSMediaOnPlay", &Multimedia::OnPlay);
        emscripten::function("JSMediaOnTimeUpdate", &Multimedia::OnTimeUpdate);
        emscripten::function("JSMediaOnEnded", &Multimedia::OnEnded);
        emscripten::function("JSMediaOnPause", &Multimedia::OnPause);
        emscripten::function("JSMediaOnWaiting", &Multimedia::OnWaiting);
    };

    EMSCRIPTEN_BINDINGS(Video)
    {
        emscripten::function("JSMediaOnResize", &Video::OnResize);
    };

    void Multimedia::OnCanPlay(emscripten::val thiz, emscripten::val event)
    {
        Multimedia *player = (Multimedia *)(thiz["instance"].as<uintptr_t>());
        printf("OnCanPlay\n");

        MediaEvent media_event;
        media_event.SetType(MediaEvent::Type::ON_CANPLAY);
        player->Notify(media_event);
    }
    void Multimedia::OnCanNotPlay(emscripten::val thiz, emscripten::val event)
    {
        Multimedia *player = (Multimedia *)(thiz["instance"].as<uintptr_t>());
        printf("OnCanNotPlay\n");

        MediaEvent media_event;
        media_event.SetType(MediaEvent::Type::ON_CANNOT_PLAY);
        player->Notify(media_event);
    }

    void Multimedia::OnLoadedData(emscripten::val thiz, emscripten::val event)
    {
        Multimedia *player = (Multimedia *)(thiz["instance"].as<uintptr_t>());
        printf("OnLoadedData\n");

        /*MediaEvent media_event;
        media_event.SetType(MediaEvent::Type::ON_LOADED_DATA);
        player->Notify(media_event);*/

        player->mState = MediaState::IDLE;
    }
    void Multimedia::OnTimeUpdate(emscripten::val thiz, emscripten::val event)
    {
        Multimedia *player = (Multimedia *)(thiz["instance"].as<uintptr_t>());
        // printf("OnTimeUpdate\n");

        /*MediaEvent media_event;
        media_event.SetType(MediaEvent::Type::ON_TIME_UPDATE);
        player->Notify(media_event);*/

        player->mTimeupdate = true;
        player->CheckReady();
    }
    void Multimedia::OnWaiting(emscripten::val thiz, emscripten::val event)
    {
        Multimedia *player = (Multimedia *)(thiz["instance"].as<uintptr_t>());
        printf("OnWaiting\n");

        MediaEvent media_event;
        media_event.SetType(MediaEvent::Type::ON_WAITING);
        player->Notify(media_event);

        player->mState = MediaState::WAITING;
    }

    void Multimedia::OnPlay(emscripten::val thiz, emscripten::val event)
    {
        Multimedia *player = (Multimedia *)(thiz["instance"].as<uintptr_t>());
        printf("OnPlay\n");
        player->mPlaying = true;
        player->CheckReady();

        MediaEvent media_event;
        media_event.SetType(MediaEvent::Type::ON_PLAY);
        player->Notify(media_event);

        player->mState = MediaState::PLAYING;
    }
    void Multimedia::OnPause(emscripten::val thiz, emscripten::val event)
    {
        Multimedia *player = (Multimedia *)(thiz["instance"].as<uintptr_t>());
        printf("OnPause\n");

        MediaEvent media_event;
        media_event.SetType(MediaEvent::Type::ON_PAUSE);
        player->Notify(media_event);

        player->mState = MediaState::PAUSED;
    }
    void Multimedia::OnEnded(emscripten::val thiz, emscripten::val event)
    {
        Multimedia *player = (Multimedia *)(thiz["instance"].as<uintptr_t>());
        printf("OnEnded\n");

        MediaEvent media_event;
        media_event.SetType(MediaEvent::Type::ON_ENDED);
        player->Notify(media_event);

        player->mState = MediaState::ENDED;
    }

    Multimedia::Multimedia(/* args */) : mMedia(emscripten::val::undefined())
    {
        mState = MediaState::NONE;
        mIsReady = mPlaying = mTimeupdate = false;
    }

    Multimedia::~Multimedia()
    {
    }

    void Multimedia::AddListener(IEventListener *listener)
    {
        mEventListeners.push_back(listener);
    }
    void Multimedia::RemoveListener(IEventListener *listener)
    {
        mEventListeners.remove(listener);
    }

    void Multimedia::Notify(IEvent &event)
    {
        for (IEventListener *ev_listener : mEventListeners)
        {
            (*ev_listener)(event);
        }
    }

    void Multimedia::Create(MediaType type)
    {
        mState = MediaState::NONE;
        mIsReady = mPlaying = mTimeupdate = false;

        if (type == MediaType::VIDEO)
        {
            emscripten::val document = emscripten::val::global("document");
            mMedia = document.call<emscripten::val>("createElement", std::string("video"));
        }
        if (type == MediaType::AUDIO)
        {
            emscripten::val document = emscripten::val::global("document");
            mMedia = document.call<emscripten::val>("createElement", std::string("audio"));
        }

        mMedia.set("crossOrigin", std::string("anonymous"));

        emscripten::val params = emscripten::val::object();
        params.set("instance", (uintptr_t)this);

        emscripten::val MediaCanPlay = emscripten::val::module_property("JSMediaOnCanPlay").call<emscripten::val>("bind", emscripten::val::null(), params);
        emscripten::val MediaCanNotPlay = emscripten::val::module_property("JSMediaOnCanNotPlay").call<emscripten::val>("bind", emscripten::val::null(), params);
        emscripten::val MediaLoadedData = emscripten::val::module_property("JSMediaOnLoadedData").call<emscripten::val>("bind", emscripten::val::null(), params);
        emscripten::val MediaPlaying = emscripten::val::module_property("JSMediaOnPlay").call<emscripten::val>("bind", emscripten::val::null(), params);
        emscripten::val MediaTimeUpdate = emscripten::val::module_property("JSMediaOnTimeUpdate").call<emscripten::val>("bind", emscripten::val::null(), params);
        emscripten::val MediaEnded = emscripten::val::module_property("JSMediaOnEnded").call<emscripten::val>("bind", emscripten::val::null(), params);
        emscripten::val MediaPause = emscripten::val::module_property("JSMediaOnPause").call<emscripten::val>("bind", emscripten::val::null(), params);
        emscripten::val MediaWaiting = emscripten::val::module_property("JSMediaOnWaiting").call<emscripten::val>("bind", emscripten::val::null(), params);

        mMedia.call<void>("addEventListener", std::string("canplay"), MediaCanPlay, true);
        mMedia.call<void>("addEventListener", std::string("error"), MediaCanNotPlay, true);
        mMedia.call<void>("addEventListener", std::string("loadeddata"), MediaLoadedData, true);
        mMedia.call<void>("addEventListener", std::string("playing"), MediaPlaying, true);
        mMedia.call<void>("addEventListener", std::string("timeupdate"), MediaTimeUpdate, true);
        mMedia.call<void>("addEventListener", std::string("pause"), MediaPause, true);
        mMedia.call<void>("addEventListener", std::string("waiting"), MediaWaiting, true);
        mMedia.call<void>("addEventListener", std::string("ended"), MediaEnded, true);
    }

    void Multimedia::Destroy()
    {
        mState = MediaState::NONE;

        mMedia.call<void>("pause");
        mMedia.call<void>("removeAttribute", emscripten::val("src"));
        mMedia.call<void>("load");

        // element.parentNode.removeChild(element);
        mMedia["parentNode"].call<void>("removeChild", mMedia);
        mMedia = emscripten::val::undefined();
    }

    Video::Video()
    {
        // mIsFirstFrame = true;

        Create(MediaType::VIDEO);

        emscripten::val params = emscripten::val::object();
        params.set("instance", (uintptr_t)this);

        emscripten::val MediaResize = emscripten::val::module_property("JSMediaOnResize").call<emscripten::val>("bind", emscripten::val::null(), params);

        mMedia.call<void>("addEventListener", std::string("resize"), MediaResize, true);
    }
    Video::~Video()
    {
        Destroy();
    }

    void Video::OnResize(emscripten::val thiz, emscripten::val event)
    {
        Video *player = (Video *)(thiz["instance"].as<uintptr_t>());
        printf("OnResize\n");

        player->mWidth = player->mMedia["videoWidth"].as<uint32_t>();
        player->mHeight = player->mMedia["videoHeight"].as<uint32_t>();

        // player->mIsFirstFrame = true;
    }

    void Video::Load(const char *url)
    {
        mMedia.call<void>("pause");
        mMedia.set("src", std::string(url));
        mMedia.call<void>("load");
    }

}

#endif