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

#include "ui_effect_napi_utils.h"
#ifdef ENABLE_IPC_SECURITY
#include "ipc_skeleton.h"
#include "accesstoken_kit.h"
#endif

namespace OHOS {
namespace Rosen {

napi_valuetype UIEffectNapiUtils::GetType(napi_env env, napi_value root)
{
    napi_valuetype res = napi_undefined;
    napi_typeof(env, root, &res);
    return res;
}

bool UIEffectNapiUtils::IsSystemApp()
{
#ifdef ENABLE_IPC_SECURITY
    uint64_t tokenId = OHOS::IPCSkeleton::GetCallingFullTokenID();
    return Security::AccessToken::AccessTokenKit::IsSystemAppByFullTokenID(tokenId);
#else
    return true;
#endif
}

} // namespace Rosen
} // namespace OHOS
