#ifdef __EMSCRIPTEN__
#include "webxr.h"
#include <core/Application.h>

EMSCRIPTEN_BINDINGS(WebXR)
{
    emscripten::function("OnSessionStarted", &WebXR::OnSessionStarted);
    emscripten::function("OnFrame", &WebXR::OnFrame);
    emscripten::function("OnSessionEnd", &WebXR::OnSessionEnd);
    emscripten::function("OnError", &WebXR::OnError);
    emscripten::function("OnSelect", &WebXR::OnSelect);
    emscripten::function("OnSelectStart", &WebXR::OnSelectStart);
    emscripten::function("OnSelectEnd", &WebXR::OnSelectEnd);
    emscripten::function("OnGotLocalFloorSpace", &WebXR::OnGotLocalFloorSpace);
    emscripten::function("OnFailedLocalFloorSpace", &WebXR::OnFailedLocalFloorSpace);
    emscripten::function("OnGotViewerSpace", &WebXR::OnGotViewerSpace);
    emscripten::function("OnRequestSession", &WebXR::OnRequestSession);
};

util::EMObject WebXR::mNavigator = util::EMObject::Global("navigator");
util::EMObject WebXR::mXR = WebXR::mNavigator["xr"];
emscripten::val WebXR::mXRSession = emscripten::val::undefined();
emscripten::val WebXR::mCanvas = emscripten::val::undefined();
emscripten::val WebXR::mRenderContext = emscripten::val::undefined();

core::Application *WebXR::mApplication = nullptr;

WebXR::SessionStartedCallBack WebXR::mSessionStartedCallBack;
WebXR::FrameCallBack WebXR::mFrameCallBack;
WebXR::SessionEndCallBack WebXR::mSessionEndCallBack;
WebXR::ErrorCallBack WebXR::mErrorCallBack;

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

void WebXR::OnRequestSession(emscripten::val event)
{
    if (WebXR::IsSessionSupported(XRSessionMode::IMMERSIVE_VR))
    {
        RequestSession(XRSessionMode::IMMERSIVE_VR, XRSessionFeatures::LOCAL_FLOOR);
    }
    else
    {
        RequestSession(XRSessionMode::INLINE);
    }
}

void WebXR::Start(core::Application *application)
{
    mApplication = application;

    emscripten::val document = emscripten::val::global("document");
    mCanvas = document.call<emscripten::val>("getElementById", std::string("canvas"));

    mRenderContext = mCanvas.call<emscripten::val>("getContext", std::string("webgl2"));
    if (mRenderContext.isNull() || mRenderContext.isUndefined())
        mRenderContext = mCanvas.call<emscripten::val>("getContext", std::string("webgl"));

    emscripten::val makeXRCompatiblePromise = mRenderContext.call<emscripten::val>("makeXRCompatible");
    makeXRCompatiblePromise.call<void>("then", emscripten::val::module_property("OnRequestSession"), emscripten::val::module_property("OnError"));
}

void WebXR::OnSessionStarted(emscripten::val session)
{
    mXRSession = session;
    mXRSession.call<void>("addEventListener", std::string("end"), emscripten::val::module_property("OnSessionEnd"));
    mXRSession.call<void>("addEventListener", std::string("select"), emscripten::val::module_property("OnSelect"));
    mXRSession.call<void>("addEventListener", std::string("selectstart"), emscripten::val::module_property("OnSelectStart"));
    mXRSession.call<void>("addEventListener", std::string("selectend"), emscripten::val::module_property("OnSelectEnd"));

    printf("WebXR::OnSessionStarted\n");

    printf("WebXR::OnSessionStarted getContext\n");
    emscripten::val::global("console").call<void>("log", mRenderContext);
    emscripten::val::global("console").call<void>("log", mXRSession);

    // Scene Initialize
    mApplication->OnInitialize();

    emscripten::val glLayer = emscripten::val::global("XRWebGLLayer").new_(mXRSession, mRenderContext);
    printf("WebXR::OnSessionStarted XRWebGLLayer\n");

    emscripten::val::global("console").call<void>("log", glLayer);

    emscripten::val state = emscripten::val::object();
    state.set("baseLayer", glLayer);

    mXRSession.call<void>("updateRenderState", state);
    printf("WebXR::OnSessionStarted updateRenderState\n");

    emscripten::val request = mXRSession.call<emscripten::val>("requestReferenceSpace", _XRReferenceSpaceType[XRReferenceSpaceType::LOCAL_FLOOR]);
    request.call<void>("then", emscripten::val::module_property("OnGotLocalFloorSpace"), emscripten::val::module_property("OnFailedLocalFloorSpace"));
}

void WebXR::OnGotLocalFloorSpace(emscripten::val refSpace)
{
    printf("OnGotLocalFloorSpace\n");
    uint32_t id = mXRSession.call<uint32_t>("requestAnimationFrame", emscripten::val::module_property("OnFrame"));
    printf("requestAnimationFrame id: %d\n", id);
}

void WebXR::OnFailedLocalFloorSpace(emscripten::val error)
{
    emscripten::val::global("console").call<void>("log", error);

    emscripten::val request = mXRSession.call<emscripten::val>("requestReferenceSpace", _XRReferenceSpaceType[XRReferenceSpaceType::VIEWER]);
    request.call<void>("then", emscripten::val::module_property("OnGotViewerSpace"));
}

void WebXR::OnGotViewerSpace(emscripten::val refSpace)
{
    printf("OnGotViewerSpace\n");
    uint32_t id = mXRSession.call<uint32_t>("requestAnimationFrame", emscripten::val::module_property("OnFrame"));
    printf("requestAnimationFrame id: %d\n", id);
}

void WebXR::OnFrame(emscripten::val time, emscripten::val frame)
{
    emscripten::val session = frame["session"];

    printf("WebXR::OnFrame\n");
    // mFrameCallBack();

    // Scene Update
    core::Application::OnUpdate(mApplication);

    // session.requestAnimationFrame(OnFrame);
    uint32_t id = session.call<uint32_t>("requestAnimationFrame", emscripten::val::module_property("OnFrame"));
    if (id == 0)
        printf("failed requestAnimationFrame id: %d\n", id);
}
void WebXR::OnSessionEnd(emscripten::val session)
{

    // Scene Finalize
    mApplication->OnFinalize();

    printf("WebXR::OnSessionEnd\n");
    session.call<void>("end");
    mXRSession = emscripten::val::undefined();
    // mSessionEndCallBack();
}

void WebXR::OnError(emscripten::val error)
{
    emscripten::val::global("console").call<void>("log", error);
    printf("WebXR::OnError\n");
    // mErrorCallBack();
}

void WebXR::OnSelect(emscripten::val event)
{
    printf("WebXR::OnSelect\n");
    /*let session = ev.frame.session;
    let refSpace = getRefSpace(session, true);

    let headPose = ev.frame.getPose(xrViewerSpaces[session.mode], refSpace);

    if (!headPose)
        return;*/
}
void WebXR::OnSelectStart(emscripten::val event)
{

    printf("WebXR::OnSelectstart\n");
}
void WebXR::OnSelectEnd(emscripten::val event)
{
    printf("WebXR::OnSelectend\n");
}

bool WebXR::IsSessionSupported(XRSessionMode mode)
{
    return mXR.CallAwait("isSessionSupported", _XRSessionMode[mode]).GetAs<bool>();
}

void WebXR::RequestSession(XRSessionMode mode)
{
    emscripten::val promise = mXR.GetJSObject().call<emscripten::val>("requestSession", _XRSessionMode[mode]);
    promise.call<void>("then", emscripten::val::module_property("OnSessionStarted"), emscripten::val::module_property("OnError"));
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