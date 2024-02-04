/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef INTERFACES_KITS_NAPI_GRAPHIC_ANIMATION_RS_WINDOW_ANIMATION_MANAGER_H
#define INTERFACES_KITS_NAPI_GRAPHIC_ANIMATION_RS_WINDOW_ANIMATION_MANAGER_H

#include <string>

#include <native_engine/native_engine.h>

#include "rs_window_animation_controller.h"

#include "napi/native_api.h"

#include "rs_window_animation_target.h"

#include "rs_window_animation_finished_callback.h"
namespace OHOS {
namespace Rosen {
class RSWindowAnimationManager final {
public:
    RSWindowAnimationManager() = default;
    ~RSWindowAnimationManager() = default;

    static napi_value Init(napi_env env, napi_value exportObj);

    static void Finalizer(napi_env env, void* data, void* hint);

    static napi_value SetController(napi_env env, napi_callback_info info);

    static napi_value MinimizeWindowWithAnimation(napi_env env, napi_callback_info info);

    static napi_value GetWindowAnimationTargets(napi_env env, napi_callback_info info);

private:
    napi_value OnSetController(napi_env env, napi_callback_info info);

    napi_value OnMinimizeWindowWithAnimation(napi_env env, napi_callback_info info);

    napi_value OnGetWindowAnimationTargets(napi_env env, napi_callback_info info);

    int32_t GetWindowId(const napi_env& env, const size_t& argc, const napi_value& object, uint32_t& windowId);

    int32_t GetMissionIds(
        const napi_env& env, const size_t& argc, const napi_value& arrary, std::vector<uint32_t>& missionIds);
};
} // namespace Rosen
} // namespace OHOS

#endif // INTERFACES_KITS_NAPI_GRAPHIC_ANIMATION_RS_WINDOW_ANIMATION_MANAGER_H
