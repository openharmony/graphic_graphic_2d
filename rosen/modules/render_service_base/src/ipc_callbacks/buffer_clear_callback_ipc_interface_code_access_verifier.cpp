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

#include "ipc_callbacks/buffer_clear_callback_ipc_interface_code_access_verifier.h"

namespace OHOS {
namespace Rosen {
RSIBufferClearCallbackInterfaceCodeAccessVerifier::RSIBufferClearCallbackInterfaceCodeAccessVerifier()
{
#ifdef ENABLE_IPC_SECURITY
    CheckCodeUnderlyingTypeStandardized<CodeEnumType>(codeEnumTypeName_);
#endif
}

void RSIBufferClearCallbackInterfaceCodeAccessVerifier::InitializeAccessMap()
{
#ifdef ENABLE_IPC_SECURITY
    // next: specify the initialization of accessMap_ here
    accessMap_ = {};
#endif
}

bool RSIBufferClearCallbackInterfaceCodeAccessVerifier::IsExtraVerificationPassed(
    CodeUnderlyingType /* code */, const std::string& /* caller */)
{
    // Since no exclusive verification rule is temporarily required by this verifier, directly return true.
    // If any exclusive rule is required in the future, overwrite this function.
    return true;
}
} // namespace Rosen
} // namespace OHOS
