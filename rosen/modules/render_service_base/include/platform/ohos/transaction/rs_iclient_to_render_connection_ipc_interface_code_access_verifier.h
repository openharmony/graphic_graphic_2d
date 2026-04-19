/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RS_BASE_PLATFORM_OHOS_TRANSACTION_RS_ICLIENT_TO_RENDER_CONNECTION_IPC_INTERFACE_CODE_ACCESS_VERIFIER_H
#define RS_BASE_PLATFORM_OHOS_TRANSACTION_RS_ICLIENT_TO_RENDER_CONNECTION_IPC_INTERFACE_CODE_ACCESS_VERIFIER_H

#include "ipc_security/rs_ipc_interface_code_access_verifier_base.h"
#include "platform/ohos/transaction/rs_irender_service_ipc_interface_code.h"
#include "platform/ohos/transaction/rs_iclient_to_render_connection_ipc_interface_code.h"
namespace OHOS {
namespace Rosen {
class RSIClientToRenderConnectionInterfaceCodeAccessVerifier : public RSInterfaceCodeAccessVerifierBase {
public:
    /*
     * specify the enum class of the associated interface code (i.e. CodeEnumType) here
     * note that term **CodeEnumType** should not be changed
     */
    using CodeEnumType = RSIClientToRenderConnectionInterfaceCode;
    static inline const std::string codeEnumTypeName_ {"RSIClientToRenderConnectionInterfaceCode"};

    /* specify constructor and destructor here */
    RSIClientToRenderConnectionInterfaceCodeAccessVerifier();
    ~RSIClientToRenderConnectionInterfaceCodeAccessVerifier() noexcept override = default;
#ifdef ENABLE_IPC_SECURITY
    bool IsAccessTimesVerificationPassed(CodeUnderlyingType code, uint32_t times) const override;
#endif

protected:
    /* specify exclusive verification rules here */
    bool IsExclusiveVerificationPassed(CodeUnderlyingType code) override;

private:
    DISALLOW_COPY_AND_MOVE(RSIClientToRenderConnectionInterfaceCodeAccessVerifier);
#ifdef ENABLE_IPC_SECURITY
    void AddRSIRenderServiceConnectionInterfaceCodePermission();
    static std::vector<std::pair<CodeEnumType, PermissionType>>
        permissionRSIRenderServiceConnectionInterfaceMappings_;
    static inline const std::unordered_map<CodeEnumType, uint32_t>
        accessRSIRenderServiceConnectionInterfaceTimesRestrictions_ {
            { CodeEnumType::REGISTER_APPLICATION_AGENT, 15 },
        };
#endif
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_PLATFORM_OHOS_TRANSACTION_RS_IRENDER_SERVICE_IPC_INTERFACE_CODE_ACCESS_VERIFIER_H
