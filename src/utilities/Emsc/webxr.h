#ifdef ___________
#ifdef __EMSCRIPTEN__
#ifndef _WEBXR_H_
#define _WEBXR_H_

#include <core/Platform.h>
#include "EMObject.h"
#include <map>
#include <string>
#include "utilities/Events/CallBack.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace core
{
    class Application;
}

/** WebXR 'XRSessionMode' enum*/
enum struct XRSessionMode
{
    INLINE,
    IMMERSIVE_VR,
    IMMERSIVE_AR
};

/** WebXR 'XRReferenceSpaceType' enum*/
enum struct XRReferenceSpaceType
{
    VIEWER,
    LOCAL,
    LOCAL_FLOOR,
    BOUNDED_FLOOR,
    UNBOUNDED
};

enum struct XRSessionFeatures
{
    ANCHOR,
    DEPTH_SENSING,
    DOM_OVERLAY,
    HAND_TRACKING,
    HIT_TEST,
    LAYERS,
    LIGHT_ESTIMATION,
    LOCAL,
    LOCAL_FLOOR,
    BOUNDED_FLOOR,
    SECONDARY_VIEWS,
    UNBOUNDED,
    VIEWER
};

/** WebXR current eye */
enum struct XREye
{
    NONE,
    LEFT,
    RIGHT
};

/** WebXR handedness */
enum struct XRHandedness
{
    NONE,
    LEFT,
    RIGHT
};

/** WebXR target ray mode */
enum struct XRTargetRayMode
{
    GAZE,
    TRACKED_POINTER,
    SCREEN
};

/** WebXR 'XRSessionMode' enum*/
enum WebXRInputPoseMode
{
    WEBXR_INPUT_POSE_GRIP = 0,       /** gripSpace */
    WEBXR_INPUT_POSE_TARGET_RAY = 1, /** targetRaySpace */
};

/** WebXR rigid transform */
typedef struct WebXRRigidTransform
{
    glm::mat4 matrix;
    glm::vec3 position;
    glm::quat orientation;
} WebXRRigidTransform;

typedef struct WebXRViewport
{
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
} WebXRViewport;

/** WebXR view */
typedef struct WebXRView
{
    XREye eye;
    /* view pose */
    WebXRRigidTransform viewPose;
    /* projection matrix */
    glm::mat4 projectionMatrix;
    /* x, y, width, height of the eye viewport on target texture */
    WebXRViewport viewport;
} WebXRView;

typedef struct WebXRInputSource
{
    WebXRRigidTransform rigidTransform;
    XRHandedness handedness;
    XRTargetRayMode targetRayMode;
} WebXRInputSource;

class WebXR
{
public:
    using SessionStartedCallBack = util::CallBack<>;
    using FrameCallBack = util::CallBack<>;
    using SessionEndCallBack = util::CallBack<>;
    using ErrorCallBack = util::CallBack<>;

    static SessionStartedCallBack mSessionStartedCallBack;
    static FrameCallBack mFrameCallBack;
    static SessionEndCallBack mSessionEndCallBack;
    static ErrorCallBack mErrorCallBack;

private:
    static std::map<XRSessionMode, std::string> _XRSessionMode;
    static std::map<XRSessionFeatures, std::string> _XRSessionFeatures;
    static std::map<XRReferenceSpaceType, std::string> _XRReferenceSpaceType;
    static std::map<XREye, std::string> _XREye;
    static std::map<XRHandedness, std::string> _XRHandedness;
    static std::map<XRTargetRayMode, std::string> _XRTargetRayMode;
    /* data */

    static emscripten::val mNavigator;
    static emscripten::val mXR;
    static emscripten::val mXRSession;
    static emscripten::val mXRFrame;
    static emscripten::val mXRInputSources;
    static emscripten::val mXRViews;
    static emscripten::val mXRGLLayer;
    static emscripten::val mCanvas;
    static emscripten::val mRenderContext;

    static emscripten::val mRefSpace;

    static core::Application *mApplication;
    static constexpr uint32_t MAX_SOURCE_COUNT = 4;
    static constexpr uint32_t MAX_VIEW_COUNT = 2;

    // static uint32_t mSourceCount;
    //  static uint32_t mEyeCount;
    static WebXRInputSource mInputSources[MAX_SOURCE_COUNT];
    static WebXRRigidTransform mHeadPose;
    static WebXRView mViews[MAX_VIEW_COUNT];

public:
    WebXR(/* args */);
    ~WebXR();

    static void Start(core::Application *application);

    static bool IsWebXRSupported();
    static bool IsSessionSupported(XRSessionMode mode);

    static void RequestSession(XRSessionMode mode);
    static void RequestSession(XRSessionMode mode, XRSessionFeatures required);
    static void RequestSession(XRSessionMode mode, XRSessionFeatures required, XRSessionFeatures optional);

    static const WebXRRigidTransform &GetHeadPose();

    static bool GetViews(WebXRView **sourceArray, uint32_t *arraySize);
    static bool GetInputSources(WebXRInputSource **sourceArray, uint32_t *arraySize, WebXRInputPoseMode mode = WEBXR_INPUT_POSE_GRIP);
    static emscripten::val &GetGLLayer();
    static emscripten::val GetGLRenderContext();
    static emscripten::val GetSession();

    static void OnMakeXRCompatible(emscripten::val event);

    static void OnSessionStarted(emscripten::val session);
    static void OnFrame(emscripten::val time, emscripten::val frame);
    static void OnSessionEnd(emscripten::val session);
    static void OnError(emscripten::val error);

    static void OnSelect(emscripten::val event);
    static void OnSelectStart(emscripten::val event);
    static void OnSelectEnd(emscripten::val event);

    static void OnGotLocalFloorSpace(emscripten::val refSpace);
    static void OnFailedLocalFloorSpace(emscripten::val error);
    static void OnGotViewerSpace(emscripten::val refSpace);
};

#endif
#else //__EMSCRIPTEN__
#ifndef __EMSCRIPTEN__
typedef void *WebXRInputSource;
#endif
#endif
#endif
