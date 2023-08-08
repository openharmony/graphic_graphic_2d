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

#ifndef ROSEN_RENDER_SERVICE_BASE_RS_IPC_INTERFACE_CODE_SECURITY_MANAGER_H
#define ROSEN_RENDER_SERVICE_BASE_RS_IPC_INTERFACE_CODE_SECURITY_MANAGER_H

#include "ipc_security/rs_ipc_interface_code_access_verifier_base.h"

namespace OHOS {
namespace Rosen {
template<typename CodeEnumType>
class RSInterfaceCodeSecurityManager {
public:
    RSInterfaceCodeSecurityManager() = default;
    RSInterfaceCodeSecurityManager(RSInterfaceCodeSecurityManager&&) noexcept = default;
    RSInterfaceCodeSecurityManager& operator=(RSInterfaceCodeSecurityManager&&) noexcept = default;
    ~RSInterfaceCodeSecurityManager() noexcept = default;

    /* specify the customized access verifier for the designated interface code */
    void InitializeAccessVerifier(std::unique_ptr<RSInterfaceCodeAccessVerifierBase> accessVerifier,
        const std::string& caller = "CreateRSInterfaceCodeSecurityManager")
    {
#ifdef ENABLE_IPC_SECURITY
        if (accessVerifier == nullptr) {
            ROSEN_LOGE("%s: Initialization failed. Access verifier is nullptr.", caller.c_str());
            return;
        }
        accessVerifier_.reset();
        accessVerifier_ = std::move(accessVerifier);
        accessVerifier_->InitializeAccessMap();
#endif
    }

    /* decide whether the designated interface code is accessible for the visitor */
    bool IsInterfaceCodeAccessible(CodeEnumType enumCode, const std::string& caller = "RSProxy") const
    {
#ifdef ENABLE_IPC_SECURITY
        if (accessVerifier_ == nullptr) {
            ROSEN_LOGE("%s: Verification failed. Access verifier is not initialized.", caller.c_str());
            return false;
        }
        CodeUnderlyingType code = CastEnumToUnderlying<CodeEnumType>(enumCode);
        if (!accessVerifier_->IsInterfaceCodeAccessible(code, caller)) {
            ROSEN_LOGE("%s: Verification failed. IPC Interface code is inaccessible.", caller.c_str());
            return false;
        }
#endif
        return true;
    }

private:
    RSInterfaceCodeSecurityManager(const RSInterfaceCodeSecurityManager&) = delete;
    RSInterfaceCodeSecurityManager& operator=(const RSInterfaceCodeSecurityManager&) = delete;

    std::unique_ptr<RSInterfaceCodeAccessVerifierBase> accessVerifier_{nullptr};
};

/*
 * RSInterfaceCodeAccessVerifierDerived should inherit from RSInterfaceCodeAccessVerifierBase.
 * RSInterfaceCodeAccessVerifierDerived::CodeEnumType refers to the enum class of its associated interface code.
 */
template<typename RSInterfaceCodeAccessVerifierDerived, typename... AccessVerifierDerivedConstructorArgs>
inline RSInterfaceCodeSecurityManager<typename RSInterfaceCodeAccessVerifierDerived::CodeEnumType> \
    CreateRSInterfaceCodeSecurityManager(AccessVerifierDerivedConstructorArgs&&... constructorArgs)
{
    RSInterfaceCodeSecurityManager<typename RSInterfaceCodeAccessVerifierDerived::CodeEnumType> securityManager;
#ifdef ENABLE_IPC_SECURITY
    auto accessVerifier = std::make_unique<RSInterfaceCodeAccessVerifierDerived>(
        std::forward<AccessVerifierDerivedConstructorArgs>(constructorArgs)...);
    securityManager.InitializeAccessVerifier(std::move(accessVerifier), __func__);
#endif
    return securityManager;
}
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_RENDER_SERVICE_BASE_RS_IPC_INTERFACE_CODE_SECURITY_MANAGER_H
