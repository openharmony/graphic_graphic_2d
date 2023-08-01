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

#ifdef ENABLE_IPC_SECURITY
#include "ipc_skeleton.h"
#include "tokenid_kit.h"
#endif

#include "common/rs_macros.h"
#include "ipc_security/rs_ipc_interface_code_underlying_type.h"

namespace OHOS {
namespace Rosen {
using TokenIdType = uint64_t;

class RSB_EXPORT RSInterfaceCodeAccessVerifierBase {
public:
    virtual ~RSInterfaceCodeAccessVerifierBase() noexcept = default;

    bool IsInterfaceCodeAccessible(CodeUnderlyingType code, const std::string& caller);

    /* specify the initialization of accessMap_ in the derived class */
    virtual void InitializeAccessMap() = 0;

protected:
    /* this class cannot be instantiated */
    RSInterfaceCodeAccessVerifierBase() = default;

    /* specify the exclusive verification rules in the derived class */
    virtual bool IsExtraVerificationPassed(CodeUnderlyingType code, const std::string& caller) = 0;

    /* specify tools for verifying the access right */
    TokenIdType GetCallingFullTokenID() const;
    bool IsSystemApp() const;

    std::unordered_map<CodeUnderlyingType, std::unordered_set<TokenIdType>> accessMap_;

private:
    RSInterfaceCodeAccessVerifierBase(const RSInterfaceCodeAccessVerifierBase&) = delete;
    RSInterfaceCodeAccessVerifierBase& operator=(const RSInterfaceCodeAccessVerifierBase&) = delete;
    RSInterfaceCodeAccessVerifierBase(RSInterfaceCodeAccessVerifierBase&&) = delete;
    RSInterfaceCodeAccessVerifierBase& operator=(RSInterfaceCodeAccessVerifierBase&&) = delete;

    /* specify the communal verification rules in the base class */
    bool IsCommonVerificationPassed(CodeUnderlyingType code, const std::string& caller);
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_RENDER_SERVICE_BASE_RS_IPC_INTERFACE_CODE_ACCESS_VERIFIER_BASE_H
