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
#include "../include/context/webgl_rendering_context_base.h"
#include "../include/context/webgl2_rendering_context_base.h"
#include "../include/context/webgl_context_attributes.h"
#include "../../common/napi/n_func_arg.h"
#include "../../common/napi/n_class.h"
#include "../include/webgl/webgl_shader.h"
#include "../include/webgl/webgl_buffer.h"
#include "../include/webgl/webgl_framebuffer.h"
#include "../include/webgl/webgl_program.h"
#include "../include/webgl/webgl_renderbuffer.h"
#include "../include/webgl/webgl_texture.h"
#include "../include/webgl/webgl_uniform_location.h"
#include "../include/webgl/webgl_active_info.h"
#include "../include/webgl/webgl_shader_precision_format.h"
#include "../include/util/log.h"
#include "../include/util/egl_manager.h"
#include "../include/util/object_source.h"
#include "../include/util/util.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Rosen {
using namespace std;

napi_value WebGLRenderingContextBase::GetContextAttributes(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContextBasicBase *obj = Util::GetContextObject(env, funcArg.GetThisVar(), "webgl");
    if (obj == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    WebGLContextAttributes *webGlContextAttributes = obj->webGlContextAttributes;
    if (webGlContextAttributes == nullptr) {
        webGlContextAttributes = new WebGLContextAttributes();
    }
    NVal res = NVal::CreateObject(env);
    napi_value alpha = NVal::CreateBool(env, webGlContextAttributes->alpha).val_;
    napi_value antialias = NVal::CreateBool(env, webGlContextAttributes->antialias).val_;
    napi_value depth = NVal::CreateBool(env, webGlContextAttributes->depth).val_;
    napi_value failIfMajorPerformanceCaveat =
        NVal::CreateBool(env, webGlContextAttributes->failIfMajorPerformanceCaveat).val_;
    napi_value desynchronized = NVal::CreateBool(env, webGlContextAttributes->desynchronized).val_;
    napi_value premultipliedAlpha = NVal::CreateBool(env, webGlContextAttributes->premultipliedAlpha).val_;
    napi_value preserveDrawingBuffer = NVal::CreateBool(env, webGlContextAttributes->preserveDrawingBuffer).val_;
    napi_value stencil = NVal::CreateBool(env, webGlContextAttributes->stencil).val_;
    napi_value powerPreference = NVal::CreateUTF8String(env, webGlContextAttributes->powerPreference).val_;
    res.AddProp("alpha", alpha);
    res.AddProp("antialias", antialias);
    res.AddProp("depth", depth);
    res.AddProp("failIfMajorPerformanceCaveat", failIfMajorPerformanceCaveat);
    res.AddProp("desynchronized", desynchronized);
    res.AddProp("premultipliedAlpha", premultipliedAlpha);
    res.AddProp("preserveDrawingBuffer", preserveDrawingBuffer);
    res.AddProp("stencil", stencil);
    res.AddProp("powerPreference", powerPreference);
    delete webGlContextAttributes;
    return res.val_;
}

napi_value WebGLRenderingContextBase::IsContextLost(napi_env env, napi_callback_info info)
{
    bool res = false;
    if (EglManager::GetInstance().GetEGLContext() == nullptr) {
        res = true;
    };
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGLRenderingContextBase::GetSupportedExtensions(napi_env env, napi_callback_info info)
{
    LOGI("WebGL getSupportedExtensions start");
    const char* extensions = eglQueryString(EglManager::GetInstance().GetEGLDisplay(), EGL_EXTENSIONS);
    string str = extensions;
    vector<string> vec;
    Util::SplitString(str, vec, " ");
    napi_value result = nullptr;
    napi_create_array_with_length(env, vec.size(), &result);
    for (vector<string>::size_type i = 0; i != vec.size(); ++i) {
        napi_set_element(env, result, i, NVal::CreateUTF8String(env, vec[i]).val_);
    }
    LOGI("WebGL getSupportedExtensions end");
    return result;
}

napi_value WebGLRenderingContextBase::GetExtension(napi_env env, napi_callback_info info)
{
    return nullptr;
}

napi_value WebGLRenderingContextBase::ActiveTexture(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL activeTexture start");
    int64_t texture;
    tie(succ, texture) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::activeTexture texture = %{public}u", texture);
    glActiveTexture(static_cast<GLenum>(texture));
    LOGI("WebGL activeTexture end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::AttachShader(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr || funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    LOGI("WebGL attachShader start");
    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    int programId = webGlProgram->GetProgramId();

    WebGLShader *webGlShader = nullptr;
    napi_status shaderStatus = napi_unwrap(env, funcArg[NARG_POS::SECOND], (void **) &webGlShader);
    if (shaderStatus != napi_ok) {
        return nullptr;
    }
    int shaderId = webGlShader->GetShaderId();
    glAttachShader(static_cast<GLuint>(programId), static_cast<GLuint>(shaderId));
    LOGI("WebGL attachShader end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::BindAttribLocation(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL bindAttribLocation start");
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    int programId = webGlProgram->GetProgramId();

    int64_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::bindAttribLocation index = %{public}u", index);

    unique_ptr<char[]> name;
    tie(succ, name, ignore) = NVal(env, funcArg[NARG_POS::THIRD]).ToUTF8String();
    if (!succ) {
        return nullptr;
    }
    glBindAttribLocation(static_cast<GLuint>(programId), static_cast<GLuint>(index),
        const_cast<char *>(name.get()));
    LOGI("WebGL bindAttribLocation end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::BindBuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL bindBuffer start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::bindBuffer target = %{public}u", target);

    if (funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    WebGLBuffer *webGlBuffer = nullptr;
    napi_status bufferStatus = napi_unwrap(env, funcArg[NARG_POS::SECOND], (void **) &webGlBuffer);
    if (bufferStatus != napi_ok) {
        return nullptr;
    }
    unsigned int buffer = webGlBuffer->GetBuffer();
    glBindBuffer(static_cast<GLenum>(target), static_cast<GLuint>(buffer));
    LOGI("WebGL bindBuffer end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::BindFramebuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL bindFramebuffer start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::bindFramebuffer target = %{public}u", target);

    if (funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    WebGLFramebuffer *webGlFramebuffer = nullptr;
    napi_status framebufferStatus = napi_unwrap(env, funcArg[NARG_POS::SECOND], (void **) &webGlFramebuffer);
    if (framebufferStatus != napi_ok) {
        return nullptr;
    }
    unsigned int framebuffer = webGlFramebuffer->GetFramebuffer();
    glBindFramebuffer(static_cast<GLenum>(target), static_cast<GLuint>(framebuffer));
    LOGI("WebGL bindFramebuffer end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::BindRenderbuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL bindRenderbuffer start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::bindFramebuffer target = %{public}u", target);

    if (funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    WebGLRenderbuffer *webGlRenderbuffer = nullptr;
    napi_status renderbufferStatus = napi_unwrap(env, funcArg[NARG_POS::SECOND], (void **) &webGlRenderbuffer);
    if (renderbufferStatus != napi_ok) {
        return nullptr;
    }
    unsigned int renderbuffer = webGlRenderbuffer->GetRenderbuffer();
    glBindRenderbuffer(static_cast<GLenum>(target), static_cast<GLuint>(renderbuffer));
    LOGI("WebGL bindRenderbuffer end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::BindTexture(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL bindTexture start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::bindTexture target = %{public}u", target);

    if (funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    WebGLTexture *webGlTexture = nullptr;
    napi_status textureStatus = napi_unwrap(env, funcArg[NARG_POS::SECOND], (void **) &webGlTexture);
    if (textureStatus != napi_ok) {
        return nullptr;
    }
    unsigned int texture = webGlTexture->GetTexture();
    LOGI("WebGL WebGLRenderingContextBase::bindTexture textureId = %{public}u", texture);
    glBindTexture(static_cast<GLenum>(target), static_cast<GLuint>(texture));
    LOGI("WebGL bindTexture end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::BlendColor(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL blendColor start");
    double red;
    tie(succ, red) = NVal(env, funcArg[NARG_POS::FIRST]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    double green;
    tie(succ, green) = NVal(env, funcArg[NARG_POS::SECOND]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    double blue;
    tie(succ, blue) = NVal(env, funcArg[NARG_POS::THIRD]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    double alpha;
    tie(succ, alpha) = NVal(env, funcArg[NARG_POS::FOURTH]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    glBlendColor(static_cast<GLclampf>((float) red), static_cast<GLclampf>((float) green),
        static_cast<GLclampf>((float) blue), static_cast<GLclampf>((float) alpha));
    LOGI("WebGL blendColor end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::BlendEquation(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL blendEquation start");
    int64_t mode;
    tie(succ, mode) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::blendEquation mode = %{public}u", mode);
    glBlendEquation(static_cast<GLenum>(mode));
    LOGI("WebGL blendEquation end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::BlendEquationSeparate(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL blendEquationSeparate start");
    int64_t modeRGB;
    tie(succ, modeRGB) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::blendEquationSeparate modeRGB = %{public}u", modeRGB);
    int64_t modeAlpha;
    tie(succ, modeAlpha) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::blendEquationSeparate modeAlpha = %{public}u", modeAlpha);
    glBlendEquationSeparate(static_cast<GLenum>(modeRGB), static_cast<GLenum>(modeAlpha));
    LOGI("WebGL blendEquationSeparate end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::BlendFunc(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL blendFunc start");
    int64_t sFactor;
    tie(succ, sFactor) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::blendFunc sFactor = %{public}u", sFactor);
    int64_t dFactor;
    tie(succ, dFactor) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::blendFunc dFactor = %{public}u", dFactor);
    glBlendFunc(static_cast<GLenum>(sFactor), static_cast<GLenum>(dFactor));
    LOGI("WebGL blendFunc end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::BlendFuncSeparate(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL blendFuncSeparate start");
    int64_t srcRGB;
    tie(succ, srcRGB) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::blendFuncSeparate srcRGB = %{public}u", srcRGB);
    int64_t dstRGB;
    tie(succ, dstRGB) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::blendFuncSeparate dstRGB = %{public}u", dstRGB);
    int64_t srcAlpha;
    tie(succ, srcAlpha) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::blendFuncSeparate srcAlpha = %{public}u", srcAlpha);
    int64_t dstAlpha;
    tie(succ, dstAlpha) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::blendFuncSeparate dstAlpha = %{public}u", dstAlpha);
    glBlendFuncSeparate(static_cast<GLenum>(srcRGB), static_cast<GLenum>(dstRGB), static_cast<GLenum>(srcAlpha),
        static_cast<GLenum>(dstAlpha));
    LOGI("WebGL blendFuncSeparate end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::CheckFramebufferStatus(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL checkFramebufferStatus start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::checkFramebufferStatus target = %{public}u", target);
    int64_t res = static_cast<int64_t>(glCheckFramebufferStatus(static_cast<GLenum>(target)));
    LOGI("WebGL checkFramebufferStatus end");
    return NVal::CreateInt64(env, res).val_;
}

napi_value WebGLRenderingContextBase::Clear(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL clear start");
    int64_t mask;
    tie(succ, mask) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::clear mask = %{public}u", mask);
    glClear(static_cast<GLbitfield>(mask));
    LOGI("WebGL clear end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::ClearColor(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL clearColor start");
    double red;
    tie(succ, red) = NVal(env, funcArg[NARG_POS::FIRST]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    double green;
    tie(succ, green) = NVal(env, funcArg[NARG_POS::SECOND]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    double blue;
    tie(succ, blue) = NVal(env, funcArg[NARG_POS::THIRD]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    double alpha;
    tie(succ, alpha) = NVal(env, funcArg[NARG_POS::FOURTH]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    glClearColor(static_cast<GLclampf>((float) red), static_cast<GLclampf>((float) green),
        static_cast<GLclampf>((float) blue), static_cast<GLclampf>((float) alpha));
    LOGI("WebGL clearColor end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::ClearDepth(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL clearDepth start");
    double depth;
    tie(succ, depth) = NVal(env, funcArg[NARG_POS::FIRST]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    glClearDepthf(static_cast<GLclampf>((float) depth));
    LOGI("WebGL clearDepth end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::ClearStencil(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL clearStencil start");
    int64_t s;
    tie(succ, s) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::clearStencil s = %{public}u", s);
    glClearStencil(static_cast<GLint>(s));
    LOGI("WebGL clearStencil end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::ColorMask(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL colorMask start");
    bool red = false;
    tie(succ, red) = NVal(env, funcArg[NARG_POS::FIRST]).ToBool();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::colorMask red = %{public}u", red);
    bool green = false;
    tie(succ, green) = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::colorMask green = %{public}u", green);
    bool blue = false;
    tie(succ, blue) = NVal(env, funcArg[NARG_POS::THIRD]).ToBool();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::colorMask blue = %{public}u", blue);
    bool alpha = false;
    tie(succ, alpha) = NVal(env, funcArg[NARG_POS::FOURTH]).ToBool();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::colorMask alpha = %{public}u", alpha);
    glColorMask(static_cast<GLboolean>(red), static_cast<GLboolean>(green), static_cast<GLboolean>(blue),
        static_cast<GLboolean>(alpha));
    LOGI("WebGL colorMask end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::CompileShader(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    LOGI("WebGL compileShader start");
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }

    WebGLShader *webGlShader = nullptr;
    napi_status shaderStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlShader);
    if (shaderStatus != napi_ok) {
        return nullptr;
    }
    int shaderId = webGlShader->GetShaderId();
    glCompileShader(static_cast<GLuint>(shaderId));
    LOGI("WebGL compileShader end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::CopyTexImage2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::EIGHT)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL copyTexImage2D start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::copyTexImage2D target = %{public}u", target);
    int64_t level;
    tie(succ, level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::copyTexImage2D level = %{public}u", level);
    int64_t internalformat;
    tie(succ, internalformat) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::copyTexImage2D internalformat = %{public}u", internalformat);
    int64_t x;
    tie(succ, x) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::copyTexImage2D x = %{public}u", x);
    int64_t y;
    tie(succ, y) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::copyTexImage2D y = %{public}u", y);
    int64_t width;
    tie(succ, width) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::copyTexImage2D width = %{public}u", width);
    int64_t height;
    tie(succ, height) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::copyTexImage2D height = %{public}u", height);
    int64_t border;
    tie(succ, border) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::copyTexImage2D border = %{public}u", border);
    glCopyTexImage2D(static_cast<GLenum>(target), static_cast<GLint>(level),
        static_cast<GLenum>(internalformat), static_cast<GLint>(x), static_cast<GLint>(y),
        static_cast<GLsizei>(width), static_cast<GLsizei>(height), static_cast<GLint>(border));
    LOGI("WebGL copyTexImage2D end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::CopyTexSubImage2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::EIGHT)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL copyTexSubImage2D start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::copyTexSubImage2D target = %{public}u", target);
    int64_t level;
    tie(succ, level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::copyTexSubImage2D level = %{public}u", level);
    int64_t xoffset;
    tie(succ, xoffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::copyTexSubImage2D xoffset = %{public}u", xoffset);
    int64_t yoffset;
    tie(succ, yoffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::copyTexSubImage2D yoffset = %{public}u", yoffset);
    int64_t x;
    tie(succ, x) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::copyTexSubImage2D x = %{public}u", x);
    int64_t y;
    tie(succ, y) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::copyTexSubImage2D y = %{public}u", y);
    int64_t width;
    tie(succ, width) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::copyTexSubImage2D width = %{public}u", width);
    int64_t height;
    tie(succ, height) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::copyTexSubImage2D height = %{public}u", height);
    glCopyTexSubImage2D(static_cast<GLenum>(target), static_cast<GLint>(level), static_cast<GLint>(xoffset),
        static_cast<GLint>(yoffset), static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLsizei>(width),
        static_cast<GLsizei>(height));
    LOGI("WebGL copyTexSubImage2D end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::CreateBuffer(napi_env env, napi_callback_info info)
{
    LOGI("WebGL createBuffer start");
    napi_value objBuffer = NClass::InstantiateClass(env, WebGLBuffer::className, {});
    if (!objBuffer) {
        return nullptr;
    }
    auto webGlBuffer = NClass::GetEntityOf<WebGLBuffer>(env, objBuffer);
    if (!webGlBuffer) {
        return nullptr;
    }
    unsigned int bufferId;
    glGenBuffers(1, &bufferId);
    webGlBuffer->SetBuffer(bufferId);
    LOGI("WebGL WebGLRenderingContextBase::createBuffer bufferId = %{public}u", bufferId);
    LOGI("WebGL createBuffer end");
    return objBuffer;
}

napi_value WebGLRenderingContextBase::CreateFramebuffer(napi_env env, napi_callback_info info)
{
    LOGI("WebGL createFramebuffer start");
    napi_value objFramebuffer = NClass::InstantiateClass(env, WebGLFramebuffer::className, {});
    if (!objFramebuffer) {
        return nullptr;
    }
    auto webGlFramebuffer = NClass::GetEntityOf<WebGLFramebuffer>(env, objFramebuffer);
    if (!webGlFramebuffer) {
        return nullptr;
    }
    unsigned int framebufferId;
    glGenFramebuffers(1, &framebufferId);
    webGlFramebuffer->SetFramebuffer(framebufferId);
    LOGI("WebGL WebGLRenderingContextBase::createFramebuffer framebufferId = %{public}u", framebufferId);
    LOGI("WebGL createFramebuffer end");
    return objFramebuffer;
}

napi_value WebGLRenderingContextBase::CreateProgram(napi_env env, napi_callback_info info)
{
    LOGI("WebGL createProgram start");
    napi_value objProgram = NClass::InstantiateClass(env, WebGLProgram::className, {});
    if (!objProgram) {
        return nullptr;
    }
    auto webGlProgram = NClass::GetEntityOf<WebGLProgram>(env, objProgram);
    if (!webGlProgram) {
        return nullptr;
    }
    int programId = static_cast<int>(glCreateProgram());
    LOGI("WebGL WebGLRenderingContextBase::createProgram programId = %{public}u", programId);
    webGlProgram->SetProgramId(programId);
    LOGI("WebGL createProgram end");
    return objProgram;
}

napi_value WebGLRenderingContextBase::CreateRenderbuffer(napi_env env, napi_callback_info info)
{
    LOGI("WebGL createRenderbuffer start");
    napi_value objRenderbuffer = NClass::InstantiateClass(env, WebGLRenderbuffer::className, {});
    if (!objRenderbuffer) {
        return nullptr;
    }
    auto webGlRenderbuffer = NClass::GetEntityOf<WebGLRenderbuffer>(env, objRenderbuffer);
    if (!webGlRenderbuffer) {
        return nullptr;
    }
    unsigned int renderbufferId;
    glGenRenderbuffers(1, &renderbufferId);
    webGlRenderbuffer->SetRenderbuffer(renderbufferId);
    LOGI("WebGL WebGLRenderingContextBase::createRenderbuffer renderbufferId = %{public}u", renderbufferId);
    LOGI("WebGL createRenderbuffer end");
    return objRenderbuffer;
}

napi_value WebGLRenderingContextBase::CreateShader(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL createShader start");
    int64_t type;
    tie(succ, type) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    LOGI("WebGL WebGLRenderingContextBase::createShader type = %{public}u", type);

    napi_value objShader = NClass::InstantiateClass(env, WebGLShader::className, {});
    if (!objShader) {
        return nullptr;
    }
    auto webGlShader = NClass::GetEntityOf<WebGLShader>(env, objShader);
    if (!webGlShader) {
        return nullptr;
    }
    int shaderId = static_cast<int>(glCreateShader(static_cast<GLenum>(type)));
    webGlShader->SetShaderId(shaderId);
    LOGI("WebGL WebGLRenderingContextBase::createShader shaderId = %{public}u", shaderId);
    LOGI("WebGL createShader end");
    return objShader;
}

napi_value WebGLRenderingContextBase::CreateTexture(napi_env env, napi_callback_info info)
{
    LOGI("WebGL createTexture start");
    napi_value objTexture = NClass::InstantiateClass(env, WebGLTexture::className, {});
    if (!objTexture) {
        return nullptr;
    }
    auto webGlTexture = NClass::GetEntityOf<WebGLTexture>(env, objTexture);
    if (!webGlTexture) {
        return nullptr;
    }
    unsigned int textureId;
    glGenTextures(1, &textureId);
    webGlTexture->SetTexture(textureId);
    LOGI("WebGL WebGLRenderingContextBase::createTexture textureId = %{public}u", textureId);
    LOGI("WebGL createTexture end");
    return objTexture;
}

napi_value WebGLRenderingContextBase::CullFace(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL cullFace start");
    int64_t mode;
    tie(succ, mode) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::cullFace mode = %{public}u", mode);
    glCullFace(static_cast<GLenum>(mode));
    LOGI("WebGL cullFace end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::DeleteBuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    LOGI("WebGL deleteBuffer start");
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }

    WebGLBuffer *webGlBuffer = nullptr;
    napi_status bufferStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlBuffer);
    if (bufferStatus != napi_ok) {
        return nullptr;
    }
    unsigned int buffer = webGlBuffer->GetBuffer();

    glDeleteBuffers(1, &buffer);
    LOGI("WebGL deleteBuffer end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::DeleteFramebuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    LOGI("WebGL deleteFramebuffer start");
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }

    WebGLFramebuffer *webGlFramebuffer = nullptr;
    napi_status framebufferStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlFramebuffer);
    if (framebufferStatus != napi_ok) {
        return nullptr;
    }
    unsigned int framebuffer = webGlFramebuffer->GetFramebuffer();
    glDeleteFramebuffers(1, &framebuffer);
    LOGI("WebGL deleteFramebuffer end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::DeleteProgram(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    LOGI("WebGL deleteProgram start");
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }

    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    int programId = webGlProgram->GetProgramId();

    glDeleteProgram(static_cast<GLuint>(programId));
    LOGI("WebGL deleteProgram end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::DeleteRenderbuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    LOGI("WebGL deleteRenderbuffer start");
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }

    WebGLRenderbuffer *webGlRenderbuffer = nullptr;
    napi_status renderbufferStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlRenderbuffer);
    if (renderbufferStatus != napi_ok) {
        return nullptr;
    }
    unsigned int renderbuffer = webGlRenderbuffer->GetRenderbuffer();

    glDeleteRenderbuffers(1, &renderbuffer);
    LOGI("WebGL deleteRenderbuffer end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::DeleteShader(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    LOGI("WebGL deleteShader start");
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }

    WebGLShader *webGlShader = nullptr;
    napi_status shaderStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlShader);
    if (shaderStatus != napi_ok) {
        return nullptr;
    }
    int shaderId = webGlShader->GetShaderId();

    glDeleteShader(static_cast<GLuint>(shaderId));
    LOGI("WebGL deleteShader end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::DeleteTexture(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    LOGI("WebGL deleteTexture start");
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }

    WebGLTexture *webGlTexture = nullptr;
    napi_status textureStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlTexture);
    if (textureStatus != napi_ok) {
        return nullptr;
    }
    unsigned int texture = webGlTexture->GetTexture();

    glDeleteTextures(1, &texture);
    LOGI("WebGL deleteTexture end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::DepthFunc(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL depthFunc start");
    int64_t func;
    tie(succ, func) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::depthFunc func = %{public}u", func);
    glDepthFunc(static_cast<GLenum>(func));
    LOGI("WebGL depthFunc end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::DepthMask(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL depthMask start");
    bool flag = false;
    tie(succ, flag) = NVal(env, funcArg[NARG_POS::FIRST]).ToBool();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::depthMask flag = %{public}u", flag);
    glDepthMask(static_cast<GLboolean>(flag));
    LOGI("WebGL depthMask end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::DepthRange(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL depthRange start");
    double zNear;
    tie(succ, zNear) = NVal(env, funcArg[NARG_POS::FIRST]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    double zFar;
    tie(succ, zFar) = NVal(env, funcArg[NARG_POS::SECOND]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    glDepthRangef(static_cast<GLclampf>((float) zNear), static_cast<GLclampf>((float) zFar));
    LOGI("WebGL depthRange end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::DetachShader(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr || funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    LOGI("WebGL detachShader start");
    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    int programId = webGlProgram->GetProgramId();

    WebGLShader *webGlShader = nullptr;
    napi_status shaderStatus = napi_unwrap(env, funcArg[NARG_POS::SECOND], (void **) &webGlShader);
    if (shaderStatus != napi_ok) {
        return nullptr;
    }
    int shaderId = webGlShader->GetShaderId();

    glDetachShader(static_cast<GLuint>(programId), static_cast<GLuint>(shaderId));
    LOGI("WebGL detachShader end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::Disable(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL disable start");
    int64_t cap;
    tie(succ, cap) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::disable cap = %{public}u", cap);
    glDisable(static_cast<GLenum>(cap));
    LOGI("WebGL disable end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::DisableVertexAttribArray(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL disableVertexAttribArray start");
    int64_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::disableVertexAttribArray index = %{public}u", index);
    glDisableVertexAttribArray(static_cast<GLuint>(index));
    LOGI("WebGL disableVertexAttribArray end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::DrawArrays(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL drawArrays start");
    int64_t mode;
    tie(succ, mode) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::drawArrays mode = %{public}u", mode);
    int64_t first;
    tie(succ, first) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::drawArrays first = %{public}u", first);
    int64_t count;
    tie(succ, count) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::drawArrays count = %{public}u", count);
    glDrawArrays(static_cast<GLenum>(mode), static_cast<GLint>(first), static_cast<GLsizei>(count));
    LOGI("WebGL drawArrays end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::DrawElements(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL drawElements start");
    int64_t mode;
    tie(succ, mode) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::drawElements mode = %{public}u", mode);
    int64_t count;
    tie(succ, count) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::drawElements count = %{public}u", count);
    int64_t type;
    tie(succ, type) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::drawElements type = %{public}u", type);
    int64_t offset;
    tie(succ, offset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::drawElements offset = %{public}u", offset);
    glDrawElements(static_cast<GLenum>(mode), static_cast<GLsizei>(count), static_cast<GLenum>(type),
        reinterpret_cast<GLvoid *>(static_cast<intptr_t>(offset)));
    LOGI("WebGL drawElements end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::Enable(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL enable start");
    int64_t cap;
    tie(succ, cap) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::enable cap = %{public}u", cap);
    glEnable(static_cast<GLenum>(cap));
    LOGI("WebGL enable end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::EnableVertexAttribArray(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL enableVertexAttribArray start");
    int64_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::enableVertexAttribArray index = %{public}u", index);
    glEnableVertexAttribArray(static_cast<GLuint>(index));
    LOGI("WebGL enableVertexAttribArray end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::Finish(napi_env env, napi_callback_info info)
{
    LOGI("WebGL finish start");
    glFinish();
    LOGI("WebGL finish end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::Flush(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return nullptr;
    }
    LOGI("WebGL flush start");
    WebGLRenderingContextBasicBase *obj = Util::GetContextObject(env, funcArg.GetThisVar(), "webgl");
    if (obj == nullptr) {
        LOGE("WebGL flush obj == nullptr");
        return nullptr;
    }
    glFlush();
    obj->Update();
    LOGI("WebGL flush end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::FramebufferRenderbuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL framebufferRenderbuffer start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::framebufferRenderbuffer target = %{public}u", target);

    int64_t attachment;
    tie(succ, attachment) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::framebufferRenderbuffer attachment = %{public}u", attachment);

    int64_t renderBufferTarget;
    tie(succ, renderBufferTarget) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }

    if (funcArg[NARG_POS::FOURTH] == nullptr) {
        return nullptr;
    }
    WebGLRenderbuffer *webGlRenderbuffer = nullptr;
    napi_status renderbufferStatus = napi_unwrap(env, funcArg[NARG_POS::FOURTH], (void **) &webGlRenderbuffer);
    if (renderbufferStatus != napi_ok) {
        return nullptr;
    }
    unsigned int renderbuffer = webGlRenderbuffer->GetRenderbuffer();
    glFramebufferRenderbuffer(static_cast<GLenum>(target), static_cast<GLenum>(attachment),
        static_cast<GLenum>(renderBufferTarget), static_cast<GLuint>(renderbuffer));
    LOGI("WebGL framebufferRenderbuffer end");
    return nullptr;
}


napi_value WebGLRenderingContextBase::FramebufferTexture2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL framebufferTexture2D start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::framebufferTexture2D target = %{public}u", target);

    int64_t attachment;
    tie(succ, attachment) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::framebufferTexture2D attachment = %{public}u", attachment);

    int64_t textarget;
    tie(succ, textarget) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::framebufferTexture2D textarget = %{public}u", textarget);

    if (funcArg[NARG_POS::FOURTH] == nullptr) {
        return nullptr;
    }
    WebGLTexture *webGlTexture = nullptr;
    napi_status textureStatus = napi_unwrap(env, funcArg[NARG_POS::FOURTH], (void **) &webGlTexture);
    if (textureStatus != napi_ok) {
        return nullptr;
    }
    unsigned int texture = webGlTexture->GetTexture();

    int64_t level;
    tie(succ, level) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::framebufferTexture2D level = %{public}u", level);
    glFramebufferTexture2D(static_cast<GLenum>(target), static_cast<GLenum>(attachment),
        static_cast<GLenum>(textarget), static_cast<GLuint>(texture), static_cast<GLint>(level));
    LOGI("WebGL framebufferTexture2D end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::GetUniformLocation(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL getUniformLocation start");
    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    int programId = webGlProgram->GetProgramId();
    LOGI("WebGL WebGLRenderContext::getUniformLocation programId = %{public}u", programId);
    unique_ptr<char[]> name;
    tie(succ, name, ignore) = NVal(env, funcArg[NARG_POS::SECOND]).ToUTF8String();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::getUniformLocation name = %{public}s", name.get());

    napi_value objUniformLocation = NClass::InstantiateClass(env, WebGLUniformLocation::className, {});
    if (!objUniformLocation) {
        return nullptr;
    }
    auto webGLUniformLocation = NClass::GetEntityOf<WebGLUniformLocation>(env, objUniformLocation);
    if (!webGLUniformLocation) {
        return nullptr;
    }
    int64_t locationId = static_cast<int64_t>(glGetUniformLocation(static_cast<GLuint>(programId),
        const_cast<char *>(name.get())));
    LOGI("WebGL WebGLRenderingContextBase::getUniformLocation locationId = %{public}u", locationId);
    webGLUniformLocation->SetUniformLocationId(locationId);
    LOGI("WebGL getUniformLocation end");
    return objUniformLocation;
}

napi_value WebGLRenderingContextBase::GetVertexAttribOffset(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL getVertexAttribOffset start");
    int32_t pnameindex;
    tie(succ, pnameindex) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::getVertexAttribOffset index = %{public}u", pnameindex);
    int32_t pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::getVertexAttribOffset pname = %{public}u", pname);
    GLvoid *point = nullptr;
    glGetVertexAttribPointerv(static_cast<GLint>(pnameindex), static_cast<GLint>(pname), &point);
    LOGI("WebGL getVertexAttribOffset end");
    int po = (double) (intptr_t) point;
    return NVal::CreateBool(env, po).val_;
}

napi_value WebGLRenderingContextBase::ShaderSource(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL shaderSource start");
    WebGLShader *webGlShader = nullptr;
    napi_status shaderStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlShader);
    if (shaderStatus != napi_ok) {
        return nullptr;
    }
    int shader = webGlShader->GetShaderId();
    LOGI("WebGL WebGLRenderContext::shaderSource shader = %{public}u", shader);
    unique_ptr<char[]> source;
    tie(succ, source, ignore) = NVal(env, funcArg[NARG_POS::SECOND]).ToUTF8String();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::shaderSource source = %{public}s", source.get());
    const char *shaderString = source.get();
    glShaderSource(static_cast<GLuint>(shader), 1, &shaderString, 0);
    string res = shaderString;
    auto& objects = ObjectSource::GetInstance().GetObjectMap();
    objects.insert({shader, res});
    LOGI("WebGL shaderSource end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::Hint(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL hint start");
    int32_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::hint target = %{public}u", target);
    int32_t mode;
    tie(succ, mode) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::hint mode = %{public}u", mode);
    glHint(static_cast<GLenum>(target), static_cast<GLenum>(mode));
    LOGI("WebGL hint end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::IsBuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    bool res = false;
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, res).val_;
    }
    LOGI("WebGL isBuffer start");
    WebGLBuffer *webGlBuffer = nullptr;
    napi_status bufferStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlBuffer);
    if (bufferStatus != napi_ok) {
        return NVal::CreateBool(env, res).val_;
    }
    unsigned int buffer = webGlBuffer->GetBuffer();
    res = static_cast<bool>(glIsBuffer(static_cast<GLuint>(buffer)));
    LOGI("WebGL WebGLRenderContext::isBuffer res = %{public}u", res);
    LOGI("WebGL isBuffer end");
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGLRenderingContextBase::IsEnabled(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    bool res = false;
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, res).val_;
    }
    bool succ = false;
    LOGI("WebGL isEnabled start");
    int64_t cap;
    tie(succ, cap) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return NVal::CreateBool(env, res).val_;
    }
    LOGI("WebGL WebGLRenderContext::isEnabled cap = %{public}u", cap);
    res = static_cast<bool>(glIsEnabled(static_cast<GLenum>(cap)));
    LOGI("WebGL isEnabled end");
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGLRenderingContextBase::RenderbufferStorage(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL renderbufferStorage start");
    int32_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::renderbufferStorage target = %{public}u", target);
    int32_t internalformat;
    tie(succ, internalformat) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::renderbufferStorage internalformat = %{public}u", internalformat);
    int32_t width;
    tie(succ, width) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::renderbufferStorage width = %{public}u", width);
    int32_t height;
    tie(succ, height) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::renderbufferStorage height = %{public}u", height);
    glRenderbufferStorage(static_cast<GLenum>(target), static_cast<GLenum>(internalformat),
                          static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    LOGI("WebGL renderbufferStorage end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::SampleCoverage(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL sampleCoverage start");
    double value;
    tie(succ, value) = NVal(env, funcArg[NARG_POS::FIRST]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::sampleCoverage value = %{public}f", value);
    bool invert = false;
    tie(succ, invert) = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::sampleCoverage invert = %{public}u", invert);
    glSampleCoverage(static_cast<GLclampf>((float) value), static_cast<GLboolean>(invert));
    LOGI("WebGL sampleCoverage end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::Scissor(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL scissor start");
    int32_t x;
    tie(succ, x) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::scissor x = %{public}u", x);
    int32_t y;
    tie(succ, y) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::scissor y = %{public}u", y);
    int32_t width;
    tie(succ, width) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::scissor width = %{public}u", width);
    int32_t height;
    tie(succ, height) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::scissor height = %{public}u", height);
    glScissor(static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLsizei>(width),
              static_cast<GLsizei>(height));
    LOGI("WebGL scissor end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::StencilFunc(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL stencilFunc start");
    int32_t func;
    tie(succ, func) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::stencilFunc func = %{public}u", func);
    int32_t ref;
    tie(succ, ref) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::stencilFunc ref = %{public}u", ref);
    int32_t mask;
    tie(succ, mask) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::stencilFunc mask = %{public}u", mask);
    glStencilFunc(static_cast<GLenum>(func), static_cast<GLint>(ref), static_cast<GLuint>(mask));
    LOGI("WebGL stencilFunc end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::StencilFuncSeparate(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL stencilFuncSeparate start");
    int32_t face;
    tie(succ, face) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::stencilFuncSeparate face = %{public}u", face);
    int32_t func;
    tie(succ, func) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::stencilFuncSeparate func = %{public}u", func);
    int32_t ref;
    tie(succ, ref) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::stencilFuncSeparate ref = %{public}u", ref);
    int32_t mask;
    tie(succ, mask) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::stencilFuncSeparate mask = %{public}u", mask);
    glStencilFuncSeparate(static_cast<GLenum>(face), static_cast<GLenum>(func), static_cast<GLint>(ref),
                          static_cast<GLuint>(mask));
    LOGI("WebGL stencilFuncSeparate end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::StencilMask(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL stencilMask start");
    int32_t mask;
    tie(succ, mask) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::stencilMask mask = %{public}u", mask);
    glStencilMask(static_cast<GLuint>(mask));
    LOGI("WebGL stencilMask end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::StencilMaskSeparate(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL stencilMaskSeparate start");
    int32_t face;
    tie(succ, face) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::stencilMaskSeparate face = %{public}u", face);
    int32_t mask;
    tie(succ, mask) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::stencilMaskSeparate mask = %{public}u", mask);
    glStencilMaskSeparate(static_cast<GLenum>(face), static_cast<GLuint>(mask));
    LOGI("WebGL stencilMaskSeparate end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::StencilOp(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL stencilOp start");
    int32_t fail;
    tie(succ, fail) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::stencilOp fail = %{public}u", fail);
    int32_t zfail;
    tie(succ, zfail) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::stencilOp zfail = %{public}u", zfail);
    int32_t zpass;
    tie(succ, zpass) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::stencilOp zpass = %{public}u", zpass);
    glStencilOp(static_cast<GLenum>(fail), static_cast<GLenum>(zfail), static_cast<GLenum>(zpass));
    LOGI("WebGL stencilOp end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::StencilOpSeparate(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL stencilOpSeparate start");
    int32_t face;
    tie(succ, face) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::stencilOpSeparate face = %{public}u", face);
    int32_t sfail;
    tie(succ, sfail) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::stencilOpSeparate sfail = %{public}u", sfail);
    int32_t dpfail;
    tie(succ, dpfail) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::stencilOpSeparate dpfail = %{public}u", dpfail);
    int32_t dppass;
    tie(succ, dppass) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::stencilOpSeparate dppass = %{public}u", dppass);
    glStencilOpSeparate(static_cast<GLenum>(face), static_cast<GLenum>(sfail), static_cast<GLenum>(dpfail),
                        static_cast<GLenum>(dppass));
    LOGI("WebGL stencilOpSeparate end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::TexParameterf(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL texParameterf start");
    int32_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::texParameterf target = %{public}u", target);
    int32_t pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::texParameterf pname = %{public}u", pname);
    double param;
    tie(succ, param) = NVal(env, funcArg[NARG_POS::THIRD]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::texParameterf param = %{public}f", param);
    glTexParameterf(static_cast<GLenum>(target), static_cast<GLenum>(pname), static_cast<GLfloat>((float) param));
    LOGI("WebGL texParameterf end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::TexParameteri(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL texParameteri start");
    int32_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::texParameteri target = %{public}u", target);
    int32_t pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::texParameteri pname = %{public}u", pname);
    int32_t param;
    tie(succ, param) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::texParameteri param = %{public}u", param);
    glTexParameteri(static_cast<GLenum>(target), static_cast<GLenum>(pname), static_cast<GLint>(param));
    LOGI("WebGL texParameteri end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::Uniform1f(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL uniform1f start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    double v0;
    tie(succ, v0) = NVal(env, funcArg[NARG_POS::SECOND]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::uniform1f v0 = %{public}f", v0);
    glUniform1f(static_cast<GLint>(location), static_cast<GLfloat>((float) v0));
    LOGI("WebGL uniform1f end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::Uniform2f(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL uniform2f start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    double v0;
    tie(succ, v0) = NVal(env, funcArg[NARG_POS::SECOND]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::uniform2f v0 = %{public}f", v0);
    double v1;
    tie(succ, v1) = NVal(env, funcArg[NARG_POS::THIRD]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::uniform2f v1 = %{public}f", v1);
    glUniform2f(static_cast<GLint>(location), static_cast<GLfloat>((float) v0), static_cast<GLfloat>((float) v1));
    LOGI("WebGL uniform2f end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::Uniform3f(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL uniform3f start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    double v0;
    tie(succ, v0) = NVal(env, funcArg[NARG_POS::SECOND]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::uniform3f v0 = %{public}f", v0);
    double v1;
    tie(succ, v1) = NVal(env, funcArg[NARG_POS::THIRD]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::uniform3f v1 = %{public}f", v1);
    double v2;
    tie(succ, v2) = NVal(env, funcArg[NARG_POS::FOURTH]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::uniform3f v2 = %{public}f", v2);
    glUniform3f(static_cast<GLint>(location), static_cast<GLfloat>((float) v0), static_cast<GLfloat>((float) v1),
                static_cast<GLfloat>((float) v2));
    LOGI("WebGL uniform3f end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::Uniform4f(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL uniform4f start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    double v0;
    tie(succ, v0) = NVal(env, funcArg[NARG_POS::SECOND]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::uniform4f v0 = %{public}f", v0);
    double v1;
    tie(succ, v1) = NVal(env, funcArg[NARG_POS::THIRD]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::uniform4f v1 = %{public}f", v1);
    double v2;
    tie(succ, v2) = NVal(env, funcArg[NARG_POS::FOURTH]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::uniform4f v2 = %{public}f", v2);
    double v3;
    tie(succ, v3) = NVal(env, funcArg[NARG_POS::FIFTH]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::uniform4f v3 = %{public}f", v3);
    glUniform4f(static_cast<GLint>(location), static_cast<GLfloat>((float) v0), static_cast<GLfloat>((float) v1),
                static_cast<GLfloat>((float) v2), static_cast<GLfloat>((float) v3));
    LOGI("WebGL uniform4f end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::Uniform1i(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL uniform1i start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    int32_t v0;
    tie(succ, v0) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::uniform1i v0 = %{public}u", v0);
    glUniform1i(static_cast<GLint>(location), static_cast<GLint>(v0));
    LOGI("WebGL uniform1i end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::Uniform2i(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL uniform2i start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    int32_t v0;
    tie(succ, v0) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::uniform2i v0 = %{public}u", v0);
    int32_t v1;
    tie(succ, v1) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::uniform2i v1 = %{public}u", v1);
    glUniform2i(static_cast<GLint>(location), static_cast<GLint>(v0), static_cast<GLint>(v1));
    LOGI("WebGL uniform2i end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::Uniform3i(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL uniform3i start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    int32_t v0;
    tie(succ, v0) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::uniform3i v0 = %{public}u", v0);
    int32_t v1;
    tie(succ, v1) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::uniform3i v1 = %{public}u", v1);
    int32_t v2;
    tie(succ, v2) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::uniform3i v2 = %{public}u", v2);
    glUniform3i(static_cast<GLint>(location), static_cast<GLint>(v0), static_cast<GLint>(v1),
                static_cast<GLint>(v2));
    LOGI("WebGL uniform3i end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::Uniform4i(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL uniform4i start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    int32_t v0;
    tie(succ, v0) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::uniform4i v0 = %{public}u", v0);
    int32_t v1;
    tie(succ, v1) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::uniform4i v1 = %{public}u", v1);
    int32_t v2;
    tie(succ, v2) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::uniform4i v2 = %{public}u", v2);
    int32_t v3;
    tie(succ, v3) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::uniform4i v3 = %{public}u", v3);
    glUniform4i(static_cast<GLint>(location), static_cast<GLint>(v0), static_cast<GLint>(v1),
                static_cast<GLint>(v2), static_cast<GLint>(v3));
    LOGI("WebGL uniform4i end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::UseProgram(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    LOGI("WebGL useProgram start");
    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    int program = webGlProgram->GetProgramId();
    glUseProgram(static_cast<GLuint>(program));
    LOGI("WebGL useProgram end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::ValidateProgram(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    LOGI("WebGL validateProgram start");
    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    int program = webGlProgram->GetProgramId();
    LOGI("WebGL WebGLRenderContext::validateProgram program = %{public}u", program);
    glValidateProgram(static_cast<GLuint>(program));
    LOGI("WebGL validateProgram end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::VertexAttrib1f(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL vertexAttrib1f start");
    int32_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::vertexAttrib1f index = %{public}u", index);
    double v0;
    tie(succ, v0) = NVal(env, funcArg[NARG_POS::SECOND]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::vertexAttrib1f v0 = %{public}f", v0);
    glVertexAttrib1f(static_cast<GLuint>(index), static_cast<GLfloat>((float) v0));
    LOGI("WebGL vertexAttrib1f end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::VertexAttrib2f(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL vertexAttrib2f start");
    int32_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::vertexAttrib2f index = %{public}u", index);
    double x;
    tie(succ, x) = NVal(env, funcArg[NARG_POS::SECOND]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::vertexAttrib2f x = %{public}f", x);
    double y;
    tie(succ, y) = NVal(env, funcArg[NARG_POS::THIRD]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::vertexAttrib2f y = %{public}f", y);
    glVertexAttrib2f(static_cast<GLint>(index), static_cast<GLfloat>((float) x), static_cast<GLfloat>((float) y));
    LOGI("WebGL vertexAttrib2f end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::VertexAttrib3f(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL vertexAttrib3f start");
    int32_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::vertexAttrib3f index = %{public}u", index);
    double x;
    tie(succ, x) = NVal(env, funcArg[NARG_POS::SECOND]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::vertexAttrib3f x = %{public}f", x);
    double y;
    tie(succ, y) = NVal(env, funcArg[NARG_POS::THIRD]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::vertexAttrib3f y = %{public}f", y);
    double z;
    tie(succ, z) = NVal(env, funcArg[NARG_POS::FOURTH]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::vertexAttrib3f z = %{public}f", z);
    glVertexAttrib3f(static_cast<GLuint>(index), static_cast<GLfloat>((float) x), static_cast<GLfloat>((float) y),
                     static_cast<GLfloat>(z));
    LOGI("WebGL vertexAttrib3f end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::VertexAttrib4f(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL vertexAttrib4f start");
    int32_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::vertexAttrib4f index = %{public}u", index);
    double x;
    tie(succ, x) = NVal(env, funcArg[NARG_POS::SECOND]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::vertexAttrib4f x = %{public}f", x);
    double y;
    tie(succ, y) = NVal(env, funcArg[NARG_POS::THIRD]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::vertexAttrib4f y = %{public}f", y);
    double z;
    tie(succ, z) = NVal(env, funcArg[NARG_POS::FOURTH]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::vertexAttrib4f z = %{public}f", z);
    double w;
    tie(succ, w) = NVal(env, funcArg[NARG_POS::FIFTH]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::vertexAttrib4f w = %{public}f", w);
    glVertexAttrib4f(static_cast<GLuint>(index), static_cast<GLfloat>((float) x), static_cast<GLfloat>((float) y),
                     static_cast<GLfloat>((float) z), static_cast<GLfloat>((float) w));
    LOGI("WebGL vertexAttrib4f end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::VertexAttribPointer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::SIX)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL vertexAttribPointer start");
    int32_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::vertexAttribPointer index = %{public}u", index);
    int32_t size;
    tie(succ, size) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::vertexAttribPointer size = %{public}u", size);
    int32_t type;
    tie(succ, type) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::vertexAttribPointer type = %{public}u", type);
    bool normalized = false;
    tie(succ, normalized) = NVal(env, funcArg[NARG_POS::FOURTH]).ToBool();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::vertexAttribPointer normalized = %{public}u", normalized);
    int32_t stride;
    tie(succ, stride) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::vertexAttribPointer stride = %{public}u", stride);
    int32_t offset;
    tie(succ, offset) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::vertexAttribPointer offset = %{public}u", offset);
    glVertexAttribPointer(static_cast<GLuint>(index), static_cast<GLint>(size), static_cast<GLenum>(type),
                          static_cast<GLboolean>(normalized), static_cast<GLsizei>(stride),
                          reinterpret_cast<GLvoid *>(static_cast<intptr_t>(offset)));
    LOGI("WebGL vertexAttribPointer end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::Viewport(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("webgl viewport start");
    int32_t x;
    tie(succ, x) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("webgl WebGLRenderContext::viewport x = %{public}u", x);
    int32_t y;
    tie(succ, y) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("webgl WebGLRenderContext::viewport y = %{public}u", y);
    int32_t width;
    tie(succ, width) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("webgl WebGLRenderContext::viewport width = %{public}u", width);
    int32_t height;
    tie(succ, height) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("webgl WebGLRenderContext::viewport height = %{public}u", height);
    glViewport(static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLsizei>(width),
               static_cast<GLsizei>(height));
    LOGI("webgl viewport end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::IsFramebuffer(napi_env env, napi_callback_info info)
{
    LOGI("WebGL isFramebuffer into one");
    NFuncArg funcArg(env, info);
    bool res = false;
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, res).val_;
    }

    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return NVal::CreateBool(env, res).val_;
    }
    LOGI("WebGL isFramebuffer start");
    WebGLFramebuffer *webGLFramebuffer = nullptr;
    napi_status framebufferStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLFramebuffer);
    if (framebufferStatus != napi_ok) {
        return NVal::CreateBool(env, res).val_;
    }
    unsigned int framebuffer = webGLFramebuffer->GetFramebuffer();
    LOGI("WebGL WebGLRenderContext::isFramebuffer framebuffer = %{public}u", framebuffer);
    res = static_cast<bool>(glIsFramebuffer(static_cast<GLuint>(framebuffer)));
    LOGI("WebGL isFramebuffer end");
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGLRenderingContextBase::IsProgram(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    bool res = false;
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, res).val_;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return NVal::CreateBool(env, res).val_;
    }
    LOGI("WebGL isProgram start");
    WebGLProgram *webGLProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLProgram);
    if (programStatus != napi_ok) {
        return NVal::CreateBool(env, res).val_;
    }
    unsigned int program = webGLProgram->GetProgramId();
    LOGI("WebGL WebGLRenderContext::isProgram program = %{public}u", program);
    res = static_cast<bool>(glIsProgram(static_cast<GLuint>(program)));
    LOGI("WebGL isProgram end");
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGLRenderingContextBase::IsRenderbuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    bool res = false;
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, res).val_;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return NVal::CreateBool(env, res).val_;
    }
    LOGI("WebGL isRenderbuffer start");
    WebGLRenderbuffer *webGLRenderbuffer = nullptr;
    napi_status renderbufferStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLRenderbuffer);
    if (renderbufferStatus != napi_ok) {
        return NVal::CreateBool(env, res).val_;
    }
    unsigned int renderbuffer = webGLRenderbuffer->GetRenderbuffer();
    LOGI("WebGL WebGLRenderContext::isRenderbuffer renderbuffer = %{public}u", renderbuffer);
    res = static_cast<bool>(glIsRenderbuffer(static_cast<GLuint>(renderbuffer)));
    LOGI("WebGL isRenderbuffer end");
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGLRenderingContextBase::IsShader(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    bool res = false;
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, res).val_;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return NVal::CreateBool(env, res).val_;
    }
    LOGI("WebGL isShader start");
    WebGLShader *webGLShader = nullptr;
    napi_status shaderStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLShader);
    if (shaderStatus != napi_ok) {
        return NVal::CreateBool(env, res).val_;
    }
    unsigned int shader = webGLShader->GetShaderId();
    LOGI("WebGL WebGLRenderContext::isShader shader = %{public}u", shader);
    res = static_cast<bool>(glIsShader(static_cast<GLuint>(shader)));
    LOGI("WebGL isShader end");
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGLRenderingContextBase::IsTexture(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    bool res = false;
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, res).val_;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return NVal::CreateBool(env, res).val_;
    }
    LOGI("WebGL isTexture start");
    WebGLTexture *webGLTexture = nullptr;
    napi_status textureStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLTexture);
    if (textureStatus != napi_ok) {
        return NVal::CreateBool(env, res).val_;
    }
    unsigned int texture = webGLTexture->GetTexture();
    LOGI("WebGL WebGLRenderContext::isTexture texture = %{public}u", texture);
    res = static_cast<bool>(glIsTexture(static_cast<GLuint>(texture)));
    LOGI("WebGL isTexture end");
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGLRenderingContextBase::LineWidth(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL lineWidth start");
    double linewidth;
    tie(succ, linewidth) = NVal(env, funcArg[NARG_POS::FIRST]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::lineWidth linewidth = %{public}f", linewidth);
    glLineWidth(static_cast<GLfloat>((float) linewidth));
    LOGI("WebGL lineWidth end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::LinkProgram(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    LOGI("WebGL linkProgram start");
    WebGLProgram *webGLProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    unsigned int linkprogram = webGLProgram->GetProgramId();
    LOGI("WebGL WebGLRenderContext::linkProgram linkprogram = %{public}u", linkprogram);
    glLinkProgram(static_cast<GLuint>(linkprogram));
    LOGI("WebGL linkProgram end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::PixelStorei(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL pixelStorei start");
    int32_t pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::pixelStorei pname = %{public}u", pname);
    bool parambool = false;
    tie(succ, parambool) = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (succ) {
        return nullptr;
    }
    int32_t param;
    tie(succ, param) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::pixelStorei pname = %{public}u", param);
    glPixelStorei(static_cast<GLenum>(pname), static_cast<GLint>(param));
    LOGI("WebGL pixelStorei end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::PolygonOffset(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL polygonOffset start");
    double factor;
    tie(succ, factor) = NVal(env, funcArg[NARG_POS::FIRST]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::polygonOffset factor = %{public}f", factor);
    double units;
    tie(succ, units) = NVal(env, funcArg[NARG_POS::SECOND]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::polygonOffset units = %{public}f", units);
    glPolygonOffset(static_cast<GLfloat>((float) factor), static_cast<GLfloat>((float) units));
    LOGI("WebGL polygonOffset end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::FrontFace(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL frontFace start");
    int64_t mode;
    tie(succ, mode) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::frontFace mode = %{public}u", mode);
    glFrontFace(static_cast<GLenum>(mode));
    LOGI("WebGL frontFace end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::GenerateMipmap(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL generateMipmap start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::generateMipmap target = %{public}u", target);
    glGenerateMipmap(static_cast<GLenum>(target));
    LOGI("WebGL generateMipmap end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::GetActiveAttrib(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL getActiveAttrib start");
    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGlProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    int programId = webGlProgram->GetProgramId();
    LOGI("WebGL WebGLRenderContext::getActiveAttrib programId = %{public}u", programId);
    int64_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::getActiveAttrib index = %{public}u", index);

    napi_value objActiveInfo = NClass::InstantiateClass(env, WebGLActiveInfo::className, {});
    if (!objActiveInfo) {
        return nullptr;
    }
    auto webGLActiveInfo = NClass::GetEntityOf<WebGLActiveInfo>(env, objActiveInfo);
    if (!webGLActiveInfo) {
        return nullptr;
    }
    int64_t bufSize = 0;
    GLsizei length;
    GLint size;
    GLenum type;
    char name;
    glGetActiveAttrib(static_cast<GLuint>(programId), static_cast<GLuint>(index),
        static_cast<GLsizei>(bufSize), &length, &size, &type, &name);
    webGLActiveInfo->SetActiveName(name);
    webGLActiveInfo->SetActiveType(type);
    webGLActiveInfo->SetActiveSize(size);
    LOGI("WebGL getActiveAttrib end");
    return objActiveInfo;
}

napi_value WebGLRenderingContextBase::GetActiveUniform(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL getActiveUniform start");
    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGlProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    int programId = webGlProgram->GetProgramId();
    LOGI("WebGL WebGLRenderContext::getActiveUniform programId = %{public}u", programId);
    int64_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::getActiveUniform index = %{public}u", index);

    napi_value objActiveInfo = NClass::InstantiateClass(env, WebGLActiveInfo::className, {});
    if (!objActiveInfo) {
        return nullptr;
    }
    auto webGLActiveInfo = NClass::GetEntityOf<WebGLActiveInfo>(env, objActiveInfo);
    if (!webGLActiveInfo) {
        return nullptr;
    }
    int64_t bufSize = 0;
    GLsizei length;
    GLint size;
    GLenum type;
    char name;
    glGetActiveUniform(static_cast<GLuint>(programId), static_cast<GLuint>(index),
        static_cast<GLsizei>(bufSize), &length, &size, &type, &name);
    webGLActiveInfo->SetActiveName(name);
    webGLActiveInfo->SetActiveType(type);
    webGLActiveInfo->SetActiveSize(size);
    LOGI("WebGL getActiveUniform end");
    return objActiveInfo;
}

napi_value WebGLRenderingContextBase::GetBufferParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL getBufferParameter start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::getBufferParameter target = %{public}u", target);
    int64_t pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::getBufferParameter pname = %{public}u", pname);
    if (pname == GL_BUFFER_SIZE || pname == GL_BUFFER_USAGE) {
        GLint params;
        glGetBufferParameteriv(static_cast<GLenum>(target), static_cast<GLenum>(pname), &params);
        int64_t res = static_cast<int64_t>(params);
        LOGI("WebGL getBufferParameter end");
        return NVal::CreateInt64(env, res).val_;
    } else {
        LOGI("WebGL getBufferParameter : pname is wrong");
        return nullptr;
    }
}

napi_value WebGLRenderingContextBase::GetError(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    LOGI("WebGL getError start");
    int64_t res = static_cast<int64_t>(glGetError());
    LOGI("WebGL WebGLRenderContext::getError res = %{public}u", res);
    LOGI("WebGL getError end");
    return NVal::CreateInt64(env, res).val_;
}

napi_value WebGLRenderingContextBase::GetFramebufferAttachmentParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL getFramebufferAttachmentParameter start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::getFramebufferAttachmentParameter target = %{public}u", target);
    int64_t attachment;
    tie(succ, attachment) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::getFramebufferAttachmentParameter attachment = %{public}u", attachment);
    int64_t pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::getFramebufferAttachmentParameter pname = %{public}u", pname);
    if (pname == FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE ||
        pname == FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL ||
        pname == FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE ||
        pname == WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE ||
        pname == WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_BLUE_SIZE ||
        pname == WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING ||
        pname == WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE ||
        pname == WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE ||
        pname == WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_GREEN_SIZE ||
        pname == WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_RED_SIZE ||
        pname == WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE ||
        pname == WebGL2RenderingContextBase::FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER) {
        GLint params;
        glGetFramebufferAttachmentParameteriv(static_cast<GLenum>(target), static_cast<GLenum>(attachment),
                                              static_cast<GLenum>(pname), &params);
        int64_t res = static_cast<int64_t>(params);
        LOGI("WebGL getFramebufferAttachmentParameter end");
        return NVal::CreateInt64(env, res).val_;
    } else if (pname == FRAMEBUFFER_ATTACHMENT_OBJECT_NAME) {
        GLint params;
        napi_value res = nullptr;
        napi_get_cb_info(env, info, nullptr, nullptr, &res, nullptr);
        glGetFramebufferAttachmentParameteriv (static_cast<GLenum>(target), static_cast<GLenum>(attachment),
                                               FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &params);
        int64_t type = static_cast<int64_t>(params);
        if (type == RENDERBUFFER) {
            LOGI("WebGL getFramebufferAttachmentParameter WebGLRenderbuffer start");
            napi_value objFramebuffer = NClass::InstantiateClass(env, WebGLFramebuffer::className, {});
            if (!objFramebuffer) {
                return nullptr;
            }
            auto webGlFramebuffer = NClass::GetEntityOf<WebGLFramebuffer>(env, objFramebuffer);
            if (!webGlFramebuffer) {
                return nullptr;
            }
            GLint framebufferId;
            glGetFramebufferAttachmentParameteriv(static_cast<GLenum>(target), static_cast<GLenum>(attachment),
                                                  static_cast<GLenum>(pname), &framebufferId);
            webGlFramebuffer->SetFramebuffer(static_cast<unsigned int>(framebufferId));
            LOGI("WebGL getFramebufferAttachmentParameter framebufferId = %{public}u", framebufferId);
            LOGI("WebGL getFramebufferAttachmentParameter WebGLRenderbuffer end");
            return objFramebuffer;
        } else if (type == TEXTURE) {
            LOGI("WebGL getFramebufferAttachmentParameter WebGLTexture start");
            napi_value objTexture = NClass::InstantiateClass(env, WebGLTexture::className, {});
            if (!objTexture) {
                return nullptr;
            }
            auto webGlTexture = NClass::GetEntityOf<WebGLTexture>(env, objTexture);
            if (!webGlTexture) {
                return nullptr;
            }
            GLint textureId;
            glGetFramebufferAttachmentParameteriv(static_cast<GLenum>(target), static_cast<GLenum>(attachment),
                                                  static_cast<GLenum>(pname), &textureId);
            webGlTexture->SetTexture(static_cast<unsigned int>(textureId));
            LOGI("WebGL getFramebufferAttachmentParameter textureId = %{public}u", textureId);
            LOGI("WebGL getFramebufferAttachmentParameter WebGLTexture end");
            return objTexture;
        } else {
            LOGI("WebGL getFramebufferAttachmentParameter : no image is attached");
            return 0;
        }
    } else {
        LOGI("WebGL getFramebufferAttachmentParameter : pname is wrong");
        return nullptr;
    }
}

napi_value WebGLRenderingContextBase::GetProgramParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL getProgramParameter start");
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGlProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    int programId = webGlProgram->GetProgramId();
    LOGI("WebGL WebGLRenderContext::getProgramParameter programId = %{public}u", programId);
    int64_t pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::getProgramParameter pname = %{public}u", pname);
    GLint params;
    glGetProgramiv(static_cast<GLuint>(programId), static_cast<GLenum>(pname), &params);
    if (pname == GL_DELETE_STATUS || pname == GL_LINK_STATUS || pname == GL_VALIDATE_STATUS) {
        bool res = (params == GL_FALSE) ? false : true;
        LOGI("WebGL WebGLRenderContext::getProgramParameter params = %{public}u", params);
        LOGI("WebGL getProgramParameter end");
        return NVal::CreateBool(env, res).val_;
    } else if (pname == GL_ATTACHED_SHADERS || pname == GL_ACTIVE_ATTRIBUTES || pname == GL_ACTIVE_UNIFORMS) {
        int64_t res = static_cast<int64_t>(params);
        LOGI("WebGL getProgramParameter end");
        return NVal::CreateInt64(env, res).val_;
    } else {
        LOGI("WebGL getProgramParameter : pname is wrong");
        return nullptr;
    }
}

napi_value WebGLRenderingContextBase::GetRenderbufferParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL getRenderbufferParameter start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::getRenderbufferParameter target = %{public}u", target);
    int64_t pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::getRenderbufferParameter pname = %{public}u", pname);
    if (pname == GL_RENDERBUFFER_WIDTH ||
        pname == GL_RENDERBUFFER_HEIGHT ||
        pname == GL_RENDERBUFFER_INTERNAL_FORMAT ||
        pname == GL_RENDERBUFFER_RED_SIZE ||
        pname == GL_RENDERBUFFER_GREEN_SIZE ||
        pname == GL_RENDERBUFFER_BLUE_SIZE ||
        pname == GL_RENDERBUFFER_ALPHA_SIZE ||
        pname == GL_RENDERBUFFER_DEPTH_SIZE ||
        pname == GL_RENDERBUFFER_STENCIL_SIZE) {
        GLint params;
        glGetRenderbufferParameteriv(static_cast<GLenum>(target), static_cast<GLenum>(pname), &params);
        int64_t res = static_cast<int64_t>(params);
        LOGI("WebGL getRenderbufferParameter end");
        return NVal::CreateInt64(env, res).val_;
    } else {
        LOGI("WebGL getRenderbufferParameter : pname is wrong");
        return nullptr;
    }
}

napi_value WebGLRenderingContextBase::GetTexParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL getTexParameter start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::getTexParameter target = %{public}u", target);
    int64_t pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::getTexParameter pname = %{public}u", pname);
    if (pname == GL_TEXTURE_MAX_ANISOTROPY_EXT) {
        GLfloat params;
        glGetTexParameterfv(static_cast<GLenum>(target), static_cast<GLenum>(pname), &params);
        float res = static_cast<float>(params);
        LOGI("WebGL getTexParameter end");
        return NVal::CreateDouble(env, (double)res).val_;
    } else if (pname == GL_TEXTURE_MAG_FILTER || pname == GL_TEXTURE_MIN_FILTER ||
               pname == GL_TEXTURE_WRAP_S || pname == GL_TEXTURE_WRAP_T) {
        GLint params;
        glGetTexParameteriv(static_cast<GLenum>(target), static_cast<GLenum>(pname), &params);
        int64_t res = static_cast<int64_t>(params);
        LOGI("WebGL getTexParameter end");
        return NVal::CreateInt64(env, res).val_;
    } else {
        LOGI("WebGL getTexParameter : pname is wrong");
        return nullptr;
    }
}

napi_value WebGLRenderingContextBase::GetShaderParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL getShaderParameter start");
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    WebGLShader *webGlShader = nullptr;
    napi_status shaderStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGlShader);
    if (shaderStatus != napi_ok) {
        return nullptr;
    }
    int shaderId = webGlShader->GetShaderId();
    LOGI("WebGL WebGLRenderContext::getShaderParameter shaderId = %{public}u", shaderId);
    int64_t pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::getShaderParameter pname = %{public}u", pname);
    GLint params;
    glGetShaderiv(static_cast<GLuint>(shaderId), static_cast<GLenum>(pname), &params);
    if (pname == GL_SHADER_TYPE) {
        int64_t res = static_cast<int64_t>(params);
        LOGI("WebGL getShaderParameter end");
        return NVal::CreateInt64(env, res).val_;
    } else if (pname == GL_DELETE_STATUS || pname == GL_COMPILE_STATUS) {
        bool res = (params == GL_FALSE) ? false : true;
        LOGI("WebGL WebGLRenderContext::getShaderParameter params = %{public}u", params);
        LOGI("WebGL getShaderParameter end");
        return NVal::CreateBool(env, res).val_;
    } else {
        LOGI("WebGL getShaderParameter : pname is wrong");
        return nullptr;
    }
}

napi_value WebGLRenderingContextBase::GetAttribLocation(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL getAttribLocation start");
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGlProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    int programId = webGlProgram->GetProgramId();
    LOGI("WebGL WebGLRenderContext::getAttribLocation programId = %{public}u", programId);
    unique_ptr<char[]> name;
    tie(succ, name, ignore) = NVal(env, funcArg[NARG_POS::SECOND]).ToUTF8String();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::getAttribLocation name = %{public}s", name.get());
    GLint returnValue = glGetAttribLocation(static_cast<GLuint>(programId), const_cast<char *>(name.get()));
    int64_t res = static_cast<int64_t>(returnValue);
    LOGI("WebGL getAttribLocation end");
    return NVal::CreateInt64(env, res).val_;
}

napi_value WebGLRenderingContextBase::VertexAttrib1fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL vertexAttrib1fv start");
    int32_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::vertexAttrib1fv index = %{public}u", index);
    napi_value array = funcArg[NARG_POS::SECOND];
    bool isArray = false;
    tie(succ, isArray) = NVal(env, array).IsArray();
    if (isArray) {     // []
        LOGI("WebGL vertexAttrib1fv is Array");
        uint32_t length;
        napi_status lengthStatus = napi_get_array_length(env, array, &length);
        if (lengthStatus != napi_ok) {
            return nullptr;
        }
        float vertexAttrib1fv[length];
        uint32_t i;
        for (i = 0; i < length; i++) {
            napi_value element;
            napi_status eleStatus = napi_get_element(env, array, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            double ele;
            napi_status doubleStatus = napi_get_value_double(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            vertexAttrib1fv[i] = (float) ele;
        }
        glVertexAttrib1fv(static_cast<GLuint>(index), static_cast<GLfloat *>(vertexAttrib1fv));
        LOGI("WebGL2 vertexAttrib1fv array end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, array).IsTypeArray();
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *data = nullptr;
    size_t length;
    napi_typedarray_type type;
    tie(succ, type, data, length) = NVal(env, array).ToTypedArray();
    if (!succ) {
        return nullptr;
    }
    if (type == napi_float32_array) {
        float *inputFloat = (float *) ((uint8_t*)(data));
        glVertexAttrib1fv(static_cast<GLuint>(index), static_cast<GLfloat *>(inputFloat));
    }
    LOGI("WebGL2 vertexAttrib1fv typeArray end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::VertexAttrib2fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL vertexAttrib2fv start");
    int32_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::vertexAttrib2fv index = %{public}u", index);
    napi_value array = funcArg[NARG_POS::SECOND];
    bool isArray = false;
    tie(succ, isArray) = NVal(env, array).IsArray();
    if (isArray) {     // []
        LOGI("WebGL vertexAttrib2fv is Array");
        uint32_t length;
        napi_status lengthStatus = napi_get_array_length(env, array, &length);
        if (lengthStatus != napi_ok) {
            return nullptr;
        }
        float vertexAttrib2fv[length];
        uint32_t i;
        for (i = 0; i < length; i++) {
            napi_value element;
            napi_status eleStatus = napi_get_element(env, array, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            double ele;
            napi_status doubleStatus = napi_get_value_double(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            vertexAttrib2fv[i] = (float) ele;
        }
        glVertexAttrib2fv(static_cast<GLuint>(index), static_cast<GLfloat *>(vertexAttrib2fv));
        LOGI("WebGL2 vertexAttrib2fv array end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, array).IsTypeArray();
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *data = nullptr;
    size_t length;
    napi_typedarray_type type;
    tie(succ, type, data, length) = NVal(env, array).ToTypedArray();
    if (!succ) {
        return nullptr;
    }
    if (type == napi_float32_array) {
        float *inputFloat = (float *) ((uint8_t*)(data));
        glVertexAttrib2fv(static_cast<GLuint>(index), static_cast<GLfloat *>(inputFloat));
    }
    LOGI("WebGL2 vertexAttrib2fv typeArray end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::VertexAttrib3fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL vertexAttrib3fv start");
    int32_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::vertexAttrib3fv index = %{public}u", index);
    napi_value array = funcArg[NARG_POS::SECOND];
    bool isArray = false;
    tie(succ, isArray) = NVal(env, array).IsArray();
    if (isArray) {     // []
        LOGI("WebGL vertexAttrib3fv is Array");
        uint32_t length;
        napi_status lengthStatus = napi_get_array_length(env, array, &length);
        if (lengthStatus != napi_ok) {
            return nullptr;
        }
        float vertexAttrib3fv[length];
        uint32_t i;
        for (i = 0; i < length; i++) {
            napi_value element;
            napi_status eleStatus = napi_get_element(env, array, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            double ele;
            napi_status doubleStatus = napi_get_value_double(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            vertexAttrib3fv[i] = (float) ele;
        }
        glVertexAttrib3fv(static_cast<GLuint>(index), static_cast<GLfloat *>(vertexAttrib3fv));
        LOGI("WebGL2 vertexAttrib3fv array end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, array).IsTypeArray();
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *data = nullptr;
    size_t length;
    napi_typedarray_type type;
    tie(succ, type, data, length) = NVal(env, array).ToTypedArray();
    if (!succ) {
        return nullptr;
    }
    if (type == napi_float32_array) {
        float *inputFloat = (float *) ((uint8_t*)(data));
        glVertexAttrib3fv(static_cast<GLuint>(index), static_cast<GLfloat *>(inputFloat));
    }
    LOGI("WebGL2 vertexAttrib3fv typeArray end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::VertexAttrib4fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL vertexAttrib4fv start");
    int32_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::vertexAttrib4fv index = %{public}u", index);
    napi_value array = funcArg[NARG_POS::SECOND];
    bool isArray = false;
    tie(succ, isArray) = NVal(env, array).IsArray();
    if (isArray) {     // []
        LOGI("WebGL vertexAttrib4fv is Array");
        uint32_t length;
        napi_status lengthStatus = napi_get_array_length(env, array, &length);
        if (lengthStatus != napi_ok) {
            return nullptr;
        }
        float vertexAttrib4fv[length];
        uint32_t i;
        for (i = 0; i < length; i++) {
            napi_value element;
            napi_status eleStatus = napi_get_element(env, array, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            double ele;
            napi_status doubleStatus = napi_get_value_double(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            vertexAttrib4fv[i] = (float) ele;
        }
        glVertexAttrib4fv(static_cast<GLuint>(index), static_cast<GLfloat *>(vertexAttrib4fv));
        LOGI("WebGL2 vertexAttrib4fv array end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, array).IsTypeArray();
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *data = nullptr;
    size_t length;
    napi_typedarray_type type;
    tie(succ, type, data, length) = NVal(env, array).ToTypedArray();
    if (!succ) {
        return nullptr;
    }
    if (type == napi_float32_array) {
        float *inputFloat = (float *) ((uint8_t*)(data));
        glVertexAttrib4fv(static_cast<GLuint>(index), static_cast<GLfloat *>(inputFloat));
    }
    LOGI("WebGL2 vertexAttrib4fv typeArray end");
    return nullptr;
}

napi_value WebGLRenderingContextBase::GetVertexAttrib(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL getVertexAttrib start");
    int32_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::getVertexAttrib index = %{public}u", index);
    int32_t pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::getVertexAttrib pname = %{public}u", pname);
    if (pname == GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING) {  // webglBuffer
        int32_t params;
        glGetVertexAttribiv(static_cast<GLuint>(index), static_cast<GLenum>(pname), &params);
        GLfloat par = static_cast<float>(params);
        napi_value objBuffer = NClass::InstantiateClass(env, WebGLBuffer::className, {});
        if (!objBuffer) {
            return nullptr;
        }
        auto webGlBuffer = NClass::GetEntityOf<WebGLBuffer>(env, objBuffer);
        if (!webGlBuffer) {
            return nullptr;
        }
        webGlBuffer->SetParams(par);
        LOGI("WebGL WebGLRenderingContextBase::createBuffer par = %{public}u", par);
        LOGI("WebGL getVertexAttrib panem is array end");
        return objBuffer;
    } else if (pname == GL_VERTEX_ATTRIB_ARRAY_ENABLED || pname == GL_VERTEX_ATTRIB_ARRAY_NORMALIZED ||
        pname == GL_VERTEX_ATTRIB_ARRAY_INTEGER) {
        int32_t params;
        glGetVertexAttribiv(static_cast<GLuint>(index), static_cast<GLenum>(pname), &params);
        bool res = static_cast<bool>(params);
        LOGI("WebGL getVertexAttrib pname is GLBoolean end");
        return NVal::CreateBool(env, res).val_;
    } else if (pname == GL_VERTEX_ATTRIB_ARRAY_SIZE || pname == GL_VERTEX_ATTRIB_ARRAY_STRIDE ||
        pname == GL_VERTEX_ATTRIB_ARRAY_DIVISOR || pname == GL_VERTEX_ATTRIB_ARRAY_DIVISOR_ANGLE) {
        int32_t params;
        glGetVertexAttribiv(static_cast<GLuint>(index), static_cast<GLenum>(pname), &params);
        int res = static_cast<int>(params);
        LOGI("WebGL getVertexAttrib pname is GLint end");
        return NVal::CreateInt64(env, res).val_;
    } else if (pname == GL_VERTEX_ATTRIB_ARRAY_TYPE) {  // GLenum
        int32_t params;
        glGetVertexAttribiv(static_cast<GLuint>(index), static_cast<GLenum>(pname), &params);
        int64_t res = static_cast<int>(params);
        LOGI("WebGL getVertexAttrib pname is Glenum end");
        return NVal::CreateInt64(env, res).val_;
    } else if (pname == GL_CURRENT_VERTEX_ATTRIB) {   // float32Array
        static float res[4] = {};
        glGetVertexAttribfv(static_cast<GLuint>(index), static_cast<GLenum>(pname), reinterpret_cast<GLfloat*>(res));
        napi_value outputBuffer = nullptr;
        napi_create_external_arraybuffer(env, res, sizeof(res),
                                         [](napi_env env, void *finalize_data, void *finalize_hint) {},
                                         NULL, &outputBuffer);
        napi_value output_array = nullptr;
        napi_create_typedarray(env, napi_float32_array, sizeof(res) / sizeof(float), outputBuffer, 0, &output_array);
        LOGI("WebGL getVertexAttrib pname is float32Array end");
        return output_array;
    } else {
        return nullptr;
    }
}

napi_value WebGLRenderingContextBase::GetParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL getParameter start");
    int64_t pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::getParameter pname = %{public}u", pname);
    if (pname == GL_DEPTH_CLEAR_VALUE || pname == GL_LINE_WIDTH ||
        pname == GL_POLYGON_OFFSET_FACTOR || pname == GL_POLYGON_OFFSET_UNITS ||
        pname == GL_SAMPLE_COVERAGE_VALUE) {
        GLfloat params;
        glGetFloatv(static_cast<GLenum>(pname), &params);
        float res = static_cast<float>(params);
        LOGI("WebGL getParameter end");
        return NVal::CreateDouble(env, (double)res).val_;
    } else if (pname == GL_TEXTURE_BINDING_2D || pname == GL_TEXTURE_BINDING_CUBE_MAP) {
        GLint params;
        glGetIntegerv(static_cast<GLenum>(pname), &params);
        napi_value objTexture = NClass::InstantiateClass(env, WebGLTexture::className, {});
        if (!objTexture) {
            return nullptr;
        }
        auto webGlTexture = NClass::GetEntityOf<WebGLTexture>(env, objTexture);
        if (!webGlTexture) {
            return nullptr;
        }
        webGlTexture->SetTexture(params);
        LOGI("WebGL getParameter end");
        return objTexture;
    } else if (pname == GL_RENDERER || pname == GL_SHADING_LANGUAGE_VERSION ||
               pname == GL_VENDOR || pname == GL_VERSION) {
        LOGI("WebGL pname : string");
        const unsigned char* extensions = glGetString(static_cast<GLenum>(pname));
        string str = (char*)extensions;
        vector<string> vec;
        Util::SplitString(str, vec, " ");
        napi_value result = nullptr;
        napi_create_array_with_length(env, vec.size(), &result);
        for (vector<string>::size_type i = 0; i != vec.size(); ++i) {
            napi_set_element(env, result, i, NVal::CreateUTF8String(env, vec[i]).val_);
        }
        return result;
    } else if (pname == GL_ARRAY_BUFFER_BINDING || pname == GL_ELEMENT_ARRAY_BUFFER_BINDING) {
        GLint params;
        glGetIntegerv(static_cast<GLenum>(pname), &params);
        napi_value objBuffer = NClass::InstantiateClass(env, WebGLBuffer::className, {});
        if (!objBuffer) {
            return nullptr;
        }
        auto webGlBuffer = NClass::GetEntityOf<WebGLBuffer>(env, objBuffer);
        if (!webGlBuffer) {
            return nullptr;
        }
        webGlBuffer->SetBuffer(params);
        LOGI("WebGL getParameter end");
        return objBuffer;
    } else if (pname == GL_FRAMEBUFFER_BINDING) {
        GLint params;
        glGetIntegerv(static_cast<GLenum>(pname), &params);
        napi_value objFramebuffer = NClass::InstantiateClass(env, WebGLFramebuffer::className, {});
        if (!objFramebuffer) {
            return nullptr;
        }
        auto webGLFramebuffer = NClass::GetEntityOf<WebGLFramebuffer>(env, objFramebuffer);
        if (!webGLFramebuffer) {
            return nullptr;
        }
        webGLFramebuffer->SetFramebuffer(params);
        LOGI("WebGL getParameter end");
        return objFramebuffer;
    } else if (pname == GL_CURRENT_PROGRAM) {
        GLint params;
        glGetIntegerv(static_cast<GLenum>(pname), &params);
        napi_value objProgram = NClass::InstantiateClass(env, WebGLProgram::className, {});
        if (!objProgram) {
            return nullptr;
        }
        auto webGlProgram = NClass::GetEntityOf<WebGLProgram>(env, objProgram);
        if (!webGlProgram) {
            return nullptr;
        }
        webGlProgram->SetProgramId(params);
        LOGI("WebGL getParameter end");
        return objProgram;
    } else if (pname == GL_RENDERBUFFER_BINDING) {
        GLint params;
        glGetIntegerv(static_cast<GLenum>(pname), &params);
        napi_value objRenderbuffer = NClass::InstantiateClass(env, WebGLRenderbuffer::className, {});
        if (!objRenderbuffer) {
            return nullptr;
        }
        auto webGlRenderbuffer = NClass::GetEntityOf<WebGLRenderbuffer>(env, objRenderbuffer);
        if (!webGlRenderbuffer) {
            return nullptr;
        }
        webGlRenderbuffer->SetRenderbuffer(params);
        LOGI("WebGL getParameter end");
        return objRenderbuffer;
    } else if (pname == GL_ALIASED_LINE_WIDTH_RANGE || pname == GL_ALIASED_POINT_SIZE_RANGE ||
               pname == GL_DEPTH_RANGE) {
        LOGI("WebGL pname : Float32Array with 2 elements");
        static float res[2] = {};
        glGetFloatv(static_cast<GLenum>(pname), reinterpret_cast<GLfloat*>(res));
        napi_value outputBuffer = nullptr;
        napi_create_external_arraybuffer(env, res, sizeof(res),
                                         [](napi_env env, void *finalize_data, void *finalize_hint) {  },
                                         NULL, &outputBuffer);
        napi_value outputArray = nullptr;
        napi_create_typedarray(env, napi_float32_array, sizeof(res) / sizeof(float),
                               outputBuffer, 0, &outputArray);
        LOGI("WebGL getParameter end");
        return outputArray;
    } else if (pname == GL_COLOR_CLEAR_VALUE || pname == GL_BLEND_COLOR) {
        LOGI("WebGL pname : Float32Array with 4 valus");
        static float res[4] = {};
        glGetFloatv(static_cast<GLenum>(pname), reinterpret_cast<GLfloat*>(res));
        napi_value outputBuffer = nullptr;
        napi_create_external_arraybuffer(env, res, sizeof(res),
                                         [](napi_env env, void *finalize_data, void *finalize_hint) {  },
                                         NULL, &outputBuffer);
        napi_value outputArray = nullptr;
        napi_create_typedarray(env, napi_float32_array, sizeof(res) / sizeof(float),
                               outputBuffer, 0, &outputArray);
        LOGI("WebGL getParameter end");
        return outputArray;
    } else if (pname == GL_COMPRESSED_TEXTURE_FORMATS) {
        LOGI("WebGL pname : Uint32Array");
        GLint count = 0;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &count);
        GLint params[count];
        glGetIntegerv(static_cast<GLenum>(pname), static_cast<GLint*>(params));
        static uint32_t *res = new uint32_t[count];
        int i;
        for (i = 0; i < count; i++) {
            res[i] = static_cast<uint32_t>(params[i]);
        }
        napi_value outputBuffer = nullptr;
        napi_create_external_arraybuffer(env, res, sizeof(params),
                                         [](napi_env env, void *finalize_data, void *finalize_hint) {  },
                                         NULL, &outputBuffer);
        napi_value outputArray = nullptr;
        napi_create_typedarray(env, napi_uint32_array, sizeof(params) / sizeof(uint32_t),
                               outputBuffer, 0, &outputArray);
        LOGI("WebGL getParameter end");
        return outputArray;
    } else if (pname == GL_MAX_VIEWPORT_DIMS) {
        LOGI("WebGL pname : int32Array with 2 elements");
        static int64_t res[2] = {};
        glGetIntegerv(static_cast<GLenum>(pname), reinterpret_cast<GLint*>(res));
        napi_value outputBuffer = nullptr;
        napi_create_external_arraybuffer(env, res, sizeof(res),
                                         [](napi_env env, void *finalize_data, void *finalize_hint) {  },
                                         NULL, &outputBuffer);
        napi_value outputArray = nullptr;
        napi_create_typedarray(env, napi_int32_array, sizeof(res) / sizeof(int64_t),
                               outputBuffer, 0, &outputArray);
        LOGI("WebGL getParameter end");
        return outputArray;
    } else if (pname == GL_SCISSOR_BOX) {
        LOGI("WebGL pname : int32Array with 2 4 valus");
        static int64_t res[4] = {};
        glGetIntegerv(static_cast<GLenum>(pname), reinterpret_cast<GLint*>(res));
        napi_value outputBuffer = nullptr;
        napi_create_external_arraybuffer(env, res, sizeof(res),
                                         [](napi_env env, void *finalize_data, void *finalize_hint) {  },
                                         NULL, &outputBuffer);
        napi_value outputArray = nullptr;
        napi_create_typedarray(env, napi_int32_array, sizeof(res) / sizeof(int64_t),
                               outputBuffer, 0, &outputArray);
        LOGI("WebGL getParameter end");
        return outputArray;
    } else if (pname == GL_BLEND || pname == GL_CULL_FACE || pname == GL_DEPTH_TEST ||
               pname == GL_DEPTH_WRITEMASK || pname == GL_DITHER || pname == GL_POLYGON_OFFSET_FILL ||
               pname == GL_SAMPLE_ALPHA_TO_COVERAGE || pname == GL_SAMPLE_COVERAGE ||
               pname == GL_SAMPLE_COVERAGE_INVERT || pname == GL_SCISSOR_TEST || pname == GL_STENCIL_TEST ||
               pname == WebGLRenderingContextBase::UNPACK_FLIP_Y_WEBGL ||
               pname == WebGLRenderingContextBase::UNPACK_PREMULTIPLY_ALPHA_WEBGL) {
        GLboolean params;
        glGetBooleanv(static_cast<GLenum>(pname), &params);
        bool res = static_cast<bool>(params);
        LOGI("WebGL getParameter end");
        return NVal::CreateBool(env, res).val_;
    } else if (pname == GL_COLOR_WRITEMASK) {
        LOGI("WebGL bool end");
        GLboolean params[4];
        glGetBooleanv(static_cast<GLenum>(pname), static_cast<GLboolean*>(params));
        napi_value res = nullptr;
        napi_create_array(env, &res);
        uint32_t i;
        uint32_t length = 4;
        for (i = 0; i < length; i++) {
            bool a = static_cast<bool>(params[i]);
            napi_value result = nullptr;
            napi_status status = napi_get_boolean(env, a, &result);
            if (status != napi_ok) {
                return nullptr;
            }
            napi_set_element(env, res, i, result);
        }
        LOGI("WebGL getParameter end");
        return res;
    } else if (pname == GL_ACTIVE_TEXTURE ||
               pname == GL_ALPHA_BITS ||
               pname == GL_BLEND_DST_ALPHA ||
               pname == GL_BLEND_DST_RGB ||
               pname == GL_BLEND_EQUATION_ALPHA ||
               pname == GL_BLEND_EQUATION_RGB ||
               pname == GL_BLEND_SRC_ALPHA ||
               pname == GL_BLEND_SRC_RGB ||
               pname == GL_BLUE_BITS ||
               pname == GL_CULL_FACE_MODE ||
               pname == GL_DEPTH_BITS ||
               pname == GL_DEPTH_FUNC ||
               pname == GL_FRONT_FACE ||
               pname == GL_GENERATE_MIPMAP_HINT ||
               pname == GL_GREEN_BITS ||
               pname == GL_IMPLEMENTATION_COLOR_READ_FORMAT ||
               pname == GL_IMPLEMENTATION_COLOR_READ_TYPE ||
               pname == GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS ||
               pname == GL_MAX_CUBE_MAP_TEXTURE_SIZE ||
               pname == GL_MAX_FRAGMENT_UNIFORM_VECTORS ||
               pname == GL_MAX_RENDERBUFFER_SIZE ||
               pname == GL_MAX_TEXTURE_IMAGE_UNITS ||
               pname == GL_MAX_TEXTURE_SIZE ||
               pname == GL_MAX_VARYING_VECTORS ||
               pname == GL_MAX_VERTEX_ATTRIBS ||
               pname == GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS ||
               pname == GL_MAX_VERTEX_UNIFORM_VECTORS ||
               pname == GL_PACK_ALIGNMENT ||
               pname == GL_RED_BITS ||
               pname == GL_SAMPLE_BUFFERS ||
               pname == GL_SAMPLES ||
               pname == GL_STENCIL_BACK_FAIL ||
               pname == GL_STENCIL_BACK_FUNC ||
               pname == GL_STENCIL_BACK_PASS_DEPTH_FAIL ||
               pname == GL_STENCIL_BACK_PASS_DEPTH_PASS ||
               pname == GL_STENCIL_BACK_REF ||
               pname == GL_STENCIL_BACK_VALUE_MASK ||
               pname == GL_STENCIL_BACK_WRITEMASK ||
               pname == GL_STENCIL_BITS ||
               pname == GL_STENCIL_CLEAR_VALUE ||
               pname == GL_STENCIL_FAIL ||
               pname == GL_STENCIL_FUNC ||
               pname == GL_STENCIL_PASS_DEPTH_FAIL ||
               pname == GL_STENCIL_PASS_DEPTH_PASS ||
               pname == GL_STENCIL_REF ||
               pname == GL_STENCIL_VALUE_MASK ||
               pname == GL_STENCIL_WRITEMASK ||
               pname == GL_SUBPIXEL_BITS ||
               pname == GL_UNPACK_ALIGNMENT ||
               pname == UNPACK_COLORSPACE_CONVERSION_WEBGL) {
        GLint params;
        glGetIntegerv(static_cast<GLenum>(pname), &params);
        int64_t res = static_cast<int64_t>(params);
        LOGI("WebGL getParameter end");
        return NVal::CreateInt64(env, res).val_;
    } else {
        LOGI("WebGL getParameter : pname is wrong");
        return nullptr;
    }
}

napi_value WebGLRenderingContextBase::GetAttachedShaders(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    LOGI("WebGL getAttachedShaders start");
    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGlProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    int programId = webGlProgram->GetProgramId();
    LOGI("WebGL WebGLRenderContext::getAttachedShaders programId = %{public}u", programId);
    int64_t maxCount = 0;
    GLsizei count = 1;
    GLuint shader[count];
    glGetAttachedShaders(static_cast<GLuint>(programId), static_cast<GLsizei>(maxCount), &count,
                         static_cast<GLuint*>(shader));
    LOGI("WebGL WebGLRenderContext::getAttachedShaders count = %{public}u", count);
    GLsizei i;
    napi_value objShader = NClass::InstantiateClass(env, WebGLShader::className, {});
    if (!objShader) {
        return nullptr;
    }
    auto webGlShader = NClass::GetEntityOf<WebGLShader>(env, objShader);
    if (!webGlShader) {
        return nullptr;
    }
    napi_value shaderArr = nullptr;
    napi_create_array(env, &shaderArr);
    for (i = 0; i < count; i++) {
        webGlShader->SetShaderId(shader[i]);
        napi_set_element(env, shaderArr, i, objShader);
    }
    LOGI("WebGL getAttachedShaders end");
    return shaderArr;
}

napi_value WebGLRenderingContextBase::GetShaderPrecisionFormat(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL getShaderPrecisionFormat start");
    int64_t shaderType;
    tie(succ, shaderType) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::getShaderPrecisionFormat shaderType = %{public}u", shaderType);
    int64_t precisionType;
    tie(succ, precisionType) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::getShaderPrecisionFormat precisionType = %{public}u", precisionType);
    GLint range[2] { };
    GLint precision = 0;
    napi_value objShaderPrecisionFormat = NClass::InstantiateClass(env, WebGLShaderPrecisionFormat::className, {});
    if (!objShaderPrecisionFormat) {
        LOGI("WebGL WebGLRenderContext::getShaderPrecisionFormat objShaderPrecisionFormat = %{public}u",
             objShaderPrecisionFormat);
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::getShaderPrecisionFormat objShaderPrecisionFormat = %{public}u",
         objShaderPrecisionFormat);
    auto webGLShaderPrecisionFormat = NClass::GetEntityOf<WebGLShaderPrecisionFormat>(env, objShaderPrecisionFormat);
    if (!webGLShaderPrecisionFormat) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::getShaderPrecisionFormat webGLShaderPrecisionFormat = %{public}u",
         webGLShaderPrecisionFormat);
    glGetShaderPrecisionFormat(static_cast<GLenum>(shaderType), static_cast<GLenum>(precisionType), range, &precision);
    webGLShaderPrecisionFormat->SetShaderPrecisionFormatRangeMin(range[0]);
    webGLShaderPrecisionFormat->SetShaderPrecisionFormatRangeMax(range[1]);
    webGLShaderPrecisionFormat->SetShaderPrecisionFormatPrecision(precision);
    LOGI("WebGL getShaderPrecisionFormat end");
    return objShaderPrecisionFormat;
}

napi_value WebGLRenderingContextBase::GetShaderInfoLog(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }
    LOGI("WebGL getShaderInfoLog start");
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    WebGLShader *webGlShader = nullptr;
    napi_status shaderStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGlShader);
    if (shaderStatus != napi_ok) {
        return NVal::CreateNull(env).val_;
    }
    int shaderId = webGlShader->GetShaderId();
    LOGI("WebGL WebGLRenderContext::getShaderInfoLog shaderId = %{public}u", shaderId);
    GLint length = 0;
    glGetShaderiv(static_cast<GLuint>(shaderId), GL_INFO_LOG_LENGTH, &length);
    GLsizei size = 0;
    std::unique_ptr<char[]> buf = std::make_unique<char[]>(length);
    glGetShaderInfoLog(shaderId, length, &size, buf.get());
    string str = buf.get();
    LOGI("WebGL getShaderInfoLog end");
    return NVal::CreateUTF8String(env, str).val_;
}

napi_value WebGLRenderingContextBase::GetProgramInfoLog(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    LOGI("WebGL getProgramInfoLog start");
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGlProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    int programId = webGlProgram->GetProgramId();
    LOGI("WebGL WebGLRenderContext::getProgramInfoLog programId = %{public}u", programId);
    GLint length = 0;
    GLsizei size = 0;
    glGetProgramiv(static_cast<GLuint>(programId), GL_INFO_LOG_LENGTH, &length);
    std::unique_ptr<char[]> buf = std::make_unique<char[]>(length);
    LOGI("WebGL WebGLRenderContext::getProgramInfoLog bufSize = %{public}u", length);
    if (buf == nullptr) {
        return nullptr;
    }
    glGetProgramInfoLog(programId, length, &size, buf.get());
    string str = buf.get();
    LOGI("WebGL getProgramInfoLog end");
    return NVal::CreateUTF8String(env, str).val_;
}

napi_value WebGLRenderingContextBase::GetShaderSource(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    LOGI("WebGL getShaderSource start");
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    WebGLShader *webGlShader = nullptr;
    napi_status shaderStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGlShader);
    if (shaderStatus != napi_ok) {
        return nullptr;
    }
    GLuint shaderId = webGlShader->GetShaderId();
    LOGI("WebGL WebGLRenderContext::getShaderSource shaderId = %{public}u", shaderId);
    auto objects = ObjectSource::GetInstance().GetObjectMap();
    auto it = objects.find(shaderId);
    if (it == objects.end()) {
        return NVal::CreateNull(env).val_;
    }
    LOGI("WebGL getShaderSource end");
    return NVal::CreateUTF8String(env, it->second).val_;
}

napi_value WebGLRenderingContextBase::GetUniform(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    LOGI("WebGL getUniform start");
    if (funcArg[NARG_POS::FIRST] == nullptr || funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    WebGLProgram *webGlProgram = nullptr;
    napi_value a = funcArg[NARG_POS::FIRST];
    LOGI("WebGL WebGLRenderContext::getUniform a = %{public}d", a);
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    int programId = webGlProgram->GetProgramId();
    LOGI("WebGL WebGLRenderContext::getUniform programId = %{public}d", programId);

    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::SECOND], (void **) &webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int locationId = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL WebGLRenderContext::getUniform locationId = %{public}d", locationId);
    GLint maxNameLength = -1;
    glGetProgramiv(programId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);
    LOGI("WebGL WebGLRenderContext::getUniform maxNameLength = %{public}d", maxNameLength);
    if (maxNameLength <= 0) {
        return nullptr;
    }
    GLint activeUniforms = 0;
    glGetProgramiv(programId, GL_ACTIVE_UNIFORMS, &activeUniforms);
    LOGI("WebGL WebGLRenderContext::getUniform activeUniforms = %{public}d", activeUniforms);
    for (GLint i = 0; i < activeUniforms; i++) {
        char name_ptr[maxNameLength];
        GLsizei name_length = 0;
        GLint size = -1;
        GLenum type = 0;
        glGetActiveUniform(programId, i, maxNameLength, &name_length, &size, &type,
                           reinterpret_cast<GLchar *>(name_ptr));
        LOGI("WebGL WebGLRenderContext::getUniform type = %{public}u", type);
        if (type == GL_INT || type == GL_SAMPLER_2D || type == GL_SAMPLER_CUBE) {
            LOGI("WebGL getUniform1 end");
            GLint params;
            glGetUniformiv(static_cast<GLuint>(programId), static_cast<GLint>(locationId), &params);
            int64_t res = static_cast<int64_t>(params);
            LOGI("WebGL getUniform end");
            return NVal::CreateInt64(env, res).val_;
        } else if (type == GL_FLOAT) {
            LOGI("WebGL getUniform2 end");
            GLfloat params;
            glGetUniformfv(static_cast<GLuint>(programId), static_cast<GLint>(locationId), &params);
            float res = static_cast<float>(params);
            LOGI("WebGL getUniform end");
            return NVal::CreateDouble(env, (double) res).val_;
        } else if (type == GL_FLOAT_VEC2) {
            LOGI("WebGL getUniform3 end");
            static float res[2] = {};
            glGetUniformfv(static_cast<GLuint>(programId), static_cast<GLint>(locationId),
                           reinterpret_cast<GLfloat *>(res));
            napi_value outputBuffer = nullptr;
            napi_create_external_arraybuffer(env, res, sizeof(res),
                                             [](napi_env env, void *finalize_data, void *finalize_hint) {},
                                             NULL, &outputBuffer);
            napi_value outputArray = nullptr;
            napi_create_typedarray(env, napi_float32_array, sizeof(res) / sizeof(float),
                                   outputBuffer, 0, &outputArray);
            LOGI("WebGL getUniform end");
            return outputArray;
        } else if (type == GL_FLOAT_VEC3) {
            LOGI("WebGL getUniform4 end");
            static float res[3] = {};
            glGetUniformfv(static_cast<GLuint>(programId), static_cast<GLint>(locationId),
                           reinterpret_cast<GLfloat *>(res));
            napi_value outputBuffer = nullptr;
            napi_create_external_arraybuffer(env, res, sizeof(res),
                                             [](napi_env env, void *finalize_data, void *finalize_hint) {},
                                             NULL, &outputBuffer);
            napi_value outputArray = nullptr;
            napi_create_typedarray(env, napi_float32_array, sizeof(res) / sizeof(float),
                                   outputBuffer, 0, &outputArray);
            LOGI("WebGL getUniform end");
            return outputArray;
        } else if (type == GL_FLOAT_VEC4 || locationId == GL_FLOAT_MAT2) {
            LOGI("WebGL getUniform5 end");
            static float res[4] = {};
            glGetUniformfv(static_cast<GLuint>(programId), static_cast<GLint>(locationId),
                           reinterpret_cast<GLfloat *>(res));
            napi_value outputBuffer = nullptr;
            napi_create_external_arraybuffer(env, res, sizeof(res),
                                             [](napi_env env, void *finalize_data, void *finalize_hint) {},
                                             NULL, &outputBuffer);
            napi_value outputArray = nullptr;
            napi_create_typedarray(env, napi_float32_array, sizeof(res) / sizeof(float),
                                   outputBuffer, 0, &outputArray);
            LOGI("WebGL getUniform end");
            return outputArray;
        } else if (type == GL_FLOAT_MAT3) {
            LOGI("WebGL getUniform6 end");
            static float res[9] = {};
            glGetUniformfv(static_cast<GLuint>(programId), static_cast<GLint>(locationId),
                           reinterpret_cast<GLfloat *>(res));
            napi_value outputBuffer = nullptr;
            napi_create_external_arraybuffer(env, res, sizeof(res),
                                             [](napi_env env, void *finalize_data, void *finalize_hint) {},
                                             NULL, &outputBuffer);
            napi_value outputArray = nullptr;
            napi_create_typedarray(env, napi_float32_array, sizeof(res) / sizeof(float),
                                   outputBuffer, 0, &outputArray);
            LOGI("WebGL getUniform end");
            return outputArray;
        } else if (type == GL_FLOAT_MAT4) {
            LOGI("WebGL getUniform7 end");
            static float res[16] = {};
            glGetUniformfv(static_cast<GLuint>(programId), static_cast<GLint>(locationId),
                           reinterpret_cast<GLfloat *>(res));
            napi_value outputBuffer = nullptr;
            napi_create_external_arraybuffer(env, res, sizeof(res),
                                             [](napi_env env, void *finalize_data, void *finalize_hint) {},
                                             NULL, &outputBuffer);
            napi_value outputArray = nullptr;
            napi_create_typedarray(env, napi_float32_array, sizeof(res) / sizeof(float),
                                   outputBuffer, 0, &outputArray);
            LOGI("WebGL getUniform end");
            return outputArray;
        } else if (type == GL_INT_VEC2) {
            LOGI("WebGL getUniform8 end");
            static int64_t res[2] = {};
            glGetUniformiv(static_cast<GLuint>(programId), static_cast<GLint>(locationId),
                           reinterpret_cast<GLint *>(res));
            napi_value outputBuffer = nullptr;
            napi_create_external_arraybuffer(env, res, sizeof(res),
                                             [](napi_env env, void *finalize_data, void *finalize_hint) {},
                                             NULL, &outputBuffer);
            napi_value outputArray = nullptr;
            napi_create_typedarray(env, napi_int32_array, sizeof(res) / sizeof(int64_t),
                                   outputBuffer, 0, &outputArray);
            LOGI("WebGL getUniform end");
            return outputArray;
        } else if (type == GL_INT_VEC3) {
            LOGI("WebGL getUniform9 end");
            static int64_t res[3] = {};
            glGetUniformiv(static_cast<GLuint>(programId), static_cast<GLint>(locationId),
                           reinterpret_cast<GLint *>(res));
            napi_value outputBuffer = nullptr;
            napi_create_external_arraybuffer(env, res, sizeof(res),
                                             [](napi_env env, void *finalize_data, void *finalize_hint) {},
                                             NULL, &outputBuffer);
            napi_value outputArray = nullptr;
            napi_create_typedarray(env, napi_int32_array, sizeof(res) / sizeof(int64_t),
                                   outputBuffer, 0, &outputArray);
            LOGI("WebGL getUniform end");
            return outputArray;
        } else if (type == GL_INT_VEC4) {
            LOGI("WebGL getUniform10 end");
            static int64_t res[4] = {};
            glGetUniformiv(static_cast<GLuint>(programId), static_cast<GLint>(locationId),
                           reinterpret_cast<GLint *>(res));
            napi_value outputBuffer = nullptr;
            napi_create_external_arraybuffer(env, res, sizeof(res),
                                             [](napi_env env, void *finalize_data, void *finalize_hint) {},
                                             NULL, &outputBuffer);
            napi_value outputArray = nullptr;
            napi_create_typedarray(env, napi_int32_array, sizeof(res) / sizeof(int64_t),
                                   outputBuffer, 0, &outputArray);
            LOGI("WebGL getUniform end");
            return outputArray;
        } else if (type == GL_BOOL) {
            LOGI("WebGL getUniform11 end");
            GLint params;
            glGetUniformiv(static_cast<GLuint>(programId), static_cast<GLint>(locationId), &params);
            bool res = (params == GL_FALSE) ? false : true;
            LOGI("WebGL getUniform end");
            return NVal::CreateBool(env, res).val_;
        } else if (type == GL_BOOL_VEC2) {
            LOGI("WebGL getUniform12 end");
            GLint params[2] = {0};
            glGetUniformiv(static_cast<GLuint>(programId), static_cast<GLint>(locationId),
                           static_cast<GLint *>(params));
            uint32_t i;
            uint32_t length = 2;
            napi_value res = nullptr;
            napi_create_array(env, &res);
            for (i = 0; i < length; i++) {
                bool a = static_cast<bool>(params[i]);
                napi_value result = nullptr;
                napi_status status = napi_get_boolean(env, a, &result);
                if (status != napi_ok) {
                    return nullptr;
                }
                napi_set_element(env, res, i, result);
            }
            LOGI("WebGL getUniform end");
            return res;
        } else if (type == GL_BOOL_VEC4) {
            LOGI("WebGL getUniform13 end");
            LOGI("WebGL getUniform end");
            GLint params[4] = {0};
            glGetUniformiv(static_cast<GLuint>(programId), static_cast<GLint>(locationId),
                           static_cast<GLint *>(params));
            uint32_t i;
            uint32_t length = 4;
            napi_value res = nullptr;
            napi_create_array(env, &res);
            for (i = 0; i < length; i++) {
                bool a = static_cast<bool>(params[i]);
                napi_value result = nullptr;
                napi_status status = napi_get_boolean(env, a, &result);
                if (status != napi_ok) {
                    return nullptr;
                }
                napi_set_element(env, res, i, result);
            }
            LOGI("WebGL getUniform end");
            return res;
        } else {
            LOGI("WebGL getUniform end");
            return nullptr;
        }
    }
    return nullptr;
}
} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif
