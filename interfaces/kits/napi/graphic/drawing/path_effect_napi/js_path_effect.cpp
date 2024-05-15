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

#include "js_path_effect.h"
#include "js_drawing_utils.h"
#include "native_value.h"

namespace OHOS::Rosen {
namespace Drawing {
const std::string CLASS_NAME = "PathEffect";
thread_local napi_ref JsPathEffect::constructor_ = nullptr;
napi_value JsPathEffect::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("createDashPathEffect", JsPathEffect::CreateDashPathEffect),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsPathEffect::Init failed to define pathEffect class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("JsPathEffect::Init failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsPathEffect::Init failed to set constructor");
        return nullptr;
    }

    status = napi_define_properties(env, exportObj, sizeof(properties) / sizeof(properties[0]), properties);
    if (status != napi_ok) {
        ROSEN_LOGE("JsPathEffect::Init failed to define static function");
        return nullptr;
    }
    return exportObj;
}

void JsPathEffect::Finalizer(napi_env env, void* data, void* hint)
{
    std::unique_ptr<JsPathEffect>(static_cast<JsPathEffect*>(data));
}

JsPathEffect::~JsPathEffect()
{
    m_pathEffect = nullptr;
}

napi_value JsPathEffect::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("JsPathEffect::Constructor failed to napi_get_cb_info");
        return nullptr;
    }

    JsPathEffect *jsPathEffect = new(std::nothrow) JsPathEffect();
    if (jsPathEffect == nullptr) {
        return nullptr;
    }
    status = napi_wrap(env, jsThis, jsPathEffect, JsPathEffect::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsPathEffect;
        ROSEN_LOGE("JsPathEffect::Constructor failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

void JsPathEffect::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsPathEffect *napi = reinterpret_cast<JsPathEffect *>(nativeObject);
        delete napi;
    }
}

napi_value JsPathEffect::CreateDashPathEffect(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);
    CHECK_EACH_PARAM(ARGC_ZERO, napi_object);
    CHECK_EACH_PARAM(ARGC_ONE, napi_number);

    uint32_t arrayLength = 0;
    napi_get_array_length(env, argv[ARGC_ZERO], &arrayLength);
    if (arrayLength % NUMBER_TWO) { // arrayLength must be an even number
        ROSEN_LOGE("JsPathEffect::CreateDashPathEffect count of intervals is not even : %{public}u", arrayLength);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    scalar intervals[arrayLength];
    for (size_t i = 0; i < arrayLength; i++) {
        bool hasElement = false;
        napi_has_element(env, argv[ARGC_ZERO], i, &hasElement);
        if (!hasElement) {
            ROSEN_LOGE("JsPathEffect::CreateDashPathEffect parameter check error");
            return NapiGetUndefined(env);
        }

        napi_value element = nullptr;
        napi_get_element(env, argv[ARGC_ZERO], i, &element);

        double value = 0;
        ConvertFromJsNumber(env, element, value);
        intervals[i] = value;
    }

    double phase = 0;
    if (!ConvertFromJsNumber(env, argv[ARGC_ONE], phase)) {
        ROSEN_LOGE("JsPathEffect::CreateDashPathEffect argv[1] is invalid");
        return NapiGetUndefined(env);
    }

    std::shared_ptr<PathEffect> pathEffect = PathEffect::CreateDashPathEffect(intervals, arrayLength, phase);
    return JsPathEffect::Create(env, pathEffect);
}

napi_value JsPathEffect::Create(napi_env env, std::shared_ptr<PathEffect> pathEffect)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr || pathEffect == nullptr) {
        ROSEN_LOGE("JsPathEffect::Create object is null");
        return NapiGetUndefined(env);
    }

    std::unique_ptr<JsPathEffect> jsPathEffect = std::make_unique<JsPathEffect>(pathEffect);
    napi_wrap(env, objValue, jsPathEffect.release(), JsPathEffect::Finalizer, nullptr, nullptr);

    if (objValue == nullptr) {
        ROSEN_LOGE("JsPathEffect::Create objValue is null");
        return NapiGetUndefined(env);
    }
    return objValue;
}

std::shared_ptr<PathEffect> JsPathEffect::GetPathEffect()
{
    return m_pathEffect;
}
} // namespace Drawing
} // namespace OHOS::Rosen
