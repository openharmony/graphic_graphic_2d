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

#include "platform/ohos/transaction/rs_iclient_to_render_connection_ipc_interface_code_access_verifier.h"

namespace OHOS {
namespace Rosen {

std::vector<std::pair<RSIClientToRenderConnectionInterfaceCodeAccessVerifier::CodeEnumType, PermissionType>>
    RSIClientToRenderConnectionInterfaceCodeAccessVerifier::permissionRSIRenderServiceConnectionInterfaceMappings_ {
        { RSIClientToRenderConnectionInterfaceCodeAccessVerifier::CodeEnumType::TAKE_SURFACE_CAPTURE,
            PermissionType::CAPTURE_SCREEN },
        { RSIClientToRenderConnectionInterfaceCodeAccessVerifier::CodeEnumType::TAKE_SURFACE_CAPTURE_WITH_ALL_WINDOWS,
            PermissionType::CAPTURE_SCREEN_ALL }
};

RSIClientToRenderConnectionInterfaceCodeAccessVerifier::RSIClientToRenderConnectionInterfaceCodeAccessVerifier()
{
    CheckCodeUnderlyingTypeStandardized<CodeEnumType>(codeEnumTypeName_);
#ifdef ENABLE_IPC_SECURITY
    AddRSIRenderServiceConnectionInterfaceCodePermission();
#endif
}

bool RSIClientToRenderConnectionInterfaceCodeAccessVerifier::IsExclusiveVerificationPassed(CodeUnderlyingType code)
{
    bool hasPermission = true;
    switch (code) {
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_BRIGHTNESS_INFO): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_BRIGHTNESS_INFO");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::DROP_FRAME_BY_PID): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::DROP_FRAME_BY_PID");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_FOCUS_APP_INFO): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_FOCUS_APP_INFO");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_SURFACE_WATERMARK): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_SURFACE_WATERMARK");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::FORCE_REFRESH_ONE_FRAME_WITH_NEXT_VSYNC): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::FORCE_REFRESH_ONE_FRAME_WITH_NEXT_VSYNC");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::CLEAR_SURFACE_WATERMARK_FOR_NODES): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::CLEAR_SURFACE_WATERMARK_FOR_NODES");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::CLEAR_SURFACE_WATERMARK): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::CLEAR_SURFACE_WATERMARK");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_SYSTEM_ANIMATED_SCENES): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_SYSTEM_ANIMATED_SCENES");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_GLOBAL_DARK_COLOR_MODE): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_GLOBAL_DARK_COLOR_MODE");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::TAKE_UI_CAPTURE_IN_RANGE): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::TAKE_UI_CAPTURE_IN_RANGE");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_POINTER_POSITION): {
            hasPermission = CheckPermission(code);
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_ANCO_FORCE_DO_DIRECT): {
            hasPermission = IsAncoCalling(codeEnumTypeName_ + "::SET_ANCO_FORCE_DO_DIRECT");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_BUFFER_AVAILABLE_LISTENER): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_BUFFER_AVAILABLE_LISTENER");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_BUFFER_CLEAR_LISTENER): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_BUFFER_CLEAR_LISTENER");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::CREATE_DISPLAY_NODE): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::CREATE_DISPLAY_NODE");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_LAYER_TOP_FOR_HARDWARE_COMPOSER): {
            hasPermission = IsStylusServiceCalling(codeEnumTypeName_ + "::SET_LAYER_TOP_FOR_HARDWARE_COMPOSER");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_WINDOW_FREEZE_IMMEDIATELY): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_WINDOW_FREEZE_IMMEDIATELY");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_WINDOW_CONTAINER): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_WINDOW_CONTAINER");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::CLEAR_UIFIRST_CACHE): {
            hasPermission = IsTaskManagerCalling(codeEnumTypeName_ + "::CLEAR_UIFIRST_CACHE");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SCREEN_HDR_STATUS): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_SCREEN_HDR_STATUS");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::TAKE_SURFACE_CAPTURE_WITH_ALL_WINDOWS): {
            hasPermission = CheckPermission(code);
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::FREEZE_SCREEN): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::FREEZE_SCREEN");
            break;
        }
        default: {
            break;
        }
    }
    return hasPermission;
}

#ifdef ENABLE_IPC_SECURITY
void RSIClientToRenderConnectionInterfaceCodeAccessVerifier::AddRSIRenderServiceConnectionInterfaceCodePermission()
{
    for (auto& mapping : permissionRSIRenderServiceConnectionInterfaceMappings_) {
        CodeEnumType interfaceName = mapping.first;
        PermissionType permission = mapping.second;
        std::string newPermission = PermissionEnumToString(permission);
        if (newPermission == "unknown") {
            continue;
        }
        CodeUnderlyingType code = static_cast<CodeUnderlyingType>(interfaceName);
        AddPermission(code, newPermission);
    }
}

bool RSIClientToRenderConnectionInterfaceCodeAccessVerifier::IsAccessTimesVerificationPassed(
    CodeUnderlyingType code, uint32_t times) const
{
    auto interfaceName = static_cast<CodeEnumType>(code);
    if (accessRSIRenderServiceConnectionInterfaceTimesRestrictions_.count(interfaceName) == 0) {
        return true;
    }
    uint32_t restrictedTimes = accessRSIRenderServiceConnectionInterfaceTimesRestrictions_.at(interfaceName);
    return times < restrictedTimes;
}
#endif

} // namespace Rosen
} // namespace OHOS
