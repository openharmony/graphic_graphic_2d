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

#ifndef ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_IRENDER_SERVICE_CONNECTION_INTERFACE_CODE_ACCESS_VERIFIER_H
#define ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_IRENDER_SERVICE_CONNECTION_INTERFACE_CODE_ACCESS_VERIFIER_H

#include "ipc_security/rs_ipc_interface_code_access_verifier_base.h"
#include "platform/ohos/rs_irender_service_connection_ipc_interface_code.h"

namespace OHOS {
namespace Rosen {
class RSIRenderServiceConnectionInterfaceCodeAccessVerifier : public RSInterfaceCodeAccessVerifierBase {
public:
    /*
     * specify the enum class of the associated interface code (i.e. CodeEnumType) here
     * note that term **CodeEnumType** should not be changed
     */
    using CodeEnumType = RSIRenderServiceConnectionInterfaceCode;
    static inline const std::string codeEnumTypeName_{"RSIRenderServiceConnectionInterfaceCode"};

    /* specify constructor and destructor here */
    RSIRenderServiceConnectionInterfaceCodeAccessVerifier();
    ~RSIRenderServiceConnectionInterfaceCodeAccessVerifier() noexcept override = default;

protected:
    /* specify the initialization of accessMap_ here */
    void InitializeAccessMap() override;

    /* specify exclusive verification rules here */
    bool IsExtraVerificationPassed(CodeUnderlyingType code, const std::string& caller) override;

private:
    RSIRenderServiceConnectionInterfaceCodeAccessVerifier(
        const RSIRenderServiceConnectionInterfaceCodeAccessVerifier&) = delete;
    RSIRenderServiceConnectionInterfaceCodeAccessVerifier& operator=(
        const RSIRenderServiceConnectionInterfaceCodeAccessVerifier&) = delete;
    RSIRenderServiceConnectionInterfaceCodeAccessVerifier(
        RSIRenderServiceConnectionInterfaceCodeAccessVerifier&&) = delete;
    RSIRenderServiceConnectionInterfaceCodeAccessVerifier& operator=(
        RSIRenderServiceConnectionInterfaceCodeAccessVerifier&&) = delete;
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_IRENDER_SERVICE_CONNECTION_INTERFACE_CODE_ACCESS_VERIFIER_H
