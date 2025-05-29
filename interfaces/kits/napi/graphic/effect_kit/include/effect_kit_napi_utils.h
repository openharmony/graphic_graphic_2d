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

#ifndef EFFECT_KIT_NAPI_UTILS_H
#define EFFECT_KIT_NAPI_UTILS_H

#include "napi/native_api.h"
#include "napi/native_node_api.h"

#define EFFECT_NAPI_CHECK_RET(x, res) \
do \
{ \
    if (!(x)) \
    { \
        return (res); \
    } \
} while (0)

#define EFFECT_NAPI_CHECK_RET_VOID_D(x, msg) \
do \
{ \
    if (!(x)) \
    { \
        msg; \
        return; \
    } \
} while (0)

#define EFFECT_NAPI_CHECK_RET_D(x, res, msg) \
do \
{ \
    if (!(x)) \
    { \
        msg; \
        return (res); \
    } \
} while (0)

#define EFFECT_NAPI_CHECK_RET_DELETE_POINTER(x, res, pointer, msg) \
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

#define EFFECT_NAPI_CHECK_BUILD_ERROR(x, build, res, result) \
do \
{ \
    if (!(x)) \
    { \
        build; \
        { \
            res; \
        } \
        return (result); \
    } \
} while (0)

#define EFFECT_JS_ARGS(env, info, status, argc, argv, thisVar) \
do \
{ \
    status = napi_get_cb_info(env, info, &(argc), argv, &(thisVar), nullptr); \
} while (0)

#define EFFECT_JS_NO_ARGS(env, info, status, thisVar) \
do \
{ \
    status = napi_get_cb_info(env, info, nullptr, nullptr, &(thisVar), nullptr); \
} while (0)

#define EFFECT_ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

namespace OHOS {
namespace Rosen {
class EffectKitNapiUtils {
public:
    EffectKitNapiUtils(const EffectKitNapiUtils&) = delete;
    EffectKitNapiUtils& operator=(const EffectKitNapiUtils&) = delete;

    static EffectKitNapiUtils& GetInstance()
    {
        static EffectKitNapiUtils instance;
        return instance;
    }
    napi_valuetype GetType(napi_env env, napi_value root);
    
    template<typename T>
    napi_status CreateAsyncWork(napi_env& env, napi_status& status, const char* workName,
        napi_async_execute_callback exec, napi_async_complete_callback complete, std::unique_ptr<T>& aContext,
        napi_async_work& work)
    {
        napi_value resource = nullptr;
        status = napi_create_string_utf8(env, workName, NAPI_AUTO_LENGTH, &resource);
        if (status == napi_ok) {
            status = napi_create_async_work(
                env, nullptr, resource, exec, complete, static_cast<void*>(aContext.get()), &work);
            if (status == napi_ok) {
                status = napi_queue_async_work(env, work);
            }
        }
        aContext.release();
        return status;
    };
private:
    EffectKitNapiUtils() {}
};
} // namespace Rosen
} // namespace OHOS
#endif // EFFECT_KIT_NAPI_UTILS_H
