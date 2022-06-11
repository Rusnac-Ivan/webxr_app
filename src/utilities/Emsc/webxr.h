#ifdef __EMSCRIPTEN__
#ifndef _WEBXR_H_
#define _WEBXR_H_

#include <core/Platform.h>
#include "EMObject.h"
#include <map>
#include <string>
#include "utilities/Events/CallBack.h"

/** WebXR 'XRSessionMode' enum*/
enum struct XRSessionMode
{
    INLINE,
    IMMERSIVE_VR,
    IMMERSIVE_AR
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

/** WebXR 'XRReferenceSpaceType' enum*/
enum struct XRReferenceSpaceType
{
    VIEWER,
    LOCAL,
    LOCAL_FLOOR,
    BOUNDED_FLOOR,
    UNBOUNDED
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

    /** WebXR rigid transform */
    typedef struct WebXRRigidTransform
    {
        float matrix[16];
        float position[3];
        float orientation[4];
    } WebXRRigidTransform;

    /** WebXR view */
    typedef struct WebXRView
    {
        /* view pose */
        WebXRRigidTransform viewPose;
        /* projection matrix */
        float projectionMatrix[16];
        /* x, y, width, height of the eye viewport on target texture */
        int viewport[4];
    } WebXRView;

    typedef struct WebXRInputSource
    {
        int id;
        XRHandedness handedness;
        XRTargetRayMode targetRayMode;
    } WebXRInputSource;

private:
    static std::map<XRSessionMode, std::string> _XRSessionMode;
    static std::map<XRSessionFeatures, std::string> _XRSessionFeatures;
    static std::map<XRReferenceSpaceType, std::string> _XRReferenceSpaceType;
    static std::map<XREye, std::string> _XREye;
    static std::map<XRHandedness, std::string> _XRHandedness;
    static std::map<XRTargetRayMode, std::string> _XRTargetRayMode;
    /* data */

    static util::EMObject mNavigator;
    static util::EMObject mXR;

public:
    WebXR(/* args */);
    ~WebXR();

    static void Init(SessionStartedCallBack start_ckb, FrameCallBack frame_ckb, SessionEndCallBack end_ckb, ErrorCallBack error_ckb);

    static bool IsWebXRSupported() { return mXR.IsValid(); }
    static bool IsSessionSupported(XRSessionMode mode);
    static void RequestSession(XRSessionMode mode, XRSessionFeatures required);
    static void RequestSession(XRSessionMode mode, XRSessionFeatures required, XRSessionFeatures optional);

    static void ConsoleLogXR();
    static void ConsoleLogNavigator();

private:
    static void OnSessionStarted(emscripten::val session);
    static void OnFrame(emscripten::val time, emscripten::val frame);
    static void OnSessionEnd(emscripten::val event);
    static void OnError(emscripten::val error);
};

#endif
#endif //__EMSCRIPTEN__