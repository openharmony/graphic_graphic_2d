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

#include "window.h"

#include <string>

#include <ability.h>
#include <hilog/log.h>
#include <window_manager.h>

using namespace OHOS;

namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "WindowNAPILayer" };
#define GNAPI_LOG(fmt, ...) HiviewDFX::HiLog::Info(LABEL, \
    "%{public}s:%{public}d " fmt, __func__, __LINE__, ##__VA_ARGS__)
}

#define GNAPI_CALL(env, call)                     \
    do {                                          \
        napi_status s = (call);                   \
        if (s != napi_ok) {                       \
            GNAPI_LOG(#call " is %{public}d", s); \
            return nullptr;                       \
        }                                         \
    } while (0)

#define GNAPI_ASSERT(env, assertion, fmt, ...)  \
    do {                                        \
        if (assertion) {                        \
            GNAPI_LOG(fmt, ##__VA_ARGS__);      \
            return nullptr;                     \
        }                                       \
    } while (0)

#define GNAPI_INNER(call)                         \
    do {                                          \
        napi_status s = (call);                   \
        if (s != napi_ok) {                       \
            GNAPI_LOG(#call " is %{public}d", s); \
            return s;                             \
        }                                         \
    } while (0)

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

template<typename ParamT>
napi_value CreatePromise(napi_env env,
                         std::string funcname,
                         void(* async)(napi_env env, std::unique_ptr<ParamT>& param),
                         napi_value(* resolve)(napi_env env, std::unique_ptr<ParamT>& param),
                         std::unique_ptr<ParamT>& param)
{
    struct AsyncCallbackInfo {
        napi_async_work asyncWork;
        napi_deferred deferred;
        void (* async)(napi_env env, std::unique_ptr<ParamT>& param);
        napi_value (* resolve)(napi_env env, std::unique_ptr<ParamT>& param);
        std::unique_ptr<ParamT> param;
    };

    AsyncCallbackInfo *info = new AsyncCallbackInfo {
        .async = async,
        .resolve = resolve,
        .param = std::move(param),
    };

    napi_value resourceName;
    GNAPI_CALL(env, napi_create_string_latin1(env,
        funcname.c_str(), NAPI_AUTO_LENGTH, &resourceName));

    napi_value promise;
    GNAPI_CALL(env, napi_create_promise(env, &info->deferred, &promise));

    auto asyncFunc = [](napi_env env, void *data) {
        AsyncCallbackInfo *info = reinterpret_cast<AsyncCallbackInfo *>(data);
        if (info->async) {
            info->async(env, info->param);
        }
    };

    auto completeFunc = [](napi_env env, napi_status status, void *data) {
        AsyncCallbackInfo *info = reinterpret_cast<AsyncCallbackInfo *>(data);
        napi_value resolveValue;
        if (info->resolve) {
            resolveValue = info->resolve(env, info->param);
        } else {
            napi_get_undefined(env, &resolveValue);
        }

        napi_resolve_deferred(env, info->deferred, resolveValue);
        napi_delete_async_work(env, info->asyncWork);
        delete info;
    };

    GNAPI_CALL(env, napi_create_async_work(env, nullptr, resourceName, asyncFunc, completeFunc,
        reinterpret_cast<void *>(info), &info->asyncWork));

    GNAPI_CALL(env, napi_queue_async_work(env, info->asyncWork));
    return promise;
}
} // namespace

namespace NAPIWindow {
napi_value WindowConstructor(napi_env env, napi_callback_info info)
{
    napi_value jsthis = nullptr;
    GNAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr));
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
    param->ability->GetWindow()->ReSize(param->width, param->height);
}

napi_value MainFunc(napi_env env, napi_callback_info info)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);
    constexpr int argumentSize = 2;
    size_t argc = argumentSize;
    napi_value argv[argc];

    GNAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    GNAPI_ASSERT(env, argc < argumentSize, "ResetSize need %{public}d arguments", argumentSize);

    auto param = std::make_unique<Param>();
    GNAPI_CALL(env, GetAbility(env, info, param->ability));
    GNAPI_CALL(env, napi_get_value_int32(env, argv[0], &param->width));
    GNAPI_CALL(env, napi_get_value_int32(env, argv[1], &param->height));

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

    GNAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    GNAPI_ASSERT(env, argc < argumentSize, "MoveTo need %{public}d arguments", argumentSize);

    auto param = std::make_unique<Param>();
    GNAPI_CALL(env, GetAbility(env, info, param->ability));
    GNAPI_CALL(env, napi_get_value_int32(env, argv[0], &param->x));
    GNAPI_CALL(env, napi_get_value_int32(env, argv[1], &param->y));

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
    param->ability->GetWindow()->ChangeWindowType(static_cast<WindowType>(param->windowType));
}

napi_value MainFunc(napi_env env, napi_callback_info info)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);
    constexpr int argumentSize = 1;
    size_t argc = argumentSize;
    napi_value argv[argc];

    GNAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    GNAPI_ASSERT(env, argc < argumentSize, "SetWindowType need %{public}d arguments", argumentSize);

    auto param = std::make_unique<Param>();
    GNAPI_CALL(env, GetAbility(env, info, param->ability));
    GNAPI_CALL(env, napi_get_value_int32(env, argv[0], &param->windowType));

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
    GNAPI_CALL(env, napi_new_instance(env, g_classWindow, 0, nullptr, &ret));
    return ret;
}

napi_value MainFunc(napi_env env, napi_callback_info info)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);
    auto param = std::make_unique<Param>();
    return CreatePromise<Param>(env, __PRETTY_FUNCTION__, nullptr, Resolve, param);
}
} // namespace getTopWindow }}}

napi_status WindowInit(napi_env env, napi_value exports)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("resetSize", NAPIWindow::ResetSize::MainFunc),
        DECLARE_NAPI_FUNCTION("moveTo", NAPIWindow::MoveTo::MainFunc),
        DECLARE_NAPI_FUNCTION("setWindowType", NAPIWindow::SetWindowType::MainFunc),
    };

    GNAPI_INNER(napi_define_class(env, "Window", NAPI_AUTO_LENGTH, NAPIWindow::WindowConstructor,
        nullptr, sizeof(desc) / sizeof(*desc), desc, &g_classWindow));

    napi_property_descriptor exportFuncs[] = {
        DECLARE_NAPI_FUNCTION("getTopWindow", getTopWindow::MainFunc),
        DECLARE_NAPI_PROPERTY("Window", g_classWindow),
    };

    GNAPI_INNER(napi_define_properties(env, exports,
        sizeof(exportFuncs) / sizeof(*exportFuncs), exportFuncs));

    return napi_ok;
}
