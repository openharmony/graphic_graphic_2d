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
    return Security::AccessToken::AccessTokenKit::GetTokenType(tokenId);
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
#endif

bool RSInterfaceCodeAccessVerifierBase::IsCommonVerificationPassed(CodeUnderlyingType /* code */)
{
    // Since no common verification rule is temporarily required, directly return true.
    // If any common rule is required in the future, overwrite this function.
    return true;
}
} // namespace Rosen
} // namespace OHOS
