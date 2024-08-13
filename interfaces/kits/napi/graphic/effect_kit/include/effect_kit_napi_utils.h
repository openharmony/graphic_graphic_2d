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

#define EFFECT_IS_OK(x) ((x) == napi_ok)
#define EFFECT_NOT_NULL(p) ((p) != nullptr)
#define EFFECT_IS_READY(x, p) (EFFECT_IS_OK(x) && EFFECT_NOT_NULL(p))

#define EFFECT_NAPI_CHECK_RET_D(x, res, msg) \
do \
{ \
    if (!(x)) \
    { \
        msg; \
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

#define EFFECT_NAPI_CHECK_RET(x, res) \
do \
{ \
    if (!(x)) \
    { \
        return (res); \
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

#define EFFECT_CREATE_CREATE_ASYNC_WORK(env, status, workName, exec, complete, aContext, work) \
do \
{ \
    napi_value _resource = nullptr; \
    napi_create_string_utf8((env), (workName), NAPI_AUTO_LENGTH, &_resource); \
    (status) = napi_create_async_work(env, nullptr, _resource, (exec), \
            (complete), static_cast<void*>((aContext).get()), &(work)); \
    if ((status) == napi_ok) { \
        (status) = napi_queue_async_work((env), (work)); \
        if ((status) == napi_ok) { \
            (aContext).release(); \
        } \
    } \
} while (0)

#define EFFECT_CREATE_CREATE_ASYNC_WORK_WITH_QOS(env, status, workName, exec, complete, aContext, work, qos) \
do \
{ \
    napi_value _resource = nullptr; \
    napi_create_string_utf8((env), (workName), NAPI_AUTO_LENGTH, &_resource); \
    (status) = napi_create_async_work(env, nullptr, _resource, (exec), \
            (complete), static_cast<void*>((aContext).get()), &(work)); \
    if ((status) == napi_ok) { \
        (status) = napi_queue_async_work_with_qos((env), (work), (qos)); \
        if ((status) == napi_ok) { \
            (aContext).release(); \
        } \
    } \
} while (0)

#define EFFECT_ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

namespace OHOS {
namespace Rosen {
class EffectKitNapiUtils {
public:
    static napi_valuetype getType(napi_env env, napi_value root);
};
} // namespace Rosen
} // namespace OHOS
#endif // EFFECT_KIT_NAPI_UTILS_H
