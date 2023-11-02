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
class RSInterfaceCodeSecurityManager {
public:
    RSInterfaceCodeSecurityManager(RSInterfaceCodeSecurityManager&&) noexcept = default;
    RSInterfaceCodeSecurityManager& operator=(RSInterfaceCodeSecurityManager&&) noexcept = default;
    ~RSInterfaceCodeSecurityManager() noexcept = default;

    /*
     * specify a customized access verifier to create the corresponding security manager;
     * RSInterfaceCodeAccessVerifierDerived should inherit from RSInterfaceCodeAccessVerifierBase
     */
    template<typename RSInterfaceCodeAccessVerifierDerived, typename... AccessVerifierDerivedConstructorArgs>
    static RSInterfaceCodeSecurityManager CreateInstance(AccessVerifierDerivedConstructorArgs&&... constructorArgs)
    {
        RSInterfaceCodeSecurityManager securityManager;
#ifdef ENABLE_IPC_SECURITY
        auto accessVerifier = std::make_unique<RSInterfaceCodeAccessVerifierDerived>(
            std::forward<AccessVerifierDerivedConstructorArgs>(constructorArgs)...);
        securityManager.InitializeAccessVerifier(std::move(accessVerifier));
#endif
        return securityManager;
    }

    /* decide whether the designated ipc interface code is accessible for the visitor */
    bool IsInterfaceCodeAccessible(CodeUnderlyingType code) const
    {
#ifdef ENABLE_IPC_SECURITY
        if (accessVerifier_ == nullptr) {
            RS_LOGE("RSInterfaceCodeSecurityManager::IsInterfaceCodeAccessible access verifier is nullptr.");
            return false;
        }
        if (!accessVerifier_->IsInterfaceCodeAccessible(code)) {
            RS_LOGE("RSInterfaceCodeSecurityManager::IsInterfaceCodeAccessible verification failed.");
            return false;
        }
#endif
        return true;
    }
    
    bool IsAccessTimesRestricted(CodeUnderlyingType code, uint32_t times) const
    {
#ifdef ENABLE_IPC_SECURITY
        if (accessVerifier_ == nullptr) {
            RS_LOGE("RSInterfaceCodeSecurityManager::IsAccessTimesRestricted access verifier is nullptr.");
            return false;
        }
        if (!accessVerifier_->IsAccessTimesVerificationPassed(code, times)) {
            RS_LOGE("RSInterfaceCodeSecurityManager::IsAccessTimesRestricted verification failed.");
            return false;
        }
#endif
        return true;
    }

private:
    RSInterfaceCodeSecurityManager() = default;
    DISALLOW_COPY(RSInterfaceCodeSecurityManager);

    /* specify a customized access verifier for the designated ipc interface code */
    void InitializeAccessVerifier(std::unique_ptr<RSInterfaceCodeAccessVerifierBase> accessVerifier)
    {
#ifdef ENABLE_IPC_SECURITY
        if (accessVerifier == nullptr) {
            RS_LOGE("RSInterfaceCodeSecurityManager::InitializeAccessVerifier access verifier is nullptr.");
            return;
        }
        accessVerifier_.reset();
        accessVerifier_ = std::move(accessVerifier);
#endif
    }

    std::unique_ptr<RSInterfaceCodeAccessVerifierBase> accessVerifier_{nullptr};
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_RENDER_SERVICE_BASE_RS_IPC_INTERFACE_CODE_SECURITY_MANAGER_H
