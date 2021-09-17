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

#include "native_window_module.h"

#include <ability.h>

#include "graphic_napi_common.h"

namespace OHOS {
namespace {
napi_value g_classWindow;
napi_status GetAbility(napi_env env, napi_callback_info info, AppExecFwk::Ability* &pAbility)
{
    napi_value global;
    GNAPI_INNER(napi_get_global(env, &global));

    napi_value jsAbility;
    GNAPI_INNER(napi_get_named_property(env, global, "ability", &jsAbility));

    GNAPI_INNER(napi_get_value_external(env, jsAbility, reinterpret_cast<void **>(&pAbility)));

    return napi_ok;
}
} // namespace

namespace NAPIWindow {
napi_value WindowConstructor(napi_env env, napi_callback_info info)
{
    napi_value jsthis = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr));
    return jsthis;
}

// Window.ResetSize {{{
namespace ResetSize {
struct Param {
    ::OHOS::AppExecFwk::Ability *ability;
    int width;
    int height;
};

void Async(napi_env env, std::unique_ptr<Param>& param)
{
    param->ability->GetWindow()->Resize(param->width, param->height);
}

napi_value MainFunc(napi_env env, napi_callback_info info)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);
    constexpr int argumentSize = 2;
    size_t argc = argumentSize;
    napi_value argv[argc];

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    GNAPI_ASSERT(env, argc < argumentSize, "ResetSize need %{public}d arguments", argumentSize);

    auto param = std::make_unique<Param>();
    NAPI_CALL(env, GetAbility(env, info, param->ability));
    NAPI_CALL(env, napi_get_value_int32(env, argv[0], &param->width));
    NAPI_CALL(env, napi_get_value_int32(env, argv[1], &param->height));

    return CreatePromise<Param>(env, __PRETTY_FUNCTION__, Async, nullptr, param);
}
} // namespace NAPIWindow.ResetSize }}}

// Window.MoveTo {{{
namespace MoveTo {
struct Param {
    ::OHOS::AppExecFwk::Ability *ability;
    int x;
    int y;
};

void Async(napi_env env, std::unique_ptr<Param>& param)
{
    param->ability->GetWindow()->Move(param->x, param->y);
}

napi_value MainFunc(napi_env env, napi_callback_info info)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);
    constexpr int argumentSize = 2;
    size_t argc = argumentSize;
    napi_value argv[argc];

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    GNAPI_ASSERT(env, argc < argumentSize, "MoveTo need %{public}d arguments", argumentSize);

    auto param = std::make_unique<Param>();
    NAPI_CALL(env, GetAbility(env, info, param->ability));
    NAPI_CALL(env, napi_get_value_int32(env, argv[0], &param->x));
    NAPI_CALL(env, napi_get_value_int32(env, argv[1], &param->y));

    return CreatePromise<Param>(env, __PRETTY_FUNCTION__, Async, nullptr, param);
}
} // namespace NAPIWindow.MoveTo }}}

// Window.SetWindowType {{{
namespace SetWindowType {
struct Param {
    ::OHOS::AppExecFwk::Ability *ability;
    int windowType;
};

void Async(napi_env env, std::unique_ptr<Param>& param)
{
    param->ability->GetWindow()->SetWindowType(static_cast<WindowType>(param->windowType));
}

void CreateWindowTypeObject(napi_env env, napi_value value)
{
    SetMemberInt32(env, value, "TYPE_APP", static_cast<int32_t>(WindowType::WINDOW_TYPE_NORMAL));
    SetMemberInt32(env, value, "TYPE_SYSTEM_ALERT", static_cast<int32_t>(WindowType::WINDOW_TYPE_ALARM_SCREEN));
    SetMemberInt32(env, value, "TYPE_SYSTEM_VOLUME", static_cast<int32_t>(WindowType::WINDOW_TYPE_VOLUME_OVERLAY));
    SetMemberInt32(env, value, "TYPE_SYSTEM_PANEL", static_cast<int32_t>(WindowType::WINDOW_TYPE_NOTIFICATION_SHADE));
}

napi_value MainFunc(napi_env env, napi_callback_info info)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);
    constexpr int argumentSize = 1;
    size_t argc = argumentSize;
    napi_value argv[argc];

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    GNAPI_ASSERT(env, argc < argumentSize, "SetWindowType need %{public}d arguments", argumentSize);

    auto param = std::make_unique<Param>();
    NAPI_CALL(env, GetAbility(env, info, param->ability));
    NAPI_CALL(env, napi_get_value_int32(env, argv[0], &param->windowType));

    return CreatePromise<Param>(env, __PRETTY_FUNCTION__, Async, nullptr, param);
}
} // namespace NAPIWindow.SetWindowType }}}
} // namespace NAPIWindow

// getTopWindow {{{
namespace getTopWindow {
struct Param {
};

napi_value Resolve(napi_env env, std::unique_ptr<Param>& userdata)
{
    napi_value ret;
    NAPI_CALL(env, napi_new_instance(env, g_classWindow, 0, nullptr, &ret));
    return ret;
}

napi_value MainFunc(napi_env env, napi_callback_info info)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);
    auto param = std::make_unique<Param>();
    return CreatePromise<Param>(env, __PRETTY_FUNCTION__, nullptr, Resolve, param);
}
} // namespace getTopWindow }}}

napi_value WindowModuleInit(napi_env env, napi_value exports)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);

    napi_value nWindowType = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nWindowType));
    NAPIWindow::SetWindowType::CreateWindowTypeObject(env, nWindowType);

    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("resetSize", NAPIWindow::ResetSize::MainFunc),
        DECLARE_NAPI_FUNCTION("moveTo", NAPIWindow::MoveTo::MainFunc),
        DECLARE_NAPI_FUNCTION("setWindowType", NAPIWindow::SetWindowType::MainFunc),
    };

    NAPI_CALL(env, napi_define_class(env, "Window", NAPI_AUTO_LENGTH,
        NAPIWindow::WindowConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &g_classWindow));

    napi_property_descriptor exportFuncs[] = {
        DECLARE_NAPI_FUNCTION("getTopWindow", getTopWindow::MainFunc),
        DECLARE_NAPI_PROPERTY("WindowType", nWindowType),
        DECLARE_NAPI_PROPERTY("Window", g_classWindow),
    };

    NAPI_CALL(env, napi_define_properties(env,
        exports, sizeof(exportFuncs) / sizeof(*exportFuncs), exportFuncs));
    return exports;
}
} // namespace OHOS

extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module windowModule = {
        .nm_version = 1, // NAPI v1
        .nm_flags = 0, // normal
        .nm_filename = nullptr,
        .nm_register_func = OHOS::WindowModuleInit,
        .nm_modname = "window",
        .nm_priv = nullptr,
    };
    napi_module_register(&windowModule);
}
