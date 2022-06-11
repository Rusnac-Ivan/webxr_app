#ifdef __EMSCRIPTEN__
#include "webxr.h"

EMSCRIPTEN_BINDINGS(WebXR)
{
    emscripten::function("OnSessionStarted", &webxr::OnSessionStarted);
    emscripten::function("OnFrame", &webxr::OnFrame);
    emscripten::function("OnSessionEnd", &webxr::OnSessionEnd);
    emscripten::function("OnError", &webxr::OnError);
};

util::EMObject WebXR::mNavigator = util::EMObject::Global("navigator");
util::EMObject WebXR::mXR = WebXR::mNavigator["xr"];

SessionStartedCallBack WebXR::mSessionStartedCallBack;
FrameCallBack WebXR::mFrameCallBack;
SessionEndCallBack WebXR::mSessionEndCallBack;
ErrorCallBack WebXR::mErrorCallBack;

std::map<XRSessionMode, std::string> WebXR::_XRSessionMode = {
    {XRSessionMode::INLINE, "inline"},
    {XRSessionMode::IMMERSIVE_VR, "immersive-vr"},
    {XRSessionMode::IMMERSIVE_AR, "immersive-ar"},
};

std::map<XRReferenceSpaceType, std::string> WebXR::_XRReferenceSpaceType = {
    {XRReferenceSpaceType::VIEWER, "viewer"},
    {XRReferenceSpaceType::LOCAL, "local"},
    {XRReferenceSpaceType::LOCAL_FLOOR, "local-floor"},
    {XRReferenceSpaceType::BOUNDED_FLOOR, "bounded-floor"},
    {XRReferenceSpaceType::UNBOUNDED, "unbounded"},
};

std::map<XREye, std::string> WebXR::_XREye = {
    {XREye::NONE, "none"},
    {XREye::LEFT, "left"},
    {XREye::RIGHT, "right"},
};

std::map<XRHandedness, std::string> WebXR::_XRHandedness = {
    {XRHandedness::NONE, "none"},
    {XRHandedness::LEFT, "left"},
    {XRHandedness::RIGHT, "right"},
};

std::map<XRTargetRayMode, std::string> WebXR::_XRTargetRayMode = {
    {XRTargetRayMode::GAZE, "gaze"},
    {XRTargetRayMode::TRACKED_POINTER, "tracked-pointer"},
    {XRTargetRayMode::SCREEN, "screen"},
};

std::map<XRSessionFeatures, std::string> WebXR::_XRSessionFeatures = {
    {XRSessionFeatures::ANCHOR, "anchor"},
    {XRSessionFeatures::DEPTH_SENSING, "depth-sensing"},
    {XRSessionFeatures::DOM_OVERLAY, "dom-overlay"},
    {XRSessionFeatures::HAND_TRACKING, "hand-tracking"},
    {XRSessionFeatures::HIT_TEST, "hit-test"},
    {XRSessionFeatures::LAYERS, "layers"},
    {XRSessionFeatures::LIGHT_ESTIMATION, "light-estimation"},
    {XRSessionFeatures::LOCAL, "local"},
    {XRSessionFeatures::LOCAL_FLOOR, "local-floor"},
    {XRSessionFeatures::BOUNDED_FLOOR, "bounded-floor"},
    {XRSessionFeatures::SECONDARY_VIEWS, "secondary-views"},
    {XRSessionFeatures::UNBOUNDED, "unbounded"},
    {XRSessionFeatures::VIEWER, "viewer"},
};

WebXR::WebXR(/* args */)
{
}

WebXR::~WebXR()
{
}

void WebXR::ConsoleLogXR()
{
    mXR.ConsoleLog();
}

void WebXR::ConsoleLogNavigator()
{
    mNavigator.ConsoleLog();
}

void Init(SessionStartedCallBack start_ckb, FrameCallBack frame_ckb, SessionEndCallBack end_ckb, ErrorCallBack error_ckb)
{
    mSessionStartedCallBack = start_ckb;
    mFrameCallBack = frame_ckb;
    mSessionEndCallBack = end_ckb;
    mErrorCallBack = error_ckb;
}

void WebXR::OnSessionStarted(emscripten::val session)
{
    mSessionStartedCallBack();
}
void WebXR::OnFrame(emscripten::val time, emscripten::val frame)
{
    mFrameCallBack();
}
void WebXR::OnSessionEnd(emscripten::val event)
{
    mSessionEndCallBack();
}
void WebXR::OnError(emscripten::val error)
{
    mErrorCallBack();
}

bool WebXR::IsSessionSupported(XRSessionMode mode)
{
    return mXR.CallAwait("isSessionSupported", _XRSessionMode[mode]).GetAs<bool>();
}

void WebXR::RequestSession(XRSessionMode mode, XRSessionFeatures required)
{
    emscripten::val option = emscripten::val::object();
    option.set("requiredFeatures", _XRSessionFeatures[required]);
    emscripten::val promise = mXR.GetJSObject().call<emscripten::val>("requestSession", _XRSessionMode[mode], option);
    promise.call<void>("then", emscripten::val::module_property("OnSessionStarted"), emscripten::val::module_property("OnError"));
}

void WebXR::RequestSession(XRSessionMode mode, XRSessionFeatures required, XRSessionFeatures optional)
{
    emscripten::val option = emscripten::val::object();
    option.set("requiredFeatures", _XRSessionFeatures[required]);
    option.set("optionalFeatures", _XRSessionFeatures[optional]);
    emscripten::val promise = mXR.GetJSObject().call<emscripten::val>("requestSession", _XRSessionMode[mode], option);
    promise.call<void>("then", emscripten::val::module_property("OnSessionStarted"), emscripten::val::module_property("OnError"));
}

#endif