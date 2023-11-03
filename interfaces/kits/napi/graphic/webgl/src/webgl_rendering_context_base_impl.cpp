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
#include "context/webgl_rendering_context_base_impl.h"

#include "context/webgl2_rendering_context_base.h"
#include "context/webgl_rendering_context_base.h"
#include "napi/n_class.h"
#include "util/log.h"
#include "util/object_source.h"
#include "util/util.h"

namespace OHOS {
namespace Rosen {
namespace Impl {
using namespace std;
#define SET_ERROR(error)                                \
    do {                                                \
        LOGE("WebGL set error code %{public}u", error); \
        SetError(error);                                \
    } while (0)

#define SET_ERROR_WITH_LOG(error, info, ...)                                \
    do {                                                                    \
        LOGE("WebGL set error code %{public}u" info, error, ##__VA_ARGS__); \
        SetError(error);                                                    \
    } while (0)

WebGLRenderingContextBaseImpl::~WebGLRenderingContextBaseImpl() {}

void WebGLRenderingContextBaseImpl::Init()
{
    if (maxTextureImageUnits_ != 0) {
        return;
    }

    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureImageUnits_);
    boundTexture_[BoundTextureType::TEXTURE_3D].resize(maxTextureImageUnits_);
    boundTexture_[BoundTextureType::TEXTURE_2D_ARRAY].resize(maxTextureImageUnits_);

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize_);
    boundTexture_[BoundTextureType::TEXTURE_2D].resize(maxTextureSize_);

    glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &maxCubeMapTextureSize_);
    boundTexture_[BoundTextureType::TEXTURE_CUBE_MAP].resize(maxCubeMapTextureSize_);

    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderBufferSize_);

    unpackColorspaceConversion_ = WebGLRenderingContextBase::BROWSER_DEFAULT_WEBGL;

    GLint max = 0;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max);
    maxVertexAttribs_ = static_cast<GLuint>(max);
    arrayVertexAttribs_.resize(maxVertexAttribs_);

    stencilMask_[0] = 0xffffffff;
    stencilMask_[1] = 0xffffffff;
    stencilFuncMask_[0] = 0xffffffff;
    stencilFuncMask_[1] = 0xffffffff;
    stencilFuncRef_[0] = 0;
    stencilFuncRef_[1] = 0;

    clearColor_[0] = 0; // 0: r
    clearColor_[1] = 0; // 1: g
    clearColor_[2] = 0; // 2: b
    clearColor_[3] = 0; // 3: a

    colorMask_[0] = false; // 0: r
    colorMask_[1] = false; // 1: g
    colorMask_[2] = false; // 2: b
    colorMask_[3] = false; // 3: a

    LOGD("WebGL Init maxTextureImageUnits %{public}u, maxTextureSize %{public}u, maxCubeMapTextureSize %{public}u, "
        "maxRenderBufferSize %{public}u, maxVertexAttribs %{public}u", maxTextureImageUnits_,
        maxTextureSize_, maxCubeMapTextureSize_, maxRenderBufferSize_, maxVertexAttribs_);
}

napi_value WebGLRenderingContextBaseImpl::GetContextAttributes(napi_env env)
{
    auto webGlContextAttributes = webGLRenderingContext_->CreateWebGlContextAttributes();
    if (webGlContextAttributes == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    NVal res = NVal::CreateObject(env);
    napi_value alpha = NVal::CreateBool(env, webGlContextAttributes->alpha_).val_;
    napi_value antialias = NVal::CreateBool(env, webGlContextAttributes->antialias_).val_;
    napi_value depth = NVal::CreateBool(env, webGlContextAttributes->depth_).val_;
    napi_value failIfMajorPerformanceCaveat =
        NVal::CreateBool(env, webGlContextAttributes->failIfMajorPerformanceCaveat_).val_;
    napi_value desynchronized = NVal::CreateBool(env, webGlContextAttributes->desynchronized_).val_;
    napi_value premultipliedAlpha = NVal::CreateBool(env, webGlContextAttributes->premultipliedAlpha_).val_;
    napi_value preserveDrawingBuffer = NVal::CreateBool(env, webGlContextAttributes->preserveDrawingBuffer_).val_;
    napi_value stencil = NVal::CreateBool(env, webGlContextAttributes->stencil_).val_;
    napi_value powerPreference = NVal::CreateUTF8String(env, webGlContextAttributes->powerPreference_).val_;
    res.AddProp("alpha", alpha);
    res.AddProp("antialias", antialias);
    res.AddProp("depth", depth);
    res.AddProp("failIfMajorPerformanceCaveat", failIfMajorPerformanceCaveat);
    res.AddProp("desynchronized", desynchronized);
    res.AddProp("premultipliedAlpha", premultipliedAlpha);
    res.AddProp("preserveDrawingBuffer", preserveDrawingBuffer);
    res.AddProp("stencil", stencil);
    res.AddProp("powerPreference", powerPreference);
    return res.val_;
}

napi_value WebGLRenderingContextBaseImpl::CreateTextureObject(napi_env env)
{
    WebGLTexture* webGlTexture = nullptr;
    napi_value objTexture = WebGLTexture::CreateObjectInstance(env, &webGlTexture).val_;
    if (!webGlTexture) {
        return NVal::CreateNull(env).val_;
    }
    GLuint textureId;
    glGenTextures(1, &textureId);
    webGlTexture->SetTexture(textureId);
    LOGD("WebGL createTexture textureId %{public}u", textureId);
    AddObject<WebGLTexture>(env, textureId, objTexture);
    return objTexture;
}

napi_value WebGLRenderingContextBaseImpl::ActiveTexture(napi_env env, GLenum texture)
{
    if (texture < GL_TEXTURE0 || static_cast<GLint>(texture - GL_TEXTURE0) >= maxTextureImageUnits_) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM,
            "WebGL activeTexture texture unit out of range %{public}u", texture);
        return nullptr;
    }
    activeTextureIndex_ = texture - GL_TEXTURE0;
    glActiveTexture(texture);
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::BindTexture(napi_env env, GLenum target, napi_value textureRef)
{
    GLuint texture = WebGLTexture::DEFAULT_TEXTURE;
    WebGLTexture* webGlTexture = WebGLTexture::GetObjectInstance(env, textureRef);
    if (webGlTexture != nullptr) {
        texture = webGlTexture->GetTexture();
        if (webGlTexture->GetTarget() && webGlTexture->GetTarget() != target) {
            SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION,
                "WebGL bindTexture textureId %{public}u has been bound", texture);
            return nullptr;
        }
    }
    LOGD("WebGL bindTexture target %{public}u textureId %{public}u activeTextureIndex %{public}u",
        target, texture, activeTextureIndex_);

    uint32_t index = 0;
    if (!WebGLRenderingContextBaseImpl::CheckTextureTarget(env, target, index)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    if (activeTextureIndex_ >= boundTexture_[index].size()) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    if (boundTexture_[index][activeTextureIndex_] && boundTexture_[index][activeTextureIndex_] != texture) {
        LOGD("WebGL bindTexture has been bound to textureId %{public}u", boundTexture_[index][activeTextureIndex_]);
    }
    boundTexture_[index][activeTextureIndex_] = texture;

    glBindTexture(target, texture);
    if (webGlTexture) {
        webGlTexture->SetTarget(target);
    }
    LOGD("WebGL bindTexture textureId %{public}u result %{public}u ", texture, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::IsBuffer(napi_env env, napi_value object)
{
    WebGLBuffer* webGlBuffer = GetValidBuffer(env, object);
    if (webGlBuffer == nullptr) {
        return NVal::CreateBool(env, false).val_;
    }
    unsigned int buffer = webGlBuffer->GetBufferId();
    bool result = static_cast<bool>(glIsBuffer(static_cast<GLuint>(buffer)));
    LOGD("WebGL isBuffer res %{public}u  %{public}u", result, buffer);
    return NVal::CreateBool(env, result).val_;
}

napi_value WebGLRenderingContextBaseImpl::BindBuffer(napi_env env, GLenum target, napi_value object)
{
    LOGD("WebGL bindBuffer target %{public}u ", target);
    // support default value
    uint32_t bufferId = WebGLBuffer::DEFAULT_BUFFER;
    WebGLBuffer* webGlBuffer = GetValidBuffer(env, object);
    if (webGlBuffer != nullptr) {
        bufferId = webGlBuffer->GetBufferId();
    }
    LOGD("WebGL bindBuffer target %{public}u bufferId %{public}u", target, bufferId);
    uint32_t index = BoundBufferType::ARRAY_BUFFER;
    if (!CheckBufferTarget(env, target, index)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    if (webGlBuffer && webGlBuffer->GetTarget() && webGlBuffer->GetTarget() != target) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    if (boundBufferIds_[index] && boundBufferIds_[index] != bufferId) {
        LOGD("WebGL bindBuffer has been bound bufferId %{public}u", boundBufferIds_[index]);
    }
    boundBufferIds_[index] = bufferId;

    glBindBuffer(target, static_cast<GLuint>(bufferId));
    if (webGlBuffer) {
        webGlBuffer->SetTarget(target);
    }
    LOGD("WebGL bindBuffer bufferId %{public}u result %{public}u", bufferId, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::IsFrameBuffer(napi_env env, napi_value object)
{
    GLuint frameBufferId = WebGLFramebuffer::DEFAULT_FRAME_BUFFER;
    WebGLFramebuffer* webGlFramebuffer = GetValidFrameBuffer(env, object);
    if (webGlFramebuffer == nullptr) {
        return NVal::CreateBool(env, false).val_;
    }
    frameBufferId = webGlFramebuffer->GetFramebuffer();
    bool res = static_cast<bool>(glIsFramebuffer(frameBufferId));
    LOGD("WebGL isFramebuffer framebuffer %{public}u result %{public}u", frameBufferId, res);
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGLRenderingContextBaseImpl::BindFrameBuffer(napi_env env, GLenum target, napi_value object)
{
    LOGD("WebGL bindFramebuffer target %{public}u ", target);
    uint32_t index = 0;
    if (!CheckFrameBufferTarget(env, target, index)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "BindFrameBuffer Invalid target");
        return nullptr;
    }

    GLuint frameBufferId = WebGLFramebuffer::DEFAULT_FRAME_BUFFER;
    WebGLFramebuffer* webGlFramebuffer = GetValidFrameBuffer(env, object);
    if (webGlFramebuffer == nullptr) {
        glBindFramebuffer(target, 0);
        return nullptr;
    }

    frameBufferId = webGlFramebuffer->GetFramebuffer();
    if (boundFrameBufferIds_[index] && boundFrameBufferIds_[index] != frameBufferId) {
        LOGD("WebGL bindFramebuffer has been bound %{public}u", boundFrameBufferIds_[index]);
    }
    glBindFramebuffer(target, frameBufferId);
    boundFrameBufferIds_[index] = frameBufferId;
    if (IsHighWebGL()) {
        if (target == GL_FRAMEBUFFER || target == GL_READ_FRAMEBUFFER) {
            boundFrameBufferIds_[BoundFrameBufferType::READ_FRAMEBUFFER] = frameBufferId;
        }
    }
    if (target == GL_FRAMEBUFFER || target == GL_DRAW_FRAMEBUFFER) {
        boundFrameBufferIds_[BoundFrameBufferType::FRAMEBUFFER] = frameBufferId;
    }

    if (webGlFramebuffer) {
        webGlFramebuffer->SetTarget(target);
    }
    LOGD("WebGL bindFramebuffer frameBufferId %{public}u result %{public}u", frameBufferId, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::BindRenderBuffer(napi_env env, GLenum target, napi_value object)
{
    LOGD("WebGL bindRenderbuffer target %{public}u", target);
    uint32_t index = 0;
    if (!CheckRenderBufferTarget(env, target, index)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "BindRenderBuffer Invalid target");
        return nullptr;
    }

    GLuint renderBufferId = WebGLRenderbuffer::DEFAULT_RENDER_BUFFER;
    WebGLRenderbuffer* renderBuffer = GetValidRenderBuffer(env, object);
    if (renderBuffer != nullptr) {
        renderBufferId = renderBuffer->GetRenderbuffer();
    }

    glBindRenderbuffer(target, renderBufferId);

    // record render buffer
    if (boundRenderBufferIds_[index] && boundRenderBufferIds_[index] != renderBufferId) {
        LOGD("WebGL bindRenderbuffer has been bound %{public}u", boundRenderBufferIds_[index]);
    }
    boundRenderBufferIds_[index] = renderBufferId;

    if (renderBuffer) {
        renderBuffer->SetTarget(target);
    }
    LOGD("WebGL bindRenderbuffer %{public}u", GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::CreateBuffer(napi_env env)
{
    WebGLBuffer* webGlBuffer = nullptr;
    napi_value objBuffer = WebGLBuffer::CreateObjectInstance(env, &webGlBuffer).val_;
    if (webGlBuffer == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    unsigned int bufferId;
    glGenBuffers(1, &bufferId);
    webGlBuffer->SetBufferId(bufferId);
    bool ret = AddObject<WebGLBuffer>(env, bufferId, objBuffer);
    LOGD("WebGL createBuffer bufferId %{public}u %{public}u", bufferId, ret);
    return objBuffer;
}

napi_value WebGLRenderingContextBaseImpl::CreateFrameBuffer(napi_env env)
{
    WebGLFramebuffer* webGlFramebuffer = nullptr;
    napi_value objFramebuffer = WebGLFramebuffer::CreateObjectInstance(env, &webGlFramebuffer).val_;
    if (!webGlFramebuffer) {
        return NVal::CreateNull(env).val_;
    }
    unsigned int framebufferId;
    glGenFramebuffers(1, &framebufferId);
    webGlFramebuffer->SetFramebuffer(framebufferId);
    (void)AddObject<WebGLFramebuffer>(env, framebufferId, objFramebuffer);
    LOGD("WebGL createFramebuffer framebufferId %{public}u", framebufferId);
    return objFramebuffer;
}

napi_value WebGLRenderingContextBaseImpl::CreateProgram(napi_env env)
{
    WebGLProgram* webGlProgram = nullptr;
    napi_value objProgram = WebGLProgram::CreateObjectInstance(env, &webGlProgram).val_;
    if (webGlProgram == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    GLuint programId = glCreateProgram();
    if (programId == 0) {
        LOGD("WebGL createProgram fail programId %{public}u", programId);
        return NVal::CreateNull(env).val_;
    }
    webGlProgram->SetProgramId(programId);

    (void)AddObject<WebGLProgram>(env, programId, objProgram);
    LOGD("WebGL createProgram programId %{public}u result %{public}u", programId, GetError_());
    return objProgram;
}

napi_value WebGLRenderingContextBaseImpl::CreateRenderBuffer(napi_env env)
{
    WebGLRenderbuffer* webGlRenderbuffer = nullptr;
    napi_value objRenderbuffer = WebGLRenderbuffer::CreateObjectInstance(env, &webGlRenderbuffer).val_;
    if (!webGlRenderbuffer) {
        return NVal::CreateNull(env).val_;
    }
    GLuint renderbufferId;
    glGenRenderbuffers(1, &renderbufferId);
    webGlRenderbuffer->SetRenderbuffer(renderbufferId);
    (void)AddObject<WebGLRenderbuffer>(env, renderbufferId, objRenderbuffer);
    LOGD("WebGL createRenderbuffer renderbufferId %{public}u result %{public}p", renderbufferId, objRenderbuffer);
    return objRenderbuffer;
}

napi_value WebGLRenderingContextBaseImpl::CreateShader(napi_env env, GLenum type)
{
    if (type != WebGLRenderingContextBase::VERTEX_SHADER && type != WebGLRenderingContextBase::FRAGMENT_SHADER) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "createShader Invalid type");
        return NVal::CreateNull(env).val_;
    }
    WebGLShader* webGlShader = nullptr;
    napi_value objShader = WebGLShader::CreateObjectInstance(env, &webGlShader).val_;
    if (!webGlShader) {
        return NVal::CreateNull(env).val_;
    }
    GLuint shaderId = glCreateShader(type);
    webGlShader->SetShaderId(shaderId);
    LOGD("WebGL createShader shaderId = %{public}u type %{public}u", shaderId, type);
    if (shaderId == 0) {
        LOGE("WebGL create shader failed. type %40{public}x", type);
        return NVal::CreateNull(env).val_;
    }
    (void)AddObject<WebGLShader>(env, shaderId, objShader);
    webGlShader->SetShaderType(type);
    return objShader;
}

napi_value WebGLRenderingContextBaseImpl::AttachShader(napi_env env, napi_value programObj, napi_value shaderObj)
{
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, programObj);
    if (webGLProgram == nullptr) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "WebGL attach shader invalid program");
        return nullptr;
    }
    uint32_t programId = webGLProgram->GetProgramId();

    WebGLShader* webGlShader = WebGLShader::GetObjectInstance(env, shaderObj);
    if (webGlShader == nullptr) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "WebGL attach shader invalid shader");
        return nullptr;
    }
    uint32_t shaderId = webGlShader->GetShaderId();
    LOGD("WebGL attach shader programId %{public}u, shaderId %{public}u", programId, shaderId);
    uint32_t index = 0;
    if (!CheckShaderType(env, webGlShader->GetShaderType(), index) || !webGLProgram->AttachShader(index, shaderId)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "WebGL attach shader invalid shader");
        return nullptr;
    }
    glAttachShader(static_cast<GLuint>(programId), static_cast<GLuint>(shaderId));
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::DetachShader(napi_env env, napi_value programObj, napi_value shaderObj)
{
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, programObj);
    if (webGLProgram == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    uint32_t programId = webGLProgram->GetProgramId();

    WebGLShader* webGlShader = WebGLShader::GetObjectInstance(env, shaderObj);
    if (webGlShader == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    uint32_t shaderId = webGlShader->GetShaderId();
    LOGD("WebGL detach shader programId %{public}u, shaderId %{public}u", programId, shaderId);
    uint32_t index = 0;
    if (!CheckShaderType(env, webGlShader->GetShaderType(), index) || !webGLProgram->DetachShader(index, shaderId)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "Invalid shader");
        return nullptr;
    }
    glDetachShader(static_cast<GLuint>(programId), static_cast<GLuint>(shaderId));
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::CompileShader(napi_env env, napi_value object)
{
    WebGLShader* webGlShader = WebGLShader::GetObjectInstance(env, object);
    if (webGlShader == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    uint32_t shaderId = webGlShader->GetShaderId();
    glCompileShader(static_cast<GLuint>(shaderId));
    LOGD("WebGL compile shader shaderId %{public}u result %{public}u", shaderId, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::ShaderSource(napi_env env, napi_value object, const std::string& source)
{
    WebGLShader* webGlShader = WebGLShader::GetObjectInstance(env, object);
    if (webGlShader == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    uint32_t shaderId = webGlShader->GetShaderId();
    LOGD("WebGL shaderSource shaderId %{public}u  source: '%{public}s'", shaderId, source.c_str());
    if (!WebGLArg::CheckString(source)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    GLint length = static_cast<GLint>(source.size());
    GLchar* str = const_cast<GLchar*>(source.c_str());
    glShaderSource(static_cast<GLuint>(shaderId), 1, &str, &length);
    webGlShader->SetShaderRes(source);
    LOGD("WebGL shaderSource shaderId %{public}u result %{public}u", shaderId, GetError_());
    return nullptr;
}

napi_value WebGLRenderingContextBaseImpl::DeleteBuffer(napi_env env, napi_value object)
{
    LOGD("WebGL deleteBuffer ");
    WebGLBuffer* webGlBuffer = GetValidBuffer(env, object);
    if (webGlBuffer == nullptr) {
        return nullptr;
    }
    GLuint buffer = webGlBuffer->GetBufferId();
    DoObjectDelete(WebGLBuffer::objectType, webGlBuffer);
    DeleteObject<WebGLBuffer>(env, buffer);
    glDeleteBuffers(1, &buffer);
    LOGD("WebGL deleteBuffer bufferId %{public}u %{public}p result %{public}u", buffer, webGlBuffer, GetError_());
    for (int i = 0; i < BoundBufferType::BUFFER_MAX; i++) {
        if (boundBufferIds_[i] == buffer) {
            boundBufferIds_[i] = 0;
        }
    }
    return nullptr;
}

} // namespace Impl
} // namespace Rosen
} // namespace OHOS
