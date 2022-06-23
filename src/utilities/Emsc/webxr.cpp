#ifdef __EMSCRIPTEN__
#include "webxr.h"
#include <core/Application.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <widgets3d/ImGui_Impl_2d_to_3d.h>

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

emscripten::val WebXR::mNavigator = emscripten::val::global("navigator");
emscripten::val WebXR::mXR = emscripten::val::undefined();
emscripten::val WebXR::mXRSession = emscripten::val::undefined();
emscripten::val WebXR::mXRFrame = emscripten::val::undefined();
emscripten::val WebXR::mCanvas = emscripten::val::undefined();
emscripten::val WebXR::mRenderContext = emscripten::val::undefined();
emscripten::val WebXR::mRefSpace = emscripten::val::undefined();
emscripten::val WebXR::mXRInputSources = emscripten::val::undefined();
emscripten::val WebXR::mXRViews = emscripten::val::undefined();
emscripten::val WebXR::XRGLLayer = emscripten::val::undefined();

core::Application *WebXR::mApplication = nullptr;
constexpr uint32_t WebXR::MAX_SOURCE_COUNT;

// uint32_t WebXR::mEyeCount = 0;
// WebXRInputSource WebXR::mXRInputSources[WebXR::mMaxSourcesCount] = {};
WebXRInputSource WebXR::mInputSources[MAX_SOURCE_COUNT] = {};
WebXRRigidTransform WebXR::mHeadPose = {};
WebXRView WebXR::mViews[MAX_VIEW_COUNT] = {};

// const WebXRInputSource &WebXR::GetLeftInputSource() { return mXRLeftInputSource; }
// const WebXRInputSource &WebXR::GetRightInputSource() { return mXRRightInputSource; }
const WebXRRigidTransform &WebXR::GetHeadPose() { return mHeadPose; }
// const WebXRView &WebXR::GetLeftEyeView() { return mLeftEyeView; }
// const WebXRView &WebXR::GetRightEyeView() { return mRightEyeView; }

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

bool WebXR::IsWebXRSupported()
{
    if (mNavigator.hasOwnProperty("xr"))
    {
        mXR = mNavigator["xr"];
        return true;
    }
    else
        return false;
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

void WebXR::GetInputSources(WebXRInputSource **sourceArray, uint32_t *arraySize, WebXRInputPoseMode mode)
{
    // emscripten::val::global("console").call<void>("log", mXRInputSources);
    uint32_t inputSourcesCount = mXRInputSources["length"].as<uint32_t>();
    assert(inputSourcesCount <= MAX_SOURCE_COUNT);

    for (uint32_t i = 0; i < inputSourcesCount; i++)
    {
        emscripten::val inputSource = mXRInputSources[i];

        if (inputSource["handedness"].call<int>("localeCompare", std::string("right")) == 0)
            mInputSources[i].handedness = XRHandedness::RIGHT;
        else if (inputSource["handedness"].call<int>("localeCompare", std::string("left")) == 0)
            mInputSources[i].handedness = XRHandedness::LEFT;
        else
            mInputSources[i].handedness = XRHandedness::NONE;

        if (inputSource["handedness"].call<int>("localeCompare", std::string("gaze")) == 0)
            mInputSources[i].targetRayMode = XRTargetRayMode::GAZE;
        else if (inputSource["handedness"].call<int>("localeCompare", std::string("tracked-pointer")) == 0)
            mInputSources[i].targetRayMode = XRTargetRayMode::TRACKED_POINTER;
        else
            mInputSources[i].targetRayMode = XRTargetRayMode::SCREEN;

        if (mode == WEBXR_INPUT_POSE_GRIP)
        {
            emscripten::val gripSpace = inputSource["gripSpace"];
            if (!(gripSpace.isNull() || gripSpace.isUndefined()))
            {
                emscripten::val gripPose = mXRFrame.call<emscripten::val>("getPose", gripSpace, mRefSpace);
                emscripten::val transform = gripPose["transform"];
                emscripten::val position = transform["position"];
                emscripten::val orientation = transform["orientation"];
                emscripten::val matrix = transform["matrix"];

                convertJSDOMPointToVec3(position, mInputSources[i].rigidTransform.position);
                convertJSDOMPointToQuat(orientation, mInputSources[i].rigidTransform.orientation);
                convertJSArrayToMat4(matrix, mInputSources[i].rigidTransform.matrix);
            }
        }
        if (mode == WEBXR_INPUT_POSE_TARGET_RAY)
        {
            emscripten::val targetRaySpace = inputSource["targetRaySpace"];
            if (!(targetRaySpace.isNull() || targetRaySpace.isUndefined()))
            {
                emscripten::val targetRayPose = mXRFrame.call<emscripten::val>("getPose", targetRaySpace, mRefSpace);
                emscripten::val transform = targetRayPose["transform"];
                emscripten::val position = transform["position"];
                emscripten::val orientation = transform["orientation"];
                emscripten::val matrix = transform["matrix"];

                convertJSDOMPointToVec3(position, mInputSources[i].rigidTransform.position);
                convertJSDOMPointToQuat(orientation, mInputSources[i].rigidTransform.orientation);
                convertJSArrayToMat4(matrix, mInputSources[i].rigidTransform.matrix);
            }
        }
    }

    *sourceArray = mInputSources;
    *arraySize = inputSourcesCount;
}

void WebXR::GetViews(WebXRView **sourceArray, uint32_t *arraySize)
{
    uint32_t viewCount = mXRViews["length"].as<uint32_t>();
    assert(viewCount == MAX_VIEW_COUNT);
    for (uint32_t i = 0; i < viewCount; i++)
    {
        const emscripten::val xrView = mXRViews[i];
        const emscripten::val transform = xrView["transform"];
        const emscripten::val projectionMatrix = xrView["projectionMatrix"];
        const emscripten::val viewport = XRGLLayer.call<emscripten::val>("getViewport", xrView);

        const emscripten::val position = transform["position"];
        const emscripten::val orientation = transform["orientation"];
        const emscripten::val matrix = transform["matrix"];

        const emscripten::val eye = xrView["eye"];
        if (xrView["eye"].call<int>("localeCompare", std::string("right")) == 0)
            mViews[i].eye = XREye::RIGHT;
        else if (xrView["eye"].call<int>("localeCompare", std::string("left")) == 0)
            mViews[i].eye = XREye::LEFT;
        else
            mViews[i].eye = XREye::NONE;

        convertJSDOMPointToVec3(position, mViews[i].viewPose.position);
        convertJSDOMPointToQuat(orientation, mViews[i].viewPose.orientation);
        // convertJSArrayToMat4(matrix, mViews[i].viewPose.matrix);
        mViews[i].viewPose.matrix = glm::transpose(glm::toMat4(mViews[i].viewPose.orientation)) * glm::translate(glm::mat4(1.f), -mViews[i].viewPose.position);

        convertJSArrayToMat4(projectionMatrix, mViews[i].projectionMatrix);

        mViews[i].viewport.x = viewport["x"].as<uint32_t>();
        mViews[i].viewport.y = viewport["y"].as<uint32_t>();
        mViews[i].viewport.width = viewport["width"].as<uint32_t>();
        mViews[i].viewport.height = viewport["height"].as<uint32_t>();
    }
    *sourceArray = mViews;
    *arraySize = viewCount;
}

void WebXR::OnFrame(emscripten::val time, emscripten::val frame)
{
    mXRFrame = frame;
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
    XRGLLayer = renderState["baseLayer"];

    emscripten::val framebuffer = XRGLLayer["framebuffer"];

    // emscripten::val::global("console").call<void>("log", XRGLLayer);
    if (!(framebuffer.isNull() || framebuffer.isUndefined()))
    {
        // emscripten::val gl = emscripten::val::global("gl");
        // gl.call<void>("bindFramebuffer", gl["FRAMEBUFFER"], framebuffer);
        mRenderContext.call<void>("bindFramebuffer", mRenderContext["FRAMEBUFFER"], framebuffer);
    }

    mXRViews = viewPose["views"];
    mXRInputSources = session["inputSources"];

    uint32_t id = session.call<uint32_t>("requestAnimationFrame", emscripten::val::module_property("WXROnFrame"));
    if (id == 0)
        printf("failed requestAnimationFrame id: %d\n", id);

    // Scene Update
    core::Application::OnUpdate(mApplication);
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
    emscripten::val::global("console").call<void>("log", event);
    /*let session = ev.frame.session;
    let refSpace = getRefSpace(session, true);

    let headPose = ev.frame.getPose(xrViewerSpaces[session.mode], refSpace);

    if (!headPose)
        return;*/
}
void WebXR::OnSelectStart(emscripten::val event)
{
    printf("WebXR::OnSelectstart\n");
    emscripten::val::global("console").call<void>("log", event);
    ImGui_Impl_2d_to_3d_MouseButtonCallback(MOUSEBUTTON_LEFT, PRESS, 0);
}
void WebXR::OnSelectEnd(emscripten::val event)
{
    printf("WebXR::OnSelectend\n");
    emscripten::val::global("console").call<void>("log", event);
    ImGui_Impl_2d_to_3d_MouseButtonCallback(MOUSEBUTTON_LEFT, RELEASE, 0);
}

bool WebXR::IsSessionSupported(XRSessionMode mode)
{
    return mXR.call<emscripten::val>("isSessionSupported", _XRSessionMode[mode]).await().as<bool>();
}

void WebXR::RequestSession(XRSessionMode mode)
{
    emscripten::val promise = mXR.call<emscripten::val>("requestSession", _XRSessionMode[mode]);
    promise.call<void>("then", emscripten::val::module_property("WXROnSessionStarted"), emscripten::val::module_property("WXROnError"));
}

void WebXR::RequestSession(XRSessionMode mode, XRSessionFeatures required)
{
    emscripten::val option = emscripten::val::object();
    option.set("requiredFeatures", _XRSessionFeatures[required]);
    emscripten::val promise = mXR.call<emscripten::val>("requestSession", _XRSessionMode[mode], option);
    promise.call<void>("then", emscripten::val::module_property("WXROnSessionStarted"), emscripten::val::module_property("WXROnError"));
}

void WebXR::RequestSession(XRSessionMode mode, XRSessionFeatures required, XRSessionFeatures optional)
{
    emscripten::val option = emscripten::val::object();
    option.set("requiredFeatures", _XRSessionFeatures[required]);
    option.set("optionalFeatures", _XRSessionFeatures[optional]);
    emscripten::val promise = mXR.call<emscripten::val>("requestSession", _XRSessionMode[mode], option);
    promise.call<void>("then", emscripten::val::module_property("WXROnSessionStarted"), emscripten::val::module_property("WXROnError"));
}

#endif