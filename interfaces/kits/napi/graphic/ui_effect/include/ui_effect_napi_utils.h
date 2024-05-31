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

#include "napi/native_api.h"
#include "napi/native_node_api.h"

#define UIEFFECT_IS_OK(x) ((x) == napi_ok)
#define UIEFFECT_NOT_NULL(p) ((p) != nullptr)
#define UIEFFECT_IS_READY(x, p) (UIEFFECT_IS_OK(x) && UIEFFECT_NOT_NULL(p))

#define UIEFFECT_NAPI_CHECK_RET_D(x, res, msg) \
do \
{ \
    if (!(x)) \
    { \
        msg; \
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
class UIEffectNapiUtils {
public:
    static napi_valuetype getType(napi_env env, napi_value root)
    {
        napi_valuetype res = napi_undefined;
        napi_typeof(env, root, &res);
        return res;
    }
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_UIEFFECT_NAPI_UTILS_H