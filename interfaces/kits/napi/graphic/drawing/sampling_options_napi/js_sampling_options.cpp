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

#include "js_sampling_options.h"
#include "js_drawing_utils.h"
#include "native_value.h"

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsSamplingOptions::constructor_ = nullptr;
const std::string CLASS_NAME = "SamplingOptions";
napi_value JsSamplingOptions::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("__createTransfer__", SamplingOptionsTransferDynamic),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsSamplingOptions::Init failed to define SamplingOptions class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("JsSamplingOptions::Init failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsSamplingOptions::Init failed to set constructor");
        return nullptr;
    }

    return exportObj;
}

napi_value JsSamplingOptions::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, argv, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("JsSamplingOptions::Constructor failed to napi_get_cb_info");
        return nullptr;
    }
    JsSamplingOptions* jsSamplingOptions = nullptr;
    std::shared_ptr<SamplingOptions> samplingOptions = nullptr;
    if (argCount == ARGC_ZERO) {
        samplingOptions = std::make_shared<SamplingOptions>();
        jsSamplingOptions = new JsSamplingOptions(samplingOptions);
    } else {
        int32_t jsFilterMode = 0;
        GET_INT32_CHECK_GE_ZERO_PARAM(ARGC_ZERO, jsFilterMode);
        samplingOptions = std::make_shared<SamplingOptions>(FilterMode(jsFilterMode));
        jsSamplingOptions = new JsSamplingOptions(samplingOptions);
    }

    status = napi_wrap(env, jsThis, jsSamplingOptions,
                       JsSamplingOptions::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsSamplingOptions;
        ROSEN_LOGE("JsSamplingOptions::Constructor failed to wrap native instance");
        return nullptr;
    }

    return jsThis;
}

void JsSamplingOptions::Destructor(napi_env env, void* nativeObject, void* finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsSamplingOptions* napi = reinterpret_cast<JsSamplingOptions*>(nativeObject);
        delete napi;
    }
}

JsSamplingOptions::~JsSamplingOptions()
{
    m_samplingOptions = nullptr;
}

napi_value JsSamplingOptions::CreateJsSamplingOptionsDynamic(
    napi_env env, const std::shared_ptr<SamplingOptions> samplingOptions)
{
    napi_value result = nullptr;
    napi_value constructor = nullptr;
    if (napi_get_reference_value(env, constructor_, &constructor) != napi_ok) {
        ROSEN_LOGE("Failed to get the representation of constructor object");
        return nullptr;
    }
    if (napi_new_instance(env, constructor, 0, nullptr, &result) != napi_ok || result == nullptr) {
        ROSEN_LOGE("Failed to instantiate JavaScript SamplingOptions instance");
        return nullptr;
    }
    JsSamplingOptions* jsSamplingOptions = new JsSamplingOptions(samplingOptions);
    napi_status status = napi_wrap(env, result, jsSamplingOptions, JsSamplingOptions::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsSamplingOptions;
        ROSEN_LOGE("Failed to wrap native instance");
        return nullptr;
    }
    return result;
}

napi_value JsSamplingOptions::SamplingOptionsTransferDynamic(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv;
    if (napi_get_cb_info(env, info, &argc, &argv, nullptr, nullptr) != napi_ok || argc != 1) {
        return nullptr;
    }

    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv, &valueType);
    if (valueType != napi_number) {
        return nullptr;
    }

    int64_t addr = 0;
    napi_get_value_int64(env, argv, &addr);
    std::shared_ptr<SamplingOptions> samplingOptions = *reinterpret_cast<std::shared_ptr<SamplingOptions>*>(addr);
    if (samplingOptions == nullptr) {
        return nullptr;
    }
    return CreateJsSamplingOptionsDynamic(env, samplingOptions);
}

} // namespace Drawing
} // namespace OHOS::Rosen
