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

#ifndef WEBGL_RENDERING_CONTEXT_BASIC_BASE_H
#define WEBGL_RENDERING_CONTEXT_BASIC_BASE_H

#include <map>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>

#include "canvas_render_context_base.h"
#include "context/webgl_context_attributes.h"
#include "napi/n_class.h"
#include "napi/n_exporter.h"
#include "webgl/webgl_object.h"

namespace OHOS {
namespace Rosen {
enum : int { WEBGL_1_0 = 0, WEBGL_2_0 };

class WebGLRenderingContextBasicBase : public OHOS::Ace::CanvasRenderContextBase {
public:
    WebGLRenderingContextBasicBase() {};
    virtual ~WebGLRenderingContextBasicBase();

    void SetEglWindow(void* window);
    void Create(void* context) override;
    void Init() override;
    void Attach(uint64_t textureId) override;
    void Update() override;
    void Detach() override;
    void SetBitMapPtr(char* bitMapPtr, int bitMapWidth, int bitMapHeight) override;
    uint64_t CreateTexture() override;
    void SetTexture(uint64_t id);
    void SetUpdateCallback(std::function<void()>) override;
    bool CreateSurface();
    bool SetWebGLContextAttributes(const std::vector<std::string>& vec);
    napi_value GetContextInstance(napi_env env, std::string className,
        napi_callback constructor, napi_finalize finalize_cb);

    int GetBufferWidth() const
    {
        return bitMapWidth_;
    }

    int GetBufferHeight() const
    {
        return bitMapHeight_;
    }

    std::shared_ptr<WebGLContextAttributes> CreateWebGlContextAttributes()
    {
        if (webGlContextAttributes_ == nullptr) {
            webGlContextAttributes_ = std::make_shared<WebGLContextAttributes>();
        }
        return webGlContextAttributes_;
    }

    void SetPackAlignment(GLint packAlignment)
    {
        packAlignment_ = packAlignment;
    }
    std::function<void()> updateCallback_;
    napi_ref contextRef_ = nullptr;
private:
    std::string GetContextAttr(const std::string& str, const std::string& key, int keyLength, int value);
    char* bitMapPtr_ = nullptr;
    int bitMapWidth_ = 0;
    int bitMapHeight_ = 0;
    GLint packAlignment_ = 4;
    EGLSurface eglSurface_ = nullptr;
    NativeWindow* eglWindow_ = nullptr;
    std::shared_ptr<WebGLContextAttributes> webGlContextAttributes_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // WEBGL_RENDERING_CONTEXT_BASIC_BASE_H
