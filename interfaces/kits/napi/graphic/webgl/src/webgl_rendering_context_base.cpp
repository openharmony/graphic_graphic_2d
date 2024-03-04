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

#include "context/webgl_rendering_context_base.h"

#include "context/webgl2_rendering_context_base.h"
#include "context/webgl_context_attributes.h"
#include "context/webgl_rendering_context.h"
#include "context/webgl_rendering_context_base_impl.h"
#include "context/webgl_rendering_context_basic_base.h"
#include "napi/n_class.h"
#include "napi/n_func_arg.h"
#include "util/egl_manager.h"
#include "util/log.h"
#include "util/util.h"
#include "webgl/webgl_active_info.h"
#include "webgl/webgl_arg.h"
#include "webgl/webgl_buffer.h"
#include "webgl/webgl_framebuffer.h"
#include "webgl/webgl_program.h"
#include "webgl/webgl_renderbuffer.h"
#include "webgl/webgl_shader.h"
#include "webgl/webgl_shader_precision_format.h"
#include "webgl/webgl_texture.h"
#include "webgl/webgl_uniform_location.h"

#ifdef __cplusplus
extern "C" {
#endif
namespace OHOS {
namespace Rosen {
using namespace std;
static WebGLRenderingContext* GetWebGLRenderingContextBase(napi_env env, napi_value thisVar)
{
    return static_cast<WebGLRenderingContext*>(Util::GetContextObject(env, thisVar));
}

static bool IsHighWebGL(napi_env env, napi_value thisVar)
{
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, thisVar);
    if (context == nullptr) {
        return false;
    }
    return context->GetWebGLRenderingContextImpl().IsHighWebGL();
}

static bool CheckGLenum(napi_env env, napi_value thisVar, GLenum type, const std::vector<GLenum>& glSupport,
    const std::vector<GLenum>& gl2Support)
{
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, thisVar);
    if (context == nullptr) {
        return false;
    }
    return context->GetWebGLRenderingContextImpl().CheckGLenum(type, glSupport, gl2Support);
}

napi_value WebGLRenderingContextBase::GetContextAttributes(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGLRenderingContextImpl().GetContextAttributes(env);
}

napi_value WebGLRenderingContextBase::IsContextLost(napi_env env, napi_callback_info info)
{
    bool res = false;
    if (EglManager::GetInstance().GetEGLContext() == nullptr) {
        res = true;
    }
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGLRenderingContextBase::GetSupportedExtensions(napi_env env, napi_callback_info info)
{
    if (EglManager::GetInstance().GetEGLContext() == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    const char* extensions = eglQueryString(EglManager::GetInstance().GetEGLDisplay(), EGL_EXTENSIONS);
    string str = extensions;
    vector<string> vec {};
    Util::SplitString(str, vec, " ");
    napi_value result = nullptr;
    napi_create_array_with_length(env, vec.size(), &result);
    for (vector<string>::size_type i = 0; i != vec.size(); ++i) {
        LOGD("WebGL %{public}s", vec[i].c_str());
        napi_set_element(env, result, i, NVal::CreateUTF8String(env, vec[i]).val_);
    }
    return result;
}

napi_value LoseContext(napi_env env, napi_callback_info info)
{
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBase::GetExtension(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    unique_ptr<char[]> name = nullptr;
    uint32_t nameLen = 0;
    tie(succ, name, nameLen) = NVal(env, funcArg[NARG_POS::FIRST]).ToUTF8String();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    LOGD("WebGL getExtension %{public}s", name.get());
#ifdef SUPPORT_COMPRESSED_RGB_S3TC
    if (strcmp(name.get(), "WEBGL_compressed_texture_s3tc") == 0 ||
        strcmp(name.get(), "MOZ_WEBGL_compressed_texture_s3tc") == 0 ||
        strcmp(name.get(), "WEBKIT_WEBGL_compressed_texture_s3tc") == 0) {
        NVal res = NVal::CreateObject(env);
        res.AddProp("COMPRESSED_RGB_S3TC_DXT1_EXT", NVal::CreateInt64(env, GL_COMPRESSED_RGB_S3TC_DXT1_EXT).val_);
        res.AddProp("COMPRESSED_RGBA_S3TC_DXT1_EXT", NVal::CreateInt64(env, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT).val_);
        res.AddProp("COMPRESSED_RGBA_S3TC_DXT3_EXT", NVal::CreateInt64(env, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT).val_);
        res.AddProp("COMPRESSED_RGBA_S3TC_DXT5_EXT", NVal::CreateInt64(env, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT).val_);
        return res.val_;
    }
#endif
    if (strcmp(name.get(), "WEBGL_lose_context") == 0) { // WEBGL_lose_context
        NVal res = NVal::CreateObject(env);
        vector<napi_property_descriptor> props = {
            NVal::DeclareNapiFunction("loseContext", LoseContext),
        };
        res.AddProp(std::move(props));
        return res.val_;
    }
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBase::ActiveTexture(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLenum texture = 0;
    tie(succ, texture) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGLRenderingContextImpl().ActiveTexture(env, texture);
}

napi_value WebGLRenderingContextBase::AttachShader(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGLRenderingContextImpl().AttachShader(
        env, funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND]);
}

napi_value WebGLRenderingContextBase::BindAttribLocation(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    int64_t index = 0;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ || index < 0) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }
    unique_ptr<char[]> name;
    uint32_t nameLen = 0;
    tie(succ, name, nameLen) = NVal(env, funcArg[NARG_POS::THIRD]).ToUTF8String();
    if (!succ) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }
    std::string str(name.get(), nameLen);
    return context->GetWebGLRenderingContextImpl().BindAttribLocation(
        env, funcArg[NARG_POS::FIRST], static_cast<GLuint>(index), str);
}

napi_value WebGLRenderingContextBase::BindBuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLenum target = 0;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context != nullptr) {
        context->GetWebGLRenderingContextImpl().BindBuffer(env, target, funcArg[NARG_POS::SECOND]);
    }
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBase::BindFramebuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLenum target = 0;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context != nullptr) {
        context->GetWebGLRenderingContextImpl().BindFrameBuffer(env, target, funcArg[NARG_POS::SECOND]);
    }
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBase::BindRenderbuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLenum target = 0;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().BindRenderBuffer(env, target, funcArg[NARG_POS::SECOND]);
}

napi_value WebGLRenderingContextBase::BindTexture(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLenum target = 0;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().BindTexture(env, target, funcArg[NARG_POS::SECOND]);
}

napi_value WebGLRenderingContextBase::BlendColor(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    double red = 0.0;
    tie(succ, red) = NVal(env, funcArg[NARG_POS::FIRST]).ToDouble();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    double green = 0.0;
    tie(succ, green) = NVal(env, funcArg[NARG_POS::SECOND]).ToDouble();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    double blue = 0.0;
    tie(succ, blue) = NVal(env, funcArg[NARG_POS::THIRD]).ToDouble();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    double alpha = 0.0;
    tie(succ, alpha) = NVal(env, funcArg[NARG_POS::FOURTH]).ToDouble();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    LOGD("WebGL blendColor %{public}f %{public}f %{public}f %{public}f ", static_cast<GLclampf>(red),
        static_cast<GLclampf>(green), static_cast<GLclampf>(blue), static_cast<GLclampf>(alpha));
    glBlendColor(static_cast<GLclampf>(red), static_cast<GLclampf>(green), static_cast<GLclampf>(blue),
        static_cast<GLclampf>(alpha));
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBase::BlendEquation(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLenum mode = 0;
    tie(succ, mode) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    LOGD("WebGL blendEquation mode %{public}u", mode);
    if (!CheckGLenum(env, funcArg.GetThisVar(), mode,
        {
            WebGLRenderingContextBase::FUNC_ADD,
            WebGLRenderingContextBase::FUNC_SUBTRACT,
            WebGLRenderingContextBase::FUNC_REVERSE_SUBTRACT
        },
        {
            WebGL2RenderingContextBase::MIN,
            WebGL2RenderingContextBase::MAX
        })) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    glBlendEquation(mode);
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBase::BlendEquationSeparate(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLenum modeRGB = 0;
    tie(succ, modeRGB) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    GLenum modeAlpha = 0;
    tie(succ, modeAlpha) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    LOGD("WebGL blendEquationSeparate modeRGB %{public}u  %{public}u", modeRGB, modeAlpha);
    if (!CheckGLenum(env, funcArg.GetThisVar(), modeRGB, {
        WebGLRenderingContextBase::FUNC_ADD,
        WebGLRenderingContextBase::FUNC_SUBTRACT,
        WebGLRenderingContextBase::FUNC_REVERSE_SUBTRACT
    }, {
        WebGL2RenderingContextBase::MIN,
        WebGL2RenderingContextBase::MAX
    })) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    /*
     * When using the EXT_blend_minmax extension:
     *   ext.MIN_EXT: Minimum of source and destination,
     *   ext.MAX_EXT: Maximum of source and destination.
    */
    if (!CheckGLenum(env, funcArg.GetThisVar(), modeAlpha, {
        WebGLRenderingContextBase::FUNC_ADD,
        WebGLRenderingContextBase::FUNC_SUBTRACT,
        WebGLRenderingContextBase::FUNC_REVERSE_SUBTRACT
    }, {
        WebGL2RenderingContextBase::MIN,
        WebGL2RenderingContextBase::MAX
    })) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    glBlendEquationSeparate(modeRGB, modeAlpha);
    return NVal::CreateNull(env).val_;
}

static bool CheckBlendFuncFactors(GLenum sFactor, GLenum dFactor)
{
    if (((sFactor == WebGLRenderingContextBase::CONSTANT_COLOR ||
            sFactor == WebGLRenderingContextBase::ONE_MINUS_CONSTANT_COLOR) &&
            (dFactor == WebGLRenderingContextBase::CONSTANT_ALPHA ||
                dFactor == WebGLRenderingContextBase::ONE_MINUS_CONSTANT_ALPHA)) ||
        ((dFactor == WebGLRenderingContextBase::CONSTANT_COLOR ||
            dFactor == WebGLRenderingContextBase::ONE_MINUS_CONSTANT_COLOR) &&
            (sFactor == WebGLRenderingContextBase::CONSTANT_ALPHA ||
                sFactor == WebGLRenderingContextBase::ONE_MINUS_CONSTANT_ALPHA))) {
        return false;
    }
    return true;
}

napi_value WebGLRenderingContextBase::BlendFunc(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLenum sFactor = 0;
    tie(succ, sFactor) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        sFactor = WebGLRenderingContextBase::ONE;
    }
    GLenum dFactor = 0;
    tie(succ, dFactor) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        dFactor = WebGLRenderingContextBase::ZERO;
    }
    LOGD("WebGL blendFunc sFactor %{public}u dFactor %{public}u", sFactor, dFactor);

    if (!CheckBlendFuncFactors(sFactor, dFactor)) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return NVal::CreateNull(env).val_;
    }
    glBlendFunc(sFactor, dFactor);
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBase::BlendFuncSeparate(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLenum srcRGB = 0;
    tie(succ, srcRGB) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        srcRGB = WebGLRenderingContextBase::ONE;
    }
    GLenum dstRGB = 0;
    tie(succ, dstRGB) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        dstRGB = WebGLRenderingContextBase::ZERO;
    }
    GLenum srcAlpha = 0;
    tie(succ, srcAlpha) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        srcAlpha = WebGLRenderingContextBase::ONE;
    }
    GLenum dstAlpha = 0;
    tie(succ, dstAlpha) = NVal(env, funcArg[NARG_POS::FOURTH]).ToGLenum();
    if (!succ) {
        dstAlpha = WebGLRenderingContextBase::ZERO;
    }
    LOGD("WebGL blendFuncSeparate srcRGB %{public}u, dstAlpha %{public}u, dstRGB %{public}u, srcAlpha %{public}u",
        srcRGB, dstAlpha, dstRGB, srcAlpha);
    if (!CheckBlendFuncFactors(srcRGB, dstRGB)) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return NVal::CreateNull(env).val_;
    }
    glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBase::CheckFramebufferStatus(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLenum target = 0;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    return !succ ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().CheckFrameBufferStatus(env, target);
}

napi_value WebGLRenderingContextBase::Clear(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    int64_t mask = 0;
    tie(succ, mask) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().Clear(env, static_cast<GLbitfield>(mask));
}

napi_value WebGLRenderingContextBase::ClearColor(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    float red = -1.0;
    tie(succ, red) = NVal(env, funcArg[NARG_POS::FIRST]).ToFloat();
    if (!succ || std::isnan(red)) {
        red = 0.0;
    }
    float green = -1.0;
    tie(succ, green) = NVal(env, funcArg[NARG_POS::SECOND]).ToFloat();
    if (!succ || std::isnan(green)) {
        green = 0.0;
    }
    float blue = -1.0;
    tie(succ, blue) = NVal(env, funcArg[NARG_POS::THIRD]).ToFloat();
    if (!succ || std::isnan(blue)) {
        blue = 0.0;
    }
    float alpha = -1.0;
    tie(succ, alpha) = NVal(env, funcArg[NARG_POS::FOURTH]).ToFloat();
    if (!succ || std::isnan(alpha)) {
        alpha = 0.0;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().ClearColor(env, static_cast<GLclampf>(red),
        static_cast<GLclampf>(green), static_cast<GLclampf>(blue), static_cast<GLclampf>(alpha));
}

napi_value WebGLRenderingContextBase::ClearDepth(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    float depth = -1.0;
    tie(succ, depth) = NVal(env, funcArg[NARG_POS::FIRST]).ToFloat();
    if (!succ || std::isnan(depth)) {
        depth = 0.0;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().ClearDepth(env, depth);
}

napi_value WebGLRenderingContextBase::ClearStencil(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    int32_t stenci = -1;
    tie(succ, stenci) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        stenci = 0;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().ClearStencil(env, stenci);
}

napi_value WebGLRenderingContextBase::ColorMask(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    bool red = false;
    tie(succ, red) = NVal(env, funcArg[NARG_POS::FIRST]).ToBool();
    if (!succ) {
        red = true;
    }
    bool green = false;
    tie(succ, green) = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (!succ) {
        green = true;
    }
    bool blue = false;
    tie(succ, blue) = NVal(env, funcArg[NARG_POS::THIRD]).ToBool();
    if (!succ) {
        blue = true;
    }
    bool alpha = false;
    tie(succ, alpha) = NVal(env, funcArg[NARG_POS::FOURTH]).ToBool();
    if (!succ) {
        alpha = true;
    }
    LOGD("WebGL colorMask red %{public}d green %{public}d blue %{public}d alpha %{public}d",
        red, green, blue, alpha);
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().ColorMask(env, static_cast<GLboolean>(red),
        static_cast<GLboolean>(green), static_cast<GLboolean>(blue), static_cast<GLboolean>(alpha));
}

napi_value WebGLRenderingContextBase::CompileShader(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().CompileShader(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGLRenderingContextBase::CopyTexImage2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::EIGHT)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    CopyTexImage2DArg imgArg = {};
    imgArg.func = Impl::IMAGE_COPY_TEX_IMAGE_2D;
    bool succ = false;
    tie(succ, imgArg.target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.internalFormat) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.x) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.y) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.width) = NVal(env, funcArg[NARG_POS::SIXTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.height) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.border) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToInt32();
    return !succ ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().CopyTexImage2D(env, imgArg);
}

napi_value WebGLRenderingContextBase::CopyTexSubImage2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::EIGHT)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    CopyTexSubImageArg imgArg = {};
    imgArg.func = Impl::IMAGE_COPY_TEX_SUB_IMAGE_2D;
    bool succ = false;
    tie(succ, imgArg.target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.xOffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.yOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.x) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.x) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.width) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.height) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToGLsizei();
    return !succ ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().CopyTexSubImage2D(env, imgArg);
}

napi_value WebGLRenderingContextBase::CreateBuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().CreateBuffer(env);
}

napi_value WebGLRenderingContextBase::CreateFramebuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().CreateFrameBuffer(env);
}

napi_value WebGLRenderingContextBase::CreateProgram(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().CreateProgram(env);
}

napi_value WebGLRenderingContextBase::CreateRenderbuffer(napi_env env, napi_callback_info info)
{
    LOGD("WebGL createRenderbuffer start");
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().CreateRenderBuffer(env);
}

napi_value WebGLRenderingContextBase::CreateShader(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLenum type = 0;
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, type) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGLRenderingContextImpl().CreateShader(env, type);
}

napi_value WebGLRenderingContextBase::CreateTexture(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().CreateTextureObject(env);
}

napi_value WebGLRenderingContextBase::CullFace(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLenum mode = 0;
    tie(succ, mode) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    LOGD("WebGL cullFace mode %{public}u", mode);
    switch (mode) {
        case WebGLRenderingContextBase::FRONT_AND_BACK:
        case WebGLRenderingContextBase::FRONT:
        case WebGLRenderingContextBase::BACK:
            break;
        default:
            context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
            return NVal::CreateNull(env).val_;
    }
    glCullFace(static_cast<GLenum>(mode));
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBase::DeleteBuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().DeleteBuffer(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGLRenderingContextBase::DeleteFramebuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().DeleteFrameBuffer(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGLRenderingContextBase::DeleteProgram(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().DeleteProgram(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGLRenderingContextBase::DeleteRenderbuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().DeleteRenderBuffer(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGLRenderingContextBase::DeleteShader(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().DeleteShader(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGLRenderingContextBase::DeleteTexture(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().DeleteTexture(env, funcArg[NARG_POS::FIRST]);
}

static bool CheckFuncForStencilOrDepth(GLenum func)
{
    switch (func) {
        case WebGLRenderingContextBase::NEVER:
        case WebGLRenderingContextBase::LESS:
        case WebGLRenderingContextBase::LEQUAL:
        case WebGLRenderingContextBase::GREATER:
        case WebGLRenderingContextBase::GEQUAL:
        case WebGLRenderingContextBase::EQUAL:
        case WebGLRenderingContextBase::NOTEQUAL:
        case WebGLRenderingContextBase::ALWAYS:
            return true;
        default:
            return false;
    }
}

napi_value WebGLRenderingContextBase::DepthFunc(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLenum func = 0;
    tie(succ, func) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ || !CheckFuncForStencilOrDepth(func)) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
    }
    LOGD("WebGL depthFunc func %{public}u", func);
    glDepthFunc(func);
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBase::DepthMask(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    bool flag = false;
    tie(succ, flag) = NVal(env, funcArg[NARG_POS::FIRST]).ToBool();
    if (!succ) {
        flag = true;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().DepthMask(env, flag);
}

napi_value WebGLRenderingContextBase::DepthRange(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    double zNear = -1.0;
    tie(succ, zNear) = NVal(env, funcArg[NARG_POS::FIRST]).ToDouble();
    if (!succ) {
        zNear = 0.0;
    }
    double zFar = -1.0;
    tie(succ, zFar) = NVal(env, funcArg[NARG_POS::SECOND]).ToDouble();
    if (!succ) {
        zFar = 1.0;
    }
    if (zNear > zFar) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return NVal::CreateNull(env).val_;
    }
    LOGD("WebGL depthRange zNear %{public}f zFar %{public}f ", zNear, zFar);
    glDepthRangef(static_cast<GLclampf>(zNear), static_cast<GLclampf>(zFar));
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBase::DetachShader(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().DetachShader(
        env, funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND]);
}

napi_value WebGLRenderingContextBase::Disable(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLenum cap = 0;
    tie(succ, cap) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        cap = 0;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().Disable(env, cap);
}

napi_value WebGLRenderingContextBase::DrawArrays(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLenum mode = 0;
    tie(succ, mode) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    int64_t first = 0;
    tie(succ, first) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLsizei count = 0;
    tie(succ, count) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLsizei();
    return !succ ? nullptr : context->GetWebGLRenderingContextImpl().DrawArrays(
        env, mode, static_cast<GLint>(first), count);
}

napi_value WebGLRenderingContextBase::DrawElements(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLenum mode = 0;
    tie(succ, mode) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    int64_t count = 0;
    tie(succ, count) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLenum type = 0;
    tie(succ, type) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    int64_t offset = 0;
    tie(succ, offset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().DrawElements(
        env, mode, static_cast<GLsizei>(count), type, static_cast<GLintptr>(offset));
}

napi_value WebGLRenderingContextBase::Enable(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLenum cap = 0;
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, cap) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGLRenderingContextImpl().Enable(env, cap);
}

napi_value WebGLRenderingContextBase::EnableVertexAttribArray(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    int64_t index = 0;
    bool succ = false;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        index = -1;
    }
    return context->GetWebGLRenderingContextImpl().EnableVertexAttribArray(env, index);
}

napi_value WebGLRenderingContextBase::DisableVertexAttribArray(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    int64_t index = 0;
    bool succ = false;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        index = -1;
    }
    return context->GetWebGLRenderingContextImpl().DisableVertexAttribArray(env, index);
}

napi_value WebGLRenderingContextBase::Finish(napi_env env, napi_callback_info info)
{
    glFinish();
    LOGD("WebGL finish end");
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBase::Flush(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        LOGE("WebGL flush context is nullptr");
        return NVal::CreateNull(env).val_;
    }
    glFlush();
    context->Update();
    LOGD("WebGL flush end");
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBase::FramebufferRenderbuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLenum target = 0;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLenum attachment = 0;
    tie(succ, attachment) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLenum renderBufferTarget = 0;
    tie(succ, renderBufferTarget) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().FrameBufferRenderBuffer(
        env, target, attachment, renderBufferTarget, funcArg[NARG_POS::FOURTH]);
}

napi_value WebGLRenderingContextBase::FramebufferTexture2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLenum target = 0;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLenum attachment = 0;
    tie(succ, attachment) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLenum textarget = 0;
    tie(succ, textarget) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    int64_t level = 0;
    tie(succ, level) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().FrameBufferTexture2D(
        env, target, attachment, textarget, funcArg[NARG_POS::FOURTH], static_cast<GLint>(level));
}

napi_value WebGLRenderingContextBase::GetUniformLocation(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        LOGE("WebGL getUniformLocation fail to get context");
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    unique_ptr<char[]> name = nullptr;
    size_t size = 0;
    tie(succ, name, size) = NVal(env, funcArg[NARG_POS::SECOND]).ToUTF8String();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    std::string str(name.get(), size);
    return context->GetWebGLRenderingContextImpl().GetUniformLocation(env, funcArg[NARG_POS::FIRST], str);
}

napi_value WebGLRenderingContextBase::GetVertexAttribOffset(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        LOGE("WebGL getUniformLocation fail to get context");
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    uint32_t index = 0;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToUint32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLenum pname = 0;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    return !succ ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().GetVertexAttribOffset(env, index, pname);
}

napi_value WebGLRenderingContextBase::ShaderSource(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    size_t size = 0;
    unique_ptr<char[]> source = nullptr;
    tie(succ, source, size) = NVal(env, funcArg[NARG_POS::SECOND]).ToUTF8String();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    LOGD("WebGL shaderSource source %{public}zu, %{public}s", size, source.get());
    std::string str(source.get(), size);
    return context->GetWebGLRenderingContextImpl().ShaderSource(env, funcArg[NARG_POS::FIRST], str);
}

napi_value WebGLRenderingContextBase::Hint(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLenum target = 0;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    switch (target) {
        case WebGLRenderingContextBase::GENERATE_MIPMAP_HINT:
            break;
        case WebGL2RenderingContextBase::FRAGMENT_SHADER_DERIVATIVE_HINT:
            if (IsHighWebGL(env, funcArg.GetThisVar())) {
                break;
            }
            [[fallthrough]];
        default:
            context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
            return NVal::CreateNull(env).val_;
    }
    GLenum mode = 0;
    tie(succ, mode) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    LOGD("WebGL hint target %{public}u, mode %{public}u", target, mode);
    switch (mode) {
        case WebGLRenderingContextBase::DONT_CARE:
        case WebGLRenderingContextBase::FASTEST:
        case WebGLRenderingContextBase::NICEST:
            break;
        default:
            context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
            return NVal::CreateNull(env).val_;
    }
    glHint(target, mode);
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBase::IsBuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    bool res = false;
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, res).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateBool(env, res).val_ :
        context->GetWebGLRenderingContextImpl().IsBuffer(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGLRenderingContextBase::IsEnabled(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    bool res = false;
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, res).val_;
    }
    bool succ = false;
    GLenum cap = 0;
    tie(succ, cap) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        cap = 0;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateBool(env, res).val_ :
        context->GetWebGLRenderingContextImpl().IsEnabled(env, cap);
}

napi_value WebGLRenderingContextBase::RenderbufferStorage(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    TexStorageArg arg = {};
    tie(succ, arg.target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, arg.internalFormat) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, arg.width) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, arg.height) = NVal(env, funcArg[NARG_POS::FOURTH]).ToGLsizei();
    return !succ ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().RenderBufferStorage(env, arg);
}

napi_value WebGLRenderingContextBase::SampleCoverage(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    double value = 0.0;
    tie(succ, value) = NVal(env, funcArg[NARG_POS::FIRST]).ToDouble();
    if (!succ) {
        value = 1.0;
    }
    bool invert = false;
    tie(succ, invert) = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    LOGD("WebGL sampleCoverage %{public}f %{public}d", value, invert);
    glSampleCoverage(static_cast<GLclampf>(value), static_cast<GLboolean>(invert));
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBase::Scissor(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    int32_t x = 0;
    tie(succ, x) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    int32_t y = 0;
    tie(succ, y) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    int64_t width = 0;
    tie(succ, width) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    int64_t height = 0;
    tie(succ, height) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    LOGD("WebGL scissor x %{public}d y %{public}d width %{public}" PRIi64 " height %{public}u" PRIi64,
        x, y, width, height);

    if (width < 0 || height < 0) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }
    glScissor(static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBase::StencilFunc(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLenum func = 0;
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, func) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    int32_t ref = 0;
    tie(succ, ref) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    uint32_t mask = 0;
    tie(succ, mask) = NVal(env, funcArg[NARG_POS::THIRD]).ToUint32();
    if (!succ) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGLRenderingContextImpl().StencilFunc(
        env, func, static_cast<GLint>(ref), static_cast<GLuint>(mask));
}

napi_value WebGLRenderingContextBase::StencilFuncSeparate(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLenum face = 0;
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, face) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    GLenum func = 0;
    tie(succ, func) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    int32_t ref = 0;
    tie(succ, ref) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    uint32_t mask = 0;
    tie(succ, mask) = NVal(env, funcArg[NARG_POS::FOURTH]).ToUint32();
    if (!succ) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGLRenderingContextImpl().StencilFuncSeparate(
        env, face, func, static_cast<GLint>(ref), static_cast<GLuint>(mask));
}

napi_value WebGLRenderingContextBase::StencilMask(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    uint32_t mask = 0;
    tie(succ, mask) = NVal(env, funcArg[NARG_POS::FIRST]).ToUint32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGLRenderingContextImpl().StencilMask(env, static_cast<GLuint>(mask));
}

napi_value WebGLRenderingContextBase::StencilMaskSeparate(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLenum face = 0;
    tie(succ, face) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    uint32_t mask = 0;
    tie(succ, mask) = NVal(env, funcArg[NARG_POS::SECOND]).ToUint32();
    if (!succ) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGLRenderingContextImpl().StencilMaskSeparate(env, face, static_cast<GLuint>(mask));
}

napi_value WebGLRenderingContextBase::StencilOp(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLenum fail = 0;
    tie(succ, fail) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        fail = WebGLRenderingContextBase::KEEP;
    }
    GLenum zfail = 0;
    tie(succ, zfail) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        zfail = WebGLRenderingContextBase::KEEP;
    }
    GLenum zpass = 0;
    tie(succ, zpass) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        zpass = WebGLRenderingContextBase::KEEP;
    }
    LOGD("WebGL stencilOp fail %{public}u zpass zfail %{public}u %{public}u", fail, zfail, zpass);
    glStencilOp(fail, zfail, zpass);
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBase::StencilOpSeparate(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLenum face = 0;
    tie(succ, face) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }

    if (!CheckGLenum(env, funcArg.GetThisVar(), face, {
        WebGLRenderingContextBase::FRONT,
        WebGLRenderingContextBase::BACK,
        WebGLRenderingContextBase::FRONT_AND_BACK
    }, {})) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }

    GLenum sfail = 0;
    tie(succ, sfail) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        sfail = WebGLRenderingContextBase::KEEP;
    }
    GLenum dpfail = 0;
    tie(succ, dpfail) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        dpfail = WebGLRenderingContextBase::KEEP;
    }
    GLenum dppass = 0;
    tie(succ, dppass) = NVal(env, funcArg[NARG_POS::FOURTH]).ToGLenum();
    if (!succ) {
        dppass = WebGLRenderingContextBase::KEEP;
    }
    LOGD("WebGL stencilOpSeparate face %{public}u sfail %{public}u dpfail %{public}u dppass %{public}u",
        face, sfail, dpfail, dppass);
    glStencilOpSeparate(face, sfail, dpfail, dppass);
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBase::TexParameterf(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLenum target = 0;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLenum pname = 0;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    double param = 0.0;
    tie(succ, param) = NVal(env, funcArg[NARG_POS::THIRD]).ToDouble();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().TexParameterf(env, target, pname, static_cast<GLfloat>(param));
}

napi_value WebGLRenderingContextBase::TexParameteri(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLenum target = 0;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLenum pname = 0;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    int32_t param = 0;
    tie(succ, param) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().TexParameterf(env, target, pname, static_cast<GLint>(param));
}

napi_value WebGLRenderingContextBase::Uniform1f(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLfloat data[4] = { 0 }; // max len 4
    tie(succ, data[0]) = NVal(env, funcArg[NARG_POS::SECOND]).ToFloat();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().UniformF(env, funcArg[NARG_POS::FIRST], 1, data);
}

napi_value WebGLRenderingContextBase::Uniform2f(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    const uint32_t count = 2; // 2 is 2x2
    GLfloat data[4] = {}; // max len 4
    for (size_t i = 0; i < count; i++) {
        tie(succ, data[i]) = NVal(env, funcArg[i + NARG_POS::SECOND]).ToFloat();
        if (!succ) {
            LOGE("WebGL uniform2f invalid arg");
            return NVal::CreateNull(env).val_;
        }
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().UniformF(env, funcArg[NARG_POS::FIRST], count, data);
}

napi_value WebGLRenderingContextBase::Uniform3f(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    const uint32_t count = 3; // 3 is 3x3
    GLfloat data[4] = {}; // max len 4
    for (size_t i = 0; i < count; i++) {
        tie(succ, data[i]) = NVal(env, funcArg[i + NARG_POS::SECOND]).ToFloat();
        if (!succ) {
            LOGE("WebGL uniform3f invalid arg");
            return NVal::CreateNull(env).val_;
        }
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().UniformF(env, funcArg[NARG_POS::FIRST], count, data);
}

napi_value WebGLRenderingContextBase::Uniform4f(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    const uint32_t count = 4; // 4 is 4x4
    GLfloat data[4] = {}; // max len 4
    for (size_t i = 0; i < count; i++) {
        tie(succ, data[i]) = NVal(env, funcArg[i + NARG_POS::SECOND]).ToFloat();
        if (!succ) {
            LOGE("WebGL uniform4f invalid arg");
            return NVal::CreateNull(env).val_;
        }
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().UniformF(env, funcArg[NARG_POS::FIRST], count, data);
}

napi_value WebGLRenderingContextBase::Uniform1i(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    const uint32_t count = 1;
    GLint data[4] = {}; // max len 4
    for (size_t i = 0; i < count; i++) {
        tie(succ, data[i]) = NVal(env, funcArg[i + NARG_POS::SECOND]).ToInt32();
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().UniformI(env, funcArg[NARG_POS::FIRST], count, data);
}

napi_value WebGLRenderingContextBase::Uniform2i(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    const uint32_t count = 2; // 2 is 2x2
    GLint data[4] = {}; // max len 4
    for (size_t i = 0; i < count; i++) {
        tie(succ, data[i]) = NVal(env, funcArg[i + NARG_POS::SECOND]).ToInt32();
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().UniformI(env, funcArg[NARG_POS::FIRST], count, data);
}

napi_value WebGLRenderingContextBase::Uniform3i(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    const uint32_t count = 3; // 3 is 3x3
    GLint data[4] = {}; // max len 4
    for (size_t i = 0; i < count; i++) {
        tie(succ, data[i]) = NVal(env, funcArg[i + NARG_POS::SECOND]).ToInt32();
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().UniformI(env, funcArg[NARG_POS::FIRST], count, data);
}

napi_value WebGLRenderingContextBase::Uniform4i(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    const uint32_t count = 4; // 4 is 4x4
    GLint data[4] = {}; // max len 4
    for (size_t i = 0; i < count; i++) {
        tie(succ, data[i]) = NVal(env, funcArg[i + NARG_POS::SECOND]).ToInt32();
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().UniformI(env, funcArg[NARG_POS::FIRST], count, data);
}

napi_value WebGLRenderingContextBase::UseProgram(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().UseProgram(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGLRenderingContextBase::ValidateProgram(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, funcArg[NARG_POS::FIRST]);
    if (webGLProgram == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    uint32_t program = webGLProgram->GetProgramId();
    LOGD("WebGL validateProgram program %{public}u", program);
    glValidateProgram(static_cast<GLuint>(program));
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBase::VertexAttrib1f(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }

    const int32_t count = 1;
    GLfloat data[count] = { 0 };
    GLuint index = 0;
    bool succ = false;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToUint32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    for (int32_t i = 0; i < count; i++) {
        tie(succ, data[i]) = NVal(env, funcArg[i + NARG_POS::SECOND]).ToFloat();
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().VertexAttribf(env, index, count, data);
}

napi_value WebGLRenderingContextBase::VertexAttrib2f(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return NVal::CreateNull(env).val_;
    }

    const int32_t count = 2; // 2 is 2x2
    GLfloat data[count] = { 0 };
    GLuint index = 0;
    bool succ = false;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToUint32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    for (int32_t i = 0; i < count; i++) {
        tie(succ, data[i]) = NVal(env, funcArg[i + NARG_POS::SECOND]).ToFloat();
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().VertexAttribf(env, index, count, data);
}

napi_value WebGLRenderingContextBase::VertexAttrib3f(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return NVal::CreateNull(env).val_;
    }

    const int32_t count = 3; // 3 is 3x3
    GLfloat data[count] = { 0 };
    GLuint index = 0;
    bool succ = false;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToUint32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    for (int32_t i = 0; i < count; i++) {
        tie(succ, data[i]) = NVal(env, funcArg[i + NARG_POS::SECOND]).ToFloat();
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().VertexAttribf(env, index, count, data);
}

napi_value WebGLRenderingContextBase::VertexAttrib4f(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return NVal::CreateNull(env).val_;
    }

    const int32_t count = 4; // 4 is 4x4
    GLfloat data[count] = { 0 };
    GLuint index = 0;
    bool succ = false;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToUint32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    for (int32_t i = 0; i < count; i++) {
        tie(succ, data[i]) = NVal(env, funcArg[i + NARG_POS::SECOND]).ToFloat();
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().VertexAttribf(env, index, count, data);
}

napi_value WebGLRenderingContextBase::VertexAttribPointer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::SIX)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    VertexAttribArg vertexInfo = { 0 };
    tie(succ, vertexInfo.index) = NVal(env, funcArg[NARG_POS::FIRST]).ToUint32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, vertexInfo.size) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, vertexInfo.type) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, vertexInfo.normalized) = NVal(env, funcArg[NARG_POS::FOURTH]).ToBool();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, vertexInfo.stride) = NVal(env, funcArg[NARG_POS::FIFTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLenum result;
    tie(result, vertexInfo.offset) = WebGLArg::ToGLintptr(env, funcArg[NARG_POS::SIXTH]);
    if (result) {
        context->GetWebGLRenderingContextImpl().SetError(result);
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGLRenderingContextImpl().VertexAttribPointer(env, vertexInfo);
}

napi_value WebGLRenderingContextBase::Viewport(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    int32_t x = 0;
    tie(succ, x) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    int32_t y = 0;
    tie(succ, y) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    int64_t width = 0;
    tie(succ, width) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    int64_t height = 0;
    tie(succ, height) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    LOGD("webgl viewport x %{public}d y %{public}d width %{public}" PRIi64 " height %{public}" PRIi64,
        x, y, width, height);
    if (width < 0 || height < 0) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }
    glViewport(static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBase::IsFramebuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    bool res = false;
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, res).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateBool(env, res).val_ :
        context->GetWebGLRenderingContextImpl().IsFrameBuffer(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGLRenderingContextBase::IsProgram(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    bool res = false;
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, res).val_;
    }

    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, funcArg[NARG_POS::FIRST]);
    if (webGLProgram == nullptr) {
        return NVal::CreateBool(env, res).val_;
    }
    uint32_t program = webGLProgram->GetProgramId();
    res = static_cast<bool>(glIsProgram(static_cast<GLuint>(program)));
    LOGD("WebGL isProgram programId %{public}u res %{public}d", program, res);
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGLRenderingContextBase::IsRenderbuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    bool res = false;
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, res).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateBool(env, res).val_;
    }
    WebGLRenderbuffer* webGLRenderbuffer = context->GetWebGLRenderingContextImpl().GetValidRenderBuffer(
        env, funcArg[NARG_POS::FIRST]);
    if (webGLRenderbuffer == nullptr) {
        return NVal::CreateBool(env, res).val_;
    }
    uint32_t renderbuffer = webGLRenderbuffer->GetRenderbuffer();
    res = static_cast<bool>(glIsRenderbuffer(static_cast<GLuint>(renderbuffer)));
    LOGD("WebGL isRenderbuffer renderbuffer %{public}u res %{public}d", renderbuffer, res);
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGLRenderingContextBase::IsShader(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    bool res = false;
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, res).val_;
    }

    WebGLShader* webGLShader = WebGLShader::GetObjectInstance(env, funcArg[NARG_POS::FIRST]);
    if (webGLShader == nullptr) {
        LOGE("webGLShader is nullptr.");
        return NVal::CreateBool(env, res).val_;
    }
    uint32_t shaderId = webGLShader->GetShaderId();
    res = static_cast<bool>(glIsShader(static_cast<GLuint>(shaderId)));
    LOGD("WebGL isShader shaderId %{public}u res %{public}d", shaderId, res);
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGLRenderingContextBase::IsTexture(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    bool res = false;
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, res).val_;
    }

    WebGLTexture* webGLTexture = WebGLTexture::GetObjectInstance(env, funcArg[NARG_POS::FIRST]);
    if (webGLTexture == nullptr) {
        return NVal::CreateBool(env, res).val_;
    }
    uint32_t texture = webGLTexture->GetTexture();
    res = static_cast<bool>(glIsTexture(static_cast<GLuint>(texture)));
    LOGD("WebGL isTexture texture %{public}u res %{public}d", texture, res);
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGLRenderingContextBase::LineWidth(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    double linewidth = 0.0;
    tie(succ, linewidth) = NVal(env, funcArg[NARG_POS::FIRST]).ToDouble();
    if (!succ) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }
    LOGD("WebGL lineWidth linewidth %{public}f", linewidth);
    glLineWidth(static_cast<GLfloat>(linewidth));
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBase::LinkProgram(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGLRenderingContextImpl().LinkProgram(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGLRenderingContextBase::PixelStorei(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLenum pname = 0;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    GLint param = 0;
    bool isbool = NVal(env, funcArg[NARG_POS::SECOND]).TypeIs(napi_boolean);
    if (isbool) {
        bool parambool = false;
        tie(succ, parambool) = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
        if (!succ) {
            context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_VALUE);
            return NVal::CreateNull(env).val_;
        }
        param = parambool ? 1 : 0;
    } else {
        tie(succ, param) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
        if (!succ) {
            context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_VALUE);
            return NVal::CreateNull(env).val_;
        }
    }
    context->GetWebGLRenderingContextImpl().PixelStorei(env, pname, param);
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBase::PolygonOffset(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    double factor = 0.0;
    tie(succ, factor) = NVal(env, funcArg[NARG_POS::FIRST]).ToDouble();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    double units = 0.0;
    tie(succ, units) = NVal(env, funcArg[NARG_POS::SECOND]).ToDouble();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    LOGD("WebGL polygonOffset factor %{public}f units %{public}f", factor, units);
    glPolygonOffset(static_cast<GLfloat>(factor), static_cast<GLfloat>(units));
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBase::FrontFace(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLenum mode = 0;
    tie(succ, mode) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    LOGD("WebGL frontFace mode %{public}u", mode);
    switch (mode) {
        case WebGLRenderingContextBase::CW:
        case WebGLRenderingContextBase::CCW:
            break;
        default:
            context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
            return NVal::CreateNull(env).val_;
    }
    glFrontFace(mode);
    return NVal::CreateNull(env).val_;
}

napi_value WebGLRenderingContextBase::GenerateMipmap(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLenum target = 0;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGLRenderingContextImpl().GenerateMipmap(env, target);
}

static napi_value CreateWebGLActiveInfo(napi_env env, napi_callback_info info,
    void (*handleActiveInfo)(GLuint programId, GLuint index, GLint& size, GLenum& type, GLchar* name))
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        LOGE("WebGL getActiveAttrib invalid param");
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, funcArg[NARG_POS::FIRST]);
    if (webGLProgram == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    uint32_t programId = webGLProgram->GetProgramId();
    int64_t index = 0;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    LOGD("WebGL CreateWebGLActiveInfo programId %{public}u index %{public}" PRIi64, programId, index);
    WebGLActiveInfo* webGLActiveInfo = nullptr;
    napi_value objActiveInfo = WebGLActiveInfo::CreateObjectInstance(env, &webGLActiveInfo).val_;
    if (!objActiveInfo) {
        return NVal::CreateNull(env).val_;
    }
    GLint size = 0;
    GLenum type = 0;
    GLchar name[WEBGL_ACTIVE_INFO_NAME_MAX_LENGTH + 1] = { 0 };
    handleActiveInfo(static_cast<GLuint>(programId), static_cast<GLuint>(index), size, type, name);
    webGLActiveInfo->SetActiveName(name, WEBGL_ACTIVE_INFO_NAME_MAX_LENGTH);
    webGLActiveInfo->SetActiveType(type);
    webGLActiveInfo->SetActiveSize(size);
    return objActiveInfo;
}

napi_value WebGLRenderingContextBase::GetActiveAttrib(napi_env env, napi_callback_info info)
{
    return CreateWebGLActiveInfo(
        env, info, [](GLuint programId, GLuint index, GLint& size, GLenum& type, GLchar* name) {
            GLsizei bufSize = WEBGL_ACTIVE_INFO_NAME_MAX_LENGTH;
            GLsizei length;
            glGetActiveAttrib(programId, index, bufSize, &length, &size, &type, name);
            if (length > WEBGL_ACTIVE_INFO_NAME_MAX_LENGTH) {
                LOGE("WebGL: getActiveAttrib: error bufSize exceed!");
            }
            LOGD("WebGL getActiveAttrib %{public}s %{public}u %{public}d %{public}u", name, type, size, length);
        });
}

napi_value WebGLRenderingContextBase::GetActiveUniform(napi_env env, napi_callback_info info)
{
    return CreateWebGLActiveInfo(
        env, info, [](GLuint programId, GLuint index, GLint& size, GLenum& type, GLchar* name) {
            GLsizei bufSize = WEBGL_ACTIVE_INFO_NAME_MAX_LENGTH;
            GLsizei length;
            glGetActiveUniform(programId, index, bufSize, &length, &size, &type, name);
            if (length > WEBGL_ACTIVE_INFO_NAME_MAX_LENGTH) {
                LOGE("WebGL: getActiveUniform: error bufSize exceed!");
            }
            LOGD("WebGL GetActiveUniform %{public}s %{public}u %{public}d", name, type, size);
        });
}

napi_value WebGLRenderingContextBase::GetBufferParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLenum target = 0;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLenum pname = 0;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateInt64(env, WebGLRenderingContextBase::INVALID_OPERATION).val_ :
        context->GetWebGLRenderingContextImpl().GetBufferParameter(env, target, pname);
}

napi_value WebGLRenderingContextBase::GetError(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    funcArg.InitArgs(NARG_CNT::ZERO);
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateInt64(env, WebGLRenderingContextBase::INVALID_OPERATION).val_ :
        context->GetWebGLRenderingContextImpl().GetError(env);
}

napi_value WebGLRenderingContextBase::GetFramebufferAttachmentParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLenum target = 0;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLenum attachment = 0;
    tie(succ, attachment) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLenum pname = 0;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().GetFrameBufferAttachmentParameter(env, target, attachment, pname);
}

napi_value WebGLRenderingContextBase::GetProgramParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLenum pname = 0;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    return !succ ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().GetProgramParameter(env, funcArg[NARG_POS::FIRST], pname);
}

napi_value WebGLRenderingContextBase::GetRenderbufferParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLenum target = 0;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLenum pname = 0;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().GetRenderBufferParameter(env, target, pname);
}

napi_value WebGLRenderingContextBase::GetTexParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLenum target = 0;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    LOGD("WebGL getTexParameter target %{public}u", target);
    GLenum pname = 0;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGLRenderingContextImpl().GetTexParameter(env, target, pname);
}

napi_value WebGLRenderingContextBase::GetShaderParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLenum pname = 0;
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGLRenderingContextImpl().GetShaderParameter(env, funcArg[NARG_POS::FIRST], pname);
}

napi_value WebGLRenderingContextBase::GetAttribLocation(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateInt64(env, -1).val_;
    }

    bool succ = false;
    unique_ptr<char[]> name = nullptr;
    size_t size = 0;
    tie(succ, name, size) = NVal(env, funcArg[NARG_POS::SECOND]).ToUTF8String();
    if (!succ) {
        return NVal::CreateInt64(env, -1).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    std::string str(name.get(), size);
    return context->GetWebGLRenderingContextImpl().GetAttribLocation(env, funcArg[NARG_POS::FIRST], str);
}

napi_value WebGLRenderingContextBase::VertexAttrib1fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    int64_t index = 0;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGLRenderingContextImpl().VertexAttribfv(env, index, 1, funcArg[NARG_POS::SECOND]);
}

napi_value WebGLRenderingContextBase::VertexAttrib2fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    int64_t index = 0;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().VertexAttribfv(env, index, 2, funcArg[NARG_POS::SECOND]); // 2 count
}

napi_value WebGLRenderingContextBase::VertexAttrib3fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    int64_t index = 0;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().VertexAttribfv(env, index, 3, funcArg[NARG_POS::SECOND]); // 3 count
}

napi_value WebGLRenderingContextBase::VertexAttrib4fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    int64_t index = 0;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().VertexAttribfv(env, index, 4, funcArg[NARG_POS::SECOND]); // 4 count
}

napi_value WebGLRenderingContextBase::GetVertexAttrib(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLuint index = 0;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToUint32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLenum pname = 0;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().GetVertexAttrib(env, pname, index);
}

napi_value WebGLRenderingContextBase::GetParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLenum pname = 0;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().GetParameter(env, pname);
}

napi_value WebGLRenderingContextBase::GetAttachedShaders(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().GetAttachedShaders(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGLRenderingContextBase::GetShaderPrecisionFormat(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLenum shaderType = 0;
    tie(succ, shaderType) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    LOGD("WebGL getShaderPrecisionFormat shaderType %{public}u", shaderType);
    GLenum precisionType;
    tie(succ, precisionType) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().GetShaderPrecisionFormat(env, shaderType, precisionType);
}

napi_value WebGLRenderingContextBase::GetShaderInfoLog(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateUTF8String(env, "").val_;
    }
    WebGLShader* webGlShader = WebGLShader::GetObjectInstance(env, funcArg[NARG_POS::FIRST]);
    if (webGlShader == nullptr) {
        return NVal::CreateUTF8String(env, "").val_;
    }
    GLuint shaderId = webGlShader->GetShaderId();
    GLint length = 0;
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
    if (length == 0) {
        return NVal::CreateUTF8String(env, "").val_;
    }
    GLsizei size = 0;
    std::unique_ptr<char[]> buf = std::make_unique<char[]>(length + 1);
    if (buf == nullptr) {
        return NVal::CreateUTF8String(env, "").val_;
    }
    glGetShaderInfoLog(shaderId, length, &size, buf.get());
    if (size >= length) {
        return NVal::CreateUTF8String(env, "").val_;
    }
    string str = buf.get();
    LOGD("WebGL getShaderInfoLog shaderId %{public}u length %{public}d %{public}zu %{public}s",
        shaderId, length, str.size(), str.c_str());
    return NVal::CreateUTF8String(env, str).val_;
}

napi_value WebGLRenderingContextBase::GetProgramInfoLog(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateUTF8String(env, "").val_;
    }
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, funcArg[NARG_POS::FIRST]);
    if (webGLProgram == nullptr) {
        return NVal::CreateUTF8String(env, "").val_;
    }

    GLuint programId = webGLProgram->GetProgramId();
    GLint length = 0;
    GLsizei size = 0;
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &length);
    if (length == 0) {
        return NVal::CreateUTF8String(env, "").val_;
    }
    std::unique_ptr<char[]> buf = std::make_unique<char[]>(length + 1);
    if (buf == nullptr) {
        return NVal::CreateUTF8String(env, "").val_;
    }
    glGetProgramInfoLog(programId, length, &size, buf.get());
    if (size >= length) {
        return NVal::CreateUTF8String(env, "").val_;
    }
    string str = buf.get();
    LOGD("WebGL getProgramInfoLog programId %{public}u length %{public}d %{public}zu %{public}s",
        programId, length, str.size(), str.c_str());
    return NVal::CreateUTF8String(env, str).val_;
}

napi_value WebGLRenderingContextBase::GetShaderSource(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateUTF8String(env, "").val_;
    }

    WebGLShader* webGlShader = WebGLShader::GetObjectInstance(env, funcArg[NARG_POS::FIRST]);
    if (webGlShader == nullptr) {
        return NVal::CreateUTF8String(env, "").val_;
    }
    uint32_t shaderId = webGlShader->GetShaderId();
    LOGD("WebGL getShaderSource shaderId %{public}u", shaderId);
    return NVal::CreateUTF8String(env, webGlShader->GetShaderRes()).val_;
}

napi_value WebGLRenderingContextBase::GetUniform(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGLRenderingContextImpl().GetUniform(env, funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND]);
}

napi_value WebGLRenderingContextBase::GetDrawingBufferWidth(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return NVal::CreateInt64(env, 0).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateInt64(env, 0).val_ :
        NVal::CreateInt64(env, context->GetBufferWidth()).val_;
}

napi_value WebGLRenderingContextBase::GetDrawingBufferHeight(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return NVal::CreateInt64(env, 0).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateInt64(env, 0).val_ :
        NVal::CreateInt64(env, context->GetBufferHeight()).val_;
}

void WebGLRenderingContextBase::SetError(napi_env env, napi_value thisVar, GLenum error, std::string func, int32_t line)
{
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, thisVar);
    if (context == nullptr) {
        return;
    }
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        LOGE("SetError last error %{public}u", err);
    }
    LOGE("Set error %{public}u for %{public}s:%{public}d ", error, func.c_str(), line);
    context->GetWebGLRenderingContextImpl().SetError(error);
}
} // namespace Rosen
} // namespace OHOS
#ifdef __cplusplus
}
#endif
