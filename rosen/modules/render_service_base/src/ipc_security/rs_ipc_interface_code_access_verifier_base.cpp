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
bool RSInterfaceCodeAccessVerifierBase::IsInterfaceCodeAccessible(CodeUnderlyingType code, const std::string& caller)
{
#ifdef ENABLE_IPC_SECURITY
    if (!IsCommonVerificationPassed(code)) {
        ROSEN_LOGE("%s: IsCommonVerificationPassed is false.", caller.c_str());
        return false;
    }
    if (!IsExtraVerificationPassed(code)) {
        ROSEN_LOGE("%s: IsExtraVerificationPassed is false.", caller.c_str());
        return false;
    }
#endif
    return true;
}

TokenIdType RSInterfaceCodeAccessVerifierBase::GetCallingFullTokenID() const
{
#ifdef ENABLE_IPC_SECURITY
    // next: check the correctness of this part
    return IPCSkeleton::GetCallingFullTokenID();
#endif
    return 0;
}

bool RSInterfaceCodeAccessVerifierBase::IsSystemApp() const
{
#ifdef ENABLE_IPC_SECURITY
    // next: check the correctness of this part
    TokenIdType tokenId = GetCallingFullTokenID();
    return Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(tokenId);
#endif
    return true;
}

bool RSInterfaceCodeAccessVerifierBase::IsCommonVerificationPassed(CodeUnderlyingType code, const std::string& caller)
{
    if (accessMap_.count(code) == 0) {
        ROSEN_LOGE("%s: IPC code is not contained in accessMap.", caller.c_str());
        return false;
    }
    const std::unordered_set<TokenIdType> *const accessSet = &(accessMap_[code]);
    TokenIdType tokenId = GetCallingFullTokenID();
    if (accessSet->count(tokenId) == 0) {
        ROSEN_LOGE("%s: tokenId is not contained in accessSet.", caller.c_str());
        return false;
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS
