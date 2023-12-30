/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "context/webgl2_rendering_context_base.h"
#include "context/webgl2_rendering_context_impl.h"
#include "context/webgl_rendering_context_base.h"
#include "napi/n_func_arg.h"
#include "util/log.h"
#include "webgl/webgl_arg.h"
#include "webgl/webgl_buffer.h"
#include "webgl/webgl_framebuffer.h"
#include "webgl/webgl_program.h"
#include "webgl/webgl_renderbuffer.h"
#include "webgl/webgl_sampler.h"
#include "webgl/webgl_shader.h"
#include "webgl/webgl_texture.h"
#include "webgl/webgl_transform_feedback.h"
#include "webgl/webgl_uniform_location.h"
#include "webgl/webgl_vertex_array_object.h"

namespace OHOS {
namespace Rosen {
namespace Impl {
using namespace std;
napi_value WebGL2RenderingContextImpl::GetParameter(napi_env env, GLenum pname)
{
    switch (pname) {
        case GL_SHADING_LANGUAGE_VERSION: {
            std::string str("WebGL GLSL ES 3.00 (");
            str += std::string(const_cast<char*>(reinterpret_cast<const char*>(glGetString(pname))));
            str += ")";
            return NVal::CreateUTF8String(env, str).val_;
        }
        case GL_VERSION: {
            std::string str("WebGL 2.0 (");
            str += std::string(const_cast<char*>(reinterpret_cast<const char*>(glGetString(pname))));
            str += ")";
            return NVal::CreateUTF8String(env, str).val_;
        }
        case GL_COPY_READ_BUFFER_BINDING:
            return GetObject<WebGLBuffer>(env, boundBufferIds_[BoundBufferType::COPY_READ_BUFFER]);
        case GL_COPY_WRITE_BUFFER_BINDING:
            return GetObject<WebGLBuffer>(env, boundBufferIds_[BoundBufferType::COPY_WRITE_BUFFER]);
        case GL_DRAW_FRAMEBUFFER_BINDING:
            return GetObject<WebGLBuffer>(env, boundFrameBufferIds_[BoundFrameBufferType::DRAW_FRAMEBUFFER]);
        case GL_FRAGMENT_SHADER_DERIVATIVE_HINT:
            return WebGLArg::GetUint32Parameter(env, pname);
        case GL_MAX_3D_TEXTURE_SIZE:
        case GL_MAX_ARRAY_TEXTURE_LAYERS:
        case GL_MAX_COLOR_ATTACHMENTS:
        case GL_MAX_COMBINED_UNIFORM_BLOCKS:
        case GL_MAX_DRAW_BUFFERS:
        case GL_MAX_ELEMENTS_INDICES:
        case GL_MAX_ELEMENTS_VERTICES:
        case GL_MAX_FRAGMENT_INPUT_COMPONENTS:
        case GL_MAX_FRAGMENT_UNIFORM_BLOCKS: //
        case GL_MAX_FRAGMENT_UNIFORM_COMPONENTS:
        case GL_MAX_PROGRAM_TEXEL_OFFSET:
        case GL_MAX_SAMPLES:
        case GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS:
        case GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS:
        case GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS:
        case GL_MAX_UNIFORM_BUFFER_BINDINGS:
        case GL_MAX_VARYING_COMPONENTS:
        case GL_MAX_VERTEX_OUTPUT_COMPONENTS:
        case GL_MAX_VERTEX_UNIFORM_BLOCKS:
        case GL_MAX_VERTEX_UNIFORM_COMPONENTS:
        case GL_MIN_PROGRAM_TEXEL_OFFSET:
        case GL_PACK_ROW_LENGTH:
        case GL_PACK_SKIP_PIXELS:
        case GL_PACK_SKIP_ROWS:
        case GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT:
        case GL_UNPACK_IMAGE_HEIGHT:
        case GL_UNPACK_ROW_LENGTH:
            return WebGLArg::GetInt32Parameter(env, pname);
        case GL_MAX_UNIFORM_BLOCK_SIZE:
        case GL_MAX_SERVER_WAIT_TIMEOUT:
        case GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS:
        case GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS:
        case GL_MAX_ELEMENT_INDEX:
            return WebGLArg::GetInt64Parameter(env, pname);
        case GL_MAX_TEXTURE_LOD_BIAS:
            return WebGLArg::GetFloatParameter(env, pname);
        case GL_PIXEL_PACK_BUFFER_BINDING: //
            return GetObject<WebGLBuffer>(env, boundBufferIds_[BoundBufferType::PIXEL_PACK_BUFFER]);
        case GL_PIXEL_UNPACK_BUFFER_BINDING:
            return GetObject<WebGLBuffer>(env, boundBufferIds_[BoundBufferType::PIXEL_UNPACK_BUFFER]);
        case GL_RASTERIZER_DISCARD:
        case GL_SAMPLE_ALPHA_TO_COVERAGE:
        case GL_SAMPLE_COVERAGE:
        case GL_TRANSFORM_FEEDBACK_ACTIVE:
        case GL_TRANSFORM_FEEDBACK_PAUSED:
        case GL_UNPACK_SKIP_IMAGES:
        case GL_UNPACK_SKIP_PIXELS:
        case GL_UNPACK_SKIP_ROWS:
            return WebGLArg::GetBoolParameter(env, pname);
        case WebGL2RenderingContextBase::MAX_CLIENT_WAIT_TIMEOUT_WEBGL:
            return WebGLArg::GetInt32Parameter(env, WebGL2RenderingContextBase::MAX_CLIENT_WAIT_TIMEOUT_WEBGL);
        case GL_READ_BUFFER:{
            WebGLFramebuffer* readFrameBuffer = GetBoundFrameBuffer(env, GL_READ_FRAMEBUFFER);
            GLenum value = readFrameBuffer != nullptr ? readFrameBuffer->GetReadBufferMode() : defaultReadBufferMode_;
            return NVal::CreateInt64(env, value).val_;
        }
        case GL_READ_FRAMEBUFFER_BINDING:
            return GetObject<WebGLFramebuffer>(env, boundBufferIds_[BoundFrameBufferType::READ_FRAMEBUFFER]);
        case GL_TRANSFORM_FEEDBACK_BUFFER_BINDING:
            return GetObject<WebGLBuffer>(env, boundBufferIds_[BoundBufferType::TRANSFORM_FEEDBACK_BUFFER]);
        case GL_TRANSFORM_FEEDBACK_BINDING:
            return GetObject<WebGLTransformFeedback>(env, boundTransformFeedback_);
        case GL_UNIFORM_BUFFER_BINDING:
            return GetObject<WebGLBuffer>(env, boundBufferIds_[BoundBufferType::UNIFORM_BUFFER]);
        case GL_SAMPLER_BINDING:
            return GetObject<WebGLSampler>(env, samplerUnits_[activeTextureIndex_]);
        case GL_TEXTURE_BINDING_2D_ARRAY:
            return GetObject<WebGLTexture>(env, boundTexture_[BoundTextureType::TEXTURE_2D_ARRAY][activeTextureIndex_]);
        case GL_TEXTURE_BINDING_3D:
            return GetObject<WebGLTexture>(env, boundTexture_[BoundTextureType::TEXTURE_3D][activeTextureIndex_]);
        case GL_VERTEX_ARRAY_BINDING:
            return GetObject<WebGLVertexArrayObject>(env, boundVertexArrayId_);
        default:
            break;
    }
    return WebGLRenderingContextBaseImpl::GetParameter(env, pname);
}

napi_value WebGL2RenderingContextImpl::GetTexParameter(napi_env env, GLenum target, GLenum pname)
{
    if (GetBoundTexture(env, target, false) == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    switch (pname) {
        case GL_TEXTURE_WRAP_R:
        case GL_TEXTURE_COMPARE_FUNC:
        case GL_TEXTURE_COMPARE_MODE:
        case GL_TEXTURE_IMMUTABLE_LEVELS: {
            GLint value = 0;
            glGetTexParameteriv(target, pname, &value);
            return NVal::CreateInt64(env, static_cast<int64_t>(value)).val_;
        }
        case GL_TEXTURE_IMMUTABLE_FORMAT: {
            GLint value = 0;
            glGetTexParameteriv(target, pname, &value);
            return NVal::CreateBool(env, value != 0).val_;
        }
        case GL_TEXTURE_BASE_LEVEL:
        case GL_TEXTURE_MAX_LEVEL: {
            GLint value = 0;
            glGetTexParameteriv(target, pname, &value);
            return NVal::CreateInt64(env, static_cast<int64_t>(value)).val_;
        }
        case GL_TEXTURE_MAX_LOD:
        case GL_TEXTURE_MIN_LOD: {
            GLfloat value = 0.f;
            glGetTexParameterfv(target, pname, &value);
            return NVal::CreateDouble(env, static_cast<double>(value)).val_;
        }
        default:
            break;
    }
    return WebGLRenderingContextBaseImpl::GetTexParameter(env, target, pname);
}

bool WebGL2RenderingContextImpl::CheckGetFrameBufferAttachmentParameter(
    napi_env env, GLenum target, GLenum attachment, const WebGLFramebuffer* frameBuffer)
{
    uint32_t index;
    if (!CheckFrameBufferTarget(env, target, index)) {
        SET_ERROR(GL_INVALID_ENUM);
        return false;
    }
    if (frameBuffer == nullptr) { // for the default framebuffer
        switch (attachment) {
            case GL_BACK:
            case GL_DEPTH:
            case GL_STENCIL:
                break;
            default:
                SET_ERROR(GL_INVALID_ENUM);
                return false;
        }
        return true;
    }
    switch (attachment) {
        case GL_COLOR_ATTACHMENT0:
        case GL_DEPTH_ATTACHMENT:
        case GL_STENCIL_ATTACHMENT:
        case GL_DEPTH_STENCIL_ATTACHMENT:
            break;
        default:
            if (attachment > GL_COLOR_ATTACHMENT0 &&
                attachment < static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + GetMaxColorAttachments())) {
                break;
            }
            SET_ERROR(GL_INVALID_ENUM);
            return false;
    }
    return true;
}

napi_value WebGL2RenderingContextImpl::GetFrameBufferAttachmentParameterForDefault(
    napi_env env, GLenum target, GLenum attachment, GLenum pname)
{
    auto contextAttributes = webGLRenderingContext_->CreateWebGlContextAttributes();
    if (contextAttributes == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    if ((attachment == GL_DEPTH && !contextAttributes->depth_) ||
        (attachment == GL_STENCIL && !contextAttributes->stencil_)) {
        if (pname == GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE) {
            return NVal::CreateInt64(env, GL_FRAMEBUFFER_DEFAULT).val_;
        } else {
            SET_ERROR(GL_INVALID_ENUM);
            return NVal::CreateNull(env).val_;
        }
    }
    switch (pname) {
        case GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE:
            return NVal::CreateInt64(env, GL_FRAMEBUFFER_DEFAULT).val_;
        case GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE:
        case GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE:
        case GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE: {
            GLint value = attachment == GL_BACK ? 8 : 0; // 8 GREEN size
            return NVal::CreateInt64(env, value).val_;
        }
        case GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE: {
            GLint value = (attachment == GL_BACK && contextAttributes->alpha_) ? 8 : 0; // 8 ALPHA size
            return NVal::CreateInt64(env, value).val_;
        }
        case GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE: {
            // For ES3 capable backend, DEPTH24_STENCIL8 has to be supported.
            GLint value = attachment == GL_DEPTH ? 24 : 0; // 24 depth size
            return NVal::CreateInt64(env, value).val_;
        }
        case GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE: {
            GLint value = attachment == GL_STENCIL ? 8 : 0; // 8 STENCIL size
            return NVal::CreateInt64(env, value).val_;
        }
        case GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE:
            return NVal::CreateInt64(env, GL_UNSIGNED_NORMALIZED).val_;
        case GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING:
            return NVal::CreateInt64(env, GL_LINEAR).val_;
        default:
            break;
    }
    SET_ERROR(GL_INVALID_ENUM);
    return NVal::CreateNull(env).val_;
}

napi_value WebGL2RenderingContextImpl::HandleFrameBufferPname(
    napi_env env, GLenum target, GLenum attachment, GLenum pname, WebGLAttachment* attachmentObject)
{
    switch (pname) {
        case GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE:
            return attachmentObject->IsTexture() ? NVal::CreateInt64(env, GL_TEXTURE).val_ :
                NVal::CreateInt64(env, GL_RENDERBUFFER).val_;
        case GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME:
            return attachmentObject->IsTexture() ? GetObject<WebGLTexture>(env, attachmentObject->id) :
                GetObject<WebGLRenderbuffer>(env, attachmentObject->id);
        case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE:
        case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER:
        case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL:
            if (!attachmentObject->IsTexture()) {
                break;
            }
            [[fallthrough]];
        case GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE:
        case GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE:
        case GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE:
        case GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE:
        case GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE:
        case GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE: {
            GLint value = 0;
            glGetFramebufferAttachmentParameteriv(target, attachment, pname, &value);
            return NVal::CreateInt64(env, value).val_;
        }
        case GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE:
            if (attachment == GL_DEPTH_STENCIL_ATTACHMENT) {
                SET_ERROR(GL_INVALID_OPERATION);
                return NVal::CreateNull(env).val_;
            }
            [[fallthrough]];
        case GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING: {
            GLint value = 0;
            glGetFramebufferAttachmentParameteriv(target, attachment, pname, &value);
            return NVal::CreateInt64(env, value).val_;
        }
        default:
            break;
    }
    SET_ERROR(GL_INVALID_ENUM);
    return NVal::CreateNull(env).val_;
}

napi_value WebGL2RenderingContextImpl::GetFrameBufferAttachmentParameter(
    napi_env env, GLenum target, GLenum attachment, GLenum pname)
{
    WebGLFramebuffer* frameBuffer = GetBoundFrameBuffer(env, target);
    if (CheckGetFrameBufferAttachmentParameter(env, target, attachment, frameBuffer)) {
        return NVal::CreateNull(env).val_;
    }

    if (frameBuffer == nullptr) {
        return GetFrameBufferAttachmentParameterForDefault(env, target, attachment, pname);
    }

    WebGLAttachment* attachmentObject = nullptr;
    if (attachment == GL_DEPTH_STENCIL_ATTACHMENT) {
        WebGLAttachment* depthAttachment = frameBuffer->GetAttachment(GL_DEPTH_ATTACHMENT);
        WebGLAttachment* stencilAttachment = frameBuffer->GetAttachment(GL_STENCIL_ATTACHMENT);
        if (depthAttachment == nullptr || stencilAttachment == nullptr ||
            depthAttachment->id != stencilAttachment->id) {
            SET_ERROR(GL_INVALID_OPERATION);
            return NVal::CreateNull(env).val_;
        }
        attachmentObject = depthAttachment;
    } else {
        attachmentObject = frameBuffer->GetAttachment(attachment);
    }

    if (attachmentObject == nullptr) {
        if (pname == GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE) {
            return NVal::CreateInt64(env, GL_NONE).val_;
        } else if (pname != GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME) {
            SET_ERROR(GL_INVALID_OPERATION);
        }
        return NVal::CreateNull(env).val_;
    }
    return HandleFrameBufferPname(env, target, attachment, pname, attachmentObject);
}

void WebGL2RenderingContextImpl::DoObjectDelete(int32_t type, WebGLObject *obj)
{
    WebGLRenderingContextBaseImpl::DoObjectDelete(type, obj);
    if (type == WebGLObject::WEBGL_OBJECT_BUFFER) {
        WebGLBuffer *buffer = static_cast<WebGLBuffer *>(obj);
        LOGD("DoObjectDelete %{public}d %{public}u", type, buffer->GetBufferId());
        for (size_t i = 0; i < boundIndexedTransformFeedbackBuffers_.size(); ++i) {
            if (boundIndexedTransformFeedbackBuffers_[i] == buffer->GetBufferId()) {
                boundIndexedTransformFeedbackBuffers_.erase(i);
                break;
            }
        }
        for (auto it = boundIndexedUniformBuffers_.begin(); it != boundIndexedUniformBuffers_.end(); ++it) {
            if (it->second == buffer->GetBufferId()) {
                boundIndexedUniformBuffers_.erase(it);
                break;
            }
        }
    }
}
} // namespace Impl
} // namespace Rosen
} // namespace OHOS
