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

#ifndef WEBGL_FRAMEBUFFER_H
#define WEBGL_FRAMEBUFFER_H

#include <map>

#include "napi/n_exporter.h"
#include "webgl_object.h"

namespace OHOS {
namespace Rosen {
namespace Impl {
class WebGLRenderingContextBaseImpl;
}

enum AttachmentType : uint32_t {
    RENDER_BUFFER,
    TEXTURE,
    INVALID_TYPE,
};

struct WebGLAttachmentInfo {
    GLsizei width = 0;
    GLsizei height = 0;
    GLenum format;
    GLenum type;
};

struct WebGLAttachment {
    WebGLAttachment(AttachmentType type, GLenum attachment, GLuint id)
    {
        this->type = type;
        this->attachment = attachment;
        this->id = id;
    }
    bool IsTexture() const
    {
        return type == AttachmentType::TEXTURE;
    }
    bool IsRenderBuffer() const
    {
        return type == AttachmentType::RENDER_BUFFER;
    }
    AttachmentType type;
    GLenum attachment;
    GLuint id;
};

struct AttachmentTexture : public WebGLAttachment {
    AttachmentTexture(AttachmentType type, GLenum attachment, GLuint id)
        : WebGLAttachment(type, attachment, id) {}
    GLenum target {};
    GLint level {};
};

class WebGLFramebuffer : public NExporter, public WebGLObject {
public:
    inline static const std::string className = "WebGLFramebuffer";
    inline static const int objectType = WEBGL_OBJECT_FRAME_BUFFER;
    inline static const int DEFAULT_FRAME_BUFFER = 0;

    bool Export(napi_env env, napi_value exports) override;
    std::string GetClassName() override;
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static NVal CreateObjectInstance(napi_env env, WebGLFramebuffer** instance);
    explicit WebGLFramebuffer() : framebuffer_(0) {};
    WebGLFramebuffer(napi_env env, napi_value exports) : NExporter(env, exports), framebuffer_(0) {};
    ~WebGLFramebuffer();

    void SetFramebuffer(unsigned int framebuffer)
    {
        framebuffer_ = framebuffer;
    }

    unsigned int GetFramebuffer() const
    {
        return framebuffer_;
    }

    void SetTarget(GLenum target)
    {
        target_ = target;
    }

    GLenum GetTarget() const
    {
        return target_;
    }

    void SetReadBufferMode(GLenum mode)
    {
        readBufferMode_ = mode;
    }

    GLenum GetReadBufferMode() const
    {
        return readBufferMode_;
    }

    bool AddAttachment(GLenum target, GLenum attachment, GLuint id);
    bool AddAttachment(GLenum target, GLenum attachment, GLuint id, GLenum textureTarget, GLint level);
    WebGLAttachment* GetAttachment(GLenum attachment) const;
    void RemoveAttachment(GLenum target, GLuint id, AttachmentType type);
    GLenum CheckStatus(napi_env env, Impl::WebGLRenderingContextBaseImpl* context) const;
    GLenum CheckStatus(napi_env env, Impl::WebGLRenderingContextBaseImpl* context, WebGLAttachmentInfo info,
        std::vector<WebGLAttachment*>& attachments, WebGLAttachment* attachedObject) const;
    GLenum CheckAttachStatus(Impl::WebGLRenderingContextBaseImpl* context,
        std::vector<WebGLAttachment*>& attachments) const;
    bool GetWebGLAttachmentInfo(napi_env env, Impl::WebGLRenderingContextBaseImpl* context,
        const WebGLAttachment* attachedObject, WebGLAttachmentInfo& info) const;
private:
    void DoDetachment(GLenum target, WebGLAttachment* attachment);
    bool CheckAttachmentComplete(bool isHighWebGL, WebGLAttachment* attachedObject,
        const WebGLAttachmentInfo& info, std::vector<WebGLAttachment*>& attachments) const;
    bool IsStencilRenderAble(GLenum internalFormat, bool includesDepthStencil) const;
    bool IsDepthRenderAble(GLenum internalFormat, bool includesDepthStencil) const;
    bool IsColorRenderAble(GLenum internalFormat) const;
    void Clear();
    std::map<GLenum, WebGLAttachment*> attachments_ {};
    GLenum target_ { GL_NONE };
    unsigned int framebuffer_  { 0 };
    GLenum readBufferMode_ { GL_COLOR_ATTACHMENT0 };
};
} // namespace Rosen
} // namespace OHOS
#endif // WEBGL_FRAMEBUFFER_H
