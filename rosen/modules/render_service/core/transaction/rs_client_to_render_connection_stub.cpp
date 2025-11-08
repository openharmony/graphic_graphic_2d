/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "rs_client_to_render_connection_stub.h"
#include <memory>
#include <mutex>
#include "ivsync_connection.h"
#ifdef RES_SCHED_ENABLE
#include "res_sched_client.h"
#include "res_type.h"
#include <sched.h>
#endif
#include "securec.h"
#include "sys_binder.h"

#include "command/rs_command_factory.h"
#include "command/rs_command_verify_helper.h"
#include "common/rs_xcollie.h"
#include "hgm_frame_rate_manager.h"
#include "memory/rs_memory_flow_control.h"
#include "pipeline/render_thread/rs_base_render_util.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_ashmem_helper.h"
#include "transaction/rs_unmarshal_thread.h"
#include "transaction/rs_hrp_service.h"
#include "render/rs_typeface_cache.h"
#include "rs_trace.h"
#include "rs_profiler.h"
#include "app_mgr_client.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t MAX_DATA_SIZE_FOR_UNMARSHALLING_IN_PLACE = 1024 * 15; // 15kB
constexpr size_t FILE_DESCRIPTOR_LIMIT = 15;
constexpr size_t MAX_OBJECTNUM = 512;
constexpr size_t MAX_DATA_SIZE = 1024 * 1024; // 1MB
static constexpr int MAX_SECURITY_EXEMPTION_LIST_NUMBER = 1024; // securityExemptionList size not exceed 1024
const uint32_t MAX_VOTER_SIZE = 100;
constexpr uint32_t MAX_PID_SIZE_NUMBER = 100000;
#ifdef RES_SCHED_ENABLE
const uint32_t RS_IPC_QOS_LEVEL = 7;
constexpr const char* RS_BUNDLE_NAME = "render_service";
#endif
static constexpr std::array descriptorCheckList = {
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_FOCUS_APP_INFO),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_DEFAULT_SCREEN_ID),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ACTIVE_SCREEN_ID),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ALL_SCREEN_IDS),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_ROG_SCREEN_RESOLUTION),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ROG_SCREEN_RESOLUTION),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_PHYSICAL_SCREEN_RESOLUTION),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_RESOLUTION),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_SURFACE),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_BLACKLIST),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_TYPE_BLACKLIST),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::ADD_VIRTUAL_SCREEN_BLACKLIST),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REMOVE_VIRTUAL_SCREEN_BLACKLIST),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_SECURITY_MASK),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_MIRROR_SCREEN_VISIBLE_RECT),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REMOVE_VIRTUAL_SCREEN),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_BRIGHTNESS_INFO_CHANGE_CALLBACK),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_BRIGHTNESS_INFO),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_CHANGE_CALLBACK),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_SWITCHING_NOTIFY_CALLBACK),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_ACTIVE_MODE),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_REFRESH_RATE),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_REFRESH_RATE_MODE),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SYNC_FRAME_RATE_RANGE),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_FRAME_RATE_LINKER),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_CURRENT_REFRESH_RATE),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_CURRENT_REFRESH_RATE_MODE),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_REFRESH_RATES),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SHOW_REFRESH_RATE_ENABLED),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SHOW_REFRESH_RATE_ENABLED),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_REALTIME_REFRESH_RATE),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::MARK_POWER_OFF_NEED_PROCESS_ONE_FRAME),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPAINT_EVERYTHING),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::FORCE_REFRESH_ONE_FRAME_WITH_NEXT_VSYNC),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::DISABLE_RENDER_CONTROL_SCREEN),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_DISPLAY_IDENTIFICATION_DATA),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_REFRESH_INFO_TO_SP),
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_POINTER_COLOR_INVERSION_CONFIG),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_POINTER_COLOR_INVERSION_ENABLED),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_POINTER_LUMINANCE_CALLBACK),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_POINTER_LUMINANCE_CALLBACK),
#endif
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_POWER_STATUS),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_BACK_LIGHT),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_ACTIVE_MODE),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_MODES),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_CAPABILITY),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_POWER_STATUS),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_BACK_LIGHT),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_DATA),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_VIRTUAL_SCREEN_RESOLUTION),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_BUFFER_AVAILABLE_LISTENER),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_BUFFER_CLEAR_LISTENER),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_GAMUTS),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_METADATAKEYS),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_GAMUT),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_GAMUT),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_GAMUT_MAP),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_CORRECTION),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_AUTO_ROTATION),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_GLOBAL_DARK_COLOR_MODE),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_GAMUT_MAP),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_PIXEL_MAP_FROM_SURFACE),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_HDR_CAPABILITY),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_PIXEL_FORMAT),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PIXEL_FORMAT),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_HDR_FORMATS),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_HDR_FORMAT),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_HDR_FORMAT),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_HDR_STATUS),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_COLORSPACES),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_COLORSPACE),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_COLORSPACE),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_TYPE),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_SKIP_FRAME_INTERVAL),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_REFRESH_RATE),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_ACTIVE_RECT),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_OFFSET),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_FRAME_GRAVITY),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_OCCLUSION_CHANGE_CALLBACK),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_APP_WINDOW_NUM),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SYSTEM_ANIMATED_SCENES),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_WATERMARK),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SURFACE_WATERMARK),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CLEAR_SURFACE_WATERMARK_FOR_NODES),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CLEAR_SURFACE_WATERMARK),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SHOW_WATERMARK),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::RESIZE_VIRTUAL_SCREEN),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_MEMORY_GRAPHIC),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_MEMORY_GRAPHICS),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_TOTAL_APP_MEM_SIZE),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_JANK_STATS),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_BITMAP),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PIXELMAP),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::EXECUTE_SYNCHRONOUS_TASK),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_LIGHT_FACTOR_STATUS),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_PACKAGE_EVENT),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_APP_STRATEGY_CONFIG_CHANGE_EVENT),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_REFRESH_RATE_EVENT),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_WINDOW_EXPECTED_BY_VSYNC_NAME),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_WINDOW_EXPECTED_BY_WINDOW_ID),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_SOFT_VSYNC_EVENT),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_SOFT_VSYNC_RATE_DISCOUNT_EVENT),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_RESPONSE),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_COMPLETE),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_JANK_FRAME),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_RS_SCENE_JANK_START),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_RS_SCENE_JANK_END),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_GAMESTATE),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_TOUCH_EVENT),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_DYNAMIC_MODE_EVENT),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_HGMCONFIG_EVENT),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_XCOMPONENT_EXPECTED_FRAMERATE),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_HARDWARE_ENABLED),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_HIDE_PRIVACY_CONTENT),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_HGM_CFG_CALLBACK),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_ROTATION_CACHE_ENABLED),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_SCREEN_SWITCHED),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_TP_FEATURE_CONFIG),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_USING_STATUS),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REFRESH_RATE_MODE_CHANGE_CALLBACK),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_CURTAIN_SCREEN_USING_STATUS),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::DROP_FRAME_BY_PID),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_TYPEFACE),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_TYPEFACE),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_SHARED_TYPEFACE),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REFRESH_RATE_UPDATE_CALLBACK),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::ON_FIRST_FRAME_COMMIT),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ACTIVE_DIRTY_REGION_INFO),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_GLOBAL_DIRTY_REGION_INFO),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_LAYER_COMPOSE_INFO),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_CAST_SCREEN_ENABLE_SKIP_WINDOW),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_UIEXTENSION_CALLBACK),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VMA_CACHE_STATUS),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_STATUS),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_ANCO_FORCE_DO_DIRECT),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NEED_REGISTER_TYPEFACE),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_DISPLAY_NODE),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_FREE_MULTI_WINDOW_STATUS),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_SURFACE_BUFFER_CALLBACK),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_SURFACE_BUFFER_CALLBACK),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_LAYER_TOP_FOR_HARDWARE_COMPOSER),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_LAYER_TOP),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_FORCE_REFRESH),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_COLOR_FOLLOW),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_WINDOW_CONTAINER),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_TRANSACTION_DATA_CALLBACK),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::AVCODEC_VIDEO_START),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::AVCODEC_VIDEO_STOP),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_GPU_CRC_DIRTY_ENABLED_PIDLIST),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_OPTIMIZE_CANVAS_DIRTY_ENABLED_PIDLIST),
#ifdef RS_ENABLE_OVERLAY_DISPLAY
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_OVERLAY_DISPLAY_MODE),
#endif
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_PAGE_NAME),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_BEHIND_WINDOW_FILTER_ENABLED),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_BEHIND_WINDOW_FILTER_ENABLED),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PID_GPU_MEMORY_IN_MB),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::PROFILER_SERVICE_OPEN_FILE),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::PROFILER_SERVICE_POPULATE_FILES),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::PROFILER_IS_SECURE_SCREEN),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_SURFACE_CAPTURE_WITH_ALL_WINDOWS),
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::FREEZE_SCREEN),
};

void CopyFileDescriptor(MessageParcel& old, MessageParcel& copied)
{
    binder_size_t* object = reinterpret_cast<binder_size_t*>(old.GetObjectOffsets());
    binder_size_t* copiedObject = reinterpret_cast<binder_size_t*>(copied.GetObjectOffsets());

    size_t objectNum = old.GetOffsetsSize();

    uintptr_t data = old.GetData();
    uintptr_t copiedData = copied.GetData();

    for (size_t i = 0; i < objectNum; i++) {
        const flat_binder_object* flat = reinterpret_cast<flat_binder_object*>(data + object[i]);
        flat_binder_object* copiedFlat = reinterpret_cast<flat_binder_object*>(copiedData + copiedObject[i]);

        if (flat->hdr.type == BINDER_TYPE_FD && flat->handle >= 0) {
            int32_t val = dup(flat->handle);
            if (val < 0) {
                ROSEN_LOGW("CopyFileDescriptor dup failed, fd:%{public}d, handle:%{public}" PRIu32, val,
                    static_cast<uint32_t>(flat->handle));
            }
            copiedFlat->handle = static_cast<uint32_t>(val);
        }
    }
}

std::shared_ptr<MessageParcel> CopyParcelIfNeed(MessageParcel& old, pid_t callingPid)
{
    if (RSSystemProperties::GetCacheEnabledForRotation() &&
        RSMainThread::Instance()->GetDesktopPidForRotationScene() != callingPid) {
        return nullptr;
    }
    auto dataSize = old.GetDataSize();
    if (dataSize <= MAX_DATA_SIZE_FOR_UNMARSHALLING_IN_PLACE && old.GetOffsetsSize() < FILE_DESCRIPTOR_LIMIT) {
        return nullptr;
    }
    if (dataSize > MAX_DATA_SIZE) {
        return nullptr;
    }
    if (dataSize == 0) {
        return nullptr;
    }

    if (old.GetOffsetsSize() > MAX_OBJECTNUM) {
        ROSEN_LOGW("RSRenderServiceConnectionStub::CopyParcelIfNeed failed, parcel fdCnt: %{public}zu is too large",
            old.GetOffsetsSize());
        return nullptr;
    }

    RS_TRACE_NAME("CopyParcelForUnmarsh: size:" + std::to_string(dataSize));
    void* base = malloc(dataSize);
    if (base == nullptr) {
        RS_LOGE("RSRenderServiceConnectionStub::CopyParcelIfNeed malloc failed");
        return nullptr;
    }
    if (memcpy_s(base, dataSize, reinterpret_cast<void*>(old.GetData()), dataSize) != 0) {
        RS_LOGE("RSRenderServiceConnectionStub::CopyParcelIfNeed copy parcel data failed");
        free(base);
        return nullptr;
    }

    auto parcelCopied = RS_PROFILER_COPY_PARCEL(old);
    if (!parcelCopied->ParseFrom(reinterpret_cast<uintptr_t>(base), dataSize)) {
        RS_LOGE("RSRenderServiceConnectionStub::CopyParcelIfNeed ParseFrom failed");
        free(base);
        return nullptr;
    }

    auto objectNum = old.GetOffsetsSize();
    if (objectNum != 0) {
        parcelCopied->InjectOffsets(old.GetObjectOffsets(), objectNum);
        CopyFileDescriptor(old, *parcelCopied);
    }
    int32_t data{0};
    if (!parcelCopied->ReadInt32(data)) {
        RS_LOGE("RSRenderServiceConnectionStub::CopyParcelIfNeed parcel data Read failed");
        return nullptr;
    }
    if (data != 0) {
        RS_LOGE("RSRenderServiceConnectionStub::CopyParcelIfNeed parcel data not match");
        return nullptr;
    }
    return parcelCopied;
}

bool CheckCreateNodeAndSurface(pid_t pid, RSSurfaceNodeType nodeType, SurfaceWindowType windowType)
{
    constexpr int nodeTypeMin = static_cast<int>(RSSurfaceNodeType::DEFAULT);
    constexpr int nodeTypeMax = static_cast<int>(RSSurfaceNodeType::NODE_MAX);

    int typeNum = static_cast<int>(nodeType);
    if (typeNum < nodeTypeMin || typeNum > nodeTypeMax) {
        RS_LOGW("CREATE_NODE_AND_SURFACE invalid RSSurfaceNodeType");
        return false;
    }  
    if (windowType != SurfaceWindowType::DEFAULT_WINDOW && !IS_SCB_WINDOW_TYPE(windowType)) {
        RS_LOGW("CREATE_NODE_AND_SURFACE invalid SurfaceWindowType");
        return false;
    }

    bool isTokenTypeValid = true;
    bool isNonSystemAppCalling = false;
    RSInterfaceCodeAccessVerifierBase::GetAccessType(isTokenTypeValid, isNonSystemAppCalling);
    if (isNonSystemAppCalling) {
        if (nodeType != RSSurfaceNodeType::DEFAULT &&
            nodeType != RSSurfaceNodeType::APP_WINDOW_NODE &&
            nodeType != RSSurfaceNodeType::SELF_DRAWING_NODE &&
            nodeType != RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE) {
            RS_LOGW("CREATE_NODE_AND_SURFACE NonSystemAppCalling invalid RSSurfaceNodeType %{public}d, pid %d",
                typeNum, pid);
            return false;
        }
        if (windowType != SurfaceWindowType::DEFAULT_WINDOW) {
            RS_LOGW("CREATE_NODE_AND_SURFACE NonSystemAppCalling invalid SurfaceWindowType %{public}d, pid %d",
                static_cast<int>(windowType), pid);
            return false;
        }
    }

    return true;
}

std::string GetBundleName(pid_t pid)
{
    std::string bundleName;
    static const auto appMgrClient = std::makie_shared<AppExecFwk::AppMgrClient>();
    if (appMgrClient == nullptr) {
        RS_LOGE("GetBundleName get appMgrClient fail");
        return bundleName;
    }
    int32_t uid = 0;
    appMgrClient->GetBundleNameByPid(pid, bundleName, uid);
    return bundleName;
}

bool IsValidCallingPid(pid_t pid, pid_t callingPid)
{
    return (callingPid == getpid()) || (callingPid == pid);
}

static void TypefaceXcollieCallback(void* arg)
{
    if (arg) {
        bool* isTrigger = static_cast<bool*>(arg);
        *isTrigger = true;
    }
}
}

void RSRenderServiceConnectionStub::SetQos()
{
#ifdef RES_SCHED_ENABLE
    std::string strBundleName = RS_BUNDLE_NAME;
    std::string strPid = std::to_string(getpid());
    std::string strTid = std::to_string(gettid());
    std::string strQos = std::to_string(RS_IPC_QOS_LEVEL);
    std::unordered_map<std::string, std::string> mapPayload;
    mapPayload["pid"] = strPid;
    mapPayload[strTid] = strQos;
    mapPayload["bundleName"] = strBundleName;
    OHOS::ResourceSchedule::ResSchedClient::GetInstance().ReportData(
        OHOS::ResourceSchedule::ResType::RES_TYPE_THREAD_QOS_CHANGE, 0, mapPayload);
    struct sched_param param = {0};
    param.sched_priority = 1;
    if (sched_setscheduler(0, SCHED_FIFO, &param) != 0) {
        RS_LOGE("RSRenderServiceConnectionStub Couldn't set SCHED_FIFO.");
    } else {
        RS_LOGI("RSRenderServiceConnectionStub set SCHED_FIFO succeed.");
    }
#endif
}

int RSRenderServiceConnectionStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    uint32_t parcelNumber = RS_PROFILER_ON_REMOTE_REQUEST(this, code, data, reply, option);

    AshmemFdContainer::SetIsUnmarshalThread(false);
    pid_t callingPid = GetCallingPid();
    RSMarshallingHelper::SetCallingPid(callingPid);
    auto tid = gettid();
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (tids_.find(tid) == tids_.end()) {
            SetQos();
            tids_.insert(tid);
        }
    }
    if (std::find(std::cbegin(descriptorCheckList), std::cend(descriptorCheckList), code) !=
        std::cend(descriptorCheckList)) {
        auto token = data.ReadInterfaceToken();
        if (token != RSIRenderServiceConnection::GetDescriptor()) {
            if (code == static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_PIXEL_MAP_FROM_SURFACE)) {
                if (!reply.WriteInt32(0)) {
                    RS_LOGE("RSRenderServiceConnectionStub::OnRemoteRequest Write failed.");
                    return ERR_INVALID_REPLY;
                }
            }
            return ERR_INVALID_STATE;
        }
    }
    auto accessible = securityManager_.IsInterfaceCodeAccessible(code);
    if (!accessible && code != static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_SURFACE_CAPTURE) &&
        code != static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_MEMORY_GRAPHIC) &&
        code != static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_REFRESH_INFO) &&
        code != static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_BUFFER_AVAILABLE_LISTENER) &&
        code != static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_BUFFER_CLEAR_LISTENER) &&
        code != static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_SURFACE_CAPTURE_WITH_ALL_WINDOWS)) {
        RS_LOGE("RSRenderServiceConnectionStub::OnRemoteRequest no permission code:%{public}u", code);
        return ERR_INVALID_STATE;
    }
    int ret = ERR_NONE;
    switch (code) {
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_FOCUS_APP_INFO): {
            int32_t pid{0};
            if (!data.ReadInt32(pid)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_FOCUS_APP_INFO read pid failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            RS_PROFILER_PATCH_PID(data, pid);
            int32_t uid{0};
            std::string bundleName;
            std::string abilityName;
            uint64_t focusNodeId{0};
            if (!data.ReadInt32(uid) || !data.ReadString(bundleName) || !data.ReadString(abilityName) ||
                !RSMarshallingHelper::UnmarshallingPidPlusId(data, focusNodeId)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_FOCUS_APP_INFO read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            FocusAppInfo info = {
                .pid = pid,
                .uid = uid,
                .bundleName = bundleName,
                .abilityName = abilityName,
                .focusNodeId = focusNodeId};
            int32_t repCode;
            if (SetFocusAppInfo(info, repCode) != ERR_OK || !reply.WriteInt32(repCode)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_FOCUS_APP_INFO Write status failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::FORCE_REFRESH_ONE_FRAME_WITH_NEXT_VSYNC): {
            ForceRefreshOneFrameWithNextVSync();
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_SURFACE_CAPTURE): {
            NodeId id{0};
            if (!RSMarshallingHelper::UnmarshallingPidPlusId(data, id)) {
                RS_LOGE("RSRenderServiceConnectionStub::TAKE_SURFACE_CAPTURE Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                ret = ERR_NULL_OBJECT;
                RS_LOGE("RSRenderServiceConnectionStub::TakeSurfaceCapture remoteObject is nullptr");
                break;
            }
            sptr<RSISurfaceCaptureCallback> cb = iface_cast<RSISurfaceCaptureCallback>(remoteObject);
            if (cb == nullptr) {
                ret = ERR_NULL_OBJECT;
                RS_LOGE("RSRenderServiceConnectionStub::TakeSurfaceCapture cb is nullptr");
                break;
            }
            RSSurfaceCaptureConfig captureConfig;
            RSSurfaceCaptureBlurParam blurParam;
            Drawing::Rect specifiedAreaRect;
            if (!ReadSurfaceCaptureConfig(captureConfig, data)) {
                ret = ERR_INVALID_DATA;
                RS_LOGE("RSRenderServiceConnectionStub::TakeSurfaceCapture read captureConfig failed");
                break;
            }
            if (!ReadSurfaceCaptureBlurParam(blurParam, data)) {
                ret = ERR_INVALID_DATA;
                RS_LOGE("RSRenderServiceConnectionStub::TakeSurfaceCapture read blurParam failed");
                break;
            }
            if (!ReadSurfaceCaptureAreaRect(specifiedAreaRect, data)) {
                ret = ERR_INVALID_DATA;
                RS_LOGE("RSRenderServiceConnectionStub::TakeSurfaceCapture read specifiedAreaRect failed");
                break;
            }

            RSSurfaceCapturePermissions permissions;
            permissions.screenCapturePermission = accessible;
            permissions.isSystemCalling = RSInterfaceCodeAccessVerifierBase::IsSystemCalling(
                RSIRenderServiceConnectionInterfaceCodeAccessVerifier::codeEnumTypeName_ + "::TAKE_SURFACE_CAPTURE");
            // Since GetCallingPid interface always returns 0 in asynchronous binder in Linux kernel system,
            // we temporarily add a white list to avoid abnormal functionality or abnormal display.
            // The white list will be removed after GetCallingPid interface can return real PID.
            permissions.selfCapture = (ExtractPid(id) == callingPid || callingPid == 0);
            TakeSurfaceCapture(id, cb, captureConfig, blurParam, specifiedAreaRect, permissions);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_SURFACE_CAPTURE_SOLO): {
            NodeId id{0};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::TAKE_SURFACE_CAPTURE_SOLO Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            RSSurfaceCaptureConfig captureConfig;
            if (!ReadSurfaceCaptureConfig(captureConfig, data)) {
                ret = ERR_INVALID_DATA;
                RS_LOGE("RSRenderServiceConnectionStub::TAKE_SURFACE_CAPTURE_SOLO read captureConfig failed");
                break;
            }
            RSSurfaceCapturePermissions permissions;
            permissions.isSystemCalling = RSInterfaceCodeAccessVerifierBase::IsSystemCalling(
                RSIRenderServiceConnectionInterfaceCodeAccessVerifier::codeEnumTypeName_ +
                "::TAKE_SURFACE_CAPTURE_SOLO");
            permissions.selfCapture = ExtractPid(id) == callingPid;
            std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> pixelMapIdPairVector;
            pixelMapIdPairVector = TakeSurfaceCaptureSoloNode(id, captureConfig, permissions);
            if (!RSMarshallingHelper::Marshalling(reply, pixelMapIdPairVector)) {
                ret = ERR_INVALID_REPLY;
                RS_LOGE("RSRenderServiceConnectionStub::TAKE_SURFACE_CAPTURE_SOLO Marshalling failed");
                break;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_SELF_SURFACE_CAPTURE): {
            NodeId id{0};
            if (!data.ReadUint64(id)) {
                ret = ERR_INVALID_DATA;
                break;
            }
            if (ExtractPid(id) != callingPid) {
                RS_LOGW("RSRenderServiceConnectionStub::TakeSelfSurfaceCapture failed, nodeId:[%{public}" PRIu64
                        "], callingPid:[%{public}d], pid:[%{public}d]", id, callingPid, ExtractPid(id));
                ret = ERR_INVALID_DATA;
                break;
            }
            RS_PROFILER_PATCH_NODE_ID(data, id);
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                ret = ERR_NULL_OBJECT;
                RS_LOGE("RSRenderServiceConnectionStub::TakeSelfSurfaceCapture remoteObject is nullptr");
                break;
            }
            sptr<RSISurfaceCaptureCallback> cb = iface_cast<RSISurfaceCaptureCallback>(remoteObject);
            if (cb == nullptr) {
                ret = ERR_NULL_OBJECT;
                RS_LOGE("RSRenderServiceConnectionStub::TakeSelfSurfaceCapture cb is nullptr");
                break;
            }
            RSSurfaceCaptureConfig captureConfig;
            if (!ReadSurfaceCaptureConfig(captureConfig, data)) {
                ret = ERR_INVALID_DATA;
                RS_LOGE("RSRenderServiceConnectionStub::TakeSelfSurfaceCapture read captureConfig failed");
                break;
            }
            TakeSelfSurfaceCapture(id, cb, captureConfig);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_UI_CAPTURE_IN_RANGE): {
            NodeId id{0};
            if (!RSMarshallingHelper::UnmarshallingPidPlusId(data, id)) {
                RS_LOGE("RSRenderServiceConnectionStub::TAKE_UI_CAPTURE_IN_RANGE Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                ret = ERR_NULL_OBJECT;
                RS_LOGE("RSRenderServiceConnectionStub::TAKE_UI_CAPTURE_IN_RANGE remoteObject is nullptr");
                break;
            }
            sptr<RSISurfaceCaptureCallback> cb = iface_cast<RSISurfaceCaptureCallback>(remoteObject);
            if (cb == nullptr) {
                ret = ERR_NULL_OBJECT;
                RS_LOGE("RSRenderServiceConnectionStub::TAKE_UI_CAPTURE_IN_RANGE cb is nullptr");
                break;
            }
            RSSurfaceCaptureConfig captureConfig;
            if (!ReadSurfaceCaptureConfig(captureConfig, data)) {
                ret = ERR_INVALID_DATA;
                RS_LOGE("RSRenderServiceConnectionStub::TAKE_UI_CAPTURE_IN_RANGE read captureConfig failed");
                break;
            }
            TakeUICaptureInRange(id, cb, captureConfig);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_WINDOW_FREEZE_IMMEDIATELY): {
            NodeId id{0};
            if (!RSMarshallingHelper::UnmarshallingPidPlusId(data, id)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_WINDOW_FREEZE_IMMEDIATELY Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            bool isFreeze {false};
            if (!data.ReadBool(isFreeze)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_WINDOW_FREEZE_IMMEDIATELY Read isFreeze failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            sptr<RSISurfaceCaptureCallback> cb;
            RSSurfaceCaptureConfig captureConfig;
            RSSurfaceCaptureBlurParam blurParam;
            if (isFreeze) {
                auto remoteObject = data.ReadRemoteObject();
                if (remoteObject == nullptr) {
                    ret = ERR_NULL_OBJECT;
                    RS_LOGE("RSRenderServiceConnectionStub::SET_WINDOW_FREEZE_IMMEDIATELY remoteObject is nullptr");
                    break;
                }
                cb = iface_cast<RSISurfaceCaptureCallback>(remoteObject);
                if (cb == nullptr) {
                    ret = ERR_NULL_OBJECT;
                    RS_LOGE("RSRenderServiceConnectionStub::SET_WINDOW_FREEZE_IMMEDIATELY cb is nullptr");
                    break;
                }
                if (!ReadSurfaceCaptureConfig(captureConfig, data)) {
                    ret = ERR_INVALID_DATA;
                    RS_LOGE("RSRenderServiceConnectionStub::SET_WINDOW_FREEZE_IMMEDIATELY write captureConfig failed");
                    break;
                }
                if (!ReadSurfaceCaptureBlurParam(blurParam, data)) {
                    ret = ERR_INVALID_DATA;
                    RS_LOGE("RSRenderServiceConnectionStub::TakeSurfaceCapture read blurParam failed");
                    break;
                }
            }
            SetWindowFreezeImmediately(id, isFreeze, cb, captureConfig, blurParam);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_SURFACE_CAPTURE_WITH_ALL_WINDOWS): {
            NodeId id { 0 };
            if (!RSMarshallingHelper::UnmarshallingPidPlusId(data, id)) {
                RS_LOGE("RSRenderServiceConnectionStub::TAKE_SURFACE_CAPTURE_WITH_ALL_WINDOWS read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            bool checkDrmAndSurfaceLock { false };
            if (!data.ReadBool(checkDrmAndSurfaceLock)) {
                RS_LOGE("RSRenderServiceConnectionStub::TAKE_SURFACE_CAPTURE_WITH_ALL_WINDOWS \
                    read checkDrmAndSurfaceLock failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            sptr<RSISurfaceCaptureCallback> cb;
            RSSurfaceCaptureConfig captureConfig;
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                ret = ERR_NULL_OBJECT;
                RS_LOGE(
                    "RSRenderServiceConnectionStub::TAKE_SURFACE_CAPTURE_WITH_ALL_WINDOWS remoteObject is nullptr");
                break;
            }
            cb = iface_cast<RSISurfaceCaptureCallback>(remoteObject);
            if (cb == nullptr) {
                ret = ERR_NULL_OBJECT;
                RS_LOGE("RSRenderServiceConnectionStub::TAKE_SURFACE_CAPTURE_WITH_ALL_WINDOWS cb is nullptr");
                break;
            }
            if (!ReadSurfaceCaptureConfig(captureConfig, data)) {
                ret = ERR_INVALID_DATA;
                RS_LOGE(
                    "RSRenderServiceConnectionStub::TAKE_SURFACE_CAPTURE_WITH_ALL_WINDOWS read captureConfig failed");
                break;
            }
            RSSurfaceCapturePermissions permissions;
            permissions.screenCapturePermission = accessible;
            permissions.isSystemCalling = RSInterfaceCodeAccessVerifierBase::IsSystemCalling(
                RSIRenderServiceConnectionInterfaceCodeAccessVerifier::codeEnumTypeName_ + \
                "::TAKE_SURFACE_CAPTURE_WITH_ALL_WINDOWS");
            ret = TakeSurfaceCaptureWithAllWindows(id, cb, captureConfig, checkDrmAndSurfaceLock, permissions);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::FREEZE_SCREEN): {
            NodeId id { 0 };
            if (!RSMarshallingHelper::UnmarshallingPidPlusId(data, id)) {
                RS_LOGE("RSRenderServiceConnectionStub::FREEZE_SCREEN read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            bool isFreeze { false };
            if (!data.ReadBool(isFreeze)) {
                RS_LOGE("RSRenderServiceConnectionStub::FREEZE_SCREEN read isFreeze failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            ret = FreezeScreen(id, isFreeze);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_POINTER_POSITION): {
            NodeId id { 0 };
            if (!RSMarshallingHelper::UnmarshallingPidPlusId(data, id)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_POINTER_POSITION read nodeId failed!");
                break;
            }
            float positionX { 0.f };
            float positionY { 0.f };
            float positionZ { 0.f };
            float positionW { 0.f };
            if (!data.ReadFloat(positionX) || !data.ReadFloat(positionY) || !data.ReadFloat(positionZ) ||
                !data.ReadFloat(positionW)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_POINTER_POSITION read position failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            SetHwcNodeBounds(id, positionX, positionY, positionZ, positionW);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_HDR_STATUS): {
            ScreenId id{INVALID_SCREEN_ID};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_HDR_STATUS Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            HdrStatus hdrStatus;
            int32_t resCode;
            ret = GetScreenHDRStatus(id, hdrStatus, resCode);
            if (ret != ERR_OK) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_HDR_STATUS Business error(%{public}d)!", ret);
                resCode = ret;
            }
            if (!reply.WriteInt32(resCode)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_HDR_STATUS Write resCode failed!");
                ret = ERR_INVALID_REPLY;
                break;
            }
            if (resCode != StatusCode::SUCCESS) {
                break;
            }
            if (!reply.WriteUint32(hdrStatus)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_HDR_STATUS Write hdrStatus failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::DROP_FRAME_BY_PID) : {
            std::vector<int32_t> pidList;
            if (!data.ReadInt32Vector(&pidList)) {
                RS_LOGE("RSRenderServiceConnectionStub::DROP_FRAME_BY_PID Read "
                        "pidList failed!");
                ret = ERR_INVALID_REPLY;
                break;
            }
            if (pidList.size() > MAX_DROP_FRAME_PID_LIST_SIZE) {
                ret = ERR_INVALID_DATA;
                break;
            }
            DropFrameByPid(pidList);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_ANCO_FORCE_DO_DIRECT) : {
            bool direct{false};
            if (!data.ReadBool(direct)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_ANCO_FORCE_DO_DIRECT Read direct failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            bool res;
            if (SetAncoForceDoDirect(direct, res) != ERR_OK || !reply.WriteBool(res)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_ANCO_FORCE_DO_DIRECT Write result failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_SURFACE_BUFFER_CALLBACK) : {
            int32_t pid{0};
            uint64_t uid{0};
            if (!data.ReadInt32(pid) ||
                !data.ReadUint64(uid)) {
                RS_LOGE("RSRenderServiceConnectionStub::REGISTER_SURFACE_BUFFER_CALLBACK Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            if (!IsValidCallingPid(pid, callingPid)) {
                RS_LOGW("REGISTER_SURFACE_BUFFER_CALLBACK invalid pid[%{public}d]", callingPid);
                ret = ERR_INVALID_DATA;
                break;
            }
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                ret = ERR_NULL_OBJECT;
                RS_LOGE("RSRenderServiceConnectionStub::OnRemoteRequest remoteObject == nullptr");
                break;
            }
            sptr<RSISurfaceBufferCallback> callback = iface_cast<RSISurfaceBufferCallback>(remoteObject);
            if (callback == nullptr) {
                ret = ERR_NULL_OBJECT;
                RS_LOGE("RSRenderServiceConnectionStub::OnRemoteRequest remoteObject cast error");
                break;
            }
            RegisterSurfaceBufferCallback(pid, uid, callback);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_SURFACE_BUFFER_CALLBACK) : {
            int32_t pid{0};
            uint64_t uid{0};
            if (!data.ReadInt32(pid) ||
                !data.ReadUint64(uid)) {
                RS_LOGE("RSRenderServiceConnectionStub::UNREGISTER_SURFACE_BUFFER_CALLBACK Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            if (!IsValidCallingPid(pid, callingPid)) {
                RS_LOGW("UNREGISTER_SURFACE_BUFFER_CALLBACK invalid pid[%{public}d]", callingPid);
                ret = ERR_INVALID_DATA;
                break;
            }
            UnregisterSurfaceBufferCallback(pid, uid);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_LAYER_TOP_FOR_HARDWARE_COMPOSER) : {
            NodeId nodeId = {};
            bool isTop{false};
            uint32_t topLayerZOrder = 0;
            if (!data.ReadUint64(nodeId) || !data.ReadBool(isTop) || !data.ReadUint32(topLayerZOrder)) {
                RS_LOGE("RSRenderServiceConntionStub::SET_LAYER_TOP_FOR_HARDWARE_COMPOSER Read parcel failed");
                ret = ERR_INVALID_DATA;
                break;
            }
            SetLayerTopForHWC(nodeId, isTop, topLayerZOrder);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_WINDOW_CONTAINER) : {
            NodeId nodeId = {};
            bool isEnabled = {};
            if (!data.ReadUint64(nodeId) || !data.ReadBool(isEnabled)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_WINDOW_CONTAINER Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            if (!IsValidCallingPid(ExtractPid(nodeId), callingPid)) {
                RS_LOGW("SET_WINDOW_CONTAINER invalid nodeId[%{public}" PRIu64 "] pid[%{public}d]", nodeId, callingPid);
                ret = ERR_INVALID_DATA;
                break;
            }
            SetWindowContainer(nodeId, isEnabled);
            break;
        }
        case static_cast<uint32_t>(
            RSIRenderServiceConnectionInterfaceCode::REGISTER_TRANSACTION_DATA_CALLBACK): {
            uint64_t token = data.ReadUint64();
            uint64_t timeStamp = data.ReadUint64();
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                ret = ERR_NULL_OBJECT;
                RS_LOGE("RSRenderServiceConnectionStub::OnRemoteRequest remoteObject == nullptr");
                break;
            }
            sptr<RSITransactionDataCallback> callback =
                iface_cast<RSITransactionDataCallback>(remoteObject);
            if (callback == nullptr) {
                ret = ERR_NULL_OBJECT;
                RS_LOGE("RSRenderServiceConnectionStub::OnRemoteRequest callback == nullptr");
                break;
            }
            RS_LOGD("RSRenderServiceConnectionStub: already decode unicode, timeStamp: %{public}"
                PRIu64 " token: %{public}" PRIu64, timeStamp, token);
            RegisterTransactionDataCallback(token, timeStamp, callback);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CLEAR_UIFIRST_CACHE) : {
            NodeId nodeId = {};
            if (!data.ReadUint64(nodeId)) {
                RS_LOGE("RSRenderServiceConnectionStub::CLEAR_UIFIRST_CACHE : read data err!");
                ret = ERR_INVALID_DATA;
                break;
            }
            ClearUifirstCache(nodeId);
            break;
        }
        default: {
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }

    return ret;
}

bool RSRenderServiceConnectionStub::ReadDataBaseRs(DataBaseRs& info, MessageParcel& data)
{
    if (!data.ReadInt32(info.appPid) || !data.ReadInt32(info.eventType) ||
        !data.ReadInt32(info.versionCode) || !data.ReadInt64(info.uniqueId) ||
        !data.ReadInt64(info.inputTime) || !data.ReadInt64(info.beginVsyncTime) ||
        !data.ReadInt64(info.endVsyncTime) || !data.ReadBool(info.isDisplayAnimator) ||
        !data.ReadString(info.sceneId) || !data.ReadString(info.versionName) ||
        !data.ReadString(info.bundleName) || !data.ReadString(info.processName) ||
        !data.ReadString(info.abilityName) ||!data.ReadString(info.pageUrl) ||
        !data.ReadString(info.sourceType) || !data.ReadString(info.note)) {
        RS_LOGE("RSRenderServiceConnectionStub::ReadDataBaseRs Read parcel failed!");
        return false;
    }
    return true;
}

bool RSRenderServiceConnectionStub::ReadSurfaceCaptureConfig(RSSurfaceCaptureConfig& captureConfig, MessageParcel& data)
{
    uint8_t captureType { 0 };
    if (!data.ReadFloat(captureConfig.scaleX) || !data.ReadFloat(captureConfig.scaleY) ||
        !data.ReadBool(captureConfig.useDma) || !data.ReadBool(captureConfig.useCurWindow) ||
        !data.ReadUint8(captureType) || !data.ReadBool(captureConfig.isSync) ||
        !data.ReadBool(captureConfig.isHdrCapture) ||
        !data.ReadBool(captureConfig.needF16WindowCaptureForScRGB) ||
        !data.ReadFloat(captureConfig.mainScreenRect.left_) ||
        !data.ReadFloat(captureConfig.mainScreenRect.top_) ||
        !data.ReadFloat(captureConfig.mainScreenRect.right_) ||
        !data.ReadFloat(captureConfig.mainScreenRect.bottom_) ||
        !data.ReadUint64(captureConfig.uiCaptureInRangeParam.endNodeId) ||
        !data.ReadBool(captureConfig.uiCaptureInRangeParam.useBeginNodeSize) ||
        !data.ReadFloat(captureConfig.specifiedAreaRect.left_) ||
        !data.ReadFloat(captureConfig.specifiedAreaRect.top_) ||
        !data.ReadFloat(captureConfig.specifiedAreaRect.right_) ||
        !data.ReadFloat(captureConfig.specifiedAreaRect.bottom_) ||
        !data.ReadUInt64Vector(&captureConfig.blackList) ||
        !data.ReadUint32(captureConfig.backGroundColor)) {
        RS_LOGE("RSRenderServiceConnectionStub::ReadSurfaceCaptureConfig Read captureConfig failed!");
        return false;
    }
    if (captureType >= static_cast<uint8_t>(SurfaceCaptureType::SURFACE_CAPTURE_TYPE_BUTT)) {
        RS_LOGE("RSRenderServiceConnectionStub::ReadSurfaceCaptureConfig Read captureType failed!");
        return false;
    }
    captureConfig.captureType = static_cast<SurfaceCaptureType>(captureType);
    return true;
}

bool RSRenderServiceConnectionStub::ReadSurfaceCaptureBlurParam(
    RSSurfaceCaptureBlurParam& blurParam, MessageParcel& data)
{
    if (!data.ReadBool(blurParam.isNeedBlur) || !data.ReadFloat(blurParam.blurRadius)) {
        RS_LOGE("RSRenderServiceConnectionStub::ReadSurfaceCaptureBlurParam Read blurParam failed!");
        return false;
    }
    return true;
}

bool RSRenderServiceConnectionStub::ReadSurfaceCaptureAreaRect(
    Drawing::Rect& specifiedAreaRect, MessageParcel& data)
{
    if (!data.ReadFloat(specifiedAreaRect.left_) || !data.ReadFloat(specifiedAreaRect.top_) ||
        !data.ReadFloat(specifiedAreaRect.right_) || !data.ReadFloat(specifiedAreaRect.bottom_)) {
        RS_LOGE("RSRenderServiceConnectionStub::ReadSurfaceCaptureAreaRect Read specifiedAreaRect failed!");
        return false;
    }
    return true;
}

const RSInterfaceCodeSecurityManager RSRenderServiceConnectionStub::securityManager_ = \
    RSInterfaceCodeSecurityManager::CreateInstance<RSIRenderServiceConnectionInterfaceCodeAccessVerifier>();
} // namespace Rosen
} // namespace OHOS
