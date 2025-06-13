/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "ui_effect_napi_utils.h"
#include "common/rs_vector4.h"
#ifdef ENABLE_IPC_SECURITY
#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#endif

namespace OHOS {
namespace Rosen {
namespace UIEffect {

constexpr const char* POINT_STRING[2] = { "x", "y" };
constexpr const char* POINT3D_STRING[3] = { "x", "y", "z" };
constexpr const char* COLOR_STRING[4] = {"red", "green", "blue", "alpha"};
constexpr const char* RECT_STRING[4] = {"left", "top", "right", "bottom"};

bool ConvertDoubleValueFromJsElement(napi_env env, napi_value jsObject, uint32_t idx, double& data)
{
    napi_value value = nullptr;
    napi_get_element(env, jsObject, idx, &value);
    if (!value) {
        return false;
    }
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, value, &valueType);
    if (valueType == napi_undefined) {
        return false;
    }
    if (napi_get_value_double(env, value, &data) != napi_ok) {
        return false;
    }
    return true;
}

bool ParseJsDoubleValue(napi_env env, napi_value jsObject, double& data)
{
    if (UIEffectNapiUtils::GetType(env, jsObject) == napi_number &&
        napi_get_value_double(env, jsObject, &data) == napi_ok) {
            return true;
    }

    return false;
}

bool ParseJsDoubleValue(napi_env env, napi_value jsObject, const std::string& name, double& data)
{
    napi_value value = nullptr;
    napi_get_named_property(env, jsObject, name.c_str(), &value);
    if (!value) {
        return false;
    }
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, value, &valueType);
    if (valueType == napi_undefined) {
        return false;
    }
    if (napi_get_value_double(env, value, &data) != napi_ok) {
        return false;
    }
    return true;
}

bool ParseJsVector2f(napi_env env, napi_value jsObject, Vector2f& values)
{
    uint32_t length = 0;
    if (napi_get_array_length(env, jsObject, &length) != napi_ok || length != NUM_2) {
        return false;
    }
    double val;
    if (!ConvertDoubleValueFromJsElement(env, jsObject, NUM_0, val)) {
        return false;
    }
    values[NUM_0] = static_cast<float>(val);
    if (!ConvertDoubleValueFromJsElement(env, jsObject, NUM_1, val)) {
        return false;
    }
    values[NUM_1] = static_cast<float>(val);
    return true;
}

bool ParseJsVector3f(napi_env env, napi_value jsObject, Vector3f& values)
{
    for (size_t idx = 0; idx < NUM_3; idx ++) {
        napi_value tempValue = nullptr;
        napi_get_named_property(env, jsObject, POINT3D_STRING[idx], &tempValue);
        double value = 0.0;
        if (tempValue == nullptr || napi_get_value_double(env, tempValue, &value) != napi_ok) {
            return false;
        }
        values[idx] = static_cast<float>(value);
    }
    return true;
}

bool ConvertFromJsPoint(napi_env env, napi_value jsObject, double* point, size_t size)
{
    if (!point || size < NUM_2) {
        return false;
    }
    napi_value tmpValue = nullptr;
    for (size_t idx = 0; idx < NUM_2; idx++) {
        double* curEdge = point + idx;
        napi_get_named_property(env, jsObject, POINT_STRING[idx], &tmpValue);
        if (napi_get_value_double(env, tmpValue, curEdge) != napi_ok) {
            return false;
        }
    }
    return true;
}

bool ParseJsPoint(napi_env env, napi_value jsObject, Vector2f& point)
{
    napi_value tmpValue = nullptr;
    for (size_t idx = 0; idx < NUM_2; idx++) {
        if (napi_get_named_property(env, jsObject, POINT_STRING[idx], &tmpValue) != napi_ok || tmpValue == nullptr) {
            return false;
        }

        double value = 0.0;
        if (napi_get_value_double(env, tmpValue, &value) != napi_ok) {
            return false;
        }

        point[idx] = static_cast<float>(value);
    }
    return true;
}

bool ParseJsRGBAColor(napi_env env, napi_value jsValue, Vector4f& rgba)
{
    for (size_t idx = 0; idx < NUM_4; idx++) {
        napi_value tempValue = nullptr;
        napi_get_named_property(env, jsValue, COLOR_STRING[idx], &tempValue);
        double value = 0.0;
        if (tempValue == nullptr || napi_get_value_double(env, tempValue, &value) != napi_ok) {
            return false;
        }
        rgba[idx] = static_cast<float>(value);
    }
    return true;
}

bool ParseJsLTRBRect(napi_env env, napi_value jsValue, Vector4f& ltrb)
{
    for (size_t idx = 0; idx < NUM_4; idx++) {
        napi_value tempValue = nullptr;
        napi_get_named_property(env, jsValue, RECT_STRING[idx], &tempValue);
        double value = 0.0;
        if (tempValue == nullptr || napi_get_value_double(env, tempValue, &value) != napi_ok) {
            return false;
        }
        ltrb[idx] = static_cast<float>(value);
    }
    return true;
}

} // namespace UIEffect

napi_valuetype UIEffectNapiUtils::GetType(napi_env env, napi_value root)
{
    napi_valuetype res = napi_undefined;
    napi_typeof(env, root, &res);
    return res;
}

bool UIEffectNapiUtils::IsSystemApp()
{
#ifdef ENABLE_IPC_SECURITY
    uint64_t tokenId = OHOS::IPCSkeleton::GetCallingFullTokenID();
    return Security::AccessToken::AccessTokenKit::IsSystemAppByFullTokenID(tokenId);
#else
    return true;
#endif
}

} // namespace Rosen
} // namespace OHOS
