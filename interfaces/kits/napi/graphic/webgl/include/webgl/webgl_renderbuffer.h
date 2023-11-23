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

#ifndef WEBGL_RENDERBUFFER_H
#define WEBGL_RENDERBUFFER_H

#include "napi/n_exporter.h"
#include "webgl_object.h"

namespace OHOS {
namespace Rosen {
class WebGLRenderbuffer final : public NExporter, public WebGLObject {
public:
    inline static const std::string className = "WebGLRenderbuffer";
    inline static const int objectType = WEBGL_OBJECT_RENDER_BUFFER;
    inline static const int DEFAULT_RENDER_BUFFER = 0;

    bool Export(napi_env env, napi_value exports) override;

    std::string GetClassName() override;

    static napi_value Constructor(napi_env env, napi_callback_info info);
    static NVal CreateObjectInstance(napi_env env, WebGLRenderbuffer** instance)
    {
        return WebGLObject::CreateObjectInstance<WebGLRenderbuffer>(env, instance);
    }

    void SetRenderbuffer(uint32_t renderbuffer)
    {
        renderbuffer_ = renderbuffer;
    }

    uint32_t GetRenderbuffer() const
    {
        return renderbuffer_;
    }

    explicit WebGLRenderbuffer() : renderbuffer_(0) {};

    WebGLRenderbuffer(napi_env env, napi_value exports) : NExporter(env, exports), renderbuffer_(0) {};

    ~WebGLRenderbuffer() {};

    GLenum GetTarget() const
    {
        return target_;
    }
    void SetTarget(GLenum target)
    {
        target_ = target;
    }

    void SetInternalFormat(GLenum internalFormat)
    {
        internalFormat_ = internalFormat;
    }
    GLenum GetInternalFormat() const
    {
        return internalFormat_;
    }

    void SetSize(GLsizei width, GLsizei height)
    {
        width_ = width;
        height_ = height;
    }
    GLsizei GetWidth() const
    {
        return width_;
    }
    GLsizei GetHeight() const
    {
        return height_;
    }
private:
    uint32_t renderbuffer_;
    GLenum target_ { 0 };
    GLenum internalFormat_ { GL_RGBA4 };
    GLsizei width_ { 0 };
    GLsizei height_ { 0 };
};
} // namespace Rosen
} // namespace OHOS
#endif // WEBGL_RENDERBUFFER_H
