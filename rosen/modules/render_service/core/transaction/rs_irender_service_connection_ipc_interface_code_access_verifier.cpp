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
}

bool RSIRenderServiceConnectionInterfaceCodeAccessVerifier::IsExclusiveVerificationPassed(CodeUnderlyingType code)
{
    bool hasPermission = true;
    switch (code) {
        case static_cast<CodeUnderlyingType>(CodeEnumType::COMMIT_TRANSACTION): {
            /* to implement access interception */
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_UNI_RENDER_ENABLED): {
            /* low risk, skip this case */
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::CREATE_NODE): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::CREATE_NODE");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::CREATE_NODE_AND_SURFACE): {
            /* to implement access interception */
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_FOCUS_APP_INFO): {
            /* to implement access interception */
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_DEFAULT_SCREEN_ID): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_DEFAULT_SCREEN_ID");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_ALL_SCREEN_IDS): {
            /* low risk, skip this case */
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::CREATE_VIRTUAL_SCREEN): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::CREATE_VIRTUAL_SCREEN");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_VIRTUAL_SCREEN_SURFACE): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_VIRTUAL_SCREEN_SURFACE");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::REMOVE_VIRTUAL_SCREEN): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::REMOVE_VIRTUAL_SCREEN");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_SCREEN_CHANGE_CALLBACK): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_SCREEN_CHANGE_CALLBACK");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_SCREEN_ACTIVE_MODE): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_SCREEN_ACTIVE_MODE");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_SCREEN_REFRESH_RATE): {
            /* to implement access interception */
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_REFRESH_RATE_MODE): {
            /* to implement access interception */
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SCREEN_CURRENT_REFRESH_RATE): {
            /* to implement access interception */
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SCREEN_SUPPORTED_REFRESH_RATES): {
            /* to implement access interception */
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_VIRTUAL_SCREEN_RESOLUTION): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_VIRTUAL_SCREEN_RESOLUTION");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_SCREEN_POWER_STATUS): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_SCREEN_POWER_STATUS");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::TAKE_SURFACE_CAPTURE): {
            /* to implement access interception */
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::REGISTER_APPLICATION_AGENT): {
            /* to implement access interception */
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_VIRTUAL_SCREEN_RESOLUTION): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_VIRTUAL_SCREEN_RESOLUTION");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SCREEN_ACTIVE_MODE): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_SCREEN_ACTIVE_MODE");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SCREEN_SUPPORTED_MODES): {
            /* low risk, skip this case */
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_MEMORY_GRAPHIC): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_MEMORY_GRAPHIC");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_MEMORY_GRAPHICS): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_MEMORY_GRAPHICS");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_TOTAL_APP_MEM_SIZE): {
            /* to implement access interception */
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SCREEN_CAPABILITY): {
            /* to implement access interception */
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SCREEN_POWER_STATUS): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_SCREEN_POWER_STATUS");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SCREEN_DATA): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_SCREEN_DATA");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SCREEN_BACK_LIGHT): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_SCREEN_BACK_LIGHT");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_SCREEN_BACK_LIGHT): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_SCREEN_BACK_LIGHT");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_BUFFER_AVAILABLE_LISTENER): {
            /* to implement access interception */
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_BUFFER_CLEAR_LISTENER): {
            /* to implement access interception */
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SCREEN_SUPPORTED_GAMUTS): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_SCREEN_SUPPORTED_GAMUTS");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SCREEN_SUPPORTED_METADATAKEYS): {
            /* to implement access interception */
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SCREEN_GAMUT): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_SCREEN_GAMUT");
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
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SCREEN_GAMUT_MAP): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_SCREEN_GAMUT_MAP");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::CREATE_VSYNC_CONNECTION): {
            /* to implement access interception */
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SCREEN_HDR_CAPABILITY): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_SCREEN_HDR_CAPABILITY");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_SCREEN_TYPE): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::GET_SCREEN_TYPE");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::GET_BITMAP): {
            /* to implement access interception */
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_SCREEN_SKIP_FRAME_INTERVAL): {
            /* to implement access interception */
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::REGISTER_OCCLUSION_CHANGE_CALLBACK): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::REGISTER_OCCLUSION_CHANGE_CALLBACK");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_APP_WINDOW_NUM): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SET_APP_WINDOW_NUM");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SHOW_WATERMARK): {
            hasPermission = IsSystemCalling(codeEnumTypeName_ + "::SHOW_WATERMARK");
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::REPORT_JANK_STATS): {
            /* to implement access interception */
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::REPORT_EVENT_RESPONSE): {
            /* to implement access interception */
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::REPORT_EVENT_COMPLETE): {
            /* to implement access interception */
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::REPORT_EVENT_JANK_FRAME): {
            /* to implement access interception */
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::EXECUTE_SYNCHRONOUS_TASK): {
            /* to implement access interception */
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::SET_HARDWARE_ENABLED): {
            /* to implement access interception */
            break;
        }
        case static_cast<CodeUnderlyingType>(CodeEnumType::REGISTER_HGM_CFG_CALLBACK): {
            /* to implement access interception */
            break;
        }
        default: {
            break;
        }
    }
    return hasPermission;
}
} // namespace Rosen
} // namespace OHOS
