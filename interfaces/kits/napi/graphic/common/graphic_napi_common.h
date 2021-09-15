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

#ifndef INTERFACES_KITS_NAPI_GRAPHIC_COMMON_COMMON_H
#define INTERFACES_KITS_NAPI_GRAPHIC_COMMON_COMMON_H

#include <memory>
#include <string>

#include <hilog/log.h>
#include <napi/native_api.h>
#include <napi/native_common.h>
#include <napi/native_node_api.h>

constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "NapiGraphicCommonLayer" };

#define GNAPI_LOG(fmt, ...) OHOS::HiviewDFX::HiLog::Info(LABEL, \
    "%{public}s:%{public}d " fmt, __func__, __LINE__, ##__VA_ARGS__)

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

namespace OHOS {
napi_status SetMemberInt32(napi_env env, napi_value result, const char *key, int32_t value);
napi_status SetMemberUint32(napi_env env, napi_value result, const char *key, uint32_t value);
napi_status SetMemberUndefined(napi_env env, napi_value result, const char *key);

template<typename ParamT>
napi_value CreatePromise(napi_env env,
                         std::string funcname,
                         void(*async)(napi_env env, std::unique_ptr<ParamT>& param),
                         napi_value(*resolve)(napi_env env, std::unique_ptr<ParamT>& param),
                         std::unique_ptr<ParamT>& param)
{
    struct AsyncCallbackInfo {
        napi_async_work asyncWork;
        napi_deferred deferred;
        void (*async)(napi_env env, std::unique_ptr<ParamT>& param);
        napi_value (*resolve)(napi_env env, std::unique_ptr<ParamT>& param);
        std::unique_ptr<ParamT> param;
    };

    AsyncCallbackInfo *info = new AsyncCallbackInfo {
        .async = async,
        .resolve = resolve,
        .param = std::move(param),
    };

    napi_value resourceName;
    NAPI_CALL(env, napi_create_string_latin1(env,
        funcname.c_str(), NAPI_AUTO_LENGTH, &resourceName));

    napi_value promise;
    NAPI_CALL(env, napi_create_promise(env, &info->deferred, &promise));

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

    NAPI_CALL(env, napi_create_async_work(env, nullptr, resourceName, asyncFunc, completeFunc,
        reinterpret_cast<void *>(info), &info->asyncWork));

    NAPI_CALL(env, napi_queue_async_work(env, info->asyncWork));
    return promise;
};
} // namespace OHOS

#endif // INTERFACES_KITS_NAPI_GRAPHIC_COMMON_COMMON_H
