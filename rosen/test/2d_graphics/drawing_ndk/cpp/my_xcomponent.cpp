/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "common/log_common.h"
#include <bits/alltypes.h>
#include <chrono>
#include <fcntl.h>
#include <memory>
#include <native_drawing/drawing_font_collection.h>
#include <native_drawing/drawing_text_typography.h>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <unordered_map>
#include "my_xcomponent.h"
#include "testcasefactory.h"
#include "test_common.h"

static std::unordered_map<std::string, MyXComponent *> g_instance;

EGLConfig getConfig(int version, EGLDisplay eglDisplay)
{
    int attribList[] = {EGL_SURFACE_TYPE,
                        EGL_WINDOW_BIT,
                        EGL_RED_SIZE,
                        8,
                        EGL_GREEN_SIZE,
                        8,
                        EGL_BLUE_SIZE,
                        8,
                        EGL_ALPHA_SIZE,
                        8,
                        EGL_RENDERABLE_TYPE,
                        EGL_OPENGL_ES2_BIT,
                        EGL_NONE};
    EGLConfig configs = NULL;
    int configsNum;

    if (!eglChooseConfig(eglDisplay, attribList, &configs, 1, &configsNum)) {
        DRAWING_LOGE("eglChooseConfig ERROR");
        return NULL;
    }

    return configs;
}

static void OnSurfaceCreatedCB(OH_NativeXComponent *component, void *window)
{
    DRAWING_LOGI("OnSurfaceCreatedCB");
    if ((component == nullptr) || (window == nullptr)) {
        DRAWING_LOGE("OnSurfaceCreatedCB: component or window is null");
        return;
    }
    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NativeXComponent_GetXComponentId(component, idStr, &idSize) != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
        DRAWING_LOGE("OnSurfaceCreatedCB: Unable to get XComponent id");
        return;
    }
    std::string id(idStr);
    auto render = MyXComponent::GetOrCreateInstance(id);
    if (render == nullptr) {
        DRAWING_LOGE("OnSurfaceCreatedCB: Failed to get instance");
        return;
    }
    OHNativeWindow *nativeWindow = static_cast<OHNativeWindow *>(window);
    render->SetNativeWindow(nativeWindow);

    uint64_t width;
    uint64_t height;
    int32_t status = OH_NativeXComponent_GetXComponentSize(component, window, &width, &height);
    if ((status == OH_NATIVEXCOMPONENT_RESULT_SUCCESS) && (render != nullptr)) {
        render->SetSceenHeight(height);
        render->SetScreenWidth(width);
    }
}

static void OnSurfaceDestroyedCB(OH_NativeXComponent *component, void *window)
{
    DRAWING_LOGI("OnSurfaceDestroyedCB");
    if ((component == nullptr) || (window == nullptr)) {
        DRAWING_LOGE("OnSurfaceDestroyedCB: component or window is null");
        return;
    }
    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NativeXComponent_GetXComponentId(component, idStr, &idSize) != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
        DRAWING_LOGE("OnSurfaceDestroyedCB: Unable to get XComponent id");
        return;
    }
    std::string id(idStr);
    MyXComponent::Release(id);
}

// 通过callback获取到宽高信息
void MyXComponent::SetScreenWidth(uint64_t width)
{
    screenWidth_ = width;
}

// 通过callback获取到宽高信息
void MyXComponent::SetSceenHeight(uint64_t height)
{
    screenHeight_ = height;
}

// 通过callback获取到window
void MyXComponent::SetNativeWindow(OHNativeWindow *nativeWindow)
{
    nativeWindow_ = nativeWindow;
}

//获取屏幕xcomponent的绘制canvas
void MyXComponent::InitScreenCanvas()
{
    if (nativeWindow_ == nullptr) {
        DRAWING_LOGE("nativeWindow_ is nullptr");
        return;
    }
    // 这里的nativeWindow是从上一步骤中的回调函数中获得的
    // 通过 OH_NativeWindow_NativeWindowRequestBuffer 获取 OHNativeWindowBuffer 实例
    int ret = OH_NativeWindow_NativeWindowRequestBuffer(nativeWindow_, &buffer_, &fenceFd_);
    if (ret != 0) {
        DRAWING_LOGE("failed to NativeWindowRequestBuffer");
        return;
    }
    DRAWING_LOGI("request buffer ret = %{public}d", ret);
    // 通过 OH_NativeWindow_GetBufferHandleFromNative 获取 buffer 的 handle
    bufferHandle_ = OH_NativeWindow_GetBufferHandleFromNative(buffer_);
    // 使用系统mmap接口拿到bufferHandle的内存虚拟地址
    mappedAddr_ = static_cast<uint32_t *>(
        mmap(bufferHandle_->virAddr, bufferHandle_->size, PROT_READ | PROT_WRITE, MAP_SHARED, bufferHandle_->fd, 0));
    if (mappedAddr_ == MAP_FAILED) {
        DRAWING_LOGE("mmap failed");
        return;
    }

    if (screenCanvas_) {
        OH_Drawing_CanvasDestroy(screenCanvas_);
        screenCanvas_ = nullptr;
    }
    if (screenBitmap_) {
        OH_Drawing_BitmapDestroy(screenBitmap_);
        screenBitmap_ = nullptr;
    }
    screenImageInfo_ = {static_cast<int32_t>(screenWidth_), static_cast<int32_t>(screenHeight_),
        COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    screenBitmap_ = OH_Drawing_BitmapCreateFromPixels(&screenImageInfo_, mappedAddr_, screenWidth_ * 4); // 4:rgba
    screenCanvas_ = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(screenCanvas_, screenBitmap_);
    OH_Drawing_CanvasClear(screenCanvas_, OH_Drawing_ColorSetArgb(0x00, 0x00, 0x00, 0x00));
}

void MyXComponent::BitmapToScreenCanvas(OH_Drawing_Bitmap* bitmap)
{
    DRAWING_LOGE("MyXComponent  BitmapToScreenCanvas");
    if (screenCanvas_ == nullptr || bitmap == nullptr) {
        return;
    }
    OH_Drawing_CanvasDrawBitmap(screenCanvas_, bitmap, 0, 0);
    FlushScreen();

    int result = munmap(mappedAddr_, bufferHandle_->size);
    if (result == -1) {
        DRAWING_LOGE("munmap failed!");
    }
    mappedAddr_ = nullptr;
}

void MyXComponent::FlushScreen()
{
    if (nativeWindow_ != nullptr && buffer_ != nullptr) {
        // 设置刷新区域，如果Region中的Rect为nullptr,或者rectNumber为0，则认为OHNativeWindowBuffer全部有内容更改。
        Region region {nullptr, 0};
        // 通过OH_NativeWindow_NativeWindowFlushBuffer 提交给消费者使用，例如：显示在屏幕上。
        OH_NativeWindow_NativeWindowFlushBuffer(nativeWindow_, buffer_, fenceFd_, region);
    }
}

int32_t MyXComponent::InitializeEglContext()
{
    mEGLDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (mEGLDisplay == EGL_NO_DISPLAY) {
        DRAWING_LOGE("unable to get EGL display.");
        return -1;
    }

    EGLint eglMajVers;
    EGLint eglMinVers;
    if (!eglInitialize(mEGLDisplay, &eglMajVers, &eglMinVers)) {
        mEGLDisplay = EGL_NO_DISPLAY;
        DRAWING_LOGE("unable to initialize display");
        return -1;
    }

    int version = 3;
    mEGLConfig = getConfig(version, mEGLDisplay);
    if (mEGLConfig == nullptr) {
        DRAWING_LOGE("GLContextInit config ERROR");
        return -1;
    }

    /* Create EGLContext from */
    int attribList[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE}; // 2 size

    mEGLContext = eglCreateContext(mEGLDisplay, mEGLConfig, EGL_NO_CONTEXT, attribList);

    EGLint attribs[] = {EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE};
    mEGLSurface = eglCreatePbufferSurface(mEGLDisplay, mEGLConfig, attribs);
    if (!eglMakeCurrent(mEGLDisplay, mEGLSurface, mEGLSurface, mEGLContext)) {
        DRAWING_LOGE("eglMakeCurrent error = %{public}d", eglGetError());
        return -1;
    }

    DRAWING_LOGI("Init success.");
    return 0;
}

void MyXComponent::DeInitializeEglContext()
{
    EGLBoolean ret = eglDestroySurface(mEGLDisplay, mEGLSurface);
    if (!ret) {
        DRAWING_LOGW("eglDestroySurface failure.");
    }

    ret = eglDestroyContext(mEGLDisplay, mEGLContext);
    if (!ret) {
        DRAWING_LOGW("eglDestroyContext failure.");
    }

    ret = eglTerminate(mEGLDisplay);
    if (!ret) {
        DRAWING_LOGW("eglTerminate failure.");
    }

    mEGLSurface = NULL;
    mEGLContext = NULL;
    mEGLDisplay = NULL;
}

void MyXComponent::TestPerformanceCpu(napi_env env, std::string caseName)
{
    auto testCase = TestCaseFactory::GetPerformanceCpuCase(caseName);
    if (testCase == nullptr) {
        FlushScreen();
        DRAWING_LOGE("failed to get testcase");
        return;
    }
    testCase->SetTestCount(testCount_);
    testCase->TestPerformanceCpu(env);
    usedTime_ = testCase->GetTime();
    BitmapToScreenCanvas(testCase->GetBitmap());
}

void MyXComponent::TestPerformanceGpu(napi_env env, std::string caseName)
{
    auto testCase = TestCaseFactory::GetPerformanceCpuCase(caseName);
    if (testCase == nullptr) {
        FlushScreen();
        DRAWING_LOGE("failed to get testcase");
        return;
    }
    testCase->SetTestCount(testCount_);
    testCase->TestPerformanceGpu(env);
    usedTime_ = testCase->GetTime();
    BitmapToScreenCanvas(testCase->GetBitmap());
}

void MyXComponent::TestFunctionCpu(napi_env env, std::string caseName)
{
    if (caseName == "All") {
        for (auto map : TestCaseFactory::GetFunctionCpuCaseAll()) {
            std::shared_ptr<TestBase> testCase = map.second();
            if (testCase == nullptr) {
                DRAWING_LOGE("TestCase is null");
                return;
            }
            testCase->SetFileName(map.first);
            testCase->TestFunctionCpu(env);
        }
    } else {
        auto testCase = TestCaseFactory::GetFunctionCpuCase(caseName);
        if (testCase == nullptr) {
            FlushScreen();
            DRAWING_LOGE("failed to get testcase");
            return;
        }
        testCase->SetFileName(caseName);
        testCase->TestFunctionCpu(env);
        BitmapToScreenCanvas(testCase->GetBitmap());
    }
}

void MyXComponent::TestFunctionGpu(napi_env env, std::string caseName)
{
    if (caseName == "All") {
        for (auto map : TestCaseFactory::GetFunctionCpuCaseAll()) {
            std::shared_ptr<TestBase> testCase = map.second();
            if (testCase == nullptr) {
                DRAWING_LOGE("TestCase is null");
                return;
            }
            testCase->SetFileName(map.first);
            testCase->TestFunctionGpu(env);
        }
    } else {
        auto testCase = TestCaseFactory::GetFunctionCpuCase(caseName);
        if (testCase == nullptr) {
            FlushScreen();
            DRAWING_LOGE("failed to get testcase");
            return;
        }
        testCase->SetFileName(caseName);
        testCase->TestFunctionGpu(env);
        BitmapToScreenCanvas(testCase->GetBitmap());
    }
}

void MyXComponent::SetTestCount(uint32_t testCount)
{
    testCount_ = testCount;
}

napi_value MyXComponent::NapiSetTestCount(napi_env env, napi_callback_info info)
{
    DRAWING_LOGI("NapiSetTestCount");
    if ((env == nullptr) || (info == nullptr)) {
        DRAWING_LOGE("NapiDrawPattern: env or info is null");
        return nullptr;
    }

    napi_value thisArg;
    size_t argc = 1;
    napi_value argv[1] = {nullptr};
    if (napi_get_cb_info(env, info, &argc, argv, &thisArg, nullptr) != napi_ok) {
        DRAWING_LOGE("NapiDrawPattern: napi_get_cb_info fail");
        return nullptr;
    }
    uint32_t testCount;
    if (!ConvertIntFromJsValue(env, argv[0], testCount)) {
        DRAWING_LOGE("NapiDrawPattern: get caseName fail");
        return nullptr;
    }
    DRAWING_LOGI("testCount = %{public}u", testCount);

    napi_value exportInstance;
    if (napi_get_named_property(env, thisArg, OH_NATIVE_XCOMPONENT_OBJ, &exportInstance) != napi_ok) {
        DRAWING_LOGE("NapiDrawPattern: napi_get_named_property fail");
        return nullptr;
    }

    OH_NativeXComponent *nativeXComponent = nullptr;
    if (napi_unwrap(env, exportInstance, reinterpret_cast<void **>(&nativeXComponent)) != napi_ok) {
        DRAWING_LOGE("NapiDrawPattern: napi_unwrap fail");
        return nullptr;
    }

    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NativeXComponent_GetXComponentId(nativeXComponent, idStr, &idSize) != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
        DRAWING_LOGE("NapiDrawPattern: Unable to get XComponent id");
        return nullptr;
    }
    DRAWING_LOGI("ID = %{public}s", idStr);
    std::string id(idStr);
    MyXComponent *render = MyXComponent().GetOrCreateInstance(id);
    if (render != nullptr) {
        render->SetTestCount(testCount);
    } else {
        DRAWING_LOGE("render is nullptr");
    }
    return nullptr;
}

uint32_t MyXComponent::GetTime()
{
    return usedTime_;
}

napi_value MyXComponent::NapiGetTime(napi_env env, napi_callback_info info)
{
    DRAWING_LOGI("MyXComponent NapiGetTime");
    if ((env == nullptr) || (info == nullptr)) {
        DRAWING_LOGE("NapiDrawPattern: env or info is null");
        return nullptr;
    }

    napi_value thisArg;
    size_t argc = 1;
    napi_value argv[1] = {nullptr};
    if (napi_get_cb_info(env, info, &argc, argv, &thisArg, nullptr) != napi_ok) {
        DRAWING_LOGE("NapiDrawPattern: napi_get_cb_info fail");
        return nullptr;
    }

    napi_value exportInstance;
    if (napi_get_named_property(env, thisArg, OH_NATIVE_XCOMPONENT_OBJ, &exportInstance) != napi_ok) {
        DRAWING_LOGE("NapiDrawPattern: napi_get_named_property fail");
        return nullptr;
    }

    OH_NativeXComponent *nativeXComponent = nullptr;
    if (napi_unwrap(env, exportInstance, reinterpret_cast<void **>(&nativeXComponent)) != napi_ok) {
        DRAWING_LOGE("NapiDrawPattern: napi_unwrap fail");
        return nullptr;
    }

    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NativeXComponent_GetXComponentId(nativeXComponent, idStr, &idSize) != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
        DRAWING_LOGE("NapiDrawPattern: Unable to get XComponent id");
        return nullptr;
    }
    DRAWING_LOGI("ID = %{public}s", idStr);
    std::string id(idStr);
    MyXComponent *render = MyXComponent().GetOrCreateInstance(id);
    if (render != nullptr) {
        DRAWING_LOGE("DrawingTest render->GetTime");
        napi_value value = nullptr;
        (void)napi_create_int32(env, render->GetTime(), &value);
        return value;
    } else {
        DRAWING_LOGE("DrawingTest render is nullptr");
    }
    return nullptr;
}

napi_value MyXComponent::NapiFunctionCpu(napi_env env, napi_callback_info info)
{
    DRAWING_LOGI("NapiFunctionCpu");
    if ((env == nullptr) || (info == nullptr)) {
        DRAWING_LOGE("NapiFunctionCpu: env or info is null");
        return nullptr;
    }

    napi_value thisArg;
    size_t argc = 1;
    napi_value argv[1] = {nullptr};
    if (napi_get_cb_info(env, info, &argc, argv, &thisArg, nullptr) != napi_ok) {
        DRAWING_LOGE("NapiFunctionCpu: napi_get_cb_info fail");
        return nullptr;
    }
    std::string caseName = "";
    if (!ConvertStringFromJsValue(env, argv[0], caseName)) {
        DRAWING_LOGE("NapiFunctionCpu: get caseName fail");
        return nullptr;
    }
    DRAWING_LOGI("caseName = %{public}s", caseName.c_str());

    napi_value exportInstance;
    if (napi_get_named_property(env, thisArg, OH_NATIVE_XCOMPONENT_OBJ, &exportInstance) != napi_ok) {
        DRAWING_LOGE("NapiFunctionCpu: napi_get_named_property fail");
        return nullptr;
    }

    OH_NativeXComponent *nativeXComponent = nullptr;
    if (napi_unwrap(env, exportInstance, reinterpret_cast<void **>(&nativeXComponent)) != napi_ok) {
        DRAWING_LOGE("NapiFunctionCpu: napi_unwrap fail");
        return nullptr;
    }

    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NativeXComponent_GetXComponentId(nativeXComponent, idStr, &idSize) != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
        DRAWING_LOGE("NapiFunctionCpu: Unable to get XComponent id");
        return nullptr;
    }
    DRAWING_LOGI("NapiFunctionCpu ID = %{public}s", idStr);
    std::string id(idStr);
    MyXComponent *render = MyXComponent().GetOrCreateInstance(id);
    if (render != nullptr) {
        render->InitScreenCanvas();
        render->TestFunctionCpu(env, caseName);
    } else {
        DRAWING_LOGE("NapiFunctionCpu: render is nullptr");
    }
    return nullptr;
}

napi_value MyXComponent::NapiFunctionGpu(napi_env env, napi_callback_info info)
{
    DRAWING_LOGI("NapiFunctionGpu");
    if ((env == nullptr) || (info == nullptr)) {
        DRAWING_LOGE("NapiFunctionGpu: env or info is null");
        return nullptr;
    }

    napi_value thisArg;
    size_t argc = 1;
    napi_value argv[1] = {nullptr};
    if (napi_get_cb_info(env, info, &argc, argv, &thisArg, nullptr) != napi_ok) {
        DRAWING_LOGE("NapiFunctionGpu: napi_get_cb_info fail");
        return nullptr;
    }
    std::string caseName = "";
    if (!ConvertStringFromJsValue(env, argv[0], caseName)) {
        DRAWING_LOGE("NapiFunctionGpu: get caseName fail");
        return nullptr;
    }
    DRAWING_LOGI("caseName = %{public}s", caseName.c_str());

    napi_value exportInstance;
    if (napi_get_named_property(env, thisArg, OH_NATIVE_XCOMPONENT_OBJ, &exportInstance) != napi_ok) {
        DRAWING_LOGE("NapiFunctionGpu: napi_get_named_property fail");
        return nullptr;
    }

    OH_NativeXComponent *nativeXComponent = nullptr;
    if (napi_unwrap(env, exportInstance, reinterpret_cast<void **>(&nativeXComponent)) != napi_ok) {
        DRAWING_LOGE("NapiFunctionGpu: napi_unwrap fail");
        return nullptr;
    }

    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NativeXComponent_GetXComponentId(nativeXComponent, idStr, &idSize) != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
        DRAWING_LOGE("NapiFunctionGpu: Unable to get XComponent id");
        return nullptr;
    }
    DRAWING_LOGI("NapiFunctionGpu ID = %{public}s", idStr);
    std::string id(idStr);
    MyXComponent *render = MyXComponent().GetOrCreateInstance(id);
    if (render != nullptr) {
        render->InitScreenCanvas();
        render->InitializeEglContext();
        render->TestFunctionGpu(env, caseName);
        render->DeInitializeEglContext();
    } else {
        DRAWING_LOGE("NapiFunctionGpu: render is nullptr");
    }
    return nullptr;
}

napi_value MyXComponent::NapiPerformanceCpu(napi_env env, napi_callback_info info)
{
    DRAWING_LOGI("NapiPerformanceCpu");
    if ((env == nullptr) || (info == nullptr)) {
        DRAWING_LOGE("NapiDrawPattern: env or info is null");
        return nullptr;
    }

    napi_value thisArg;
    size_t argc = 1;
    napi_value argv[1] = {nullptr};
    if (napi_get_cb_info(env, info, &argc, argv, &thisArg, nullptr) != napi_ok) {
        DRAWING_LOGE("NapiDrawPattern: napi_get_cb_info fail");
        return nullptr;
    }
    std::string caseName = "";
    if (!ConvertStringFromJsValue(env, argv[0], caseName)) {
        DRAWING_LOGE("NapiDrawPattern: get caseName fail");
        return nullptr;
    }
    DRAWING_LOGI("caseName = %{public}s", caseName.c_str());

    napi_value exportInstance;
    if (napi_get_named_property(env, thisArg, OH_NATIVE_XCOMPONENT_OBJ, &exportInstance) != napi_ok) {
        DRAWING_LOGE("NapiDrawPattern: napi_get_named_property fail");
        return nullptr;
    }

    OH_NativeXComponent *nativeXComponent = nullptr;
    if (napi_unwrap(env, exportInstance, reinterpret_cast<void **>(&nativeXComponent)) != napi_ok) {
        DRAWING_LOGE("NapiDrawPattern: napi_unwrap fail");
        return nullptr;
    }

    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NativeXComponent_GetXComponentId(nativeXComponent, idStr, &idSize) != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
        DRAWING_LOGE("NapiDrawPattern: Unable to get XComponent id");
        return nullptr;
    }
    DRAWING_LOGI("ID = %{public}s", idStr);
    std::string id(idStr);
    MyXComponent *render = MyXComponent().GetOrCreateInstance(id);
    if (render != nullptr) {
        render->InitScreenCanvas();
        render->TestPerformanceCpu(env, caseName);
    } else {
        DRAWING_LOGE("render is nullptr");
    }
    
    return nullptr;
}

napi_value MyXComponent::NapiPerformanceGpu(napi_env env, napi_callback_info info)
{
    DRAWING_LOGI("NapiPerformanceGpu");
    if ((env == nullptr) || (info == nullptr)) {
        DRAWING_LOGE("NapiDrawPattern: env or info is null");
        return nullptr;
    }

    napi_value thisArg;
    size_t argc = 1;
    napi_value argv[1] = {nullptr};
    if (napi_get_cb_info(env, info, &argc, argv, &thisArg, nullptr) != napi_ok) {
        DRAWING_LOGE("NapiDrawPattern: napi_get_cb_info fail");
        return nullptr;
    }
    std::string caseName = "";
    if (!ConvertStringFromJsValue(env, argv[0], caseName)) {
        DRAWING_LOGE("NapiDrawPattern: get caseName fail");
        return nullptr;
    }

    napi_value exportInstance;
    if (napi_get_named_property(env, thisArg, OH_NATIVE_XCOMPONENT_OBJ, &exportInstance) != napi_ok) {
        DRAWING_LOGE("NapiDrawPattern: napi_get_named_property fail");
        return nullptr;
    }

    OH_NativeXComponent *nativeXComponent = nullptr;
    if (napi_unwrap(env, exportInstance, reinterpret_cast<void **>(&nativeXComponent)) != napi_ok) {
        DRAWING_LOGE("NapiDrawPattern: napi_unwrap fail");
        return nullptr;
    }

    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NativeXComponent_GetXComponentId(nativeXComponent, idStr, &idSize) != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
        DRAWING_LOGE("NapiDrawPattern: Unable to get XComponent id");
        return nullptr;
    }
    std::string id(idStr);
    MyXComponent *render = MyXComponent().GetOrCreateInstance(id);
    if (render != nullptr) {
        render->InitScreenCanvas();
        render->InitializeEglContext();
        render->TestPerformanceGpu(env, caseName);
        render->DeInitializeEglContext();
    } else {
        DRAWING_LOGE("render is nullptr");
    }
    DRAWING_LOGI("DrawingTest NapiPerformanceGpu, ID = %{public}s, caseName = %{public}s", idStr, caseName.c_str());
    
    return nullptr;
}

MyXComponent::~MyXComponent()
{
    if (screenCanvas_) {
        OH_Drawing_CanvasDestroy(screenCanvas_);
        screenCanvas_ = nullptr;
    }
    if (screenBitmap_) {
        OH_Drawing_BitmapDestroy(screenBitmap_);
        screenBitmap_ = nullptr;
    }

    if (mappedAddr_ != nullptr && bufferHandle_ != nullptr) {
        int result = munmap(mappedAddr_, bufferHandle_->size);
        if (result == -1) {
            DRAWING_LOGE("munmap failed!");
        }
    }
    buffer_ = nullptr;
    bufferHandle_ = nullptr;
    nativeWindow_ = nullptr;
    mappedAddr_ = nullptr;
}

void MyXComponent::Release(std::string &id)
{
    auto map = g_instance.find(id);
    if (map == g_instance.end()) {
        return;
    }
    MyXComponent *render = g_instance[id];
    if (render != nullptr) {
        g_instance.erase(map);
        delete render;
        render = nullptr;
    }
}

void MyXComponent::Export(napi_env env, napi_value exports)
{
    DRAWING_LOGI("MyXComponent napi init");
    if ((env == nullptr) || (exports == nullptr)) {
        DRAWING_LOGE("Export: env or exports is null");
        return;
    }

    napi_property_descriptor desc[] = {
        {"setTestCount", nullptr, MyXComponent::NapiSetTestCount, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"getTime", nullptr, MyXComponent::NapiGetTime, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"testFunctionCpu", nullptr, MyXComponent::NapiFunctionCpu, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"testFunctionGpu", nullptr, MyXComponent::NapiFunctionGpu, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"testPerformanceCpu", nullptr, MyXComponent::NapiPerformanceCpu,
            nullptr, nullptr, nullptr, napi_default, nullptr},
        {"testPerformanceGpu", nullptr, MyXComponent::NapiPerformanceGpu,
            nullptr, nullptr, nullptr, napi_default, nullptr},
    };
    if (napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc) != napi_ok) {
        DRAWING_LOGE("Export: napi_define_properties failed");
    }
}

void MyXComponent::RegisterCallback(OH_NativeXComponent *nativeXComponent)
{
    DRAWING_LOGI("register callback");
    renderCallback_.OnSurfaceCreated = OnSurfaceCreatedCB;
    renderCallback_.OnSurfaceDestroyed = OnSurfaceDestroyedCB;
    // Callback must be initialized
    renderCallback_.DispatchTouchEvent = nullptr;
    renderCallback_.OnSurfaceChanged = nullptr;
    OH_NativeXComponent_RegisterCallback(nativeXComponent, &renderCallback_);
}

MyXComponent *MyXComponent::GetOrCreateInstance(std::string &id)
{
    if (g_instance.find(id) == g_instance.end()) {
        MyXComponent *render = new MyXComponent(id);
        g_instance[id] = render;
        return render;
    } else {
        return g_instance[id];
    }
}
