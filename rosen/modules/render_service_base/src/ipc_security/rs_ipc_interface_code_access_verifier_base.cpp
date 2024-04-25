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
Security::AccessToken::ATokenTypeEnum RSInterfaceCodeAccessVerifierBase::GetTokenType() const
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
    bool securityPermissionCheckEnabled =  RSSystemProperties::GetSecurityPermissionCheckEnabled();
    if (!securityPermissionCheckEnabled) {
        return true;
    }
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

bool RSInterfaceCodeAccessVerifierBase::IsPermissionAuthenticated(CodeUnderlyingType code) const
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
            RS_LOGE("%{public}d ipc interface code access denied: permission authentication failed", code);
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
    int countSz = 0;
    for (auto& [permissionKey, permissionVal] : interfacePermissions_) {
        countSz += permissionVal.size();
    }
    return countSz;
}

bool RSInterfaceCodeAccessVerifierBase::IsSystemApp() const
{
    uint64_t fullTokenId = IPCSkeleton::GetCallingFullTokenID();
    return Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(fullTokenId);
}

bool RSInterfaceCodeAccessVerifierBase::IsSystemCalling(const std::string& callingCode) const
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
#else
bool RSInterfaceCodeAccessVerifierBase::IsSystemCalling(const std::string& /* callingCode */) const
{
    return true;
}

bool RSInterfaceCodeAccessVerifierBase::CheckPermission(CodeUnderlyingType code) const
{
    return true;
}

bool RSInterfaceCodeAccessVerifierBase::IsPermissionAuthenticated(CodeUnderlyingType code) const
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
