/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_ANI_TRANSFER_UTILS_H
#define OHOS_ANI_TRANSFER_UTILS_H
#include <ani.h>
#include <functional>

#include "interop_js/arkts_esvalue.h"
#include "interop_js/arkts_interop_js_api.h"

namespace OHOS::Text::ANI {
class AniTransferUtils final {
public:
    static ani_object TransferStatic(
        ani_env* env, ani_object input, std::function<ani_object(ani_env*, void*)> convert);
    static ani_object TransferDynamic(
        ani_env* aniEnv, ani_long nativeObj, std::function<napi_value(napi_env, ani_long, napi_value)> convert);
};

} // namespace OHOS::Text::ANI
#endif // OHOS_ANI_TRANSFER_UTILS_H