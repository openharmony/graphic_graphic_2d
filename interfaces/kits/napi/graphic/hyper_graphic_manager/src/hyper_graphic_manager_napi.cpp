/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <string>

#include "hgm_command.h"
#include "hgm_log.h"
#include "js_native_api.h"
#include "js_native_api_types.h"
#include "napi/native_common.h"
#include "napi/native_api.h"
#include "screen_types.h"
#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {
thread_local static RSInterfaces& interfaces = RSInterfaces::GetInstance();

static napi_value SetRefreshRateMode(napi_env env, napi_callback_info info)
{
    napi_status status;
    HGM_LOGE("Napi SetRefreshRateMode starting");

    size_t argc = 1;
    napi_value arg[argc];
    napi_get_cb_info(env, info, &argc, arg, nullptr, nullptr);

    //get refresh rate mode to set
    napi_valuetype valueType;
    status = napi_typeof(env, arg[0], &valueType);
    if (status != napi_ok) {
        HGM_LOGE("Napi SetRefreshRateMode failed to get argument");
        napi_throw_error(env, NULL, "Napi SetRefreshRateMode failed to get argument");
    }
    if (valueType != napi_number) {
        HGM_LOGE("Napi SetRefreshRateMode wrong argument type");
        napi_throw_error(env, NULL, "Napi SetRefreshRateMode wrong argument type");
    }

    int32_t mode;
    napi_get_value_int32(env, arg[0], &mode);
    interfaces.SetRefreshRateMode(mode);

    napi_value napiValue = nullptr;
    NAPI_CALL(env, napi_create_int32(env, EXEC_SUCCESS, &napiValue));
    return napiValue;
}

static napi_value GetScreenCurrentRefreshRate(napi_env env, napi_callback_info info)
{
    napi_status status;
    HGM_LOGE("Napi GetScreenCurrentRefreshRate starting");

    size_t argc = 1;
    napi_value arg[argc];
    napi_get_cb_info(env, info, &argc, arg, nullptr, nullptr);

    //get the screenId
    napi_valuetype valueType;
    status = napi_typeof(env, arg[0], &valueType);
    if (status != napi_ok) {
        HGM_LOGE("Napi GetScreenCurrentRefreshRate failed to get argument");
        napi_throw_error(env, NULL, "Napi GetScreenCurrentRefreshRate failed to get argument");
    }
    if (valueType != napi_number) {
        HGM_LOGE("Napi GetScreenCurrentRefreshRate wrong argument type");
        napi_throw_error(env, NULL, "Napi GetScreenCurrentRefreshRate wrong argument type");
    }

    int32_t screen = 0;
    ScreenId id = 0;
    napi_get_value_int32(env, arg[0], &screen);
    if (screen < 0) {
        HGM_LOGE("Napi GetScreenCurrentRefreshRate illegal screenId input: %d", screen);
        napi_throw_error(env, NULL, "Napi GetScreenCurrentRefreshRate illegal screenId");
    } else {
        id = static_cast<ScreenId>(screen);
    }

    uint32_t currentRate = interfaces.GetScreenCurrentRefreshRate(id);
    int32_t returnInt = 0;
    if (currentRate > static_cast<uint32_t>(OLED_MAX_HZ)) {
        HGM_LOGE("Napi GetScreenCurrentRefreshRate got a rate too above maximal value");
        napi_throw_error(env, NULL, "Napi GetScreenCurrentRefreshRate got a rate too above maximal value");
    } else {
        returnInt = static_cast<int32_t>(currentRate);
    }

    napi_value returnRate = nullptr;
    napi_create_int32(env, returnInt, &returnRate);
    return returnRate;
}

static napi_value GetScreenSupportedRefreshRates(napi_env env, napi_callback_info info)
{
    napi_status status;
    HGM_LOGE("Napi GetScreenSupportedRefreshRates starting");

    size_t argc = 1;
    napi_value arg[argc];
    napi_get_cb_info(env, info, &argc, arg, nullptr, nullptr);

    //get the screenId
    napi_valuetype valueType;
    status = napi_typeof(env, arg[0], &valueType);
    if (status != napi_ok) {
        HGM_LOGE("Napi GetScreenSupportedRefreshRates failed to get argument");
        napi_throw_error(env, NULL, "Napi GetScreenSupportedRefreshRates failed to get argument");
    }
    if (valueType != napi_number) {
        HGM_LOGE("Napi GetScreenSupportedRefreshRates wrong argument type");
        napi_throw_error(env, NULL, "Napi GetScreenSupportedRefreshRates wrong argument type");
    }

    int32_t screen = 0;
    ScreenId id = 0;
    napi_get_value_int32(env, arg[0], &screen);
    if (screen < 0) {
        HGM_LOGE("Napi GetScreenSupportedRefreshRates illegal screenId input: %d", screen);
        napi_throw_error(env, NULL, "Napi GetScreenSupportedRefreshRates illegal screenId input");
    } else {
        id = static_cast<ScreenId>(screen);
    }
    std::vector<uint32_t> currentRates = interfaces.GetScreenSupportedRefreshRates(id);

    napi_value returnRates = nullptr;
    NAPI_CALL(env, napi_create_array(env, &returnRates));
    int numSupportedRates = currentRates.size();
    std::vector<napi_value> napiVector(numSupportedRates, nullptr);
    for (int index = 0; index < numSupportedRates; ++index) {
        if (currentRates[index] > static_cast<uint32_t>(OLED_MAX_HZ)) {
            HGM_LOGE("Napi GetScreenCurrentRefreshRate got a rate too above maximal value");
            napi_throw_error(env, NULL, "Napi GetScreenCurrentRefreshRate got a rate too above maximal value");
        }
        NAPI_CALL(env, napi_create_int32(env, currentRates[index], &napiVector[index]));
        NAPI_CALL(env, napi_set_element(env, returnRates, currentRates[index], napiVector[index]));
    }

    return returnRates;
}

EXTERN_C_START
static napi_value HgmInit(napi_env env, napi_value exports)
{
    HGM_LOGD("HgmInit Napi");

    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("setRefreshRateMode", SetRefreshRateMode),
        DECLARE_NAPI_FUNCTION("getScreenCurrentRefreshRate", GetScreenCurrentRefreshRate),
        DECLARE_NAPI_FUNCTION("getScreenSupportedRefreshRates", GetScreenSupportedRefreshRates),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(napi_property_descriptor), desc));

    return exports;
}
EXTERN_C_END

static napi_module hgm_module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = "libhgmnapi",
    .nm_register_func = HgmInit,
    .nm_modname = "libhgmnapi",
    .nm_priv = ((void*)0),
    .reserved = {0}
};

extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&hgm_module);
}
} // namespace Rosen
} // namespace OHOS