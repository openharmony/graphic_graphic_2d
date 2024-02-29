/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "js_hdr_capability.h"

#include <memory>

#include "js_hdr_format_utils.h"

namespace OHOS {
namespace HdrCapability {
void JsHdrCapability::Finalizer(napi_env env, void* data, void* hint)
{
    std::unique_ptr<JsHdrCapability>(static_cast<JsHdrCapability*>(data));
}

napi_value JsHdrCapabilityInit(napi_env env, napi_value exportObj)
{
    if (env == nullptr || exportObj == nullptr) {
        HCLOGE("[NAPI]JsHdrCapabilityInit engine or exportObj is nullptr");
        return nullptr;
    }

    std::unique_ptr<JsHdrCapability> jsHdrCapability = std::make_unique<JsHdrCapability>();
    napi_wrap(env, exportObj, jsHdrCapability.release(), JsHdrCapability::Finalizer, nullptr, nullptr);
    auto valueHDRFormat = HDRFormatInit(env);
    napi_set_named_property(env, exportObj, "HDRFormat", valueHDRFormat);
    return exportObj;
}
}  // namespace HdrCapability
}  // namespace OHOS