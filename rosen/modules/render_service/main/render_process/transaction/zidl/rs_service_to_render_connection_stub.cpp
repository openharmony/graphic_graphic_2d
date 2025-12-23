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

#include "rs_service_to_render_connection_stub.h"

#include "buffer_utils.h"
#include "rs_iservice_to_render_connection_ipc_interface_code.h"
#include "ipc_security/rs_ipc_interface_code_access_verifier_base.h"
#include "rs_profiler.h"
#include "common/rs_xcollie.h"
#include "platform/common/rs_log.h"

#undef LOG_TAG
#define LOG_TAG "RSServiceToRenderConnectionStub"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t MAX_PID_SIZE_NUMBER = 100000;
}

static void TypefaceXcollieCallback(void* arg)
{
    if (arg) {
        bool* isTrigger = static_cast<bool*>(arg);
        *isTrigger = true;
    }
}

int RSServiceToRenderConnectionStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    int ret = ERR_NONE;
    if (auto interfaceToken = data.ReadInterfaceToken();
        interfaceToken != RSIServiceToRenderConnection::GetDescriptor()) {
        RS_LOGE("%{public}s: Read interfaceToken failed!", __func__);
        return ERR_INVALID_STATE;
    }
    bool isTokenTypeValid = true;
    bool isNonSystemAppCalling = false;
    RSInterfaceCodeAccessVerifierBase::GetAccessType(isTokenTypeValid, isNonSystemAppCalling);
    if (!isTokenTypeValid) {
        RS_LOGE("RSServiceToRenderConnectionStub::OnRemoteRequest invalid token type");
        return ERR_INVALID_STATE;
    }
    if (isNonSystemAppCalling) {
        RS_LOGE("RSServiceToRenderConnectionStub::OnRemoteRequest isNonSystemAppCalling");
        return ERR_INVALID_STATE;
    }
    switch (code) {
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REGISTER_OCCLUSION_CHANGE_CALLBACK): {
            pid_t pid;
            if (!data.ReadInt32(pid)) {
                RS_LOGE("RSServiceToRenderStub::REGISTER_OCCLUSION_CHANGE_CALLBACK Read pid failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                RS_LOGE("RSServiceToRenderStub::REGISTER_OCCLUSION_CHANGE_CALLBACK ReadRemoteObject failed!");
                ret = ERR_NULL_OBJECT;
                break;
            }
            sptr<RSIOcclusionChangeCallback> callback = iface_cast<RSIOcclusionChangeCallback>(remoteObject);
            if (callback == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            int32_t status = RegisterOcclusionChangeCallback(pid, callback);
            if (!reply.WriteInt32(status)) {
                RS_LOGE("RSServiceToRenderStub::REGISTER_OCCLUSION_CHANGE_CALLBACK Write status failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(
            RSIServiceToRenderConnectionInterfaceCode::REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK): {
            NodeId id{0};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSServiceToRenderStub::REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            pid_t pid;
            if (!data.ReadInt32(pid)) {
                RS_LOGE("RSServiceToRenderStub::REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK Read pid failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                RS_LOGE("RSServiceToRenderStub::REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK Read remoteObject "
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
                RS_LOGE("RSServiceToRenderStub::REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK Read "
                        "partitionPoints failed!");
                ret = ERR_TRANSACTION_FAILED;
                break;
            }
            int32_t status = RegisterSurfaceOcclusionChangeCallback(id, pid, callback, partitionPoints);
            if (!reply.WriteInt32(status)) {
                RS_LOGE("RSServiceToRenderStub::REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK Write status failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(
            RSIServiceToRenderConnectionInterfaceCode::UNREGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK): {
            NodeId id{0};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSServiceToRenderStub::UNREGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t status = UnRegisterSurfaceOcclusionChangeCallback(id);
            if (!reply.WriteInt32(status)) {
                RS_LOGE("RSServiceToRenderStub::UNREGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK "
                    "Write status failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_BRIGHTNESS_INFO_CHANGE_CALLBACK): {
            auto interfaceToken = data.ReadInterfaceToken();
            if (interfaceToken != RSIServiceToRenderConnection::GetDescriptor()) {
                RS_LOGE("RSServiceToRenderStub::SET_BRIGHTNESS_INFO_CHANGE_CALLBACK Read interfaceToken failed!");
                ret = ERR_INVALID_STATE;
                break;
            }
            pid_t pid;
            if (!data.ReadInt32(pid)) {
                RS_LOGE("RSServiceToRenderStub::SET_BRIGHTNESS_INFO_CHANGE_CALLBACK Read pid failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                RS_LOGE("RSServiceToRenderStub::SET_BRIGHTNESS_INFO_CHANGE_CALLBACK ReadRemoteObject failed!");
                ret = ERR_NULL_OBJECT;
                break;
            }
            sptr<RSIBrightnessInfoChangeCallback> callback = iface_cast<RSIBrightnessInfoChangeCallback>(remoteObject);
            int32_t status = SetBrightnessInfoChangeCallback(pid, callback); 
            if (!reply.WriteInt32(status)) {
                RS_LOGE("RSServiceToRenderStub::SET_BRIGHTNESS_INFO_CHANGE_CALLBACK Write status failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_DISCARD_JANK_FRAME): {
            bool discardJankFrames{false};
            if (!data.ReadBool(discardJankFrames)) {
                RS_LOGE("RSServiceToRenderStub::SET_DISCARD_JANK_FRAME read flag failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            SetDiscardJankFrames(discardJankFrames);
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REPORT_JANK_STATS): {
            ReportJankStats();
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REPORT_EVENT_RESPONSE): {
            DataBaseRs info;
            if (!ReadDataBaseRs(info, data)) {
                ret = ERR_INVALID_DATA;
                break;
            }
            ReportEventResponse(info);
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REPORT_EVENT_COMPLETE): {
            DataBaseRs info;
            if (!ReadDataBaseRs(info, data)) {
                ret = ERR_INVALID_DATA;
                break;
            }
            ReportEventComplete(info);
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REPORT_EVENT_JANK_FRAME): {
            DataBaseRs info;
            if (!ReadDataBaseRs(info, data)) {
                ret = ERR_INVALID_DATA;
                break;
            }
            ReportEventJankFrame(info);
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REPORT_RS_SCENE_JANK_START): {
            AppInfo info;
            if (!ReadAppInfo(info, data)) {
                ret = ERR_INVALID_DATA;
                break;
            }
            ReportRsSceneJankStart(info);
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REPORT_RS_SCENE_JANK_END): {
            AppInfo info;
            if (!ReadAppInfo(info, data)) {
                ret = ERR_INVALID_DATA;
                break;
            }
            ReportRsSceneJankEnd(info);
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::AVCODEC_VIDEO_START): {
            std::vector<uint64_t> uniqueIdList;
            std::vector<std::string> surfaceNameList;
            uint32_t fps{0};
            uint64_t reportTime{0};
            if (!data.ReadUInt64Vector(&uniqueIdList) || !data.ReadStringVector(&surfaceNameList) ||
                !data.ReadUint32(fps) || !data.ReadUint64(reportTime)) {
                RS_LOGE("RSServiceToRenderStub::AVCODEC_VIDEO_START : read data err!");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t result = AvcodecVideoStart(uniqueIdList, surfaceNameList, fps, reportTime);
            if (!reply.WriteInt32(result)) {
                RS_LOGE("RSServiceToRenderStub::AVCODEC_VIDEO_START Write status failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::AVCODEC_VIDEO_STOP): {
            std::vector<uint64_t> uniqueIdList;
            std::vector<std::string> surfaceNameList;
            uint32_t fps{0};
            if (!data.ReadUInt64Vector(&uniqueIdList) || !data.ReadStringVector(&surfaceNameList) ||
                !data.ReadUint32(fps)) {
                RS_LOGE("RSServiceToRenderStub::AVCODEC_VIDEO_STOP : read data err!");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t result = AvcodecVideoStop(uniqueIdList, surfaceNameList, fps);
            if (!reply.WriteInt32(result)) {
                RS_LOGE("RSServiceToRenderStub::AVCODEC_VIDEO_STOP Write status failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_MEMORY_GRAPHICS): {
            std::vector<MemoryGraphic> memoryGraphics;
            if (GetMemoryGraphics(memoryGraphics) != ERR_OK ||
                !reply.WriteUint64(static_cast<uint64_t>(memoryGraphics.size()))) {
                ret = ERR_INVALID_REPLY;
                break;
            }
            for (uint32_t index = 0; index < memoryGraphics.size(); index++) {
                if (!reply.WriteParcelable(&memoryGraphics[index])) {
                    ret = ERR_INVALID_REPLY;
                    break;
                }
            }
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_LAYER_TOP) : {
            std::string nodeIdStr;
            bool isTop{false};
            if (!data.ReadString(nodeIdStr) ||
                !data.ReadBool(isTop)) {
                RS_LOGE("RSClientToServiceConnectionStub::SET_LAYER_TOP Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            SetLayerTop(nodeIdStr, isTop);
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::CLEAN_RESOURCE): {
            pid_t pid;
            if (!data.ReadInt32(pid)) {
                RS_LOGE("RSClientToRenderConnectionStub::CLEAN_RESOURCE read pid failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            RS_TRACE_NAME_FMT("ccc: RSClientToRenderConnectionStub::CLEAN_RESOURCE pid is %d", pid);
            CleanResources(pid);
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_TOTAL_APP_MEM_SIZE): {
            float cpuMemSize = 0.f;
            float gpuMemSize = 0.f;
            if (GetTotalAppMemSize(cpuMemSize, gpuMemSize) != ERR_OK || !reply.WriteFloat(cpuMemSize)
                || !reply.WriteFloat(gpuMemSize)) {
                RS_LOGE("RSServiceToRenderStub::GET_TOTAL_APP_MEM_SIZE Write parcel failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_PIXELMAP_BY_PROCESSID): {
            uint64_t pid;
            if (!data.ReadUint64(pid)) {
                RS_LOGE("RSServiceToRenderStub::GET_PIXELMAP_BY_PROCESSID Read pid failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            std::vector<PixelMapInfo> pixelMapInfoVector;
            int32_t repCode;
            if (GetPixelMapByProcessId(pixelMapInfoVector, static_cast<pid_t>(pid), repCode) != ERR_OK ||
                !reply.WriteInt32(repCode)) {
                RS_LOGE("RSServiceToRenderStub::GET_PIXELMAP_BY_PROCESSID Write repCode failed!");
                ret = ERR_INVALID_REPLY;
                break;
            }
            if (!RSMarshallingHelper::Marshalling(reply, pixelMapInfoVector)) {
                ret = ERR_INVALID_REPLY;
                break;
            }
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_VMA_CACHE_STATUS) : {
            bool flag{false};
            if (!data.ReadBool(flag)) {
                RS_LOGE("RSClientToRenderConnectionStub::SET_VMA_CACHE_STATUS read flag failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            SetVmaCacheStatus(flag);
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_WATERMARK): {
            if (!RSSystemProperties::GetSurfaceNodeWatermarkEnabled()) {
                RS_LOGI("RSServiceToRenderStub::SET_WATERMARK Current disenable water mark");
                break;
            }
            pid_t callingPid;
            if (!data.ReadInt32(callingPid)) {
                ret = ERR_INVALID_VALUE;
                break;
            }
            std::string name;
            if (!data.ReadString(name)) {
                ret = ERR_INVALID_DATA;
                break;
            }
            auto watermark = std::shared_ptr<Media::PixelMap>(data.ReadParcelable<Media::PixelMap>());
            if (watermark == nullptr) {
                ret = ERR_NULL_OBJECT;
                RS_LOGE("RSServiceToRenderStub::SET_WATERMARK:std::shared_ptr<Media::PixelMap> watermark == nullptr");
                break;
            }
            bool success;
            if (SetWatermark(callingPid, name, watermark, success) != ERR_OK || !success) {
                RS_LOGE("RSServiceToRenderStub::SetWatermark failed");
            }
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SHOW_WATERMARK): {
            std::shared_ptr<Media::PixelMap> watermarkImg =
                std::shared_ptr<Media::PixelMap>(data.ReadParcelable<Media::PixelMap>());
            bool isShow{false};
            if (!data.ReadBool(isShow)) {
                RS_LOGE("RSServiceToRenderStub::SHOW_WATERMARK Read isShow failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            if (!watermarkImg) {
                RS_LOGE("RSServiceToRenderStub::SHOW_WATERMARK watermarkImg is nullptr");
                ret = ERR_INVALID_DATA;
                break;
            }
            ShowWatermark(watermarkImg, isShow);
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_SURFACE_ROOT_NODE): {
            NodeId windowNodeId{UINT64_MAX};
            if (!data.ReadUint64(windowNodeId)) {
                RS_LOGE("RSServiceToRenderStub::GET_SURFACE_ROOT_NODE Read windowId failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            GetSurfaceRootNodeId(windowNodeId);
            if (!reply.WriteUint64(windowNodeId)) {
                RS_LOGE("RSServiceToRenderStub::GET_SURFACE_ROOT_NODE Write result failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_FORCE_REFRESH) : {
            std::string nodeIdStr;
            bool isForceRefresh{false};
            if (!data.ReadString(nodeIdStr) ||
                !data.ReadBool(isForceRefresh)) {
                RS_LOGE("RSServiceToRenderStub::SET_FORCE_REFRESH Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            SetForceRefresh(nodeIdStr, isForceRefresh);
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::DFX_DUMP): {
            std::vector<std::u16string> args;
            if (!data.ReadString16Vector(&args)) {
                RS_LOGE("RSRenderServiceStub::DFX_DUMP Read args failed!");
                ret = ERR_INVALID_DATA;
            }

            std::unordered_set<std::u16string> argSets;
            for (decltype(args.size()) index = 0; index < args.size(); ++index) {
                argSets.insert(args[index]);
            }
            DoDump(argSets);
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::NOTIFY_PACKAGE_EVENT): {
            uint32_t listSize{0};
            if (!data.ReadUint32(listSize)) {
                RS_LOGE("RSClientToServiceConnectionStub::NOTIFY_PACKAGE_EVENT Read listSize failed!");
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
                    RS_LOGE("RSClientToServiceConnectionStub::NOTIFY_PACKAGE_EVENT Read package failed!");
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
#ifdef RS_ENABLE_OVERLAY_DISPLAY
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_OVERLAY_DISPLAY_MODE) : {
            RS_LOGI("RSServiceToRenderConnectionStub::OnRemoteRequest SET_OVERLAY_DISPLAY_MODE");
            int32_t mode{0};
            if (!data.ReadInt32(mode)) {
                ret = ERR_INVALID_DATA;
                break;
            }
            if (SetOverlayDisplayMode(mode) != ERR_OK) {
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
#endif
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REPORT_EVENT_GAMESTATE): {
            GameStateData info;
            if (!data.ReadInt32(info.pid) || !data.ReadInt32(info.uid) ||
                !data.ReadInt32(info.state) || !data.ReadInt32(info.renderTid) ||
                !data.ReadString(info.bundleName)) {
                RS_LOGE("RSServiceToRenderStub::ReadGameStateDataRs Read parcel failed!");
                break;
            }
            ReportGameStateData(info);
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_BEHIND_WINDOW_FILTER_ENABLED): {
            bool enabled {false};
            if(!data.ReadBool(enabled)) {
                RS_LOGE("RSServiceToRenderStub::SET_BEHIND_WINDOW_FILTER_ENABLED Read enabled failed");
                ret = ERR_INVALID_DATA;
                break;
            }
            auto err = SetBehindWindowFilterEnabled(enabled);
            if (err != ERR_OK) {
                RS_LOGE("RSServiceToRenderStub::SET_BEHIND_WINDOW_FILTER_ENABLED Write status failed");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_BEHIND_WINDOW_FILTER_ENABLED): {
            bool enabled;
            if (GetBehindWindowFilterEnabled(enabled) != ERR_OK || !reply.WriteBool(enabled)) {
                RS_LOGE("RSServiceToRenderStub::GET_BEHIND_WINDOW_FILTER_ENABLED Write enabled failed");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_MEMORY_GRAPHIC): {
            int32_t pid{0};
            if (!data.ReadInt32(pid)) {
                RS_LOGE("RSServiceToRenderStub::GET_MEMORY_GRAPHIC Read pid failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            // 安全校验
            // RS_PROFILER_PATCH_PID(data, pid);
            // if (!accessible && !IsValidCallingPid(pid, callingPid)) {
            //     RS_LOGW("RSServiceToRenderStub::GET_MEMORY_GRAPHIC invalid pid[%{public}d]", callingPid);
            //     ret = ERR_INVALID_DATA;
            //     break;
            // }
            MemoryGraphic memoryGraphic;
            if (GetMemoryGraphic(pid, memoryGraphic) != ERR_OK || !reply.WriteParcelable(&memoryGraphic)) {
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REGISTER_TYPEFACE): {
            bool xcollieFlag = false;
            bool result = false;
            std::shared_ptr<Drawing::Typeface> typeface = nullptr;
            {
                // timer: 3s
                OHOS::Rosen::RSXCollie registerTypefaceXCollie("registerTypefaceXCollie_", 3, TypefaceXcollieCallback,
                    &xcollieFlag, 0);
                uint64_t uniqueId{0};
                uint32_t hash{0};
                if (!data.ReadUint64(uniqueId) || !data.ReadUint32(hash)) {
                    RS_LOGE("RSServiceToRenderStub::REGISTER_TYPEFACE read parcel failed!");
                    ret = ERR_INVALID_DATA;
                    break;
                }
                result = RSMarshallingHelper::Unmarshalling(data, typeface);
                if (result && typeface) {
                    typeface->SetHash(hash);
                    RS_PROFILER_PATCH_TYPEFACE_GLOBALID(data, uniqueId);
                    RegisterTypeface(uniqueId, typeface);
                }
            }
            if (xcollieFlag && typeface) {
                RS_LOGW("RSServiceToRenderStub::OnRemoteRequest typeface[%{public}s], size[%{public}u], too big.",
                    typeface->GetFamilyName().c_str(), typeface->GetSize());
            }
            if (!reply.WriteBool(result)) {
                RS_LOGE("RSServiceToRenderStub::REGISTER_TYPEFACE Write result failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::UNREGISTER_TYPEFACE): {
            uint64_t uniqueId{0};
            if (!data.ReadUint64(uniqueId)) {
                RS_LOGE("RSServiceToRenderStub::UNREGISTER_TYPEFACE read uniqueId failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            RS_PROFILER_PATCH_TYPEFACE_GLOBALID(data, uniqueId);
            UnRegisterTypeface(uniqueId);
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REGISTER_UIEXTENSION_CALLBACK): {
            uint64_t userId{0};
            pid_t pid;
            if (!data.ReadInt32(pid)) {
                RS_LOGE("RSServiceToRenderStub::REGISTER_UIEXTENSION_CALLBACK Read "
                        "pid failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            if (!data.ReadUint64(userId)) {
                RS_LOGE("RSServiceToRenderStub::REGISTER_UIEXTENSION_CALLBACK Read "
                        "userId failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                RS_LOGE("RSServiceToRenderStub::REGISTER_UIEXTENSION_CALLBACK Read remoteObject failed!");
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
                RS_LOGE("RSServiceToRenderStub::REGISTER_UIEXTENSION_CALLBACK Read unobscured failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            int32_t status = RegisterUIExtensionCallback(pid, userId, callback, unobscured);
            if (!reply.WriteInt32(status)) {
                RS_LOGE("RSServiceToRenderStub::REGISTER_UIEXTENSION_CALLBACK Write status failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::CREATE_PIXEL_MAP_FROM_SURFACE): {
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                if (!reply.WriteInt32(0)) {
                    RS_LOGE("RSClientToRenderConnectionStub::CREATE_PIXEL_MAP_FROM_SURFACE Write Object failed!");
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
                    RS_LOGE("RSClientToRenderConnectionStub::CREATE_PIXEL_MAP_FROM_SURFACE Write parcel failed!");
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
                RS_LOGE("RSClientToRenderConnectionStub::CREATE_PIXEL_MAP_FROM_SURFACE Read parcel failed!");
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
                    RS_LOGE("RSClientToRenderConnectionStub::CREATE_PIXEL_MAP_FROM_SURFACE Read parcel failed");
                    ret = ERR_INVALID_REPLY;
                    break;
                }
                if (!pixelMap->Marshalling(reply)) {
                    RS_LOGE("pixelMap Marshalling fail");
                    ret = ERR_INVALID_REPLY;
                }
            } else {
                if (!reply.WriteBool(false)) {
                    RS_LOGE("RSClientToRenderConnectionStub::CREATE_PIXEL_MAP_FROM_SURFACE Read parcel failed!");
                    ret = ERR_INVALID_REPLY;
                    break;
                }
            }
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_PID_GPU_MEMORY_IN_MB): {
            int32_t pid{0};
            float gpuMemInMB{0.0};
            if (!data.ReadInt32(pid)) {
                RS_LOGE("RSServiceToRenderStub::GET_PID_GPU_MEMORY_IN_MB : read data err!");
                ret = ERR_INVALID_DATA;
                break;
            }
            ret = GetPidGpuMemoryInMB(pid, gpuMemInMB);
            if (ret != 0) {
                RS_LOGE("RSServiceToRenderStub::GET_PID_GPU_MEMORY_IN_MB : read ret err!");
                ret = ERR_INVALID_DATA;
                break;
            }
            if (!reply.WriteFloat(gpuMemInMB)) {
                RS_LOGE("RSServiceToRenderStub::GET_PID_GPU_MEMORY_IN_MB write gpuMemInMB err!");
                ret = ERR_INVALID_REPLY;
            }
            RS_LOGD("RSServiceToRenderStub::GET_PID_GPU_MEMORY_IN_MB, ret: %{public}d, gpuMemInMB: %{public}f",
                ret, gpuMemInMB);
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::HGM_FORCE_UPDATE_TASK): {
            bool flag{false};
            if (!data.ReadBool(flag)) {
                ret = ERR_INVALID_DATA;
                break;
            }
            std::string fromWhom;
            if (!data.ReadString(fromWhom)) {
                ret = ERR_INVALID_DATA;
                break;
            }
            RS_LOGI("dmulti_process HGM_FORCE_UPDATE_TASK flag[%{public}d] fromWhom[%{public}s]", flag, fromWhom.c_str());
            HgmForceUpdateTask(flag, fromWhom);
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REPAINT_EVERYTHING): {
            auto replyMessage = RepaintEverything();
            RS_LOGI("REPAINT_EVERYTHING replyMsg: %{public}d", replyMessage);
            reply.WriteInt32(replyMessage);
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_COLOR_FOLLOW): {
            std::string nodeIdStr;
            bool isColorFollow { false };
            if (!data.ReadString(nodeIdStr) || !data.ReadBool(isColorFollow)) {
                RS_LOGE("RSServiceToRenderStub::SET_COLOR_FOLLOW Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            SetColorFollow(nodeIdStr, isColorFollow);
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_FREE_MULTI_WINDOW_STATUS): {
            bool enable{false};
            if (!data.ReadBool(enable)) {
                RS_LOGE("RSServiceToRenderStub::SET_FREE_MULTI_WINDOW_STATUS read uniqueId failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            SetFreeMultiWindowStatus(enable);
            break;
        }
        case static_cast<uint32_t>(
            RSIServiceToRenderConnectionInterfaceCode::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK): {
            pid_t remotePid;
            if (!data.ReadInt32(remotePid)) {
                ROSEN_LOGE("RSIServiceToRenderConnectionStub::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK Read "
                           "remotePid failed");
                ret = ERR_INVALID_REPLY;
                break;
            }
            uint32_t size;
            if (!data.ReadUint32(size)) {
                ROSEN_LOGE(
                    "RSIServiceToRenderConnectionStub::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK Read size failed");
                ret = ERR_INVALID_REPLY;
                break;
            }
            RectConstraint constraint;
            if (size > MAX_PID_SIZE_NUMBER) {
                ROSEN_LOGE("RSIServiceToRenderConnectionStub::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK "
                           "size number is too large.");
                break;
            }
            for (uint32_t i = 0; i < size; ++i) {
                pid_t pid;
                if (!data.ReadInt32(pid)) {
                    ROSEN_LOGE("RSIServiceToRenderConnectionStub::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK Read "
                               "pid failed");
                    ret = ERR_INVALID_REPLY;
                    break;
                }
                constraint.pids.insert(pid);
            }
            if (!data.ReadInt32(constraint.range.lowLimit.width) || !data.ReadInt32(constraint.range.lowLimit.height) ||
                !data.ReadInt32(constraint.range.highLimit.width) ||
                !data.ReadInt32(constraint.range.highLimit.height)) {
                ROSEN_LOGE("RSIServiceToRenderConnectionStub::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK Read "
                           "rectRange failed");
                ret = ERR_INVALID_REPLY;
                break;
            }
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            sptr<RSISelfDrawingNodeRectChangeCallback> callback =
                iface_cast<RSISelfDrawingNodeRectChangeCallback>(remoteObject);
            if (callback == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            int32_t status = RegisterSelfDrawingNodeRectChangeCallback(remotePid, constraint, callback);
            if (!reply.WriteInt32(status)) {
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(
            RSIServiceToRenderConnectionInterfaceCode::UNREGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK): {
            pid_t remotePid;
            if (!data.ReadInt32(remotePid)) {
                ROSEN_LOGE("RSIServiceToRenderConnectionStub::UNREGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK Read "
                           "remotePid failed");
                ret = ERR_INVALID_REPLY;
                break;
            }
            int32_t status = UnRegisterSelfDrawingNodeRectChangeCallback(remotePid);
            if (!reply.WriteInt32(status)) {
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_REALTIME_REFRESH_RATE): {
            ScreenId id{INVALID_SCREEN_ID};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSServiceToRenderStub::GET_REALTIME_REFRESH_RATE Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            uint32_t refreshRate = GetRealtimeRefreshRate(id);
            if (!reply.WriteUint32(refreshRate)) {
                RS_LOGE("RSServiceToRenderStub::GET_REALTIME_REFRESH_RATE Read refreshRate failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_SHOW_REFRESH_RATE_ENABLED): {
            bool enabled{false};
            int32_t type{0};
            if (!data.ReadBool(enabled) || !data.ReadInt32(type)) {
                RS_LOGE("RSServiceToRenderStub::SET_SHOW_REFRESH_RATE_ENABLED Read parcel failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            SetShowRefreshRateEnabled(enabled, type);
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_SHOW_REFRESH_RATE_ENABLED): {
            bool enable = false;
            if (GetShowRefreshRateEnabled(enable) != ERR_OK || !reply.WriteBool(enable)) {
                RS_LOGE("RSServiceToRenderStub::GET_SHOW_REFRESH_RATE_ENABLED Write enabled failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::HANDLE_HWC_EVENT): {
            uint32_t deviceId{0};
            if (!data.ReadUint32(deviceId)) {
                RS_LOGE("HandleHwcEvent: read deviceId err.");
                ret = ERR_INVALID_DATA;
                break;
            }
            uint32_t eventId{0};
            if (!data.ReadUint32(eventId)) {
                RS_LOGE("HandleHwcEvent: read eventId err.");
                ret = ERR_INVALID_DATA;
                break;
            }
            std::vector<int32_t> eventData;
            if (!data.ReadInt32Vector(&eventData)) {
                RS_LOGE("HandleHwcEvent: read eventData err.");
                ret = ERR_INVALID_DATA;
                break;
            }
            HandleHwcEvent(deviceId, eventId, eventData);
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_GPU_CRC_DIRTY_ENABLED_PIDLIST): {
            std::vector<int32_t> pidList;
            if (!data.ReadInt32Vector(&pidList)) {
                RS_LOGE("SetGpuCrcDirtyEnabledPidList: read pidList err.");
                ret = ERR_INVALID_DATA;
                break;
            }
            SetGpuCrcDirtyEnabledPidList(pidList);
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_ACTIVE_DIRTY_REGION_INFO) : {
            const auto& activeDirtyRegionInfos = GetActiveDirtyRegionInfo();
            if (!reply.WriteInt32(activeDirtyRegionInfos.size())) {
                RS_LOGE("RSClientToRenderConnectionStub::GET_ACTIVE_DIRTY_REGION_INFO Write activeDirtyRegionInfosSize "
                        "failed!");
                ret = ERR_INVALID_REPLY;
                break;
            }
            for (const auto& activeDirtyRegionInfo : activeDirtyRegionInfos) {
                if (!reply.WriteInt64(activeDirtyRegionInfo.activeDirtyRegionArea) ||
                    !reply.WriteInt32(activeDirtyRegionInfo.activeFramesNumber) ||
                    !reply.WriteInt32(activeDirtyRegionInfo.pidOfBelongsApp) ||
                    !reply.WriteString(activeDirtyRegionInfo.windowName)) {
                    RS_LOGE("RSServiceToRenderStub::GET_ACTIVE_DIRTY_REGION_INFO Write activeDirtyRegionInfos "
                            "failed!");
                    ret = ERR_INVALID_REPLY;
                    break;
                }
            }
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_GLOBAL_DIRTY_REGION_INFO) : {
            const auto& globalDirtyRegionInfo = GetGlobalDirtyRegionInfo();
            if (!reply.WriteInt64(globalDirtyRegionInfo.globalDirtyRegionAreas) ||
                !reply.WriteInt32(globalDirtyRegionInfo.globalFramesNumber) ||
                !reply.WriteInt32(globalDirtyRegionInfo.skipProcessFramesNumber) ||
                !reply.WriteInt32(globalDirtyRegionInfo.mostSendingPidWhenDisplayNodeSkip)) {
                RS_LOGE(
                    "RSServiceToRenderStub::GET_GLOBAL_DIRTY_REGION_INFO Write globalDirtyRegionInfo failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_LAYER_COMPOSE_INFO) : {
            const auto& layerComposeInfo = GetLayerComposeInfo();
            if (!reply.WriteInt32(layerComposeInfo.uniformRenderFrameNumber) ||
                !reply.WriteInt32(layerComposeInfo.offlineComposeFrameNumber) ||
                !reply.WriteInt32(layerComposeInfo.redrawFrameNumber)) {
                RS_LOGE("RSServiceToRenderStub::GET_LAYER_COMPOSE_INFO Write LayerComposeInfo failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(
            RSIServiceToRenderConnectionInterfaceCode::GET_HARDWARE_COMPOSE_DISABLED_REASON_INFO): {
            const auto& hwcDisabledReasonInfos = GetHwcDisabledReasonInfo();
            if (!reply.WriteInt32(hwcDisabledReasonInfos.size())) {
                RS_LOGE("RSServiceToRenderStub::GET_HARDWARE_COMPOSE_DISABLED_REASON_INFO Write "
                        "hwcDisabledReasonInfos failed!");
                ret = ERR_INVALID_REPLY;
                break;
            }
            for (const auto& hwcDisabledReasonInfo : hwcDisabledReasonInfos) {
                for (const auto& disabledReasonCount : hwcDisabledReasonInfo.disabledReasonStatistics) {
                    if (!reply.WriteInt32(disabledReasonCount)) {
                        RS_LOGE("RSServiceToRenderStub::GET_HARDWARE_COMPOSE_DISABLED_REASON_INFO Write "
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
                    RS_LOGE("RSServiceToRenderStub::GET_HARDWARE_COMPOSE_DISABLED_REASON_INFO Write "
                            "hwcDisabledReasonInfo failed!");
                    ret = ERR_INVALID_REPLY;
                    break;
                }
            }
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_HDR_ON_DURATION) : {
            int64_t hdrOnDuration = 0;
            auto errCode = GetHdrOnDuration(hdrOnDuration);
            if (errCode != ERR_OK || !reply.WriteInt64(hdrOnDuration)) {
                RS_LOGE("RSServiceToRenderStub::GET_HDR_ON_DURATION Write "
                        "hdrOnDuration failed!");
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::NOTIFY_SCREEN_REFRESH): {
            ScreenId id{INVALID_SCREEN_ID};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSServiceToRenderStub::NOTIFY_SCREEN_REFRESH Read id failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            NotifyScreenRefresh(id);
            break;
        }
        case static_cast<uint32_t>(
            RSIServiceToRenderConnectionInterfaceCode::SET_OPTIMIZE_CANVAS_DIRTY_ENABLED_PIDLIST): {
            std::vector<int32_t> pidList;
            if (!data.ReadInt32Vector(&pidList)) {
                RS_LOGE(
                    "RSServiceToRenderStub::SET_OPTIMIZE_CANVAS_DIRTY_ENABLED_PIDLIST Read pidList failed!");
                ret = ERR_INVALID_REPLY;
                break;
            }
            ret = SetOptimizeCanvasDirtyPidList(pidList);
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_CURTAIN_SCREEN_USING_STATUS): {
            bool isCurtainScreenOn{false};
            if (!data.ReadBool(isCurtainScreenOn)) {
                RS_LOGE("RSServiceToRenderStub::SET_CURTAIN_SCREEN_USING_STATUS Read "
                        "isCurtainScreenOn failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            SetCurtainScreenUsingStatus(isCurtainScreenOn);
            break;
        }
        case static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_BACKLIGHT_LEVEL) : {
            ScreenId id{INVALID_SCREEN_ID};
            if (!data.ReadUint64(id)) {
                RS_LOGE("RSServiceToRenderStub::SET_BACKLIGHT_LEVEL Read ScreenId failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            uint32_t level{};
            if (!data.ReadUint32(level)) {
                RS_LOGE("RSServiceToRenderStub::SET_BACKLIGHT_LEVEL Read level failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            OnScreenBacklightChanged(id, level);
            break;
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return ret;
}

bool RSServiceToRenderConnectionStub::ReadDataBaseRs(DataBaseRs& info, MessageParcel& data)
{
    if (!data.ReadInt32(info.appPid) || !data.ReadInt32(info.eventType) ||
        !data.ReadInt32(info.versionCode) || !data.ReadInt64(info.uniqueId) ||
        !data.ReadInt64(info.inputTime) || !data.ReadInt64(info.beginVsyncTime) ||
        !data.ReadInt64(info.endVsyncTime) || !data.ReadBool(info.isDisplayAnimator) ||
        !data.ReadString(info.sceneId) || !data.ReadString(info.versionName) ||
        !data.ReadString(info.bundleName) || !data.ReadString(info.processName) ||
        !data.ReadString(info.abilityName) ||!data.ReadString(info.pageUrl) ||
        !data.ReadString(info.sourceType) || !data.ReadString(info.note)) {
        RS_LOGE("RSServiceToRenderConnectionStub::ReadDataBaseRs Read parcel failed!");
        return false;
    }
    return true;
}

bool RSServiceToRenderConnectionStub::ReadAppInfo(AppInfo& info, MessageParcel& data)
{
    if (!data.ReadInt64(info.startTime)) {
        RS_LOGE("RSServiceToRenderConnectionStub::ReadAppInfo Read startTime failed!");
        return false;
    }
    if (!data.ReadInt64(info.endTime)) {
        RS_LOGE("RSServiceToRenderConnectionStub::ReadAppInfo Read endTime failed!");
        return false;
    }
    if (!data.ReadInt32(info.pid)) {
        RS_LOGE("RSServiceToRenderConnectionStub::ReadAppInfo Read pid failed!");
        return false;
    }
    if (!data.ReadString(info.versionName)) {
        RS_LOGE("RSServiceToRenderConnectionStub::ReadAppInfo Read versionName failed!");
        return false;
    }
    if (!data.ReadInt32(info.versionCode)) {
        RS_LOGE("RSServiceToRenderConnectionStub::ReadAppInfo Read versionCode failed!");
        return false;
    }
    if (!data.ReadString(info.bundleName)) {
        RS_LOGE("RSServiceToRenderConnectionStub::ReadAppInfo Read bundleName failed!");
        return false;
    }
    if (!data.ReadString(info.processName)) {
        RS_LOGE("RSServiceToRenderConnectionStub::ReadAppInfo Read processName failed!");
        return false;
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS