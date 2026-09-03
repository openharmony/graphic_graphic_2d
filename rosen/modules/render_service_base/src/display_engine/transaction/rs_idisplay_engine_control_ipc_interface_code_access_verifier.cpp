/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
 
#include "display_engine/transaction/rs_idisplay_engine_control_ipc_interface_code_access_verifier.h"
 
namespace OHOS {
namespace Rosen {
RSIDisplayEngineControlInterfaceCodeAccessVerifier::RSIDisplayEngineControlInterfaceCodeAccessVerifier()
{
    CheckCodeUnderlyingTypeStandardized<CodeEnumType>(codeEnumTypeName_);
#ifdef ENABLE_IPC_SECURITY
    AddRSIDisplayEngineControlInterfaceCodePermission();
#endif
}
 
bool RSIDisplayEngineControlInterfaceCodeAccessVerifier::IsExclusiveVerificationPassed(CodeUnderlyingType code)
{
    bool hasPermission = true;
    switch (code) {
        case static_cast<CodeUnderlyingType>(CodeEnumType::NOTIFY_DE_STATUS_CHANGE): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::NOTIFY_DE_STATUS_CHANGE");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::REGISTER_DE_STATUS_CHANGE_CALLBACK): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::REGISTER_DE_STATUS_CHANGE_CALLBACK");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::UNREGISTER_DE_STATUS_CHANGE_CALLBACK): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::UNREGISTER_DE_STATUS_CHANGE_CALLBACK");
            break;
        }
        default: {
            hasPermission = false;
            break;
        }
    }
    return hasPermission;
}
 
#ifdef ENABLE_IPC_SECURITY
void RSIDisplayEngineControlInterfaceCodeAccessVerifier::AddRSIDisplayEngineControlInterfaceCodePermission()
{
    for (auto& mapping : permissionRSIDisplayEngineControlInterfaceMappings_) {
        CodeEnumType interfaceName = mapping.first;
        PermissionType permission = mapping.second;
        std::string newPermission = PermissionEnumToString(permission);
        if (newPermission == "unknown") {
            continue;
        }
        CodeUnderlyingType code = static_cast<CodeUnderlyingType>(interfaceName);
        AddPermission(code, newPermission);
    }
}
 
bool RSIDisplayEngineControlInterfaceCodeAccessVerifier::IsAccessTimesVerificationPassed(
    CodeUnderlyingType code, uint32_t times) const
{
    auto interfaceName = static_cast<CodeEnumType>(code);
    if (accessRSIDisplayEngineControlInterfaceTimesRestrictions_.count(interfaceName) == 0) {
        return true;
    }
    uint32_t restrictedTimes = accessRSIDisplayEngineControlInterfaceTimesRestrictions_.at(interfaceName);
    return times < restrictedTimes;
}
#endif
 
} // namespace Rosen
} // namespace OHOS