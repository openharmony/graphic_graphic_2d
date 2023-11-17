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
    REMOVE_VIRTUAL_SCREEN,
    SET_SCREEN_CHANGE_CALLBACK,
    SET_SCREEN_ACTIVE_MODE,
    SET_SCREEN_REFRESH_RATE,
    SET_REFRESH_RATE_MODE,
    SYNC_FRAME_RATE_RANGE,
    GET_SCREEN_CURRENT_REFRESH_RATE,
    GET_CURRENT_REFRESH_RATE_MODE,
    GET_SCREEN_SUPPORTED_REFRESH_RATES,
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
    GET_SCREEN_GAMUT_MAP,
    CREATE_VSYNC_CONNECTION,
    GET_SCREEN_HDR_CAPABILITY,
    GET_SCREEN_TYPE,
    SET_SCREEN_SKIP_FRAME_INTERVAL,
    REGISTER_OCCLUSION_CHANGE_CALLBACK,
    SET_APP_WINDOW_NUM,
    SHOW_WATERMARK,
    RESIZE_VIRTUAL_SCREEN,
    GET_MEMORY_GRAPHIC,
    GET_MEMORY_GRAPHICS,
    GET_TOTAL_APP_MEM_SIZE,
    REPORT_JANK_STATS,
    GET_BITMAP,
    GET_PIXELMAP,
    EXECUTE_SYNCHRONOUS_TASK,
    REPORT_EVENT_RESPONSE,
    REPORT_EVENT_COMPLETE,
    REPORT_EVENT_JANK_FRAME,
    SET_HARDWARE_ENABLED,
    REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK,
    UNREGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK,
    REGISTER_HGM_CFG_CALLBACK,
    SET_ROTATION_CACHE_ENABLED,
    SET_TP_FEATURE_CONFIG,
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_IRENDER_SERVICE_CONNECTION_INTERFACE_CODE_H
