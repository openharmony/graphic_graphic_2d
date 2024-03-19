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

#include "webgl/webgl_framebuffer.h"

#include "context/webgl2_rendering_context_base.h"
#include "context/webgl_rendering_context_base.h"
#include "context/webgl_rendering_context_base_impl.h"
#include "napi/n_class.h"
#include "napi/n_func_arg.h"
#include "napi/n_val.h"
#include "util/util.h"

namespace OHOS {
namespace Rosen {
using namespace std;
napi_value WebGLFramebuffer::Constructor(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return nullptr;
    }

    unique_ptr<WebGLFramebuffer> webGlFramebuffer = make_unique<WebGLFramebuffer>();
    if (!NClass::SetEntityFor<WebGLFramebuffer>(env, funcArg.GetThisVar(), move(webGlFramebuffer))) {
        LOGE("SetEntityFor webGlFramebuffer failed.");
        return nullptr;
    }
    return funcArg.GetThisVar();
}

bool WebGLFramebuffer::Export(napi_env env, napi_value exports)
{
    vector<napi_property_descriptor> props = {};

    string className = GetClassName();
    bool succ = false;
    napi_value clas = nullptr;
    tie(succ, clas) = NClass::DefineClass(exports_.env_, className, WebGLFramebuffer::Constructor, std::move(props));
    if (!succ) {
        LOGE("DefineClass webGlFramebuffer failed.");
        return false;
    }
    succ = NClass::SaveClass(exports_.env_, className, clas);
    if (!succ) {
        LOGE("SaveClass webGlFramebuffer failed.");
        return false;
    }

    return exports_.AddProp(className, clas);
}

string WebGLFramebuffer::GetClassName()
{
    return WebGLFramebuffer::className;
}

NVal WebGLFramebuffer::CreateObjectInstance(napi_env env, WebGLFramebuffer** instance)
{
    return WebGLObject::CreateObjectInstance<WebGLFramebuffer>(env, instance);
}

WebGLFramebuffer::~WebGLFramebuffer()
{
    auto it = attachments_.begin();
    while (it != attachments_.end()) {
        delete it->second;
        attachments_.erase(it);
        it = attachments_.begin();
    }
}

bool WebGLFramebuffer::AddAttachment(GLenum target, GLenum attachment, GLuint id)
{
    WebGLAttachment* attachmentObject = new WebGLAttachment(AttachmentType::RENDER_BUFFER, attachment, id);
    if (attachmentObject == nullptr) {
        return false;
    }
    LOGD("AddAttachment target %{public}u attachment [%{public}u %{public}u]", target, attachment, id);
    if (attachments_[attachment]) {
        DoDetachment(target, attachments_[attachment]);
        delete attachments_[attachment];
    }
    attachments_[attachment] = attachmentObject;
    return true;
}

bool WebGLFramebuffer::AddAttachment(GLenum target, GLenum attachment, GLuint id, GLenum textureTarget, GLint level)
{
    AttachmentTexture* attachmentObject = new AttachmentTexture(AttachmentType::TEXTURE, attachment, id);
    if (attachmentObject == nullptr) {
        return false;
    }
    LOGD("AddAttachment target %{public}u attachment [%{public}u %{public}u] texture [%{public}u %{public}d]", target,
        attachment, id, textureTarget, level);
    if (attachments_[attachment]) {
        DoDetachment(target, attachments_[attachment]);
        delete attachments_[attachment];
    }
    attachments_[attachment] = attachmentObject;
    attachmentObject->target = textureTarget;
    attachmentObject->level = level;
    return true;
}

void WebGLFramebuffer::DoDetachment(GLenum target, WebGLAttachment* attachment)
{
    if (attachment == nullptr) {
        return;
    }
    LOGD("DoDetachment target %{public}u attachment [%{public}u %{public}u] %{public}u",
        target, attachment->attachment, attachment->id, attachment->type);
    if (attachment->IsRenderBuffer()) {
        if (attachment->attachment == GL_DEPTH_STENCIL_ATTACHMENT) {
            glFramebufferRenderbuffer(target, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
            glFramebufferRenderbuffer(target, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
        } else {
            glFramebufferRenderbuffer(target, attachment->attachment, GL_RENDERBUFFER, 0);
        }
    } else {
        auto textureAttachment = static_cast<const AttachmentTexture*>(attachment);
        if (attachment->attachment == GL_DEPTH_STENCIL_ATTACHMENT) {
            glFramebufferTexture2D(target,
                GL_DEPTH_ATTACHMENT, textureAttachment->target, 0, textureAttachment->level);
            glFramebufferTexture2D(target,
                GL_STENCIL_ATTACHMENT, textureAttachment->target, 0, textureAttachment->level);
        } else {
            glFramebufferTexture2D(target,
                attachment->attachment, textureAttachment->target, 0, textureAttachment->level);
        }
    }
}

void WebGLFramebuffer::RemoveAttachment(GLenum target, GLuint id, AttachmentType type)
{
    for (auto iter = attachments_.begin(); iter != attachments_.end(); iter++) {
        auto object = iter->second;
        if (object != nullptr && object->id == id && type == object->type) {
            DoDetachment(target, iter->second);
            delete iter->second;
            attachments_.erase(iter);
            break;
        }
    }
}

WebGLAttachment* WebGLFramebuffer::GetAttachment(GLenum attachment) const
{
    auto it = attachments_.find(attachment);
    if (it != attachments_.end()) {
        return it->second;
    }
    return nullptr;
}

bool WebGLFramebuffer::IsDepthRenderAble(GLenum internalFormat, bool includesDepthStencil) const
{
    switch (internalFormat) {
        case GL_DEPTH_COMPONENT:
        case GL_DEPTH_COMPONENT16:
        case GL_DEPTH_COMPONENT24:
        case GL_DEPTH_COMPONENT32F:
            return true;
        case GL_DEPTH_STENCIL:
        case GL_DEPTH24_STENCIL8:
        case GL_DEPTH32F_STENCIL8:
            return includesDepthStencil;
        default:
            return false;
    }
}

bool WebGLFramebuffer::IsColorRenderAble(GLenum internalFormat) const
{
    switch (internalFormat) {
        case WebGLRenderingContextBase::RGB:
        case WebGLRenderingContextBase::RGBA:
        case WebGL2RenderingContextBase::R8:
        case WebGL2RenderingContextBase::R8UI:
        case WebGL2RenderingContextBase::R8I:
        case WebGL2RenderingContextBase::R16UI:
        case WebGL2RenderingContextBase::R16I:
        case WebGL2RenderingContextBase::R32UI:
        case WebGL2RenderingContextBase::R32I:
        case WebGL2RenderingContextBase::RG8:
        case WebGL2RenderingContextBase::RG8UI:
        case WebGL2RenderingContextBase::RG8I:
        case WebGL2RenderingContextBase::RG16UI:
        case WebGL2RenderingContextBase::RG16I:
        case WebGL2RenderingContextBase::RG32UI:
        case WebGL2RenderingContextBase::RG32I:
        case WebGL2RenderingContextBase::RGB8:
        case WebGLRenderingContextBase::RGB565:
        case WebGL2RenderingContextBase::RGBA8:
        case WebGL2RenderingContextBase::SRGB8_ALPHA8:
        case WebGLRenderingContextBase::RGB5_A1:
        case WebGLRenderingContextBase::RGBA4:
        case WebGL2RenderingContextBase::RGB10_A2:
        case WebGL2RenderingContextBase::RGBA8UI:
        case WebGL2RenderingContextBase::RGBA8I:
        case WebGL2RenderingContextBase::RGB10_A2UI:
        case WebGL2RenderingContextBase::RGBA16UI:
        case WebGL2RenderingContextBase::RGBA16I:
        case WebGL2RenderingContextBase::RGBA32UI:
        case WebGL2RenderingContextBase::RGBA32I:
            return true;
        default:
            return false;
    }
}

bool WebGLFramebuffer::IsStencilRenderAble(GLenum internalFormat, bool includesDepthStencil) const
{
    switch (internalFormat) {
        case WebGLRenderingContextBase::STENCIL_INDEX8:
            return true;
        case WebGLRenderingContextBase::DEPTH_STENCIL:
        case WebGL2RenderingContextBase::DEPTH24_STENCIL8:
        case WebGL2RenderingContextBase::DEPTH32F_STENCIL8:
            return includesDepthStencil;
        default:
            return false;
    }
}

bool WebGLFramebuffer::GetWebGLAttachmentInfo(napi_env env, Impl::WebGLRenderingContextBaseImpl* context,
    const WebGLAttachment* attachedObject, WebGLAttachmentInfo& info) const
{
    if (attachedObject == nullptr) {
        return false;
    }
    
    LOGD("GetWebGLAttachmentInfo %{public}u %{public}d %{public}lu",
         static_cast<unsigned int>(attachedObject->type), attachedObject->attachment,
         static_cast<unsigned long>(attachedObject->id));
    if (attachedObject->IsRenderBuffer()) {
        WebGLRenderbuffer* renderBuffer =
            context->GetObjectInstance<WebGLRenderbuffer>(env, static_cast<uint64_t>(attachedObject->id));
        if (renderBuffer == nullptr) {
            return false;
        }
        info.format = renderBuffer->GetInternalFormat();
        info.width = renderBuffer->GetWidth();
        info.height = renderBuffer->GetHeight();
        info.type = 0;
        return true;
    }
    if (attachedObject->IsTexture()) {
        WebGLTexture* texture = context->GetObjectInstance<WebGLTexture>(
            env, static_cast<uint64_t>(attachedObject->id));
        if (texture == nullptr) {
            return false;
        }
        auto textureAttachment = static_cast<const AttachmentTexture *>(attachedObject);
        LOGD("GetWebGLAttachmentInfo textureAttachment target %{public}u %{public}d",
            textureAttachment->target, textureAttachment->level);
        info.format = texture->GetInternalFormat(textureAttachment->target, textureAttachment->level);
        info.width = texture->GetWidth(textureAttachment->target, textureAttachment->level);
        info.height = texture->GetHeight(textureAttachment->target, textureAttachment->level);
        info.type = texture->GetType(textureAttachment->target, textureAttachment->level);
        return true;
    }
    return false;
}

bool WebGLFramebuffer::CheckAttachmentComplete(bool isHighWebGL, WebGLAttachment* attachedObject,
    const WebGLAttachmentInfo& info, std::vector<WebGLAttachment*>& attachments) const
{
    switch (attachedObject->attachment) {
        case WebGLRenderingContextBase::DEPTH_ATTACHMENT: {
            if (!IsDepthRenderAble(info.format, isHighWebGL)) {
                return false;
            }
            attachments[0] = attachedObject; // attachments[0]: depthAttachment
            break;
        }
        case WebGLRenderingContextBase::STENCIL_ATTACHMENT: {
            if (!IsStencilRenderAble(info.format, isHighWebGL)) {
                return false;
            }
            attachments[1] = attachedObject; // attachments[1]: stencilAttachment
            break;
        }
        case WebGLRenderingContextBase::DEPTH_STENCIL_ATTACHMENT: {
            if (info.format != GL_DEPTH_STENCIL_OES) {
                return false;
            }
            attachments[2] = attachedObject; // attachments[2]: depthStencilAttachment
            break;
        }
        default:
            if (!IsColorRenderAble(info.format)) {
                return false;
            }
            break;
    }
    return true;
}

GLenum WebGLFramebuffer::CheckStatus(napi_env env, Impl::WebGLRenderingContextBaseImpl* context,
    WebGLAttachmentInfo info, std::vector<WebGLAttachment*>& attachments, WebGLAttachment* attachedObject) const
{
    if (!GetWebGLAttachmentInfo(env, context, attachedObject, info)) {
        LOGE("GetWebGLAttachmentInfo failed.");
        return WebGLRenderingContextBase::FRAMEBUFFER_UNSUPPORTED;
    }
    if (!info.format || !info.width || !info.height) {
        LOGE("CheckStatus info failed.");
        return WebGLRenderingContextBase::FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
    }
    if (!CheckAttachmentComplete(context->IsHighWebGL(), attachedObject, info, attachments)) {
        LOGE("CheckAttachmentComplete failed.");
        return WebGLRenderingContextBase::FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
    }
    return WebGLRenderingContextBase::FRAMEBUFFER_COMPLETE;
}

GLenum WebGLFramebuffer::CheckAttachStatus(Impl::WebGLRenderingContextBaseImpl* context,
    std::vector<WebGLAttachment*>& attachments) const
{
    // WebGL 1 specific: no conflicting DEPTH/STENCIL/DEPTH_STENCIL attachments.
    // 0 depthAttachment, 1 stencilAttachment, 2 depthStencilAttachment
    if (!context->IsHighWebGL() && ((attachments[2] && (attachments[0] || attachments[1])) ||
        (attachments[0] && attachments[1]))) {
        LOGE("Check status depthStencilAttachment failed.");
        return WebGLRenderingContextBase::FRAMEBUFFER_UNSUPPORTED;
    }
    if (context->IsHighWebGL() && (attachments[0] && attachments[1] && attachments[0]->id != attachments[1]->id)) {
        LOGE("Check status depthAttachment failed.");
        return WebGLRenderingContextBase::FRAMEBUFFER_UNSUPPORTED;
    }
    return WebGLRenderingContextBase::FRAMEBUFFER_COMPLETE;
}

GLenum WebGLFramebuffer::CheckStatus(napi_env env, Impl::WebGLRenderingContextBaseImpl* context) const
{
    uint32_t count = 0;
    GLsizei width = 0;
    GLsizei height = 0;
    std::vector<WebGLAttachment*> attachments = {nullptr, nullptr, nullptr};
    WebGLAttachmentInfo info;
    GLenum result;
    LOGD("CheckStatus %{public}u", static_cast<unsigned int>(attachments_.size()));
    for (const auto& it : attachments_) {
        WebGLAttachment* attachedObject = it.second;
        result = CheckStatus(env, context, info, attachments, attachedObject);
        if (result != WebGLRenderingContextBase::FRAMEBUFFER_COMPLETE) {
            return result;
        }
        if (!context->IsHighWebGL()) {
            if (!count) {
                width = info.width;
                height = info.height;
            } else if (width != info.width || height != info.height) {
                LOGE("CheckStatus attachmentInfo failed.");
                return WebGLRenderingContextBase::FRAMEBUFFER_INCOMPLETE_DIMENSIONS;
            }
        }
        ++count;
    }
    if (!count) {
        return WebGLRenderingContextBase::FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT;
    }

    return CheckAttachStatus(context, attachments);
}
} // namespace Rosen
} // namespace OHOS
