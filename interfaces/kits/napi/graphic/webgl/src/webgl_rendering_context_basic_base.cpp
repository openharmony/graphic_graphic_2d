/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "context/webgl_rendering_context_basic_base.h"

#include <limits>

#include "context/webgl2_rendering_context.h"
#include "context/webgl_rendering_context.h"
#include "napi/n_class.h"
#include "util/egl_manager.h"
#include "util/log.h"
#include "util/object_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

using namespace std;
namespace OHOS {
namespace Rosen {

WebGLRenderingContextBasicBase::~WebGLRenderingContextBasicBase()
{
    if (eglSurface_ != nullptr) {
        eglDestroySurface(EglManager::GetInstance().GetEGLDisplay(), eglSurface_);
        eglSurface_ = nullptr;
    }
}

void WebGLRenderingContextBasicBase::SetEglWindow(void* window)
{
    LOGD("WebGLRenderingContextBasicBase::SetEglWindow.");
    eglWindow_ = reinterpret_cast<NativeWindow*>(window);
}

void WebGLRenderingContextBasicBase::Create(void* context)
{
    LOGD("WebGLRenderingContextBasicBase::Create.");
}

void WebGLRenderingContextBasicBase::Init()
{
    LOGD("WebGLRenderingContextBasicBase::Init");
    EglManager::GetInstance().Init();
    if (eglSurface_ == nullptr) {
        eglSurface_ = EglManager::GetInstance().CreateSurface(eglWindow_);
    }
}

void WebGLRenderingContextBasicBase::SetBitMapPtr(char* bitMapPtr, int bitMapWidth, int bitMapHeight)
{
    LOGD("WebGLRenderingContextBasicBase::SetBitMapPtr");
    LOGD("WebGLRenderingContextBasicBase SetBitMapPtr [%{public}d %{public}d]", bitMapWidth, bitMapHeight);
    constexpr size_t RGBA_BYTES_PER_PIXEL = 4;
    std::lock_guard<std::mutex> lock(bitMapMutex_);
    if (bitMapPtr == nullptr || bitMapWidth <= 0 || bitMapHeight <= 0 ||
        static_cast<size_t>(bitMapWidth) > std::numeric_limits<size_t>::max() / RGBA_BYTES_PER_PIXEL /
        static_cast<size_t>(bitMapHeight)) {
        bitMapPtr_ = nullptr;
        bitMapWidth_ = 0;
        bitMapHeight_ = 0;
        LOGE("WebGLRenderingContextBasicBase invalid bitmap parameters");
        return;
    }
    bitMapPtr_ = bitMapPtr;
    bitMapWidth_ = bitMapWidth;
    bitMapHeight_ = bitMapHeight;
    EglManager::GetInstance().SetPbufferAttributes(bitMapWidth, bitMapHeight);
}

uint64_t WebGLRenderingContextBasicBase::CreateTexture()
{
    return 0;
}

void WebGLRenderingContextBasicBase::SetUpdateCallback(std::function<void()> callback)
{
    updateCallback_ = callback;
}

void WebGLRenderingContextBasicBase::SetTexture(uint64_t id) {}

void WebGLRenderingContextBasicBase::Attach(uint64_t textureId) {}

void WebGLRenderingContextBasicBase::Update()
{
    if (eglWindow_) {
        LOGD("eglSwapBuffers");
        EGLDisplay eglDisplay = EglManager::GetInstance().GetEGLDisplay();
        eglSwapBuffers(eglDisplay, eglSurface_);
    } else {
        std::lock_guard<std::mutex> lock(bitMapMutex_);
        if (bitMapPtr_ != nullptr && bitMapWidth_ > 0 && bitMapHeight_ > 0) {
            LOGD("glReadPixels packAlignment %{public}d", packAlignment_);
            glPixelStorei(GL_PACK_ALIGNMENT, 4); // 4 alignment
            glReadPixels(0, 0, bitMapWidth_, bitMapHeight_, GL_RGBA, GL_UNSIGNED_BYTE, bitMapPtr_);
            glPixelStorei(GL_PACK_ALIGNMENT, packAlignment_);
            LOGD("glReadPixels result %{public}u", glGetError());
        }
    }
    if (updateCallback_) {
        LOGD("mUpdateCallback");
        updateCallback_();
    } else {
        LOGE("mUpdateCallback is null");
    }
}

void WebGLRenderingContextBasicBase::Detach() {}

bool WebGLRenderingContextBasicBase::CreateSurface()
{
    if (eglSurface_ == nullptr) {
        eglSurface_ = EglManager::GetInstance().CreateSurface(eglWindow_);
        if (eglSurface_ == nullptr) {
            return false;
        }
    }
    return true;
}

string WebGLRenderingContextBasicBase::GetContextAttr(
    const std::string& str, const std::string& key, int keyLength, int value)
{
    size_t item = str.find(key);
    if (item != string::npos) {
        string itemVar = str.substr(item + keyLength, value);
        return itemVar;
    }
    return "false";
}

bool WebGLRenderingContextBasicBase::SetWebGLContextAttributes(const std::vector<std::string>& vec)
{
    if (vec.size() <= 1) {
        return true;
    }
    if (webGlContextAttributes_ == nullptr) {
        webGlContextAttributes_ = std::make_shared<WebGLContextAttributes>();
        if (webGlContextAttributes_ == nullptr) {
            return false;
        }
    }
    for (size_t i = 1; i < vec.size(); i++) {
        if (GetContextAttr(vec[i], "alpha", 7, 4) == "true") { // 7 alpha length  4 true
            webGlContextAttributes_->alpha_ = true;
        }
        if (GetContextAttr(vec[i], "depth", 7, 4) == "true") { // 7 depth length  4 true
            webGlContextAttributes_->depth_ = true;
        }
        if (GetContextAttr(vec[i], "stencil", 9, 4) == "true") { // 9 stencil length  4 true
            webGlContextAttributes_->stencil_ = true;
        }
        if (GetContextAttr(vec[i], "premultipliedAlpha", 23, 4) == "true") { // 23 premultipliedAlpha length  4 true
            webGlContextAttributes_->premultipliedAlpha_ = true;
        }
        // 18 preserveDrawingBuffer length 4 true
        if (GetContextAttr(vec[i], "preserveDrawingBuffer", 18, 4) == "true") {
            webGlContextAttributes_->preserveDrawingBuffer_ = true;
        }
        // 30 failIfMajorPerformanceCaveat length  4 true
        if (GetContextAttr(vec[i], "failIfMajorPerformanceCaveat", 30, 4) == "true") {
            webGlContextAttributes_->failIfMajorPerformanceCaveat_ = true;
        }
        if (GetContextAttr(vec[i], "desynchronized", 16, 4) == "true") { // 16 desynchronized length 4 true
            webGlContextAttributes_->desynchronized_ = true;
        }
        string highPerformance = GetContextAttr(vec[i], "powerPreference", 18, 16); // 18 16 powerPreference length
        if (highPerformance == "high-performance") {
            webGlContextAttributes_->powerPreference_ = highPerformance;
            return true;
        }
        string lowPower = GetContextAttr(vec[i], "powerPreference", 18, 9); // 18 9 powerPreference length
        if (lowPower == "low-power") {
            webGlContextAttributes_->powerPreference_ = lowPower;
        } else {
            if (GetContextAttr(vec[i], "powerPreference", 18, 7) == "default") { // 18 7 powerPreference length
                webGlContextAttributes_->powerPreference_ = "default";
            }
        }
    }
    return true;
}

napi_value WebGLRenderingContextBasicBase::GetContextInstance(napi_env env,
    std::string className, napi_callback constructor, napi_finalize finalize_cb)
{
    if (env == nullptr || className.empty() || constructor == nullptr) {
        LOGE("WebGL GetContextInstance invalid arguments");
        return nullptr;
    }
    napi_value instanceValue = nullptr;
    if (contextRef_ == nullptr) {
        napi_value contextClass = nullptr;
        napi_status status = napi_define_class(
            env, className.c_str(), NAPI_AUTO_LENGTH, constructor, nullptr, 0, nullptr, &contextClass);
        if (status != napi_ok || contextClass == nullptr) {
            LOGE("WebGL GetContextInstance napi_define_class failed %{public}u", status);
            return NVal::CreateNull(env).val_;
        }
        status = napi_new_instance(env, contextClass, 0, nullptr, &instanceValue);
        if (status != napi_ok || instanceValue == nullptr) {
            LOGE("WebGL GetContextInstance napi_new_instance failed %{public}u", status);
            return NVal::CreateNull(env).val_;
        }
        status = napi_wrap(env, instanceValue, static_cast<void*>(this), finalize_cb, nullptr, nullptr);
        if (status != napi_ok) {
            LOGE("WebGL GetContextInstance napi_wrap failed %{public}u", status);
            return NVal::CreateNull(env).val_;
        }
        napi_ref contextRef = nullptr;
        status = napi_create_reference(env, instanceValue, 0, &contextRef);
        if (status != napi_ok || contextRef == nullptr) {
            LOGE("WebGL GetContextInstance napi_create_reference failed %{public}u", status);
            return NVal::CreateNull(env).val_;
        }
        contextRef_ = contextRef;
    } else {
        napi_status status = napi_get_reference_value(env, contextRef_, &instanceValue);
        if (status != napi_ok || instanceValue == nullptr) {
            LOGE("WebGL GetContextInstance napi_get_reference_value failed %{public}u", status);
            return NVal::CreateNull(env).val_;
        }
    }
    return instanceValue;
}
} // namespace Rosen
} // namespace OHOS
#ifdef __cplusplus
}
#endif
