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
#include "util/util.h"

namespace OHOS {
namespace Rosen {
namespace Impl {
using namespace std;
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

    stencilMask_[0] = 0xffffffff; // 0xffffffff: stencilMask_ max value
    stencilMask_[1] = 0xffffffff; // 0xffffffff: stencilMask_ max value
    stencilFuncMask_[0] = 0xffffffff; // 0xffffffff: stencilFuncMask_ max value
    stencilFuncMask_[1] = 0xffffffff; // 0xffffffff: stencilFuncMask_ max value
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

    LOGD("WebGL Init maxTextureImageUnits %{public}d, maxTextureSize %{public}d, maxCubeMapTextureSize %{public}d, "
        "maxRenderBufferSize %{public}d, maxVertexAttribs %{public}d", maxTextureImageUnits_,
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
    GLuint textureId = 0;
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
        return NVal::CreateNull(env).val_;
    }
    activeTextureIndex_ = texture - GL_TEXTURE0;
    glActiveTexture(texture);
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::BindTexture(napi_env env, GLenum target, napi_value textureRef)
{
    uint32_t texture = WebGLTexture::DEFAULT_TEXTURE;
    WebGLTexture* webGlTexture = WebGLTexture::GetObjectInstance(env, textureRef);
    if (webGlTexture != nullptr) {
        texture = webGlTexture->GetTexture();
        if (webGlTexture->GetTarget() && webGlTexture->GetTarget() != target) {
            SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION,
                "WebGL bindTexture textureId %{public}u has been bound", texture);
            return NVal::CreateNull(env).val_;
        }
    }

    uint32_t index = 0;
    if (!WebGLRenderingContextBaseImpl::CheckTextureTarget(env, target, index)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    if (activeTextureIndex_ >= boundTexture_[index].size()) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return NVal::CreateNull(env).val_;
    }
    if (boundTexture_[index][activeTextureIndex_] && boundTexture_[index][activeTextureIndex_] !=
        static_cast<uint32_t>(texture)) {
        LOGD("WebGL bindTexture has been bound to textureId %{public}u", boundTexture_[index][activeTextureIndex_]);
    }
    boundTexture_[index][activeTextureIndex_] = texture;

    glBindTexture(target, texture);
    if (webGlTexture) {
        webGlTexture->SetTarget(target);
    }
    LOGD("WebGL bindTexture target %{public}u textureId %{public}u activeTextureIndex %{public}u result %{public}u ",
        target, texture, activeTextureIndex_, GetError_());
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::IsBuffer(napi_env env, napi_value object)
{
    WebGLBuffer* webGlBuffer = GetValidBuffer(env, object);
    if (webGlBuffer == nullptr) {
        return NVal::CreateBool(env, false).val_;
    }
    uint32_t buffer = webGlBuffer->GetBufferId();
    bool result = static_cast<bool>(glIsBuffer(static_cast<GLuint>(buffer)));
    LOGD("WebGL isBuffer %{public}u res %{public}d", buffer, result);
    return NVal::CreateBool(env, result).val_;
}

napi_value WebGLRenderingContextBaseImpl::BindBuffer(napi_env env, GLenum target, napi_value object)
{
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
        return NVal::CreateNull(env).val_;
    }
    if (webGlBuffer && webGlBuffer->GetTarget() && webGlBuffer->GetTarget() != target) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return NVal::CreateNull(env).val_;
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
    return NVal::CreateNull(env).val_;
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
    LOGD("WebGL isFramebuffer framebuffer %{public}u result %{public}d", frameBufferId, res);
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGLRenderingContextBaseImpl::BindFrameBuffer(napi_env env, GLenum target, napi_value object)
{
    uint32_t index = 0;
    if (!CheckFrameBufferTarget(env, target, index)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "BindFrameBuffer Invalid target");
        return NVal::CreateNull(env).val_;
    }

    GLuint frameBufferId = WebGLFramebuffer::DEFAULT_FRAME_BUFFER;
    WebGLFramebuffer* webGlFramebuffer = GetValidFrameBuffer(env, object);
    if (webGlFramebuffer == nullptr) {
        glBindFramebuffer(target, 0);
        return NVal::CreateNull(env).val_;
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
    LOGD("WebGL bindFramebuffer target %{public}u frameBufferId %{public}u result %{public}u",
        target, frameBufferId, GetError_());
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::BindRenderBuffer(napi_env env, GLenum target, napi_value object)
{
    uint32_t index = 0;
    if (!CheckRenderBufferTarget(env, target, index)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "BindRenderBuffer Invalid target");
        return NVal::CreateNull(env).val_;
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
    LOGD("WebGL bindRenderbuffer target %{public}u %{public}u", target, GetError_());
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::CreateBuffer(napi_env env)
{
    WebGLBuffer* webGlBuffer = nullptr;
    napi_value objBuffer = WebGLBuffer::CreateObjectInstance(env, &webGlBuffer).val_;
    if (webGlBuffer == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    uint32_t bufferId = 0;
    glGenBuffers(1, &bufferId);
    webGlBuffer->SetBufferId(bufferId);
    bool ret = AddObject<WebGLBuffer>(env, bufferId, objBuffer);
    LOGD("WebGL createBuffer bufferId %{public}u %{public}d", bufferId, ret);
    return objBuffer;
}

napi_value WebGLRenderingContextBaseImpl::CreateFrameBuffer(napi_env env)
{
    WebGLFramebuffer* webGlFramebuffer = nullptr;
    napi_value objFramebuffer = WebGLFramebuffer::CreateObjectInstance(env, &webGlFramebuffer).val_;
    if (!webGlFramebuffer) {
        return NVal::CreateNull(env).val_;
    }
    uint32_t framebufferId = 0;
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
        LOGE("WebGL createProgram fail programId %{public}u", programId);
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
    GLuint renderbufferId = 0;
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
        LOGE("WebGL create shader failed. type %{public}x", type);
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
        return NVal::CreateNull(env).val_;
    }
    WebGLShader* webGlShader = WebGLShader::GetObjectInstance(env, shaderObj);
    if (webGlShader == nullptr) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "WebGL attach shader invalid shader");
        return NVal::CreateNull(env).val_;
    }
    uint32_t shaderId = webGlShader->GetShaderId();
    uint32_t programId = webGLProgram->GetProgramId();
    LOGD("WebGL attach shader programId %{public}u, shaderId %{public}u", programId, shaderId);
    uint32_t index = 0;
    if (!CheckShaderType(env, webGlShader->GetShaderType(), index) || !webGLProgram->AttachShader(index, shaderId)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "WebGL attach shader invalid shader");
        return NVal::CreateNull(env).val_;
    }
    glAttachShader(static_cast<GLuint>(programId), static_cast<GLuint>(shaderId));
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::DetachShader(napi_env env, napi_value programObj, napi_value shaderObj)
{
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, programObj);
    if (webGLProgram == nullptr) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "webGLProgram is nullptr.");
        return NVal::CreateNull(env).val_;
    }
    WebGLShader* webGlShader = WebGLShader::GetObjectInstance(env, shaderObj);
    if (webGlShader == nullptr) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "webGlShader is nullptr.");
        return NVal::CreateNull(env).val_;
    }
    uint32_t shaderId = webGlShader->GetShaderId();
    uint32_t programId = webGLProgram->GetProgramId();
    LOGD("WebGL detach shader programId %{public}u, shaderId %{public}u", programId, shaderId);
    uint32_t index = 0;
    if (!CheckShaderType(env, webGlShader->GetShaderType(), index) || !webGLProgram->DetachShader(index, shaderId)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "Invalid shader");
        return NVal::CreateNull(env).val_;
    }
    glDetachShader(static_cast<GLuint>(programId), static_cast<GLuint>(shaderId));
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::CompileShader(napi_env env, napi_value object)
{
    WebGLShader* webGlShader = WebGLShader::GetObjectInstance(env, object);
    if (webGlShader == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }
    uint32_t shaderId = webGlShader->GetShaderId();
    glCompileShader(static_cast<GLuint>(shaderId));
    LOGD("WebGL compile shader shaderId %{public}u result %{public}u", shaderId, GetError_());
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::ShaderSource(napi_env env, napi_value object, const std::string& source)
{
    WebGLShader* webGlShader = WebGLShader::GetObjectInstance(env, object);
    if (webGlShader == nullptr) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "webGlShader is nullptr.");
        return NVal::CreateNull(env).val_;
    }
    uint32_t shaderId = webGlShader->GetShaderId();
    if (!WebGLArg::CheckString(source)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "checkString failed.");
        return NVal::CreateNull(env).val_;
    }
    GLint length = static_cast<GLint>(source.size());
    GLchar* str = const_cast<GLchar*>(source.c_str());
    glShaderSource(static_cast<GLuint>(shaderId), 1, &str, &length);
    webGlShader->SetShaderRes(source);
    LOGD("WebGL shaderSource shaderId %{public}u source: '%{public}s' result %{public}u",
        shaderId, source.c_str(), GetError_());
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::DeleteBuffer(napi_env env, napi_value object)
{
    LOGD("WebGL deleteBuffer ");
    WebGLBuffer* webGlBuffer = GetValidBuffer(env, object);
    if (webGlBuffer == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    GLuint buffer = webGlBuffer->GetBufferId();
    DoObjectDelete(WebGLBuffer::objectType, webGlBuffer);
    DeleteObject<WebGLBuffer>(env, buffer);
    glDeleteBuffers(1, &buffer);
    LOGD("WebGL deleteBuffer bufferId %{public}u %{public}p result %{public}u", buffer, webGlBuffer, GetError_());
    for (uint32_t i = 0; i < BoundBufferType::BUFFER_MAX; i++) {
        if (boundBufferIds_[i] == buffer) {
            boundBufferIds_[i] = 0;
        }
    }
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::DeleteFrameBuffer(napi_env env, napi_value object)
{
    WebGLFramebuffer* webGlFramebuffer = WebGLObject::GetObjectInstance<WebGLFramebuffer>(env, object);
    if (webGlFramebuffer == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    GLuint frameBufferId = webGlFramebuffer->GetFramebuffer();
    DeleteObject<WebGLFramebuffer>(env, frameBufferId);
    glDeleteFramebuffers(1, &frameBufferId);
    GLenum target = 0;
    if (!IsHighWebGL()) {
        if (boundFrameBufferIds_[BoundFrameBufferType::FRAMEBUFFER] == frameBufferId) {
            boundFrameBufferIds_[BoundFrameBufferType::FRAMEBUFFER] = 0;
        }
        target = webGlFramebuffer->GetTarget();
    } else {
        if (frameBufferId == boundFrameBufferIds_[BoundFrameBufferType::FRAMEBUFFER]) {
            if (frameBufferId == boundFrameBufferIds_[BoundFrameBufferType::READ_FRAMEBUFFER]) {
                target = GL_FRAMEBUFFER;
                boundFrameBufferIds_[BoundFrameBufferType::FRAMEBUFFER] = 0;
                boundFrameBufferIds_[BoundFrameBufferType::READ_FRAMEBUFFER] = 0;
            } else {
                target = GL_DRAW_FRAMEBUFFER;
                boundFrameBufferIds_[BoundFrameBufferType::FRAMEBUFFER] = 0;
            }
        } else if (frameBufferId == boundFrameBufferIds_[BoundFrameBufferType::READ_FRAMEBUFFER]) {
            target = GL_READ_FRAMEBUFFER;
            boundFrameBufferIds_[BoundFrameBufferType::READ_FRAMEBUFFER] = 0;
        }
    }
    if (target) {
        glBindFramebuffer(target, 0);
    }
    LOGD("WebGL deleteFramebuffer target %{public}u framebufferId %{public}u result %{public}u",
        target, frameBufferId, GetError_());
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::GetBufferParameter(napi_env env, GLenum target, GLenum pname)
{
    LOGD("WebGL getBufferParameter target %{public}u %{public}u", target, pname);
    uint32_t index = 0;
    if (!CheckBufferTarget(env, target, index)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "CheckBufferTarget failed.");
        return NVal::CreateNull(env).val_;
    }
    switch (pname) {
        case WebGLRenderingContextBase::BUFFER_USAGE:
        case WebGLRenderingContextBase::BUFFER_SIZE: {
            GLint value = 0;
            glGetBufferParameteriv(target, pname, &value);
            return NVal::CreateInt64(env, value).val_;
        }
        default:
            SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
            return NVal::CreateNull(env).val_;
    }
}

napi_value WebGLRenderingContextBaseImpl::DeleteProgram(napi_env env, napi_value object)
{
    WebGLProgram* webGlProgram = WebGLObject::GetObjectInstance<WebGLProgram>(env, object);
    if (webGlProgram == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    uint32_t programId = webGlProgram->GetProgramId();
    DeleteObject<WebGLProgram>(env, programId);
    glDeleteProgram(static_cast<GLuint>(programId));
    if (currentProgramId_ == programId) {
        currentProgramId_ = 0;
    } else if (currentProgramId_ != 0) {
        LOGD("WebGL deleteProgram current programId %{public}u", currentProgramId_);
    }
    LOGD("WebGL deleteProgram programId %{public}u result %{public}u", programId, GetError_());
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::DeleteRenderBuffer(napi_env env, napi_value object)
{
    WebGLRenderbuffer* webGlRenderbuffer = WebGLObject::GetObjectInstance<WebGLRenderbuffer>(env, object);
    if (webGlRenderbuffer == nullptr) {
        return NVal::CreateNull(env).val_;
    }

    uint32_t renderbuffer = webGlRenderbuffer->GetRenderbuffer();
    WebGLFramebuffer* framebufferBinding = GetBoundFrameBuffer(env, WebGLRenderingContextBase::FRAMEBUFFER);
    if (framebufferBinding) {
        framebufferBinding->RemoveAttachment(GL_FRAMEBUFFER, renderbuffer, AttachmentType::RENDER_BUFFER);
    }
    framebufferBinding = GetBoundFrameBuffer(env, WebGL2RenderingContextBase::READ_FRAMEBUFFER);
    if (framebufferBinding != nullptr) {
        framebufferBinding->RemoveAttachment(GL_READ_FRAMEBUFFER, renderbuffer, AttachmentType::RENDER_BUFFER);
    }
    DeleteObject<WebGLRenderbuffer>(env, renderbuffer);
    glDeleteRenderbuffers(1, &renderbuffer);
    LOGD("WebGL deleteRenderbuffer renderbuffer %{public}u", renderbuffer);
    uint32_t index = 0;
    if (!CheckRenderBufferTarget(env, webGlRenderbuffer->GetTarget(), index)) {
        return NVal::CreateNull(env).val_;
    }
    boundRenderBufferIds_[index] = 0;
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::GetRenderBufferParameter(napi_env env, GLenum target, GLenum pname)
{
    uint32_t index = 0;
    LOGD("WebGL getRenderbufferParameter target %{public}u %{public}u", target, pname);
    if (!CheckRenderBufferTarget(env, target, index)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "CheckRenderBufferTarget failed.");
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderbuffer* renderBuffer = GetObjectInstance<WebGLRenderbuffer>(env, boundRenderBufferIds_[index]);
    if (renderBuffer == nullptr) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION,
            "WebGL getRenderbufferParameter can not get bound render buffer Id %{public}u",
            boundRenderBufferIds_[index]);
        return NVal::CreateNull(env).val_;
    }

    GLint params = 0;
    switch (pname) {
        case WebGL2RenderingContextBase::RENDERBUFFER_SAMPLES: {
            if (!IsHighWebGL()) {
                SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
                return NVal::CreateNull(env).val_;
            }
            [[fallthrough]];
        }
        case WebGLRenderingContextBase::RENDERBUFFER_WIDTH:
        case WebGLRenderingContextBase::RENDERBUFFER_HEIGHT:
        case WebGLRenderingContextBase::RENDERBUFFER_RED_SIZE:
        case WebGLRenderingContextBase::RENDERBUFFER_GREEN_SIZE:
        case WebGLRenderingContextBase::RENDERBUFFER_BLUE_SIZE:
        case WebGLRenderingContextBase::RENDERBUFFER_ALPHA_SIZE:
        case WebGLRenderingContextBase::RENDERBUFFER_DEPTH_SIZE:
        case WebGLRenderingContextBase::RENDERBUFFER_STENCIL_SIZE: {
            glGetRenderbufferParameteriv(target, pname, &params);
            return NVal::CreateInt64(env, static_cast<int64_t>(params)).val_;
        }
        case WebGLRenderingContextBase::RENDERBUFFER_INTERNAL_FORMAT:
            return NVal::CreateInt64(env, static_cast<int64_t>(renderBuffer->GetInternalFormat())).val_;
        default: {
            SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "invalud pname %{public}u", pname);
            return NVal::CreateNull(env).val_;
        }
    }
}

napi_value WebGLRenderingContextBaseImpl::DeleteShader(napi_env env, napi_value object)
{
    uint32_t shaderId = WebGLShader::DEFAULT_SHADER_ID;
    WebGLShader* webGlShader = WebGLObject::GetObjectInstance<WebGLShader>(env, object);
    if (webGlShader == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    shaderId = webGlShader->GetShaderId();
    DeleteObject<WebGLShader>(env, shaderId);
    glDeleteShader(static_cast<GLuint>(shaderId));
    LOGD("WebGL deleteShader shaderId %{public}u", shaderId);
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::GetShaderParameter(napi_env env, napi_value object, GLenum pname)
{
    WebGLShader* webGlShader = WebGLShader::GetObjectInstance(env, object);
    if (webGlShader == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }
    uint32_t shaderId = webGlShader->GetShaderId();
    LOGD("WebGL getShaderParameter shaderId %{public}u", shaderId);

    GLint params = 0;
    switch (pname) {
        case WebGLRenderingContextBase::SHADER_TYPE: {
            glGetShaderiv(static_cast<GLuint>(shaderId), pname, &params);
            int64_t res = static_cast<int64_t>(params);
            return NVal::CreateInt64(env, res).val_;
        }
        case WebGLRenderingContextBase::DELETE_STATUS:
        case WebGLRenderingContextBase::COMPILE_STATUS: {
            glGetShaderiv(static_cast<GLuint>(shaderId), pname, &params);
            bool res = (params == GL_FALSE) ? false : true;
            return NVal::CreateBool(env, res).val_;
        }
        default:
            break;
    }
    SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::GetShaderPrecisionFormat(
    napi_env env, GLenum shaderType, GLenum precisionType)
{
    LOGD("WebGL getShaderPrecisionFormat shaderType %{public}u precisionType %{public}u", shaderType, precisionType);
    uint32_t index = 0;
    if (!CheckShaderType(env, shaderType, index)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "CheckShaderType failed.");
        return NVal::CreateNull(env).val_;
    }
    if (!CheckGLenum(precisionType,
            { WebGLRenderingContextBase::LOW_FLOAT, WebGLRenderingContextBase::MEDIUM_FLOAT,
            WebGLRenderingContextBase::HIGH_FLOAT, WebGLRenderingContextBase::LOW_INT,
            WebGLRenderingContextBase::MEDIUM_INT, WebGLRenderingContextBase::HIGH_INT }, {})) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "CheckGLenum failed.");
        return NVal::CreateNull(env).val_;
    }
    WebGLShaderPrecisionFormat* webGLShaderPrecisionFormat = nullptr;
    napi_value objShaderPrecisionFormat =
        WebGLShaderPrecisionFormat::CreateObjectInstance(env, &webGLShaderPrecisionFormat).val_;
    if (objShaderPrecisionFormat == nullptr) {
        LOGE("WebGL getShaderPrecisionFormat fail oshaderType %{public}u", shaderType);
        return NVal::CreateNull(env).val_;
    }

    GLint range[2] = { 0 }; // 2 is glGetShaderPrecisionFormat default range array pointer.
    GLint precision = 0;
    glGetShaderPrecisionFormat(shaderType, precisionType, range, &precision);
    LOGD("WebGL getShaderPrecisionFormat [%{public}d %{public}d] precision %{public}d", range[0], range[1], precision);
    webGLShaderPrecisionFormat->SetShaderPrecisionFormatRangeMin(range[0]);
    webGLShaderPrecisionFormat->SetShaderPrecisionFormatRangeMax(range[1]);
    webGLShaderPrecisionFormat->SetShaderPrecisionFormatPrecision(precision);
    return objShaderPrecisionFormat;
}

napi_value WebGLRenderingContextBaseImpl::DeleteTexture(napi_env env, napi_value object)
{
    WebGLTexture* webGlTexture = WebGLObject::GetObjectInstance<WebGLTexture>(env, object);
    if (webGlTexture == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    uint32_t texture = webGlTexture->GetTexture();
    LOGD("WebGL deleteTexture textureId %{public}u", texture);
    WebGLFramebuffer* framebufferBinding = GetBoundFrameBuffer(env, WebGLRenderingContextBase::FRAMEBUFFER);
    if (framebufferBinding) {
        framebufferBinding->RemoveAttachment(GL_FRAMEBUFFER, texture, AttachmentType::TEXTURE);
    }
    framebufferBinding = GetBoundFrameBuffer(env, WebGL2RenderingContextBase::READ_FRAMEBUFFER);
    if (framebufferBinding != nullptr) {
        framebufferBinding->RemoveAttachment(GL_READ_FRAMEBUFFER, texture, AttachmentType::TEXTURE);
    }
    uint32_t index = 0;
    if (webGlTexture->GetTarget() &&
        !WebGLRenderingContextBaseImpl::CheckTextureTarget(env, webGlTexture->GetTarget(), index)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    for (size_t i = 0; i < boundTexture_[index].size(); i++) {
        if (boundTexture_[index][i] == texture) {
            boundTexture_[index][i] = 0;
        }
    }
    DeleteObject<WebGLTexture>(env, texture);
    glDeleteTextures(1, &texture);
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::HandleFrameBufferAttachmentPname(
    napi_env env, GLenum target, GLenum attachment, GLenum pname, GLint type)
{
    GLint params = 0;
    switch (pname) {
        case GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE:
            return NVal::CreateInt64(env, static_cast<int64_t>(type)).val_;
        case GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME: {
            glGetFramebufferAttachmentParameteriv(target, attachment, pname, &params);
            LOGD("WebGL getFramebufferAttachmentParameter objectId %{public}d", params);
            return (type == WebGLRenderingContextBase::RENDERBUFFER) ? GetObject<WebGLRenderbuffer>(env, params) :
                GetObject<WebGLTexture>(env, params);
        }
        case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER: {
            if (!IsHighWebGL()) {
                break;
            }
            [[fallthrough]];
        }
        case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL:
        case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE: {
            if (type == WebGLRenderingContextBase::RENDERBUFFER) {
                break;
            }
            glGetFramebufferAttachmentParameteriv(target, attachment, pname, &params);
            return NVal::CreateInt64(env, static_cast<int64_t>(params)).val_;
        }
        case GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE: {
            if (attachment == GL_DEPTH_STENCIL_ATTACHMENT) {
                SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
                return NVal::CreateNull(env).val_;
            }
            [[fallthrough]];
        }
        case GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE:
        case GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE:
        case GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE:
        case GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE:
        case GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE:
        case GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE:
        case GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING: {
            if (!IsHighWebGL()) {
                break;
            }
            glGetFramebufferAttachmentParameteriv(target, attachment, pname, &params);
            return NVal::CreateInt64(env, static_cast<int64_t>(params)).val_;
        }
        default:
            break;
    }
    SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
    LOGE("WebGL getFramebufferAttachmentParameter : no image is attached pname %{public}u %{public}d", pname, type);
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::GetFrameBufferAttachmentParameter(
    napi_env env, GLenum target, GLenum attachment, GLenum pname)
{
    uint32_t index = 0;
    if (!CheckFrameBufferTarget(env, target, index)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "getFramebufferAttachmentParameter Invalid target");
        return NVal::CreateNull(env).val_;
    }
    if (!CheckAttachment(env, attachment)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "CheckAttachment failed.");
        return NVal::CreateNull(env).val_;
    }

    WebGLFramebuffer* frameBuffer = GetBoundFrameBuffer(env, WebGLRenderingContextBase::FRAMEBUFFER);
    if (frameBuffer == nullptr) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "frameBuffer is nullptr");
        return NVal::CreateNull(env).val_;
    }
    GLint type = 0;
    glGetFramebufferAttachmentParameteriv(target, attachment, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &type);
    LOGD("WebGL getFramebufferAttachmentParameter target %{public}u %{public}u %{public}u %{public}d",
        target, attachment, pname, type);
    if ((type != WebGLRenderingContextBase::RENDERBUFFER) && (type != WebGLRenderingContextBase::TEXTURE)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }

    return HandleFrameBufferAttachmentPname(env, target, attachment, pname, type);
}

napi_value WebGLRenderingContextBaseImpl::FrameBufferRenderBuffer(
    napi_env env, GLenum target, GLenum attachment, GLenum renderBufferTarget, napi_value object)
{
    LOGD("WebGL framebufferRenderbuffer target %{public}u %{public}u %{public}u",
        target, attachment, renderBufferTarget);
    uint32_t frameBufferTargetIdx = 0;
    if (!CheckFrameBufferTarget(env, target, frameBufferTargetIdx)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM,
            "WebGL framebufferRenderbuffer invalid target %{public}u", target);
        return NVal::CreateNull(env).val_;
    }
    if (!CheckAttachment(env, attachment)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM,
            "WebGL framebufferRenderbuffer invalid attachment %{public}u", attachment);
        return NVal::CreateNull(env).val_;
    }
    uint32_t renderBufferTargetIdx = 0;
    if (!CheckRenderBufferTarget(env, renderBufferTarget, renderBufferTargetIdx)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM,
            "WebGL framebufferRenderbuffer invalid renderBufferTarget %{public}u", renderBufferTarget);
        return NVal::CreateNull(env).val_;
    }
    WebGLFramebuffer* frameBuffer =
        GetObjectInstance<WebGLFramebuffer>(env, boundFrameBufferIds_[frameBufferTargetIdx]);
    if (frameBuffer == nullptr) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION,
            "WebGL framebufferRenderbuffer can not find bind frame buffer");
        return NVal::CreateNull(env).val_;
    }

    GLuint renderbuffer = WebGLRenderbuffer::DEFAULT_RENDER_BUFFER;
    WebGLRenderbuffer* webGlRenderbuffer = GetValidRenderBuffer(env, object);
    if (webGlRenderbuffer != nullptr) {
        renderbuffer = webGlRenderbuffer->GetRenderbuffer();
    }

    if (attachment == WebGLRenderingContextBase::DEPTH_STENCIL_ATTACHMENT) {
        if (IsHighWebGL()) {
            frameBuffer->AddAttachment(target, WebGLRenderingContextBase::DEPTH_ATTACHMENT, renderbuffer);
            frameBuffer->AddAttachment(target, WebGLRenderingContextBase::STENCIL_ATTACHMENT, renderbuffer);
        } else {
            frameBuffer->AddAttachment(target, attachment, renderbuffer);
        }
        glFramebufferRenderbuffer(target, GL_DEPTH_ATTACHMENT, renderBufferTarget, renderbuffer);
        glFramebufferRenderbuffer(target, GL_STENCIL_ATTACHMENT, renderBufferTarget, renderbuffer);
    } else {
        frameBuffer->AddAttachment(target, attachment, renderbuffer);
        glFramebufferRenderbuffer(target, attachment, renderBufferTarget, renderbuffer);
    }
    LOGD("WebGL framebufferRenderbuffer renderbuffer %{public}u error %{public}u", renderbuffer, GetError_());
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::FrameBufferTexture2D(
    napi_env env, GLenum target, GLenum attachment, GLenum textureTarget, napi_value texture, GLint level)
{
    LOGD("WebGL framebufferTexture2D target %{public}u %{public}u %{public}u", target, attachment, textureTarget);
    if (!CheckAttachment(env, attachment)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "invalid attachment");
        return NVal::CreateNull(env).val_;
    }
    if (IsHighWebGL()) {
        if (CheckTextureLevel(textureTarget, level)) {
            SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "Invalid texture level");
            return NVal::CreateNull(env).val_;
        }
    } else if (level) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "Invalid texture level");
        return NVal::CreateNull(env).val_;
    }
    if (!CheckTexture2DTarget(env, textureTarget)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "invalid textureTarget");
        return NVal::CreateNull(env).val_;
    }
    WebGLFramebuffer* frameBuffer = GetBoundFrameBuffer(env, target);
    if (frameBuffer == nullptr) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "can not find bind frame buffer");
        return NVal::CreateNull(env).val_;
    }

    int32_t textureId = WebGLTexture::DEFAULT_TEXTURE;
    WebGLTexture* webGlTexture = WebGLTexture::GetObjectInstance(env, texture);
    if (webGlTexture != nullptr) {
        textureId = static_cast<int32_t>(webGlTexture->GetTexture());
    }
    frameBuffer->AddAttachment(target, attachment, static_cast<GLuint>(textureId), textureTarget, level);

    if (attachment == GL_DEPTH_STENCIL_ATTACHMENT) {
        glFramebufferTexture2D(target, GL_DEPTH_ATTACHMENT, textureTarget, static_cast<GLuint>(textureId), level);
        glFramebufferTexture2D(target, GL_STENCIL_ATTACHMENT, textureTarget, static_cast<GLuint>(textureId), level);
    } else {
        glFramebufferTexture2D(target, attachment, textureTarget, static_cast<GLuint>(textureId), level);
    }
    LOGD("WebGL framebufferTexture2D texture %{public}u result %{public}u", textureId, GetError_());
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::GetProgramParameter(napi_env env, napi_value object, GLenum pname)
{
    uint32_t programId = WebGLProgram::DEFAULT_PROGRAM_ID;
    WebGLProgram* webGlProgram = WebGLObject::GetObjectInstance<WebGLProgram>(env, object);
    if (webGlProgram == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }
    programId = webGlProgram->GetProgramId();
    LOGD("WebGL getProgramParameter programId %{public}u pname %{public}u", programId, pname);
    switch (pname) {
        case WebGLRenderingContextBase::DELETE_STATUS:
        case WebGLRenderingContextBase::LINK_STATUS:
        case WebGLRenderingContextBase::VALIDATE_STATUS: {
            GLint params = 0;
            glGetProgramiv(static_cast<GLuint>(programId), pname, &params);
            LOGD("WebGL getProgramParameter params %{public}d", params);
            return NVal::CreateBool(env, params ? true : false).val_;
        }
        case WebGLRenderingContextBase::ATTACHED_SHADERS:
        case WebGLRenderingContextBase::ACTIVE_ATTRIBUTES:
        case WebGLRenderingContextBase::ACTIVE_UNIFORMS: {
            GLint params = 0;
            glGetProgramiv(static_cast<GLuint>(programId), pname, &params);
            LOGD("WebGL getProgramParameter params %{public}d", params);
            return NVal::CreateInt64(env, static_cast<int64_t>(params)).val_;
        }
        case WebGL2RenderingContextBase::ACTIVE_UNIFORM_BLOCKS:
        case WebGL2RenderingContextBase::TRANSFORM_FEEDBACK_VARYINGS:
        case WebGL2RenderingContextBase::TRANSFORM_FEEDBACK_BUFFER_MODE: {
            if (!IsHighWebGL()) {
                SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
                return NVal::CreateNull(env).val_;
            }
            GLint params = 0;
            glGetProgramiv(static_cast<GLuint>(programId), pname, &params);
            LOGD("WebGL getProgramParameter params %{public}d", params);
            return NVal::CreateInt64(env, static_cast<int64_t>(params)).val_;
        }
        default:
            SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "invalid pname %{public}u", pname);
            break;
    }
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::GetAttachedShaders(napi_env env, napi_value object)
{
    uint32_t programId = WebGLProgram::DEFAULT_PROGRAM_ID;
    WebGLProgram* webGlProgram = WebGLObject::GetObjectInstance<WebGLProgram>(env, object);
    if (webGlProgram != nullptr) {
        programId = webGlProgram->GetProgramId();
    }

    GLsizei count = 1;
    std::vector<GLuint> shaderId(MAX_COUNT_ATTACHED_SHADER);
    glGetAttachedShaders(
        static_cast<GLuint>(programId), MAX_COUNT_ATTACHED_SHADER, &count, static_cast<GLuint*>(shaderId.data()));
    LOGD("WebGL getAttachedShaders programId %{public}u count %{public}d", programId, count);
    napi_value shaderArr = nullptr;
    napi_create_array(env, &shaderArr);
    for (GLsizei i = 0; i < count; i++) {
        napi_value objShader = GetObject<WebGLShader>(env, shaderId[i]);
        (void)napi_set_element(env, shaderArr, i, objShader);
    }
    return shaderArr;
}

napi_value WebGLRenderingContextBaseImpl::GetUniformLocation(napi_env env, napi_value object, const std::string& name)
{
    WebGLProgram* webGlProgram = WebGLObject::GetObjectInstance<WebGLProgram>(env, object);
    if (webGlProgram == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    GLuint programId = webGlProgram->GetProgramId();
    if (!CheckLocationName(name)) {
        return NVal::CreateNull(env).val_;
    }

    GLint locationId = glGetUniformLocation(programId, name.c_str());
    LOGD("WebGL getUniformLocation locationId programId %{public}u name '%{public}s' %{public}d",
        programId, name.c_str(), locationId);
    if (locationId == -1) {
        return NVal::CreateNull(env).val_;
    }
    // check if exit
    WebGLUniformLocation* webGLUniformLocation = GetObjectInstance<WebGLUniformLocation>(env, locationId);
    napi_value objUniformLocation = GetNapiValue<WebGLUniformLocation>(env, locationId);
    if (webGLUniformLocation == nullptr) { // create new
        objUniformLocation = WebGLUniformLocation::CreateObjectInstance(env, &webGLUniformLocation).val_;
        if (!objUniformLocation) {
            return NVal::CreateNull(env).val_;
        }
        webGLUniformLocation->SetUniformLocationId(locationId);
        AddObject<WebGLUniformLocation>(env, locationId, objUniformLocation);
    } else {
        webGLUniformLocation->SetUniformLocationName(name);
    }
    return objUniformLocation;
}

napi_value WebGLRenderingContextBaseImpl::GetAttribLocation(napi_env env, napi_value object, const std::string& name)
{
    WebGLProgram* webGlProgram = WebGLObject::GetObjectInstance<WebGLProgram>(env, object);
    if (webGlProgram == nullptr) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "webGlProgram is nullptr.");
        return NVal::CreateInt64(env, -1).val_;
    }

    if (!CheckLocationName(name)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "CheckLocationName failed.");
        return NVal::CreateInt64(env, -1).val_;
    }

    if (!CheckProgramLinkStatus(webGlProgram)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "CheckProgramLinkStatus failed.");
        return NVal::CreateInt64(env, -1).val_;
    }
    GLuint programId = webGlProgram->GetProgramId();
    GLint returnValue = glGetAttribLocation(programId, const_cast<char*>(name.c_str()));
    LOGD("WebGL getAttribLocation programId %{public}u name %{public}s, location %{public}d",
        programId, name.c_str(), returnValue);
    return NVal::CreateInt64(env, static_cast<int64_t>(returnValue)).val_;
}

napi_value WebGLRenderingContextBaseImpl::GetVertexAttrib(napi_env env, GLenum pname, GLuint index,
    VertexAttribInfo* info)
{
    if (info == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    WebGLWriteBufferArg writeBuffer(env);
    void* result = reinterpret_cast<void*>(writeBuffer.AllocBuffer(4 * sizeof(GLfloat))); // 4 args
    if (result == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    switch (info->type) {
        case BUFFER_DATA_FLOAT_32:
            glGetVertexAttribfv(index, pname, reinterpret_cast<GLfloat*>(result));
            return writeBuffer.ToNormalArray(BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32);
        case BUFFER_DATA_INT_32:
            glGetVertexAttribIuiv(index, pname, reinterpret_cast<GLuint*>(result));
            return writeBuffer.ToNormalArray(BUFFER_DATA_INT_32, BUFFER_DATA_INT_32);
        case BUFFER_DATA_UINT_32:
            glGetVertexAttribIuiv(index, pname, reinterpret_cast<GLuint*>(result));
            return writeBuffer.ToNormalArray(BUFFER_DATA_UINT_32, BUFFER_DATA_UINT_32);
        default:
            break;
    }
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::GetVertexAttrib(napi_env env, GLenum pname, GLuint index)
{
    LOGD("WebGL getVertexAttrib index %{public}u %{public}u", index, pname);
    VertexAttribInfo* info = GetVertexAttribInfo(index);
    if (info == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }
    int32_t params = -1;
    switch (pname) {
        case GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING: {
            glGetVertexAttribiv(index, pname, &params);
            LOGD("WebGL getVertexAttrib %{public}d", params);
            return GetObject<WebGLBuffer>(env, params);
        }
        case GL_VERTEX_ATTRIB_ARRAY_ENABLED:
        case GL_VERTEX_ATTRIB_ARRAY_NORMALIZED:
        case GL_VERTEX_ATTRIB_ARRAY_INTEGER:
            glGetVertexAttribiv(index, pname, &params);
            return NVal::CreateBool(env, static_cast<bool>(params)).val_;
        case GL_VERTEX_ATTRIB_ARRAY_SIZE:
        case GL_VERTEX_ATTRIB_ARRAY_STRIDE:
        case GL_VERTEX_ATTRIB_ARRAY_DIVISOR_ANGLE:
        case GL_VERTEX_ATTRIB_ARRAY_TYPE: {
            glGetVertexAttribiv(index, pname, &params);
            LOGD("WebGL getVertexAttrib %{public}d", params);
            return NVal::CreateInt64(env, params).val_;
        }
        case GL_CURRENT_VERTEX_ATTRIB:
            break;
        default:
            glGetVertexAttribiv(index, pname, &params);
            LOGD("WebGL getVertexAttrib getVertexAttrib end");
            return NVal::CreateNull(env).val_;
    }
    return GetVertexAttrib(env, pname, index, info);
}

static napi_value GetExtensionsParameter(napi_env env, GLenum pname)
{
    const unsigned char* extensions = glGetString(pname);
    if (extensions == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    std::string str = const_cast<char*>(reinterpret_cast<const char*>(extensions));
    vector<std::string> vec {};
    Util::SplitString(str, vec, " ");
    LOGD("WebGL GetExtensionsParameter  %{public}s %{public}s %{public}zu", extensions, str.c_str(), vec.size());
    napi_value result = nullptr;
    napi_create_array_with_length(env, vec.size(), &result);
    for (vector<std::string>::size_type i = 0; i != vec.size(); ++i) {
        napi_set_element(env, result, i, NVal::CreateUTF8String(env, vec[i]).val_);
    }
    return result;
}

template<class T>
napi_value WebGLRenderingContextBaseImpl::GetObjectParameter(napi_env env, GLenum pname)
{
    GLint params = 0;
    glGetIntegerv(pname, &params);
    LOGD("WebGL getParameter %{public}s params %{public}d", T::className.c_str(), params);
    return GetObject<T>(env, params);
}

template<class T>
napi_value WebGLRenderingContextBaseImpl::GetIntegerVectorParameter(
    napi_env env, GLenum pname, GLuint count, BufferDataType dstDataType)
{
    LOGD("get integer parameter pname %{public}u count %{public}u", pname, count);
    WebGLWriteBufferArg writeBuffer(env);
    T* params = reinterpret_cast<T*>(writeBuffer.AllocBuffer(count * sizeof(T))); // 2 args
    if (params == nullptr) {
        return writeBuffer.ToExternalArray(dstDataType);
    }
    glGetIntegerv(pname, params);
    return writeBuffer.ToExternalArray(dstDataType);
}

napi_value WebGLRenderingContextBaseImpl::GetFloatVectorParameter(
    napi_env env, GLenum pname, GLuint count, BufferDataType dstDataType)
{
    LOGD("get float parameter pname %{public}u count %{public}u", pname, count);
    WebGLWriteBufferArg writeBuffer(env);
    GLfloat* params = reinterpret_cast<GLfloat*>(writeBuffer.AllocBuffer(count * sizeof(GLfloat)));
    if (params == nullptr) {
        return writeBuffer.ToExternalArray(dstDataType);
    }
    glGetFloatv(pname, params);
    return writeBuffer.ToExternalArray(dstDataType);
}

napi_value WebGLRenderingContextBaseImpl::GetBoolVectorParameter(
    napi_env env, GLenum pname, GLuint count, BufferDataType dstDataType)
{
    WebGLWriteBufferArg writeBuffer(env);
    GLboolean* params = reinterpret_cast<GLboolean*>(writeBuffer.AllocBuffer(count * sizeof(GLboolean)));
    if (params == nullptr) {
        return writeBuffer.ToExternalArray(dstDataType);
    }
    glGetBooleanv(pname, static_cast<GLboolean*>(params));
    return writeBuffer.ToNormalArray(BUFFER_DATA_BOOLEAN, dstDataType);
}

napi_value WebGLRenderingContextBaseImpl::GetParameter(napi_env env, GLenum pname)
{
    LOGD("WebGL getParameter pname %{public}u", pname);
    switch (pname) {
        case GL_DEPTH_CLEAR_VALUE:
        case GL_LINE_WIDTH:
        case GL_POLYGON_OFFSET_FACTOR:
        case GL_POLYGON_OFFSET_UNITS:
        case GL_SAMPLE_COVERAGE_VALUE:
            return WebGLArg::GetFloatParameter(env, pname);
        case GL_TEXTURE_BINDING_2D:
        case GL_TEXTURE_BINDING_CUBE_MAP:
            return GetObjectParameter<WebGLTexture>(env, pname);
        case GL_RENDERER:
        case GL_SHADING_LANGUAGE_VERSION:
        case GL_VENDOR:
        case GL_VERSION:
            return GetExtensionsParameter(env, pname);
        case GL_ARRAY_BUFFER_BINDING:
        case GL_ELEMENT_ARRAY_BUFFER_BINDING:
            return GetObjectParameter<WebGLBuffer>(env, pname);
        case GL_FRAMEBUFFER_BINDING:
            return GetObjectParameter<WebGLFramebuffer>(env, pname);
        case GL_CURRENT_PROGRAM:
            return GetObjectParameter<WebGLProgram>(env, pname);
        case GL_RENDERBUFFER_BINDING:
            return GetObjectParameter<WebGLRenderbuffer>(env, pname);
        case GL_ALIASED_LINE_WIDTH_RANGE:
        case GL_ALIASED_POINT_SIZE_RANGE:
        case GL_DEPTH_RANGE:
            return GetFloatVectorParameter(env, pname, 2, BUFFER_DATA_FLOAT_32); //  2 args
        case GL_COLOR_CLEAR_VALUE:
        case GL_BLEND_COLOR:
            return GetFloatVectorParameter(env, pname, 4, BUFFER_DATA_FLOAT_32); //  4 args
        case GL_COMPRESSED_TEXTURE_FORMATS: {
            GLint count = 0;
            glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &count);
            return GetIntegerVectorParameter<GLint>(env, pname, count, BUFFER_DATA_INT_32);
        }
        case GL_MAX_VIEWPORT_DIMS:
            return GetIntegerVectorParameter<GLint>(env, pname, 2, BUFFER_DATA_INT_32); // 2 args
        case GL_SCISSOR_BOX:
        case WebGLRenderingContextBase::VIEWPORT:
            return GetIntegerVectorParameter<GLint>(env, pname, 4, BUFFER_DATA_INT_32); // 4 args
        case GL_COLOR_WRITEMASK:
            return GetBoolVectorParameter(env, pname, 4, BUFFER_DATA_BOOLEAN); // 4 args
        case WebGLRenderingContextBase::UNPACK_COLORSPACE_CONVERSION_WEBGL:
            return NVal::CreateInt64(env, unpackColorspaceConversion_).val_;
        case WebGLRenderingContextBase::UNPACK_FLIP_Y_WEBGL:
            return NVal::CreateBool(env, unpackFlipY_).val_;
        case WebGLRenderingContextBase::UNPACK_PREMULTIPLY_ALPHA_WEBGL:
            return NVal::CreateBool(env, unpackPremultiplyAlpha_).val_;
        default: {
            if (CheckInList(pname, GetBoolParaName())) {
                return WebGLArg::GetBoolParameter(env, pname);
            }
            if (CheckInList(pname, GetIntegerParaName())) {
                return WebGLArg::GetInt32Parameter(env, pname);
            }
            break;
        }
    }
    SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "WebGL getParameter invalid pname %{public}u", pname);
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::ClearColor(
    napi_env env, GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    LOGD("WebGL clearColor [%{public}f, %{public}f, %{public}f, %{public}f]", red, green, blue, alpha);
    clearColor_[0] = red; // 0:red
    clearColor_[1] = green; // 1:green
    clearColor_[2] = blue; // 2:blue
    clearColor_[3] = alpha; // 3:alpha
    glClearColor(red, green, blue, alpha);
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::ColorMask(
    napi_env env, GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
    colorMask_[0] = red; // 0:red
    colorMask_[1] = green; // 1:green
    colorMask_[2] = blue; // 2:blue
    colorMask_[3] = alpha; // 3:alpha
    glColorMask(static_cast<GLboolean>(red), static_cast<GLboolean>(green), static_cast<GLboolean>(blue),
        static_cast<GLboolean>(alpha));
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::Clear(napi_env env, GLbitfield mask)
{
    if (mask & ~(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE,
            "WebGL clear mask %{public}x failed.", mask);
        return NVal::CreateNull(env).val_;
    }
    GLenum result = CheckFrameBufferBoundComplete(env);
    if (result) {
        SET_ERROR_WITH_LOG(result, "WebGL clear mask %{public}x CheckFrameBufferBoundComplete failed.", mask);
        return NVal::CreateNull(env).val_;
    }
    glClear(mask);
    LOGD("WebGL clear mask %{public}x result %{public}u", mask, GetError_());
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::ClearDepth(napi_env env, GLclampf depth)
{
    LOGD("WebGL clearDepth %{public}f", depth);
    clearDepth_ = depth;
    glClearDepthf(depth);
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::ClearStencil(napi_env env, GLint s)
{
    LOGD("WebGL clearStencil s %{public}d", s);
    clearStencil_ = static_cast<GLint>(s);
    glClearStencil(static_cast<GLint>(s));
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::UseProgram(napi_env env, napi_value object)
{
    uint32_t program = WebGLProgram::DEFAULT_PROGRAM_ID;
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, object);
    if (webGLProgram != nullptr) {
        program = webGLProgram->GetProgramId();
    }
    if ((currentProgramId_ != program) && (currentProgramId_ != 0)) {
        glUseProgram(static_cast<GLuint>(0));
    }
    currentProgramId_ = program;
    glUseProgram(static_cast<GLuint>(program));
    LOGD("WebGL useProgram programId %{public}u %{public}u result %{public}u",
        program, currentProgramId_, GetError_());
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::ValidateProgram(napi_env env, napi_value object)
{
    uint32_t program = WebGLProgram::DEFAULT_PROGRAM_ID;
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, object);
    if (webGLProgram != nullptr) {
        program = webGLProgram->GetProgramId();
    }
    LOGD("WebGL validateProgram program %{public}u", program);
    glValidateProgram(static_cast<GLuint>(program));
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::LinkProgram(napi_env env, napi_value object)
{
    uint32_t program = WebGLProgram::DEFAULT_PROGRAM_ID;
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, object);
    if (webGLProgram != nullptr) {
        program = webGLProgram->GetProgramId();
    }
    glLinkProgram(static_cast<GLuint>(program));
    LOGD("WebGL linkProgram program %{public}u result %{public}u", program, GetError_());
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::RenderBufferStorage(napi_env env, const TexStorageArg& arg)
{
    LOGD("WebGL renderbufferStorage target %{public}u %{public}u %{public}d %{public}d",
        arg.target, arg.internalFormat, arg.width, arg.height);
    WebGLRenderbuffer* renderBuffer = CheckRenderBufferStorage(env, arg);
    if (renderBuffer == nullptr) {
        return NVal::CreateNull(env).val_;
    }

    switch (arg.internalFormat) {
        case WebGLRenderingContextBase::DEPTH_COMPONENT16:
        case WebGLRenderingContextBase::RGBA4:
        case WebGLRenderingContextBase::RGB5_A1:
        case WebGLRenderingContextBase::RGB565:
        case WebGLRenderingContextBase::STENCIL_INDEX8: {
            glRenderbufferStorage(arg.target, arg.internalFormat, arg.width, arg.height);
            renderBuffer->SetInternalFormat(arg.internalFormat);
            renderBuffer->SetSize(arg.width, arg.height);
            break;
        }
        case WebGLRenderingContextBase::DEPTH_STENCIL: {
            glRenderbufferStorage(arg.target, GL_DEPTH_COMPONENT16, arg.width, arg.height);
            glRenderbufferStorage(arg.target, GL_STENCIL_INDEX8, arg.width, arg.height);
            renderBuffer->SetInternalFormat(arg.internalFormat);
            renderBuffer->SetSize(arg.width, arg.height);
            break;
        }
        default:
            SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
            return NVal::CreateNull(env).val_;
    }
    LOGD("WebGL renderbufferStorage result %{public}u", GetError_());
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::VertexAttribPointer(napi_env env, const VertexAttribArg& vertexInfo)
{
    vertexInfo.Dump("WebGL vertexAttribPointer");
    if (!CheckGLenum(vertexInfo.type, { GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_FLOAT }, {})) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE,
            "WebGL vertexAttribPointer invalid type %{public}u", vertexInfo.type);
    }
    GLenum result = CheckVertexAttribPointer(env, vertexInfo);
    if (result) {
        SET_ERROR(result);
        return NVal::CreateNull(env).val_;
    }
    glVertexAttribPointer(vertexInfo.index, vertexInfo.size, vertexInfo.type, vertexInfo.normalized,
        vertexInfo.stride, reinterpret_cast<GLvoid*>(vertexInfo.offset));
    LOGD("WebGL vertexAttribPointer index %{public}u result %{public}u", vertexInfo.index, GetError_());
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::GetVertexAttribOffset(napi_env env, GLuint index, GLenum pname)
{
    LOGD("WebGL getVertexAttribOffset index %{public}u %{public}u", index, pname);
    if (pname != WebGLRenderingContextBase::VERTEX_ATTRIB_ARRAY_POINTER) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateInt64(env, 0).val_;
    }
    GLvoid* point = nullptr;
    glGetVertexAttribPointerv(index, pname, &point);
    return NVal::CreateInt64(env, reinterpret_cast<int64_t>(point)).val_;
}

napi_value WebGLRenderingContextBaseImpl::EnableVertexAttribArray(napi_env env, int64_t index)
{
    if (index < 0 || index >= GetMaxVertexAttribs()) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE,
            "WebGL enableVertexAttribArray invalid index %{public}" PRIi64, index);
        return NVal::CreateNull(env).val_;
    }
    LOGD("WebGL enableVertexAttribArray index %{public}" PRIi64, index);
    glEnableVertexAttribArray(static_cast<GLuint>(index));
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::DisableVertexAttribArray(napi_env env, int64_t index)
{
    if (index < 0 || index >= GetMaxVertexAttribs()) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE,
            "WebGL disableVertexAttribArray invalid index %{public}" PRIi64, index);
        return NVal::CreateNull(env).val_;
    }
    LOGD("WebGL disableVertexAttribArray index %{public}" PRIi64, index);
    glDisableVertexAttribArray(index);
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::IsEnabled(napi_env env, GLenum cap)
{
    bool result = false;
    if (!CheckCap(env, cap)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateBool(env, result).val_;
    }
    result = static_cast<bool>(glIsEnabled(cap));
    LOGD("WebGL isEnabled cap %{public}u result %{public}d", cap, result);
    return NVal::CreateBool(env, result).val_;
}

napi_value WebGLRenderingContextBaseImpl::Disable(napi_env env, GLenum cap)
{
    LOGD("WebGL disable cap %{public}u", cap);
    if (!CheckCap(env, cap)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    glDisable(cap);
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::Enable(napi_env env, GLenum cap)
{
    LOGD("WebGL enable cap %{public}u", cap);
    if (!CheckCap(env, cap)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    if (cap == WebGLRenderingContextBase::STENCIL_TEST) {
        stencilEnabled_ = true;
    }
    if (cap == WebGLRenderingContextBase::SCISSOR_TEST) {
        scissorEnabled_ = true;
    }
    glEnable(cap);
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::BindAttribLocation(
    napi_env env, napi_value program, GLuint index, const std::string& name)
{
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, program);
    if (webGLProgram == nullptr) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "WebGL bindAttribLocation invalid object");
        return NVal::CreateNull(env).val_;
    }
    GLuint programId = webGLProgram->GetProgramId();

    if (index >= GetMaxVertexAttribs()) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE,
            "WebGL bindAttribLocation invalid index %{public}u", index);
        return NVal::CreateNull(env).val_;
    }

    if (!CheckLocationName(name)) {
        return NVal::CreateNull(env).val_;
    }
    LOGD("WebGL bindAttribLocation programId %{public}u index %{public}u name %{public}s",
        programId, index, name.c_str());
    glBindAttribLocation(programId, index, const_cast<char*>(name.c_str()));
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::GenerateMipmap(napi_env env, GLenum target)
{
    if (!CheckGLenum(target,
        { WebGLRenderingContextBase::TEXTURE_2D, WebGLRenderingContextBase::TEXTURE_CUBE_MAP },
        { WebGL2RenderingContextBase::TEXTURE_3D, WebGL2RenderingContextBase::TEXTURE_2D_ARRAY })) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }

    WebGLTexture* webGLTexture = GetBoundTexture(env, target, false);
    if (webGLTexture == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return NVal::CreateNull(env).val_;
    }
    glGenerateMipmap(target);
    LOGD("WebGL generateMipmap target %{public}u %{public}u", target, GetError_());
    return NVal::CreateNull(env).val_;
}

GLenum WebGLRenderingContextBaseImpl::GetError_()
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        lastError_ = err;
    }
    return lastError_;
}

napi_value WebGLRenderingContextBaseImpl::GetError(napi_env env)
{
    GLenum err = GetError_();
    LOGD("getError result %{public}u", err);
    lastError_ = GL_NO_ERROR;
    return NVal::CreateInt64(env, static_cast<int64_t>(err)).val_;
}

napi_value WebGLRenderingContextBaseImpl::UniformF(
    napi_env env, napi_value locationObj, uint32_t count, const GLfloat* data)
{
    WebGLUniformLocation* webGLUniformLocation = WebGLUniformLocation::GetObjectInstance(env, locationObj);
    if (webGLUniformLocation == nullptr) {
        LOGE("WebGL uniform_f can not find uniform");
        return NVal::CreateNull(env).val_;
    }
    GLint location = webGLUniformLocation->GetUniformLocationId();

    // 0,1,2,3 index for data
    LOGD("WebGL uniform location %{public}d [%{public}f %{public}f %{public}f %{public}f]",
        location, data[0], data[1], data[2], data[3]);
    switch (count) {
        case PARAMETERS_NUM_1:
            glUniform1f(location, data[0]);
            break;
        case PARAMETERS_NUM_2:
            glUniform2f(location, data[0], data[1]);
            break;
        case PARAMETERS_NUM_3:
            glUniform3f(location, data[0], data[1], data[2]); // 0,1,2,3 index for data
            break;
        case PARAMETERS_NUM_4:
            glUniform4f(location, data[0], data[1], data[2], data[3]); // 0,1,2,3 index for data
            break;
        default:
            break;;
    }
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::UniformI(
    napi_env env, napi_value locationObj, uint32_t count, const GLint* data)
{
    WebGLUniformLocation* webGLUniformLocation = WebGLUniformLocation::GetObjectInstance(env, locationObj);
    if (webGLUniformLocation == nullptr) {
        LOGE("WebGL uniform_i can not find uniform");
        return NVal::CreateNull(env).val_;
    }
    GLint location = webGLUniformLocation->GetUniformLocationId();
    // 0,1,2,3 index for data
    LOGD("WebGL uniform location %{public}d [%{public}d %{public}d %{public}d %{public}d]",
        location, data[0], data[1], data[2], data[3]);
    switch (count) {
        case PARAMETERS_NUM_1:
            glUniform1i(location, data[0]);
            break;
        case PARAMETERS_NUM_2:
            glUniform2i(location, data[0], data[1]);
            break;
        case PARAMETERS_NUM_3:
            glUniform3i(location, data[0], data[1], data[2]); // 0, 1, 2 are array index
            break;
        case PARAMETERS_NUM_4:
            glUniform4i(location, data[0], data[1], data[2], data[3]); // 0, 1, 2, 3 are array index
            break;
        default:
            break;
    }
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::UniformUi(
    napi_env env, napi_value locationObj, uint32_t count, const GLuint* data)
{
    WebGLUniformLocation* webGLUniformLocation = WebGLUniformLocation::GetObjectInstance(env, locationObj);
    if (webGLUniformLocation == nullptr) {
        LOGE("WebGL uniform_ui can not find uniform");
        return NVal::CreateNull(env).val_;
    }
    GLint location = webGLUniformLocation->GetUniformLocationId();
    LOGD("WebGL uniformXui location %{public}d [%{public}u %{public}u %{public}u %{public}u]",
        location, data[0], data[1], data[2], data[3]); // 0,1,2,3 index for data
    switch (count) {
        case PARAMETERS_NUM_1:
            glUniform1ui(location, data[0]);
            break;
        case PARAMETERS_NUM_2:
            glUniform2ui(location, data[0], data[1]);
            break;
        case PARAMETERS_NUM_3:
            glUniform3ui(location, data[0], data[1], data[2]); // 0, 1, 2 are array index
            break;
        case PARAMETERS_NUM_4:
            glUniform4ui(location, data[0], data[1], data[2], data[3]); // 0, 1, 2, 3 are array index
            break;
        default:
            break;
    }
    LOGD("WebGL uniformXui result %{public}u", GetError_());
    return NVal::CreateNull(env).val_;
}

template<class T>
static std::tuple<GLenum, GLsizei, T*> CheckUniformDataInfo(
    bool isHighWebGL, const WebGLReadBufferArg& readData, const UniformExtInfo* info)
{
    GLuint count = static_cast<GLuint>(readData.GetBufferLength() / sizeof(T));
    T* srcData = reinterpret_cast<T*>(readData.GetBuffer());
    LOGD("WebGL CheckUniformDataInfo count %{public}u length %{public}zu %{public}zu Offset %{public}zu %{public}zu",
         static_cast<unsigned int>(count), static_cast<size_t>(readData.GetBufferLength()),
         static_cast<size_t>(info->elemCount), static_cast<size_t>(info->srcOffset),
         static_cast<size_t>(info->srcLength));
    if (isHighWebGL) {
        if (count <= info->srcOffset || count < info->srcLength) {
            return make_tuple(WebGLRenderingContextBase::INVALID_VALUE, 0, nullptr);
        }
        count = info->srcLength != 0 ? info->srcLength : count - info->srcOffset;
        srcData = srcData + info->srcOffset;
    }
    if (count % info->elemCount) {
        return make_tuple(WebGLRenderingContextBase::INVALID_VALUE, 0, nullptr);
    }
    if (count != info->elemCount) {
        return make_tuple(WebGLRenderingContextBase::INVALID_OPERATION, 0, nullptr);
    }
    return make_tuple(WebGLRenderingContextBase::NO_ERROR, count, srcData);
}

napi_value WebGLRenderingContextBaseImpl::UniformFv(
    napi_env env, napi_value locationObj, napi_value data, const UniformExtInfo* info)
{
    WebGLUniformLocation* webGLUniformLocation = WebGLUniformLocation::GetObjectInstance(env, locationObj);
    if (webGLUniformLocation == nullptr) {
        LOGE("WebGL uniform_fv can not find uniform");
        return NVal::CreateNull(env).val_;
    }
    GLint location = webGLUniformLocation->GetUniformLocationId();

    WebGLReadBufferArg readData(env);
    napi_status status = readData.GenBufferData(data, BUFFER_DATA_FLOAT_32);
    if (status != napi_ok) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "GenBufferData failed.");
        return NVal::CreateNull(env).val_;
    }
    readData.DumpBuffer(readData.GetBufferDataType());
    if (readData.GetBufferDataType() != BUFFER_DATA_FLOAT_32) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "GetBufferDataType failed. %{public}d",
            readData.GetBufferDataType());
        return NVal::CreateNull(env).val_;
    }
    GLenum result = 0;
    GLsizei count = 0;
    GLfloat* srcData = nullptr;
    std::tie(result, count, srcData) = CheckUniformDataInfo<GLfloat>(IsHighWebGL(), readData, info);
    if (result) {
        SET_ERROR(result);
        return NVal::CreateNull(env).val_;
    }

    switch (info->dimension) {
        case PARAMETERS_NUM_1:
            glUniform1fv(location, 1, srcData);
            break;
        case PARAMETERS_NUM_2:
            glUniform2fv(location, count / 2, srcData); // 2 array size
            break;
        case PARAMETERS_NUM_3:
            glUniform3fv(location, count / 3, srcData); // 3 array size
            break;
        case PARAMETERS_NUM_4:
            glUniform4fv(location, count / 4, srcData); // 4 array size
            break;
        default:
            break;
    }
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::UniformIv(
    napi_env env, napi_value locationObj, napi_value data, const UniformExtInfo* info)
{
    WebGLUniformLocation* webGLUniformLocation = WebGLUniformLocation::GetObjectInstance(env, locationObj);
    if (webGLUniformLocation == nullptr) {
        LOGE("WebGL UniformIv can not find uniform");
        return NVal::CreateNull(env).val_;
    }
    GLint location = webGLUniformLocation->GetUniformLocationId();

    WebGLReadBufferArg readData(env);
    napi_status status = readData.GenBufferData(data, BUFFER_DATA_INT_32);
    if (status != napi_ok) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "WebGL UniformIv failed to getbuffer data");
        return NVal::CreateNull(env).val_;
    }
    readData.DumpBuffer(readData.GetBufferDataType());
    if (readData.GetBufferDataType() != BUFFER_DATA_INT_32) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE,
            "WebGL UniformIv not support data type %{public}d", readData.GetBufferDataType());
        return NVal::CreateNull(env).val_;
    }
    GLenum result = 0;
    GLsizei count = 0;
    GLint* srcData = nullptr;
    std::tie(result, count, srcData) = CheckUniformDataInfo<GLint>(IsHighWebGL(), readData, info);
    if (result) {
        SET_ERROR(result);
        return NVal::CreateNull(env).val_;
    }
    switch (info->dimension) {
        case PARAMETERS_NUM_1:
            glUniform1iv(location, 1, srcData);
            break;
        case PARAMETERS_NUM_2:
            glUniform2iv(location, count / 2, srcData); // 2 array size
            break;
        case PARAMETERS_NUM_3:
            glUniform3iv(location, count / 3, srcData); // 3 array size
            break;
        case PARAMETERS_NUM_4:
            glUniform4iv(location, count / 4, srcData); // 4 array size
            break;
        default:
            break;
    }
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::UniformUiv(
    napi_env env, napi_value locationObj, napi_value dataObj, const UniformExtInfo* info)
{
    WebGLUniformLocation* webGLUniformLocation = WebGLUniformLocation::GetObjectInstance(env, locationObj);
    if (webGLUniformLocation == nullptr) {
        LOGE("WebGL uniform_uiv can not find uniform");
        return NVal::CreateNull(env).val_;
    }
    GLint location = webGLUniformLocation->GetUniformLocationId();

    WebGLReadBufferArg readData(env);
    napi_status status = readData.GenBufferData(dataObj, BUFFER_DATA_UINT_32);
    if (status != napi_ok) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "GenBufferData failed.");
        return NVal::CreateNull(env).val_;
    }
    readData.DumpBuffer(readData.GetBufferDataType());
    if (readData.GetBufferDataType() != BUFFER_DATA_UINT_32) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "GetBufferDataType failed. %{public}d",
            readData.GetBufferDataType());
        return NVal::CreateNull(env).val_;
    }
    GLenum result = 0;
    GLsizei count = 0;
    GLuint* srcData = nullptr;
    std::tie(result, count, srcData) = CheckUniformDataInfo<GLuint>(IsHighWebGL(), readData, info);
    if (result) {
        SET_ERROR(result);
        return NVal::CreateNull(env).val_;
    }
    switch (info->dimension) {
        case PARAMETERS_NUM_1:
            glUniform1uiv(location, 1, srcData);
            break;
        case PARAMETERS_NUM_2:
            glUniform2uiv(location, count / PARAMETERS_NUM_2, srcData);
            break;;
        case PARAMETERS_NUM_3:
            glUniform3uiv(location, count / PARAMETERS_NUM_3, srcData);
            break;
        case PARAMETERS_NUM_4:
            glUniform4uiv(location, count / PARAMETERS_NUM_4, srcData);
            break;
        default:
            break;
    }
    return NVal::CreateNull(env).val_;
}

void WebGLRenderingContextBaseImpl::HandleUniformMatrixInfo(
    GLboolean transpose, const UniformExtInfo* info, GLint location, GLsizei count, GLfloat* srcData)
{
    switch (info->dimension) {
        case WebGLArg::MATRIX_2X2_REQUIRE_MIN_SIZE:
            glUniformMatrix2fv(location, count / 4, transpose, srcData); // 4 is 2x2
            break;
        case WebGLArg::MATRIX_3X3_REQUIRE_MIN_SIZE:
            glUniformMatrix3fv(location, count / 8, transpose, srcData); // 8 is 2x2x2
            break;
        case WebGLArg::MATRIX_4X4_REQUIRE_MIN_SIZE:
            glUniformMatrix4fv(location, count / 16, transpose, srcData); // 16 is 2x2x2x2
            break;
        case WebGLArg::MATRIX_3X2_REQUIRE_MIN_SIZE:
            glUniformMatrix3x2fv(location, count / 6, transpose, srcData); // 6 matrix
            break;
        case WebGLArg::MATRIX_4X2_REQUIRE_MIN_SIZE:
            glUniformMatrix4x2fv(location, count / 8, transpose, srcData); // 8 matrix
            break;
        case WebGLArg::MATRIX_2X3_REQUIRE_MIN_SIZE:
            glUniformMatrix2x3fv(location, count / 6, transpose, srcData); // 6 matrix
            break;
        case WebGLArg::MATRIX_4X3_REQUIRE_MIN_SIZE:
            glUniformMatrix4x3fv(location, count / 12, transpose, srcData); // 12 matrix
            break;
        case WebGLArg::MATRIX_2X4_REQUIRE_MIN_SIZE:
            glUniformMatrix2x4fv(location, count / 8, transpose, srcData); // 8 matrix
            break;
        case WebGLArg::MATRIX_3X4_REQUIRE_MIN_SIZE:
            glUniformMatrix3x4fv(location, count / 12, transpose, srcData); // 12 matrix
            break;
        default:
            break;
    }
}

napi_value WebGLRenderingContextBaseImpl::UniformMatrixFv(
    napi_env env, napi_value locationObj, napi_value data, GLboolean transpose, const UniformExtInfo* info)
{
    WebGLUniformLocation* webGLUniformLocation = WebGLUniformLocation::GetObjectInstance(env, locationObj);
    if (webGLUniformLocation == nullptr) {
        LOGE("WebGL uniformMatrix_fv can not find uniform");
        return NVal::CreateNull(env).val_;
    }
    GLint location = webGLUniformLocation->GetUniformLocationId();

    WebGLReadBufferArg readData(env);
    napi_status status = readData.GenBufferData(data, BUFFER_DATA_FLOAT_32);
    if (status != napi_ok) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE,
            "WebGL uniformMatrix_fv failed to getbuffer data");
        return NVal::CreateNull(env).val_;
    }
    readData.DumpBuffer(readData.GetBufferDataType());

    if (readData.GetBufferDataType() != BUFFER_DATA_FLOAT_32) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE,
            "WebGL uniformMatrix_fv not support data type %{public}d", readData.GetBufferDataType());
        return NVal::CreateNull(env).val_;
    }
    GLenum result = 0;
    GLsizei count = 0;
    GLfloat* srcData = nullptr;
    std::tie(result, count, srcData) = CheckUniformDataInfo<GLfloat>(IsHighWebGL(), readData, info);
    if (result) {
        SET_ERROR(result);
        return NVal::CreateNull(env).val_;
    }
    HandleUniformMatrixInfo(transpose, info, location, count, srcData);
    LOGD("WebGL uniformMatrix_fv location %{public}d result %{public}u", location, GetError_());
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::CheckFrameBufferStatus(napi_env env, GLenum target)
{
    uint32_t index = 0;
    if (!CheckFrameBufferTarget(env, target, index)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateInt64(env, 0).val_;
    }
    WebGLFramebuffer* framebufferBinding = GetBoundFrameBuffer(env, target);
    if (!framebufferBinding) {
        return NVal::CreateInt64(env, WebGLRenderingContextBase::FRAMEBUFFER_COMPLETE).val_;
    }
    GLenum result = framebufferBinding->CheckStatus(env, this);
    if (result != WebGLRenderingContextBase::FRAMEBUFFER_COMPLETE) {
        return NVal::CreateInt64(env, static_cast<int64_t>(result)).val_;
    }
    result = glCheckFramebufferStatus(target);
    LOGD("WebGL checkFramebufferStatus target %{public}u result %{public}u", target, result);
    return NVal::CreateInt64(env, static_cast<int64_t>(result)).val_;
}

napi_value WebGLRenderingContextBaseImpl::DepthMask(napi_env env, bool flag)
{
    LOGD("WebGL depthMask flag %{public}d", flag);
    depthMask_ = flag;
    glDepthMask(static_cast<GLboolean>(flag));
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::StencilMask(napi_env env, GLuint mask)
{
    LOGD("WebGL stencilMask mask %{public}u", mask);
    stencilMask_[0] = mask;
    stencilMask_[1] = mask;
    glStencilMask(mask);
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::StencilMaskSeparate(napi_env env, GLenum face, GLuint mask)
{
    LOGD("WebGL stencilMaskSeparate face %{public}u mask %{public}u", face, mask);
    switch (face) {
        case WebGLRenderingContextBase::FRONT_AND_BACK: {
            stencilMask_[0] = mask;
            stencilMask_[1] = mask;
            break;
        }
        case WebGLRenderingContextBase::FRONT:
            stencilMask_[0] = mask;
            break;
        case WebGLRenderingContextBase::BACK:
            stencilMask_[1] = mask;
            break;
        default:
            SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
            return NVal::CreateNull(env).val_;
    }
    glStencilMaskSeparate(face, mask);
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::StencilFunc(napi_env env, GLenum func, GLint ref, GLuint mask)
{
    LOGD("WebGL stencilFunc func %{public}u %{public}d %{public}u", func, ref, mask);
    if (!CheckGLenum(func,
        { WebGLRenderingContextBase::NEVER, WebGLRenderingContextBase::LESS, WebGLRenderingContextBase::EQUAL,
            WebGLRenderingContextBase::LEQUAL, WebGLRenderingContextBase::GREATER,
            WebGLRenderingContextBase::NOTEQUAL, WebGLRenderingContextBase::GEQUAL,
            WebGLRenderingContextBase::ALWAYS }, {})) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    stencilFuncRef_[0] = ref;
    stencilFuncRef_[1] = ref;
    stencilFuncMask_[0] = mask;
    stencilFuncMask_[1] = mask;
    glStencilFunc(func, ref, mask);
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::StencilFuncSeparate(
    napi_env env, GLenum face, GLenum func, GLint ref, GLuint mask)
{
    LOGD("WebGL stencilFuncSeparate face %{public}u func %{public}u %{public}d %{public}u", face, func, ref, mask);
    switch (face) {
        case WebGLRenderingContextBase::FRONT_AND_BACK: {
            stencilFuncRef_[0] = ref;
            stencilFuncRef_[1] = ref;
            stencilFuncMask_[0] = mask;
            stencilFuncMask_[1] = mask;
            break;
        }
        case WebGLRenderingContextBase::FRONT: {
            stencilFuncRef_[0] = ref;
            stencilFuncMask_[0] = mask;
            break;
        }
        case WebGLRenderingContextBase::BACK: {
            stencilFuncRef_[1] = ref;
            stencilFuncMask_[1] = mask;
            break;
        }
        default:
            SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "invalid face %{public}u", face);
            return NVal::CreateNull(env).val_;
    }
    if (!CheckGLenum(func,
        { WebGLRenderingContextBase::NEVER, WebGLRenderingContextBase::LESS, WebGLRenderingContextBase::EQUAL,
            WebGLRenderingContextBase::LEQUAL, WebGLRenderingContextBase::GREATER,
            WebGLRenderingContextBase::NOTEQUAL, WebGLRenderingContextBase::GEQUAL,
            WebGLRenderingContextBase::ALWAYS }, {})) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    glStencilFuncSeparate(face, func, ref, mask);
    return NVal::CreateNull(env).val_;
}

template<class T>
GLenum WebGLRenderingContextBaseImpl::CheckTexParameter(
    napi_env env, GLenum target, GLenum pname, T param, bool isFloat)
{
    WebGLTexture* textuer = GetBoundTexture(env, target, false);
    if (textuer == nullptr) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }

    switch (pname) {
        case WebGLRenderingContextBase::TEXTURE_MIN_FILTER:
        case WebGLRenderingContextBase::TEXTURE_MAG_FILTER:
            break;
        case WebGL2RenderingContextBase::TEXTURE_WRAP_R: {
            if (!IsHighWebGL()) {
                return WebGLRenderingContextBase::INVALID_ENUM;
            }
            [[fallthrough]];
        }
        case WebGLRenderingContextBase::TEXTURE_WRAP_S:
        case WebGLRenderingContextBase::TEXTURE_WRAP_T: {
            if (param == WebGLRenderingContextBase::CLAMP_TO_EDGE ||
                param == WebGLRenderingContextBase::MIRRORED_REPEAT || param == WebGLRenderingContextBase::REPEAT) {
                break;
            }
            return WebGLRenderingContextBase::INVALID_ENUM;
        }
        case WebGL2RenderingContextBase::TEXTURE_COMPARE_FUNC:
        case WebGL2RenderingContextBase::TEXTURE_COMPARE_MODE:
        case WebGL2RenderingContextBase::TEXTURE_BASE_LEVEL:
        case WebGL2RenderingContextBase::TEXTURE_MAX_LEVEL:
        case WebGL2RenderingContextBase::TEXTURE_MAX_LOD:
        case WebGL2RenderingContextBase::TEXTURE_MIN_LOD: {
            if (!IsHighWebGL()) {
                return WebGLRenderingContextBase::INVALID_ENUM;
            }
            break;
        }
        default:
            return WebGLRenderingContextBase::INVALID_ENUM;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

napi_value WebGLRenderingContextBaseImpl::TexParameteri(napi_env env, GLenum target, GLenum pname, GLint param)
{
    GLenum result = CheckTexParameter<GLint>(env, target, pname, param, false);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR(result);
        return NVal::CreateNull(env).val_;
    }
    glTexParameteri(target, pname, static_cast<GLint>(param));
    LOGD("WebGL texParameteri target %{public}u %{public}u %{public}d result %{public}u ",
        target, pname, param, GetError_());
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::TexParameterf(napi_env env, GLenum target, GLenum pname, GLfloat param)
{
    GLenum result = CheckTexParameter<GLfloat>(env, target, pname, param, false);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR(result);
        return NVal::CreateNull(env).val_;
    }
    glTexParameterf(target, pname, static_cast<GLfloat>(param));
    LOGD("WebGL texParameterf target %{public}u %{public}u %{public}f result %{public}u ",
        target, pname, param, GetError_());
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::GetTexParameter(napi_env env, GLenum target, GLenum pname)
{
    WebGLTexture* texture = GetBoundTexture(env, target, false);
    if (!texture) {
        return NVal::CreateNull(env).val_;
    }
    if (IsHighWebGL()) {
        switch (pname) {
            case GL_TEXTURE_WRAP_R:
            case GL_TEXTURE_COMPARE_FUNC:
            case GL_TEXTURE_COMPARE_MODE:
            case GL_TEXTURE_BASE_LEVEL:
            case GL_TEXTURE_MAX_LEVEL:
            case GL_TEXTURE_IMMUTABLE_LEVELS: {
                GLint params = 0;
                glGetTexParameteriv(target, pname, &params);
                return NVal::CreateInt64(env, static_cast<int64_t>(params)).val_;
            }
            case GL_TEXTURE_IMMUTABLE_FORMAT: {
                GLint params = 0;
                glGetTexParameteriv(target, pname, &params);
                return NVal::CreateBool(env, params == 1).val_;
            }
            case GL_TEXTURE_MAX_LOD:
            case GL_TEXTURE_MIN_LOD: {
                GLfloat params = 0.f;
                glGetTexParameterfv(target, pname, &params);
                return NVal::CreateDouble(env, static_cast<double>(params)).val_;
            }
            default:
                break;
        }
    }

    switch (pname) {
        case WebGLRenderingContextBase::TEXTURE_MAG_FILTER:
        case WebGLRenderingContextBase::TEXTURE_MIN_FILTER:
        case WebGLRenderingContextBase::TEXTURE_WRAP_S:
        case WebGLRenderingContextBase::TEXTURE_WRAP_T: {
            GLint params = 0;
            glGetTexParameteriv(target, pname, &params);
            LOGD("WebGL getTexParameter params %{public}d", params);
            return NVal::CreateInt64(env, static_cast<int64_t>(params)).val_;
        }
        default:
            SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
            return NVal::CreateNull(env).val_;
    }
}

napi_value WebGLRenderingContextBaseImpl::VertexAttribfv(napi_env env, GLuint index, int32_t count, napi_value dataObj)
{
    LOGD("WebGL vertexAttribfv index %{public}u %{public}d", index, count);
    WebGLReadBufferArg bufferData(env);
    napi_status status = bufferData.GenBufferData(dataObj, BUFFER_DATA_FLOAT_32);
    if (status != 0) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "WebGL vertexAttribfv failed to getbuffer data");
        return NVal::CreateNull(env).val_;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferDataType() != BUFFER_DATA_FLOAT_32) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE,
            "WebGL :vertexAttribfv invalid buffer data type = %{public}d",
            static_cast<uint32_t>(bufferData.GetBufferDataType()));
        return NVal::CreateNull(env).val_;
    }

    if (static_cast<int32_t>(bufferData.GetBufferLength() / sizeof(float)) < count) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE,
            "WebGL vertexAttribfv invalid data length %{public}zu", bufferData.GetBufferLength());
        return NVal::CreateNull(env).val_;
    }
    VertexAttribInfo* info = GetVertexAttribInfo(index);
    if (info == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }

    switch (count) {
        case PARAMETERS_NUM_1:
            glVertexAttrib1fv(index, reinterpret_cast<GLfloat*>(bufferData.GetBuffer()));
            break;
        case PARAMETERS_NUM_2:
            glVertexAttrib2fv(index, reinterpret_cast<GLfloat*>(bufferData.GetBuffer()));
            break;
        case PARAMETERS_NUM_3:
            glVertexAttrib3fv(index, reinterpret_cast<GLfloat*>(bufferData.GetBuffer()));
            break;
        case PARAMETERS_NUM_4:
            glVertexAttrib4fv(index, reinterpret_cast<GLfloat*>(bufferData.GetBuffer()));
            break;
        default:
            break;
    }
    info->type = BUFFER_DATA_FLOAT_32;
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBaseImpl::VertexAttribf(napi_env env, GLuint index, int32_t count, GLfloat* data)
{
    VertexAttribInfo* info = GetVertexAttribInfo(index);
    if (info == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }
    // In GL, we skip setting vertexAttrib0 values.
    switch (count) {
        case PARAMETERS_NUM_1:
            glVertexAttrib1f(index, data[0]);
            break;
        case PARAMETERS_NUM_2:
            glVertexAttrib2f(index, data[0], data[1]);
            break;
        case PARAMETERS_NUM_3:
            glVertexAttrib3f(index, data[0], data[1], data[2]); // 1, 2 index
            break;
        case PARAMETERS_NUM_4:
            glVertexAttrib4f(index, data[0], data[1], data[2], data[3]); // 1, 2, 3 index
            break;
        default:
            break;
    }
    LOGD("WebGL vertexAttribf index %{public}u count %{public}d result %{public}u", index, count, GetError_());
    info->type = BUFFER_DATA_FLOAT_32;
    return NVal::CreateNull(env).val_;
}

GLenum WebGLRenderingContextBaseImpl::GetUniformType(napi_env env, GLuint programId, GLint locationId)
{
    GLint maxNameLength = -1;
    glGetProgramiv(programId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);
    if (maxNameLength <= 0) {
        return 0;
    }

    GLint activeUniforms = 0;
    glGetProgramiv(programId, GL_ACTIVE_UNIFORMS, &activeUniforms);
    LOGD("WebGL getUniform maxNameLength %{public}d activeUniforms %{public}d", maxNameLength, activeUniforms);
    if (locationId >= activeUniforms) {
        return 0;
    }
    GLenum type = 0;
    std::vector<GLchar> name(maxNameLength + 1);
    name[maxNameLength] = '\0';
    for (int32_t i = 0; i < activeUniforms; i++) {
        GLsizei nameLength = 0;
        GLint size = -1;
        glGetActiveUniform(programId, i, maxNameLength, &nameLength, &size, &type, name.data());
        LOGD("WebGL getUniform index %{public}d type 0x%{public}x name %{public}s ", i, type, name.data());
        if (locationId == i) {
            break;
        }
    }
    return type;
}

napi_value WebGLRenderingContextBaseImpl::GetUniform(napi_env env, napi_value programObj, napi_value uniformObj)
{
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, programObj);
    WebGLUniformLocation* webGLUniformLocation = WebGLUniformLocation::GetObjectInstance(env, uniformObj);
    if (webGLUniformLocation == nullptr || webGLProgram == nullptr) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE,
            "webGLUniformLocation or webGLProgram is nullptr.");
        return NVal::CreateNull(env).val_;
    }
    GLuint programId = webGLProgram->GetProgramId();
    GLint locationId = webGLUniformLocation->GetUniformLocationId();
    const UniformTypeMap* typeMap = GetUniformTypeMap(GetUniformType(env, programId, locationId));
    if (typeMap == nullptr) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "typeMap is nullptr.");
        return NVal::CreateNull(env).val_;
    }
    WebGLWriteBufferArg writeBuffer(env);
    GLint* data = reinterpret_cast<GLint*>(writeBuffer.AllocBuffer(typeMap->length * sizeof(GLfloat)));
    if (data == nullptr) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "data is nullptr.");
        return NVal::CreateNull(env).val_;
    }
    switch (typeMap->baseType) {
        case GL_FLOAT: {
            GLfloat* params = reinterpret_cast<GLfloat*>(data);
            glGetUniformfv(programId, locationId, params);
            if (typeMap->length == 1) {
                return NVal::CreateDouble(env, params[0]).val_;
            }
            break;
        }
        case GL_INT: {
            glGetUniformiv(programId, locationId, data);
            if (typeMap->length == 1) {
                return NVal::CreateInt64(env, data[0]).val_;
            }
            break;
        }
        case GL_BOOL: {
            glGetUniformiv(programId, locationId, data);
            if (typeMap->length == 1) {
                return NVal::CreateBool(env, data[0]).val_;
            }
            break;
        }
        case GL_UNSIGNED_INT: {
            GLuint* params = reinterpret_cast<GLuint*>(data);
            glGetUniformuiv(programId, locationId, params);
            if (typeMap->length == 1) {
                return NVal::CreateInt64(env, params[0]).val_;
            }
            break;
        }
        default:
            break;
    }
    writeBuffer.DumpBuffer(typeMap->dstType);
    return writeBuffer.ToNormalArray(typeMap->srcType, typeMap->dstType);
}

bool WebGLRenderingContextBaseImpl::CheckGLenum(
    GLenum type, const std::vector<GLenum>& glSupport, const std::vector<GLenum>& g2Support)
{
    auto it = std::find(glSupport.begin(), glSupport.end(), type);
    if (it != glSupport.end()) {
        return true;
    }
    if (!IsHighWebGL()) {
        return false;
    }
    auto it2 = std::find(g2Support.begin(), g2Support.end(), type);
    if (it2 != g2Support.end()) {
        return true;
    }
    return false;
}

bool WebGLRenderingContextBaseImpl::CheckInList(GLenum type, const std::vector<GLenum>& glSupport)
{
    auto it = std::find(glSupport.begin(), glSupport.end(), type);
    if (it != glSupport.end()) {
        return true;
    }
    return false;
}

bool WebGLRenderingContextBaseImpl::CheckCap(napi_env env, GLenum cap)
{
    return CheckGLenum(cap,
        { WebGLRenderingContextBase::BLEND, WebGLRenderingContextBase::CULL_FACE, WebGLRenderingContextBase::DEPTH_TEST,
            WebGLRenderingContextBase::DITHER, WebGLRenderingContextBase::POLYGON_OFFSET_FILL,
            WebGLRenderingContextBase::SAMPLE_ALPHA_TO_COVERAGE, WebGLRenderingContextBase::SAMPLE_COVERAGE,
            WebGLRenderingContextBase::SCISSOR_TEST, WebGLRenderingContextBase::STENCIL_TEST },
        { WebGL2RenderingContextBase::RASTERIZER_DISCARD });
}

bool WebGLRenderingContextBaseImpl::CheckInternalFormat(napi_env env, GLenum internalFormat)
{
    static const std::vector<GLenum> glSupport = { WebGLRenderingContextBase::RGBA4, WebGLRenderingContextBase::RGB565,
        WebGLRenderingContextBase::RGB5_A1, WebGLRenderingContextBase::DEPTH_COMPONENT16,
        WebGLRenderingContextBase::STENCIL_INDEX8, WebGLRenderingContextBase::DEPTH_STENCIL };
    static const std::vector<GLenum> gl2Support = { WebGL2RenderingContextBase::R8, WebGL2RenderingContextBase::R8UI,
        WebGL2RenderingContextBase::R8I, WebGL2RenderingContextBase::R16UI, WebGL2RenderingContextBase::R16I,
        WebGL2RenderingContextBase::R32UI, WebGL2RenderingContextBase::R32I,

        WebGL2RenderingContextBase::RG8, WebGL2RenderingContextBase::RG8UI, WebGL2RenderingContextBase::RG8I,
        WebGL2RenderingContextBase::RG16UI, WebGL2RenderingContextBase::RG16I, WebGL2RenderingContextBase::RG32UI,
        WebGL2RenderingContextBase::RG32I,

        WebGL2RenderingContextBase::RGB8, WebGL2RenderingContextBase::RGBA8, WebGL2RenderingContextBase::SRGB8_ALPHA8,
        WebGL2RenderingContextBase::RGB10_A2, WebGL2RenderingContextBase::RGBA8UI, WebGL2RenderingContextBase::RGBA8I,
        WebGL2RenderingContextBase::RGB10_A2UI,

        WebGL2RenderingContextBase::RGBA16UI, WebGL2RenderingContextBase::RGBA16I, WebGL2RenderingContextBase::RGBA32I,
        WebGL2RenderingContextBase::RGBA32UI, WebGL2RenderingContextBase::DEPTH_COMPONENT24,
        WebGL2RenderingContextBase::DEPTH_COMPONENT32F, WebGL2RenderingContextBase::DEPTH24_STENCIL8,
        WebGL2RenderingContextBase::DEPTH32F_STENCIL8 };
    return CheckGLenum(internalFormat, glSupport, gl2Support);
}

bool WebGLRenderingContextBaseImpl::CheckAttachment(napi_env env, GLenum attachment)
{
    static const std::vector<GLenum> glSupport = { WebGLRenderingContextBase::COLOR_ATTACHMENT0,
        WebGLRenderingContextBase::DEPTH_ATTACHMENT, WebGLRenderingContextBase::STENCIL_ATTACHMENT,
        WebGLRenderingContextBase::DEPTH_STENCIL_ATTACHMENT };

    if (!CheckGLenum(attachment, glSupport, {})) {
        if (IsHighWebGL()) {
            if (attachment < WebGLRenderingContextBase::COLOR_ATTACHMENT0 ||
                attachment > WebGLRenderingContextBase::COLOR_ATTACHMENT0 +
                static_cast<GLenum>(GetMaxColorAttachments())) {
                return false;
            }
        }
    }
    return true;
}

WebGLTexture* WebGLRenderingContextBaseImpl::GetBoundTexture(napi_env env, GLenum target, bool cubeMapExt)
{
    uint32_t index = 0;
    switch (target) {
        case WebGLRenderingContextBase::TEXTURE_2D:
            index = BoundTextureType::TEXTURE_2D;
            break;
        case WebGLRenderingContextBase::TEXTURE_CUBE_MAP_POSITIVE_X:
        case WebGLRenderingContextBase::TEXTURE_CUBE_MAP_NEGATIVE_X:
        case WebGLRenderingContextBase::TEXTURE_CUBE_MAP_POSITIVE_Y:
        case WebGLRenderingContextBase::TEXTURE_CUBE_MAP_NEGATIVE_Y:
        case WebGLRenderingContextBase::TEXTURE_CUBE_MAP_POSITIVE_Z:
        case WebGLRenderingContextBase::TEXTURE_CUBE_MAP_NEGATIVE_Z: {
            if (!cubeMapExt) {
                return nullptr;
            }
            index = BoundTextureType::TEXTURE_CUBE_MAP;
            break;
        }
        case WebGLRenderingContextBase::TEXTURE_CUBE_MAP: {
            if (cubeMapExt) {
                return nullptr;
            }
            index = BoundTextureType::TEXTURE_CUBE_MAP;
            break;
        }
        case WebGL2RenderingContextBase::TEXTURE_2D_ARRAY: {
            if (!IsHighWebGL()) {
                return nullptr;
            }
            index = BoundTextureType::TEXTURE_2D_ARRAY;
            break;
        }
        case WebGL2RenderingContextBase::TEXTURE_3D: {
            if (!IsHighWebGL()) {
                return nullptr;
            }
            index = BoundTextureType::TEXTURE_3D;
            break;
        }
        default:
            return nullptr;
    }
    if (activeTextureIndex_ >= boundTexture_[index].size()) {
        return nullptr;
    }

    return GetObjectInstance<WebGLTexture>(env, boundTexture_[index][activeTextureIndex_]);
}

WebGLFramebuffer* WebGLRenderingContextBaseImpl::GetBoundFrameBuffer(napi_env env, GLenum target)
{
    uint32_t index = 0;
    if (!CheckFrameBufferTarget(env, target, index)) {
        return nullptr;
    }
    LOGD("Get bound Frame buffer target %{public}u id %{public}u ", target, boundFrameBufferIds_[index]);
    return GetObjectInstance<WebGLFramebuffer>(env, boundFrameBufferIds_[index]);
}

WebGLRenderbuffer* WebGLRenderingContextBaseImpl::GetBoundRenderBuffer(napi_env env, GLenum target)
{
    uint32_t index = 0;
    if (!CheckRenderBufferTarget(env, target, index)) {
        return nullptr;
    }
    LOGD("Get bound render buffer target %{public}u id %{public}u ", target, boundRenderBufferIds_[index]);
    return GetObjectInstance<WebGLRenderbuffer>(env, boundRenderBufferIds_[index]);
}

WebGLBuffer* WebGLRenderingContextBaseImpl::GetBoundBuffer(napi_env env, GLenum target)
{
    uint32_t index = BoundBufferType::ARRAY_BUFFER;
    if (!CheckBufferTarget(env, target, index)) {
        return nullptr;
    }
    LOGD("Get bound buffer target %{public}u id %{public}u ", target, boundBufferIds_[index]);
    return GetObjectInstance<WebGLBuffer>(env, boundBufferIds_[index]);
}

bool WebGLRenderingContextBaseImpl::CheckDrawMode(napi_env env, GLenum mode)
{
    return CheckGLenum(mode,
        {
            WebGLRenderingContextBase::POINTS,
            WebGLRenderingContextBase::LINE_STRIP,
            WebGLRenderingContextBase::LINE_LOOP,
            WebGLRenderingContextBase::LINES,
            WebGLRenderingContextBase::TRIANGLE_STRIP,
            WebGLRenderingContextBase::TRIANGLE_FAN,
            WebGLRenderingContextBase::TRIANGLES
        },
        {});
}

bool WebGLRenderingContextBaseImpl::CheckFrameBufferTarget(napi_env env, GLenum target, uint32_t& index)
{
    switch (target) {
        case WebGLRenderingContextBase::FRAMEBUFFER:
            index = BoundFrameBufferType::FRAMEBUFFER;
            return true;
        default:
            break;
    }
    if (!IsHighWebGL()) {
        return false;
    }
    switch (target) {
        case WebGL2RenderingContextBase::DRAW_FRAMEBUFFER:
            index = BoundFrameBufferType::DRAW_FRAMEBUFFER;
            return true;
        case WebGL2RenderingContextBase::READ_FRAMEBUFFER:
            index = BoundFrameBufferType::READ_FRAMEBUFFER;
            return true;
        default:
            break;
    }
    return false;
}

bool WebGLRenderingContextBaseImpl::CheckRenderBufferTarget(napi_env env, GLenum target, uint32_t& index)
{
    switch (target) {
        case WebGLRenderingContextBase::RENDERBUFFER:
            index = BoundRenderBufferType::RENDERBUFFER;
            return true;
        default:
            break;
    }
    return false;
}

bool WebGLRenderingContextBaseImpl::CheckTextureTarget(napi_env env, GLenum target, uint32_t& index)
{
    switch (target) {
        case WebGLRenderingContextBase::TEXTURE_2D:
            index = BoundTextureType::TEXTURE_2D;
            break;
        case WebGLRenderingContextBase::TEXTURE_CUBE_MAP:
            index = BoundTextureType::TEXTURE_CUBE_MAP;
            break;
        case WebGL2RenderingContextBase::TEXTURE_3D:
            index = BoundTextureType::TEXTURE_3D;
            break;
        case WebGL2RenderingContextBase::TEXTURE_2D_ARRAY:
            index = BoundTextureType::TEXTURE_2D_ARRAY;
            break;
        default:
            if (IsHighWebGL()) {
                break;
            }
            return false;
    }
    return true;
}

bool WebGLRenderingContextBaseImpl::CheckShaderType(napi_env env, GLenum type, uint32_t& index)
{
    switch (type) {
        case WebGLRenderingContextBase::VERTEX_SHADER:
            index = BoundShaderType::VERTEX_SHADER;
            break;
        case WebGLRenderingContextBase::FRAGMENT_SHADER:
            index = BoundShaderType::FRAGMENT_SHADER;
            break;
        default:
            return false;
    }
    return true;
}

bool WebGLRenderingContextBaseImpl::CheckTexture2DTarget(napi_env env, GLenum target)
{
    return CheckGLenum(target,
        { WebGLRenderingContextBase::TEXTURE_2D, WebGLRenderingContextBase::TEXTURE_CUBE_MAP_POSITIVE_X,
            WebGLRenderingContextBase::TEXTURE_CUBE_MAP_NEGATIVE_X,
            WebGLRenderingContextBase::TEXTURE_CUBE_MAP_POSITIVE_Y,
            WebGLRenderingContextBase::TEXTURE_CUBE_MAP_NEGATIVE_Y,
            WebGLRenderingContextBase::TEXTURE_CUBE_MAP_POSITIVE_Z,
            WebGLRenderingContextBase::TEXTURE_CUBE_MAP_NEGATIVE_Z },
        {});
}

bool WebGLRenderingContextBaseImpl::CheckBufferTarget(napi_env env, GLenum target, uint32_t& index)
{
    switch (target) {
        case WebGLRenderingContextBase::ARRAY_BUFFER:
            index = BoundBufferType::ARRAY_BUFFER;
            break;
        case WebGLRenderingContextBase::ELEMENT_ARRAY_BUFFER:
            index = BoundBufferType::ELEMENT_ARRAY_BUFFER;
            break;
        case WebGL2RenderingContextBase::COPY_READ_BUFFER:
            index = BoundBufferType::COPY_READ_BUFFER;
            break;
        case WebGL2RenderingContextBase::COPY_WRITE_BUFFER:
            index = BoundBufferType::COPY_WRITE_BUFFER;
            break;
        case WebGL2RenderingContextBase::TRANSFORM_FEEDBACK_BUFFER:
            index = BoundBufferType::TRANSFORM_FEEDBACK_BUFFER;
            break;
        case WebGL2RenderingContextBase::UNIFORM_BUFFER:
            index = BoundBufferType::UNIFORM_BUFFER;
            break;
        case WebGL2RenderingContextBase::PIXEL_PACK_BUFFER:
            index = BoundBufferType::PIXEL_PACK_BUFFER;
            break;
        case WebGL2RenderingContextBase::PIXEL_UNPACK_BUFFER:
            index = BoundBufferType::PIXEL_UNPACK_BUFFER;
            break;
        default:
            if (IsHighWebGL()) {
                break;
            }
            return false;
    }
    return true;
}

bool WebGLRenderingContextBaseImpl::CheckBufferDataUsage(napi_env env, GLenum usage)
{
    return CheckGLenum(usage, { GL_STREAM_DRAW, GL_STATIC_DRAW, GL_DYNAMIC_DRAW },
        { GL_STREAM_READ, GL_STREAM_COPY, GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_READ, GL_DYNAMIC_COPY });
}

bool WebGLRenderingContextBaseImpl::CheckPixelsFormat(napi_env env, GLenum format)
{
    return CheckGLenum(format, { GL_ALPHA, GL_RGB, GL_RGBA },
        {
            GL_RED,
            GL_RED_INTEGER,
            GL_RG,
            GL_RG_INTEGER,
            GL_RGBA,
            GL_RGBA_INTEGER,
            GL_LUMINANCE_ALPHA,
            GL_LUMINANCE,
            GL_ALPHA,
            GL_RGB,
            GL_RGB_INTEGER,
        });
}

bool WebGLRenderingContextBaseImpl::CheckPixelsType(napi_env env, GLenum type)
{
    return CheckGLenum(type,
        { GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_5_5_5_1, GL_FLOAT,
            GL_HALF_FLOAT_OES },
        { GL_UNSIGNED_BYTE, GL_BYTE, GL_HALF_FLOAT, GL_FLOAT, GL_UNSIGNED_SHORT, GL_UNSIGNED_SHORT_5_6_5,
            GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_5_5_5_1, GL_SHORT, GL_UNSIGNED_INT,
            GL_UNSIGNED_INT_2_10_10_10_REV, GL_UNSIGNED_INT_10F_11F_11F_REV, GL_UNSIGNED_INT_5_9_9_9_REV, GL_INT
        });
}

bool WebGLRenderingContextBaseImpl::CheckStencil(napi_env env)
{
    if (stencilMask_[0] != stencilMask_[1] || stencilFuncRef_[0] != stencilFuncRef_[1] ||
        stencilFuncMask_[0] != stencilFuncMask_[1]) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION,
            "front and back stencils settings do not match");
        return false;
    }
    return true;
}

bool WebGLRenderingContextBaseImpl::CheckLocationName(const std::string& name)
{
    if (name.size() > MAX_LOCATION_LENGTH) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "check name size failed.%{public}zu",
            name.size());
        return false;
    }
    if (!WebGLArg::CheckString(name)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "CheckString failed.");
        return false;
    }
    if (WebGLArg::CheckReservedPrefix(name)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "CheckReservedPrefix failed.");
        return false;
    }
    return true;
}

WebGLRenderbuffer* WebGLRenderingContextBaseImpl::CheckRenderBufferStorage(napi_env env, const TexStorageArg& arg)
{
    uint32_t index = 0;
    if (!CheckRenderBufferTarget(env, arg.target, index)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM,
            "WebGL renderbufferStorage invalid target %{public}u", arg.target);
        return nullptr;
    }
    if (!CheckInternalFormat(env, arg.internalFormat)) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM,
            "WebGL renderbufferStorage invalid internalFormat %{public}u", arg.internalFormat);
        return nullptr;
    }
    if (arg.width < 0 || arg.height < 0) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE,
            "WebGL renderbufferStorage invalid size %{public}d %{public}d", arg.width, arg.height);
        return nullptr;
    }
    WebGLRenderbuffer* renderBuffer = GetObjectInstance<WebGLRenderbuffer>(env, boundRenderBufferIds_[index]);
    if (renderBuffer == nullptr) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION,
            "WebGL renderbufferStorage can not get bound render buffer Id %{public}u", boundRenderBufferIds_[index]);
        return nullptr;
    }
    return renderBuffer;
}

bool WebGLRenderingContextBaseImpl::CheckProgramLinkStatus(WebGLProgram* program)
{
    if (program == nullptr) {
        return false;
    }
    GLint params = 0;
    glGetProgramiv(static_cast<GLuint>(program->GetProgramId()), WebGLRenderingContextBase::LINK_STATUS, &params);
    if (params) {
        return true;
    }
    return false;
}

const UniformTypeMap* WebGLRenderingContextBaseImpl::GetUniformTypeMap(GLenum type)
{
    const static UniformTypeMap baseTypeMap[] = {
        { GL_FLOAT, GL_FLOAT, 1, BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32 },
        { GL_FLOAT_VEC2, GL_FLOAT, 2, BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32 },
        { GL_FLOAT_VEC3, GL_FLOAT, 3, BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32 },
        { GL_FLOAT_VEC4, GL_FLOAT, 4, BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32 },
        { GL_FLOAT_MAT2, GL_FLOAT, 4, BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32 },
        { GL_FLOAT_MAT3, GL_FLOAT, 9, BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32 },
        { GL_FLOAT_MAT4, GL_FLOAT, 16, BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32 },

        { GL_INT, GL_INT, 1, BUFFER_DATA_INT_32, BUFFER_DATA_INT_32 },
        { GL_INT_VEC2, GL_INT, 2, BUFFER_DATA_INT_32, BUFFER_DATA_INT_32 },
        { GL_INT_VEC3, GL_INT, 3, BUFFER_DATA_INT_32, BUFFER_DATA_INT_32 },
        { GL_INT_VEC4, GL_INT, 4, BUFFER_DATA_INT_32, BUFFER_DATA_INT_32 },

        { GL_BOOL, GL_BOOL, 1, BUFFER_DATA_INT_32, BUFFER_DATA_BOOLEAN },
        { GL_BOOL_VEC2, GL_BOOL, 2, BUFFER_DATA_INT_32, BUFFER_DATA_BOOLEAN },
        { GL_BOOL_VEC3, GL_BOOL, 3, BUFFER_DATA_INT_32, BUFFER_DATA_BOOLEAN },
        { GL_BOOL_VEC4, GL_BOOL, 4, BUFFER_DATA_INT_32, BUFFER_DATA_BOOLEAN },

        { GL_SAMPLER_2D, GL_INT, 1, BUFFER_DATA_INT_32, BUFFER_DATA_INT_32 },
        { GL_SAMPLER_CUBE, GL_INT, 1, BUFFER_DATA_INT_32, BUFFER_DATA_INT_32 },
    };

    const static UniformTypeMap webgl2BaseTypeMap[] = {
        { GL_UNSIGNED_INT, GL_UNSIGNED_INT, 1, BUFFER_DATA_UINT_32, BUFFER_DATA_UINT_32 },
        { GL_UNSIGNED_INT_VEC2, GL_UNSIGNED_INT, 2, BUFFER_DATA_UINT_32, BUFFER_DATA_UINT_32 },
        { GL_UNSIGNED_INT_VEC3, GL_UNSIGNED_INT, 3, BUFFER_DATA_UINT_32, BUFFER_DATA_UINT_32 },
        { GL_UNSIGNED_INT_VEC4, GL_UNSIGNED_INT, 4, BUFFER_DATA_UINT_32, BUFFER_DATA_UINT_32 },

        { GL_FLOAT_MAT2x3, GL_FLOAT, 6, BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32 },
        { GL_FLOAT_MAT2x4, GL_FLOAT, 8, BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32 },
        { GL_FLOAT_MAT3x2, GL_FLOAT, 6, BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32 },
        { GL_FLOAT_MAT3x4, GL_FLOAT, 12, BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32 },
        { GL_FLOAT_MAT4x2, GL_FLOAT, 8, BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32 },
        { GL_FLOAT_MAT4x3, GL_FLOAT, 12, BUFFER_DATA_FLOAT_32, BUFFER_DATA_FLOAT_32 },

        { GL_SAMPLER_3D, GL_INT, 1, BUFFER_DATA_INT_32, BUFFER_DATA_INT_32 },
        { GL_SAMPLER_2D_ARRAY, GL_INT, 1, BUFFER_DATA_INT_32, BUFFER_DATA_INT_32 },
    };
    for (size_t i = 0; i < sizeof(baseTypeMap) / sizeof(baseTypeMap[0]); i++) {
        if (baseTypeMap[i].type == type) {
            return &baseTypeMap[i];
        }
    }
    if (!IsHighWebGL()) {
        return nullptr;
    }
    for (size_t i = 0; i < sizeof(webgl2BaseTypeMap) / sizeof(webgl2BaseTypeMap[0]); i++) {
        if (webgl2BaseTypeMap[i].type == type) {
            return &webgl2BaseTypeMap[i];
        }
    }
    return nullptr;
}

GLenum WebGLRenderingContextBaseImpl::CheckFrameBufferBoundComplete(napi_env env)
{
    WebGLFramebuffer* framebufferBinding = GetBoundFrameBuffer(env, WebGLRenderingContextBase::FRAMEBUFFER);
    if (framebufferBinding &&
        framebufferBinding->CheckStatus(env, this) != WebGLRenderingContextBase::FRAMEBUFFER_COMPLETE) {
        return WebGLRenderingContextBase::INVALID_FRAMEBUFFER_OPERATION;
    }
    GLenum complete = glCheckFramebufferStatus(WebGLRenderingContextBase::FRAMEBUFFER);
    LOGD("CheckFrameBufferBoundComplete complete %{public}u", complete);
    if (complete != WebGLRenderingContextBase::FRAMEBUFFER_COMPLETE) {
        return WebGLRenderingContextBase::INVALID_FRAMEBUFFER_OPERATION;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

GLenum WebGLRenderingContextBaseImpl::CheckReadPixelsArg(napi_env env, const PixelsArg& arg, uint64_t bufferSize)
{
    if (!CheckPixelsFormat(env, arg.format) || !CheckPixelsType(env, arg.type)) {
        return WebGLRenderingContextBase::INVALID_ENUM;
    }
    return 0;
}

GLenum WebGLRenderingContextBaseImpl::CheckVertexAttribPointer(napi_env env, const VertexAttribArg& vertexInfo)
{
    if (vertexInfo.index >= GetMaxVertexAttribs()) {
        LOGE("WebGL vertexAttribPointer invalid index %{public}u", vertexInfo.index);
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    // check size,  4 is max vertexInfo size, 255 is max vertexInfo stride
    if (vertexInfo.size < 1 || vertexInfo.size > 4 || vertexInfo.stride < 0 || vertexInfo.stride > 255) {
        LOGE("WebGL vertexAttribPointer invalid size %{public}d %{public}d", vertexInfo.size, vertexInfo.stride);
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    // check offset
    WebGLBuffer* webGLBuffer = GetBoundBuffer(env, WebGLRenderingContextBase::ARRAY_BUFFER);
    if (webGLBuffer == nullptr || webGLBuffer->GetBufferSize() == 0) {
        LOGE("WebGL vertexAttribPointer can not bind buffer %{public}p ", webGLBuffer);
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }

    // check offset
    if (vertexInfo.offset >= static_cast<GLintptr>(webGLBuffer->GetBufferSize())) {
        LOGE("WebGL vertexAttribPointer invalid offset %{public}u", static_cast<unsigned int>(vertexInfo.offset));
        return WebGLRenderingContextBase::INVALID_VALUE;
    }

    uint32_t typeSize = WebGLArg::GetWebGLDataSize(vertexInfo.type);
    if ((static_cast<uint32_t>(vertexInfo.stride) & static_cast<uint32_t>(typeSize - 1)) ||
        (static_cast<uint32_t>(vertexInfo.offset) & static_cast<uint32_t>(typeSize - 1))) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

GLenum WebGLRenderingContextBaseImpl::GetBoundFrameBufferColorFormat(napi_env env)
{
    WebGLFramebuffer* frameBuffer = GetBoundFrameBuffer(env, WebGLRenderingContextBase::FRAMEBUFFER);
    if (frameBuffer != nullptr) {
        WebGLAttachmentInfo info = {};
        if (frameBuffer->GetWebGLAttachmentInfo(env, this, frameBuffer->GetAttachment(GL_COLOR_ATTACHMENT0), info)) {
            return info.format;
        }
    }
    auto webGlContextAttributes = webGLRenderingContext_->CreateWebGlContextAttributes();
    if (webGlContextAttributes != nullptr && webGlContextAttributes->alpha_) {
        return GL_RGBA;
    }
    return GL_RGB;
}

VertexAttribInfo* WebGLRenderingContextBaseImpl::GetVertexAttribInfo(GLint index)
{
    if (index >= static_cast<GLint>(arrayVertexAttribs_.size())) {
        LOGE("Invalid index for VertexAttrib %{public}d, max %{public}zu",
            index, arrayVertexAttribs_.size());
        return nullptr;
    }
    return &arrayVertexAttribs_[index];
}

bool WebGLRenderingContextBaseImpl::CheckReadBufferMode(GLenum mode)
{
    switch (mode) {
        case WebGLRenderingContextBase::BACK:
        case WebGLRenderingContextBase::NONE:
        case WebGLRenderingContextBase::COLOR_ATTACHMENT0:
            return true;
        default: {
            if (mode < WebGLRenderingContextBase::COLOR_ATTACHMENT0) {
                return false;
            }
            if (mode > static_cast<GLenum>(WebGLRenderingContextBase::COLOR_ATTACHMENT0 + GetMaxColorAttachments())) {
                return false;
            }
            break;
        }
    }
    return true;
}

GLint WebGLRenderingContextBaseImpl::GetMaxColorAttachments()
{
    if (!maxColorAttachments_) {
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &maxColorAttachments_);
    }
    LOGD("maxColorAttachments %{public}d", maxColorAttachments_);
    return maxColorAttachments_;
}

WebGLFramebuffer* WebGLRenderingContextBaseImpl::GetValidFrameBuffer(napi_env env, napi_value object)
{
    WebGLFramebuffer* webGlFramebuffer = WebGLObject::GetObjectInstance<WebGLFramebuffer>(env, object);
    if (webGlFramebuffer == nullptr) {
        return nullptr;
    }
    return GetObjectInstance<WebGLFramebuffer>(env, webGlFramebuffer->GetFramebuffer());
}

WebGLBuffer* WebGLRenderingContextBaseImpl::GetValidBuffer(napi_env env, napi_value object)
{
    WebGLBuffer* buffer = WebGLObject::GetObjectInstance<WebGLBuffer>(env, object);
    if (buffer == nullptr) {
        return nullptr;
    }
    return GetObjectInstance<WebGLBuffer>(env, buffer->GetBufferId());
}

WebGLRenderbuffer* WebGLRenderingContextBaseImpl::GetValidRenderBuffer(napi_env env, napi_value object)
{
    WebGLRenderbuffer* renderBuffer = WebGLObject::GetObjectInstance<WebGLRenderbuffer>(env, object);
    if (renderBuffer == nullptr) {
        return nullptr;
    }
    return GetObjectInstance<WebGLRenderbuffer>(env, renderBuffer->GetRenderbuffer());
}
} // namespace Impl
} // namespace Rosen
} // namespace OHOS
