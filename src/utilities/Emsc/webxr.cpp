#ifdef __EMSCRIPTEN__
#include "webxr.h"
#include <core/Application.h>
#include <glm/gtc/type_ptr.hpp>

EMSCRIPTEN_BINDINGS(WebXR)
{
    emscripten::function("WXROnSessionStarted", &WebXR::OnSessionStarted);
    emscripten::function("WXROnFrame", &WebXR::OnFrame);
    emscripten::function("WXROnSessionEnd", &WebXR::OnSessionEnd);
    emscripten::function("WXROnError", &WebXR::OnError);
    emscripten::function("WXROnSelect", &WebXR::OnSelect);
    emscripten::function("WXROnSelectStart", &WebXR::OnSelectStart);
    emscripten::function("WXROnSelectEnd", &WebXR::OnSelectEnd);
    emscripten::function("WXROnGotLocalFloorSpace", &WebXR::OnGotLocalFloorSpace);
    emscripten::function("WXROnFailedLocalFloorSpace", &WebXR::OnFailedLocalFloorSpace);
    emscripten::function("WXROnGotViewerSpace", &WebXR::OnGotViewerSpace);
    emscripten::function("WXROnRequestSession", &WebXR::OnRequestSession);
};

util::EMObject WebXR::mNavigator = util::EMObject::Global("navigator");
util::EMObject WebXR::mXR = WebXR::mNavigator["xr"];
emscripten::val WebXR::mXRSession = emscripten::val::undefined();
emscripten::val WebXR::mCanvas = emscripten::val::undefined();
emscripten::val WebXR::mRenderContext = emscripten::val::undefined();
emscripten::val WebXR::mRefSpace = emscripten::val::undefined();

core::Application *WebXR::mApplication = nullptr;
constexpr uint32_t WebXR::mMaxSourcesCount;

WebXRInputSource WebXR::mXRLeftInputSource = {};
WebXRInputSource WebXR::mXRRightInputSource = {};
WebXRRigidTransform WebXR::mHeadPose = {};
WebXRView WebXR::mLeftEyeView = {};
WebXRView WebXR::mRightEyeView = {};

const WebXRInputSource &WebXR::GetLeftInputSource() { return mXRLeftInputSource; }
const WebXRInputSource &WebXR::GetRightInputSource() { return mXRRightInputSource; }
const WebXRRigidTransform &WebXR::GetHeadPose() { return mHeadPose; }
const WebXRView &WebXR::GetLeftEyeView() { return mLeftEyeView; }
const WebXRView &WebXR::GetRightEyeView() { return mRightEyeView; }

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
    makeXRCompatiblePromise.call<void>("then", emscripten::val::module_property("WXROnRequestSession"), emscripten::val::module_property("WXROnError"));
}

void WebXR::OnSessionStarted(emscripten::val session)
{
    mXRSession = session;
    mXRSession.call<void>("addEventListener", std::string("end"), emscripten::val::module_property("WXROnSessionEnd"));
    mXRSession.call<void>("addEventListener", std::string("select"), emscripten::val::module_property("WXROnSelect"));
    mXRSession.call<void>("addEventListener", std::string("selectstart"), emscripten::val::module_property("WXROnSelectStart"));
    mXRSession.call<void>("addEventListener", std::string("selectend"), emscripten::val::module_property("WXROnSelectEnd"));

    printf("WebXR::OnSessionStarted\n");

    printf("WebXR::OnSessionStarted getContext\n");
    emscripten::val::global("console").call<void>("log", mRenderContext);
    emscripten::val::global("console").call<void>("log", mXRSession);

    // Scene Initialize
    mApplication->OnInitialize();

    if (mApplication->ResizeGLFWWindow() == EXIT_FAILURE)
        return;

    emscripten::val glLayer = emscripten::val::global("XRWebGLLayer").new_(mXRSession, mRenderContext);
    printf("WebXR::OnSessionStarted XRWebGLLayer\n");

    emscripten::val::global("console").call<void>("log", glLayer);

    emscripten::val state = emscripten::val::object();
    state.set("baseLayer", glLayer);

    mXRSession.call<void>("updateRenderState", state);
    printf("WebXR::OnSessionStarted updateRenderState\n");

    emscripten::val request = mXRSession.call<emscripten::val>("requestReferenceSpace", _XRReferenceSpaceType[XRReferenceSpaceType::LOCAL_FLOOR]);
    request.call<void>("then", emscripten::val::module_property("WXROnGotLocalFloorSpace"), emscripten::val::module_property("WXROnFailedLocalFloorSpace"));
}

void WebXR::OnGotLocalFloorSpace(emscripten::val refSpace)
{
    mRefSpace = refSpace;

    printf("OnGotLocalFloorSpace\n");
    uint32_t id = mXRSession.call<uint32_t>("requestAnimationFrame", emscripten::val::module_property("WXROnFrame"));
    printf("requestAnimationFrame id: %d\n", id);
}

void WebXR::OnFailedLocalFloorSpace(emscripten::val error)
{
    emscripten::val::global("console").call<void>("log", error);

    emscripten::val request = mXRSession.call<emscripten::val>("requestReferenceSpace", _XRReferenceSpaceType[XRReferenceSpaceType::VIEWER]);
    request.call<void>("then", emscripten::val::module_property("WXROnGotViewerSpace"));
}

void WebXR::OnGotViewerSpace(emscripten::val refSpace)
{
    mRefSpace = refSpace;
    printf("OnGotViewerSpace\n");
    uint32_t id = mXRSession.call<uint32_t>("requestAnimationFrame", emscripten::val::module_property("WXROnFrame"));
    printf("requestAnimationFrame id: %d\n", id);
}

static void convertJSArrayToVec2(const emscripten::val &v, glm::vec2 &vec2)
{
    const size_t l = v["length"].as<size_t>();

    assert(l == 2 && "");
    if (l != 2)
        return;

    // Copy the array into our vector through the use of typed arrays.
    // It will try to convert each element through Number().
    // See https://www.ecma-international.org/ecma-262/6.0/#sec-%typedarray%.prototype.set-array-offset
    // and https://www.ecma-international.org/ecma-262/6.0/#sec-tonumber
    emscripten::val memoryView{emscripten::typed_memory_view(l, glm::value_ptr(vec2))};
    memoryView.call<void>("set", v);
}
static void convertJSArrayToVec3(const emscripten::val &v, glm::vec3 &vec3)
{
    const size_t l = v["length"].as<size_t>();

    assert(l == 3 && "");
    if (l != 3)
        return;

    // Copy the array into our vector through the use of typed arrays.
    // It will try to convert each element through Number().
    // See https://www.ecma-international.org/ecma-262/6.0/#sec-%typedarray%.prototype.set-array-offset
    // and https://www.ecma-international.org/ecma-262/6.0/#sec-tonumber
    // emscripten::val memoryView{emscripten::typed_memory_view(l, glm::value_ptr(vec3))};
    // memoryView.call<void>("set", v);
}
static void convertJSArrayToVec4(const emscripten::val &v, glm::vec4 &vec4)
{
    const size_t l = v["length"].as<size_t>();

    assert(l == 4 && "");
    if (l != 4)
        return;

    // Copy the array into our vector through the use of typed arrays.
    // It will try to convert each element through Number().
    // See https://www.ecma-international.org/ecma-262/6.0/#sec-%typedarray%.prototype.set-array-offset
    // and https://www.ecma-international.org/ecma-262/6.0/#sec-tonumber
    emscripten::val memoryView{emscripten::typed_memory_view(l, glm::value_ptr(vec4))};
    memoryView.call<void>("set", v);
}
static void convertJSArrayToQuat(const emscripten::val &v, glm::quat &quat)
{
    const size_t l = v["length"].as<size_t>();

    assert(l == 4 && "");
    if (l != 4)
        return;

    // Copy the array into our vector through the use of typed arrays.
    // It will try to convert each element through Number().
    // See https://www.ecma-international.org/ecma-262/6.0/#sec-%typedarray%.prototype.set-array-offset
    // and https://www.ecma-international.org/ecma-262/6.0/#sec-tonumber
    emscripten::val memoryView{emscripten::typed_memory_view(l, glm::value_ptr(quat))};
    memoryView.call<void>("set", v);
}
static void convertJSArrayToMat3(const emscripten::val &v, glm::mat3 &mat3)
{
    const size_t l = v["length"].as<size_t>();

    assert(l == 9 && "");
    if (l != 9)
        return;

    // Copy the array into our vector through the use of typed arrays.
    // It will try to convert each element through Number().
    // See https://www.ecma-international.org/ecma-262/6.0/#sec-%typedarray%.prototype.set-array-offset
    // and https://www.ecma-international.org/ecma-262/6.0/#sec-tonumber
    emscripten::val memoryView{emscripten::typed_memory_view(l, glm::value_ptr(mat3))};
    memoryView.call<void>("set", v);
}
static void convertJSArrayToMat4(const emscripten::val &v, glm::mat4 &mat4)
{
    const size_t l = v["length"].as<size_t>();

    assert(l == 16 && "");
    if (l != 16)
        return;

    // Copy the array into our vector through the use of typed arrays.
    // It will try to convert each element through Number().
    // See https://www.ecma-international.org/ecma-262/6.0/#sec-%typedarray%.prototype.set-array-offset
    // and https://www.ecma-international.org/ecma-262/6.0/#sec-tonumber
    emscripten::val memoryView{emscripten::typed_memory_view(l, glm::value_ptr(mat4))};
    memoryView.call<void>("set", v);
}

void convertJSDOMPointToVec3(const emscripten::val &domPoint, glm::vec3 &vec3)
{
    vec3.x = domPoint["x"].as<float>();
    vec3.y = domPoint["y"].as<float>();
    vec3.z = domPoint["z"].as<float>();
}

void convertJSDOMPointToVec4(const emscripten::val &domPoint, glm::vec4 &vec4)
{
    vec4.x = domPoint["x"].as<float>();
    vec4.y = domPoint["y"].as<float>();
    vec4.z = domPoint["z"].as<float>();
    vec4.w = domPoint["w"].as<float>();
}

void convertJSDOMPointToQuat(const emscripten::val &domPoint, glm::quat &quat)
{
    quat.x = domPoint["x"].as<float>();
    quat.y = domPoint["y"].as<float>();
    quat.z = domPoint["z"].as<float>();
    quat.w = domPoint["w"].as<float>();
}

void WebXR::OnFrame(emscripten::val time, emscripten::val frame)
{
    emscripten::val session = frame["session"];

    emscripten::val viewPose = frame.call<emscripten::val>("getViewerPose", mRefSpace);
    // read head pose
    emscripten::val viewTransform = viewPose["transform"];
    // emscripten::val::global("console").call<void>("log", viewPose);
    emscripten::val viewPosition = viewTransform["position"];
    emscripten::val viewOrientation = viewTransform["orientation"];
    emscripten::val viewMatrix = viewTransform["matrix"];
    convertJSDOMPointToVec3(viewPosition, mHeadPose.position);
    convertJSDOMPointToQuat(viewOrientation, mHeadPose.orientation);
    convertJSArrayToMat4(viewMatrix, mHeadPose.matrix);

    emscripten::val renderState = session["renderState"];
    emscripten::val glLayer = renderState["baseLayer"];

    emscripten::val framebuffer = glLayer["framebuffer"];

    emscripten::val::global("console").call<void>("log", glLayer);
    if (!(framebuffer.isNull() || framebuffer.isUndefined()))
    {
        // emscripten::val gl = emscripten::val::global("gl");
        // gl.call<void>("bindFramebuffer", gl["FRAMEBUFFER"], framebuffer);
        mRenderContext.call<void>("bindFramebuffer", mRenderContext["FRAMEBUFFER"], framebuffer);
    }

    // read input sources (controllers) pose
    emscripten::val inputSources = session["inputSources"];
    uint32_t inputSourcesCount = inputSources["length"].as<uint32_t>();
    assert(inputSourcesCount <= mMaxSourcesCount);

    for (uint32_t i = 0; i < inputSourcesCount; i++)
    {
        WebXRInputSource *currentInputSource = nullptr;
        emscripten::val inputSource = inputSources[i];
        if (inputSource["handedness"].call<int>("localeCompare", std::string("right")) == 0)
            currentInputSource = &mXRRightInputSource;
        if (inputSource["handedness"].call<int>("localeCompare", std::string("left")) == 0)
            currentInputSource = &mXRLeftInputSource;
        // mXRInputSources[i].handedness = static_cast<XRHandedness>(inputSource["handedness"].as<uint32_t>());
        // mXRInputSources[i].targetRayMode = static_cast<XRTargetRayMode>(inputSource["targetRayMode"].as<uint32_t>());
        emscripten::val gripSpace = inputSource["gripSpace"];
        if (!(gripSpace.isNull() || gripSpace.isUndefined()))
        {
            emscripten::val gripPose = frame.call<emscripten::val>("getPose", gripSpace, mRefSpace);
            emscripten::val transform = gripPose["transform"];
            // emscripten::val::global("console").call<void>("log", gripSpace);
            emscripten::val position = transform["position"];
            emscripten::val orientation = transform["orientation"];
            emscripten::val matrix = transform["matrix"];

            convertJSDOMPointToVec3(position, currentInputSource->rigidTransform.position);
            convertJSDOMPointToQuat(orientation, currentInputSource->rigidTransform.orientation);
            convertJSArrayToMat4(matrix, currentInputSource->rigidTransform.matrix);
        }
    }

    // render twice left and right eye
    emscripten::val views = viewPose["views"];
    uint32_t viewCount = views["length"].as<uint32_t>();
    assert(viewCount == 2);
    for (uint32_t i = 0; i < viewCount; i++)
    {
        const emscripten::val xrView = views[i];
        const emscripten::val transform = xrView["transform"];
        const emscripten::val projectionMatrix = xrView["projectionMatrix"];
        const emscripten::val viewport = glLayer.call<emscripten::val>("getViewport", xrView);

        const emscripten::val position = transform["position"];
        const emscripten::val orientation = transform["orientation"];
        const emscripten::val matrix = transform["matrix"];

        WebXRView *view = nullptr;
        const emscripten::val eye = xrView["eye"];
        if (xrView["eye"].call<int>("localeCompare", std::string("right")) == 0)
            view = &mRightEyeView;
        if (xrView["eye"].call<int>("localeCompare", std::string("left")) == 0)
            view = &mLeftEyeView;

        convertJSDOMPointToVec3(position, view->viewPose.position);
        convertJSDOMPointToQuat(orientation, view->viewPose.orientation);
        convertJSArrayToMat4(matrix, view->viewPose.matrix);
        convertJSArrayToMat4(projectionMatrix, view->projectionMatrix);

        view->viewport.x = viewport["x"].as<uint32_t>();
        view->viewport.y = viewport["y"].as<uint32_t>();
        view->viewport.width = viewport["width"].as<uint32_t>();
        view->viewport.height = viewport["height"].as<uint32_t>();
    }

    printf("WebXR::OnFrame\n");
    // mFrameCallBack();

    // Scene Update
    core::Application::OnUpdate(mApplication);

    uint32_t id = session.call<uint32_t>("requestAnimationFrame", emscripten::val::module_property("WXROnFrame"));
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
    promise.call<void>("then", emscripten::val::module_property("WXROnSessionStarted"), emscripten::val::module_property("WXROnError"));
}

void WebXR::RequestSession(XRSessionMode mode, XRSessionFeatures required)
{
    emscripten::val option = emscripten::val::object();
    option.set("requiredFeatures", _XRSessionFeatures[required]);
    emscripten::val promise = mXR.GetJSObject().call<emscripten::val>("requestSession", _XRSessionMode[mode], option);
    promise.call<void>("then", emscripten::val::module_property("WXROnSessionStarted"), emscripten::val::module_property("WXROnError"));
}

void WebXR::RequestSession(XRSessionMode mode, XRSessionFeatures required, XRSessionFeatures optional)
{
    emscripten::val option = emscripten::val::object();
    option.set("requiredFeatures", _XRSessionFeatures[required]);
    option.set("optionalFeatures", _XRSessionFeatures[optional]);
    emscripten::val promise = mXR.GetJSObject().call<emscripten::val>("requestSession", _XRSessionMode[mode], option);
    promise.call<void>("then", emscripten::val::module_property("WXROnSessionStarted"), emscripten::val::module_property("WXROnError"));
}

#endif