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

//Each command HAVE TO have UNIQUE ID in ALL HISTORY
//If a command is not used and you want to delete it,
//just COMMENT it - and never use this value anymore
enum class RSIRenderServiceConnectionInterfaceCode : CodeUnderlyingType {
    COMMIT_TRANSACTION = 0x000000,
    GET_UNI_RENDER_ENABLED = 0x000001,
    CREATE_NODE = 0x000002,
    CREATE_NODE_AND_SURFACE = 0x000003,
    SET_FOCUS_APP_INFO = 0x000004,
    SET_PHYSICAL_SCREEN_RESOLUTION = 0x000005,
    SET_SCREEN_SECURITY_MASK = 0x000006,
    SET_MIRROR_SCREEN_VISIBLE_RECT = 0x000007,
    SET_CAST_SCREEN_ENABLE_SKIP_WINDOW = 0x000008,
    MARK_POWER_OFF_NEED_PROCESS_ONE_FRAME = 0x000009,
    DISABLE_RENDER_CONTROL_SCREEN = 0x00000A,
    SET_SCREEN_POWER_STATUS = 0x00000B,
    SET_SCREEN_BACK_LIGHT = 0x00000C,
    TAKE_SURFACE_CAPTURE = 0x00000D,
    SET_WINDOW_FREEZE_IMMEDIATELY = 0x00000E,
    SET_POINTER_POSITION = 0x00000F,
    GET_PIXELMAP_BY_PROCESSID = 0x000010,
    REGISTER_APPLICATION_AGENT = 0x000011,
    SET_BUFFER_AVAILABLE_LISTENER = 0x000012,
    SET_BUFFER_CLEAR_LISTENER = 0x000013,
    CREATE_VSYNC_CONNECTION = 0x000014,
    REGISTER_OCCLUSION_CHANGE_CALLBACK = 0x000015,
    SET_APP_WINDOW_NUM = 0x000016,
    SET_SYSTEM_ANIMATED_SCENES = 0x000017,
    REGISTER_HGM_CFG_CALLBACK = 0x000018,
    SET_ROTATION_CACHE_ENABLED = 0x000019,
    SET_TP_FEATURE_CONFIG = 0x00001A,
    SET_CURTAIN_SCREEN_USING_STATUS = 0x00001B,
    DROP_FRAME_BY_PID = 0x00001C,
    GET_LAYER_COMPOSE_INFO = 0x00001D,
    GET_HARDWARE_COMPOSE_DISABLED_REASON_INFO = 0x00001E,
    GET_HDR_ON_DURATION = 0x00001F,
    REGISTER_UIEXTENSION_CALLBACK = 0x000020,
    SET_ANCO_FORCE_DO_DIRECT = 0x000021,
    SET_VMA_CACHE_STATUS = 0x000022,
    CREATE_DISPLAY_NODE = 0x000023,
    SET_FREE_MULTI_WINDOW_STATUS = 0x000024,
    REGISTER_SURFACE_BUFFER_CALLBACK = 0x000025,
    UNREGISTER_SURFACE_BUFFER_CALLBACK = 0x000026,
    SET_LAYER_TOP = 0x000027,
    SET_SCREEN_ACTIVE_RECT = 0x000028,
    SET_HIDE_PRIVACY_CONTENT = 0x000029,
    REPAINT_EVERYTHING = 0x00002A,
    FORCE_REFRESH_ONE_FRAME_WITH_NEXT_VSYNC = 0x00002B,
    SET_WINDOW_CONTAINER = 0x00002C,

    GET_DEFAULT_SCREEN_ID = 0x001000,
    GET_ACTIVE_SCREEN_ID = 0x001001,
    GET_ALL_SCREEN_IDS = 0x001002,

    CREATE_VIRTUAL_SCREEN = 0x002000,
    SET_VIRTUAL_SCREEN_RESOLUTION = 0x002001,
    SET_VIRTUAL_SCREEN_SURFACE = 0x002002,
    SET_VIRTUAL_SCREEN_BLACKLIST = 0x002003,
    ADD_VIRTUAL_SCREEN_BLACKLIST = 0x002004,
    REMOVE_VIRTUAL_SCREEN_BLACKLIST = 0x002005,
    SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST = 0x002006,
    REMOVE_VIRTUAL_SCREEN = 0x002007,
    GET_VIRTUAL_SCREEN_RESOLUTION = 0x002008,
    RESIZE_VIRTUAL_SCREEN = 0x002009,
    SET_VIRTUAL_SCREEN_USING_STATUS = 0x00200A,
    SET_VIRTUAL_SCREEN_REFRESH_RATE = 0x00200B,
    SET_VIRTUAL_SCREEN_STATUS = 0x00200C,

#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    SET_POINTER_COLOR_INVERSION_CONFIG = 0x004000,
    SET_POINTER_COLOR_INVERSION_ENABLED = 0x004001,
    REGISTER_POINTER_LUMINANCE_CALLBACK = 0x004002,
    UNREGISTER_POINTER_LUMINANCE_CALLBACK = 0x004003,
#endif
    SET_SCREEN_CHANGE_CALLBACK = 0x005000,
    SET_SCREEN_ACTIVE_MODE = 0x005001,
    SET_SCREEN_REFRESH_RATE = 0x005002,
    SET_REFRESH_RATE_MODE = 0x005003,
    SYNC_FRAME_RATE_RANGE = 0x005004,
    UNREGISTER_FRAME_RATE_LINKER = 0x005005,
    GET_SCREEN_CURRENT_REFRESH_RATE = 0x005006,
    GET_CURRENT_REFRESH_RATE_MODE = 0x005007,
    GET_SCREEN_SUPPORTED_REFRESH_RATES = 0x005008,
    GET_SHOW_REFRESH_RATE_ENABLED = 0x005009,
    SET_SHOW_REFRESH_RATE_ENABLED = 0x00500A,
    GET_REALTIME_REFRESH_RATE = 0x00500B,
    GET_REFRESH_INFO = 0x00500C,
    REFRESH_RATE_UPDATE_CALLBACK = 0x00500D,
    REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK = 0x00500E,
    REFRESH_RATE_MODE_CHANGE_CALLBACK = 0x00500F,

    GET_SCREEN_ACTIVE_MODE = 0x006000,
    GET_SCREEN_SUPPORTED_MODES = 0x006001,
    GET_SCREEN_CAPABILITY = 0x006002,
    GET_SCREEN_POWER_STATUS = 0x006003,
    GET_SCREEN_BACK_LIGHT = 0x006004,
    GET_SCREEN_DATA = 0x006005,
    GET_SCREEN_SUPPORTED_GAMUTS = 0x006006,
    GET_SCREEN_SUPPORTED_METADATAKEYS = 0x006007,
    GET_SCREEN_GAMUT = 0x006008,
    GET_SCREEN_GAMUT_MAP = 0x006009,

    SET_SCREEN_GAMUT = 0x007000,
    SET_SCREEN_GAMUT_MAP = 0x007001,
    SET_SCREEN_CORRECTION = 0x007002,
    SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION = 0x007003,
    SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE = 0x007004,
    SET_GLOBAL_DARK_COLOR_MODE = 0x007005,
    
    CREATE_PIXEL_MAP_FROM_SURFACE = 0x008000,
    GET_SCREEN_HDR_CAPABILITY = 0x008001,
    SET_PIXEL_FORMAT = 0x008002,
    GET_PIXEL_FORMAT = 0x008003,
    GET_SCREEN_SUPPORTED_HDR_FORMATS = 0x008004,
    GET_SCREEN_HDR_FORMAT = 0x008005,
    SET_SCREEN_HDR_FORMAT = 0x008006,
    GET_SCREEN_SUPPORTED_COLORSPACES = 0x008007,
    GET_SCREEN_COLORSPACE = 0x008008,
    SET_SCREEN_COLORSPACE = 0x008009,
    GET_SCREEN_TYPE = 0x00800A,
    SET_SCREEN_SKIP_FRAME_INTERVAL = 0x00800B,

    GET_BITMAP = 0x00800C,
    GET_PIXELMAP = 0x00800D,

    SHOW_WATERMARK = 0x009000,
    SET_WATERMARK = 0x009001,

    GET_MEMORY_GRAPHIC = 0x00A000,
    GET_MEMORY_GRAPHICS = 0x00A001,
    GET_TOTAL_APP_MEM_SIZE = 0x00A002,

    REPORT_JANK_STATS = 0x00B000,
    REPORT_EVENT_RESPONSE = 0x00B001,
    REPORT_EVENT_COMPLETE = 0x00B002,
    REPORT_EVENT_JANK_FRAME = 0x00B003,
    REPORT_RS_SCENE_JANK_START = 0x00B004,
    REPORT_RS_SCENE_JANK_END = 0x00B005,
    REPORT_EVENT_GAMESTATE = 0x00B006,

    EXECUTE_SYNCHRONOUS_TASK = 0x00C000,
    NOTIFY_TOUCH_EVENT = 0x00C001,
    SET_HARDWARE_ENABLED = 0x00C002,

    REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK = 0x00D000,
    UNREGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK = 0x00D001,

    REGISTER_TYPEFACE = 0x00E000,
    UNREGISTER_TYPEFACE = 0x00E001,
    NEED_REGISTER_TYPEFACE = 0x00E002,

    GET_ACTIVE_DIRTY_REGION_INFO = 0x00F000,
    GET_GLOBAL_DIRTY_REGION_INFO = 0x00F001,

    GET_DISPLAY_IDENTIFICATION_DATA = 0x010000,
#ifdef RS_ENABLE_OVERLAY_DISPLAY
    SET_OVERLAY_DISPLAY_MODE = 0x010001,
#endif
// Special invocation. Do not change it.
    NOTIFY_LIGHT_FACTOR_STATUS = 1000,
    NOTIFY_PACKAGE_EVENT = 1001,
    NOTIFY_REFRESH_RATE_EVENT = 1002,
    NOTIFY_DYNAMIC_MODE_EVENT = 1003,
    NOTIFY_SOFT_VSYNC_EVENT = 1004,
    NOTIFY_APP_STRATEGY_CONFIG_CHANGE_EVENT = 1005,
    NOTIFY_HGMCONFIG_EVENT = 1006,
    NOTIFY_SCREEN_SWITCHED = 1007,
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_IRENDER_SERVICE_CONNECTION_INTERFACE_CODE_H
