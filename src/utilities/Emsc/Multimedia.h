#ifdef __EMSCRIPTEN__
#ifndef _EM_MULTIMEDIA_H_
#define _EM_MULTIMEDIA_H_

#include <core/Platform.h>
#include <opengl/Texture2D.h>
#include <utilities/Events/CallBack.h>
#include <events/IEventSource.h>
#include <events/IEvent.h>

namespace em
{

    class MediaEvent : public IEvent
    {
    public:
        enum Type
        {
            ON_CANPLAY,
            ON_CANNOT_PLAY,
            ON_LOADED_DATA,
            ON_PLAY,
            ON_TIME_UPDATE,
            ON_ENDED,
            ON_PAUSE,
            ON_WAITING
        };

    private:
        Type mEventType;

    public:
        Type GetType() { return mEventType; }
        void SetType(Type type) { mEventType = type; }
    };

    enum struct MediaType
    {
        VIDEO,
        AUDIO
    };

    enum struct MediaState
    {
        NONE,
        IDLE,
        PLAYING,
        PAUSED,
        WAITING,
        ENDED
    };

    class Multimedia : public IEventSource
    {
    public:
    protected:
        emscripten::val mMedia;
        MediaState mState;

        bool mPlaying;
        bool mTimeupdate;
        bool mIsReady;

    public:
        Multimedia(/* args */);
        virtual ~Multimedia();

        void Create(MediaType type);
        void Destroy();

        MediaState GetMediaState() { return mState; }

        // virtual void SetSource(const char *address) = 0;
        // virtual void SetSource(const void *mediaStream) = 0;

        float GetVolume() { return mMedia["volume"].as<float>(); }
        float GetCurrentTime() { return mMedia["currentTime"].as<float>(); }
        float GetDuration() { return mMedia["duration"].as<float>(); }
        bool IsPaused() { return mMedia["paused"].as<bool>(); }
        bool IsLoop() { return mMedia["loop"].as<bool>(); }

        void SetCurrentTime(float time) { mMedia.set("currentTime", time); }
        void SetMute(bool muted) { mMedia.set("muted", muted); }
        void SetVolume(float volume) { mMedia.set("volume", volume); }
        void SetLoop(bool loop) { mMedia.set("loop", loop); }
        void SetAutoplay(bool autoplay) { mMedia.set("autoplay", autoplay); }

        virtual void AddListener(IEventListener *listener);
        virtual void RemoveListener(IEventListener *listener);
        virtual void Notify(IEvent &event);

        static void OnCanPlay(emscripten::val thiz, emscripten::val event);
        static void OnCanNotPlay(emscripten::val thiz, emscripten::val event);

        static void OnLoadedData(emscripten::val thiz, emscripten::val event);
        static void OnTimeUpdate(emscripten::val thiz, emscripten::val event);
        static void OnWaiting(emscripten::val thiz, emscripten::val event);

        static void OnPlay(emscripten::val thiz, emscripten::val event);
        static void OnPause(emscripten::val thiz, emscripten::val event);
        static void OnEnded(emscripten::val thiz, emscripten::val event);

        void Play() { mMedia.call<void>("play"); }
        void Pause() { mMedia.call<void>("pause"); }

        virtual void Load(const char *url) = 0;

        bool IsReady() { return mIsReady; }
        bool IsPlaying() { return mPlaying; }

        void CheckReady()
        {
            if (mPlaying && mTimeupdate)
                mIsReady = true;
            else
                mIsReady = false;
        }

    private:
    };

    class Video : public Multimedia
    {

        // bool mIsFirstFrame;

        uint32_t mWidth;
        uint32_t mHeight;

    public:
        Video();
        ~Video();

        virtual void Load(const char *url);

        float GetWidth() { return mWidth; }
        float GetHeight() { return mHeight; }
        // void GetPoster();

        void SetWidth(float width);
        void SetHeight(float height);
        void SetPoster();

        static void OnResize(emscripten::val thiz, emscripten::val event);

        void UpdateFrame(gl::Texture2D &frame)
        {
            if (mIsReady)
            {
                // if (mIsFirstFrame)
                {
                    frame.Bind();
                    emscripten::val::global("Module")["canvas"].call<emscripten::val>("getContext", std::string("webgl2")).call<void>("texImage2D", GL_TEXTURE_2D, 0, GL_RGB, mWidth, mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, mMedia);
                    // mIsFirstFrame = false;
                }
                /*else
                {
                    frame.Bind();
                    emscripten::val::global("Module")["canvas"].call<emscripten::val>("getContext", std::string("webgl2")).call<void>("texSubImage2D", GL_TEXTURE_2D, 0, 0, 0, GL_RGB, GL_UNSIGNED_BYTE, mMedia);
                }*/
            }

            // mTimeupdate = false;
            CheckReady();
        }

    private:
    };
}

#endif
#endif