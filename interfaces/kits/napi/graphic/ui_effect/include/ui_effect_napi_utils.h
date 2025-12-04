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

#ifndef OHOS_UIEFFECT_NAPI_UTILS_H
#define OHOS_UIEFFECT_NAPI_UTILS_H

#include <hilog/log.h>

#include "mask/include/radial_gradient_mask_para.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "common/rs_vector3.h"
#include "common/rs_vector4.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001405

#undef LOG_TAG
#define LOG_TAG "UiEffect_Util"
#define UTIL_LOG_E(fmt, ...) HILOG_ERROR(LOG_CORE, fmt, ##__VA_ARGS__)

#define UIEFFECT_NAPI_CHECK_RET(x, res) \
do \
{ \
    if (!(x)) \
    { \
        return (res); \
    } \
} while (0)

#define UIEFFECT_NAPI_CHECK_RET_VOID_D(x, msg) \
do \
{ \
    if (!(x)) \
    { \
        msg; \
        return; \
    } \
} while (0)

#define UIEFFECT_NAPI_CHECK_RET_D(x, res, msg) \
do \
{ \
    if (!(x)) \
    { \
        msg; \
        return (res); \
    } \
} while (0)

#define UIEFFECT_NAPI_CHECK_RET_DELETE_POINTER(x, res, pointer, msg) \
do \
{ \
    if (!(x)) \
    { \
        msg; \
        delete pointer; \
        pointer = nullptr; \
        return (res); \
    } \
} while (0)

#define UIEFFECT_JS_ARGS(env, info, status, argc, argv, thisVar) \
do \
{ \
    status = napi_get_cb_info(env, info, &(argc), argv, &(thisVar), nullptr); \
} while (0)

#define UIEFFECT_ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

namespace OHOS {
namespace Rosen {
namespace UIEffect {
constexpr uint32_t NUM_0 = 0;
constexpr uint32_t NUM_1 = 1;
constexpr uint32_t NUM_2 = 2;
constexpr uint32_t NUM_3 = 3;
constexpr uint32_t NUM_4 = 4;
constexpr uint32_t NUM_5 = 5;
constexpr uint32_t NUM_6 = 6;
constexpr uint32_t NUM_7 = 7;
constexpr uint32_t NUM_8 = 8;
constexpr uint32_t NUM_9 = 9;
constexpr uint32_t NUM_10 = 10;
constexpr uint32_t NUM_11 = 11;
constexpr uint32_t NUM_12 = 12;
constexpr uint32_t NUM_13 = 13;
constexpr uint32_t NUM_14 = 14;
constexpr uint32_t NUM_15 = 15;
constexpr uint32_t NUM_16 = 16;
constexpr uint32_t NUM_17 = 17;
constexpr uint32_t NUM_18 = 18;
constexpr uint32_t NUM_19 = 19;
constexpr uint32_t NUM_20 = 20;
constexpr uint32_t NUM_21 = 21;
constexpr uint32_t NUM_22 = 22;
constexpr uint32_t NUM_23 = 23;
constexpr uint32_t NUM_24 = 24;
constexpr uint32_t NUM_25 = 25;
constexpr uint32_t NUM_26 = 26;
constexpr uint32_t NUM_27 = 27;
constexpr uint32_t NUM_1000 = 1000;
constexpr int32_t ERR_NOT_SYSTEM_APP = 202;

bool ConvertDoubleValueFromJsElement(napi_env env, napi_value jsObject, uint32_t idx, double& data);
bool ParseJsDoubleValue(napi_env env, napi_value jsObject, double& data);
bool ParseJsDoubleValue(napi_env env, napi_value jsObject, const std::string& name, double& data);
bool ParseJsBoolValue(napi_env env, napi_value jsObject, const std::string& name, bool& data);
bool ParseJsBoolValue(napi_env env, napi_value jsObject, bool& data);
bool ParseJsVector2f(napi_env env, napi_value jsObject, Vector2f& values);
bool ConvertFromJsPoint(napi_env env, napi_value jsObject, double* point, size_t size);
bool ParseJsRGBAColor(napi_env env, napi_value jsValue, Vector4f& rgba);
bool ParseJsPoint(napi_env env, napi_value jsObject, Vector2f& point);
bool ParseJsLTRBRect(napi_env env, napi_value jsValue, Vector4f& ltrb);
bool ParseJsVector3f(napi_env env, napi_value jsObject, Vector3f& values);
} // namespace UIEffect

class UIEffectNapiUtils {
public:
    static napi_valuetype GetType(napi_env env, napi_value root);
    static bool IsSystemApp();
    static bool IsFormRenderServiceCall();
private:
    static std::string GetBundleName();
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_UIEFFECT_NAPI_UTILS_H
