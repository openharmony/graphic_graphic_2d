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
#include <window_manager_service_client.h>

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
    sptr<PromiseWMError> promise;
    WMError wret;
};

bool Async(napi_env env, std::unique_ptr<Param> &param)
{
    param->wret = param->promise->Await();
    return param->wret == WM_OK;
}

napi_value Resolve(napi_env env, std::unique_ptr<Param> &param)
{
    if (param->wret != WM_OK) {
        return CreateError(env, "failed with %s", WMErrorStr(param->wret).c_str());
    }
    return nullptr;
}

napi_value MainFunc(napi_env env, napi_callback_info info)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);
    constexpr int argumentSize = 2;
    size_t argc = argumentSize;
    napi_value argv[argc];

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    GNAPI_ASSERT(env, argc >= argumentSize, "ResetSize need %d arguments", argumentSize);

    ::OHOS::AppExecFwk::Ability *ability;
    int width;
    int height;
    NAPI_CALL(env, GetAbility(env, info, ability));
    NAPI_CALL(env, napi_get_value_int32(env, argv[0], &width));
    NAPI_CALL(env, napi_get_value_int32(env, argv[1], &height));

    auto param = std::make_unique<Param>();
    param->promise = ability->GetWindow()->Resize(width, height);

    return CreatePromise<Param>(env, __PRETTY_FUNCTION__, Async, Resolve, param);
}
} // namespace NAPIWindow.ResetSize }}}

// Window.MoveTo {{{
namespace MoveTo {
struct Param {
    sptr<PromiseWMError> promise;
    WMError wret;
};

bool Async(napi_env env, std::unique_ptr<Param> &param)
{
    param->wret = param->promise->Await();
    return param->wret == WM_OK;
}

napi_value Resolve(napi_env env, std::unique_ptr<Param> &param)
{
    if (param->wret != WM_OK) {
        return CreateError(env, "failed with %s", WMErrorStr(param->wret).c_str());
    }
    return nullptr;
}

napi_value MainFunc(napi_env env, napi_callback_info info)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);
    constexpr int argumentSize = 2;
    size_t argc = argumentSize;
    napi_value argv[argc];

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    GNAPI_ASSERT(env, argc >= argumentSize, "MoveTo need %d arguments", argumentSize);

    ::OHOS::AppExecFwk::Ability *ability;
    int x;
    int y;
    NAPI_CALL(env, GetAbility(env, info, ability));
    NAPI_CALL(env, napi_get_value_int32(env, argv[0], &x));
    NAPI_CALL(env, napi_get_value_int32(env, argv[1], &y));

    auto param = std::make_unique<Param>();
    param->promise = ability->GetWindow()->Move(x, y);

    return CreatePromise<Param>(env, __PRETTY_FUNCTION__, Async, Resolve, param);
}
} // namespace NAPIWindow.MoveTo }}}

// Window.SetWindowType {{{
namespace SetWindowType {
struct Param {
    sptr<PromiseWMError> promise;
    WMError wret;
};

bool Async(napi_env env, std::unique_ptr<Param> &param)
{
    param->wret = param->promise->Await();
    return param->wret == WM_OK;
}

napi_value Resolve(napi_env env, std::unique_ptr<Param> &param)
{
    if (param->wret != WM_OK) {
        return CreateError(env, "failed with %s", WMErrorStr(param->wret).c_str());
    }
    return nullptr;
}

napi_value MainFunc(napi_env env, napi_callback_info info)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);
    constexpr int argumentSize = 1;
    size_t argc = argumentSize;
    napi_value argv[argc];

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    GNAPI_ASSERT(env, argc >= argumentSize, "SetWindowType need %d arguments", argumentSize);

    ::OHOS::AppExecFwk::Ability *ability;
    int windowType;
    NAPI_CALL(env, GetAbility(env, info, ability));
    NAPI_CALL(env, napi_get_value_int32(env, argv[0], &windowType));

    auto param = std::make_unique<Param>();
    param->promise = ability->GetWindow()->SetWindowType(static_cast<WindowType>(windowType));

    return CreatePromise<Param>(env, __PRETTY_FUNCTION__, Async, Resolve, param);
}
} // namespace NAPIWindow.SetWindowType }}}
} // namespace NAPIWindow

// getTopWindow {{{
namespace GetTopWindow {
struct Param {
    sptr<PromiseWMError> promise;
    WMError wret;
};

napi_value Resolve(napi_env env, std::unique_ptr<Param> &userdata)
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

// setSystemBarEnable {{{
namespace SetSystemBarEnable {
struct Param {
    sptr<IWindowManagerService> wms;
    sptr<PromiseWMError> statusPromise;
    WMError statusWret;
    sptr<PromiseWMError> navigationPromise;
    WMError navigationWret;
};

bool Async(napi_env env, std::unique_ptr<Param> &param)
{
    bool retval = true;
    if (param->statusPromise != nullptr) {
        param->statusWret = param->statusPromise->Await();
        retval = retval && param->statusWret == WM_OK;
    }
    if (param->navigationPromise != nullptr) {
        param->navigationWret = param->navigationPromise->Await();
        retval = retval && param->navigationWret == WM_OK;
    }
    return retval;
}

napi_value Resolve(napi_env env, std::unique_ptr<Param> &param)
{
    if (param->statusWret != WM_OK) {
        return CreateError(env, "failed with %s", WMErrorStr(param->statusWret).c_str());
    }
    if (param->navigationWret != WM_OK) {
        return CreateError(env, "failed with %s", WMErrorStr(param->navigationWret).c_str());
    }
    return nullptr;
}

napi_value MainFunc(napi_env env, napi_callback_info info)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);
    constexpr int argumentSize = 1;
    size_t argc = argumentSize;
    napi_value argv[argc];

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    GNAPI_ASSERT(env, argc >= argumentSize, "setSystemBarEnable need %d arguments", argumentSize);

    auto param = std::make_unique<Param>();
    auto wmsc = WindowManagerServiceClient::GetInstance();
    auto wret = wmsc->Init();
    GNAPI_ASSERT(env, wret == WM_OK,
                 "WindowManagerServiceClient::Init failed with %s", WMErrorStr(wret).c_str());
    param->wms = wmsc->GetService();

    bool isArray = false;
    NAPI_CALL(env, napi_is_array(env, argv[0], &isArray));
    GNAPI_ASSERT(env, isArray, "setSystemBarEnable first param need Array");

    uint32_t arrayLength = 0;
    NAPI_CALL(env, napi_get_array_length(env, argv[0], &arrayLength));
    bool haveStatus = false;
    bool haveNavigation = false;
    for (uint32_t i = 0; i < arrayLength; i++) {
        napi_value elem;
        napi_valuetype vtype;
        NAPI_CALL(env, napi_get_element(env, argv[0], i, &elem));
        NAPI_CALL(env, napi_typeof(env, elem, &vtype));
        if (vtype == napi_string) {
            char stringElem[0x10];
            size_t stringLen;
            NAPI_CALL(env, napi_get_value_string_utf8(env, elem, stringElem, sizeof(stringElem), &stringLen));
            if (strcmp(stringElem, "status") == 0) {
                haveStatus = true;
            }
            if (strcmp(stringElem, "navigation") == 0) {
                haveNavigation = true;
            }
        }
    }
    param->statusPromise = param->wms->SetStatusBarVisibility(haveStatus);
    param->navigationPromise = param->wms->SetNavigationBarVisibility(haveNavigation);
    return CreatePromise<Param>(env, __PRETTY_FUNCTION__, Async, nullptr, param);
}
} // setSystemBarEnable }}}

napi_value CreateWindowTypeEnum(napi_env env)
{
    napi_value value = nullptr;
    NAPI_CALL(env, napi_create_object(env, &value));
    SetMemberInt32(env, value, "TYPE_APP", static_cast<int32_t>(WINDOW_TYPE_NORMAL));
    SetMemberInt32(env, value, "TYPE_SYSTEM_ALERT", static_cast<int32_t>(WINDOW_TYPE_ALARM_SCREEN));
    SetMemberInt32(env, value, "TYPE_SYSTEM_VOLUME", static_cast<int32_t>(WINDOW_TYPE_VOLUME_OVERLAY));
    SetMemberInt32(env, value, "TYPE_SYSTEM_PANEL", static_cast<int32_t>(WINDOW_TYPE_NOTIFICATION_SHADE));
    return value;
}

napi_value WindowModuleInit(napi_env env, napi_value exports)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);

    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("resetSize", NAPIWindow::ResetSize::MainFunc),
        DECLARE_NAPI_FUNCTION("moveTo", NAPIWindow::MoveTo::MainFunc),
        DECLARE_NAPI_FUNCTION("setWindowType", NAPIWindow::SetWindowType::MainFunc),
    };

    NAPI_CALL(env, napi_define_class(env, "Window", NAPI_AUTO_LENGTH,
        NAPIWindow::WindowConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &g_classWindow));

    napi_property_descriptor exportFuncs[] = {
        DECLARE_NAPI_FUNCTION("getTopWindow", GetTopWindow::MainFunc),
        DECLARE_NAPI_PROPERTY("WindowType", CreateWindowTypeEnum(env)),
        DECLARE_NAPI_PROPERTY("Window", g_classWindow),
        DECLARE_NAPI_FUNCTION("setSystemBarEnable", SetSystemBarEnable::MainFunc),
    };

    NAPI_CALL(env, napi_define_properties(env,
        exports, sizeof(exportFuncs) / sizeof(*exportFuncs), exportFuncs));
    return exports;
}
} // namespace OHOS

extern "C" {
__attribute__((constructor)) static void RegisterModule(void)
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
}
