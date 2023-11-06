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
#include "context/webgl2_rendering_context_base.h"

#include "context/webgl2_rendering_context.h"
#include "napi/n_class.h"
#include "napi/n_func_arg.h"
#include "util/log.h"
#include "util/util.h"
#include "webgl/webgl_active_info.h"
#include "webgl/webgl_arg.h"
#include "webgl/webgl_buffer.h"
#include "webgl/webgl_framebuffer.h"
#include "webgl/webgl_program.h"
#include "webgl/webgl_query.h"
#include "webgl/webgl_renderbuffer.h"
#include "webgl/webgl_sampler.h"
#include "webgl/webgl_shader.h"
#include "webgl/webgl_sync.h"
#include "webgl/webgl_texture.h"
#include "webgl/webgl_transform_feedback.h"
#include "webgl/webgl_uniform_location.h"
#include "webgl/webgl_vertex_array_object.h"

#ifdef __cplusplus
extern "C" {
#endif
namespace OHOS {
namespace Rosen {
using namespace std;
static WebGL2RenderingContext* GetWebGL2RenderingContext(napi_env env, napi_value thisVar)
{
    return reinterpret_cast<WebGL2RenderingContext*>(Util::GetContextObject(env, thisVar));
}

napi_value WebGL2RenderingContextBase::DrawBuffers(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }

    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        LOGE("DrawBuffers context is nullptr");
        return nullptr;
    }
    return context->GetWebGL2RenderingContextImpl().DrawBuffers(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGL2RenderingContextBase::ClearBufferfv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FOUR)) {
        return nullptr;
    }

    bool succ = false;
    GLenum buffer;
    tie(succ, buffer) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    GLint drawbuffer;
    tie(succ, drawbuffer) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    int64_t srcOffset = 0;
    if (!NVal(env, funcArg[NARG_POS::FOURTH]).IsNull()) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        LOGE("ClearBufferfv context is nullptr");
        return nullptr;
    }
    return context->GetWebGL2RenderingContextImpl().ClearBufferV(
        env, buffer, drawbuffer, funcArg[NARG_POS::THIRD], srcOffset, BUFFER_DATA_FLOAT_32);
}

napi_value WebGL2RenderingContextBase::ClearBufferiv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FOUR)) {
        return nullptr;
    }

    bool succ = false;
    GLenum buffer;
    tie(succ, buffer) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    GLint drawbuffer;
    tie(succ, drawbuffer) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        LOGE("ClearBufferiv context is nullptr");
        return nullptr;
    }
    return context->GetWebGL2RenderingContextImpl().ClearBufferV(
        env, buffer, drawbuffer, funcArg[NARG_POS::THIRD], srcOffset, BUFFER_DATA_INT_32);
}

napi_value WebGL2RenderingContextBase::ClearBufferuiv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FOUR)) {
        return nullptr;
    }

    bool succ = false;
    GLenum buffer;
    tie(succ, buffer) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    GLint drawbuffer;
    tie(succ, drawbuffer) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        LOGE("ClearBufferuiv context is nullptr");
        return nullptr;
    }
    return context->GetWebGL2RenderingContextImpl().ClearBufferV(
        env, buffer, drawbuffer, funcArg[NARG_POS::THIRD], srcOffset, BUFFER_DATA_UINT_32);
}

napi_value WebGL2RenderingContextBase::ClearBufferfi(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }

    bool succ = false;
    GLenum buffer;
    tie(succ, buffer) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    GLint drawbuffer;
    tie(succ, drawbuffer) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    GLfloat depth;
    tie(succ, depth) = NVal(env, funcArg[NARG_POS::THIRD]).ToFloat();
    if (!succ) {
        return nullptr;
    }
    GLint stencil;
    tie(succ, stencil) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        LOGE("ClearBufferfi context is nullptr");
        return nullptr;
    }
    return context->GetWebGL2RenderingContextImpl().ClearBufferfi(env, buffer, drawbuffer, depth, stencil);
}

napi_value WebGL2RenderingContextBase::CreateQuery(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return nullptr;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        LOGE("CreateQuery context is nullptr");
        return nullptr;
    }
    return context->GetWebGL2RenderingContextImpl().CreateQuery(env);
}

napi_value WebGL2RenderingContextBase::DeleteQuery(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }

    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        LOGE("DeleteQuery context is nullptr");
        return nullptr;
    }
    return context->GetWebGL2RenderingContextImpl().DeleteQuery(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGL2RenderingContextBase::IsQuery(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, false).val_;
    }

    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        LOGE("IsQuery context is nullptr");
        return nullptr;
    }
    return context->GetWebGL2RenderingContextImpl().IsQuery(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGL2RenderingContextBase::BeginQuery(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    GLenum target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();

    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        LOGE("BeginQuery context is nullptr");
        return nullptr;
    }
    return context->GetWebGL2RenderingContextImpl().BeginQuery(env, target, funcArg[NARG_POS::SECOND]);
}

napi_value WebGL2RenderingContextBase::EndQuery(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }

    bool succ = false;
    GLenum target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        LOGE("EndQuery context is nullptr");
        return nullptr;
    }
    return context->GetWebGL2RenderingContextImpl().EndQuery(env, target);
}

napi_value WebGL2RenderingContextBase::GetQuery(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLenum target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    GLenum pName;
    tie(succ, pName) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        LOGE("GetQuery context is nullptr");
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGL2RenderingContextImpl().GetQuery(env, target, pName);
}

napi_value WebGL2RenderingContextBase::GetQueryParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLenum pName;
    tie(succ, pName) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        LOGE("GetQueryParameter context is nullptr");
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGL2RenderingContextImpl().GetQueryParameter(env, funcArg[NARG_POS::FIRST], pName);
}

napi_value WebGL2RenderingContextBase::CreateSampler(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        LOGE("CreateSampler context is nullptr");
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGL2RenderingContextImpl().CreateSampler(env);
}

napi_value WebGL2RenderingContextBase::DeleteSampler(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }

    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        LOGE("DeleteSampler context is nullptr");
        return nullptr;
    }
    return context->GetWebGL2RenderingContextImpl().DeleteSampler(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGL2RenderingContextBase::IsSampler(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, false).val_;
    }

    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        LOGE("IsSampler context is nullptr");
        return NVal::CreateBool(env, false).val_;
    }
    return context->GetWebGL2RenderingContextImpl().IsSampler(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGL2RenderingContextBase::BindSampler(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }

    bool succ = false;
    GLuint unit;
    tie(succ, unit) = NVal(env, funcArg[NARG_POS::FIRST]).ToUint32();
    if (!succ) {
        return nullptr;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        LOGE("BindSampler context is nullptr");
        return nullptr;
    }
    return context->GetWebGL2RenderingContextImpl().BindSampler(env, unit, funcArg[NARG_POS::SECOND]);
}

napi_value WebGL2RenderingContextBase::SamplerParameteri(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }

    bool succ = false;
    GLenum pName;
    tie(succ, pName) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    GLint param;
    tie(succ, param) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        LOGE("SamplerParameteri context is nullptr");
        return nullptr;
    }
    return context->GetWebGL2RenderingContextImpl().SamplerParameter(
        env, funcArg[NARG_POS::FIRST], pName, false, static_cast<void*>(&param));
}

napi_value WebGL2RenderingContextBase::SamplerParameterf(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }

    bool succ = false;
    GLenum pName;
    tie(succ, pName) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    float param;
    tie(succ, param) = NVal(env, funcArg[NARG_POS::THIRD]).ToFloat();
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        LOGE("SamplerParameterf context is nullptr");
        return nullptr;
    }
    return context->GetWebGL2RenderingContextImpl().SamplerParameter(
        env, funcArg[NARG_POS::FIRST], pName, true, static_cast<void*>(&param));
}

napi_value WebGL2RenderingContextBase::GetSamplerParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }

    bool succ = false;
    GLenum pName;
    tie(succ, pName) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        LOGE("GetSamplerParameter context is nullptr");
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGL2RenderingContextImpl().GetSamplerParameter(env, funcArg[NARG_POS::FIRST], pName);
}

napi_value WebGL2RenderingContextBase::FenceSync(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLenum condition;
    tie(succ, condition) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    int64_t flags;
    tie(succ, flags) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        LOGE("FenceSync context is nullptr");
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGL2RenderingContextImpl().FenceSync(env, condition, static_cast<GLbitfield>(flags));
}

napi_value WebGL2RenderingContextBase::IsSync(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, false).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        LOGE("IsSync context is nullptr");
        return NVal::CreateBool(env, false).val_;
    }
    return context->GetWebGL2RenderingContextImpl().IsSync(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGL2RenderingContextBase::DeleteSync(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }

    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        LOGE("DeleteSync context is nullptr");
        return nullptr;
    }
    return context->GetWebGL2RenderingContextImpl().DeleteSync(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGL2RenderingContextBase::ClientWaitSync(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return NVal::CreateInt64(env, WebGL2RenderingContextBase::WAIT_FAILED).val_;
    }

    bool succ = false;
    int64_t flags;
    tie(succ, flags) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return NVal::CreateInt64(env, WebGL2RenderingContextBase::WAIT_FAILED).val_;
    }
    int64_t timeout;
    tie(succ, timeout) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return NVal::CreateInt64(env, WebGL2RenderingContextBase::WAIT_FAILED).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        LOGE("ClientWaitSync context is nullptr");
        return NVal::CreateInt64(env, WebGL2RenderingContextBase::WAIT_FAILED).val_;
    }
    return context->GetWebGL2RenderingContextImpl().ClientWaitSync(
        env, funcArg[NARG_POS::FIRST], static_cast<GLbitfield>(flags), timeout);
}

} // namespace Rosen
} // namespace OHOS
#ifdef __cplusplus
}
#endif