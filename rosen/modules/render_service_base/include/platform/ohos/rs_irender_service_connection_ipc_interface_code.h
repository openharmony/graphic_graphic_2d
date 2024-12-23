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

#ifndef ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_IRENDER_SERVICE_CONNECTION_INTERFACE_CODE_H
#define ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_IRENDER_SERVICE_CONNECTION_INTERFACE_CODE_H

#include "ipc_security/rs_ipc_interface_code_underlying_type.h"

namespace OHOS {
namespace Rosen {
enum class RSIRenderServiceConnectionInterfaceCode : CodeUnderlyingType {
    COMMIT_TRANSACTION,
    GET_UNI_RENDER_ENABLED,
    CREATE_NODE,
    CREATE_NODE_AND_SURFACE,
    SET_FOCUS_APP_INFO,
    GET_DEFAULT_SCREEN_ID,
    GET_ACTIVE_SCREEN_ID,
    GET_ALL_SCREEN_IDS,
    CREATE_VIRTUAL_SCREEN,
    SET_VIRTUAL_SCREEN_RESOLUTION,
    SET_VIRTUAL_SCREEN_SURFACE,
    SET_VIRTUAL_SCREEN_BLACKLIST,
    ADD_VIRTUAL_SCREEN_BLACKLIST,
    REMOVE_VIRTUAL_SCREEN_BLACKLIST,
    SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST,
    SET_CAST_SCREEN_ENABLE_SKIP_WINDOW,
    REMOVE_VIRTUAL_SCREEN,
    SET_SCREEN_CHANGE_CALLBACK,
    SET_SCREEN_ACTIVE_MODE,
    SET_SCREEN_REFRESH_RATE,
    SET_REFRESH_RATE_MODE,
    SYNC_FRAME_RATE_RANGE,
    UNREGISTER_FRAME_RATE_LINKER,
    GET_SCREEN_CURRENT_REFRESH_RATE,
    GET_CURRENT_REFRESH_RATE_MODE,
    GET_SCREEN_SUPPORTED_REFRESH_RATES,
    GET_SHOW_REFRESH_RATE_ENABLED,
    SET_SHOW_REFRESH_RATE_ENABLED,
    GET_REFRESH_INFO,
    MARK_POWER_OFF_NEED_PROCESS_ONE_FRAME,
    DISABLE_RENDER_CONTROL_SCREEN,
    SET_SCREEN_POWER_STATUS,
    SET_SCREEN_BACK_LIGHT,
    TAKE_SURFACE_CAPTURE,
    GET_SCREEN_ACTIVE_MODE,
    GET_SCREEN_SUPPORTED_MODES,
    GET_SCREEN_CAPABILITY,
    GET_SCREEN_POWER_STATUS,
    GET_SCREEN_BACK_LIGHT,
    GET_SCREEN_DATA,
    GET_VIRTUAL_SCREEN_RESOLUTION,
    REGISTER_APPLICATION_AGENT,
    SET_BUFFER_AVAILABLE_LISTENER,
    SET_BUFFER_CLEAR_LISTENER,
    GET_SCREEN_SUPPORTED_GAMUTS,
    GET_SCREEN_SUPPORTED_METADATAKEYS,
    GET_SCREEN_GAMUT,
    SET_SCREEN_GAMUT,
    SET_SCREEN_GAMUT_MAP,
    SET_SCREEN_CORRECTION,
    SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION,
    SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE,
    SET_GLOBAL_DARK_COLOR_MODE,
    GET_SCREEN_GAMUT_MAP,
    CREATE_VSYNC_CONNECTION,
    CREATE_PIXEL_MAP_FROM_SURFACE,
    GET_SCREEN_HDR_CAPABILITY,
    SET_PIXEL_FORMAT,
    GET_PIXEL_FORMAT,
    GET_SCREEN_SUPPORTED_HDR_FORMATS,
    GET_SCREEN_HDR_FORMAT,
    SET_SCREEN_HDR_FORMAT,
    GET_SCREEN_SUPPORTED_COLORSPACES,
    GET_SCREEN_COLORSPACE,
    SET_SCREEN_COLORSPACE,
    GET_SCREEN_TYPE,
    SET_SCREEN_SKIP_FRAME_INTERVAL,
    REGISTER_OCCLUSION_CHANGE_CALLBACK,
    SET_APP_WINDOW_NUM,
    SET_SYSTEM_ANIMATED_SCENES,
    SHOW_WATERMARK,
    RESIZE_VIRTUAL_SCREEN,
    GET_MEMORY_GRAPHIC,
    GET_MEMORY_GRAPHICS,
    GET_TOTAL_APP_MEM_SIZE,
    REPORT_JANK_STATS,
    REPORT_EVENT_RESPONSE,
    REPORT_EVENT_COMPLETE,
    REPORT_EVENT_JANK_FRAME,
    REPORT_EVENT_GAMESTATE,
    GET_BITMAP,
    GET_PIXELMAP,
    EXECUTE_SYNCHRONOUS_TASK,
    NOTIFY_TOUCH_EVENT,
    SET_HARDWARE_ENABLED,
    REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK,
    UNREGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK,
    REGISTER_HGM_CFG_CALLBACK,
    SET_ROTATION_CACHE_ENABLED,
    SET_DEFAULT_DEVICE_ROTATION_OFFSET,
    SET_TP_FEATURE_CONFIG,
    SET_VIRTUAL_SCREEN_USING_STATUS,
    REFRESH_RATE_MODE_CHANGE_CALLBACK,
    REGISTER_TYPEFACE,
    UNREGISTER_TYPEFACE,
    REFRESH_RATE_UPDATE_CALLBACK,
    SET_CURTAIN_SCREEN_USING_STATUS,
    DROP_FRAME_BY_PID,
    GET_ACTIVE_DIRTY_REGION_INFO,
    GET_GLOBAL_DIRTY_REGION_INFO,
    GET_LAYER_COMPOSE_INFO,
    GET_HARDWARE_COMPOSE_DISABLED_REASON_INFO,
    REGISTER_UIEXTENSION_CALLBACK,
    SET_ANCO_FORCE_DO_DIRECT,
    SET_VMA_CACHE_STATUS,
    NEED_REGISTER_TYPEFACE,
    SET_LAYER_TOP,
    CREATE_DISPLAY_NODE,
    SET_HIDE_PRIVACY_CONTENT,
    REGISTER_SURFACE_BUFFER_CALLBACK,
    UNREGISTER_SURFACE_BUFFER_CALLBACK,
    SET_VIRTUAL_SCREEN_REFRESH_RATE,
// Special invocation. Do not change it.
    NOTIFY_LIGHT_FACTOR_STATUS = 1000,
    NOTIFY_PACKAGE_EVENT = 1001,
    NOTIFY_REFRESH_RATE_EVENT = 1002,
    NOTIFY_DYNAMIC_MODE_EVENT = 1003,
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_IRENDER_SERVICE_CONNECTION_INTERFACE_CODE_H
