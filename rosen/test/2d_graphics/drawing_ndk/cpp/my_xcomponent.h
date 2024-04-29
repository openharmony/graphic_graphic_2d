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

#ifndef MY_XXOMPONENT_H
#define MY_XXOMPONENT_H
#include <ace/xcomponent/native_interface_xcomponent.h>
#include <arpa/nameser.h>
#include <bits/alltypes.h>
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <native_window/external_window.h>
#include <native_drawing/drawing_bitmap.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_gpu_context.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_surface.h>
#include <cstdint>
#include <map>
#include <sys/mman.h>
#include <string>
#include "napi/native_api.h"
#include "test_base.h"

class MyXComponent {
public:
    MyXComponent() = default;
    ~MyXComponent();
    explicit MyXComponent(std::string id) : id_(id) {}
    static MyXComponent *GetInstance(std::string &id);
    void Destroy();
    void DestroyGpu();

    void Export(napi_env env, napi_value exports);
    static napi_value NapiSetTestCount(napi_env env, napi_callback_info info);
    static napi_value NapiGetTime(napi_env env, napi_callback_info info);
    static napi_value NapiFunctionCpu(napi_env env, napi_callback_info info);
    static napi_value NapiFunctionGpu(napi_env env, napi_callback_info info);
    static napi_value NapiPerformanceCpu(napi_env env, napi_callback_info info);
    static napi_value NapiPerformanceGpu(napi_env env, napi_callback_info info);

    // display to screen
    void RegisterCallback(OH_NativeXComponent *nativeXComponent);
    void SetNativeWindow(OHNativeWindow *nativeWindow);
    void SetScreenWidth(uint64_t width);
    void SetSceenHeight(uint64_t height);
    void GetScreenBuffer();
    void CreateScreenCanvas();
    void CreateScreenGpuCanvas();
    void DisplayScreenCanvas();
    void DisplayScreenGpuCanvas();
    int32_t InitializeEglContext();

    // for test
    void SetTestCount(uint32_t testCount);
    uint32_t GetTime();
    void TestFunctionCpu(napi_env env, std::string caseName);
    void TestFunctionGpu(napi_env env, std::string caseName);
    void TestPerformanceCpu(napi_env env, std::string caseName);
    void TestPerformanceGpu(napi_env env, std::string caseName);

    static void Release(std::string &id);
    std::string id_;

private:
    void BitmapToScreenCanvas(OH_Drawing_Bitmap* bitmap);

    // This is the size of the display area, not the size of the entire screen
    OHNativeWindow *nativeWindow_ = nullptr;
    EGLDisplay mEGLDisplay = EGL_NO_DISPLAY;
    EGLConfig mEGLConfig = nullptr;
    EGLContext mEGLContext = EGL_NO_CONTEXT;
    EGLSurface mEGLSurface = nullptr;
    uint64_t screenWidth_ = 0;
    uint64_t screenHeight_ = 0;
    OH_Drawing_Bitmap *screenBitmap_ = nullptr;
    OH_Drawing_Canvas *screenCanvas_ = nullptr;
    OH_Drawing_GpuContext *screenGpuContext_ = nullptr;
    OH_Drawing_Surface *screenSurface_ = nullptr;
    OH_Drawing_Image_Info screenImageInfo_;
    OH_NativeXComponent_Callback renderCallback_;
    uint32_t *mappedAddr_ = nullptr;
    BufferHandle *bufferHandle_ = nullptr;
    struct NativeWindowBuffer *buffer_ = nullptr;
    int fenceFd_ = 0;

    uint32_t testCount_ = DEFAULT_TESTCOUNT;
    uint32_t usedTime_ = 0;
};


#endif
