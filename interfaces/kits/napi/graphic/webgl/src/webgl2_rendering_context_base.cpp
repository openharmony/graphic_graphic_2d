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
        return NVal::CreateNull(env).val_;
    }

    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().DrawBuffers(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGL2RenderingContextBase::ClearBufferfv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FOUR)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLenum buffer = 0;
    tie(succ, buffer) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLint drawbuffer = 0;
    tie(succ, drawbuffer) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    int64_t srcOffset = 0;
    if (!NVal(env, funcArg[NARG_POS::FOURTH]).IsNull()) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().ClearBufferV(
        env, buffer, drawbuffer, funcArg[NARG_POS::THIRD], srcOffset, BUFFER_DATA_FLOAT_32);
}

napi_value WebGL2RenderingContextBase::ClearBufferiv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FOUR)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLenum buffer = 0;
    tie(succ, buffer) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLint drawbuffer = 0;
    tie(succ, drawbuffer) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().ClearBufferV(
        env, buffer, drawbuffer, funcArg[NARG_POS::THIRD], srcOffset, BUFFER_DATA_INT_32);
}

napi_value WebGL2RenderingContextBase::ClearBufferuiv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FOUR)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLenum buffer = 0;
    tie(succ, buffer) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLint drawbuffer = 0;
    tie(succ, drawbuffer) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().ClearBufferV(
        env, buffer, drawbuffer, funcArg[NARG_POS::THIRD], srcOffset, BUFFER_DATA_UINT_32);
}

napi_value WebGL2RenderingContextBase::ClearBufferfi(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLenum buffer = 0;
    tie(succ, buffer) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLint drawbuffer = 0;
    tie(succ, drawbuffer) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLfloat depth = 0.0;
    tie(succ, depth) = NVal(env, funcArg[NARG_POS::THIRD]).ToFloat();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLint stencil = 0;
    tie(succ, stencil) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().ClearBufferfi(env, buffer, drawbuffer, depth, stencil);
}

napi_value WebGL2RenderingContextBase::CreateQuery(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().CreateQuery(env);
}

napi_value WebGL2RenderingContextBase::DeleteQuery(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }

    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().DeleteQuery(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGL2RenderingContextBase::IsQuery(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, false).val_;
    }

    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().IsQuery(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGL2RenderingContextBase::BeginQuery(napi_env env, napi_callback_info info)
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
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().BeginQuery(env, target, funcArg[NARG_POS::SECOND]);
}

napi_value WebGL2RenderingContextBase::EndQuery(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLenum target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().EndQuery(env, target);
}

napi_value WebGL2RenderingContextBase::GetQuery(napi_env env, napi_callback_info info)
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
    GLenum pName = 0;
    tie(succ, pName) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().GetQuery(env, target, pName);
}

napi_value WebGL2RenderingContextBase::GetQueryParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLenum pName = 0;
    tie(succ, pName) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().GetQueryParameter(env, funcArg[NARG_POS::FIRST], pName);
}

napi_value WebGL2RenderingContextBase::CreateSampler(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().CreateSampler(env);
}

napi_value WebGL2RenderingContextBase::DeleteSampler(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }

    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().DeleteSampler(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGL2RenderingContextBase::IsSampler(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, false).val_;
    }

    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateBool(env, false).val_ :
        context->GetWebGL2RenderingContextImpl().IsSampler(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGL2RenderingContextBase::BindSampler(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLuint unit = 0;
    tie(succ, unit) = NVal(env, funcArg[NARG_POS::FIRST]).ToUint32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().BindSampler(env, unit, funcArg[NARG_POS::SECOND]);
}

napi_value WebGL2RenderingContextBase::SamplerParameteri(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLenum pName = 0;
    tie(succ, pName) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLint param = 0;
    tie(succ, param) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().SamplerParameter(
        env, funcArg[NARG_POS::FIRST], pName, false, static_cast<void*>(&param));
}

napi_value WebGL2RenderingContextBase::SamplerParameterf(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLenum pName = 0;
    tie(succ, pName) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    float param = 0;
    tie(succ, param) = NVal(env, funcArg[NARG_POS::THIRD]).ToFloat();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().SamplerParameter(
        env, funcArg[NARG_POS::FIRST], pName, true, static_cast<void*>(&param));
}

napi_value WebGL2RenderingContextBase::GetSamplerParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLenum pName = 0;
    tie(succ, pName) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().GetSamplerParameter(env, funcArg[NARG_POS::FIRST], pName);
}

napi_value WebGL2RenderingContextBase::FenceSync(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLenum condition = 0;
    tie(succ, condition) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    int64_t flags = 0;
    tie(succ, flags) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().FenceSync(env, condition, static_cast<GLbitfield>(flags));
}

napi_value WebGL2RenderingContextBase::IsSync(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, false).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateBool(env, false).val_ :
        context->GetWebGL2RenderingContextImpl().IsSync(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGL2RenderingContextBase::DeleteSync(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }

    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().DeleteSync(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGL2RenderingContextBase::ClientWaitSync(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return NVal::CreateInt64(env, WebGL2RenderingContextBase::WAIT_FAILED).val_;
    }

    bool succ = false;
    int64_t flags = 0;
    tie(succ, flags) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return NVal::CreateInt64(env, WebGL2RenderingContextBase::WAIT_FAILED).val_;
    }
    int64_t timeout = 0;
    tie(succ, timeout) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return NVal::CreateInt64(env, WebGL2RenderingContextBase::WAIT_FAILED).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateInt64(env, WebGL2RenderingContextBase::WAIT_FAILED).val_ :
        context->GetWebGL2RenderingContextImpl().ClientWaitSync(
        env, funcArg[NARG_POS::FIRST], static_cast<GLbitfield>(flags), timeout);
}

napi_value WebGL2RenderingContextBase::WaitSync(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    int64_t flags = 0;
    tie(succ, flags) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    int64_t timeout = 0;
    tie(succ, timeout) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateInt64(env, WebGL2RenderingContextBase::WAIT_FAILED).val_ :
        context->GetWebGL2RenderingContextImpl().WaitSync(
        env, funcArg[NARG_POS::FIRST], static_cast<GLbitfield>(flags), timeout);
}

napi_value WebGL2RenderingContextBase::GetSyncParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLenum pname = 0;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return NVal::CreateInt64(env, WebGL2RenderingContextBase::WAIT_FAILED).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateInt64(env, WebGL2RenderingContextBase::WAIT_FAILED).val_ :
        context->GetWebGL2RenderingContextImpl().GetSyncParameter(env, funcArg[NARG_POS::FIRST], pname);
}

napi_value WebGL2RenderingContextBase::CreateTransformFeedback(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().CreateTransformFeedback(env);
}

napi_value WebGL2RenderingContextBase::DeleteTransformFeedback(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().DeleteTransformFeedback(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGL2RenderingContextBase::IsTransformFeedback(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, false).val_;
    }

    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateBool(env, false).val_ :
        context->GetWebGL2RenderingContextImpl().IsTransformFeedback(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGL2RenderingContextBase::BindTransformFeedback(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLenum target = 0;
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        context->GetWebGL2RenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGL2RenderingContextImpl().BindTransformFeedback(env, funcArg[NARG_POS::SECOND], target);
}

napi_value WebGL2RenderingContextBase::BeginTransformFeedback(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLenum primitiveMode = 0;
    tie(succ, primitiveMode) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().BeginTransformFeedback(env, primitiveMode);
}

napi_value WebGL2RenderingContextBase::EndTransformFeedback(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return NVal::CreateNull(env).val_;
    }

    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().EndTransformFeedback(env);
}

napi_value WebGL2RenderingContextBase::TexStorage3D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::SIX)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    TexStorageArg args = {};
    tie(succ, args.target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, args.levels) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, args.internalFormat) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, args.width) = NVal(env, funcArg[NARG_POS::FOURTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, args.height) = NVal(env, funcArg[NARG_POS::FIFTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, args.depth) = NVal(env, funcArg[NARG_POS::SIXTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().TexStorage3D(env, args);
}

napi_value WebGL2RenderingContextBase::TexImage3D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TEN, NARG_CNT::ELEVEN)) {
        return NVal::CreateNull(env).val_;
    }

    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    TexImageArg imgArg = {};
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
    tie(succ, imgArg.width) = NVal(env, funcArg[NARG_POS::FOURTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.height) = NVal(env, funcArg[NARG_POS::FIFTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.depth) = NVal(env, funcArg[NARG_POS::SIXTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.border) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.format) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.type) = NVal(env, funcArg[NARG_POS::NINTH]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    if (NVal(env, funcArg[NARG_POS::TENTH]).TypeIs(napi_number)) {
        int64_t pboOffset = 0;
        tie(succ, pboOffset) = NVal(env, funcArg[NARG_POS::TENTH]).ToInt64();
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
        return context->GetWebGL2RenderingContextImpl().TexImage3D(env, imgArg, static_cast<GLintptr>(pboOffset));
    }
    if (NVal(env, funcArg[NARG_POS::TENTH]).IsBufferArray()) {
        GLuint srcOffset = 0;
        if (!NVal(env, funcArg[NARG_POS::ELEVENTH]).IsNull()) {
            tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::ELEVENTH]).ToUint32();
            if (!succ) {
                return NVal::CreateNull(env).val_;
            }
        }
        return context->GetWebGL2RenderingContextImpl().TexImage3D(env, imgArg, funcArg[NARG_POS::TENTH], srcOffset);
    } else {
        return context->GetWebGL2RenderingContextImpl().TexImage3D(env, imgArg, funcArg[NARG_POS::TENTH]);
    }
}

napi_value WebGL2RenderingContextBase::TexSubImage3D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ELEVEN, NARG_CNT::TWELVE)) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    TexSubImage3DArg imgArg = {};
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
    tie(succ, imgArg.zOffset) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt32();
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
    tie(succ, imgArg.depth) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.format) = NVal(env, funcArg[NARG_POS::NINTH]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.type) = NVal(env, funcArg[NARG_POS::TENTH]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    if (NVal(env, funcArg[NARG_POS::ELEVENTH]).TypeIs(napi_number)) {
        int64_t pboOffset = 0;
        tie(succ, pboOffset) = NVal(env, funcArg[NARG_POS::ELEVENTH]).ToInt64();
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
        return context->GetWebGL2RenderingContextImpl().TexSubImage3D(env, imgArg, static_cast<GLintptr>(pboOffset));
    }
    if (NVal(env, funcArg[NARG_POS::ELEVENTH]).IsBufferArray()) {
        GLuint srcOffset = 0;
        if (!NVal(env, funcArg[NARG_POS::TWELVETH]).IsNull()) {
            tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::TWELVETH]).ToUint32();
            if (!succ) {
                return NVal::CreateNull(env).val_;
            }
        }
        return context->GetWebGL2RenderingContextImpl().TexSubImage3D(
            env, imgArg, funcArg[NARG_POS::ELEVENTH], srcOffset);
    } else {
        return context->GetWebGL2RenderingContextImpl().TexSubImage3D(env, imgArg, funcArg[NARG_POS::ELEVENTH]);
    }
}

napi_value WebGL2RenderingContextBase::CopyTexSubImage3D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::NINE)) {
        return NVal::CreateNull(env).val_;
    }

    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    CopyTexSubImage3DArg imgArg = {};
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
    tie(succ, imgArg.zOffset) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.x) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.y) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.width) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.height) = NVal(env, funcArg[NARG_POS::NINTH]).ToGLsizei();
    return !succ ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().CopyTexSubImage3D(env, imgArg);
}

napi_value WebGL2RenderingContextBase::CompressedTexImage3D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::EIGHT, NARG_CNT::TEN)) {
        return NVal::CreateNull(env).val_;
    }

    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    TexImageArg imgArg = {};
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
    tie(succ, imgArg.width) = NVal(env, funcArg[NARG_POS::FOURTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.height) = NVal(env, funcArg[NARG_POS::FIFTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.depth) = NVal(env, funcArg[NARG_POS::SIXTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.border) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    if (NVal(env, funcArg[NARG_POS::EIGHTH]).IsBufferArray()) {
        uint32_t srcOffset = 0;
        uint32_t srcLengthOverride = 0;
        if (!NVal(env, funcArg[NARG_POS::NINTH]).IsNull()) {
            tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::NINTH]).ToUint32();
            if (!succ) {
                return NVal::CreateNull(env).val_;
            }
        }
        if (!NVal(env, funcArg[NARG_POS::TENTH]).IsNull()) {
            tie(succ, srcLengthOverride) = NVal(env, funcArg[NARG_POS::TENTH]).ToUint32();
            if (!succ) {
                return NVal::CreateNull(env).val_;
            }
        }
        return context->GetWebGL2RenderingContextImpl().CompressedTexImage3D(
            env, imgArg, funcArg[NARG_POS::EIGHTH], srcOffset, srcLengthOverride);
    } else {
        GLsizei imageSize = 0;
        tie(succ, imageSize) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToGLsizei();
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
        int64_t srcOffset;
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::NINTH]).ToInt64();
        return !succ ? NVal::CreateNull(env).val_ : context->GetWebGL2RenderingContextImpl().CompressedTexImage3D(
            env, imgArg, imageSize, static_cast<GLintptr>(srcOffset));
    }
}

napi_value WebGL2RenderingContextBase::CompressedTexSubImage3D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TEN, NARG_CNT::TWELVE)) {
        return NVal::CreateNull(env).val_;
    }

    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    TexSubImage3DArg imgArg = {};
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
    tie(succ, imgArg.zOffset) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt32();
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
    tie(succ, imgArg.depth) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, imgArg.format) = NVal(env, funcArg[NARG_POS::NINTH]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    if (NVal(env, funcArg[NARG_POS::TENTH]).IsBufferArray()) {
        uint32_t srcOffset = 0;
        uint32_t srcLengthOverride = 0;
        if (!NVal(env, funcArg[NARG_POS::ELEVENTH]).IsNull()) {
            tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::ELEVENTH]).ToUint32();
            if (!succ) {
                return NVal::CreateNull(env).val_;
            }
        }
        if (!NVal(env, funcArg[NARG_POS::TWELVETH]).IsNull()) {
            tie(succ, srcLengthOverride) = NVal(env, funcArg[NARG_POS::TWELVETH]).ToUint32();
            if (!succ) {
                return NVal::CreateNull(env).val_;
            }
        }
        return context->GetWebGL2RenderingContextImpl().CompressedTexSubImage3D(
            env, imgArg, funcArg[NARG_POS::TENTH], srcOffset, srcLengthOverride);
    } else {
        GLsizei imageSize = 0;
        tie(succ, imageSize) = NVal(env, funcArg[NARG_POS::TENTH]).ToGLsizei();
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
        int64_t srcOffset;
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::ELEVENTH]).ToInt64();
        return !succ ? NVal::CreateNull(env).val_ : context->GetWebGL2RenderingContextImpl().CompressedTexSubImage3D(
            env, imgArg, imageSize, static_cast<GLintptr>(srcOffset));
    }
}

napi_value WebGL2RenderingContextBase::GetFragDataLocation(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    unique_ptr<char[]> name = nullptr;
    tie(succ, name, ignore) = NVal(env, funcArg[NARG_POS::SECOND]).ToUTF8String();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().GetFragDataLocation(env, funcArg[NARG_POS::FIRST], name.get());
}

napi_value WebGL2RenderingContextBase::Uniform1ui(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    const uint32_t count = 1;
    GLuint data[4] = {}; // max len 4
    for (size_t i = 0; i < count; i++) {
        tie(succ, data[i]) = NVal(env, funcArg[i + NARG_POS::SECOND]).ToUint32();
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().UniformUi(env, funcArg[NARG_POS::FIRST], count, data);
}

napi_value WebGL2RenderingContextBase::Uniform2ui(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    const uint32_t count = 2; // 2 is 2x2
    GLuint data[4] = {}; // max len 4
    for (size_t i = 0; i < count; i++) {
        tie(succ, data[i]) = NVal(env, funcArg[i + NARG_POS::SECOND]).ToUint32();
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().UniformUi(env, funcArg[NARG_POS::FIRST], count, data);
}

napi_value WebGL2RenderingContextBase::Uniform3ui(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    const uint32_t count = 3; // 3 is 3x3
    GLuint data[4] = {}; // max len 4
    for (size_t i = 0; i < count; i++) {
        tie(succ, data[i]) = NVal(env, funcArg[i + NARG_POS::SECOND]).ToUint32();
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().UniformUi(env, funcArg[NARG_POS::FIRST], count, data);
}

napi_value WebGL2RenderingContextBase::Uniform4ui(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    const uint32_t count = 4; // 4 is 4x4
    GLuint data[4] = {}; // max len 4
    for (size_t i = 0; i < count; i++) {
        tie(succ, data[i]) = NVal(env, funcArg[i + NARG_POS::SECOND]).ToUint32();
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().UniformUi(env, funcArg[NARG_POS::FIRST], count, data);
}

napi_value WebGL2RenderingContextBase::VertexAttribI4i(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    uint32_t index = 0;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToUint32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }

    const int32_t count = 4; // 4 max
    GLint data[count] = { 0 };
    for (int32_t i = 0; i < count; i++) {
        tie(succ, data[i]) = NVal(env, funcArg[i + NARG_POS::SECOND]).ToInt32();
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().VertexAttribI4i(env, index, data);
}

napi_value WebGL2RenderingContextBase::VertexAttribI4ui(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    uint32_t index = 0;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToUint32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }

    const int32_t count = 4; // 4 max
    GLuint data[count] = { 0 };
    for (int32_t i = 0; i < count; i++) {
        tie(succ, data[i]) = NVal(env, funcArg[i + NARG_POS::SECOND]).ToUint32();
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().VertexAttribI4ui(env, index, data);
}

napi_value WebGL2RenderingContextBase::VertexAttribIPointer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return NVal::CreateNull(env).val_;
    }

    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
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
    tie(succ, vertexInfo.stride) = NVal(env, funcArg[NARG_POS::FOURTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLenum result;
    tie(result, vertexInfo.offset) = WebGLArg::ToGLintptr(env, funcArg[NARG_POS::FOURTH]);
    if (result) {
        context->GetWebGL2RenderingContextImpl().SetError(result);
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGL2RenderingContextImpl().VertexAttribIPointer(env, vertexInfo);
}

napi_value WebGL2RenderingContextBase::VertexAttribDivisor(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    int32_t index = 0;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLuint divisor;
    tie(succ, divisor) = NVal(env, funcArg[NARG_POS::SECOND]).ToUint32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().VertexAttribDivisor(env, index, divisor);
}

napi_value WebGL2RenderingContextBase::DrawArraysInstanced(napi_env env, napi_callback_info info)
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
    GLint first = 0;
    tie(succ, first) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLsizei count = 0;
    tie(succ, count) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLsizei instanceCount = 0;
    tie(succ, instanceCount) = NVal(env, funcArg[NARG_POS::FOURTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().DrawArraysInstanced(env, mode, first, count, instanceCount);
}

napi_value WebGL2RenderingContextBase::DrawElementsInstanced(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    DrawElementArg arg {};
    tie(succ, arg.mode) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, arg.count) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, arg.type) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, arg.offset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLsizei instanceCount;
    tie(succ, instanceCount) = NVal(env, funcArg[NARG_POS::FIFTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().DrawElementsInstanced(env, arg, instanceCount);
}

napi_value WebGL2RenderingContextBase::DrawRangeElements(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::SIX)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    DrawElementArg arg {};
    tie(succ, arg.mode) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLuint start;
    tie(succ, start) = NVal(env, funcArg[NARG_POS::SECOND]).ToUint32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLuint end;
    tie(succ, end) = NVal(env, funcArg[NARG_POS::THIRD]).ToUint32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, arg.count) = NVal(env, funcArg[NARG_POS::FOURTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, arg.type) = NVal(env, funcArg[NARG_POS::FIFTH]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, arg.offset) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt64();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().DrawRangeElements(env, arg, start, end);
}

napi_value WebGL2RenderingContextBase::CopyBufferSubData(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLenum targets[2] = {}; // 2 write and read
    tie(succ, targets[0]) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum(); // read
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, targets[1]) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum(); // write
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    int64_t writeOffset = 0;
    int64_t readOffset = 0;
    tie(succ, readOffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64(); // read
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, writeOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64(); // write
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    int64_t size = 0;
    tie(succ, size) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().CopyBufferSubData(env, targets, readOffset, writeOffset, size);
}

napi_value WebGL2RenderingContextBase::GetBufferSubData(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FIVE)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    BufferExt ext {};
    GLenum target = 0;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    int64_t srcByteOffset = 0;
    tie(succ, srcByteOffset) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    succ = ext.GetBufferExt(env, funcArg[NARG_POS::FOURTH], funcArg[NARG_POS::FIFTH]);
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().GetBufferSubData(
        env, target, srcByteOffset, funcArg[NARG_POS::THIRD], ext);
}

napi_value WebGL2RenderingContextBase::BlitFramebuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TEN)) {
        return NVal::CreateNull(env).val_;
    }

    GLint data[8] = { 0 }; // 8 max data
    bool succ = false;
    for (int32_t i = 0; i < 8; i++) { // 8 max data
        tie(succ, data[i]) = NVal(env, funcArg[i]).ToInt32();
        if (!succ) {
            return NVal::CreateNull(env).val_;
        }
    }
    int64_t mask = 0;
    tie(succ, mask) = NVal(env, funcArg[NARG_POS::NINTH]).ToInt64();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLenum filter = 0;
    tie(succ, filter) = NVal(env, funcArg[NARG_POS::TENTH]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().BlitFrameBuffer(env, data, static_cast<GLbitfield>(mask), filter);
}

napi_value WebGL2RenderingContextBase::FramebufferTextureLayer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    TextureLayerArg layer = { 0 };
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
    tie(succ, layer.level) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, layer.layer) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().FrameBufferTextureLayer(
        env, target, attachment, funcArg[NARG_POS::THIRD], layer);
}

napi_value WebGL2RenderingContextBase::ReadBuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLenum src = 0;
    tie(succ, src) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().ReadBuffer(env, src);
}

napi_value WebGL2RenderingContextBase::RenderbufferStorageMultisample(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    TexStorageArg arg = {};
    tie(succ, arg.target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLsizei samples = 0;
    tie(succ, samples) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }

    tie(succ, arg.internalFormat) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, arg.width) = NVal(env, funcArg[NARG_POS::FOURTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, arg.height) = NVal(env, funcArg[NARG_POS::FIFTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().RenderBufferStorageMultiSample(env, arg, samples);
}

napi_value WebGL2RenderingContextBase::RenderbufferStorage(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
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
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().RenderBufferStorageMultiSample(env, arg, 0);
}

napi_value WebGL2RenderingContextBase::TexStorage2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    TexStorageArg arg = {};
    arg.depth = 1;
    tie(succ, arg.target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, arg.levels) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, arg.internalFormat) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, arg.width) = NVal(env, funcArg[NARG_POS::FOURTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, arg.height) = NVal(env, funcArg[NARG_POS::FIFTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().TexStorage2D(env, arg);
}

napi_value WebGL2RenderingContextBase::GetTransformFeedbackVarying(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLuint index = 0;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::SECOND]).ToUint32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }

    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().GetTransformFeedbackVarying(env, funcArg[NARG_POS::FIRST], index);
}

napi_value WebGL2RenderingContextBase::PauseTransformFeedback(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    glPauseTransformFeedback();
    return NVal::CreateNull(env).val_;
}

napi_value WebGL2RenderingContextBase::ResumeTransformFeedback(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    glResumeTransformFeedback();
    return NVal::CreateNull(env).val_;
}

napi_value WebGL2RenderingContextBase::BindBufferBase(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    BufferBaseArg arg { 0 };
    tie(succ, arg.target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, arg.index) = NVal(env, funcArg[NARG_POS::SECOND]).ToUint32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().BindBufferBase(env, arg, funcArg[NARG_POS::THIRD]);
}

napi_value WebGL2RenderingContextBase::BindBufferRange(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    BufferBaseArg arg { 0 };
    tie(succ, arg.target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, arg.index) = NVal(env, funcArg[NARG_POS::SECOND]).ToUint32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    int64_t offset = 0;
    tie(succ, offset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    int64_t size = 0;
    tie(succ, size) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }

    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    if (!WebGLArg::CheckNoneNegInt<GLint>(offset) || !WebGLArg::CheckNoneNegInt<GLsizei>(size)) {
        context->GetWebGL2RenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGL2RenderingContextImpl().BindBufferRange(
        env, arg, funcArg[NARG_POS::THIRD], static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size));
}

napi_value WebGL2RenderingContextBase::GetIndexedParameter(napi_env env, napi_callback_info info)
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
    int64_t index = 0;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().GetIndexedParameter(env, target, static_cast<GLuint>(index));
}

napi_value WebGL2RenderingContextBase::GetUniformBlockIndex(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    unique_ptr<char[]> name = nullptr;
    tie(succ, name, ignore) = NVal(env, funcArg[NARG_POS::SECOND]).ToUTF8String();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().GetUniformBlockIndex(env, funcArg[NARG_POS::FIRST], name.get());
}

napi_value WebGL2RenderingContextBase::UniformBlockBinding(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLuint uniformBlockIndex = 0;
    tie(succ, uniformBlockIndex) = NVal(env, funcArg[NARG_POS::SECOND]).ToUint32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLuint uniformBlockBinding;
    tie(succ, uniformBlockBinding) = NVal(env, funcArg[NARG_POS::THIRD]).ToUint32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().UniformBlockBinding(
        env, funcArg[NARG_POS::FIRST], uniformBlockIndex, uniformBlockBinding);
}

napi_value WebGL2RenderingContextBase::CreateVertexArray(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return NVal::CreateNull(env).val_;
    }

    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().CreateVertexArray(env);
}

napi_value WebGL2RenderingContextBase::DeleteVertexArray(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }

    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().DeleteVertexArray(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGL2RenderingContextBase::IsVertexArray(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, false).val_;
    }

    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().IsVertexArray(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGL2RenderingContextBase::BindVertexArray(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }

    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().BindVertexArray(env, funcArg[NARG_POS::FIRST]);
}

napi_value WebGL2RenderingContextBase::Uniform1uiv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FOUR)) {
        return NVal::CreateNull(env).val_;
    }

    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    UniformExtInfo extInfo(WebGLArg::UNIFORM_1V_REQUIRE_MIN_SIZE);
    bool succ = extInfo.GetUniformExtInfo(env, funcArg, NARG_POS::THIRD);
    return !succ ? NVal::CreateNull(env).val_ : context->GetWebGL2RenderingContextImpl().UniformUiv(
        env, funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND], &extInfo);
}

napi_value WebGL2RenderingContextBase::Uniform2uiv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FOUR)) {
        return NVal::CreateNull(env).val_;
    }

    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    UniformExtInfo extInfo(WebGLArg::UNIFORM_2V_REQUIRE_MIN_SIZE);
    bool succ = extInfo.GetUniformExtInfo(env, funcArg, NARG_POS::THIRD);
    return !succ ? NVal::CreateNull(env).val_ : context->GetWebGL2RenderingContextImpl().UniformUiv(
        env, funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND], &extInfo);
}

napi_value WebGL2RenderingContextBase::Uniform3uiv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FOUR)) {
        return NVal::CreateNull(env).val_;
    }

    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    UniformExtInfo extInfo(WebGLArg::UNIFORM_3V_REQUIRE_MIN_SIZE);
    bool succ = extInfo.GetUniformExtInfo(env, funcArg, NARG_POS::THIRD);
    return !succ ? NVal::CreateNull(env).val_ : context->GetWebGL2RenderingContextImpl().UniformUiv(
        env, funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND], &extInfo);
}

napi_value WebGL2RenderingContextBase::Uniform4uiv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FOUR)) {
        return NVal::CreateNull(env).val_;
    }

    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    UniformExtInfo extInfo(WebGLArg::UNIFORM_4V_REQUIRE_MIN_SIZE);
    bool succ = extInfo.GetUniformExtInfo(env, funcArg, NARG_POS::THIRD);
    return !succ ? NVal::CreateNull(env).val_ : context->GetWebGL2RenderingContextImpl().UniformUiv(
        env, funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND], &extInfo);
}

napi_value WebGL2RenderingContextBase::UniformMatrix3x2fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FIVE)) {
        return NVal::CreateNull(env).val_;
    }
    UniformExtInfo extInfo(WebGLArg::MATRIX_3X2_REQUIRE_MIN_SIZE, 6); // 6 size
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
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().UniformMatrixFv(
        env, funcArg[NARG_POS::FIRST], funcArg[NARG_POS::THIRD], transpose, &extInfo);
}

napi_value WebGL2RenderingContextBase::UniformMatrix4x2fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FIVE)) {
        return NVal::CreateNull(env).val_;
    }

    UniformExtInfo extInfo(WebGLArg::MATRIX_4X2_REQUIRE_MIN_SIZE, 8); // 8 size
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
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().UniformMatrixFv(
        env, funcArg[NARG_POS::FIRST], funcArg[NARG_POS::THIRD], transpose, &extInfo);
}

napi_value WebGL2RenderingContextBase::UniformMatrix2x3fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FIVE)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    bool transpose = false;
    tie(succ, transpose) = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    UniformExtInfo extInfo(WebGLArg::MATRIX_2X3_REQUIRE_MIN_SIZE, 6); // 6 size
    succ = extInfo.GetUniformExtInfo(env, funcArg, NARG_POS::FOURTH);
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().UniformMatrixFv(
        env, funcArg[NARG_POS::FIRST], funcArg[NARG_POS::THIRD], transpose, &extInfo);
}

napi_value WebGL2RenderingContextBase::UniformMatrix4x3fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    bool transpose = false;
    tie(succ, transpose) = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    UniformExtInfo extInfo(WebGLArg::MATRIX_4X3_REQUIRE_MIN_SIZE, 12); // 12 size
    succ = extInfo.GetUniformExtInfo(env, funcArg, NARG_POS::FOURTH);
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().UniformMatrixFv(
        env, funcArg[NARG_POS::FIRST], funcArg[NARG_POS::THIRD], transpose, &extInfo);
}

napi_value WebGL2RenderingContextBase::UniformMatrix2x4fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    bool transpose = false;
    tie(succ, transpose) = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    UniformExtInfo extInfo(WebGLArg::MATRIX_2X4_REQUIRE_MIN_SIZE, 8); // 8 size
    succ = extInfo.GetUniformExtInfo(env, funcArg, NARG_POS::FOURTH);
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().UniformMatrixFv(
        env, funcArg[NARG_POS::FIRST], funcArg[NARG_POS::THIRD], transpose, &extInfo);
}

napi_value WebGL2RenderingContextBase::UniformMatrix3x4fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    bool transpose = false;
    tie(succ, transpose) = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    UniformExtInfo extInfo(WebGLArg::MATRIX_3X4_REQUIRE_MIN_SIZE, 12); // 12 size
    succ = extInfo.GetUniformExtInfo(env, funcArg, NARG_POS::FOURTH);
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().UniformMatrixFv(
        env, funcArg[NARG_POS::FIRST], funcArg[NARG_POS::THIRD], transpose, &extInfo);
}

napi_value WebGL2RenderingContextBase::VertexAttribI4iv(napi_env env, napi_callback_info info)
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
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().VertexAttribI4iv(env, index, funcArg[NARG_POS::SECOND]);
}

napi_value WebGL2RenderingContextBase::VertexAttribI4uiv(napi_env env, napi_callback_info info)
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
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().VertexAttribI4uiv(env, index, funcArg[NARG_POS::SECOND]);
}

napi_value WebGL2RenderingContextBase::InvalidateFramebuffer(napi_env env, napi_callback_info info)
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
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().InvalidateFrameBuffer(env, target, funcArg[NARG_POS::SECOND]);
}

napi_value WebGL2RenderingContextBase::InvalidateSubFramebuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::SIX)) {
        return NVal::CreateNull(env).val_;
    }
    bool succ = false;
    GLenum target = 0;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    BufferPosition position = { 0 };
    BufferSize size = { 0 };
    tie(succ, position.x) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, position.y) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, size.width) = NVal(env, funcArg[NARG_POS::FIFTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, size.height) = NVal(env, funcArg[NARG_POS::SIXTH]).ToGLsizei();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().InvalidateSubFrameBuffer(
        env, target, funcArg[NARG_POS::SECOND], position, size);
}

napi_value WebGL2RenderingContextBase::GetInternalformatParameter(napi_env env, napi_callback_info info)
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
    GLenum internalFormat = 0;
    tie(succ, internalFormat) = NVal(env, funcArg[NARG_POS::SECOND]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLenum pname = 0;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().GetInternalFormatParameter(env, target, internalFormat, pname);
}

napi_value WebGL2RenderingContextBase::TransformFeedbackVaryings(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLenum bufferMode = 0;
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    if (context == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    tie(succ, bufferMode) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        context->GetWebGL2RenderingContextImpl().SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    return context->GetWebGL2RenderingContextImpl().TransformFeedbackVaryings(
        env, funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND], bufferMode);
}

napi_value WebGL2RenderingContextBase::GetUniformIndices(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }

    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().GetUniformIndices(
        env, funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND]);
}

napi_value WebGL2RenderingContextBase::GetActiveUniforms(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLenum pname = 0;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().GetActiveUniforms(
        env, funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND], pname);
}

napi_value WebGL2RenderingContextBase::GetActiveUniformBlockParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLuint uniformBlockIndex = 0;
    tie(succ, uniformBlockIndex) = NVal(env, funcArg[NARG_POS::SECOND]).ToUint32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    GLenum pname = 0;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::THIRD]).ToGLenum();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().GetActiveUniformBlockParameter(
        env, funcArg[NARG_POS::FIRST], uniformBlockIndex, pname);
}

napi_value WebGL2RenderingContextBase::GetActiveUniformBlockName(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return NVal::CreateNull(env).val_;
    }

    bool succ = false;
    GLuint uniformBlockIndex = 0;
    tie(succ, uniformBlockIndex) = NVal(env, funcArg[NARG_POS::SECOND]).ToUint32();
    if (!succ) {
        return NVal::CreateNull(env).val_;
    }
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().GetActiveUniformBlockName(
        env, funcArg[NARG_POS::FIRST], uniformBlockIndex);
}

napi_value WebGL2RenderingContextBase::GetParameter(napi_env env, napi_callback_info info)
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
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().GetParameter(env, pname);
}

napi_value WebGL2RenderingContextBase::GetTexParameter(napi_env env, napi_callback_info info)
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
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().GetTexParameter(env, target, pname);
}

napi_value WebGL2RenderingContextBase::GetFramebufferAttachmentParameter(napi_env env, napi_callback_info info)
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
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().GetFrameBufferAttachmentParameter(env, target, attachment, pname);
}

napi_value WebGL2RenderingContextBase::BindBuffer(napi_env env, napi_callback_info info)
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
    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().BindBuffer(env, target, funcArg[NARG_POS::SECOND]);
}
napi_value WebGL2RenderingContextBase::DeleteBuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateNull(env).val_;
    }

    WebGL2RenderingContext* context = GetWebGL2RenderingContext(env, funcArg.GetThisVar());
    return (context == nullptr) ? NVal::CreateNull(env).val_ :
        context->GetWebGL2RenderingContextImpl().DeleteBuffer(env, funcArg[NARG_POS::FIRST]);
}
} // namespace Rosen
} // namespace OHOS
#ifdef __cplusplus
}
#endif