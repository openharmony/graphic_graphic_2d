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
    };
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGLRenderingContextBase::GetSupportedExtensions(napi_env env, napi_callback_info info)
{
    if (EglManager::GetInstance().GetEGLContext() == nullptr) {
        return NVal::CreateNull(env).val_;
    };
    const char* extensions = eglQueryString(EglManager::GetInstance().GetEGLDisplay(), EGL_EXTENSIONS);
    string str = extensions;
    vector<string> vec;
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
    return nullptr;
}

napi_value WebGLRenderingContextBase::GetExtension(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ;
    unique_ptr<char[]> name;
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
        return nullptr;
    }
    bool succ = false;
    GLenum texture = 0;
    tie(succ, texture) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().ActiveTexture(env, texture);
}

napi_value WebGLRenderingContextBase::AttachShader(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().AttachShader(
        env, funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND]);
}

napi_value WebGLRenderingContextBase::BindAttribLocation(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }

    bool succ = false;
    int64_t index = 0;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (index < 0) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    unique_ptr<char[]> name;
    uint32_t nameLen = 0;
    tie(succ, name, nameLen) = NVal(env, funcArg[NARG_POS::THIRD]).ToUTF8String();
    if (!succ) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    std::string str(name.get(), nameLen);
    return context->GetWebGLRenderingContextImpl().BindAttribLocation(
        env, funcArg[NARG_POS::FIRST], static_cast<GLuint>(index), str);
}

napi_value WebGLRenderingContextBase::BindBuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    GLenum target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context != nullptr) {
        context->GetWebGLRenderingContextImpl().BindBuffer(env, target, funcArg[NARG_POS::SECOND]);
    }
    return nullptr;
}

napi_value WebGLRenderingContextBase::BindFramebuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    GLenum target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context != nullptr) {
        context->GetWebGLRenderingContextImpl().BindFrameBuffer(env, target, funcArg[NARG_POS::SECOND]);
    }
    return nullptr;
}

napi_value WebGLRenderingContextBase::BindRenderbuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    GLenum target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context != nullptr) {
        context->GetWebGLRenderingContextImpl().BindRenderBuffer(env, target, funcArg[NARG_POS::SECOND]);
    }
    return nullptr;
}

napi_value WebGLRenderingContextBase::BindTexture(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    GLenum target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().BindTexture(env, target, funcArg[NARG_POS::SECOND]);
}

napi_value WebGLRenderingContextBase::BlendColor(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }

    bool succ = false;
    double red;
    tie(succ, red) = NVal(env, funcArg[NARG_POS::FIRST]).ToDouble();
    double green;
    tie(succ, green) = NVal(env, funcArg[NARG_POS::SECOND]).ToDouble();
    double blue;
    tie(succ, blue) = NVal(env, funcArg[NARG_POS::THIRD]).ToDouble();
    double alpha;
    tie(succ, alpha) = NVal(env, funcArg[NARG_POS::FOURTH]).ToDouble();

    LOGD("WebGL blendColor %{public}f %{public}f %{public}f %{public}f ", static_cast<GLclampf>(red),
        static_cast<GLclampf>(green), static_cast<GLclampf>(blue), static_cast<GLclampf>(alpha));
    glBlendColor(static_cast<GLclampf>(red), static_cast<GLclampf>(green), static_cast<GLclampf>(blue),
        static_cast<GLclampf>(alpha));
    return nullptr;
}

napi_value WebGLRenderingContextBase::BlendEquation(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    bool succ = false;
    GLenum mode;
    tie(succ, mode) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
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
        return nullptr;
    }
    glBlendEquation(mode);
    return nullptr;
}

napi_value WebGLRenderingContextBase::BlendEquationSeparate(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    bool succ = false;
    GLenum modeRGB;
    tie(succ, modeRGB) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    GLenum modeAlpha;
    tie(succ, modeAlpha) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
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
        return nullptr;
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
        return nullptr;
    }
    glBlendEquationSeparate(modeRGB, modeAlpha);
    return nullptr;
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
        return nullptr;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }

    bool succ = false;
    GLenum sFactor;
    tie(succ, sFactor) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        sFactor = WebGLRenderingContextBase::ONE;
    }
    LOGD("WebGL blendFunc sFactor %{public}u", sFactor);
    GLenum dFactor;
    tie(succ, dFactor) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        dFactor = WebGLRenderingContextBase::ZERO;
    }
    LOGD("WebGL blendFunc dFactor %{public}u", dFactor);

    if (!CheckBlendFuncFactors(sFactor, dFactor)) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    glBlendFunc(sFactor, dFactor);
    return nullptr;
}

napi_value WebGLRenderingContextBase::BlendFuncSeparate(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    bool succ = false;
    GLenum srcRGB;
    tie(succ, srcRGB) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        srcRGB = WebGLRenderingContextBase::ONE;
    }
    GLenum dstRGB;
    tie(succ, dstRGB) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        dstRGB = WebGLRenderingContextBase::ZERO;
    }
    GLenum srcAlpha;
    tie(succ, srcAlpha) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        srcAlpha = WebGLRenderingContextBase::ONE;
    }
    GLenum dstAlpha;
    tie(succ, dstAlpha) = NVal(env, funcArg[NARG_POS::FOURTH]).ToGLenum();
    if (!succ) {
        dstAlpha = WebGLRenderingContextBase::ZERO;
    }
    LOGD("WebGL blendFuncSeparate srcRGB %{public}u, dstAlpha %{public}u, dstRGB %{public}u, srcAlpha %{public}u",
        srcRGB, dstAlpha, dstRGB, srcAlpha);
    if (!CheckBlendFuncFactors(srcRGB, dstRGB)) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
    return nullptr;
}

napi_value WebGLRenderingContextBase::CheckFramebufferStatus(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }

    bool succ = false;
    GLenum target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    return context->GetWebGLRenderingContextImpl().CheckFrameBufferStatus(env, target);
}

napi_value WebGLRenderingContextBase::Clear(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }

    bool succ = false;
    int64_t mask;
    tie(succ, mask) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return nullptr;
    }
    return context->GetWebGLRenderingContextImpl().Clear(env, static_cast<GLbitfield>(mask));
}

} // namespace Rosen
} // namespace OHOS
#ifdef __cplusplus
}
#endif