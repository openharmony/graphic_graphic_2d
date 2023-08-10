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

#ifndef ROSEN_RENDER_SERVICE_BASE_RS_IPC_INTERFACE_CODE_SECURITY_MANAGER_REGISTRY_H
#define ROSEN_RENDER_SERVICE_BASE_RS_IPC_INTERFACE_CODE_SECURITY_MANAGER_REGISTRY_H

#include "ipc_security/rs_ipc_interface_code_security_manager.h"

#include "ipc_callbacks/buffer_available_callback_ipc_interface_code_access_verifier.h"
#include "ipc_callbacks/buffer_clear_callback_ipc_interface_code_access_verifier.h"
#include "ipc_callbacks/iapplication_agent_ipc_interface_code_access_verifier.h"
#include "ipc_callbacks/rs_iocclusion_change_callback_ipc_interface_code_access_verifier.h"
#include "ipc_callbacks/rs_isync_transaction_controller_ipc_interface_code_access_verifier.h"
#include "ipc_callbacks/screen_change_callback_ipc_interface_code_access_verifier.h"
#include "ipc_callbacks/surface_capture_callback_ipc_interface_code_access_verifier.h"
#include "platform/ohos/rs_irender_service_connection_ipc_interface_code_access_verifier.h"
#include "platform/ohos/rs_irender_service_ipc_interface_code_access_verifier.h"

namespace OHOS {
namespace Rosen {
template<typename... AccessVerifierConstructorArgs>
inline auto CreateRSIBufferAvailableCallbackInterfaceCodeSecurityManager(
    AccessVerifierConstructorArgs&&... constructorArgs)
{
    return CreateRSInterfaceCodeSecurityManager<RSIBufferAvailableCallbackInterfaceCodeAccessVerifier>(
        std::forward<AccessVerifierConstructorArgs>(constructorArgs)...);
}

template<typename... AccessVerifierConstructorArgs>
inline auto CreateRSIBufferClearCallbackInterfaceCodeSecurityManager(
    AccessVerifierConstructorArgs&&... constructorArgs)
{
    return CreateRSInterfaceCodeSecurityManager<RSIBufferClearCallbackInterfaceCodeAccessVerifier>(
        std::forward<AccessVerifierConstructorArgs>(constructorArgs)...);
}

template<typename... AccessVerifierConstructorArgs>
inline auto CreateIApplicationAgentInterfaceCodeSecurityManager(
    AccessVerifierConstructorArgs&&... constructorArgs)
{
    return CreateRSInterfaceCodeSecurityManager<IApplicationAgentInterfaceCodeAccessVerifier>(
        std::forward<AccessVerifierConstructorArgs>(constructorArgs)...);
}

template<typename... AccessVerifierConstructorArgs>
inline auto CreateRSIOcclusionChangeCallbackInterfaceCodeSecurityManager(
    AccessVerifierConstructorArgs&&... constructorArgs)
{
    return CreateRSInterfaceCodeSecurityManager<RSIOcclusionChangeCallbackInterfaceCodeAccessVerifier>(
        std::forward<AccessVerifierConstructorArgs>(constructorArgs)...);
}

template<typename... AccessVerifierConstructorArgs>
inline auto CreateRSISyncTransactionControllerInterfaceCodeSecurityManager(
    AccessVerifierConstructorArgs&&... constructorArgs)
{
    return CreateRSInterfaceCodeSecurityManager<RSISyncTransactionControllerInterfaceCodeAccessVerifier>(
        std::forward<AccessVerifierConstructorArgs>(constructorArgs)...);
}

template<typename... AccessVerifierConstructorArgs>
inline auto CreateRSIScreenChangeCallbackInterfaceCodeSecurityManager(
    AccessVerifierConstructorArgs&&... constructorArgs)
{
    return CreateRSInterfaceCodeSecurityManager<RSIScreenChangeCallbackInterfaceCodeAccessVerifier>(
        std::forward<AccessVerifierConstructorArgs>(constructorArgs)...);
}

template<typename... AccessVerifierConstructorArgs>
inline auto CreateRSISurfaceCaptureCallbackInterfaceCodeSecurityManager(
    AccessVerifierConstructorArgs&&... constructorArgs)
{
    return CreateRSInterfaceCodeSecurityManager<RSISurfaceCaptureCallbackInterfaceCodeAccessVerifier>(
        std::forward<AccessVerifierConstructorArgs>(constructorArgs)...);
}

template<typename... AccessVerifierConstructorArgs>
inline auto CreateRSIRenderServiceConnectionInterfaceCodeSecurityManager(
    AccessVerifierConstructorArgs&&... constructorArgs)
{
    return CreateRSInterfaceCodeSecurityManager<RSIRenderServiceConnectionInterfaceCodeAccessVerifier>(
        std::forward<AccessVerifierConstructorArgs>(constructorArgs)...);
}

template<typename... AccessVerifierConstructorArgs>
inline auto CreateRSIRenderServiceInterfaceCodeSecurityManager(
    AccessVerifierConstructorArgs&&... constructorArgs)
{
    return CreateRSInterfaceCodeSecurityManager<RSIRenderServiceInterfaceCodeAccessVerifier>(
        std::forward<AccessVerifierConstructorArgs>(constructorArgs)...);
}
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_RENDER_SERVICE_BASE_RS_IPC_INTERFACE_CODE_SECURITY_MANAGER_REGISTRY_H
