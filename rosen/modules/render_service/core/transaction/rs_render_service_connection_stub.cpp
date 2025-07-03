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

#include "rs_render_service_connection_stub.h"
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
constexpr int MAX_PID_SIZE_NUMBER = 100000;
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
    static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_CHANGE_CALLBACK),
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
    static const auto appMgrClient = std::make_shared<AppExecFwk::AppMgrClient>();
    if (appMgrClient == nullptr) {
        RS_LOGE("GetBundleName get appMgrClient fail.");
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
        code != static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_BUFFER_CLEAR_LISTENER)) {
        RS_LOGE("RSRenderServiceConnectionStub::OnRemoteRequest no permission code:%{public}d", code);
        return ERR_INVALID_STATE;
    }
    int ret = ERR_NONE;
    switch (code) {
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::COMMIT_TRANSACTION): {
            bool isTokenTypeValid = true;
            bool isNonSystemAppCalling = false;
            RSInterfaceCodeAccessVerifierBase::GetAccessType(isTokenTypeValid, isNonSystemAppCalling);
            if (!isTokenTypeValid) {
                RS_LOGE("RSRenderServiceConnectionStub::COMMIT_TRANSACTION invalid token type");
                return ERR_INVALID_STATE;
            }
            if (isNonSystemAppCalling) {
                RsCommandVerifyHelper::GetInstance().RegisterNonSystemPid(callingPid);
            }
            RS_TRACE_NAME_FMT("Recv Parcel Size:%zu, fdCnt:%zu", data.GetDataSize(), data.GetOffsetsSize());
            static bool isUniRender = RSUniRenderJudgement::IsUniRender();
            std::shared_ptr<MessageParcel> parsedParcel;
            std::unique_ptr<AshmemFdWorker> ashmemFdWorker = nullptr;
            std::shared_ptr<AshmemFlowControlUnit> ashmemFlowControlUnit = nullptr;
            int32_t readData{0};
            if (!data.ReadInt32(readData)) {
                RS_LOGE("RSRenderServiceConnectionStub::COMMIT_TRANSACTION read parcel failed");
                return ERR_INVALID_DATA;
            }
            if (readData == 0) { // indicate normal parcel
                if (isUniRender) {
                    // in uni render mode, if parcel size over threshold,
                    // Unmarshalling task will be post to RSUnmarshalThread,
                    // copy the origin parcel to maintain the parcel lifetime
                    parsedParcel = CopyParcelIfNeed(data, callingPid);
                }
                if (parsedParcel == nullptr) {
                    // no need to copy or copy failed, use original parcel
                    // execute Unmarshalling immediately

                    RSMarshallingHelper::UnmarshallingTransactionVer(data);

                    auto transactionData = RSBaseRenderUtil::ParseTransactionData(data, parcelNumber);
                    if (transactionData && isNonSystemAppCalling) {
                        const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
                        if (!transactionData->IsCallingPidValid(callingPid, nodeMap)) {
                            RS_LOGE("RSRenderServiceConnectionStub::COMMIT_TRANSACTION IsCallingPidValid check failed");
                        }
                    }
                    CommitTransaction(transactionData);
                    break;
                }
            } else {
                // indicate ashmem parcel
                // should be parsed to normal parcel before Unmarshalling
                parsedParcel = RSAshmemHelper::ParseFromAshmemParcel(&data, ashmemFdWorker, ashmemFlowControlUnit,
                    callingPid);
                if (parsedParcel) {
                    parcelNumber = RS_PROFILER_ON_REMOTE_REQUEST(this, code, *parsedParcel, reply, option);
                }
            }
            if (parsedParcel == nullptr) {
                RS_LOGE("RSRenderServiceConnectionStub::COMMIT_TRANSACTION failed: parsed parcel is nullptr");
                return ERR_INVALID_DATA;
            }
            RSMarshallingHelper::UnmarshallingTransactionVer(*parsedParcel);
            if (isUniRender) {
                // post Unmarshalling task to RSUnmarshalThread
                RSUnmarshalThread::Instance().RecvParcel(parsedParcel, isNonSystemAppCalling, callingPid,
                    std::move(ashmemFdWorker), ashmemFlowControlUnit, parcelNumber);
            } else {
                // execute Unmarshalling immediately
                auto transactionData = RSBaseRenderUtil::ParseTransactionData(*parsedParcel, parcelNumber);
                if (transactionData && isNonSystemAppCalling) {
                    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
                    if (!transactionData->IsCallingPidValid(callingPid, nodeMap)) {
                        RS_LOGE("RSRenderServiceConnectionStub::COMMIT_TRANSACTION IsCallingPidValid check failed");
                    }
                }
                CommitTransaction(transactionData);
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_UNI_RENDER_ENABLED): {
            bool enable;
            if (GetUniRenderEnabled(enable) != ERR_OK || !reply.WriteBool(enable)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_UNI_RENDER_ENABLED read enable failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_NODE): {
            uint64_t nodeId{0};
            if (!data.ReadUint64(nodeId)) {
                RS_LOGE("RSRenderServiceConnectionStub::CREATE_NODE read nodeId failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            bool isNonSystemCalling = false;
            bool isTokenTypeValid = true;
            RSInterfaceCodeAccessVerifierBase::GetAccessType(isTokenTypeValid, isNonSystemCalling);
            if (isNonSystemCalling && !IsValidCallingPid(ExtractPid(nodeId), callingPid)) {
                RS_LOGW("CREATE_NODE invalid nodeId[%{public}" PRIu64 "] pid[%{public}d]", nodeId, callingPid);
                ret = ERR_INVALID_DATA;
                break;
            }
            RS_PROFILER_PATCH_NODE_ID(data, nodeId);
            std::string surfaceName;
            if (!data.ReadString(surfaceName)) {
                RS_LOGE("RSRenderServiceConnectionStub::CREATE_NODE read surfaceName failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            RSSurfaceRenderNodeConfig config = {.id = nodeId, .name = surfaceName};
            bool success;
            if (CreateNode(config, success) != ERR_OK || !reply.WriteBool(success)) {
                RS_LOGE("RSRenderServiceConnectionStub::CREATE_NODE Write success failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_NODE_AND_SURFACE): {
            uint64_t nodeId{0};
            if (!data.ReadUint64(nodeId)) {
                RS_LOGE("RSRenderServiceConnectionStub::CREATE_NODE_AND_SURFACE read nodeId failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            if (!IsValidCallingPid(ExtractPid(nodeId), callingPid)) {
                RS_LOGW("CREATE_NODE_AND_SURFACE invalid nodeId[%" PRIu64 "] pid[%d]", nodeId, callingPid);
                ret = ERR_INVALID_DATA;
                break;
            }
            RS_PROFILER_PATCH_NODE_ID(data, nodeId);
            std::string surfaceName;
            uint8_t type { 0 };
            bool isTextureExportNode { false };
            bool isSync { false };
            uint8_t surfaceWindowType { 0 };
            bool unobscured { false };
            if (!data.ReadString(surfaceName) || !data.ReadUint8(type) || !data.ReadBool(isTextureExportNode) ||
                !data.ReadBool(isSync) || !data.ReadUint8(surfaceWindowType) || !data.ReadBool(unobscured)) {
                RS_LOGE("RSRenderServiceConnectionStub::CREATE_NODE_AND_SURFACE read surfaceRenderNodeConfig failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            if (!CheckCreateNodeAndSurface(callingPid, static_cast<RSSurfaceNodeType>(type),
                                           static_cast<SurfaceWindowType>(surfaceWindowType))) {
                RS_LOGE("RSRenderServiceConnectionStub::CREATE_NODE_AND_SURFACE CheckCreateNodeAndSurface failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            bool needGetBundleName = (type == static_cast<uint8_t>(RSSurfaceNodeType::SELF_DRAWING_NODE));
            RSSurfaceRenderNodeConfig config = { .id = nodeId,
                .name = surfaceName,
                .nodeType = static_cast<RSSurfaceNodeType>(type),
                .isTextureExportNode = isTextureExportNode,
                .isSync = isSync,
                .surfaceWindowType = static_cast<SurfaceWindowType>(surfaceWindowType),
                .bundleName = needGetBundleName ? GetBundleName(ExtractPid(nodeId)) : "" };
            sptr<Surface> surface = nullptr;
            ErrCode err = CreateNodeAndSurface(config, surface, unobscured);
            if ((err != ERR_OK) || (surface == nullptr)) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            auto producer = surface->GetProducer();
            if (!reply.WriteRemoteObject(producer->AsObject())) {
                RS_LOGE("RSRenderServiceConnectionStub::CREATE_NODE_AND_SURFACE read RemoteObject failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
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
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_DEFAULT_SCREEN_ID): {
            uint64_t screenId = INVALID_SCREEN_ID;
            if (GetDefaultScreenId(screenId) != ERR_OK || !reply.WriteUint64(screenId)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_DEFAULT_SCREEN_ID Write id failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ACTIVE_SCREEN_ID): {
            uint64_t screenId = INVALID_SCREEN_ID;
            if (GetActiveScreenId(screenId) != ERR_OK || !reply.WriteUint64(screenId)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_ACTIVE_SCREEN_ID Write id failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ALL_SCREEN_IDS): {
            std::vector<ScreenId> ids = GetAllScreenIds();
            if (!reply.WriteUint32(ids.size())) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_ALL_SCREEN_IDS Write idsSize failed!");
                ret = ERR_INVALID_REPLY;
                break;
            }
            for (uint32_t i = 0; i < ids.size(); i++) {
                if (!reply.WriteUint64(ids[i])) {
                    RS_LOGE("RSRenderServiceConnectionStub::GET_ALL_SCREEN_IDS Write ids failed!");
                    ret = ERR_INVALID_REPLY;
                    break;
                }
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN): {
            // read the parcel data.
            std::string name;
            uint32_t width{0};
            uint32_t height{0};
            bool useSurface{false};
            if (!data.ReadString(name) || !data.ReadUint32(width) ||
                !data.ReadUint32(height) || !data.ReadBool(useSurface)) {
                RS_LOGE("RSRenderServiceConnectionStub::CREATE_VIRTUAL_SCREEN read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            sptr<Surface> surface = nullptr;
            if (useSurface) {
                auto remoteObject = data.ReadRemoteObject();
                if (remoteObject != nullptr) {
                    auto bufferProducer = iface_cast<IBufferProducer>(remoteObject);
                    surface = Surface::CreateSurfaceAsProducer(bufferProducer);
                }
            }
            ScreenId mirrorId{INVALID_SCREEN_ID};
            int32_t flags{0};
            std::vector<NodeId> whiteList;
            if (!data.ReadUint64(mirrorId) || !data.ReadInt32(flags) || !data.ReadUInt64Vector(&whiteList)) {
                RS_LOGE("RSRenderServiceConnectionStub::CREATE_VIRTUAL_SCREEN read ScreenId failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            ScreenId id = CreateVirtualScreen(name, width, height, surface, mirrorId, flags, whiteList);
            if (!reply.WriteUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::CREATE_VIRTUAL_SCREEN Write id failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_BLACKLIST): {
            // read the parcel data.
            ScreenId id{INVALID_SCREEN_ID};
            std::vector<NodeId> blackListVector;
            if (!data.ReadUint64(id) || !data.ReadUInt64Vector(&blackListVector)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_VIRTUAL_SCREEN_BLACKLIST read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t status = SetVirtualScreenBlackList(id, blackListVector);
            if (!reply.WriteInt32(status)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_VIRTUAL_SCREEN_BLACKLIST Write status failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_TYPE_BLACKLIST): {
            // read the parcel data.
            ScreenId id{INVALID_SCREEN_ID};
            std::vector<NodeType> typeBlackListVector;
            if (!data.ReadUint64(id) || !data.ReadUInt8Vector(&typeBlackListVector)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_VIRTUAL_SCREEN_TYPE_BLACKLIST read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t repCode;
            SetVirtualScreenTypeBlackList(id, typeBlackListVector, repCode);
            if (!reply.WriteInt32(repCode)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_VIRTUAL_SCREEN_TYPE_BLACKLIST Write repCode failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::ADD_VIRTUAL_SCREEN_BLACKLIST): {
            // read the parcel data.
            ScreenId id{INVALID_SCREEN_ID};
            std::vector<NodeId> blackListVector;
            if (!data.ReadUint64(id) || !data.ReadUInt64Vector(&blackListVector)) {
                RS_LOGE("RSRenderServiceConnectionStub::ADD_VIRTUAL_SCREEN_BLACKLIST read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t repCode;
            AddVirtualScreenBlackList(id, blackListVector, repCode);
            if (!reply.WriteInt32(repCode)) {
                RS_LOGE("RSRenderServiceConnectionStub::ADD_VIRTUAL_SCREEN_BLACKLIST Write repCode failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REMOVE_VIRTUAL_SCREEN_BLACKLIST): {
            // read the parcel data.
            ScreenId id{INVALID_SCREEN_ID};
            std::vector<NodeId> blackListVector;
            if (!data.ReadUint64(id) || !data.ReadUInt64Vector(&blackListVector)) {
                RS_LOGE("RSRenderServiceConnectionStub::ADD_VIRTUAL_SCREEN_BLACKLIST read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t repCode;
            RemoveVirtualScreenBlackList(id, blackListVector, repCode);
            if (!reply.WriteInt32(repCode)) {
                RS_LOGE("RSRenderServiceConnectionStub::REMOVE_VIRTUAL_SCREEN_BLACKLIST Write repCode failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(
            RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST): {
            // read the parcel data.
            ScreenId id{INVALID_SCREEN_ID};
            std::vector<NodeId> securityExemptionList;
            if (!data.ReadUint64(id) || !data.ReadUInt64Vector(&securityExemptionList)) {
                RS_LOGE(
                    "RSRenderServiceConnectionStub::SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            if (securityExemptionList.size() > MAX_SECURITY_EXEMPTION_LIST_NUMBER) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST"
                    " failed: too many lists.");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t status = SetVirtualScreenSecurityExemptionList(id, securityExemptionList);
            if (!reply.WriteInt32(status)) {
                RS_LOGE(
                    "RSRenderServiceConnectionStub::SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST Write status failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(
            RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_SECURITY_MASK): {
            // read the parcel data.
            ScreenId id{INVALID_SCREEN_ID};
            bool enable{false};
            if (!data.ReadUint64(id) || !data.ReadBool(enable)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_SCREEN_SECURITY_MASK read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            std::shared_ptr<Media::PixelMap> securityMask{nullptr};
            if (enable) {
                securityMask = std::shared_ptr<Media::PixelMap>(data.ReadParcelable<Media::PixelMap>());
            }
            int32_t result = SetScreenSecurityMask(id, std::move(securityMask));
            if (!reply.WriteInt32(result)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_SCREEN_SECURITY_MASK Write result failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(
            RSIRenderServiceConnectionInterfaceCode::SET_MIRROR_SCREEN_VISIBLE_RECT): {
            // read the parcel data.
            ScreenId id = INVALID_SCREEN_ID;
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_MIRROR_SCREEN_VISIBLE_RECT Read id failed!");
                ret = ERR_INVALID_REPLY;
                break;
            }
            int32_t x = -1;
            int32_t y = -1;
            int32_t w = -1;
            int32_t h = -1;
            if (!data.ReadInt32(x) || !data.ReadInt32(y) ||
                !data.ReadInt32(w) || !data.ReadInt32(h)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_MIRROR_SCREEN_VISIBLE_RECT Read parcel failed!");
                ret = ERR_INVALID_REPLY;
                break;
            }
            auto mainScreenRect = Rect {
                .x = x,
                .y = y,
                .w = w,
                .h = h
            };
            bool supportRotation{false};
            if (!data.ReadBool(supportRotation)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_MIRROR_SCREEN_VISIBLE_RECT Read supportRotation failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t status = SetMirrorScreenVisibleRect(id, mainScreenRect, supportRotation);
            if (!reply.WriteInt32(status)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_MIRROR_SCREEN_VISIBLE_RECT Write status failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_CAST_SCREEN_ENABLE_SKIP_WINDOW): {
            // read the parcel data.
            ScreenId id{INVALID_SCREEN_ID};
            bool enable{false};
            if (!data.ReadUint64(id) || !data.ReadBool(enable)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_CAST_SCREEN_ENABLE_SKIP_WINDOW Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t result = SetCastScreenEnableSkipWindow(id, enable);
            if (!reply.WriteInt32(result)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_MIRROR_SCREEN_VISIBLE_RECT Write result failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_SURFACE): {
            // read the parcel data.
            ScreenId id{INVALID_SCREEN_ID};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_VIRTUAL_SCREEN_SURFACE Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            auto bufferProducer = iface_cast<IBufferProducer>(remoteObject);
            sptr<Surface> surface = Surface::CreateSurfaceAsProducer(bufferProducer);
            if (surface == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            int32_t status = SetVirtualScreenSurface(id, surface);
            if (!reply.WriteInt32(status)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_VIRTUAL_SCREEN_SURFACE Write status failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REMOVE_VIRTUAL_SCREEN): {
            ScreenId id{INVALID_SCREEN_ID};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::REMOVE_VIRTUAL_SCREEN Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            RemoveVirtualScreen(id);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_CHANGE_CALLBACK): {
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_SCREEN_CHANGE_CALLBACK remoteObject is nullptr");
                ret = ERR_NULL_OBJECT;
                break;
            }
            sptr<RSIScreenChangeCallback> cb = iface_cast<RSIScreenChangeCallback>(remoteObject);
            if (cb == nullptr) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_SCREEN_CHANGE_CALLBACK callback is nullptr");
                ret = ERR_NULL_OBJECT;
                break;
            }
            RS_LOGI("RSRenderServiceConnectionStub::SET_SCREEN_CHANGE_CALLBACK");
            int32_t status = SetScreenChangeCallback(cb);
            if (!reply.WriteInt32(status)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_SCREEN_CHANGE_CALLBACK Write status failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_POINTER_COLOR_INVERSION_CONFIG): {
            float darkBuffer { 0.f };
            float brightBuffer { 0.f };
            int64_t interval { 0 };
            int32_t rangeSize { 0 };
            if (!data.ReadFloat(darkBuffer) || !data.ReadFloat(brightBuffer) || !data.ReadInt64(interval) ||
                !data.ReadInt32(rangeSize)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_POINTER_COLOR_INVERSION_CONFIG read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t status = SetPointerColorInversionConfig(darkBuffer, brightBuffer, interval, rangeSize);
            if (!reply.WriteInt32(status)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_POINTER_COLOR_INVERSION_CONFIG Write status failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_POINTER_COLOR_INVERSION_ENABLED): {
            bool enable { false };
            if (!data.ReadBool(enable)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_POINTER_COLOR_INVERSION_ENABLED read enable failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t status = SetPointerColorInversionEnabled(enable);
            if (!reply.WriteInt32(status)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_POINTER_COLOR_INVERSION_ENABLED Write status failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_POINTER_LUMINANCE_CALLBACK): {
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            sptr<RSIPointerLuminanceChangeCallback> cb = iface_cast<RSIPointerLuminanceChangeCallback>(remoteObject);
            if (cb == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            int32_t status = RegisterPointerLuminanceChangeCallback(cb);
            if (!reply.WriteInt32(status)) {
                RS_LOGE("RSRenderServiceConnectionStub::REGISTER_POINTER_LUMINANCE_CALLBACK Write status failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_POINTER_LUMINANCE_CALLBACK): {
            int32_t status = UnRegisterPointerLuminanceChangeCallback();
            if (!reply.WriteInt32(status)) {
                RS_LOGE("RSRenderServiceConnectionStub::UNREGISTER_POINTER_LUMINANCE_CALLBACK Write status failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
#endif
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_ACTIVE_MODE): {
            ScreenId id{INVALID_SCREEN_ID};
            uint32_t modeId{0};
            if (!data.ReadUint64(id) || !data.ReadUint32(modeId)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_SCREEN_ACTIVE_MODE Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            SetScreenActiveMode(id, modeId);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_REFRESH_RATE): {
            ScreenId id{INVALID_SCREEN_ID};
            int32_t sceneId{0};
            int32_t rate{0};
            if (!data.ReadUint64(id) || !data.ReadInt32(sceneId) || !data.ReadInt32(rate)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_SCREEN_REFRESH_RATE Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            SetScreenRefreshRate(id, sceneId, rate);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_REFRESH_RATE_MODE): {
            int32_t mode{0};
            if (!data.ReadInt32(mode)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_REFRESH_RATE_MODE Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            SetRefreshRateMode(mode);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SYNC_FRAME_RATE_RANGE): {
            FrameRateLinkerId id{0};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::SYNC_FRAME_RATE_RANGE Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            if (ExtractPid(id) != callingPid) {
                RS_LOGW("The SyncFrameRateRange isn't legal, frameRateLinkerId: %{public}" PRIu64
                    ", callingPid:%{public}d", id, callingPid);
                ret = ERR_INVALID_DATA;
                break;
            }
            uint32_t min{0};
            uint32_t max{0};
            uint32_t preferred{0};
            uint32_t type{0};
            uint32_t componentScene{0};
            int32_t animatorExpectedFrameRate{0};
            if (!data.ReadUint32(min) || !data.ReadUint32(max) || !data.ReadUint32(preferred) ||
                !data.ReadUint32(type) || !data.ReadUint32(componentScene) ||
                !data.ReadInt32(animatorExpectedFrameRate)) {
                RS_LOGE("RSRenderServiceConnectionStub::SYNC_FRAME_RATE_RANGE Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            SyncFrameRateRange(id, {min, max, preferred, type, static_cast<ComponentScene>(componentScene)},
                animatorExpectedFrameRate);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_FRAME_RATE_LINKER): {
            FrameRateLinkerId id{0};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::UNREGISTER_FRAME_RATE_LINKER Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            if (ExtractPid(id) != callingPid) {
                RS_LOGW("The UnregisterFrameRateLinker isn't legal, frameRateLinkerId: %{public}" PRIu64
                    ", callingPid:%{public}d", id, callingPid);
                ret = ERR_INVALID_DATA;
                break;
            }
            UnregisterFrameRateLinker(id);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_CURRENT_REFRESH_RATE): {
            ScreenId id{INVALID_SCREEN_ID};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_CURRENT_REFRESH_RATE Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            uint32_t refreshRate = GetScreenCurrentRefreshRate(id);
            if (!reply.WriteUint32(refreshRate)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_CURRENT_REFRESH_RATE Write refreshRate failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_CURRENT_REFRESH_RATE_MODE): {
            int32_t refreshRateMode = GetCurrentRefreshRateMode();
            if (!reply.WriteInt32(refreshRateMode)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_CURRENT_REFRESH_RATE_MODE Write refreshRateMode failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_REFRESH_RATES): {
            ScreenId id{INVALID_SCREEN_ID};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_SUPPORTED_REFRESH_RATES Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            std::vector<int32_t> rates = GetScreenSupportedRefreshRates(id);
            if (!reply.WriteUint64(static_cast<uint64_t>(rates.size()))) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_SUPPORTED_REFRESH_RATES Write rates failed!");
                ret = ERR_INVALID_REPLY;
                break;
            }
            for (auto ratesIter : rates) {
                if (!reply.WriteInt32(ratesIter)) {
                    RS_LOGE(
                        "RSRenderServiceConnectionStub::GET_SCREEN_SUPPORTED_REFRESH_RATES Write ratesIter failed!");
                    ret = ERR_INVALID_REPLY;
                    break;
                }
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SHOW_REFRESH_RATE_ENABLED): {
            bool enabled = false;
            if (GetShowRefreshRateEnabled(enabled) != ERR_OK || !reply.WriteBool(enabled)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SHOW_REFRESH_RATE_ENABLED Write enabled failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SHOW_REFRESH_RATE_ENABLED): {
            bool enabled{false};
            int32_t type{0};
            if (!data.ReadBool(enabled) || !data.ReadInt32(type)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SHOW_REFRESH_RATE_ENABLED Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            SetShowRefreshRateEnabled(enabled, type);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_REALTIME_REFRESH_RATE): {
            ScreenId id{INVALID_SCREEN_ID};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_REALTIME_REFRESH_RATE Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            uint32_t refreshRate = GetRealtimeRefreshRate(id);
            if (!reply.WriteUint32(refreshRate)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_REALTIME_REFRESH_RATE Read refreshRate failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_REFRESH_INFO): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            int32_t pid{0};
            if (!data.ReadInt32(pid)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_REFRESH_INFO Read pid failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            if (!IsValidCallingPid(pid, callingPid)) {
                RS_LOGW("GET_REFRESH_INFO invalid pid[%{public}d]", callingPid);
                ret = ERR_INVALID_DATA;
                break;
            }
            std::string refreshInfo;
            if (GetRefreshInfo(pid, refreshInfo) != ERR_OK || !reply.WriteString(refreshInfo)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_REFRESH_INFO Write refreshInfo failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }

        case static_cast<uint64_t>(RSIRenderServiceConnectionInterfaceCode::GET_REFRESH_INFO_TO_SP): {
            uint64_t nodeId{0};
            if (!data.ReadUint64(nodeId)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_REFRESH_INFO_TO_SP Read nodeId failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            std::string refreshInfoToSP;
            if (GetRefreshInfoToSP(nodeId, refreshInfoToSP) != ERR_OK || !reply.WriteString(refreshInfoToSP)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_REFRESH_INFO_TO_SP Write refreshInfoToSP failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_PHYSICAL_SCREEN_RESOLUTION): {
            ScreenId id{INVALID_SCREEN_ID};
            uint32_t width{0};
            uint32_t height{0};
            if (!data.ReadUint64(id) || !data.ReadUint32(width) || !data.ReadUint32(height)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_PHYSICAL_SCREEN_RESOLUTION Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t status = SetPhysicalScreenResolution(id, width, height);
            if (!reply.WriteInt32(status)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_PHYSICAL_SCREEN_RESOLUTION Write status failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_RESOLUTION): {
            ScreenId id{INVALID_SCREEN_ID};
            uint32_t width{0};
            uint32_t height{0};
            if (!data.ReadUint64(id) || !data.ReadUint32(width) || !data.ReadUint32(height)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_VIRTUAL_SCREEN_RESOLUTION Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t status = SetVirtualScreenResolution(id, width, height);
            if (!reply.WriteInt32(status)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_VIRTUAL_SCREEN_RESOLUTION Write status failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_DISPLAY_IDENTIFICATION_DATA): {
            ScreenId id{INVALID_SCREEN_ID};
            uint8_t outPort{0};
            std::vector<uint8_t> edidData{};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_DISPLAY_IDENTIFICATION_DATA Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t result = GetDisplayIdentificationData(id, outPort, edidData);
            if (!reply.WriteUint8(result)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_DISPLAY_IDENTIFICATION_DATA Write result failed!");
                ret = IPC_STUB_WRITE_PARCEL_ERR;
                break;
            }
            if (result != SUCCESS) {
                break;
            }
            if (!reply.WriteUint8(outPort) ||
                !reply.WriteUint32(static_cast<uint32_t>(edidData.size())) ||
                !reply.WriteBuffer(edidData.data(), edidData.size())) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_DISPLAY_IDENTIFICATION_DATA Write parcel failed!");
                ret = IPC_STUB_WRITE_PARCEL_ERR;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::MARK_POWER_OFF_NEED_PROCESS_ONE_FRAME): {
            MarkPowerOffNeedProcessOneFrame();
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPAINT_EVERYTHING): {
            RepaintEverything();
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::FORCE_REFRESH_ONE_FRAME_WITH_NEXT_VSYNC): {
            ForceRefreshOneFrameWithNextVSync();
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::DISABLE_RENDER_CONTROL_SCREEN): {
            ScreenId id{INVALID_SCREEN_ID};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::DISABLE_RENDER_CONTROL_SCREEN Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            DisablePowerOffRenderControl(id);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_POWER_STATUS): {
            ScreenId id{INVALID_SCREEN_ID};
            uint32_t status{0};
            if (!data.ReadUint64(id) || !data.ReadUint32(status)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_SCREEN_POWER_STATUS Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            SetScreenPowerStatus(id, static_cast<ScreenPowerStatus>(status));
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
            bool isFreeze{false};
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
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_APPLICATION_AGENT): {
            pid_t pid = GetCallingPid();
            RS_PROFILER_PATCH_PID(data, pid);
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            sptr<IApplicationAgent> app = iface_cast<IApplicationAgent>(remoteObject);
            if (app == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            RegisterApplicationAgent(pid, app);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_VIRTUAL_SCREEN_RESOLUTION): {
            ScreenId id{INVALID_SCREEN_ID};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_VIRTUAL_SCREEN_RESOLUTION Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            RSVirtualScreenResolution virtualScreenResolution = GetVirtualScreenResolution(id);
            if (!reply.WriteParcelable(&virtualScreenResolution)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_VIRTUAL_SCREEN_RESOLUTION Write virtualScreenResolution "
                        "failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_ACTIVE_MODE): {
            ScreenId id{INVALID_SCREEN_ID};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_ACTIVE_MODE Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            RSScreenModeInfo screenModeInfo;
            if (GetScreenActiveMode(id, screenModeInfo) != ERR_OK || !reply.WriteParcelable(&screenModeInfo)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_ACTIVE_MODE Write screenModeInfo failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_MODES): {
            ScreenId id{INVALID_SCREEN_ID};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_SUPPORTED_MODES Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            std::vector<RSScreenModeInfo> screenSupportedModes = GetScreenSupportedModes(id);
            if (!reply.WriteUint64(static_cast<uint64_t>(screenSupportedModes.size()))) {
                RS_LOGE(
                    "RSRenderServiceConnectionStub::GET_SCREEN_SUPPORTED_MODES Write screenSupportedModesSize failed!");
                ret = ERR_INVALID_REPLY;
                break;
            }
            for (uint32_t modeIndex = 0; modeIndex < screenSupportedModes.size(); modeIndex++) {
                if (!reply.WriteParcelable(&screenSupportedModes[modeIndex])) {
                    RS_LOGE(
                        "RSRenderServiceConnectionStub::GET_SCREEN_SUPPORTED_MODES Write screenSupportedModes failed!");
                    ret = ERR_INVALID_REPLY;
                    break;
                }
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_MEMORY_GRAPHIC): {
            int32_t pid{0};
            if (!data.ReadInt32(pid)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_MEMORY_GRAPHIC Read pid failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            RS_PROFILER_PATCH_PID(data, pid);
            if (!IsValidCallingPid(pid, callingPid)) {
                RS_LOGW("GET_MEMORY_GRAPHIC invalid pid[%{public}d]", callingPid);
                ret = ERR_INVALID_DATA;
                break;
            }
            MemoryGraphic memoryGraphic;
            if (GetMemoryGraphic(pid, memoryGraphic) != ERR_OK || !reply.WriteParcelable(&memoryGraphic)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_MEMORY_GRAPHIC Write memoryGraphic failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_MEMORY_GRAPHICS): {
            std::vector<MemoryGraphic> memoryGraphics;
            if (GetMemoryGraphics(memoryGraphics) != ERR_OK ||
                !reply.WriteUint64(static_cast<uint64_t>(memoryGraphics.size()))) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_MEMORY_GRAPHIC Write memoryGraphicsSize failed!");
                ret = ERR_INVALID_REPLY;
                break;
            }
            for (uint32_t index = 0; index < memoryGraphics.size(); index++) {
                if (!reply.WriteParcelable(&memoryGraphics[index])) {
                    RS_LOGE("RSRenderServiceConnectionStub::GET_MEMORY_GRAPHIC Write memoryGraphics failed!");
                    ret = ERR_INVALID_REPLY;
                    break;
                }
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_TOTAL_APP_MEM_SIZE): {
            float cpuMemSize = 0.f;
            float gpuMemSize = 0.f;
            if (GetTotalAppMemSize(cpuMemSize, gpuMemSize) != ERR_OK || !reply.WriteFloat(cpuMemSize)
                || !reply.WriteFloat(gpuMemSize)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_TOTAL_APP_MEM_SIZE Write parcel failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_CAPABILITY): {
            ScreenId id{INVALID_SCREEN_ID};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_CAPABILITY Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            RSScreenCapability screenCapability = GetScreenCapability(id);
            if (!reply.WriteParcelable(&screenCapability)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_CAPABILITY Write screenCapability failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_POWER_STATUS): {
            ScreenId id{INVALID_SCREEN_ID};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_POWER_STATUS Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            uint32_t powerStatus{static_cast<uint32_t>(INVALID_POWER_STATUS)};
            if (GetScreenPowerStatus(id, powerStatus) != ERR_OK || !reply.WriteUint32(powerStatus)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_POWER_STATUS Write status failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_DATA): {
            ScreenId id{INVALID_SCREEN_ID};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_DATA Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            RSScreenData screenData = GetScreenData(id);
            if (!reply.WriteParcelable(&screenData)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_DATA Write screenData failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_BACK_LIGHT): {
            ScreenId id{INVALID_SCREEN_ID};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_BACK_LIGHT Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t backLightLevel{static_cast<int32_t>(INVALID_BACKLIGHT_VALUE)};
            if (GetScreenBacklight(id, backLightLevel) != ERR_OK || !reply.WriteInt32(backLightLevel)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_BACK_LIGHT Write level failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_BACK_LIGHT): {
            ScreenId id{INVALID_SCREEN_ID};
            uint32_t level{0};
            if (!data.ReadUint64(id) || !data.ReadUint32(level)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_SCREEN_BACK_LIGHT Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            SetScreenBacklight(id, level);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_BUFFER_AVAILABLE_LISTENER): {
            NodeId id{0};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_BUFFER_AVAILABLE_LISTENER Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            if (!accessible && (ExtractPid(id) != callingPid)) {
                RS_LOGW("The SetBufferAvailableListener isn't legal, nodeId:%{public}" PRIu64 ", callingPid:%{public}d",
                    id, callingPid);
                break;
            }
            RS_PROFILER_PATCH_NODE_ID(data, id);
            auto remoteObject = data.ReadRemoteObject();
            bool isFromRenderThread{false};
            if (!data.ReadBool(isFromRenderThread)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_BUFFER_AVAILABLE_LISTENER read isFromRenderThread failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            if (remoteObject == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            sptr<RSIBufferAvailableCallback> cb = iface_cast<RSIBufferAvailableCallback>(remoteObject);
            if (cb == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            RegisterBufferAvailableListener(id, cb, isFromRenderThread);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_BUFFER_CLEAR_LISTENER): {
            NodeId id{0};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_BUFFER_CLEAR_LISTENER Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            if (!accessible && (ExtractPid(id) != callingPid)) {
                RS_LOGW("The SetBufferClearListener isn't legal, nodeId:%{public}" PRIu64 ", callingPid:%{public}d",
                    id, callingPid);
                break;
            }
            RS_PROFILER_PATCH_NODE_ID(data, id);
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            sptr<RSIBufferClearCallback> cb = iface_cast<RSIBufferClearCallback>(remoteObject);
            if (cb == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            RegisterBufferClearListener(id, cb);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_GAMUTS): {
            ScreenId id{INVALID_SCREEN_ID};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_SUPPORTED_GAMUTS Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            std::vector<uint32_t> modeSend;
            std::vector<ScreenColorGamut> mode;
            int32_t result = GetScreenSupportedColorGamuts(id, mode);
            if (!reply.WriteInt32(result)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_SUPPORTED_GAMUTS Write result failed!");
                ret = ERR_INVALID_REPLY;
                break;
            }
            if (result != StatusCode::SUCCESS) {
                ret = ERR_UNKNOWN_REASON;
                break;
            }
            std::copy(mode.begin(), mode.end(), std::back_inserter(modeSend));
            if (!reply.WriteUInt32Vector(modeSend)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_SUPPORTED_GAMUTS Write modeSend failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_METADATAKEYS): {
            ScreenId id{INVALID_SCREEN_ID};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_SUPPORTED_METADATAKEYS Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            std::vector<uint32_t> keySend;
            std::vector<ScreenHDRMetadataKey> keys;
            int32_t result = GetScreenSupportedMetaDataKeys(id, keys);
            if (!reply.WriteInt32(result)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_SUPPORTED_METADATAKEYS Write result failed!");
                ret = ERR_INVALID_REPLY;
                break;
            }
            if (result != StatusCode::SUCCESS) {
                ret = ERR_UNKNOWN_REASON;
                break;
            }
            for (auto i : keys) {
                keySend.push_back(i);
            }
            if (!reply.WriteUInt32Vector(keySend)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_SUPPORTED_METADATAKEYS Write keySend failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_GAMUT): {
            ScreenId id{INVALID_SCREEN_ID};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_GAMUT Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            ScreenColorGamut mode;
            int32_t result = GetScreenColorGamut(id, mode);
            if (!reply.WriteInt32(result)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_GAMUT Write result failed!");
                ret = ERR_INVALID_REPLY;
                break;
            }
            if (result != StatusCode::SUCCESS) {
                ret = ERR_UNKNOWN_REASON;
                break;
            }
            if (!reply.WriteUint32(mode)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_GAMUT Write mode failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_GAMUT): {
            ScreenId id{INVALID_SCREEN_ID};
            int32_t modeIdx{0};
            if (!data.ReadUint64(id) || !data.ReadInt32(modeIdx)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_SCREEN_GAMUT Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t result = SetScreenColorGamut(id, modeIdx);
            if (!reply.WriteInt32(result)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_SCREEN_GAMUT Write result failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_GAMUT_MAP): {
            ScreenId id{INVALID_SCREEN_ID};
            int32_t mode{0};
            if (!data.ReadUint64(id) || !data.ReadInt32(mode)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_SCREEN_GAMUT_MAP Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t result = SetScreenGamutMap(id, static_cast<ScreenGamutMap>(mode));
            if (!reply.WriteInt32(result)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_SCREEN_GAMUT_MAP Write parcel failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_CORRECTION): {
            ScreenId id{INVALID_SCREEN_ID};
            int32_t screenRotation{0};
            if (!data.ReadUint64(id) || !data.ReadInt32(screenRotation)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_SCREEN_CORRECTION Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t result = SetScreenCorrection(id, static_cast<ScreenRotation>(screenRotation));
            if (!reply.WriteInt32(result)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_SCREEN_CORRECTION Write parcel failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(
            RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION): {
            ScreenId id{INVALID_SCREEN_ID};
            bool canvasRotation{false};
            if (!data.ReadUint64(id) || !data.ReadBool(canvasRotation)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            bool result = SetVirtualMirrorScreenCanvasRotation(id, canvasRotation);
            if (!reply.WriteBool(result)) {
                RS_LOGE(
                    "RSRenderServiceConnectionStub::SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION Write parcel failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(
            RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_AUTO_ROTATION): {
            ScreenId id{INVALID_SCREEN_ID};
            bool isAutoRotation{false};
            if (!data.ReadUint64(id) || !data.ReadBool(isAutoRotation)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_VIRTUAL_SCREEN_AUTO_ROTATION Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t result = SetVirtualScreenAutoRotation(id, isAutoRotation);
            if (!reply.WriteInt32(result)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_VIRTUAL_SCREEN_AUTO_ROTATION Write parcel failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(
            RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE): {
            ScreenId id{INVALID_SCREEN_ID};
            uint32_t scaleMode{0};
            if (!data.ReadUint64(id) || !data.ReadUint32(scaleMode)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            bool result = SetVirtualMirrorScreenScaleMode(id, static_cast<ScreenScaleMode>(scaleMode));
            if (!reply.WriteBool(result)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE Write parcel failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(
            RSIRenderServiceConnectionInterfaceCode::SET_GLOBAL_DARK_COLOR_MODE): {
            bool isDark{false};
            if (!data.ReadBool(isDark)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_GLOBAL_DARK_COLOR_MODE Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            if (SetGlobalDarkColorMode(isDark) != ERR_OK) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_GLOBAL_DARK_COLOR_MODE Write result failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_GAMUT_MAP): {
            ScreenId id{INVALID_SCREEN_ID};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_GAMUT_MAP Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            ScreenGamutMap mode;
            int32_t result = GetScreenGamutMap(id, mode);
            if (!reply.WriteInt32(result)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_GAMUT_MAP Write result failed!");
                ret = ERR_INVALID_REPLY;
                break;
            }
            if (result != StatusCode::SUCCESS) {
                ret = ERR_UNKNOWN_REASON;
                break;
            }
            if (!reply.WriteUint32(mode)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_GAMUT_MAP Write mode failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VSYNC_CONNECTION): {
            std::string name;
            if (!data.ReadString(name)) {
                RS_LOGE("RSRenderServiceConnectionStub::CREATE_VSYNC_CONNECTION read name failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            auto remoteObj = data.ReadRemoteObject();
            uint64_t id{0};
            NodeId windowNodeID{0};
            if (!data.ReadUint64(id) || !data.ReadUint64(windowNodeID)) {
                RS_LOGE("RSRenderServiceConnectionStub::CREATE_VSYNC_CONNECTION read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            if (remoteObj == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            if (!remoteObj->IsProxyObject()) {
                ret = ERR_UNKNOWN_OBJECT;
                break;
            }
            auto token = iface_cast<VSyncIConnectionToken>(remoteObj);
            if (token == nullptr) {
                ret = ERR_UNKNOWN_OBJECT;
                break;
            }
            sptr<IVSyncConnection> conn = nullptr;
            VSyncConnParam vsyncConnParam = {id, windowNodeID, false};
            CreateVSyncConnection(conn, name, token, vsyncConnParam);
            if (conn == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
#ifdef ENABLE_IPC_SECURITY_ACCESS_COUNTER
            securityUtils_.IncreaseAccessCounter(code);
#endif
            if (!reply.WriteRemoteObject(conn->AsObject())) {
                RS_LOGE("RSRenderServiceConnectionStub::CREATE_VSYNC_CONNECTION Write Object failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PIXELMAP_BY_PROCESSID): {
            uint64_t pid;
            if (!data.ReadUint64(pid)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_PIXELMAP_BY_PROCESSID Read pid failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            std::vector<PixelMapInfo> pixelMapInfoVector;
            int32_t repCode;
            if (GetPixelMapByProcessId(pixelMapInfoVector, static_cast<pid_t>(pid), repCode) != ERR_OK ||
                !reply.WriteInt32(repCode)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_PIXELMAP_BY_PROCESSID Write repCode failed!");
                ret = ERR_INVALID_REPLY;
                break;
            }
            if (!RSMarshallingHelper::Marshalling(reply, pixelMapInfoVector)) {
                ret = ERR_INVALID_REPLY;
                break;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_PIXEL_MAP_FROM_SURFACE): {
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                if (!reply.WriteInt32(0)) {
                    RS_LOGE("RSRenderServiceConnectionStub::CREATE_PIXEL_MAP_FROM_SURFACE Write Object failed!");
                    ret = ERR_INVALID_REPLY;
                    break;
                }
                ret = ERR_NULL_OBJECT;
                break;
            }
            auto bufferProducer = iface_cast<IBufferProducer>(remoteObject);
            sptr<Surface> surface = Surface::CreateSurfaceAsProducer(bufferProducer);
            if (surface == nullptr) {
                if (!reply.WriteInt32(0)) {
                    RS_LOGE("RSRenderServiceConnectionStub::CREATE_PIXEL_MAP_FROM_SURFACE Write parcel failed!");
                    ret = ERR_INVALID_REPLY;
                    break;
                }
                ret = ERR_NULL_OBJECT;
                break;
            }
            int32_t x = 0;
            int32_t y = 0;
            int32_t w = 0;
            int32_t h = 0;
            if (!data.ReadInt32(x) || !data.ReadInt32(y) || !data.ReadInt32(w) || !data.ReadInt32(h)) {
                RS_LOGE("RSRenderServiceConnectionStub::CREATE_PIXEL_MAP_FROM_SURFACE Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            auto srcRect = Rect {
                .x = x,
                .y = y,
                .w = w,
                .h = h
            };
            std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
            CreatePixelMapFromSurface(surface, srcRect, pixelMap);
            if (pixelMap) {
                if (!reply.WriteBool(true)) {
                    RS_LOGE("RSRenderServiceConnectionStub::CREATE_PIXEL_MAP_FROM_SURFACE Read parcel failed");
                    ret = ERR_INVALID_REPLY;
                    break;
                }
                if (!pixelMap->Marshalling(reply)) {
                    RS_LOGE("pixelMap Marshalling fail");
                    ret = ERR_INVALID_REPLY;
                }
            } else {
                if (!reply.WriteBool(false)) {
                    RS_LOGE("RSRenderServiceConnectionStub::CREATE_PIXEL_MAP_FROM_SURFACE Read parcel failed!");
                    ret = ERR_INVALID_REPLY;
                    break;
                }
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_HDR_CAPABILITY): {
            ScreenId id{INVALID_SCREEN_ID};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_HDR_CAPABILITY Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            RSScreenHDRCapability screenHDRCapability;
            int32_t result = GetScreenHDRCapability(id, screenHDRCapability);
            if (!reply.WriteInt32(result)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_HDR_CAPABILITY Write result failed!");
                ret = ERR_INVALID_REPLY;
                break;
            }
            if (result != StatusCode::SUCCESS) {
                ret = ERR_UNKNOWN_REASON;
                break;
            }
            if (!reply.WriteParcelable(&screenHDRCapability)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_HDR_CAPABILITY Write screenHDRCapability failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PIXEL_FORMAT): {
            ScreenId id{INVALID_SCREEN_ID};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_PIXEL_FORMAT Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            GraphicPixelFormat pixelFormat;
            int32_t resCode;
            GetPixelFormat(id, pixelFormat, resCode);
            if (!reply.WriteInt32(resCode)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_PIXEL_FORMAT Write result failed!");
                ret = ERR_INVALID_REPLY;
                break;
            }
            if (resCode != StatusCode::SUCCESS) {
                break;
            }
            if (!reply.WriteUint32(static_cast<uint32_t>(pixelFormat))) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_PIXEL_FORMAT Write pixelFormat failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_PIXEL_FORMAT): {
            ScreenId id{INVALID_SCREEN_ID};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_PIXEL_FORMAT Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t pixel{0};
            if (!data.ReadInt32(pixel)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_PIXEL_FORMAT read pixelFormat failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            GraphicPixelFormat pixelFormat = static_cast<GraphicPixelFormat>(pixel);
            int32_t resCode;
            SetPixelFormat(id, pixelFormat, resCode);
            if (!reply.WriteInt32(resCode)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_PIXEL_FORMAT Write result failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_HDR_FORMATS): {
            ScreenId id{INVALID_SCREEN_ID};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_SUPPORTED_HDR_FORMATS Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            std::vector<uint32_t> hdrFormatsSend;
            std::vector<ScreenHDRFormat> hdrFormats;
            int32_t resCode;
            GetScreenSupportedHDRFormats(id, hdrFormats, resCode);
            if (!reply.WriteInt32(resCode)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_SUPPORTED_HDR_FORMATS Write result failed!");
                ret = ERR_INVALID_REPLY;
                break;
            }
            if (resCode != StatusCode::SUCCESS) {
                break;
            }
            std::copy(hdrFormats.begin(), hdrFormats.end(), std::back_inserter(hdrFormatsSend));
            if (!reply.WriteUInt32Vector(hdrFormatsSend)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_SUPPORTED_HDR_FORMATS Write hdrFormatsSend failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_HDR_FORMAT): {
            ScreenId id{INVALID_SCREEN_ID};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_HDR_FORMAT Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            ScreenHDRFormat hdrFormat;
            int32_t resCode;
            GetScreenHDRFormat(id, hdrFormat, resCode);
            if (!reply.WriteInt32(resCode)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_HDR_FORMAT Write resCode failed!");
                ret = ERR_INVALID_REPLY;
                break;
            }
            if (resCode != StatusCode::SUCCESS) {
                break;
            }
            if (!reply.WriteUint32(hdrFormat)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_HDR_FORMAT Write hdrFormat failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_HDR_FORMAT): {
            ScreenId id{INVALID_SCREEN_ID};
            int32_t modeIdx{0};
            if (!data.ReadUint64(id) || !data.ReadInt32(modeIdx)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_SCREEN_HDR_FORMAT Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t resCode;
            SetScreenHDRFormat(id, modeIdx, resCode);
            if (!reply.WriteInt32(resCode)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_SCREEN_HDR_FORMAT Write resCode failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_COLORSPACES): {
            ScreenId id{INVALID_SCREEN_ID};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_SUPPORTED_COLORSPACES Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            std::vector<uint32_t> colorSpacesSend;
            std::vector<GraphicCM_ColorSpaceType> colorSpaces;
            int32_t resCode;
            GetScreenSupportedColorSpaces(id, colorSpaces, resCode);
            if (!reply.WriteInt32(resCode)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_SUPPORTED_COLORSPACES Write resCode failed!");
                ret = ERR_INVALID_REPLY;
                break;
            }
            if (resCode != StatusCode::SUCCESS) {
                break;
            }
            std::copy(colorSpaces.begin(), colorSpaces.end(), std::back_inserter(colorSpacesSend));
            if (!reply.WriteUInt32Vector(colorSpacesSend)) {
                RS_LOGE(
                    "RSRenderServiceConnectionStub::GET_SCREEN_SUPPORTED_COLORSPACES Write colorSpacesSend failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_COLORSPACE): {
            ScreenId id{INVALID_SCREEN_ID};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_COLORSPACE Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            GraphicCM_ColorSpaceType colorSpace;
            int32_t resCode;
            GetScreenColorSpace(id, colorSpace, resCode);
            if (!reply.WriteInt32(resCode)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_COLORSPACE Write result failed!");
                ret = ERR_INVALID_REPLY;
                break;
            }
            if (resCode != StatusCode::SUCCESS) {
                break;
            }
            if (!reply.WriteUint32(colorSpace)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_COLORSPACE Write colorSpace failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_COLORSPACE): {
            ScreenId id{INVALID_SCREEN_ID};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_SCREEN_COLORSPACE Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t color{0};
            if (!data.ReadInt32(color)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_SCREEN_COLORSPACE read colorSpace failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            GraphicCM_ColorSpaceType colorSpace = static_cast<GraphicCM_ColorSpaceType>(color);
            int32_t resCode;
            SetScreenColorSpace(id, colorSpace, resCode);
            if (!reply.WriteInt32(resCode)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_SCREEN_COLORSPACE Write result failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_TYPE): {
            ScreenId id{INVALID_SCREEN_ID};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_TYPE Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            RSScreenType type;
            int32_t result = GetScreenType(id, type);
            if (!reply.WriteInt32(result)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_TYPE Write result failed!");
                ret = ERR_INVALID_REPLY;
                break;
            }
            if (result != StatusCode::SUCCESS) {
                ret = ERR_UNKNOWN_REASON;
                break;
            }
            if (!reply.WriteUint32(type)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_SCREEN_TYPE Write type failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_BITMAP): {
            NodeId id{0};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_BITMAP Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            if (!IsValidCallingPid(ExtractPid(id), callingPid)) {
                RS_LOGW("The GetBitmap isn't legal, nodeId:%{public}" PRIu64 ", callingPid:%{public}d",
                    id, callingPid);
                break;
            }
            RS_PROFILER_PATCH_NODE_ID(data, id);
            Drawing::Bitmap bm;
            bool success;
            if (GetBitmap(id, bm, success) != ERR_OK || !reply.WriteBool(success)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_BITMAP Write success failed!");
                ret = ERR_INVALID_REPLY;
                break;
            }
            if (success) {
                RSMarshallingHelper::Marshalling(reply, bm);
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PIXELMAP): {
            NodeId id{0};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_PIXELMAP Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            if (!IsValidCallingPid(ExtractPid(id), callingPid)) {
                RS_LOGW("The GetPixelmap isn't legal, nodeId:%{public}" PRIu64 ", callingPid:%{public}d",
                    id, callingPid);
                break;
            }
            RS_PROFILER_PATCH_NODE_ID(data, id);
            std::shared_ptr<Media::PixelMap> pixelmap =
                std::shared_ptr<Media::PixelMap>(data.ReadParcelable<Media::PixelMap>());
            Drawing::Rect rect;
            RSMarshallingHelper::Unmarshalling(data, rect);
            std::shared_ptr<Drawing::DrawCmdList> drawCmdList;
            RSMarshallingHelper::Unmarshalling(data, drawCmdList);
            bool success;
            if (GetPixelmap(id, pixelmap, &rect, drawCmdList, success) != ERR_OK ||
                !reply.WriteBool(success)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_PIXELMAP Write id failed!");
                ret = ERR_INVALID_REPLY;
                break;
            }
            if (success) {
                RSMarshallingHelper::Marshalling(reply, pixelmap);
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NEED_REGISTER_TYPEFACE): {
            bool result = false;
            uint64_t uniqueId{0};
            uint32_t hash{0};
            if (!data.ReadUint64(uniqueId) || !data.ReadUint32(hash)) {
                RS_LOGE("RSRenderServiceConnectionStub::NEED_REGISTER_TYPEFACE read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            RS_PROFILER_PATCH_TYPEFACE_GLOBALID(data, uniqueId);
            if (IsValidCallingPid(ExtractPid(uniqueId), callingPid)) {
                result = !RSTypefaceCache::Instance().HasTypeface(uniqueId, hash);
            } else {
                RS_LOGE("RSRenderServiceConnectionStub::OnRemoteRequest callingPid[%{public}d] "
                        "no permission NEED_REGISTER_TYPEFACE", callingPid);
            }
            if (!reply.WriteBool(result)) {
                RS_LOGE("RSRenderServiceConnectionStub::NEED_REGISTER_TYPEFACE Write result failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_TYPEFACE): {
            bool xcollieFlag = false;
            bool result = false;
            std::shared_ptr<Drawing::Typeface> typeface = nullptr;
            {
                // timer: 3s
                OHOS::Rosen::RSXCollie registerTypefaceXCollie("registerTypefaceXCollie_" +
                    std::to_string(callingPid), 3, TypefaceXcollieCallback, &xcollieFlag, 0);
                uint64_t uniqueId{0};
                uint32_t hash{0};
                if (!data.ReadUint64(uniqueId) || !data.ReadUint32(hash)) {
                    RS_LOGE("RSRenderServiceConnectionStub::REGISTER_TYPEFACE read parcel failed!");
                    ret = ERR_INVALID_DATA;
                    break;
                }
                // safe check
                if (IsValidCallingPid(ExtractPid(uniqueId), callingPid)) {
                    result = RSMarshallingHelper::Unmarshalling(data, typeface);
                    if (result && typeface) {
                        typeface->SetHash(hash);
                        RS_PROFILER_PATCH_TYPEFACE_GLOBALID(data, uniqueId);
                        RegisterTypeface(uniqueId, typeface);
                    }
                } else {
                    RS_LOGE("RSRenderServiceConnectionStub::OnRemoteRequest callingPid[%{public}d] "
                        "no permission REGISTER_TYPEFACE", callingPid);
                }
            }
            if (xcollieFlag && typeface) {
                RS_LOGW("RSRenderServiceConnectionStub::OnRemoteRequest callingPid[%{public}d] typeface[%{public}s] "
                    "size[%{public}u], too big.", callingPid, typeface->GetFamilyName().c_str(), typeface->GetSize());
            }
            if (!reply.WriteBool(result)) {
                RS_LOGE("RSRenderServiceConnectionStub::REGISTER_TYPEFACE Write result failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_TYPEFACE): {
            uint64_t uniqueId{0};
            if (!data.ReadUint64(uniqueId)) {
                RS_LOGE("RSRenderServiceConnectionStub::UNREGISTER_TYPEFACE read uniqueId failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            // safe check
            if (IsValidCallingPid(ExtractPid(uniqueId), callingPid)) {
                RS_PROFILER_PATCH_TYPEFACE_GLOBALID(data, uniqueId);
                UnRegisterTypeface(uniqueId);
            } else {
                RS_LOGE("RSRenderServiceConnectionStub::OnRemoteRequest callingPid[%{public}d] "
                    "no permission UNREGISTER_TYPEFACE", callingPid);
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_SKIP_FRAME_INTERVAL): {
            if (!securityManager_.IsInterfaceCodeAccessible(code)) {
                RS_LOGE("RSRenderServiceConnectionStub::OnRemoteRequest no permission to access"\
                    "SET_SCREEN_SKIP_FRAME_INTERVAL");
                return ERR_INVALID_STATE;
            }
            ScreenId id{INVALID_SCREEN_ID};
            uint32_t skipFrameInterval{0};
            if (!data.ReadUint64(id) || !data.ReadUint32(skipFrameInterval)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_SCREEN_SKIP_FRAME_INTERVAL Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t statusCode{ SUCCESS };
            if (SetScreenSkipFrameInterval(id, skipFrameInterval, statusCode) != ERR_OK ||
                !reply.WriteInt32(statusCode)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_SCREEN_SKIP_FRAME_INTERVAL Write result failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_REFRESH_RATE): {
            ScreenId id = 0;
            uint32_t maxRefreshRate = 0;
            if (!data.ReadUint64(id) || !data.ReadUint32(maxRefreshRate)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_VIRTUAL_SCREEN_REFRESH_RATE Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            uint32_t actualRefreshRate = 0;
            int32_t result = 0;
            SetVirtualScreenRefreshRate(id, maxRefreshRate, actualRefreshRate, result);
            if (!reply.WriteInt32(result)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_VIRTUAL_SCREEN_REFRESH_RATE Write result failed!");
                return ERR_INVALID_REPLY;
            }
            if (!reply.WriteUint32(actualRefreshRate)) {
                RS_LOGE(
                    "RSRenderServiceConnectionStub::SET_VIRTUAL_SCREEN_REFRESH_RATE Write actualRefreshRate failed!");
                return ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_ACTIVE_RECT): {
            ScreenId id{INVALID_SCREEN_ID};
            int32_t x{0};
            int32_t y{0};
            int32_t w{0};
            int32_t h{0};
            if (!data.ReadUint64(id) || !data.ReadInt32(x) || !data.ReadInt32(y) ||
                !data.ReadInt32(w) || !data.ReadInt32(h)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_SCREEN_ACTIVE_RECT Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            Rect activeRect {
                .x = x,
                .y = y,
                .w = w,
                .h = h
            };
            uint32_t repCode;
            if (SetScreenActiveRect(id, activeRect, repCode) != ERR_OK || !reply.WriteUint32(repCode)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_SCREEN_ACTIVE_RECT Write result failed!");
                return ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_OFFSET): {
            ScreenId id = INVALID_SCREEN_ID;
            int32_t offsetX = 0;
            int32_t offsetY = 0;
            if (!data.ReadUint64(id) || !data.ReadInt32(offsetX) || !data.ReadInt32(offsetY)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_SCREEN_OFFSET Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            SetScreenOffset(id, offsetX, offsetY);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_FRAME_GRAVITY): {
            ScreenId id = INVALID_SCREEN_ID;
            int32_t gravity = 0;
            if (!data.ReadUint64(id) || !data.ReadInt32(gravity)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_SCREEN_FRAME_GRAVITY Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            SetScreenFrameGravity(id, gravity);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_OCCLUSION_CHANGE_CALLBACK): {
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                RS_LOGE("RSRenderServiceConnectionStub::REGISTER_OCCLUSION_CHANGE_CALLBACK Read remoteObject failed!");
                ret = ERR_NULL_OBJECT;
                break;
            }
            sptr<RSIOcclusionChangeCallback> callback = iface_cast<RSIOcclusionChangeCallback>(remoteObject);
            if (callback == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            int32_t repCode;
            if (RegisterOcclusionChangeCallback(callback, repCode) != ERR_OK || !reply.WriteInt32(repCode)) {
                RS_LOGE("RSRenderServiceConnectionStub::REGISTER_OCCLUSION_CHANGE_CALLBACK Write status failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(
            RSIRenderServiceConnectionInterfaceCode::REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK): {
            NodeId id{0};
            if (!RSMarshallingHelper::UnmarshallingPidPlusId(data, id)) {
                RS_LOGE("RSRenderServiceConnectionStub::REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            if (!IsValidCallingPid(ExtractPid(id), callingPid)) {
                RS_LOGW("The RegisterSurfaceOcclusionChangeCallback isn't legal, nodeId:%{public}" PRIu64 ", "
                    "callingPid:%{public}d", id, callingPid);
                ret = ERR_INVALID_DATA;
                break;
            }
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                RS_LOGE("RSRenderServiceConnectionStub::REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK Read remoteObject "
                        "failed!");
                ret = ERR_NULL_OBJECT;
                break;
            }
            sptr<RSISurfaceOcclusionChangeCallback> callback =
                iface_cast<RSISurfaceOcclusionChangeCallback>(remoteObject);
            if (callback == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            std::vector<float> partitionPoints;
            if (!data.ReadFloatVector(&partitionPoints)) {
                RS_LOGE("RSRenderServiceConnectionStub::REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK Read "
                        "partitionPoints failed!");
                ret = ERR_TRANSACTION_FAILED;
                break;
            }
            int32_t status = RegisterSurfaceOcclusionChangeCallback(id, callback, partitionPoints);
            if (!reply.WriteInt32(status)) {
                RS_LOGE(
                    "RSRenderServiceConnectionStub::REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK Write status failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(
            RSIRenderServiceConnectionInterfaceCode::UNREGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK): {
            NodeId id{0};
            if (!RSMarshallingHelper::UnmarshallingPidPlusId(data, id)) {
                RS_LOGE("RSRenderServiceConnectionStub::UNREGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            if (!IsValidCallingPid(ExtractPid(id), callingPid)) {
                RS_LOGW("The UnRegisterSurfaceOcclusionChangeCallback isn't legal, nodeId:%{public}" PRIu64 ", "
                    "callingPid:%{public}d", id, callingPid);
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t status = UnRegisterSurfaceOcclusionChangeCallback(id);
            if (!reply.WriteInt32(status)) {
                RS_LOGE(
                    "RSRenderServiceConnectionStub::UNREGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK Write status failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_APP_WINDOW_NUM): {
            uint32_t num{0};
            if (!data.ReadUint32(num)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_APP_WINDOW_NUM Read num failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            SetAppWindowNum(num);
            break;
        }
        case static_cast<uint32_t>(
            RSIRenderServiceConnectionInterfaceCode::SET_SYSTEM_ANIMATED_SCENES): {
            uint32_t systemAnimatedScenes{0};
            bool isRegularAnimation{false};
            if (!data.ReadUint32(systemAnimatedScenes) || !data.ReadBool(isRegularAnimation)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_SYSTEM_ANIMATED_SCENES Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            bool success;
            if (SetSystemAnimatedScenes(static_cast<SystemAnimatedScenes>(systemAnimatedScenes),
                isRegularAnimation, success) != ERR_OK || !reply.WriteBool(success)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_SYSTEM_ANIMATED_SCENES Write result failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_WATERMARK): {
            if (!RSSystemProperties::GetSurfaceNodeWatermarkEnabled()) {
                RS_LOGI("Current disenable water mark");
                break;
            }
            std::string name;
            if (!data.ReadString(name)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_WATERMARK Read name failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            auto watermark = std::shared_ptr<Media::PixelMap>(data.ReadParcelable<Media::PixelMap>());
            if (watermark == nullptr) {
                ret = ERR_NULL_OBJECT;
                RS_LOGE("RSRenderServiceConnectionStub::std::shared_ptr<Media::PixelMap> watermark == nullptr");
                break;
            }
            bool success;
            if (SetWatermark(name, watermark, success) != ERR_OK || !success) {
                RS_LOGE("RSRenderServiceConnectionStub::SetWatermark failed");
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SHOW_WATERMARK): {
            std::shared_ptr<Media::PixelMap> watermarkImg =
                std::shared_ptr<Media::PixelMap>(data.ReadParcelable<Media::PixelMap>());
            bool isShow{false};
            if (!data.ReadBool(isShow)) {
                RS_LOGE("RSRenderServiceConnectionStub::SHOW_WATERMARK Read isShow failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            ShowWatermark(watermarkImg, isShow);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::RESIZE_VIRTUAL_SCREEN): {
            ScreenId id{INVALID_SCREEN_ID};
            uint32_t width{0};
            uint32_t height{0};
            if (!data.ReadUint64(id) || !data.ReadUint32(width) || !data.ReadUint32(height)) {
                RS_LOGE("RSRenderServiceConnectionStub::RESIZE_VIRTUAL_SCREEN Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t status = ResizeVirtualScreen(id, width, height);
            if (!reply.WriteInt32(status)) {
                RS_LOGE("RSRenderServiceConnectionStub::RESIZE_VIRTUAL_SCREEN Write status failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_JANK_STATS): {
            ReportJankStats();
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_RESPONSE): {
            DataBaseRs info;
            if (!ReadDataBaseRs(info, data)) {
                ret = ERR_INVALID_DATA;
                break;
            }
            ReportEventResponse(info);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_COMPLETE): {
            DataBaseRs info;
            if (!ReadDataBaseRs(info, data)) {
                ret = ERR_INVALID_DATA;
                break;
            }
            ReportEventComplete(info);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_JANK_FRAME): {
            DataBaseRs info;
            if (!ReadDataBaseRs(info, data)) {
                ret = ERR_INVALID_DATA;
                break;
            }
            ReportEventJankFrame(info);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_RS_SCENE_JANK_START): {
            AppInfo info;
            if (!ReadAppInfo(info, data)) {
                ret = ERR_INVALID_DATA;
                break;
            }
            ReportRsSceneJankStart(info);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_RS_SCENE_JANK_END): {
            AppInfo info;
            if (!ReadAppInfo(info, data)) {
                ret = ERR_INVALID_DATA;
                break;
            }
            ReportRsSceneJankEnd(info);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_GAMESTATE): {
            GameStateData info;
            if (!ReadGameStateDataRs(info, data)) {
                ret = ERR_INVALID_DATA;
                break;
            }
            ReportGameStateData(info);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::EXECUTE_SYNCHRONOUS_TASK): {
            int16_t type{0};
            int16_t subType{0};
            if (!data.ReadInt16(type) || !data.ReadInt16(subType)) {
                RS_LOGE("RSRenderServiceConnectionStub::EXECUTE_SYNCHRONOUS_TASK Read parcel failed!");
                ret = ERR_INVALID_STATE;
                break;
            }
            if (type != RS_NODE_SYNCHRONOUS_READ_PROPERTY && type != RS_NODE_SYNCHRONOUS_GET_VALUE_FRACTION) {
                ret = ERR_INVALID_STATE;
                break;
            }
            auto func = RSCommandFactory::Instance().GetUnmarshallingFunc(type, subType);
            if (func == nullptr) {
                ret = ERR_INVALID_STATE;
                break;
            }
            auto command = static_cast<RSSyncTask*>((*func)(data));
            if (command == nullptr) {
                ret = ERR_INVALID_STATE;
                break;
            }
            std::shared_ptr<RSSyncTask> task(command);
            const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
            if (!task->IsCallingPidValid(callingPid, nodeMap)) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ExecuteSynchronousTask(task);
            if (!task->Marshalling(reply)) {
                ret = ERR_INVALID_STATE;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_HARDWARE_ENABLED) : {
            uint64_t id{0};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_HARDWARE_ENABLED Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            if (!IsValidCallingPid(ExtractPid(id), callingPid)) {
                RS_LOGW("The SetHardwareEnabled isn't legal, nodeId:%{public}" PRIu64 ", callingPid:%{public}d",
                    id, callingPid);
                break;
            }
            bool isEnabled{false};
            uint8_t selfDrawingType{static_cast<uint8_t>(SelfDrawingNodeType::DEFAULT)};
            bool dynamicHardwareEnable{false};
            if (!data.ReadBool(isEnabled) ||
                !data.ReadUint8(selfDrawingType) ||
                !data.ReadBool(dynamicHardwareEnable)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_HARDWARE_ENABLED Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            SetHardwareEnabled(id, isEnabled, static_cast<SelfDrawingNodeType>(selfDrawingType), dynamicHardwareEnable);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_HIDE_PRIVACY_CONTENT) : {
            uint64_t id{0};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_HIDE_PRIVACY_CONTENT Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            auto isSystemCalling = RSInterfaceCodeAccessVerifierBase::IsSystemCalling(
                RSIRenderServiceConnectionInterfaceCodeAccessVerifier::codeEnumTypeName_ +
                "::SET_HIDE_PRIVACY_CONTENT");
            if (!isSystemCalling) {
                if (!reply.WriteUint32(static_cast<uint32_t>(RSInterfaceErrorCode::NONSYSTEM_CALLING))) {
                    RS_LOGE("RSRenderServiceConnectionStub::SET_HIDE_PRIVACY_CONTENT Write isSystemCalling failed!");
                    ret = ERR_INVALID_REPLY;
                }
                break;
            }
            if (ExtractPid(id) != callingPid) {
                RS_LOGW("The SetHidePrivacyContent isn't legal, nodeId:%{public}" PRIu64 ", callingPid:%{public}d",
                    id, callingPid);
                if (!reply.WriteUint32(static_cast<uint32_t>(RSInterfaceErrorCode::NOT_SELF_CALLING))) {
                    RS_LOGE("RSRenderServiceConnectionStub::SET_HIDE_PRIVACY_CONTENT Write ErrorCode failed!");
                    ret = ERR_INVALID_REPLY;
                }
                break;
            }
            bool needHidePrivacyContent{false};
            if (!data.ReadBool(needHidePrivacyContent)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_HIDE_PRIVACY_CONTENT read needHidePrivacyContent failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            uint32_t resCode;
            if (SetHidePrivacyContent(id, needHidePrivacyContent, resCode) != ERR_OK ||
                !reply.WriteUint32(resCode)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_HIDE_PRIVACY_CONTENT Write resCode failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_LIGHT_FACTOR_STATUS) : {
            int32_t lightFactorStatus{0};
            if (!data.ReadInt32(lightFactorStatus)) {
                RS_LOGE("RSRenderServiceConnectionStub::NOTIFY_LIGHT_FACTOR_STATUS Read lightFactorStatus failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            NotifyLightFactorStatus(lightFactorStatus);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_PACKAGE_EVENT) : {
            uint32_t listSize{0};
            if (!data.ReadUint32(listSize)) {
                RS_LOGE("RSRenderServiceConnectionStub::NOTIFY_PACKAGE_EVENT Read listSize failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            const uint32_t MAX_LIST_SIZE = 50;
            if (listSize > MAX_LIST_SIZE) {
                ret = ERR_INVALID_STATE;
                break;
            }
            std::vector<std::string> packageList;
            bool errFlag{false};
            for (uint32_t i = 0; i < listSize; i++) {
                std::string package;
                if (!data.ReadString(package)) {
                    RS_LOGE("RSRenderServiceConnectionStub::NOTIFY_PACKAGE_EVENT Read package failed!");
                    errFlag = true;
                    break;
                }
                packageList.push_back(package);
            }
            if (errFlag) {
                ret = ERR_INVALID_DATA;
                break;
            }
            NotifyPackageEvent(listSize, packageList);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_APP_STRATEGY_CONFIG_CHANGE_EVENT) : {
            std::string pkgName;
            uint32_t listSize{0};
            if (!data.ReadString(pkgName) || !data.ReadUint32(listSize)) {
                RS_LOGE("RSRenderServiceConnectionStub::NOTIFY_APP_STRATEGY_CONFIG_CHANGE_EVENT Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            const uint32_t MAX_LIST_SIZE = 50;
            if (listSize > MAX_LIST_SIZE) {
                ret = ERR_INVALID_STATE;
                break;
            }

            std::vector<std::pair<std::string, std::string>> newConfig;
            bool errFlag{false};
            for (uint32_t i = 0; i < listSize; i++) {
                std::string key;
                std::string value;
                if (!data.ReadString(key) || !data.ReadString(value)) {
                    RS_LOGE("RSRenderServiceConnectionStub::NOTIFY_APP_STRATEGY_CONFIG_CHANGE_EVENT Read newConfig "
                            "failed!");
                    errFlag = true;
                    break;
                }
                newConfig.push_back(make_pair(key, value));
            }
            if (errFlag) {
                ret = ERR_INVALID_STATE;
                break;
            }
            NotifyAppStrategyConfigChangeEvent(pkgName, listSize, newConfig);
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_REFRESH_RATE_EVENT) : {
            std::string eventName;
            bool eventStatus{false};
            uint32_t minRefreshRate{0};
            uint32_t maxRefreshRate{0};
            std::string description;
            if (!data.ReadString(eventName) || !data.ReadBool(eventStatus) || !data.ReadUint32(minRefreshRate) ||
                !data.ReadUint32(maxRefreshRate) || !data.ReadString(description)) {
                RS_LOGE("RSRenderServiceConnectionStub::NOTIFY_REFRESH_RATE_EVENT Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            EventInfo eventInfo = {
                eventName, eventStatus, minRefreshRate, maxRefreshRate, description
            };
            NotifyRefreshRateEvent(eventInfo);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_WINDOW_EXPECTED_BY_WINDOW_ID) : {
            std::unordered_map<uint64_t, EventInfo> eventInfos;
            
            uint32_t mapSize{0};
            if (!data.ReadUint32(mapSize)) {
                RS_LOGE("RSRenderServiceConnectionStub::NOTIFY_WINDOW_EXPECTED_BY_WINDOW_ID Read mapSize failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            if (mapSize > MAX_VOTER_SIZE) {
                ret = ERR_INVALID_STATE;
                break;
            }
            bool shouldBreak = false;
            for (uint32_t i = 0; i < mapSize; ++i) {
                uint64_t windowId{0};
                if (!data.ReadUint64(windowId)) {
                    RS_LOGE("RSRenderServiceConnectionStub::NOTIFY_WINDOW_EXPECTED_BY_WINDOW_ID Read parcel failed!");
                    ret = ERR_INVALID_DATA;
                    shouldBreak = true;
                    break;
                }
                EventInfo eventInfo;
                if (!EventInfo::Deserialize(data, eventInfo)) {
                    RS_LOGE("RSRenderServiceConnectionStub::NOTIFY_WINDOW_EXPECTED_BY_WINDOW_ID Read parcel failed!");
                    ret = ERR_INVALID_DATA;
                    shouldBreak = true;
                    break;
                }
                eventInfos[windowId] = eventInfo;
            }
            if (!shouldBreak) {
                SetWindowExpectedRefreshRate(eventInfos);
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_WINDOW_EXPECTED_BY_VSYNC_NAME) : {
            std::unordered_map<std::string, EventInfo> eventInfos;

            uint32_t mapSize{0};
            if (!data.ReadUint32(mapSize)) {
                RS_LOGE("RSRenderServiceConnectionStub::NOTIFY_WINDOW_EXPECTED_BY_VSYNC_NAME Read mapSize failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            if (mapSize > MAX_VOTER_SIZE) {
                ret = ERR_INVALID_STATE;
                break;
            }
            bool shouldBreak = false;
            for (uint32_t i = 0; i < mapSize; ++i) {
                std::string vsyncName;
                if (!data.ReadString(vsyncName)) {
                    RS_LOGE("RSRenderServiceConnectionStub::NOTIFY_WINDOW_EXPECTED_BY_VSYNC_NAME Read parcel failed!");
                    ret = ERR_INVALID_DATA;
                    shouldBreak = true;
                    break;
                }
                EventInfo eventInfo;
                if (!EventInfo::Deserialize(data, eventInfo)) {
                    RS_LOGE("RSRenderServiceConnectionStub::NOTIFY_WINDOW_EXPECTED_BY_VSYNC_NAME Read parcel failed!");
                    ret = ERR_INVALID_DATA;
                    shouldBreak = true;
                    break;
                }
                eventInfos[vsyncName] = eventInfo;
            }
            if (!shouldBreak) {
                SetWindowExpectedRefreshRate(eventInfos);
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_SOFT_VSYNC_EVENT) : {
            uint32_t pid{0};
            uint32_t rateDiscount{0};
            if (!data.ReadUint32(pid) || !data.ReadUint32(rateDiscount)) {
                RS_LOGE("RSRenderServiceConnectionStub::NOTIFY_SOFT_VSYNC_EVENT Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            NotifySoftVsyncEvent(pid, rateDiscount);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_SOFT_VSYNC_RATE_DISCOUNT_EVENT) : {
            uint32_t pid{0};
            std::string name;
            uint32_t rateDiscount{0};
            if (!data.ReadUint32(pid) || !data.ReadString(name) || !data.ReadUint32(rateDiscount)) {
                RS_LOGE("RSRenderServiceConnectionStub::NOTIFY_SOFT_VSYNC_RATE_DISCOUNT_EVENT Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            bool result = NotifySoftVsyncRateDiscountEvent(pid, name, rateDiscount);
            if (!reply.WriteBool(result)) {
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_DYNAMIC_MODE_EVENT) : {
            bool enableDynamicMode{false};
            if (!data.ReadBool(enableDynamicMode)) {
                RS_LOGE("RSRenderServiceConnectionStub::NOTIFY_DYNAMIC_MODE_EVENT Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            NotifyDynamicModeEvent(enableDynamicMode);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_TOUCH_EVENT) : {
            int32_t touchStatus{0};
            int32_t touchCnt{0};
            if (!data.ReadInt32(touchStatus) || !data.ReadInt32(touchCnt)) {
                RS_LOGE("RSRenderServiceConnectionStub::NOTIFY_TOUCH_EVENT Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            NotifyTouchEvent(touchStatus, touchCnt);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_HGMCONFIG_EVENT) : {
            std::string eventName;
            bool state{false};
            if (!data.ReadString(eventName) || !data.ReadBool(state)) {
                RS_LOGE("RSRenderServiceConnectionStub::NOTIFY_HGMCONFIG_EVENT Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            NotifyHgmConfigEvent(eventName, state);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_XCOMPONENT_EXPECTED_FRAMERATE) : {
            std::string id;
            int32_t expectedFrameRate;
            if (!data.ReadString(id) || !data.ReadInt32(expectedFrameRate)) {
                RS_LOGE("RSRenderServiceConnectionStub::NOTIFY_XCOMPONENT_EXPECTED_FRAMERATE Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            NotifyXComponentExpectedFrameRate(id, expectedFrameRate);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_HGM_CFG_CALLBACK) : {
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                RS_LOGE("RSRenderServiceConnectionStub::REGISTER_HGM_CFG_CALLBACK Read remoteObject failed!");
                ret = ERR_NULL_OBJECT;
                break;
            }
            sptr<RSIHgmConfigChangeCallback> callback = iface_cast<RSIHgmConfigChangeCallback>(remoteObject);
            if (callback == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            int32_t status = RegisterHgmConfigChangeCallback(callback);
            if (!reply.WriteInt32(status)) {
                RS_LOGE("RSRenderServiceConnectionStub::REGISTER_HGM_CFG_CALLBACK Write status failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REFRESH_RATE_MODE_CHANGE_CALLBACK) : {
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                RS_LOGE("RSRenderServiceConnectionStub::REFRESH_RATE_MODE_CHANGE_CALLBACK Read remoteObject failed!");
                ret = ERR_NULL_OBJECT;
                break;
            }
            sptr<RSIHgmConfigChangeCallback> callback =
                iface_cast<RSIHgmConfigChangeCallback>(remoteObject);
            if (callback == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            int32_t status = RegisterHgmRefreshRateModeChangeCallback(callback);
            if (!reply.WriteInt32(status)) {
                RS_LOGE("RSRenderServiceConnectionStub::REFRESH_RATE_MODE_CHANGE_CALLBACK Write status failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REFRESH_RATE_UPDATE_CALLBACK) : {
            sptr<RSIHgmConfigChangeCallback> callback = nullptr;
            sptr<IRemoteObject> remoteObject = nullptr;
            bool readRemoteObject{false};
            if (!data.ReadBool(readRemoteObject)) {
                RS_LOGE("RSRenderServiceConnectionStub::REFRESH_RATE_UPDATE_CALLBACK Read remoteObject failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            if (readRemoteObject) {
                remoteObject = data.ReadRemoteObject();
            }
            if (remoteObject != nullptr) {
                callback = iface_cast<RSIHgmConfigChangeCallback>(remoteObject);
            }
            int32_t status = RegisterHgmRefreshRateUpdateCallback(callback);
            if (!reply.WriteInt32(status)) {
                RS_LOGE("RSRenderServiceConnectionStub::REFRESH_RATE_UPDATE_CALLBACK Write status failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::ON_FIRST_FRAME_COMMIT) : {
            sptr<RSIFirstFrameCommitCallback> callback = nullptr;
            sptr<IRemoteObject> remoteObject = nullptr;
            bool readRemoteObject{false};
            if (!data.ReadBool(readRemoteObject)) {
                ret = ERR_INVALID_DATA;
                break;
            }
            if (readRemoteObject) {
                remoteObject = data.ReadRemoteObject();
            }
            if (remoteObject != nullptr) {
                callback = iface_cast<RSIFirstFrameCommitCallback>(remoteObject);
            }
            int32_t status = RegisterFirstFrameCommitCallback(callback);
            if (!reply.WriteInt32(status)) {
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(
            RSIRenderServiceConnectionInterfaceCode::REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK) : {
            sptr<RSIFrameRateLinkerExpectedFpsUpdateCallback> callback = nullptr;
            sptr<IRemoteObject> remoteObject = nullptr;
            int32_t dstPid{0};
            bool readRemoteObject{false};
            if (!data.ReadInt32(dstPid) || !data.ReadBool(readRemoteObject)) {
                RS_LOGE("RSRenderServiceConnectionStub::REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK Read parcel "
                        "failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            if (readRemoteObject) {
                remoteObject = data.ReadRemoteObject();
            }
            if (remoteObject != nullptr) {
                callback = iface_cast<RSIFrameRateLinkerExpectedFpsUpdateCallback>(remoteObject);
            }
            int32_t status = RegisterFrameRateLinkerExpectedFpsUpdateCallback(dstPid, callback);
            if (!reply.WriteInt32(status)) {
                RS_LOGE("RSRenderServiceConnectionStub::REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK Write status "
                        "failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_ROTATION_CACHE_ENABLED) : {
            bool isEnabled = false;
            if (!data.ReadBool(isEnabled)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_ROTATION_CACHE_ENABLED Read isEnabled failed!");
                ret = IPC_STUB_INVALID_DATA_ERR;
                break;
            }
            SetCacheEnabledForRotation(isEnabled);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ACTIVE_DIRTY_REGION_INFO) : {
            const auto& activeDirtyRegionInfos = GetActiveDirtyRegionInfo();
            if (!reply.WriteInt32(activeDirtyRegionInfos.size())) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_ACTIVE_DIRTY_REGION_INFO Write activeDirtyRegionInfosSize "
                        "failed!");
                ret = ERR_INVALID_REPLY;
                break;
            }
            for (const auto& activeDirtyRegionInfo : activeDirtyRegionInfos) {
                if (!reply.WriteInt64(activeDirtyRegionInfo.activeDirtyRegionArea) ||
                    !reply.WriteInt32(activeDirtyRegionInfo.activeFramesNumber) ||
                    !reply.WriteInt32(activeDirtyRegionInfo.pidOfBelongsApp) ||
                    !reply.WriteString(activeDirtyRegionInfo.windowName)) {
                    RS_LOGE("RSRenderServiceConnectionStub::GET_ACTIVE_DIRTY_REGION_INFO Write activeDirtyRegionInfos "
                            "failed!");
                    ret = ERR_INVALID_REPLY;
                    break;
                }
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_GLOBAL_DIRTY_REGION_INFO) : {
            const auto& globalDirtyRegionInfo = GetGlobalDirtyRegionInfo();
            if (!reply.WriteInt64(globalDirtyRegionInfo.globalDirtyRegionAreas) ||
                !reply.WriteInt32(globalDirtyRegionInfo.globalFramesNumber) ||
                !reply.WriteInt32(globalDirtyRegionInfo.skipProcessFramesNumber) ||
                !reply.WriteInt32(globalDirtyRegionInfo.mostSendingPidWhenDisplayNodeSkip)) {
                RS_LOGE(
                    "RSRenderServiceConnectionStub::GET_GLOBAL_DIRTY_REGION_INFO Write globalDirtyRegionInfo failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_LAYER_COMPOSE_INFO) : {
            const auto& LayerComposeInfo = GetLayerComposeInfo();
            if (!reply.WriteInt32(LayerComposeInfo.uniformRenderFrameNumber) ||
                !reply.WriteInt32(LayerComposeInfo.offlineComposeFrameNumber) ||
                !reply.WriteInt32(LayerComposeInfo.redrawFrameNumber)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_LAYER_COMPOSE_INFO Write LayerComposeInfo failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::
            GET_HARDWARE_COMPOSE_DISABLED_REASON_INFO) : {
            const auto& hwcDisabledReasonInfos = GetHwcDisabledReasonInfo();
            if (!reply.WriteInt32(hwcDisabledReasonInfos.size())) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_HARDWARE_COMPOSE_DISABLED_REASON_INFO Write "
                        "hwcDisabledReasonInfos failed!");
                ret = ERR_INVALID_REPLY;
                break;
            }
            for (const auto& hwcDisabledReasonInfo : hwcDisabledReasonInfos) {
                for (const auto& disabledReasonCount : hwcDisabledReasonInfo.disabledReasonStatistics) {
                    if (!reply.WriteInt32(disabledReasonCount)) {
                        RS_LOGE("RSRenderServiceConnectionStub::GET_HARDWARE_COMPOSE_DISABLED_REASON_INFO Write "
                                "disabledReasonCount failed!");
                        ret = ERR_INVALID_REPLY;
                        break;
                    }
                }
                if (ret == ERR_INVALID_REPLY) {
                    break;
                }
                if (!reply.WriteInt32(hwcDisabledReasonInfo.pidOfBelongsApp) ||
                    !reply.WriteString(hwcDisabledReasonInfo.nodeName)) {
                    RS_LOGE("RSRenderServiceConnectionStub::GET_HARDWARE_COMPOSE_DISABLED_REASON_INFO Write "
                            "hwcDisabledReasonInfo failed!");
                    ret = ERR_INVALID_REPLY;
                    break;
                }
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_HDR_ON_DURATION) : {
            int64_t hdrOnDuration = 0;
            auto errCode = GetHdrOnDuration(hdrOnDuration);
            if (errCode != ERR_OK || !reply.WriteInt64(hdrOnDuration)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_HDR_ON_DURATION Write "
                        "hdrOnDuration failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
#ifdef TP_FEATURE_ENABLE
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_TP_FEATURE_CONFIG) : {
            int32_t feature{0};
            if (!data.ReadInt32(feature)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_TP_FEATURE_CONFIG Read feature failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            auto config = data.ReadCString();
            if (config == nullptr) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_TP_FEATURE_CONFIG Read config failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            uint8_t tpFeatureConfigType{0};
            if (!data.ReadUint8(tpFeatureConfigType)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_TP_FEATURE_CONFIG Read tpFeatureConfigType failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            SetTpFeatureConfig(feature, config, static_cast<TpFeatureConfigType>(tpFeatureConfigType));
            break;
        }
#endif
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_USING_STATUS) : {
            bool isVirtualScreenUsingStatus{false};
            if (!data.ReadBool(isVirtualScreenUsingStatus)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_VIRTUAL_SCREEN_USING_STATUS Read "
                        "isVirtualScreenUsingStatus failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            SetVirtualScreenUsingStatus(isVirtualScreenUsingStatus);
            break;
        }

        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_CURTAIN_SCREEN_USING_STATUS) : {
            bool isCurtainScreenOn{false};
            if (!data.ReadBool(isCurtainScreenOn)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_CURTAIN_SCREEN_USING_STATUS Read "
                        "isCurtainScreenOn failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            SetCurtainScreenUsingStatus(isCurtainScreenOn);
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
            DropFrameByPid(pidList);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_UIEXTENSION_CALLBACK): {
            uint64_t userId{0};
            if (!data.ReadUint64(userId)) {
                RS_LOGE("RSRenderServiceConnectionStub::REGISTER_UIEXTENSION_CALLBACK Read "
                        "userId failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                RS_LOGE("RSRenderServiceConnectionStub::REGISTER_UIEXTENSION_CALLBACK Read remoteObject failed!");
                ret = ERR_NULL_OBJECT;
                break;
            }
            sptr<RSIUIExtensionCallback> callback = iface_cast<RSIUIExtensionCallback>(remoteObject);
            if (callback == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            bool unobscured{false};
            if (!data.ReadBool(unobscured)) {
                RS_LOGE("RSRenderServiceConnectionStub::REGISTER_UIEXTENSION_CALLBACK Read unobscured failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t status = RegisterUIExtensionCallback(userId, callback, unobscured);
            if (!reply.WriteInt32(status)) {
                RS_LOGE("RSRenderServiceConnectionStub::REGISTER_UIEXTENSION_CALLBACK Write status failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_STATUS) : {
            ScreenId id{INVALID_SCREEN_ID};
            uint8_t screenStatus{0};
            if (!data.ReadUint64(id) || !data.ReadUint8(screenStatus)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_VIRTUAL_SCREEN_STATUS Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            bool success;
            SetVirtualScreenStatus(id, static_cast<VirtualScreenStatus>(screenStatus), success);
            if (!reply.WriteBool(success)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_VIRTUAL_SCREEN_STATUS Write result failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_PAGE_NAME) : {
            std::string packageName;
            std::string pageName;
            bool isEnter{false};
            if (!data.ReadString(packageName) || !data.ReadString(pageName) || !data.ReadBool(isEnter)) {
                RS_LOGE("NOTIFY_PAGE_NAME read data err.");
                ret = ERR_INVALID_DATA;
                break;
            }
            NotifyPageName(packageName, pageName, isEnter);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VMA_CACHE_STATUS) : {
            bool flag{false};
            if (!data.ReadBool(flag)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_VMA_CACHE_STATUS read flag failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            SetVmaCacheStatus(flag);
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
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_DISPLAY_NODE) : {
            uint64_t id{0};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSRenderServiceConnectionStub::CREATE_DISPLAY_NODE Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            bool isNonSystemCalling = false;
            bool isTokenTypeValid = true;
            RSInterfaceCodeAccessVerifierBase::GetAccessType(isTokenTypeValid, isNonSystemCalling);
            if (isNonSystemCalling && !IsValidCallingPid(ExtractPid(id), callingPid)) {
                RS_LOGW("CREATE_DISPLAY_NODE invalid nodeId[%{public}" PRIu64 "] pid[%{public}d]", id, callingPid);
                ret = ERR_INVALID_DATA;
                break;
            }
            uint64_t mirroredId{0};
            uint64_t screenId{0};
            bool isMirror{false};
            if (!data.ReadUint64(mirroredId) ||
                !data.ReadUint64(screenId) ||
                !data.ReadBool(isMirror)) {
                RS_LOGE("RSRenderServiceConnectionStub::CREATE_DISPLAY_NODE Read config failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            RSDisplayNodeConfig config = {
                .screenId = screenId,
                .isMirrored = isMirror,
                .mirrorNodeId = mirroredId,
                .isSync = true,
            };
            bool success;
            if (CreateNode(config, id, success) != ERR_OK || reply.WriteBool(success)) {
                RS_LOGE("RSRenderServiceConnectionStub::CREATE_DISPLAY_NODE Write success failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_FREE_MULTI_WINDOW_STATUS) : {
            bool enable{false};
            if (!data.ReadBool(enable)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_FREE_MULTI_WINDOW_STATUS Read enable failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            SetFreeMultiWindowStatus(enable);
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
            std::string nodeIdStr;
            bool isTop{false};
            uint32_t topLayerZOrder = 0;
            if (!data.ReadString(nodeIdStr) || !data.ReadBool(isTop) || !data.ReadUint32(topLayerZOrder)) {
                RS_LOGE("RSRenderServiceConntionStub::SET_LAYER_TOP_FOR_HARDWARE_COMPOSER Read parcel failed");
                ret = ERR_INVALID_DATA;
                break;
            }
            SetLayerTopForHWC(nodeIdStr, isTop, topLayerZOrder);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_LAYER_TOP) : {
            std::string nodeIdStr;
            bool isTop{false};
            if (!data.ReadString(nodeIdStr) ||
                !data.ReadBool(isTop)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_LAYER_TOP Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            SetLayerTop(nodeIdStr, isTop);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_FORCE_REFRESH) : {
            std::string nodeIdStr;
            bool isForceRefresh{false};
            if (!data.ReadString(nodeIdStr) ||
                !data.ReadBool(isForceRefresh)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_FORCE_REFRESH Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            SetForceRefresh(nodeIdStr, isForceRefresh);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_COLOR_FOLLOW) : {
            std::string nodeIdStr;
            bool isColorFollow{false};
            if (!data.ReadString(nodeIdStr) ||
                !data.ReadBool(isColorFollow)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_COLOR_FOLLOW Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            SetColorFollow(nodeIdStr, isColorFollow);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_SCREEN_SWITCHED) : {
            NotifyScreenSwitched();
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
            RSIRenderServiceConnectionInterfaceCode::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK): {
            uint32_t size;
            if (!data.ReadUint32(size)) {
                ROSEN_LOGE(
                    "RSRenderServiceConnectionStub::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK Read size failed");
                ret = ERR_INVALID_REPLY;
                break;
            }
            RectConstraint constraint;
            if (size > MAX_PID_SIZE_NUMBER) {
                ROSEN_LOGE("RSRenderServiceConnectionStub::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK,"
                           "size number is too large.");
                break;
            }
            for (uint32_t i = 0; i < size; ++i) {
                pid_t pid;
                if (!data.ReadInt32(pid)) {
                    ROSEN_LOGE("RSRenderServiceConnectionStub::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK Read "
                               "pid failed");
                    ret = ERR_INVALID_REPLY;
                    break;
                }
                constraint.pids.insert(pid);
            }
            if (!data.ReadInt32(constraint.range.lowLimit.width) || !data.ReadInt32(constraint.range.lowLimit.height) ||
                !data.ReadInt32(constraint.range.highLimit.width) ||
                !data.ReadInt32(constraint.range.highLimit.height)) {
                ROSEN_LOGE("RSRenderServiceConnectionStub::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK Read "
                           "rectRange failed");
                ret = ERR_INVALID_REPLY;
                break;
            }
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                RS_LOGE("RSRenderServiceConnectionStub::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK Read "
                        "remoteObject failed!");
                ret = ERR_NULL_OBJECT;
                break;
            }
            sptr<RSISelfDrawingNodeRectChangeCallback> callback =
                iface_cast<RSISelfDrawingNodeRectChangeCallback>(remoteObject);
            if (callback == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            int32_t status = RegisterSelfDrawingNodeRectChangeCallback(constraint, callback);
            if (!reply.WriteInt32(status)) {
                RS_LOGE("RSRenderServiceConnectionStub::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK Write status "
                        "failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(
            RSIRenderServiceConnectionInterfaceCode::UNREGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK): {
            int32_t status = UnRegisterSelfDrawingNodeRectChangeCallback();
            if (!reply.WriteInt32(status)) {
                ret = ERR_INVALID_REPLY;
            }
        }
        case static_cast<uint32_t>(
            RSIRenderServiceConnectionInterfaceCode::REGISTER_TRANSACTION_DATA_CALLBACK): {
            auto pid = data.ReadInt32();
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
                PRIu64 " pid: %{public}d", timeStamp, pid);
            RegisterTransactionDataCallback(pid, timeStamp, callback);
            break;
        }
#ifdef RS_ENABLE_OVERLAY_DISPLAY
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_OVERLAY_DISPLAY_MODE) : {
            RS_LOGI("RSRenderServicrConnectionStub::OnRemoteRequest SET_OVERLAY_DISPLAY_MODE");
            int32_t mode{0};
            if (!data.ReadInt32(mode)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_OVERLAY_DISPLAY_MODE Read mode failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t result = SetOverlayDisplayMode(mode);
            if (!reply.WriteInt32(result)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_OVERLAY_DISPLAY_MODE Write status failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
#endif
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_HIGH_CONTRAST_TEXT_STATE) : {
            bool highContrast = GetHighContrastTextState();
            if (!reply.WriteBool(highContrast)) {
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_BEHIND_WINDOW_FILTER_ENABLED): {
            bool enabled { false };
            if (!data.ReadBool(enabled)) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_BEHIND_WINDOW_FILTER_ENABLED read enabled failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            auto err = SetBehindWindowFilterEnabled(enabled);
            if (err != ERR_OK) {
                RS_LOGE("RSRenderServiceConnectionStub::SET_BEHIND_WINDOW_FILTER_ENABLED Write status failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_BEHIND_WINDOW_FILTER_ENABLED): {
            bool enabled;
            if (GetBehindWindowFilterEnabled(enabled) != ERR_OK || !reply.WriteBool(enabled)) {
                RS_LOGE("RSRenderServiceConnectionStub::GET_BEHIND_WINDOW_FILTER_ENABLED write enabled failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PID_GPU_MEMORY_IN_MB): {
            int32_t pid{0};
            float gpuMemInMB{0.0};
            if (!data.ReadInt32(pid)) {
                RS_LOGE("RenderServiceConnectionStub::GET_PID_GPU_MEMORY_IN_MB : read data err!");
                ret = ERR_INVALID_DATA;
                break;
            }
            ret = GetPidGpuMemoryInMB(pid, gpuMemInMB);
            if (ret != 0) {
                RS_LOGE("RenderServiceConnectionStub::GET_PID_GPU_MEMORY_IN_MB : read ret err!");
                ret = ERR_INVALID_DATA;
                break;
            }
            if (!reply.WriteFloat(gpuMemInMB)) {
                RS_LOGE("RenderServiceConnectionStub::GET_PID_GPU_MEMORY_IN_MB write gpuMemInMB err!");
                ret = ERR_INVALID_REPLY;
            }
            RS_LOGD("RenderServiceConnectionStub::GET_PID_GPU_MEMORY_IN_MB, ret: %{public}d, gpuMemInMB: %{public}f",
                ret, gpuMemInMB);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::AVCODEC_VIDEO_START): {
            uint64_t uniqueId{0};
            std::string surfaceName;
            uint32_t fps{0};
            uint64_t reportTime{0};
            if (!data.ReadUint64(uniqueId) || !data.ReadString(surfaceName)
                || !data.ReadUint32(fps) || !data.ReadUint64(reportTime)) {
                RS_LOGE("RenderServiceConnectionStub::AVCODEC_VIDEO_START : read data err!");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t result = AvcodecVideoStart(uniqueId, surfaceName, fps, reportTime);
            if (!reply.WriteInt32(result)) {
                RS_LOGE("RSRenderServiceConnectionStub::AVCODEC_VIDEO_START Write status failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::AVCODEC_VIDEO_STOP): {
            uint64_t uniqueId{0};
            std::string surfaceName;
            uint32_t fps{0};
            if (!data.ReadUint64(uniqueId) || !data.ReadString(surfaceName) || !data.ReadUint32(fps)) {
                RS_LOGE("RSRenderServiceConnectionStub::AVCODEC_VIDEO_STOP : read data err!");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t result = AvcodecVideoStop(uniqueId, surfaceName, fps);
            if (!reply.WriteInt32(result)) {
                RS_LOGE("RSRenderServiceConnectionStub::AVCODEC_VIDEO_STOP Write status failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::PROFILER_SERVICE_OPEN_FILE): {
            HrpServiceDir baseDirType = HrpServiceGetDirType(data.ReadUint32());
            std::string subDir = data.ReadString();
            std::string subDir2 = data.ReadString();
            std::string fileName = data.ReadString();
            int32_t flags = data.ReadInt32();

            int32_t retFd = -1;
            HrpServiceDirInfo dirInfo{baseDirType, subDir, subDir2};
            RetCodeHrpService retCode = ProfilerServiceOpenFile(dirInfo, fileName, flags, retFd);
            reply.WriteInt32((int32_t)retCode);
            reply.WriteFileDescriptor(retFd);
            if (retFd != -1) {
                close(retFd); // call 'close' due to dup was invoked
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::PROFILER_SERVICE_POPULATE_FILES): {
            HrpServiceDir baseDirType = HrpServiceGetDirType(data.ReadUint32());
            std::string subDir = data.ReadString();
            std::string subDir2 = data.ReadString();
            uint32_t firstFileIndex = data.ReadUint32();

            std::vector<HrpServiceFileInfo> retFiles;
            HrpServiceDirInfo dirInfo{baseDirType, subDir, subDir2};
            RetCodeHrpService retCode = ProfilerServicePopulateFiles(dirInfo, firstFileIndex, retFiles);
            reply.WriteInt32((int32_t)retCode);
            reply.WriteUint32((uint32_t)retFiles.size());
            for (const auto& fi : retFiles) {
                reply.WriteString(fi.name);
                reply.WriteUint32(fi.size);
                reply.WriteBool(fi.isDir);
                reply.WriteUint32(fi.accessBits);
                reply.WriteUint64(fi.accessTime.sec);
                reply.WriteUint64(fi.accessTime.nsec);
                reply.WriteUint64(fi.modifyTime.sec);
                reply.WriteUint64(fi.modifyTime.nsec);
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::PROFILER_IS_SECURE_SCREEN): {
            bool retValue = ProfilerIsSecureScreen();
            reply.WriteBool(retValue);
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

bool RSRenderServiceConnectionStub::ReadAppInfo(AppInfo& info, MessageParcel& data)
{
    if (!data.ReadInt64(info.startTime)) {
        RS_LOGE("RSRenderServiceConnectionStub::ReadAppInfo Read startTime failed!");
        return false;
    }
    if (!data.ReadInt64(info.endTime)) {
        RS_LOGE("RSRenderServiceConnectionStub::ReadAppInfo Read endTime failed!");
        return false;
    }
    if (!data.ReadInt32(info.pid)) {
        RS_LOGE("RSRenderServiceConnectionStub::ReadAppInfo Read pid failed!");
        return false;
    }
    if (!data.ReadString(info.versionName)) {
        RS_LOGE("RSRenderServiceConnectionStub::ReadAppInfo Read versionName failed!");
        return false;
    }
    if (!data.ReadInt32(info.versionCode)) {
        RS_LOGE("RSRenderServiceConnectionStub::ReadAppInfo Read versionCode failed!");
        return false;
    }
    if (!data.ReadString(info.bundleName)) {
        RS_LOGE("RSRenderServiceConnectionStub::ReadAppInfo Read bundleName failed!");
        return false;
    }
    if (!data.ReadString(info.processName)) {
        RS_LOGE("RSRenderServiceConnectionStub::ReadAppInfo Read processName failed!");
        return false;
    }
    return true;
}

bool RSRenderServiceConnectionStub::ReadGameStateDataRs(GameStateData& info, MessageParcel& data)
{
    if (!data.ReadInt32(info.pid) || !data.ReadInt32(info.uid) ||
        !data.ReadInt32(info.state) || !data.ReadInt32(info.renderTid) ||
        !data.ReadString(info.bundleName)) {
        RS_LOGE("RSRenderServiceConnectionStub::ReadGameStateDataRs Read parcel failed!");
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
