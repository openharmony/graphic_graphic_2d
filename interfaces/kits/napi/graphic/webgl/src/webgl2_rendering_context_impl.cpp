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
#include "context/webgl2_rendering_context_impl.h"

#include "context/webgl2_rendering_context_base.h"
#include "context/webgl_context_attributes.h"
#include "context/webgl_rendering_context.h"
#include "context/webgl_rendering_context_base.h"
#include "context/webgl_rendering_context_basic_base.h"
#include "napi/n_class.h"
#include "napi/n_func_arg.h"
#include "util/egl_manager.h"
#include "util/log.h"
#include "util/util.h"
#include "webgl/webgl_query.h"
#include "webgl/webgl_sampler.h"
#include "webgl/webgl_shader.h"
#include "webgl/webgl_sync.h"
#include "webgl/webgl_transform_feedback.h"
#include "webgl/webgl_vertex_array_object.h"

namespace OHOS {
namespace Rosen {
namespace Impl {
using namespace std;
#define SET_ERROR(error) \
    do {                      \
        LOGE("WebGL2 set error code %{public}u", error);    \
        SetError(error);      \
    } while (0)

#define SET_ERROR_WITH_LOG(error, info, ...) \
    do {                      \
        LOGE("WebGL2 set error code %{public}u" info, error, ##__VA_ARGS__);    \
        SetError(error);      \
    } while (0)

void WebGL2RenderingContextImpl::Init()
{
    WebGLRenderingContextBaseImpl::Init();
    if (maxSamplerUnit_) {
        return;
    }
    GLint max = 0;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max);
    maxSamplerUnit_ = static_cast<GLuint>(max);
    samplerUnits_.resize(max);

    glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS, &max);
    maxBoundTransformFeedbackBufferIndex_ = max;
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &max);
    maxBoundUniformBufferIndex_ = max;
    LOGD("WebGL2 Init maxBoundTransformFeedbackBufferIndex_ %{public}d, maxBoundUniformBufferIndex_ %{public}d"
        "maxSamplerUnit_ %{public}d",
        maxBoundTransformFeedbackBufferIndex_, maxBoundUniformBufferIndex_, max);
}

napi_value WebGL2RenderingContextImpl::CreateQuery(napi_env env)
{
    WebGLQuery* webGlQuery = nullptr;
    napi_value objQuery = WebGLQuery::CreateObjectInstance(env, &webGlQuery).val_;
    if (!objQuery) {
        return NVal::CreateNull(env).val_;
    }

    unsigned int queryId;
    glGenQueries(1, &queryId);
    webGlQuery->SetQuery(queryId);
    AddObject<WebGLQuery>(env, queryId, objQuery);
    LOGD("WebGL2 createQuery queryId = %{public}u", queryId);
    return objQuery;
}

napi_value WebGL2RenderingContextImpl::DeleteQuery(napi_env env, napi_value object)
{
    uint32_t queryId = 0;
    WebGLQuery* webGlQuery = WebGLObject::GetObjectInstance<WebGLQuery>(env, object);
    if (webGlQuery == nullptr) {
        return nullptr;
    }
    queryId = webGlQuery->GetQuery();
    glDeleteQueries(1, &queryId);
    DeleteObject<WebGLQuery>(env, queryId);
    uint32_t index = 0;
    LOGD("WebGL2 deleteQuery target %{public}u, queryId %{public}u", webGlQuery->GetTarget(), queryId);
    if (CheckQueryTarget(env, webGlQuery->GetTarget(), index)) {
        LOGD("WebGL2 deleteQuery currentQuery_ %{public}u", currentQuery_[index]);
        if (currentQuery_[index] == queryId) {
            currentQuery_[index] = 0;
            glEndQuery(webGlQuery->GetTarget());
        }
    }
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::IsQuery(napi_env env, napi_value object)
{
    GLuint queryId = 0;
    WebGLQuery* webGlQuery = WebGLObject::GetObjectInstance<WebGLQuery>(env, object);
    if (webGlQuery == nullptr) {
        return NVal::CreateBool(env, false).val_;
    }
    queryId = webGlQuery->GetQuery();

    GLboolean returnValue = glIsQuery(queryId);
    bool res = static_cast<bool>(returnValue);
    LOGD("WebGL2 isQuery query %{public}u result %{public}d ", queryId, res);
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGL2RenderingContextImpl::GetQuery(napi_env env, GLenum target, GLenum pName)
{
    LOGD("WebGL2 getQuery target %{public}u %{public}u", target, pName);
    if (pName != WebGL2RenderingContextBase::CURRENT_QUERY) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    uint32_t index = 0;
    if (!CheckQueryTarget(env, target, index)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }

    GLint params;
    glGetQueryiv(target, pName, &params);
    LOGD("WebGL2 getQuery params = %{public}u %{public}u", params, currentQuery_[index]);
    return GetObject<WebGLQuery>(env, params);
}

napi_value WebGL2RenderingContextImpl::BeginQuery(napi_env env, GLenum target, napi_value object)
{
    LOGD("WebGL2 beginQuery target %{public}u", target);
    GLuint queryId = 0;
    WebGLQuery* webGlQuery = WebGLObject::GetObjectInstance<WebGLQuery>(env, object);
    if (webGlQuery == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    queryId = webGlQuery->GetQuery();

    if (webGlQuery->GetTarget() && webGlQuery->GetTarget() != target) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    uint32_t index = 0;
    if (!CheckQueryTarget(env, target, index)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }

    if (currentQuery_[index] && currentQuery_[index] != queryId) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    currentQuery_[index] = queryId;
    webGlQuery->SetTarget(target);

    glBeginQuery(target, queryId);
    LOGD("WebGL2 beginQuery %{public}u queryId %{public}u result %{public}u", queryId, GetError_());
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::EndQuery(napi_env env, GLenum target)
{
    LOGD("WebGL2 endQuery target %{public}u", target);
    uint32_t index = 0;
    if (!CheckQueryTarget(env, target, index)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    if (!currentQuery_[index]) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    currentQuery_[index] = 0;
    glEndQuery(target);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::GetQueryParameter(napi_env env, napi_value queryObj, GLenum pName)
{
    GLuint queryId = 0;
    WebGLQuery* webGlQuery = WebGLObject::GetObjectInstance<WebGLQuery>(env, queryObj);
    if (webGlQuery == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }
    queryId = webGlQuery->GetQuery();

    GLuint params;
    switch (pName) {
        case GL_QUERY_RESULT: {
            glGetQueryObjectuiv(queryId, pName, &params);
            int64_t res = static_cast<int64_t>(params);
            LOGD("WebGL2 getQueryParameter params %{public}u", params);
            return NVal::CreateInt64(env, res).val_;
        }
        case GL_QUERY_RESULT_AVAILABLE: {
            glGetQueryObjectuiv(queryId, pName, &params);
            bool res = (params == GL_FALSE) ? false : true;
            LOGD("WebGL2 getQueryParameter params %{public}u", params);
            return NVal::CreateBool(env, res).val_;
        }
        default : {
            SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
            return NVal::CreateNull(env).val_;
        }
    }
}

napi_value WebGL2RenderingContextImpl::CreateSampler(napi_env env)
{
    WebGLSampler* webGlSampler = nullptr;
    napi_value objSampler = WebGLSampler::CreateObjectInstance(env, &webGlSampler).val_;
    if (!objSampler) {
        return NVal::CreateNull(env).val_;
    }
    GLuint samplerId;
    glGenSamplers(1, &samplerId);
    webGlSampler->SetSampler(samplerId);
    LOGD("WebGL2 createSampler samplerId = %{public}u", samplerId);
    AddObject<WebGLSampler>(env, samplerId, objSampler);
    return objSampler;
}

napi_value WebGL2RenderingContextImpl::DeleteSampler(napi_env env, napi_value samplerObj)
{
    WebGLSampler* sampler = WebGLObject::GetObjectInstance<WebGLSampler>(env, samplerObj);
    if (sampler == nullptr) {
        return nullptr;
    }
    GLuint samplerId = sampler->GetSampler();
    LOGD("WebGL2 deleteSampler samplerId = %{public}u", samplerId);
    // delete
    glBindSampler(sampler->GetSampleUnit(), 0);

    samplerUnits_[sampler->GetSampleUnit()] = 0;
    glDeleteSamplers(1, &samplerId);
    sampler->SetSampleUnit(0);
    DeleteObject<WebGLSampler>(env, samplerId);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::IsSampler(napi_env env, napi_value samplerObj)
{
    WebGLSampler* sampler = WebGLObject::GetObjectInstance<WebGLSampler>(env, samplerObj);
    if (sampler == nullptr) {
        return NVal::CreateBool(env, false).val_;
    }
    GLuint samplerId = sampler->GetSampler();

    GLboolean returnValue = glIsSampler(samplerId);
    bool res = static_cast<bool>(returnValue);
    LOGD("WebGL2 IsSampler samplerId = %{public}u res %{public}u", samplerId, res);
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGL2RenderingContextImpl::BindSampler(napi_env env, GLuint unit, napi_value samplerObj)
{
    WebGLSampler* sampler = WebGLObject::GetObjectInstance<WebGLSampler>(env, samplerObj);
    if (sampler == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    GLuint samplerId = sampler->GetSampler();
    if (unit >= samplerUnits_.size()) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    sampler->SetSampleUnit(unit);
    glBindSampler(unit, samplerId);
    samplerUnits_[unit] = samplerId;
    LOGD("WebGL2 bindSampler unit = %{public}u samplerId = %{public}u", unit, samplerId);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::SamplerParameter(
    napi_env env, napi_value samplerObj, GLenum pName, bool isFloat, void* param)
{
    LOGD("WebGL2 samplerParameteri pname %{public}u param %{public}u", pName, param);
    WebGLSampler* sampler = WebGLObject::GetObjectInstance<WebGLSampler>(env, samplerObj);
    if (sampler == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    GLuint samplerId = sampler->GetSampler();
    switch (pName) {
        case GL_TEXTURE_COMPARE_FUNC:
        case GL_TEXTURE_COMPARE_MODE:
        case GL_TEXTURE_MAG_FILTER:
        case GL_TEXTURE_MIN_FILTER:
        case GL_TEXTURE_WRAP_R:
        case GL_TEXTURE_WRAP_S:
        case GL_TEXTURE_WRAP_T:
        case GL_TEXTURE_MAX_LOD:
        case GL_TEXTURE_MIN_LOD:
            break;
        default:
            SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
            return nullptr;
    }
    if (isFloat) {
        GLfloat v = *static_cast<GLfloat*>(param);
        glSamplerParameterf(samplerId, pName, v);
    } else {
        GLint v = *static_cast<GLint*>(param);
        glSamplerParameteri(samplerId, pName, v);
    }
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::GetSamplerParameter(napi_env env, napi_value samplerObj, GLenum pName)
{
    WebGLSampler* sampler = WebGLObject::GetObjectInstance<WebGLSampler>(env, samplerObj);
    if (sampler == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return NVal::CreateNull(env).val_;
    }
    GLuint samplerId = sampler->GetSampler();

    switch (pName) {
        case GL_TEXTURE_COMPARE_FUNC:
        case GL_TEXTURE_COMPARE_MODE:
        case GL_TEXTURE_MAG_FILTER:
        case GL_TEXTURE_MIN_FILTER:
        case GL_TEXTURE_WRAP_R:
        case GL_TEXTURE_WRAP_S:
        case GL_TEXTURE_WRAP_T: {
            GLint params;
            glGetSamplerParameteriv(static_cast<GLuint>(samplerId), static_cast<GLenum>(pName), &params);
            int64_t res = static_cast<int64_t>(params);
            LOGD("WebGL2 getSamplerParameter samplerId %{public}u params %{public}u", samplerId, params);
            return NVal::CreateInt64(env, res).val_;
        }
        case GL_TEXTURE_MAX_LOD:
        case GL_TEXTURE_MIN_LOD: {
            GLfloat params;
            glGetSamplerParameterfv(static_cast<GLuint>(samplerId), static_cast<GLenum>(pName), &params);
            float res = static_cast<float>(params);
            LOGD("WebGL2 getSamplerParameter samplerId %{public}u params %{public}f", samplerId, params);
            return NVal::CreateDouble(env, (double)res).val_;
        }
        default:
            SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
            return NVal::CreateNull(env).val_;
    }
}

napi_value WebGL2RenderingContextImpl::CreateVertexArray(napi_env env)
{
    WebGLVertexArrayObject* webGLVertexArrayObject = nullptr;
    napi_value objVertexArrayObject = WebGLVertexArrayObject::CreateObjectInstance(env, &webGLVertexArrayObject).val_;
    if (!objVertexArrayObject) {
        return NVal::CreateNull(env).val_;
    }
    unsigned int vertexArraysId;
    glGenVertexArrays(1, &vertexArraysId);

    webGLVertexArrayObject->SetVertexArrays(vertexArraysId);
    LOGD("WebGL2 createVertexArray vertexArraysId = %{public}u", vertexArraysId);
    AddObject<WebGLVertexArrayObject>(env, vertexArraysId, objVertexArrayObject);
    return objVertexArrayObject;
}

napi_value WebGL2RenderingContextImpl::DeleteVertexArray(napi_env env, napi_value object)
{
    unsigned int vertexArrays = WebGLVertexArrayObject::DEFAULT_VERTEX_ARRAY_OBJECT;
    WebGLVertexArrayObject* webGLVertexArrayObject =
        WebGLObject::GetObjectInstance<WebGLVertexArrayObject>(env, object);
    if (webGLVertexArrayObject == nullptr) {
        return nullptr;
    }
    vertexArrays = webGLVertexArrayObject->GetVertexArrays();
    if (boundVertexArrayId_ && boundVertexArrayId_ == vertexArrays) {
        boundVertexArrayId_ = 0;
    }
    glDeleteVertexArrays(1, &vertexArrays);
    LOGD("WebGL2 deleteVertexArrays vertexArrays %{public}u", vertexArrays);
    DeleteObject<WebGLVertexArrayObject>(env, vertexArrays);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::IsVertexArray(napi_env env, napi_value object)
{
    GLuint vertexArrayId = WebGLVertexArrayObject::DEFAULT_VERTEX_ARRAY_OBJECT;
    WebGLVertexArrayObject* webGLVertexArrayObject =
        WebGLObject::GetObjectInstance<WebGLVertexArrayObject>(env, object);
    if (webGLVertexArrayObject == nullptr) {
        return NVal::CreateBool(env, false).val_;
    }
    vertexArrayId = webGLVertexArrayObject->GetVertexArrays();
    GLboolean returnValue = glIsVertexArray(vertexArrayId);
    LOGD("WebGL2 isVertexArray %{public}u %{public}u", vertexArrayId, returnValue);
    return NVal::CreateBool(env, returnValue).val_;
}

napi_value WebGL2RenderingContextImpl::BindVertexArray(napi_env env, napi_value object)
{
    GLuint vertexArrayId = WebGLVertexArrayObject::DEFAULT_VERTEX_ARRAY_OBJECT;
    WebGLVertexArrayObject* webGLVertexArrayObject =
        WebGLObject::GetObjectInstance<WebGLVertexArrayObject>(env, object);
    if (webGLVertexArrayObject == nullptr) {
        return nullptr;
    }
    vertexArrayId = webGLVertexArrayObject->GetVertexArrays();
    glBindVertexArray(vertexArrayId);
    boundVertexArrayId_ = vertexArrayId;
    LOGD("WebGL2 bindVertexArray %{public}u ", vertexArrayId);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::FenceSync(napi_env env, GLenum condition, GLbitfield flags)
{
    WebGLSync* webGlSync = nullptr;
    napi_value objSync = WebGLSync::CreateObjectInstance(env, &webGlSync).val_;
    if (!objSync) {
        return NVal::CreateNull(env).val_;
    }
    GLsync returnValue = glFenceSync(condition, flags);
    webGlSync->SetSync(reinterpret_cast<int64_t>(returnValue));
    LOGD("WebGL2 fenceSync syncId %{public}u result %{public}u condition %{public}u flags %{public}u",
        returnValue, GetError_(), condition, flags);
    return objSync;
}

napi_value WebGL2RenderingContextImpl::IsSync(napi_env env, napi_value syncObj)
{
    WebGLSync* webGlSync = WebGLObject::GetObjectInstance<WebGLSync>(env, syncObj);
    if (webGlSync == nullptr) {
        return NVal::CreateBool(env, false).val_;
    }
    int64_t syncId = webGlSync->GetSync();
    GLboolean returnValue = glIsSync(reinterpret_cast<GLsync>(syncId));
    LOGD("WebGL2 isSync syncId = %{public}ld result %{public}u", syncId, returnValue);
    return NVal::CreateBool(env, static_cast<bool>(returnValue)).val_;
}

napi_value WebGL2RenderingContextImpl::DeleteSync(napi_env env, napi_value syncObj)
{
    WebGLSync* webGlSync = WebGLObject::GetObjectInstance<WebGLSync>(env, syncObj);
    if (webGlSync == nullptr) {
        return nullptr;
    }
    int64_t syncId = webGlSync->GetSync();

    glDeleteSync(reinterpret_cast<GLsync>(syncId));
    LOGD("WebGL2 deleteSync syncId %{public}ld ", syncId);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::ClientWaitSync(
    napi_env env, napi_value syncObj, GLbitfield flags, GLint64 timeout)
{
    WebGLSync* webGlSync = WebGLObject::GetObjectInstance<WebGLSync>(env, syncObj);
    if (webGlSync == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateInt64(env, WebGL2RenderingContextBase::WAIT_FAILED).val_;
    }
    int64_t syncId = webGlSync->GetSync();
    if (timeout < WebGL2RenderingContextBase::TIMEOUT_IGNORED) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateInt64(env, WebGL2RenderingContextBase::WAIT_FAILED).val_;
    }
    GLuint64 timeout64 = (timeout == -1) ? GL_TIMEOUT_IGNORED : static_cast<GLuint64>(timeout);
    GLenum returnValue = glClientWaitSync(reinterpret_cast<GLsync>(syncId), flags, timeout64);
    LOGD("WebGL2 clientWaitSync syncId = %{public}u %{public}u result %{public}u", syncId, returnValue, GetError_());
    return NVal::CreateInt64(env, static_cast<int64_t>(returnValue)).val_;
}

napi_value WebGL2RenderingContextImpl::WaitSync(napi_env env, napi_value syncObj, GLbitfield flags, GLint64 timeout)
{
    WebGLSync* webGlSync = WebGLObject::GetObjectInstance<WebGLSync>(env, syncObj);
    if (webGlSync == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    int64_t syncId = webGlSync->GetSync();
    if (timeout < WebGL2RenderingContextBase::TIMEOUT_IGNORED) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    GLuint64 timeout64 = (timeout == -1) ? GL_TIMEOUT_IGNORED : static_cast<GLuint64>(timeout);

    glWaitSync(reinterpret_cast<GLsync>(syncId), flags, timeout64);
    LOGD("WebGL2 waitSync GL_TIMEOUT_IGNORED %{public}u", GL_TIMEOUT_IGNORED);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::GetSyncParameter(napi_env env, napi_value syncObj, GLenum pname)
{
    WebGLSync* webGlSync = WebGLObject::GetObjectInstance<WebGLSync>(env, syncObj);
    if (webGlSync == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }
    int64_t syncId = webGlSync->GetSync();
    LOGD("WebGL2 getSyncParameter syncId %{public}u pname %{public}u ", syncId, pname);
    if (CheckInList(pname, { GL_OBJECT_TYPE, GL_SYNC_STATUS, GL_SYNC_CONDITION, GL_SYNC_FLAGS })) {
        GLint value = 0;
        GLsizei length = -1;
        glGetSynciv(reinterpret_cast<GLsync>(syncId), pname, 1, &length, &value);
        LOGD("WebGL2 getSyncParameter value %{public}d ", value);
        return NVal::CreateInt64(env, value).val_;
    } else {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
}

} // namespace Impl
} // namespace Rosen
} // namespace OHOS