/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "ipc_callbacks/rs_ihgm_config_change_callback_ipc_interface_code_access_verifier.h"

namespace OHOS {
namespace Rosen {
RSIHgmConfigChangeCallbackInterfaceCodeAccessVerifier::RSIHgmConfigChangeCallbackInterfaceCodeAccessVerifier()
{
    CheckCodeUnderlyingTypeStandardized<CodeEnumType>(codeEnumTypeName_);
}

bool RSIHgmConfigChangeCallbackInterfaceCodeAccessVerifier::IsExclusiveVerificationPassed(CodeUnderlyingType code)
{
    bool hasPermission = true;
    switch (code) {
        case static_cast<CodeUnderlyingType>(CodeEnumType::ON_HGM_CONFIG_CHANGED): {
            /* to implement access interception */
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::ON_HGM_REFRESH_RATE_MODE_CHANGED): {
            /* to implement access interception */
            break;
        }
        default: {
            break;
        }
    }
    return hasPermission;
}
} // namespace Rosen
} // namespace OHOS
