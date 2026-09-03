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
 
#ifndef RENDER_SERVICE_BASE_DISPLAY_ENGINE_TRANSACTION_RS_IDISPLAY_ENGINE_CONTROL_IPC_INTERFACE_CODE_ACCESS_VERIFIER_H
#define RENDER_SERVICE_BASE_DISPLAY_ENGINE_TRANSACTION_RS_IDISPLAY_ENGINE_CONTROL_IPC_INTERFACE_CODE_ACCESS_VERIFIER_H
 
#include "display_engine/transaction/rs_idisplay_engine_control_ipc_interface_code.h"
#include "ipc_security/rs_ipc_interface_code_access_verifier_base.h"
 
namespace OHOS {
namespace Rosen {
class RSIDisplayEngineControlInterfaceCodeAccessVerifier : public RSInterfaceCodeAccessVerifierBase {
public:
    /*
     * specify the enum class of the associated interface code (i.e. CodeEnumType) here
     * note that term **CodeEnumType** should not be changed
     */
    using CodeEnumType = RSIDisplayEngineControlInterfaceCode;
    static inline const std::string codeEnumTypeName_ {"RSIDisplayEngineControlInterfaceCode"};
 
    /* specify constructor and destructor here */
    RSIDisplayEngineControlInterfaceCodeAccessVerifier();
    ~RSIDisplayEngineControlInterfaceCodeAccessVerifier() noexcept override = default;
#ifdef ENABLE_IPC_SECURITY
    bool IsAccessTimesVerificationPassed(CodeUnderlyingType code, uint32_t times) const override;
#endif
 
protected:
    /* specify exclusive verification rules here */
    bool IsExclusiveVerificationPassed(CodeUnderlyingType code) override;
 
private:
    DISALLOW_COPY_AND_MOVE(RSIDisplayEngineControlInterfaceCodeAccessVerifier);
#ifdef ENABLE_IPC_SECURITY
    void AddRSIDisplayEngineControlInterfaceCodePermission();
    static inline const std::vector<std::pair<CodeEnumType, PermissionType>>
        permissionRSIDisplayEngineControlInterfaceMappings_ {};
    static inline const std::unordered_map<CodeEnumType, uint32_t>
        accessRSIDisplayEngineControlInterfaceTimesRestrictions_ {};
#endif
};
} // namespace Rosen
} // namespace OHOS
#endif // DISPLAY_ENGINE_TRANSACTION_RS_IDISPLAY_ENGINE_CONTROL_IPC_INTERFACE_CODE_ACCESS_VERIFIER_H