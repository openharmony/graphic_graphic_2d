/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "js_path_iterator.h"

#include "native_value.h"

#include "path_napi/js_path.h"

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsPathIterator::constructor_ = nullptr;
const std::string CLASS_NAME = "PathIterator";

static const napi_property_descriptor g_properties[] = {
    DECLARE_NAPI_FUNCTION("next", JsPathIterator::Next),
    DECLARE_NAPI_FUNCTION("hasNext", JsPathIterator::HasNext),
    DECLARE_NAPI_FUNCTION("peek", JsPathIterator::Peek),
};

PathVerb JsPathIterator::nextInternal()
{
    if (m_done) {
        return PathVerb::DONE;
    }
    PathVerb verb = m_iter->Next(m_points);
    if (verb == PathVerb::CONIC) {
        float weight = m_iter->ConicWeight();
        m_points[MAX_PAIRS_PATHVERB-1].SetX(weight);
    } else if (verb == PathVerb::DONE) {
        m_done = true;
    }
    return verb;
}

PathVerb JsPathIterator::getReturnVerb(const PathVerb& cachedVerb)
{
    switch (cachedVerb) {
        case PathVerb::MOVE: return PathVerb::MOVE;
        case PathVerb::LINE: return PathVerb::LINE;
        case PathVerb::QUAD: return PathVerb::QUAD;
        case PathVerb::CONIC: return PathVerb::CONIC;
        case PathVerb::CUBIC: return PathVerb::CUBIC;
        case PathVerb::CLOSE: return PathVerb::CLOSE;
        case PathVerb::DONE: return PathVerb::DONE;
        default: return nextInternal();
    }
}

napi_value JsPathIterator::Init(napi_env env, napi_value exportObj)
{
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
        sizeof(g_properties) / sizeof(g_properties[0]), g_properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to define PathIterator class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to set constructor");
        return nullptr;
    }

    return exportObj;
}

napi_value JsPathIterator::Constructor(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_value jsThis = nullptr;
    JsPathIterator* jsPathIterator = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("PathIterator::Constructor Failed to napi_get_cb_info");
        return nullptr;
    }
    if (argc == ARGC_ONE) {
        napi_valuetype valueType = napi_undefined;
        if (argv[0] == nullptr || napi_typeof(env, argv[0], &valueType) != napi_ok || valueType != napi_object) {
            return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "JsPathIterator::Constructor Argv[0] is invalid");
        }
        JsPath* path = nullptr;
        GET_UNWRAP_PARAM(ARGC_ZERO, path);
        PathIterator* p = new PathIterator(*path->GetPath());
        jsPathIterator = new JsPathIterator(p);
    } else {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect number of parameters.");
    }
    if (!jsPathIterator) {
        ROSEN_LOGE("Failed to create JsPathIterator");
        return nullptr;
    }
    status = napi_wrap(env, jsThis, jsPathIterator,
        JsPathIterator::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsPathIterator;
        ROSEN_LOGE("JsPathIterator::Constructor Failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

void JsPathIterator::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsPathIterator *napi = reinterpret_cast<JsPathIterator *>(nativeObject);
        delete napi;
    }
}

napi_value JsPathIterator::CreateJsPathIterator(napi_env env, PathIterator* iter)
{
    napi_value constructor = nullptr;
    napi_status status = napi_get_reference_value(env, constructor_, &constructor);
    if (status != napi_ok) {
        delete iter;
        ROSEN_LOGE("JsPathIterator::CreateJsPathIterator Failed to napi_get_reference_value!");
        return nullptr;
    }
    napi_value result = nullptr;
    napi_create_object(env, &result);
    if (result == nullptr) {
        delete iter;
        ROSEN_LOGE("JsPathIterator::CreateJsPath Create pathiterator object failed!");
        return nullptr;
    }
    JsPathIterator* jsPathIterator = new JsPathIterator(iter);
    status = napi_wrap(env, result, jsPathIterator, JsPathIterator::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsPathIterator;
        ROSEN_LOGE("JsPathIterator::CreateJsPath failed to wrap native instance");
        return nullptr;
    }
    napi_define_properties(env, result, sizeof(g_properties) / sizeof(g_properties[0]), g_properties);
    return result;
}

JsPathIterator::~JsPathIterator()
{
    if (m_iter != nullptr) {
        delete m_iter;
        m_iter = nullptr;
    }
}

napi_value JsPathIterator::Next(napi_env env, napi_callback_info info)
{
    JsPathIterator* me = CheckParamsAndGetThis<JsPathIterator>(env, info);
    return (me != nullptr) ? me->OnNext(env, info) : nullptr;
}

napi_value JsPathIterator::HasNext(napi_env env, napi_callback_info info)
{
    JsPathIterator* me = CheckParamsAndGetThis<JsPathIterator>(env, info);
    return (me != nullptr) ? me->OnHasNext(env, info) : nullptr;
}

napi_value JsPathIterator::Peek(napi_env env, napi_callback_info info)
{
    JsPathIterator* me = CheckParamsAndGetThis<JsPathIterator>(env, info);
    return (me != nullptr) ? me->OnPeek(env, info) : nullptr;
}

napi_value JsPathIterator::OnNext(napi_env env, napi_callback_info info)
{
    if (m_iter == nullptr) {
        ROSEN_LOGE("JsPathIterator::OnNext pathiterator is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = { nullptr };
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_ONE, ARGC_TWO);

    napi_value array = argv[ARGC_ZERO];

    int32_t offset = 0;
    if (argc == ARGC_TWO) {
        GET_INT32_PARAM(ARGC_ONE, offset);
        if (offset < 0) {
            ROSEN_LOGE("JsPathIterator::OnNext offset is incorrect.");
            return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "Parameter verification failed. The value of offset must be greater than or equal to 0.");
        }
    }

    uint32_t size = 0;
    if (napi_get_array_length(env, array, &size) != napi_ok ||
        size < static_cast<uint32_t>(offset) + MAX_PAIRS_PATHVERB) {
        ROSEN_LOGE("JsPathIterator::OnNext array size is incorrect.");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect array size.");
    }

    PathVerb returnVerb = getReturnVerb(m_verb);
    m_verb = PathVerb::UNINIT;

    for (uint32_t i = 0; i < MAX_PAIRS_PATHVERB; i++) {
        if (napi_set_element(env, array, i + offset, ConvertPointToJsValue(env, m_points[i])) != napi_ok) {
            ROSEN_LOGE("JsPathIterator::OnNext set array failed");
            return nullptr;
        }
    }

    return CreateJsNumber(env, static_cast<uint32_t>(returnVerb));
}

napi_value JsPathIterator::OnPeek(napi_env env, napi_callback_info info)
{
    if (m_iter == nullptr) {
        ROSEN_LOGE("JsPathIterator::OnPeek pathiterator is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    if (m_done) {
        return CreateJsNumber(env, static_cast<int32_t>(PathVerb::DONE));
    }
    return CreateJsNumber(env, static_cast<int32_t>(m_iter->Peek()));
}

napi_value JsPathIterator::OnHasNext(napi_env env, napi_callback_info info)
{
    if (m_iter == nullptr) {
        ROSEN_LOGE("JsPathIterator::OnHasNext pathiterator is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    if (m_verb == PathVerb::UNINIT) {
        m_verb = nextInternal();
    }
    return CreateJsValue(env, m_verb != PathVerb::DONE);
}

PathIterator* JsPathIterator::GetPathIterator()
{
    return m_iter;
}
} // namespace Drawing
} // namespace OHOS::Rosen
