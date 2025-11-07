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

#include "ani_transfer_util.h"

#include "interop_js/hybridgref.h"
#include "interop_js/hybridgref_ani.h"
#include "interop_js/hybridgref_napi.h"
#include "ani_text_utils.h"
#include "utils/text_log.h"

namespace OHOS::Text::ANI {
ani_object AniTransferUtils::TransferStatic(
    ani_env* env, ani_object input, std::function<ani_object(ani_env*, void*)> convert)
{
    if (env == nullptr) {
        TEXT_LOGE("null env");
        return nullptr;
    }
    void* unwrapResult = nullptr;
    bool success = arkts_esvalue_unwrap(env, input, &unwrapResult);
    if (!success) {
        TEXT_LOGE("Failed to unwrap input");
        return AniTextUtils::CreateAniUndefined(env);
    }
    if (unwrapResult == nullptr) {
        TEXT_LOGE("Null unwrapResult");
        return AniTextUtils::CreateAniUndefined(env);
    }
    return convert(env, unwrapResult);
}

static ani_object CloseNapiEnvReturnUndefined(ani_env* aniEnv, napi_env napiEnv)
{
    arkts_napi_scope_close_n(napiEnv, 0, nullptr, nullptr);
    return AniTextUtils::CreateAniUndefined(aniEnv);
}

ani_object AniTransferUtils::TransferDynamic(
    ani_env* aniEnv, ani_long nativeObj, std::function<napi_value(napi_env, ani_long, napi_value)> convert)
{
    napi_env napiEnv = nullptr;
    if (!arkts_napi_scope_open(aniEnv, &napiEnv)) {
        TEXT_LOGE("Failed to open napi scope");
        return AniTextUtils::CreateAniUndefined(aniEnv);
    }
    napi_value objValue = nullptr;
    napi_status status = napi_create_object(napiEnv, &objValue);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to create napi object, ret %{public}d", status);
        return CloseNapiEnvReturnUndefined(aniEnv, napiEnv);
    }
    napi_value napiObj = convert(napiEnv, nativeObj, objValue);
    if (napiObj == nullptr) {
        TEXT_LOGE("Failed to convert static object to dynamic object");
        return CloseNapiEnvReturnUndefined(aniEnv, napiEnv);
    }
    hybridgref ref = nullptr;
    if (!hybridgref_create_from_napi(napiEnv, napiObj, &ref)) {
        TEXT_LOGE("Failed to create hybrid reference");
        return CloseNapiEnvReturnUndefined(aniEnv, napiEnv);
    }
    ani_object result = nullptr;
    if (!hybridgref_get_esvalue(aniEnv, ref, &result)) {
        hybridgref_delete_from_napi(napiEnv, ref);
        TEXT_LOGE("Failed to get esvalue from hybrid reference");
        return CloseNapiEnvReturnUndefined(aniEnv, napiEnv);
    }
    hybridgref_delete_from_napi(napiEnv, ref);
    if (!arkts_napi_scope_close_n(napiEnv, 0, nullptr, nullptr)) {
        TEXT_LOGE("Failed to close napi scope");
        return AniTextUtils::CreateAniUndefined(aniEnv);
    }
    return result;
}
    
} // namespace OHOS::Text::ANI