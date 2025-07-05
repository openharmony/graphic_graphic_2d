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

#include "ipc_security/rs_ipc_interface_code_access_verifier_base.h"

namespace OHOS {
namespace Rosen {

const std::unordered_map<PermissionType, std::string> PERMISSION_MAP {
    { PermissionType::CAPTURE_SCREEN, "ohos.permission.CAPTURE_SCREEN" },
    { PermissionType::UPDATE_CONFIGURATION, "ohos.permission.UPDATE_CONFIGURATION" },
    { PermissionType::GET_RUNNING_INFO, "ohos.permission.GET_RUNNING_INFO" },
};

bool RSInterfaceCodeAccessVerifierBase::IsInterfaceCodeAccessible(CodeUnderlyingType code)
{
#ifdef ENABLE_IPC_SECURITY
    if (!IsCommonVerificationPassed(code)) {
        RS_LOGE("RSInterfaceCodeAccessVerifierBase::IsInterfaceCodeAccessible common verification not passed.");
        return false;
    }
    if (!IsExclusiveVerificationPassed(code)) {
        RS_LOGE("RSInterfaceCodeAccessVerifierBase::IsInterfaceCodeAccessible exclusive verification not passed.");
        return false;
    }
#endif
    return true;
}
#ifdef ENABLE_IPC_SECURITY
Security::AccessToken::ATokenTypeEnum RSInterfaceCodeAccessVerifierBase::GetTokenType()
{
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    return Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId);
}

Security::AccessToken::AccessTokenID RSInterfaceCodeAccessVerifierBase::GetTokenID() const
{
    return IPCSkeleton::GetCallingTokenID();
}

bool RSInterfaceCodeAccessVerifierBase::CheckNativePermission(
    const Security::AccessToken::AccessTokenID tokenID, const std::string& permission) const
{
    int result = Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenID, permission, false);
    if (result != Security::AccessToken::PERMISSION_GRANTED) {
        return false;
    }
    return true;
}

bool RSInterfaceCodeAccessVerifierBase::CheckHapPermission(
    const Security::AccessToken::AccessTokenID tokenID, const std::string& permission) const
{
    int result = Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenID, permission, false);
    if (result != Security::AccessToken::PERMISSION_GRANTED) {
        return false;
    }
    return true;
}

bool RSInterfaceCodeAccessVerifierBase::CheckPermission(CodeUnderlyingType code) const
{
    std::vector<std::string> permissions = GetPermissions(code);
    bool hasPermission = true;
    auto tokenType = GetTokenType();
    auto tokenID = GetTokenID();
    for (auto& permission : permissions) {
        switch (tokenType) {
            case Security::AccessToken::ATokenTypeEnum::TOKEN_HAP:
                hasPermission = CheckHapPermission(tokenID, permission);
                break;
            case Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE:
                hasPermission = CheckNativePermission(tokenID, permission);
                break;
            case Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL:
                hasPermission = CheckNativePermission(tokenID, permission);
                break;
            default:
                break;
        }
        if (!hasPermission) {
            RS_LOGD("%{public}d ipc interface code access denied: HAS NO PERMISSION", code);
            return false;
        }
    }
    return true;
}

std::string RSInterfaceCodeAccessVerifierBase::PermissionEnumToString(PermissionType permission) const
{
    if (PERMISSION_MAP.count(permission) > 0) {
        return PERMISSION_MAP.at(permission);
    } else {
        return "unknown";
    }
}

bool RSInterfaceCodeAccessVerifierBase::AddPermission(
    CodeUnderlyingType interfaceName, const std::string& newPermission)
{
    if (interfacePermissions_.count(interfaceName) > 0) {
        auto& permissions = interfacePermissions_[interfaceName];
        auto iter = std::find_if(permissions.cbegin(), permissions.cend(),
            [newPermission](const auto& permission) { return newPermission == permission; });
        if (iter == permissions.cend()) {
            permissions.push_back(newPermission);
            return true;
        } else {
            return false;
        }
    } else {
        interfacePermissions_[interfaceName].push_back(newPermission);
    }
    return true;
}

std::vector<std::string> RSInterfaceCodeAccessVerifierBase::GetPermissions(CodeUnderlyingType interfaceName) const
{
    if (interfacePermissions_.count(interfaceName) == 0) {
        return {};
    } else {
        return interfacePermissions_.at(interfaceName);
    }
}

int RSInterfaceCodeAccessVerifierBase::GetInterfacePermissionSize() const
{
    uint32_t countSz = 0;
    for (auto& [permissionKey, permissionVal] : interfacePermissions_) {
        countSz += permissionVal.size();
    }
    return countSz;
}

bool RSInterfaceCodeAccessVerifierBase::IsSystemApp()
{
    uint64_t fullTokenId = IPCSkeleton::GetCallingFullTokenID();
    return Security::AccessToken::AccessTokenKit::IsSystemAppByFullTokenID(fullTokenId);
}

bool RSInterfaceCodeAccessVerifierBase::IsSystemCalling(const std::string& callingCode)
{
    bool isSystemCalling = false;
    auto tokenType = GetTokenType();
    if (tokenType == Security::AccessToken::ATokenTypeEnum::TOKEN_HAP) {
        isSystemCalling = IsSystemApp();
    } else if (tokenType == Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE) {
        isSystemCalling = true;
    } else if (tokenType == Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL) {
        isSystemCalling = true;
    }
    if (!isSystemCalling) {
        RS_LOGE("%{public}s ipc interface code access denied: not system calling", callingCode.c_str());
    }
    return isSystemCalling;
}

bool RSInterfaceCodeAccessVerifierBase::IsAncoCalling(const std::string& callingCode) const
{
    static constexpr uint32_t ANCO_UID = 5557;
    bool isAncoCalling = (OHOS::IPCSkeleton::GetCallingUid() == ANCO_UID);
    if (!isAncoCalling) {
        RS_LOGE("%{public}s ipc interface code access denied: not anco calling", callingCode.c_str());
    }
    return isAncoCalling;
}

bool RSInterfaceCodeAccessVerifierBase::IsFoundationCalling(const std::string& callingCode) const
{
    static constexpr uint32_t FOUNDATION_UID = 5523;
    bool isFoundationCalling = (OHOS::IPCSkeleton::GetCallingUid() == FOUNDATION_UID);
    if (!isFoundationCalling) {
        RS_LOGE("%{public}s ipc interface code access denied: not foundation calling", callingCode.c_str());
    }
    return isFoundationCalling;
}

void RSInterfaceCodeAccessVerifierBase::GetAccessType(bool& isTokenTypeValid, bool& isNonSystemAppCalling)
{
    switch (GetTokenType()) {
        case (Security::AccessToken::ATokenTypeEnum::TOKEN_HAP): {
            isTokenTypeValid = true;
            isNonSystemAppCalling = !IsSystemApp();
            break;
        }
        case (Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE): {
            isTokenTypeValid = true;
            isNonSystemAppCalling = false;
            break;
        }
        case (Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL): {
            isTokenTypeValid = true;
            isNonSystemAppCalling = false;
            break;
        }
        default: { // TOKEN_INVALID and TOKEN_TYPE_BUTT
            isTokenTypeValid = false;
            isNonSystemAppCalling = false;
            break;
        }
    }
}

bool RSInterfaceCodeAccessVerifierBase::IsStylusServiceCalling(const std::string& callingCode) const
{
    // Stylus service calls only
    static constexpr uint32_t STYLUS_SERVICE_UID = 7555;
    static const std::string STYLUS_SERVICE_PROCESS_NAME = "stylus_service";
    Security::AccessToken::NativeTokenInfo tokenInfo;
    int32_t ret = Security::AccessToken::AccessTokenKit::GetNativeTokenInfo(GetTokenID(), tokenInfo);
    if (ret == ERR_OK) {
        bool isStylusServiceProcessName = (tokenInfo.processName == STYLUS_SERVICE_PROCESS_NAME);
        bool isNativeCalling = (GetTokenType() == Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE);
        bool isStylusServiceUid = (OHOS::IPCSkeleton::GetCallingUid() == STYLUS_SERVICE_UID);
        bool isStylusServiceCalling = isNativeCalling && isStylusServiceUid && isStylusServiceProcessName;
        if (!isStylusServiceCalling) {
            RS_LOGE("%{public}s ipc interface code access denied: not stylus service calling", callingCode.c_str());
        }
        return isStylusServiceCalling;
    }
    RS_LOGE("%{public}s ipc interface code access denied: GetNativeTokenInfo error", callingCode.c_str());
    return false;
}

bool RSInterfaceCodeAccessVerifierBase::IsTaskManagerCalling(const std::string& callingCode) const
{
    static constexpr uint32_t TASK_MANAGER_SERVICE_UID = 7005;
    bool isTaskManagerCalling = (OHOS::IPCSkeleton::GetCallingUid() == TASK_MANAGER_SERVICE_UID);
    if (!isTaskManagerCalling) {
        RS_LOGE("%{public}s ipc interface code access denied: not taskmanager calling", callingCode.c_str());
    }
    return isTaskManagerCalling;
}
#else
bool RSInterfaceCodeAccessVerifierBase::IsSystemCalling(const std::string& /* callingCode */)
{
    return true;
}

bool RSInterfaceCodeAccessVerifierBase::CheckPermission(CodeUnderlyingType code) const
{
    return true;
}

bool RSInterfaceCodeAccessVerifierBase::IsAncoCalling(const std::string& callingCode) const
{
    return true;
}

void RSInterfaceCodeAccessVerifierBase::GetAccessType(bool& isTokenTypeValid, bool& isNonSystemAppCalling)
{
    isTokenTypeValid = true;
    isNonSystemAppCalling = false;
}

bool RSInterfaceCodeAccessVerifierBase::IsStylusServiceCalling(const std::string& callingCode) const
{
    return true;
}

bool RSInterfaceCodeAccessVerifierBase::IsTaskManagerCalling(const std::string& callingCode) const
{
    return true;
}
#endif

bool RSInterfaceCodeAccessVerifierBase::IsCommonVerificationPassed(CodeUnderlyingType /* code */)
{
    // Since no common verification rule is temporarily required, directly return true.
    // If any common rule is required in the future, overwrite this function.
    return true;
}

bool RSInterfaceCodeAccessVerifierBase::IsAccessTimesVerificationPassed(CodeUnderlyingType code, uint32_t times) const
{
    return true;
}

} // namespace Rosen
} // namespace OHOS
