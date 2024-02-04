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

#include "context/webgl_rendering_context_overloads.h"

#include <string>
#include <vector>

#include "context/webgl_rendering_context.h"
#include "context/webgl_rendering_context_base_impl.h"
#include "napi/n_func_arg.h"
#include "util/log.h"
#include "util/util.h"
#include "webgl/webgl_arg.h"
#include "webgl/webgl_buffer.h"
#include "webgl/webgl_framebuffer.h"
#include "webgl/webgl_program.h"
#include "webgl/webgl_renderbuffer.h"
#include "webgl/webgl_shader.h"
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

static int64_t GetOffset(napi_env env, napi_value data, WebGLRenderingContext* context)
{
    if (!NVal(env, data).IsNull()) {
        int64_t srcOffset = 0;
        bool succ = false;
        tie(succ, srcOffset) = NVal(env, data).ToInt64();
        if (!succ || srcOffset < 0) {
            context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_VALUE);
            return -1;
        }
    }
    return 0;
}

napi_value WebGLRenderingContextOverloads::BufferData(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FIVE)) {
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
    GLenum usage = 0;
    tie(succ, usage) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    bool usageSucc = NVal(env, funcArg[NARG_POS::SECOND]).TypeIs(napi_number);
    if (usageSucc) {
        int64_t size;
        tie(succ, size) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
        return context->GetWebGLRenderingContextImpl().BufferData(env, target, size, usage);
    }

    BufferExt ext {};
    if (!context->GetWebGLRenderingContextImpl().IsHighWebGL()) {
        return context->GetWebGLRenderingContextImpl().BufferData(env, target, funcArg[NARG_POS::SECOND], usage, ext);
    }
    succ = ext.GetBufferExt(env, funcArg[NARG_POS::FOURTH], funcArg[NARG_POS::FIFTH]);
    if (!succ) {
        context->GetWebGLRenderingContextImpl().SetError(GL_INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGLRenderingContextImpl().BufferData(env, target, funcArg[NARG_POS::SECOND], usage, ext);
}

napi_value WebGLRenderingContextOverloads::BufferSubData(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FIVE)) {
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
    int64_t offset = 0;
    tie(succ, offset) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    BufferExt ext {};
    succ = ext.GetBufferExt(env, funcArg[NARG_POS::FOURTH], funcArg[NARG_POS::FIFTH]);
    if (!succ) {
        context->GetWebGLRenderingContextImpl().SetError(GL_INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGLRenderingContextImpl().BufferSubData(env, target,
        static_cast<GLintptr>(offset), funcArg[NARG_POS::THIRD], ext);
}

napi_value WebGLRenderingContextOverloads::CompressedTexImage2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::SEVEN, NARG_CNT::NINE)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    TexImageArg imgArg = {};
    imgArg.func = Impl::IMAGE_COMPRESSED_TEX_IMAGE_2D;
    bool succ = false;
    tie(succ, imgArg.target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    tie(succ, imgArg.level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.internalFormat) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    tie(succ, imgArg.width) = NVal(env, funcArg[NARG_POS::FOURTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.height) = NVal(env, funcArg[NARG_POS::FIFTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.border) = NVal(env, funcArg[NARG_POS::SIXTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    imgArg.format = imgArg.internalFormat;
    bool usageNumber = NVal(env, funcArg[NARG_POS::SEVENTH]).TypeIs(napi_number);
    if (context->GetWebGLRenderingContextImpl().IsHighWebGL() && usageNumber) {
        int64_t imageSize = 0;
        tie(succ, imageSize) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToInt64();
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
        int64_t offset = 0;
        tie(succ, offset) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToInt64();
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
        return context->GetWebGLRenderingContextImpl().CompressedTexImage2D(env,
            imgArg, static_cast<GLsizei>(imageSize), static_cast<GLintptr>(offset));
    }

    GLuint srcOffset = 0;
    if (funcArg[NARG_POS::EIGHTH] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToUint32();
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
    }
    GLuint srcLengthOverride = 0;
    if (funcArg[NARG_POS::NINTH] != nullptr) {
        tie(succ, srcLengthOverride) = NVal(env, funcArg[NARG_POS::NINTH]).ToUint32();
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
    }
    return context->GetWebGLRenderingContextImpl().CompressedTexImage2D(env,
        imgArg, funcArg[NARG_POS::SEVENTH], srcOffset, srcLengthOverride);
}

napi_value WebGLRenderingContextOverloads::CompressedTexSubImage2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::EIGHT, NARG_CNT::TEN)) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }

    TexSubImage2DArg imgArg = {};
    imgArg.func = Impl::IMAGE_COMPRESSED_TEX_SUB_IMAGE_2D;
    bool succ = false;
    tie(succ, imgArg.target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
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
    tie(succ, imgArg.width) = NVal(env, funcArg[NARG_POS::FIFTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.height) = NVal(env, funcArg[NARG_POS::SIXTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.format) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToGLenum();
    imgArg.internalFormat = imgArg.format;
    bool usageNumber = NVal(env, funcArg[NARG_POS::EIGHTH]).TypeIs(napi_number);
    if (context->GetWebGLRenderingContextImpl().IsHighWebGL() && usageNumber) {
        int64_t imageSize = 0;
        tie(succ, imageSize) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToInt64();
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
        int64_t offset = 0;
        tie(succ, offset) = NVal(env, funcArg[NARG_POS::NINTH]).ToInt64();
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
        return context->GetWebGLRenderingContextImpl().CompressedTexSubImage2D(env,
            imgArg, static_cast<GLsizei>(imageSize), static_cast<GLintptr>(offset));
    }

    GLuint srcOffset = 0;
    if (funcArg[NARG_POS::NINTH] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToUint32();
    }
    GLuint srcLengthOverride = 0;
    if (funcArg[NARG_POS::TENTH] != nullptr) {
        tie(succ, srcLengthOverride) = NVal(env, funcArg[NARG_POS::NINTH]).ToUint32();
    }
    return context->GetWebGLRenderingContextImpl().CompressedTexSubImage2D(env,
        imgArg, funcArg[NARG_POS::EIGHTH], srcOffset, srcLengthOverride);
}

napi_value WebGLRenderingContextOverloads::ReadPixels(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::SEVEN, NARG_CNT::EIGHT)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    PixelsArg arg = { 0 };
    tie(succ, arg.x) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, arg.y) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, arg.width) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, arg.height) = NVal(env, funcArg[NARG_POS::FOURTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, arg.format) = NVal(env, funcArg[NARG_POS::FIFTH]).ToGLenum();
    if (!succ) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    tie(succ, arg.type) = NVal(env, funcArg[NARG_POS::SIXTH]).ToGLenum();
    if (!succ) {
        context->GetWebGLRenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    if (NVal(env, funcArg[NARG_POS::SEVENTH]).TypeIs(napi_number)) {
        GLintptr offset;
        GLenum result;
        tie(result, offset) = WebGLArg::ToGLintptr(env, funcArg[NARG_POS::SEVENTH]);
        if (result != 0) {
            context->GetWebGLRenderingContextImpl().SetError(result);
            return NVal::CreateNull(env).val_;
        }
        return context->GetWebGLRenderingContextImpl().ReadPixels(env, arg, offset);
    }
    GLuint dstOffset = 0;
    if (!NVal(env, funcArg[NARG_POS::EIGHTH]).IsNull()) {
        tie(succ, dstOffset) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToUint32();
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
    }
    return context->GetWebGLRenderingContextImpl().ReadPixels(env, arg, funcArg[NARG_POS::SEVENTH], dstOffset);
}

napi_value WebGLRenderingContextOverloads::TexImage2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::SIX, NARG_CNT::TEN)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    TexImageArg imgArg = {};
    imgArg.func = Impl::IMAGE_TEX_IMAGE_2D;
    imgArg.depth = 1;
    bool succ = false;
    tie(succ, imgArg.target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.internalFormat) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    bool usageNumber = NVal(env, funcArg[NARG_POS::SIXTH]).TypeIs(napi_number);
    if (!usageNumber) { // for source: TexImageSource
        tie(succ, imgArg.format) = NVal(env, funcArg[NARG_POS::FOURTH]).ToGLenum();
            if (!succ) {
                return NVal::CreateNull(env).val_;
            }
        tie(succ, imgArg.type) = NVal(env, funcArg[NARG_POS::FIFTH]).ToGLenum();
        return !succ ? nullptr :
            context->GetWebGLRenderingContextImpl().TexImage2D(env, imgArg, funcArg[NARG_POS::SIXTH]);
    }
    tie(succ, imgArg.width) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.height) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.border) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.format) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.type) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    usageNumber = NVal(env, funcArg[NARG_POS::NINTH]).TypeIs(napi_number);
    if (context->GetWebGLRenderingContextImpl().IsHighWebGL() && usageNumber) { // support pboOffset
        int64_t pboOffset = 0;
        tie(succ, pboOffset) = NVal(env, funcArg[NARG_POS::NINTH]).ToInt64();
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
        return context->GetWebGLRenderingContextImpl().TexImage2D(env, imgArg, static_cast<GLintptr>(pboOffset));
    } else if (NVal(env, funcArg[NARG_POS::NINTH]).IsBufferArray()) {
        int64_t srcOffset = GetOffset(env, funcArg[NARG_POS::TENTH], context);
        if (srcOffset < 0) {
            return NVal::CreateNull(env).val_;
        }
        return context->GetWebGLRenderingContextImpl().TexImage2D(
            env, imgArg, funcArg[NARG_POS::NINTH], static_cast<GLuint>(srcOffset));
    } else {
        return context->GetWebGLRenderingContextImpl().TexImage2D(env, imgArg, funcArg[NARG_POS::NINTH]);
    }
}

napi_value WebGLRenderingContextOverloads::TexSubImage2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!(funcArg.InitArgs(NARG_CNT::NINE, NARG_CNT::TEN) || funcArg.InitArgs(NARG_CNT::SEVEN))) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    TexSubImage2DArg imgArg = { };
    imgArg.func = Impl::IMAGE_TEX_SUB_IMAGE_2D;
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
    bool usageNumber = NVal(env, funcArg[NARG_POS::SEVENTH]).TypeIs(napi_number);
    if (!usageNumber) {
        tie(succ, imgArg.format) = NVal(env, funcArg[NARG_POS::FIFTH]).ToGLenum();
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
        tie(succ, imgArg.type) = NVal(env, funcArg[NARG_POS::SIXTH]).ToGLenum();
        return !succ ? nullptr :
            context->GetWebGLRenderingContextImpl().TexSubImage2D(env, imgArg, funcArg[NARG_POS::SEVENTH]);
    }

    tie(succ, imgArg.width) = NVal(env, funcArg[NARG_POS::FIFTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.height) = NVal(env, funcArg[NARG_POS::SIXTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.format) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.type) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    usageNumber = NVal(env, funcArg[NARG_POS::NINTH]).TypeIs(napi_number);
    if (context->GetWebGLRenderingContextImpl().IsHighWebGL() && usageNumber) { // support pboOffset
        int64_t pboOffset = 0;
        tie(succ, pboOffset) = NVal(env, funcArg[NARG_POS::NINTH]).ToInt64();
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
        return context->GetWebGLRenderingContextImpl().TexSubImage2D(env, imgArg, static_cast<GLintptr>(pboOffset));
    } else if (NVal(env, funcArg[NARG_POS::NINTH]).IsBufferArray()) { // buffer
        int64_t srcOffset = GetOffset(env, funcArg[NARG_POS::TENTH], context);
        if (srcOffset < 0) {
            return NVal::CreateNull(env).val_;
        }
        return context->GetWebGLRenderingContextImpl().TexSubImage2D(env, imgArg, funcArg[NARG_POS::NINTH], 0);
    } else { // for image source
        return context->GetWebGLRenderingContextImpl().TexSubImage2D(env, imgArg, funcArg[NARG_POS::NINTH]);
    }
}

napi_value WebGLRenderingContextOverloads::Uniform1fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::FOUR)) {
        return NVal::CreateNull(env).val_;
    }

    UniformExtInfo extInfo(1);
    bool succ = extInfo.GetUniformExtInfo(env, funcArg, NARG_POS::THIRD);
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? nullptr : context->GetWebGLRenderingContextImpl().UniformFv(env,
        funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND], &extInfo);
}

napi_value WebGLRenderingContextOverloads::Uniform2fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::FOUR)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    UniformExtInfo extInfo(WebGLArg::UNIFORM_2V_REQUIRE_MIN_SIZE);
    bool succ = extInfo.GetUniformExtInfo(env, funcArg, NARG_POS::THIRD);
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGLRenderingContextImpl().UniformFv(env,
        funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND], &extInfo);
}

napi_value WebGLRenderingContextOverloads::Uniform3fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::FOUR)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    UniformExtInfo extInfo(WebGLArg::UNIFORM_3V_REQUIRE_MIN_SIZE);
    bool succ = extInfo.GetUniformExtInfo(env, funcArg, NARG_POS::THIRD);
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGLRenderingContextImpl().UniformFv(env,
        funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND], &extInfo);
}

napi_value WebGLRenderingContextOverloads::Uniform4fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::FOUR)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    UniformExtInfo extInfo(WebGLArg::UNIFORM_4V_REQUIRE_MIN_SIZE);
    bool succ = extInfo.GetUniformExtInfo(env, funcArg, NARG_POS::THIRD);
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGLRenderingContextImpl().UniformFv(env,
        funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND], &extInfo);
}

napi_value WebGLRenderingContextOverloads::Uniform1iv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::FOUR)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    UniformExtInfo extInfo(1);
    extInfo.dimension = 1;
    bool succ = extInfo.GetUniformExtInfo(env, funcArg, NARG_POS::THIRD);
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGLRenderingContextImpl().UniformIv(env,
        funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND], &extInfo);
}

napi_value WebGLRenderingContextOverloads::Uniform2iv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::FOUR)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    UniformExtInfo extInfo(WebGLArg::UNIFORM_2V_REQUIRE_MIN_SIZE);
    bool succ = extInfo.GetUniformExtInfo(env, funcArg, NARG_POS::THIRD);
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGLRenderingContextImpl().UniformIv(env,
        funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND], &extInfo);
}

napi_value WebGLRenderingContextOverloads::Uniform3iv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::FOUR)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    UniformExtInfo extInfo(WebGLArg::UNIFORM_3V_REQUIRE_MIN_SIZE);
    bool succ = extInfo.GetUniformExtInfo(env, funcArg, NARG_POS::THIRD);
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGLRenderingContextImpl().UniformIv(env,
        funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND], &extInfo);
}

napi_value WebGLRenderingContextOverloads::Uniform4iv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::FOUR)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    UniformExtInfo extInfo(WebGLArg::UNIFORM_4V_REQUIRE_MIN_SIZE);
    bool succ = extInfo.GetUniformExtInfo(env, funcArg, NARG_POS::THIRD);
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGLRenderingContextImpl().UniformIv(env,
        funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND], &extInfo);
}

napi_value WebGLRenderingContextOverloads::UniformMatrix2fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FIVE)) {
        return NVal::CreateNull(env).val_;
    }
    UniformExtInfo extInfo(WebGLArg::MATRIX_2X2_REQUIRE_MIN_SIZE);
    bool succ = false;
    bool transpose = false;
    tie(succ, transpose) = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    succ = extInfo.GetUniformExtInfo(env, funcArg, NARG_POS::FOURTH);
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? nullptr : context->GetWebGLRenderingContextImpl().UniformMatrixFv(env,
        funcArg[NARG_POS::FIRST], funcArg[NARG_POS::THIRD], transpose, &extInfo);
}

napi_value WebGLRenderingContextOverloads::UniformMatrix3fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FIVE)) {
        return NVal::CreateNull(env).val_;
    }
    UniformExtInfo extInfo(WebGLArg::MATRIX_3X3_REQUIRE_MIN_SIZE);
    bool succ = false;
    bool transpose = false;
    tie(succ, transpose) = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    succ = extInfo.GetUniformExtInfo(env, funcArg, NARG_POS::FOURTH);
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? nullptr : context->GetWebGLRenderingContextImpl().UniformMatrixFv(env,
        funcArg[NARG_POS::FIRST], funcArg[NARG_POS::THIRD], transpose, &extInfo);
}

napi_value WebGLRenderingContextOverloads::UniformMatrix4fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FIVE)) {
        return NVal::CreateNull(env).val_;
    }

    UniformExtInfo extInfo(WebGLArg::MATRIX_4X4_REQUIRE_MIN_SIZE);
    bool succ = false;
    bool transpose = false;
    tie(succ, transpose) = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    succ = extInfo.GetUniformExtInfo(env, funcArg, NARG_POS::FOURTH);
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGLRenderingContext* context = GetWebGLRenderingContextBase(env, funcArg.GetThisVar());
    return (context == nullptr) ? nullptr : context->GetWebGLRenderingContextImpl().UniformMatrixFv(env,
        funcArg[NARG_POS::FIRST], funcArg[NARG_POS::THIRD], transpose, &extInfo);
}
} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif