/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "js_hdr_format_utils.h"

#include <cstdint>

namespace OHOS {
namespace HdrCapability {
napi_value HDRFormatInit(napi_env env)
{
    if (env == nullptr) {
        ROSEN_LOGE("[NAPI] Engine is nullptr");
        return nullptr;
    }
    napi_value object = nullptr;
    napi_create_object(env, &object);
    if (object == nullptr) {
        ROSEN_LOGE("[NAPI] Failed to get object");
        return nullptr;
    }

    for (auto& [HDRFormatName, HDRFormat] : STRING_TO_JS_MAP) {
        napi_value value = nullptr;
        napi_create_int32(env, static_cast<int32_t>(HDRFormat), &value);
        napi_set_named_property(env, object, HDRFormatName.c_str(), value);
    }
    return object;
}
}  // namespace HdrCapability
}  // namespace OHOS
