/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "native_display_module.h"

#include <window_manager_service_client.h>

#include "graphic_napi_common.h"

namespace OHOS {
// getDefaultDisplay {{{
namespace getDefaultDisplay {
struct Param {
    WMError wret;
    std::vector<WMDisplayInfo> displayInfos;
};

bool Async(napi_env env, std::unique_ptr<Param> &param)
{
    const auto &wmsc = WindowManagerServiceClient::GetInstance();
    auto wret = wmsc->Init();
    if (wret != WM_OK) {
        GNAPI_LOG("WindowManagerServiceClient::Init() return %{public}s", WMErrorStr(wret).c_str());
        param->wret = wret;
        return false;
    }

    auto iWindowManagerService = wmsc->GetService();
    if (!iWindowManagerService) {
        GNAPI_LOG("can not get iWindowManagerService");
        param->wret = wret;
        return false;
    }

    param->wret = iWindowManagerService->GetDisplays(param->displayInfos);
    return true;
}

napi_value Resolve(napi_env env, std::unique_ptr<Param> &param)
{
    if (param->wret != WM_OK) {
        return CreateError(env, "failed with %s", WMErrorStr(param->wret).c_str());
    }

    if (param->displayInfos.empty()) {
        GNAPI_LOG("-----displayInfos is null-----");
        WMDisplayInfo displayInfo = {};
        param->displayInfos.push_back(displayInfo);
    }

    const auto &displayInfo = param->displayInfos[0];
    GNAPI_LOG("id        : %{public}d", param->displayInfos[0].id);
    GNAPI_LOG("width     : %{public}d", param->displayInfos[0].width);
    GNAPI_LOG("height    : %{public}d", param->displayInfos[0].height);
    GNAPI_LOG("phyWidth  : %{public}d", param->displayInfos[0].phyWidth);
    GNAPI_LOG("phyHeight : %{public}d", param->displayInfos[0].phyHeight);
    GNAPI_LOG("vsync     : %{public}d", param->displayInfos[0].vsync);

    napi_value result;
    NAPI_CALL(env, napi_create_object(env, &result));
    NAPI_CALL(env, SetMemberInt32(env, result, "id", displayInfo.id));
    NAPI_CALL(env, SetMemberUndefined(env, result, "name"));
    NAPI_CALL(env, SetMemberUndefined(env, result, "alive"));
    NAPI_CALL(env, SetMemberUndefined(env, result, "state"));
    NAPI_CALL(env, SetMemberUndefined(env, result, "refreshRate"));
    NAPI_CALL(env, SetMemberUndefined(env, result, "rotation"));
    NAPI_CALL(env, SetMemberUint32(env, result, "width", displayInfo.width));
    NAPI_CALL(env, SetMemberUint32(env, result, "height", displayInfo.height));
    NAPI_CALL(env, SetMemberUndefined(env, result, "densityDPI"));
    NAPI_CALL(env, SetMemberUndefined(env, result, "densityPixels"));
    NAPI_CALL(env, SetMemberUndefined(env, result, "scaledDensity"));
    NAPI_CALL(env, SetMemberUndefined(env, result, "xDPI"));
    NAPI_CALL(env, SetMemberUndefined(env, result, "yDPI"));
    return result;
}

napi_value MainFunc(napi_env env, napi_callback_info info)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);
    auto param = std::make_unique<Param>();
    return CreatePromise<Param>(env, __PRETTY_FUNCTION__, Async, Resolve, param);
}
} // namespace getDefaultDisplay }}}

napi_value DisplayModuleInit(napi_env env, napi_value exports)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("getDefaultDisplay", getDefaultDisplay::MainFunc),
    };

    NAPI_CALL(env, napi_define_properties(env,
        exports, sizeof(properties) / sizeof(properties[0]), properties));
    return exports;
}
} // namespace OHOS

extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module displayModule = {
        .nm_version = 1, // NAPI v1
        .nm_flags = 0, // normal
        .nm_filename = nullptr,
        .nm_register_func = OHOS::DisplayModuleInit,
        .nm_modname = "display",
        .nm_priv = nullptr,
    };
    napi_module_register(&displayModule);
}
