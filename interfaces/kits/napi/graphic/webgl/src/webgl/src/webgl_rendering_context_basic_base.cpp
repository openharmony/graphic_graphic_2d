/*
* Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "../include/context/webgl_rendering_context_basic_base.h"
#include "../include/context/webgl_rendering_context.h"
#include "../include/util/egl_manager.h"
#include "../include/context/webgl2_rendering_context.h"
#include "../../common/napi/n_class.h"
#include "../include/util/log.h"
#include "../include/util/object_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

using namespace std;

OHOS::Rosen::WebGLRenderingContextBasicBase *GetWebGLInstance(string id)
{
    return OHOS::Rosen::WebGLRenderingContextBasicBase::GetContext(id);
}

namespace OHOS {
namespace Rosen {
WebGLRenderingContextBasicBase *WebGLRenderingContextBasicBase::instance = nullptr;

WebGLRenderingContextBasicBase *WebGLRenderingContextBasicBase::GetContext(string id)
{
    LOGI("WebGLRenderingContextBasicBase::GetContext.");
    return instance;
}

void WebGLRenderingContextBasicBase::SetEglWindow(void *window)
{
    mEglWindow = (NativeWindow *) window;
}

void WebGLRenderingContextBasicBase::Create(void *context)
{
    LOGI("WebGLRenderingContextBasicBase::Create.\n");
}

void WebGLRenderingContextBasicBase::Init()
{
    EglManager::GetInstance().Init();
}

void WebGLRenderingContextBasicBase::SetBitMapPtr(char *bitMapPtr, int bitMapWidth, int bitMapHeight)
{
    LOGI("WebGLRenderingContextBasicBase bitMapWidth = %{public}d, bitMapHeight = %{public}d",
        bitMapWidth, bitMapHeight);
    mBitMapPtr = bitMapPtr;
    mBitMapWidth = bitMapWidth;
    mBitMapHeight = bitMapHeight;
    EglManager::GetInstance().SetPbufferAttributes(bitMapWidth, bitMapHeight);
}

uint64_t WebGLRenderingContextBasicBase::CreateTexture()
{
    return 0;
}

void WebGLRenderingContextBasicBase::SetUpdateCallback(std::function<void()> callback)
{
    mUpdateCallback = callback;
}

void WebGLRenderingContextBasicBase::SetTexture(uint64_t id) {}

void WebGLRenderingContextBasicBase::Attach(uint64_t id) {}

void WebGLRenderingContextBasicBase::Update()
{
    if (mEglWindow) {
        LOGI("WebGLRenderingContextBasicBase eglSwapBuffers");
        EGLDisplay eglDisplay = EglManager::GetInstance().GetEGLDisplay();
        eglSwapBuffers(eglDisplay, mEGLSurface);
    } else {
        LOGI("WebGLRenderingContextBasicBase glReadPixels");
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glReadPixels(0, 0, mBitMapWidth, mBitMapHeight, GL_RGBA, GL_UNSIGNED_BYTE, mBitMapPtr);
    }
    if (mUpdateCallback) {
        LOGI("WebGLRenderingContextBasicBase mUpdateCallback");
        mUpdateCallback();
    } else {
        LOGE("WebGLRenderingContextBasicBase mUpdateCallback null");
    }
}

void WebGLRenderingContextBasicBase::Detach() {}
} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif
