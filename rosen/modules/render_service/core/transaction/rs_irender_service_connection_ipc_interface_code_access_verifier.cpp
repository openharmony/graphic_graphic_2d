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

#include "platform/ohos/rs_irender_service_connection_ipc_interface_code_access_verifier.h"

namespace OHOS {
namespace Rosen {
RSIRenderServiceConnectionInterfaceCodeAccessVerifier::RSIRenderServiceConnectionInterfaceCodeAccessVerifier()
{
    CheckCodeUnderlyingTypeStandardized<CodeEnumType>(codeEnumTypeName_);
#ifdef ENABLE_IPC_SECURITY
    AddRSIRenderServiceConnectionInterfaceCodePermission();
#endif
}

bool RSIRenderServiceConnectionInterfaceCodeAccessVerifier::IsExclusiveVerificationPassed(CodeUnderlyingType code)
{
    bool hasPermission = true;
    switch (code) {
        case static_cast<CodeUnderlyingType>(CodeEnumType::CREATE_VIRTUAL_SCREEN): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::CREATE_VIRTUAL_SCREEN");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::DISABLE_RENDER_CONTROL_SCREEN): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::DISABLE_RENDER_CONTROL_SCREEN");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_ACTIVE_SCREEN_ID): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_ACTIVE_SCREEN_ID");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_ALL_SCREEN_IDS): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_ALL_SCREEN_IDS");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_DEFAULT_SCREEN_ID): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_DEFAULT_SCREEN_ID");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SCREEN_ACTIVE_MODE): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_SCREEN_ACTIVE_MODE");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SCREEN_BACK_LIGHT): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_SCREEN_BACK_LIGHT");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SCREEN_CAPABILITY): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_SCREEN_CAPABILITY");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SCREEN_DATA): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_SCREEN_DATA");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SCREEN_GAMUT): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_SCREEN_GAMUT");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SCREEN_GAMUT_MAP): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_SCREEN_GAMUT_MAP");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SCREEN_HDR_CAPABILITY): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_SCREEN_HDR_CAPABILITY");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SCREEN_POWER_STATUS): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_SCREEN_POWER_STATUS");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SCREEN_SUPPORTED_GAMUTS): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_SCREEN_SUPPORTED_GAMUTS");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SCREEN_SUPPORTED_METADATAKEYS): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_SCREEN_SUPPORTED_METADATAKEYS");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SCREEN_SUPPORTED_MODES): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_SCREEN_SUPPORTED_MODES");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SCREEN_TYPE): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_SCREEN_TYPE");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_VIRTUAL_SCREEN_RESOLUTION): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_VIRTUAL_SCREEN_RESOLUTION");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::MARK_POWER_OFF_NEED_PROCESS_ONE_FRAME): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::MARK_POWER_OFF_NEED_PROCESS_ONE_FRAME");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::REGISTER_OCCLUSION_CHANGE_CALLBACK): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::REGISTER_OCCLUSION_CHANGE_CALLBACK");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::REGISTER_UIEXTENSION_CALLBACK): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::REGISTER_UIEXTENSION_CALLBACK");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::REMOVE_VIRTUAL_SCREEN): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::REMOVE_VIRTUAL_SCREEN");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::RESIZE_VIRTUAL_SCREEN): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::RESIZE_VIRTUAL_SCREEN");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_CAST_SCREEN_ENABLE_SKIP_WINDOW): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_CAST_SCREEN_ENABLE_SKIP_WINDOW");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_CURTAIN_SCREEN_USING_STATUS): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_CURTAIN_SCREEN_USING_STATUS");
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
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_SCREEN_ACTIVE_MODE): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_SCREEN_ACTIVE_MODE");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_SCREEN_BACK_LIGHT): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_SCREEN_BACK_LIGHT");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_SCREEN_CHANGE_CALLBACK): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_SCREEN_CHANGE_CALLBACK");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_SCREEN_CORRECTION): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_SCREEN_CORRECTION");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_SCREEN_GAMUT): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_SCREEN_GAMUT");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_SCREEN_GAMUT_MAP): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_SCREEN_GAMUT_MAP");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_SYSTEM_ANIMATED_SCENES): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_SYSTEM_ANIMATED_SCENES");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_TP_FEATURE_CONFIG): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_TP_FEATURE_CONFIG");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_GLOBAL_DARK_COLOR_MODE): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_GLOBAL_DARK_COLOR_MODE");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_VIRTUAL_SCREEN_BLACKLIST): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_VIRTUAL_SCREEN_BLACKLIST");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::ADD_VIRTUAL_SCREEN_BLACKLIST): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::ADD_VIRTUAL_SCREEN_BLACKLIST");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::REMOVE_VIRTUAL_SCREEN_BLACKLIST): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::REMOVE_VIRTUAL_SCREEN_BLACKLIST");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_PHYSICAL_SCREEN_RESOLUTION): {
            hasPermission = IsFoundationCalling(codeEnumTypeName_ + "::SET_PHYSICAL_SCREEN_RESOLUTION");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_VIRTUAL_SCREEN_RESOLUTION): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_VIRTUAL_SCREEN_RESOLUTION");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_VIRTUAL_SCREEN_SURFACE): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_VIRTUAL_SCREEN_SURFACE");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_VIRTUAL_SCREEN_USING_STATUS): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_VIRTUAL_SCREEN_USING_STATUS");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SHOW_WATERMARK): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SHOW_WATERMARK") &&
                CheckPermission(code);
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_REFRESH_RATE_MODE): {
            hasPermission = IsSystemApp();
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_CURRENT_REFRESH_RATE_MODE): {
            hasPermission = IsSystemApp();
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SCREEN_SUPPORTED_REFRESH_RATES): {
            hasPermission = IsSystemApp() ||
                IsSystemCalling(codeEnumTypeName_ + "::GET_SCREEN_SUPPORTED_REFRESH_RATES");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SHOW_REFRESH_RATE_ENABLED): {
            hasPermission = IsSystemApp();
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_SHOW_REFRESH_RATE_ENABLED): {
            hasPermission = IsSystemApp();
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_SCREEN_REFRESH_RATE): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_SCREEN_REFRESH_RATE");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SCREEN_CURRENT_REFRESH_RATE): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_SCREEN_CURRENT_REFRESH_RATE");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::NOTIFY_LIGHT_FACTOR_STATUS): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::NOTIFY_LIGHT_FACTOR_STATUS");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::NOTIFY_PACKAGE_EVENT): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::NOTIFY_PACKAGE_EVENT");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::NOTIFY_REFRESH_RATE_EVENT): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::NOTIFY_REFRESH_RATE_EVENT");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::NOTIFY_DYNAMIC_MODE_EVENT): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::NOTIFY_DYNAMIC_MODE_EVENT");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::NOTIFY_TOUCH_EVENT): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::NOTIFY_TOUCH_EVENT");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::TAKE_SURFACE_CAPTURE): {
            hasPermission = CheckPermission(code);
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_MEMORY_GRAPHICS): {
            hasPermission = CheckPermission(code);
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_SCREEN_POWER_STATUS): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_SCREEN_POWER_STATUS");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::REPORT_EVENT_GAMESTATE): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::REPORT_EVENT_GAMESTATE");
            break;
        }

        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST): {
            hasPermission = IsFoundationCalling(codeEnumTypeName_ + "::SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_ANCO_FORCE_DO_DIRECT): {
            hasPermission = IsAncoCalling(codeEnumTypeName_ + "::SET_ANCO_FORCE_DO_DIRECT");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_VMA_CACHE_STATUS): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_VMA_CACHE_STATUS");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_ROTATION_CACHE_ENABLED): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_ROTATION_CACHE_ENABLED");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_SCREEN_SKIP_FRAME_INTERVAL): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_SCREEN_SKIP_FRAME_INTERVAL");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_VIRTUAL_SCREEN_REFRESH_RATE): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_VIRTUAL_SCREEN_REFRESH_RATE");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_SCREEN_ACTIVE_RECT): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_SCREEN_ACTIVE_RECT");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_PIXEL_FORMAT): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_PIXEL_FORMAT");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_PIXEL_FORMAT): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_PIXEL_FORMAT");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SCREEN_SUPPORTED_HDR_FORMATS): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_SCREEN_SUPPORTED_HDR_FORMATS");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SCREEN_HDR_FORMAT): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_SCREEN_HDR_FORMAT");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_SCREEN_HDR_FORMAT): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_SCREEN_HDR_FORMAT");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SCREEN_SUPPORTED_COLORSPACES): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_SCREEN_SUPPORTED_COLORSPACES");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SCREEN_COLORSPACE): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_SCREEN_COLORSPACE");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_SCREEN_COLORSPACE): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_SCREEN_COLORSPACE");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_ACTIVE_DIRTY_REGION_INFO): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_ACTIVE_DIRTY_REGION_INFO");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_GLOBAL_DIRTY_REGION_INFO): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_GLOBAL_DIRTY_REGION_INFO");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_LAYER_COMPOSE_INFO): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_LAYER_COMPOSE_INFO");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_HARDWARE_COMPOSE_DISABLED_REASON_INFO): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_HARDWARE_COMPOSE_DISABLED_REASON_INFO");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_LAYER_TOP): {
            hasPermission = IsStylusServiceCalling(codeEnumTypeName_ + "::SET_LAYER_TOP");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::CREATE_DISPLAY_NODE): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::CREATE_DISPLAY_NODE");
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
        default: {
            break;
        }
    }
    return hasPermission;
}

#ifdef ENABLE_IPC_SECURITY
void RSIRenderServiceConnectionInterfaceCodeAccessVerifier::AddRSIRenderServiceConnectionInterfaceCodePermission()
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

bool RSIRenderServiceConnectionInterfaceCodeAccessVerifier::IsAccessTimesVerificationPassed(
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
