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

#ifndef ROSEN_RENDER_SERVICE_BASE_RS_IPC_INTERFACE_CODE_ACCESS_VERIFIER_BASE_H
#define ROSEN_RENDER_SERVICE_BASE_RS_IPC_INTERFACE_CODE_ACCESS_VERIFIER_BASE_H

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>

#ifdef ENABLE_IPC_SECURITY
#include "accesstoken_kit.h"
#include "access_token.h"
#include "ipc_skeleton.h"
#include "tokenid_kit.h"
#endif

#include "common/rs_macros.h"
#include "ipc_security/rs_ipc_interface_code_underlying_type.h"
#include "ipc_security/rs_ipc_interface_permission_type.h"

#include "nocopyable.h"
#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSInterfaceCodeAccessVerifierBase {
public:
    virtual ~RSInterfaceCodeAccessVerifierBase() noexcept = default;

    bool IsInterfaceCodeAccessible(CodeUnderlyingType code);
    virtual bool IsAccessTimesVerificationPassed(CodeUnderlyingType code, uint32_t times) const;

protected:
    /* this class cannot be instantiated */
    RSInterfaceCodeAccessVerifierBase() = default;

    /* specify the exclusive verification rules in the derived class */
    virtual bool IsExclusiveVerificationPassed(CodeUnderlyingType code) = 0;

    /* specify tools for verifying the access right */
#ifdef ENABLE_IPC_SECURITY
    Security::AccessToken::ATokenTypeEnum GetTokenType() const;
    Security::AccessToken::AccessTokenID GetTokenID() const;
    bool CheckNativePermission(const Security::AccessToken::AccessTokenID tokenID, const std::string& permission) const;
    bool CheckHapPermission(const Security::AccessToken::AccessTokenID tokenID, const std::string& permission) const;
    std::string PermissionEnumToString(PermissionType permission) const;
    bool AddPermission(CodeUnderlyingType interfaceName, const std::string& newPermission);
    std::vector<std::string> GetPermissions(CodeUnderlyingType interfaceName) const;
    int GetInterfacePermissionSize() const;

    bool IsSystemApp() const;
#endif
    bool IsSystemCalling(const std::string& callingCode) const;
    bool CheckPermission(CodeUnderlyingType code) const;
    bool IsPermissionAuthenticated(CodeUnderlyingType code) const;

private:
    DISALLOW_COPY_AND_MOVE(RSInterfaceCodeAccessVerifierBase);

    /* specify the communal verification rules in the base class */
    bool IsCommonVerificationPassed(CodeUnderlyingType code);
    std::unordered_map<CodeUnderlyingType, std::vector<std::string>> interfacePermissions_;

};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_RENDER_SERVICE_BASE_RS_IPC_INTERFACE_CODE_ACCESS_VERIFIER_BASE_H
