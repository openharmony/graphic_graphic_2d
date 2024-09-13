/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#include "js_color_space.h"

#include <array>

#include "color_space.h"
#include "js_color_space_utils.h"
#include "napi/native_api.h"

namespace OHOS {
namespace ColorManager {
void JsColorSpace::Finalizer(napi_env env, void* data, void* hint)
{
    auto jsColorSpace = std::unique_ptr<JsColorSpace>(static_cast<JsColorSpace*>(data));
    if (jsColorSpace == nullptr) {
        CMLOGE("[NAPI]Finalizer jsColorSpace is nullptr");
        return;
    }
    auto csToken = jsColorSpace->colorSpaceToken_;
    if (csToken == nullptr) {
        CMLOGE("[NAPI]Finalizer colorSpaceToken_ is nullptr");
        return;
    }
}

napi_value JsColorSpace::GetColorSpaceName(napi_env env, napi_callback_info info)
{
    JsColorSpace* me = CheckParamsAndGetThis<JsColorSpace>(env, info);
    if (me == nullptr) {
        napi_throw(env,
            CreateJsError(env, static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_NULLPTR)),
            "Parameter check fails. Js color space object is nullptr."));
        return nullptr;
    }
    return me->OnGetColorSpaceName(env, info);
}

napi_value JsColorSpace::GetSendableColorSpaceName(napi_env env, napi_callback_info info)
{
    JsColorSpace* me = CheckSendableParamsAndGetThis<JsColorSpace>(env, info);
    if (me == nullptr) {
        napi_throw(env,
            CreateJsError(env, static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_NULLPTR)),
            "Parameter check fails. Js color space object is nullptr."));
        return nullptr;
    }
    return me->OnGetSendableColorSpaceName(env, info);
}

napi_value JsColorSpace::GetWhitePoint(napi_env env, napi_callback_info info)
{
    JsColorSpace* me = CheckParamsAndGetThis<JsColorSpace>(env, info);
    if (me == nullptr) {
        napi_throw(env,
            CreateJsError(env, static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_NULLPTR)),
            "Parameter check fails. Js color space object is nullptr."));
        return nullptr;
    }
    return me->OnGetWhitePoint(env, info);
}

napi_value JsColorSpace::GetSendableWhitePoint(napi_env env, napi_callback_info info)
{
    JsColorSpace* me = CheckSendableParamsAndGetThis<JsColorSpace>(env, info);
    if (me == nullptr) {
        napi_throw(env,
            CreateJsError(env, static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_NULLPTR)),
            "Parameter check fails. Js color space object is nullptr."));
        return nullptr;
    }
    return me->OnGetSendableWhitePoint(env, info);
}

napi_value JsColorSpace::GetGamma(napi_env env, napi_callback_info info)
{
    JsColorSpace* me = CheckParamsAndGetThis<JsColorSpace>(env, info);
    if (me == nullptr) {
        napi_throw(env,
            CreateJsError(env, static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_NULLPTR)),
            "Parameter check fails. Js color space object is nullptr."));
        return nullptr;
    }
    return me->OnGetGamma(env, info);
}

napi_value JsColorSpace::GetSendableGamma(napi_env env, napi_callback_info info)
{
    JsColorSpace* me = CheckSendableParamsAndGetThis<JsColorSpace>(env, info);
    if (me == nullptr) {
        napi_throw(env,
            CreateJsError(env, static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_NULLPTR)),
            "Parameter check fails. Js color space object is nullptr."));
        return nullptr;
    }
    return me->OnGetSendableGamma(env, info);
}

napi_value JsColorSpace::OnGetColorSpaceName(napi_env env, napi_callback_info info)
{
    napi_value value = nullptr;
    if (colorSpaceToken_ == nullptr) {
        CMLOGE("[NAPI]colorSpaceToken_ is nullptr");
        napi_throw(env,
            CreateJsError(env, static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_NULLPTR)),
            "Parameter check fails. Native color space object is nullptr."));
        NAPI_CALL_WITH_ERRCODE_DEFAULT(env, napi_get_undefined(env, &value));
        return value;
    }
    ColorSpaceName csName = colorSpaceToken_->GetColorSpaceName();
    auto iter = NATIVE_TO_JS_COLOR_SPACE_TYPE_MAP.find(csName);
    if (iter != NATIVE_TO_JS_COLOR_SPACE_TYPE_MAP.end()) {
        CMLOGI("[NAPI]get color space name %{public}u, api type %{public}u",
            csName, iter->second);
        return CreateJsValue(env, iter->second);
    }
    CMLOGE("[NAPI]get color space name %{public}u, but not in api type", csName);
    std::string errMsg = "BusinessError 401: Parameter error, the type of colorspace " +
        std::to_string(static_cast<int32_t>(csName)) + "must be in supported type list.";
    napi_throw(env,
        CreateJsError(env, static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_INVALID_PARAM)),
        errMsg));
    NAPI_CALL_WITH_ERRCODE_DEFAULT(env, napi_get_undefined(env, &value));
    return value;
}

napi_value JsColorSpace::OnGetSendableColorSpaceName(napi_env env, napi_callback_info info)
{
    napi_value value = nullptr;
    if (colorSpaceToken_ == nullptr) {
        CMLOGE("[NAPI]colorSpaceToken_ is nullptr");
        napi_throw(env,
            CreateJsError(env, static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_NULLPTR)),
            "Parameter check fails. Native color space object is nullptr."));
        NAPI_CALL_WITH_ERRCODE_DEFAULT(env, napi_get_undefined(env, &value));
        return value;
    }
    ColorSpaceName csName = colorSpaceToken_->GetColorSpaceName();
    auto iter = NATIVE_TO_JS_COLOR_SPACE_TYPE_MAP.find(csName);
    if (iter != NATIVE_TO_JS_COLOR_SPACE_TYPE_MAP.end()) {
        CMLOGI("[NAPI]get color space name %{public}u, api type %{public}u",
            csName, iter->second);
        return CreateJsValue(env, iter->second);
    }
    CMLOGE("[NAPI]get color space name %{public}u, but not in api type", csName);
    std::string errMsg = "BusinessError 401: Parameter error, the type of colorspace " +
        std::to_string(static_cast<int32_t>(csName)) + "must be in supported type list.";
    napi_throw(env,
        CreateJsError(env, static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_INVALID_PARAM)),
        errMsg));
    NAPI_CALL_WITH_ERRCODE_DEFAULT(env, napi_get_undefined(env, &value));
    return value;
}

napi_value JsColorSpace::OnGetWhitePoint(napi_env env, napi_callback_info info)
{
    napi_value arrayValue = nullptr;
    if (colorSpaceToken_ == nullptr) {
        CMLOGE("[NAPI]colorSpaceToken_ is nullptr");
        napi_throw(env,
            CreateJsError(env, static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_NULLPTR)),
            "Parameter check fails. Native color space object is nullptr."));
        NAPI_CALL_WITH_ERRCODE_DEFAULT(env, napi_get_undefined(env, &arrayValue));
        return arrayValue;
    }
    std::array<float, DIMES_2> wp = colorSpaceToken_->GetWhitePoint();
    NAPI_CALL_WITH_ERRCODE_DEFAULT(env, napi_create_array_with_length(env, DIMES_2, &arrayValue));
    for (uint32_t i = 0; i < DIMES_2; i++) {
        napi_value eleValue = CreateJsValue(env, wp[i]);
        NAPI_CALL_WITH_ERRCODE_DEFAULT(env, napi_set_element(env, arrayValue, i, eleValue));
    }
    return arrayValue;
}

napi_value JsColorSpace::OnGetSendableWhitePoint(napi_env env, napi_callback_info info)
{
    napi_value arrayValue = nullptr;
    if (colorSpaceToken_ == nullptr) {
        CMLOGE("[NAPI]colorSpaceToken_ is nullptr");
        napi_throw(env,
            CreateJsError(env, static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_NULLPTR)),
            "Parameter check fails. Native color space object is nullptr."));
        NAPI_CALL_WITH_ERRCODE_DEFAULT(env, napi_get_undefined(env, &arrayValue));
        return arrayValue;
    }
    std::array<float, DIMES_2> wp = colorSpaceToken_->GetWhitePoint();
    NAPI_CALL_WITH_ERRCODE_DEFAULT(env, napi_create_sendable_array_with_length(env, DIMES_2, &arrayValue));
    for (uint32_t i = 0; i < DIMES_2; i++) {
        napi_value eleValue = CreateJsValue(env, wp[i]);
        NAPI_CALL_WITH_ERRCODE_DEFAULT(env, napi_set_element(env, arrayValue, i, eleValue));
    }
    return arrayValue;
}

napi_value JsColorSpace::OnGetGamma(napi_env env, napi_callback_info info)
{
    napi_value value = nullptr;
    if (colorSpaceToken_ == nullptr) {
        CMLOGE("[NAPI]colorSpaceToken_ is nullptr");
        napi_throw(env,
            CreateJsError(env, static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_NULLPTR)),
            "Parameter check fails. Native color space object is nullptr."));
        NAPI_CALL_WITH_ERRCODE_DEFAULT(env, napi_get_undefined(env, &value));
        return value;
    }
    float gamma = colorSpaceToken_->GetGamma();
    return CreateJsValue(env, gamma);
}

napi_value JsColorSpace::OnGetSendableGamma(napi_env env, napi_callback_info info)
{
    napi_value value = nullptr;
    if (colorSpaceToken_ == nullptr) {
        CMLOGE("[NAPI]colorSpaceToken_ is nullptr");
        napi_throw(env,
            CreateJsError(env, static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_NULLPTR)),
            "Parameter check fails. Native color space object is nullptr."));
        NAPI_CALL_WITH_ERRCODE_DEFAULT(env, napi_get_undefined(env, &value));
        return value;
    }
    float gamma = colorSpaceToken_->GetGamma();
    return CreateJsValue(env, gamma);
}

napi_status BindFunctions(napi_env env, napi_value object)
{
    napi_status status = napi_ok;
    status = BindNativeFunction(env, object, "getColorSpaceName", nullptr, JsColorSpace::GetColorSpaceName);
    if (status != napi_ok) {
        return status;
    }
    status = BindNativeFunction(env, object, "getWhitePoint", nullptr, JsColorSpace::GetWhitePoint);
    if (status != napi_ok) {
        return status;
    }
    status = BindNativeFunction(env, object, "getGamma", nullptr, JsColorSpace::GetGamma);
    if (status != napi_ok) {
        return status;
    }
    return status;
}
} // namespace ColorManager
} // namespace OHOS
