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

#include "ipc/rs_irender_service_connection_ipc_interface_code_utils.h"

#include <unordered_map>
#include <unordered_set>

#include "common/safuzz_log.h"

namespace OHOS {
namespace Rosen {
namespace {
#define DECLARE_INTERFACE_CODE_NAME(code) { #code, RSIRenderServiceConnectionInterfaceCode::code }

const std::unordered_map<std::string, RSIRenderServiceConnectionInterfaceCode> INTERFACE_NAME_TO_CODE_MAPPING = {
    DECLARE_INTERFACE_CODE_NAME(COMMIT_TRANSACTION),
    DECLARE_INTERFACE_CODE_NAME(GET_UNI_RENDER_ENABLED),
    DECLARE_INTERFACE_CODE_NAME(CREATE_NODE),
    DECLARE_INTERFACE_CODE_NAME(CREATE_NODE_AND_SURFACE),
    DECLARE_INTERFACE_CODE_NAME(SET_FOCUS_APP_INFO),
    DECLARE_INTERFACE_CODE_NAME(GET_DEFAULT_SCREEN_ID),
    DECLARE_INTERFACE_CODE_NAME(GET_ACTIVE_SCREEN_ID),
    DECLARE_INTERFACE_CODE_NAME(GET_ALL_SCREEN_IDS),
    DECLARE_INTERFACE_CODE_NAME(CREATE_VIRTUAL_SCREEN),
    DECLARE_INTERFACE_CODE_NAME(SET_PHYSICAL_SCREEN_RESOLUTION),
    DECLARE_INTERFACE_CODE_NAME(SET_VIRTUAL_SCREEN_RESOLUTION),
    DECLARE_INTERFACE_CODE_NAME(SET_VIRTUAL_SCREEN_SURFACE),
    DECLARE_INTERFACE_CODE_NAME(SET_VIRTUAL_SCREEN_BLACKLIST),
    DECLARE_INTERFACE_CODE_NAME(ADD_VIRTUAL_SCREEN_BLACKLIST),
    DECLARE_INTERFACE_CODE_NAME(REMOVE_VIRTUAL_SCREEN_BLACKLIST),
    DECLARE_INTERFACE_CODE_NAME(SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST),
    DECLARE_INTERFACE_CODE_NAME(SET_SCREEN_SECURITY_MASK),
    DECLARE_INTERFACE_CODE_NAME(SET_MIRROR_SCREEN_VISIBLE_RECT),
    DECLARE_INTERFACE_CODE_NAME(SET_CAST_SCREEN_ENABLE_SKIP_WINDOW),
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    DECLARE_INTERFACE_CODE_NAME(SET_POINTER_COLOR_INVERSION_CONFIG),
    DECLARE_INTERFACE_CODE_NAME(SET_POINTER_COLOR_INVERSION_ENABLED),
    DECLARE_INTERFACE_CODE_NAME(REGISTER_POINTER_LUMINANCE_CALLBACK),
    DECLARE_INTERFACE_CODE_NAME(UNREGISTER_POINTER_LUMINANCE_CALLBACK),
#endif
    DECLARE_INTERFACE_CODE_NAME(REMOVE_VIRTUAL_SCREEN),
    DECLARE_INTERFACE_CODE_NAME(SET_SCREEN_CHANGE_CALLBACK),
    DECLARE_INTERFACE_CODE_NAME(SET_SCREEN_ACTIVE_MODE),
    DECLARE_INTERFACE_CODE_NAME(SET_SCREEN_REFRESH_RATE),
    DECLARE_INTERFACE_CODE_NAME(SET_REFRESH_RATE_MODE),
    DECLARE_INTERFACE_CODE_NAME(SYNC_FRAME_RATE_RANGE),
    DECLARE_INTERFACE_CODE_NAME(UNREGISTER_FRAME_RATE_LINKER),
    DECLARE_INTERFACE_CODE_NAME(GET_SCREEN_CURRENT_REFRESH_RATE),
    DECLARE_INTERFACE_CODE_NAME(GET_CURRENT_REFRESH_RATE_MODE),
    DECLARE_INTERFACE_CODE_NAME(GET_SCREEN_SUPPORTED_REFRESH_RATES),
    DECLARE_INTERFACE_CODE_NAME(GET_SHOW_REFRESH_RATE_ENABLED),
    DECLARE_INTERFACE_CODE_NAME(SET_SHOW_REFRESH_RATE_ENABLED),
    DECLARE_INTERFACE_CODE_NAME(GET_REALTIME_REFRESH_RATE),
    DECLARE_INTERFACE_CODE_NAME(GET_REFRESH_INFO),
    DECLARE_INTERFACE_CODE_NAME(MARK_POWER_OFF_NEED_PROCESS_ONE_FRAME),
    DECLARE_INTERFACE_CODE_NAME(DISABLE_RENDER_CONTROL_SCREEN),
    DECLARE_INTERFACE_CODE_NAME(SET_SCREEN_POWER_STATUS),
    DECLARE_INTERFACE_CODE_NAME(SET_SCREEN_BACK_LIGHT),
    DECLARE_INTERFACE_CODE_NAME(TAKE_SURFACE_CAPTURE),
    DECLARE_INTERFACE_CODE_NAME(SET_WINDOW_FREEZE_IMMEDIATELY),
    DECLARE_INTERFACE_CODE_NAME(SET_POINTER_POSITION),
    DECLARE_INTERFACE_CODE_NAME(GET_PIXELMAP_BY_PROCESSID),
    DECLARE_INTERFACE_CODE_NAME(GET_SCREEN_ACTIVE_MODE),
    DECLARE_INTERFACE_CODE_NAME(GET_SCREEN_SUPPORTED_MODES),
    DECLARE_INTERFACE_CODE_NAME(GET_SCREEN_CAPABILITY),
    DECLARE_INTERFACE_CODE_NAME(GET_SCREEN_POWER_STATUS),
    DECLARE_INTERFACE_CODE_NAME(GET_SCREEN_BACK_LIGHT),
    DECLARE_INTERFACE_CODE_NAME(GET_SCREEN_DATA),
    DECLARE_INTERFACE_CODE_NAME(GET_VIRTUAL_SCREEN_RESOLUTION),
    DECLARE_INTERFACE_CODE_NAME(REGISTER_APPLICATION_AGENT),
    DECLARE_INTERFACE_CODE_NAME(SET_BUFFER_AVAILABLE_LISTENER),
    DECLARE_INTERFACE_CODE_NAME(SET_BUFFER_CLEAR_LISTENER),
    DECLARE_INTERFACE_CODE_NAME(GET_SCREEN_SUPPORTED_GAMUTS),
    DECLARE_INTERFACE_CODE_NAME(GET_SCREEN_SUPPORTED_METADATAKEYS),
    DECLARE_INTERFACE_CODE_NAME(GET_SCREEN_GAMUT),
    DECLARE_INTERFACE_CODE_NAME(SET_SCREEN_GAMUT),
    DECLARE_INTERFACE_CODE_NAME(SET_SCREEN_GAMUT_MAP),
    DECLARE_INTERFACE_CODE_NAME(SET_SCREEN_CORRECTION),
    DECLARE_INTERFACE_CODE_NAME(SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION),
    DECLARE_INTERFACE_CODE_NAME(SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE),
    DECLARE_INTERFACE_CODE_NAME(SET_GLOBAL_DARK_COLOR_MODE),
    DECLARE_INTERFACE_CODE_NAME(GET_SCREEN_GAMUT_MAP),
    DECLARE_INTERFACE_CODE_NAME(CREATE_VSYNC_CONNECTION),
    DECLARE_INTERFACE_CODE_NAME(CREATE_PIXEL_MAP_FROM_SURFACE),
    DECLARE_INTERFACE_CODE_NAME(GET_SCREEN_HDR_CAPABILITY),
    DECLARE_INTERFACE_CODE_NAME(SET_PIXEL_FORMAT),
    DECLARE_INTERFACE_CODE_NAME(GET_PIXEL_FORMAT),
    DECLARE_INTERFACE_CODE_NAME(GET_SCREEN_SUPPORTED_HDR_FORMATS),
    DECLARE_INTERFACE_CODE_NAME(GET_SCREEN_HDR_FORMAT),
    DECLARE_INTERFACE_CODE_NAME(SET_SCREEN_HDR_FORMAT),
    DECLARE_INTERFACE_CODE_NAME(GET_SCREEN_SUPPORTED_COLORSPACES),
    DECLARE_INTERFACE_CODE_NAME(GET_SCREEN_COLORSPACE),
    DECLARE_INTERFACE_CODE_NAME(SET_SCREEN_COLORSPACE),
    DECLARE_INTERFACE_CODE_NAME(GET_SCREEN_TYPE),
    DECLARE_INTERFACE_CODE_NAME(SET_SCREEN_SKIP_FRAME_INTERVAL),
    DECLARE_INTERFACE_CODE_NAME(REGISTER_OCCLUSION_CHANGE_CALLBACK),
    DECLARE_INTERFACE_CODE_NAME(SET_APP_WINDOW_NUM),
    DECLARE_INTERFACE_CODE_NAME(SET_SYSTEM_ANIMATED_SCENES),
    DECLARE_INTERFACE_CODE_NAME(SHOW_WATERMARK),
    DECLARE_INTERFACE_CODE_NAME(SET_WATERMARK),
    DECLARE_INTERFACE_CODE_NAME(RESIZE_VIRTUAL_SCREEN),
    DECLARE_INTERFACE_CODE_NAME(GET_MEMORY_GRAPHIC),
    DECLARE_INTERFACE_CODE_NAME(GET_MEMORY_GRAPHICS),
    DECLARE_INTERFACE_CODE_NAME(GET_TOTAL_APP_MEM_SIZE),
    DECLARE_INTERFACE_CODE_NAME(REPORT_JANK_STATS),
    DECLARE_INTERFACE_CODE_NAME(REPORT_EVENT_RESPONSE),
    DECLARE_INTERFACE_CODE_NAME(REPORT_EVENT_COMPLETE),
    DECLARE_INTERFACE_CODE_NAME(REPORT_EVENT_JANK_FRAME),
    DECLARE_INTERFACE_CODE_NAME(REPORT_RS_SCENE_JANK_START),
    DECLARE_INTERFACE_CODE_NAME(REPORT_RS_SCENE_JANK_END),
    DECLARE_INTERFACE_CODE_NAME(REPORT_EVENT_GAMESTATE),
    DECLARE_INTERFACE_CODE_NAME(GET_BITMAP),
    DECLARE_INTERFACE_CODE_NAME(GET_PIXELMAP),
    DECLARE_INTERFACE_CODE_NAME(EXECUTE_SYNCHRONOUS_TASK),
    DECLARE_INTERFACE_CODE_NAME(NOTIFY_TOUCH_EVENT),
    DECLARE_INTERFACE_CODE_NAME(SET_HARDWARE_ENABLED),
    DECLARE_INTERFACE_CODE_NAME(REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK),
    DECLARE_INTERFACE_CODE_NAME(UNREGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK),
    DECLARE_INTERFACE_CODE_NAME(REGISTER_HGM_CFG_CALLBACK),
    DECLARE_INTERFACE_CODE_NAME(SET_ROTATION_CACHE_ENABLED),
    DECLARE_INTERFACE_CODE_NAME(SET_TP_FEATURE_CONFIG),
    DECLARE_INTERFACE_CODE_NAME(SET_VIRTUAL_SCREEN_USING_STATUS),
    DECLARE_INTERFACE_CODE_NAME(REFRESH_RATE_MODE_CHANGE_CALLBACK),
    DECLARE_INTERFACE_CODE_NAME(SET_CURTAIN_SCREEN_USING_STATUS),
    DECLARE_INTERFACE_CODE_NAME(DROP_FRAME_BY_PID),
    DECLARE_INTERFACE_CODE_NAME(REGISTER_TYPEFACE),
    DECLARE_INTERFACE_CODE_NAME(UNREGISTER_TYPEFACE),
    DECLARE_INTERFACE_CODE_NAME(REFRESH_RATE_UPDATE_CALLBACK),
    DECLARE_INTERFACE_CODE_NAME(SET_CURTAIN_SCREEN_USING_STATUS),
    DECLARE_INTERFACE_CODE_NAME(DROP_FRAME_BY_PID),
    DECLARE_INTERFACE_CODE_NAME(REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK),
    DECLARE_INTERFACE_CODE_NAME(GET_ACTIVE_DIRTY_REGION_INFO),
    DECLARE_INTERFACE_CODE_NAME(GET_GLOBAL_DIRTY_REGION_INFO),
    DECLARE_INTERFACE_CODE_NAME(GET_LAYER_COMPOSE_INFO),
    DECLARE_INTERFACE_CODE_NAME(GET_HARDWARE_COMPOSE_DISABLED_REASON_INFO),
    DECLARE_INTERFACE_CODE_NAME(GET_HDR_ON_DURATION),
    DECLARE_INTERFACE_CODE_NAME(REGISTER_UIEXTENSION_CALLBACK),
    DECLARE_INTERFACE_CODE_NAME(SET_ANCO_FORCE_DO_DIRECT),
    DECLARE_INTERFACE_CODE_NAME(SET_VMA_CACHE_STATUS),
    DECLARE_INTERFACE_CODE_NAME(NEED_REGISTER_TYPEFACE),
    DECLARE_INTERFACE_CODE_NAME(SET_VIRTUAL_SCREEN_STATUS),
    DECLARE_INTERFACE_CODE_NAME(CREATE_DISPLAY_NODE),
    DECLARE_INTERFACE_CODE_NAME(REGISTER_SURFACE_BUFFER_CALLBACK),
    DECLARE_INTERFACE_CODE_NAME(UNREGISTER_SURFACE_BUFFER_CALLBACK),
    DECLARE_INTERFACE_CODE_NAME(SET_LAYER_TOP),
    DECLARE_INTERFACE_CODE_NAME(SET_HIDE_PRIVACY_CONTENT),
    DECLARE_INTERFACE_CODE_NAME(SET_VIRTUAL_SCREEN_REFRESH_RATE),
    DECLARE_INTERFACE_CODE_NAME(SET_SCREEN_ACTIVE_RECT),
    DECLARE_INTERFACE_CODE_NAME(SET_FREE_MULTI_WINDOW_STATUS),
    DECLARE_INTERFACE_CODE_NAME(REPAINT_EVERYTHING),
    DECLARE_INTERFACE_CODE_NAME(FORCE_REFRESH_ONE_FRAME_WITH_NEXT_VSYNC),
    DECLARE_INTERFACE_CODE_NAME(SET_WINDOW_CONTAINER),
    DECLARE_INTERFACE_CODE_NAME(GET_DISPLAY_IDENTIFICATION_DATA),
#ifdef RS_ENABLE_OVERLAY_DISPLAY
    DECLARE_INTERFACE_CODE_NAME(SET_OVERLAY_DISPLAY_MODE),
#endif
    DECLARE_INTERFACE_CODE_NAME(NOTIFY_PAGE_NAME),
    DECLARE_INTERFACE_CODE_NAME(NOTIFY_LIGHT_FACTOR_STATUS),
    DECLARE_INTERFACE_CODE_NAME(NOTIFY_PACKAGE_EVENT),
    DECLARE_INTERFACE_CODE_NAME(NOTIFY_REFRESH_RATE_EVENT),
    DECLARE_INTERFACE_CODE_NAME(NOTIFY_DYNAMIC_MODE_EVENT),
    DECLARE_INTERFACE_CODE_NAME(NOTIFY_SOFT_VSYNC_EVENT),
    DECLARE_INTERFACE_CODE_NAME(NOTIFY_HGMCONFIG_EVENT),
    DECLARE_INTERFACE_CODE_NAME(NOTIFY_SCREEN_SWITCHED),
};

const std::unordered_set<std::string> PARCEL_SPLIT_ENABLED_INTERFACES = {
    "COMMIT_TRANSACTION",
};
}

int RSIRenderServiceConnectionInterfaceCodeUtils::GetCodeFromName(const std::string& interfaceName)
{
    auto it = INTERFACE_NAME_TO_CODE_MAPPING.find(interfaceName);
    if (it == INTERFACE_NAME_TO_CODE_MAPPING.end()) {
        SAFUZZ_LOGE("RSIRenderServiceConnectionInterfaceCodeUtils::GetCodeFromName interfaceName %s not found",
            interfaceName.c_str());
        return -1;
    }
    return static_cast<int>(it->second);
}

bool RSIRenderServiceConnectionInterfaceCodeUtils::IsParcelSplitEnabled(const std::string& interfaceName)
{
    return PARCEL_SPLIT_ENABLED_INTERFACES.find(interfaceName) != PARCEL_SPLIT_ENABLED_INTERFACES.end();
}
} // namespace Rosen
} // namespace OHOS
