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
#include "ivsync_connection.h"
#include "securec.h"
#include "sys_binder.h"

#include "command/rs_command_factory.h"
#include "hgm_frame_rate_manager.h"
#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "pipeline/rs_unmarshal_thread.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_ashmem_helper.h"
#include "rs_trace.h"
#include "rs_profiler.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t MAX_DATA_SIZE_FOR_UNMARSHALLING_IN_PLACE = 1024 * 15; // 15kB
constexpr size_t FILE_DESCRIPTOR_LIMIT = 15;
constexpr size_t MAX_OBJECTNUM = INT_MAX;
constexpr size_t MAX_DATA_SIZE = INT_MAX;

void CopyFileDescriptor(MessageParcel& old, MessageParcel& copied)
{
    binder_size_t* object = reinterpret_cast<binder_size_t*>(old.GetObjectOffsets());
    binder_size_t* copiedObject = reinterpret_cast<binder_size_t*>(copied.GetObjectOffsets());

    size_t objectNum = old.GetOffsetsSize();
    if (objectNum > MAX_OBJECTNUM) {
        return;
    }

    uintptr_t data = old.GetData();
    uintptr_t copiedData = copied.GetData();

    for (size_t i = 0; i < objectNum; i++) {
        const flat_binder_object* flat = reinterpret_cast<flat_binder_object*>(data + object[i]);
        flat_binder_object* copiedFlat = reinterpret_cast<flat_binder_object*>(copiedData + copiedObject[i]);

        if (flat->hdr.type == BINDER_TYPE_FD && flat->handle > 0) {
            int32_t val = dup(flat->handle);
            if (val < 0) {
                ROSEN_LOGW("CopyFileDescriptor dup failed, fd:%{public}d", val);
                return;
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
    if (parcelCopied->ReadInt32() != 0) {
        RS_LOGE("RSRenderServiceConnectionStub::CopyParcelIfNeed parcel data not match");
        return nullptr;
    }
    return parcelCopied;
}
}

int RSRenderServiceConnectionStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    RS_PROFILER_ON_REMOTE_REQUEST(this, code, data, reply, option);

    pid_t callingPid = GetCallingPid();
#ifdef ENABLE_IPC_SECURITY_ACCESS_COUNTER
    auto accessCount = securityUtils_.GetCodeAccessCounter(code);
    if (!securityManager_.IsAccessTimesRestricted(code, accessCount)) {
        RS_LOGE("RSRenderServiceConnectionStub::OnRemoteRequest This Function[ID=%{public}u] invoke times:%{public}d"
            "by pid[%{public}d]", code, accessCount, callingPid);
        return ERR_INVALID_STATE;
    }
#endif
    static constexpr std::array descriptorCheckList = {
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_FOCUS_APP_INFO),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_DEFAULT_SCREEN_ID),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ACTIVE_SCREEN_ID),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ALL_SCREEN_IDS),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_RESOLUTION),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_SURFACE),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_BLACKLIST),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REMOVE_VIRTUAL_SCREEN),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_CHANGE_CALLBACK),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_ACTIVE_MODE),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_REFRESH_RATE),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_REFRESH_RATE_MODE),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SYNC_FRAME_RATE_RANGE),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_CURRENT_REFRESH_RATE),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_CURRENT_REFRESH_RATE_MODE),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_REFRESH_RATES),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SHOW_REFRESH_RATE_ENABLED),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SHOW_REFRESH_RATE_ENABLED),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::MARK_POWER_OFF_NEED_PROCESS_ONE_FRAME),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::DISABLE_RENDER_CONTROL_SCREEN),
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
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE),
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
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_OCCLUSION_CHANGE_CALLBACK),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_APP_WINDOW_NUM),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SYSTEM_ANIMATED_SCENES),
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
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_RESPONSE),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_COMPLETE),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_JANK_FRAME),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_GAMESTATE),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_TOUCH_EVENT),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_DYNAMIC_MODE_EVENT),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_HARDWARE_ENABLED),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_HGM_CFG_CALLBACK),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_ROTATION_CACHE_ENABLED),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_TP_FEATURE_CONFIG),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_USING_STATUS),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REFRESH_RATE_MODE_CHANGE_CALLBACK),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_CURTAIN_SCREEN_USING_STATUS),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_TYPEFACE),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_TYPEFACE),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REFRESH_RATE_UPDATE_CALLBACK),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ACTIVE_DIRTY_REGION_INFO),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_GLOBAL_DIRTY_REGION_INFO),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_LAYER_COMPOSE_INFO),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_CAST_SCREEN_ENABLE_SKIP_WINDOW),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_UIEXTENSION_CALLBACK),
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_STATUS)
    };
    if (std::find(std::cbegin(descriptorCheckList), std::cend(descriptorCheckList), code) !=
        std::cend(descriptorCheckList)) {
        auto token = data.ReadInterfaceToken();
        if (token != RSIRenderServiceConnection::GetDescriptor()) {
            if (code == static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_PIXEL_MAP_FROM_SURFACE)) {
                reply.WriteInt32(0);
            }
            return ERR_INVALID_STATE;
        }
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
            RS_TRACE_NAME_FMT("Recv Parcel Size:%zu, fdCnt:%zu", data.GetDataSize(), data.GetOffsetsSize());
            static bool isUniRender = RSUniRenderJudgement::IsUniRender();
            std::shared_ptr<MessageParcel> parsedParcel;
            if (data.ReadInt32() == 0) { // indicate normal parcel
                if (isUniRender) {
                    // in uni render mode, if parcel size over threshold,
                    // Unmarshalling task will be post to RSUnmarshalThread,
                    // copy the origin parcel to maintain the parcel lifetime
                    parsedParcel = CopyParcelIfNeed(data, callingPid);
                }
                if (parsedParcel == nullptr) {
                    // no need to copy or copy failed, use original parcel
                    // execute Unmarshalling immediately
                    auto transactionData = RSBaseRenderUtil::ParseTransactionData(data);
                    if (transactionData && isNonSystemAppCalling) {
                        const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
                        pid_t conflictCommandPid = 0;
                        std::string commandMapDesc = "";
                        if (!transactionData->IsCallingPidValid(callingPid, nodeMap, conflictCommandPid,
                                                                commandMapDesc)) {
                            RS_LOGE("RSRenderServiceConnectionStub::COMMIT_TRANSACTION non-system callingPid %{public}d"
                                    " is denied to access commandPid %{public}d, commandMap = %{public}s",
                                    callingPid, conflictCommandPid, commandMapDesc.c_str());
                            return ERR_INVALID_STATE;
                        }
                    }
                    CommitTransaction(transactionData);
                    break;
                }
            } else {
                // indicate ashmem parcel
                // should be parsed to normal parcel before Unmarshalling
                parsedParcel = RSAshmemHelper::ParseFromAshmemParcel(&data);
            }
            if (parsedParcel == nullptr) {
                RS_LOGE("RSRenderServiceConnectionStub::COMMIT_TRANSACTION failed: parsed parcel is nullptr");
                return ERR_INVALID_DATA;
            }
            if (isUniRender) {
                // post Unmarshalling task to RSUnmarshalThread
                RSUnmarshalThread::Instance().RecvParcel(parsedParcel, isNonSystemAppCalling, callingPid);
            } else {
                // execute Unmarshalling immediately
                auto transactionData = RSBaseRenderUtil::ParseTransactionData(*parsedParcel);
                if (transactionData && isNonSystemAppCalling) {
                    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
                    pid_t conflictCommandPid = 0;
                    std::string commandMapDesc = "";
                    if (!transactionData->IsCallingPidValid(callingPid, nodeMap, conflictCommandPid, commandMapDesc)) {
                        RS_LOGE("RSRenderServiceConnectionStub::COMMIT_TRANSACTION non-system callingPid %{public}d"
                                " is denied to access commandPid %{public}d, commandMap = %{public}s",
                                callingPid, conflictCommandPid, commandMapDesc.c_str());
                        return ERR_INVALID_STATE;
                    }
                }
                CommitTransaction(transactionData);
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_UNI_RENDER_ENABLED): {
            reply.WriteBool(GetUniRenderEnabled());
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_NODE): {
            auto nodeId = data.ReadUint64();
            RS_PROFILER_PATCH_NODE_ID(data, nodeId);
            auto surfaceName = data.ReadString();
            auto bundleName = data.ReadString();
            RSSurfaceRenderNodeConfig config = {.id = nodeId, .name = surfaceName, .bundleName = bundleName};
            reply.WriteBool(CreateNode(config));
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_NODE_AND_SURFACE): {
            auto nodeId = data.ReadUint64();
            RS_PROFILER_PATCH_NODE_ID(data, nodeId);
            auto surfaceName = data.ReadString();
            auto type = static_cast<RSSurfaceNodeType>(data.ReadUint8());
            auto bundleName = data.ReadString();
            bool isTextureExportNode = data.ReadBool();
            bool isSync = data.ReadBool();
            auto surfaceWindowType = static_cast<SurfaceWindowType>(data.ReadUint8());
            RSSurfaceRenderNodeConfig config = {
                .id = nodeId, .name = surfaceName, .bundleName = bundleName, .nodeType = type,
                .isTextureExportNode = isTextureExportNode, .isSync = isSync,
                .surfaceWindowType = surfaceWindowType};
            sptr<Surface> surface = CreateNodeAndSurface(config);
            if (surface == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            auto producer = surface->GetProducer();
            reply.WriteRemoteObject(producer->AsObject());
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_FOCUS_APP_INFO): {
            int32_t pid = data.ReadInt32();
            RS_PROFILER_PATCH_PID(data, pid);
            int32_t uid = data.ReadInt32();
            std::string bundleName = data.ReadString();
            std::string abilityName = data.ReadString();
            uint64_t focusNodeId = data.ReadUint64();
            RS_PROFILER_PATCH_NODE_ID(data, focusNodeId);
            int32_t status = SetFocusAppInfo(pid, uid, bundleName, abilityName, focusNodeId);
            reply.WriteInt32(status);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_DEFAULT_SCREEN_ID): {
            ScreenId id = GetDefaultScreenId();
            reply.WriteUint64(id);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ACTIVE_SCREEN_ID): {
            ScreenId id = GetActiveScreenId();
            reply.WriteUint64(id);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ALL_SCREEN_IDS): {
            std::vector<ScreenId> ids = GetAllScreenIds();
            reply.WriteUint32(ids.size());
            for (uint32_t i = 0; i < ids.size(); i++) {
                reply.WriteUint64(ids[i]);
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN): {
            // read the parcel data.
            std::string name = data.ReadString();
            uint32_t width = data.ReadUint32();
            uint32_t height = data.ReadUint32();
            sptr<Surface> surface = nullptr;
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject != nullptr) {
                auto bufferProducer = iface_cast<IBufferProducer>(remoteObject);
                surface = Surface::CreateSurfaceAsProducer(bufferProducer);
            }

            ScreenId mirrorId = data.ReadUint64();
            int32_t flags = data.ReadInt32();
            std::vector<NodeId> whiteList;
            data.ReadUInt64Vector(&whiteList);
            ScreenId id = CreateVirtualScreen(name, width, height, surface, mirrorId, flags, whiteList);
            reply.WriteUint64(id);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_BLACKLIST): {
            // read the parcel data.
            ScreenId id = data.ReadUint64();
            std::vector<NodeId> blackListVector;
            data.ReadUInt64Vector(&blackListVector);
            int32_t status = SetVirtualScreenBlackList(id, blackListVector);
            reply.WriteInt32(status);
            break;
        }
        case static_cast<uint32_t>(
            RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST): {
            if (!securityManager_.IsInterfaceCodeAccessible(code)) {
                RS_LOGE("RSRenderServiceConnectionStub::OnRemoteRequest no permission"
                    "SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST");
                return ERR_INVALID_STATE;
            }
            // read the parcel data.
            ScreenId id = data.ReadUint64();
            std::vector<NodeId> securityExemptionList;
            if (!data.ReadUInt64Vector(&securityExemptionList)) {
                ret = ERR_INVALID_REPLY;
                break;
            }
            int32_t status = SetVirtualScreenSecurityExemptionList(id, securityExemptionList);
            if (!reply.WriteInt32(status)) {
                ret = ERR_INVALID_REPLY;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_CAST_SCREEN_ENABLE_SKIP_WINDOW): {
            // read the parcel data.
            ScreenId id = data.ReadUint64();
            bool enable = data.ReadBool();
            int32_t result = SetCastScreenEnableSkipWindow(id, enable);
            reply.WriteInt32(result);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_SURFACE): {
            // read the parcel data.
            ScreenId id = data.ReadUint64();
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
            reply.WriteInt32(status);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REMOVE_VIRTUAL_SCREEN): {
            ScreenId id = data.ReadUint64();
            RemoveVirtualScreen(id);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_CHANGE_CALLBACK): {
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            sptr<RSIScreenChangeCallback> cb = iface_cast<RSIScreenChangeCallback>(remoteObject);
            if (cb == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            int32_t status = SetScreenChangeCallback(cb);
            reply.WriteInt32(status);
            break;
        }
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_POINTER_COLOR_INVERSION_CONFIG): {
            float darkBuffer = data.ReadFloat();
            float brightBuffer = data.ReadFloat();
            int64_t interval = data.ReadInt64();
            int32_t rangeSize = data.ReadInt32();
            int32_t status = SetPointerColorInversionConfig(darkBuffer, brightBuffer, interval, rangeSize);
            reply.WriteInt32(status);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_POINTER_COLOR_INVERSION_ENABLED): {
            bool enable = data.ReadBool();
            int32_t status = SetPointerColorInversionEnabled(enable);
            reply.WriteInt32(status);
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
            reply.WriteInt32(status);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_POINTER_LUMINANCE_CALLBACK): {
            int32_t status = UnRegisterPointerLuminanceChangeCallback();
            reply.WriteInt32(status);
            break;
        }
#endif
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_ACTIVE_MODE): {
            ScreenId id = data.ReadUint64();
            uint32_t modeId = data.ReadUint32();
            SetScreenActiveMode(id, modeId);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_REFRESH_RATE): {
            ScreenId id = data.ReadUint64();
            int32_t sceneId = data.ReadInt32();
            int32_t rate = data.ReadInt32();
            SetScreenRefreshRate(id, sceneId, rate);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_REFRESH_RATE_MODE): {
            if (!securityManager_.IsInterfaceCodeAccessible(code)) {
                RS_LOGE("RSRenderServiceConnectionStub::OnRemoteRequest no permission to access SET_REFRESH_RATE_MODE");
                return ERR_INVALID_STATE;
            }
            int32_t mode = data.ReadInt32();
            SetRefreshRateMode(mode);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SYNC_FRAME_RATE_RANGE): {
            FrameRateLinkerId id = data.ReadUint64();
            uint32_t min = data.ReadUint32();
            uint32_t max = data.ReadUint32();
            uint32_t preferred = data.ReadUint32();
            uint32_t type = data.ReadUint32();
            int32_t animatorExpectedFrameRate = data.ReadInt32();
            SyncFrameRateRange(id, {min, max, preferred, type}, animatorExpectedFrameRate);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_CURRENT_REFRESH_RATE): {
            ScreenId id = data.ReadUint64();
            uint32_t refreshRate = GetScreenCurrentRefreshRate(id);
            reply.WriteUint32(refreshRate);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_CURRENT_REFRESH_RATE_MODE): {
            int32_t refreshRateMode = GetCurrentRefreshRateMode();
            reply.WriteInt32(refreshRateMode);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_REFRESH_RATES): {
            ScreenId id = data.ReadUint64();
            std::vector<int32_t> rates = GetScreenSupportedRefreshRates(id);
            reply.WriteUint64(static_cast<uint64_t>(rates.size()));
            for (auto ratesIter : rates) {
                reply.WriteInt32(ratesIter);
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SHOW_REFRESH_RATE_ENABLED): {
            if (!securityManager_.IsInterfaceCodeAccessible(code)) {
                RS_LOGE("RSRenderServiceConnectionStub::OnRemoteRequest no permission to access"\
                    "GET_SHOW_REFRESH_RATE_ENABLED");
                return ERR_INVALID_STATE;
            }
            bool enable = GetShowRefreshRateEnabled();
            reply.WriteBool(enable);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SHOW_REFRESH_RATE_ENABLED): {
            if (!securityManager_.IsInterfaceCodeAccessible(code)) {
                RS_LOGE("RSRenderServiceConnectionStub::OnRemoteRequest no permission to access"\
                    "SET_SHOW_REFRESH_RATE_ENABLED");
                return ERR_INVALID_STATE;
            }
            bool enable = data.ReadBool();
            SetShowRefreshRateEnabled(enable);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_REFRESH_INFO): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            pid_t pid = data.ReadInt32();
            std::string refreshInfo = GetRefreshInfo(pid);
            reply.WriteString(refreshInfo);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_RESOLUTION): {
            ScreenId id = data.ReadUint64();
            uint32_t width = data.ReadUint32();
            uint32_t height = data.ReadUint32();
            int32_t status = SetVirtualScreenResolution(id, width, height);
            reply.WriteInt32(status);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::MARK_POWER_OFF_NEED_PROCESS_ONE_FRAME): {
            MarkPowerOffNeedProcessOneFrame();
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::DISABLE_RENDER_CONTROL_SCREEN): {
            ScreenId id = data.ReadUint64();
            DisablePowerOffRenderControl(id);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_POWER_STATUS): {
            if (!securityManager_.IsInterfaceCodeAccessible(code)) {
                RS_LOGE(
                    "RSRenderServiceConnectionStub::OnRemoteRequest no permission to access SET_SCREEN_POWER_STATUS");
                return ERR_INVALID_STATE;
            }
            ScreenId id = data.ReadUint64();
            uint32_t status = data.ReadUint32();
            SetScreenPowerStatus(id, static_cast<ScreenPowerStatus>(status));
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_SURFACE_CAPTURE): {
            NodeId id = data.ReadUint64();
            RS_PROFILER_PATCH_NODE_ID(data, id);
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            sptr<RSISurfaceCaptureCallback> cb = iface_cast<RSISurfaceCaptureCallback>(remoteObject);
            if (cb == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            RSSurfaceCaptureConfig captureConfig;
            captureConfig.scaleX = data.ReadFloat();
            captureConfig.scaleY = data.ReadFloat();
            captureConfig.useDma = data.ReadBool();
            captureConfig.useCurWindow = data.ReadBool();
            captureConfig.captureType = static_cast<SurfaceCaptureType>(data.ReadUint8());
            captureConfig.isSync = data.ReadBool();

            TakeSurfaceCapture(id, cb, captureConfig, securityManager_.IsInterfaceCodeAccessible(code));
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_APPLICATION_AGENT): {
            auto pid = data.ReadInt32();
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
            ScreenId id = data.ReadUint64();
            RSVirtualScreenResolution virtualScreenResolution = GetVirtualScreenResolution(id);
            reply.WriteParcelable(&virtualScreenResolution);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_ACTIVE_MODE): {
            ScreenId id = data.ReadUint64();
            RSScreenModeInfo screenModeInfo = GetScreenActiveMode(id);
            reply.WriteParcelable(&screenModeInfo);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_MODES): {
            ScreenId id = data.ReadUint64();
            std::vector<RSScreenModeInfo> screenSupportedModes = GetScreenSupportedModes(id);
            reply.WriteUint64(static_cast<uint64_t>(screenSupportedModes.size()));
            for (uint32_t modeIndex = 0; modeIndex < screenSupportedModes.size(); modeIndex++) {
                reply.WriteParcelable(&screenSupportedModes[modeIndex]);
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_MEMORY_GRAPHIC): {
            auto pid = data.ReadInt32();
            RS_PROFILER_PATCH_PID(data, pid);
            MemoryGraphic memoryGraphic = GetMemoryGraphic(pid);
            reply.WriteParcelable(&memoryGraphic);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_MEMORY_GRAPHICS): {
            if (!securityManager_.IsInterfaceCodeAccessible(code)) {
                RS_LOGE(
                    "RSRenderServiceConnectionStub::OnRemoteRequest no permission to access GET_MEMORY_GRAPHICS");
                return ERR_INVALID_STATE;
            }
            std::vector<MemoryGraphic> memoryGraphics = GetMemoryGraphics();
            reply.WriteUint64(static_cast<uint64_t>(memoryGraphics.size()));
            for (uint32_t index = 0; index < memoryGraphics.size(); index++) {
                reply.WriteParcelable(&memoryGraphics[index]);
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_TOTAL_APP_MEM_SIZE): {
            float cpuMemSize = 0.f;
            float gpuMemSize = 0.f;
            GetTotalAppMemSize(cpuMemSize, gpuMemSize);
            reply.WriteFloat(cpuMemSize);
            reply.WriteFloat(gpuMemSize);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_CAPABILITY): {
            ScreenId id = data.ReadUint64();
            RSScreenCapability screenCapability = GetScreenCapability(id);
            reply.WriteParcelable(&screenCapability);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_POWER_STATUS): {
            ScreenId id = data.ReadUint64();
            ScreenPowerStatus status = GetScreenPowerStatus(id);
            reply.WriteUint32(static_cast<uint32_t>(status));
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_DATA): {
            ScreenId id = data.ReadUint64();
            RSScreenData screenData = GetScreenData(id);
            reply.WriteParcelable(&screenData);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_BACK_LIGHT): {
            ScreenId id = data.ReadUint64();
            int32_t level = GetScreenBacklight(id);
            reply.WriteInt32(level);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_BACK_LIGHT): {
            ScreenId id = data.ReadUint64();
            uint32_t level = data.ReadUint32();
            SetScreenBacklight(id, level);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_BUFFER_AVAILABLE_LISTENER): {
            NodeId id = data.ReadUint64();
            RS_PROFILER_PATCH_NODE_ID(data, id);
            auto remoteObject = data.ReadRemoteObject();
            bool isFromRenderThread = data.ReadBool();
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
            NodeId id = data.ReadUint64();
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
            ScreenId id = data.ReadUint64();
            std::vector<uint32_t> modeSend;
            std::vector<ScreenColorGamut> mode;
            int32_t result = GetScreenSupportedColorGamuts(id, mode);
            reply.WriteInt32(result);
            if (result != StatusCode::SUCCESS) {
                break;
            }
            std::copy(mode.begin(), mode.end(), std::back_inserter(modeSend));
            reply.WriteUInt32Vector(modeSend);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_METADATAKEYS): {
            ScreenId id = data.ReadUint64();
            std::vector<uint32_t> keySend;
            std::vector<ScreenHDRMetadataKey> keys;
            int32_t result = GetScreenSupportedMetaDataKeys(id, keys);
            reply.WriteInt32(result);
            if (result != StatusCode::SUCCESS) {
                break;
            }
            for (auto i : keys) {
                keySend.push_back(i);
            }
            reply.WriteUInt32Vector(keySend);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_GAMUT): {
            ScreenId id = data.ReadUint64();
            ScreenColorGamut mode;
            int32_t result = GetScreenColorGamut(id, mode);
            reply.WriteInt32(result);
            if (result != StatusCode::SUCCESS) {
                break;
            }
            reply.WriteUint32(mode);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_GAMUT): {
            ScreenId id = data.ReadUint64();
            int32_t modeIdx = data.ReadInt32();
            int32_t result = SetScreenColorGamut(id, modeIdx);
            reply.WriteInt32(result);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_GAMUT_MAP): {
            ScreenId id = data.ReadUint64();
            ScreenGamutMap mode = static_cast<ScreenGamutMap>(data.ReadInt32());
            int32_t result = SetScreenGamutMap(id, mode);
            reply.WriteInt32(result);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_CORRECTION): {
            ScreenId id = data.ReadUint64();
            ScreenRotation screenRotation = static_cast<ScreenRotation>(data.ReadInt32());
            int32_t result = SetScreenCorrection(id, screenRotation);
            reply.WriteInt32(result);
            break;
        }
        case static_cast<uint32_t>(
            RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION): {
            ScreenId id = data.ReadUint64();
            bool canvasRotation = data.ReadBool();
            bool result = SetVirtualMirrorScreenCanvasRotation(id, canvasRotation);
            reply.WriteBool(result);
            break;
        }
        case static_cast<uint32_t>(
            RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE): {
            ScreenId id = data.ReadUint64();
            ScreenScaleMode scaleMode = static_cast<ScreenScaleMode>(data.ReadUint32());
            bool result = SetVirtualMirrorScreenScaleMode(id, scaleMode);
            reply.WriteBool(result);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_GAMUT_MAP): {
            ScreenId id = data.ReadUint64();
            ScreenGamutMap mode;
            int32_t result = GetScreenGamutMap(id, mode);
            reply.WriteInt32(result);
            if (result != StatusCode::SUCCESS) {
                break;
            }
            reply.WriteUint32(mode);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VSYNC_CONNECTION): {
            std::string name = data.ReadString();
            auto remoteObj = data.ReadRemoteObject();
            uint64_t id = data.ReadUint64();
            NodeId windowNodeID = data.ReadUint64();
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
            sptr<IVSyncConnection> conn = CreateVSyncConnection(name, token, id, windowNodeID);
            if (conn == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
#ifdef ENABLE_IPC_SECURITY_ACCESS_COUNTER
            securityUtils_.IncreaseAccessCounter(code);
#endif
            reply.WriteRemoteObject(conn->AsObject());
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_PIXEL_MAP_FROM_SURFACE): {
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                reply.WriteInt32(0);
                ret = ERR_NULL_OBJECT;
                break;
            }
            auto bufferProducer = iface_cast<IBufferProducer>(remoteObject);
            sptr<Surface> surface = Surface::CreateSurfaceAsProducer(bufferProducer);
            if (surface == nullptr) {
                reply.WriteInt32(0);
                ret = ERR_NULL_OBJECT;
                break;
            }
            auto x = data.ReadInt32();
            auto y = data.ReadInt32();
            auto w = data.ReadInt32();
            auto h = data.ReadInt32();
            auto srcRect = Rect {
                .x = x,
                .y = y,
                .w = w,
                .h = h
            };
            std::shared_ptr<Media::PixelMap> pixelMap = CreatePixelMapFromSurface(surface, srcRect);
            if (pixelMap) {
                reply.WriteBool(true);
                if (!pixelMap->Marshalling(reply)) {
                    RS_LOGE("pixelMap Marshalling fail");
                }
            } else {
                reply.WriteBool(false);
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_HDR_CAPABILITY): {
            ScreenId id = data.ReadUint64();
            RSScreenHDRCapability screenHDRCapability;
            int32_t result = GetScreenHDRCapability(id, screenHDRCapability);
            reply.WriteInt32(result);
            if (result != StatusCode::SUCCESS) {
                break;
            }
            reply.WriteParcelable(&screenHDRCapability);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PIXEL_FORMAT): {
            ScreenId id = data.ReadUint64();
            GraphicPixelFormat pixelFormat;
            int32_t result = GetPixelFormat(id, pixelFormat);
            reply.WriteInt32(result);
            if (result != StatusCode::SUCCESS) {
                break;
            }
            reply.WriteUint32(static_cast<uint32_t>(pixelFormat));
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_PIXEL_FORMAT): {
            ScreenId id = data.ReadUint64();
            GraphicPixelFormat pixelFormat = static_cast<GraphicPixelFormat>(data.ReadInt32());
            int32_t result = SetPixelFormat(id, pixelFormat);
            reply.WriteInt32(result);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_HDR_FORMATS): {
            ScreenId id = data.ReadUint64();
            std::vector<uint32_t> hdrFormatsSend;
            std::vector<ScreenHDRFormat> hdrFormats;
            int32_t result = GetScreenSupportedHDRFormats(id, hdrFormats);
            reply.WriteInt32(result);
            if (result != StatusCode::SUCCESS) {
                break;
            }
            std::copy(hdrFormats.begin(), hdrFormats.end(), std::back_inserter(hdrFormatsSend));
            reply.WriteUInt32Vector(hdrFormatsSend);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_HDR_FORMAT): {
            ScreenId id = data.ReadUint64();
            ScreenHDRFormat hdrFormat;
            int32_t result = GetScreenHDRFormat(id, hdrFormat);
            reply.WriteInt32(result);
            if (result != StatusCode::SUCCESS) {
                break;
            }
            reply.WriteUint32(hdrFormat);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_HDR_FORMAT): {
            ScreenId id = data.ReadUint64();
            int32_t modeIdx = data.ReadInt32();
            int32_t result = SetScreenHDRFormat(id, modeIdx);
            reply.WriteInt32(result);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_COLORSPACES): {
            ScreenId id = data.ReadUint64();
            std::vector<uint32_t> colorSpacesSend;
            std::vector<GraphicCM_ColorSpaceType> colorSpaces;
            int32_t result = GetScreenSupportedColorSpaces(id, colorSpaces);
            reply.WriteInt32(result);
            if (result != StatusCode::SUCCESS) {
                break;
            }
            std::copy(colorSpaces.begin(), colorSpaces.end(), std::back_inserter(colorSpacesSend));
            reply.WriteUInt32Vector(colorSpacesSend);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_COLORSPACE): {
            ScreenId id = data.ReadUint64();
            GraphicCM_ColorSpaceType colorSpace;
            int32_t result = GetScreenColorSpace(id, colorSpace);
            reply.WriteInt32(result);
            if (result != StatusCode::SUCCESS) {
                break;
            }
            reply.WriteUint32(colorSpace);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_COLORSPACE): {
            ScreenId id = data.ReadUint64();
            GraphicCM_ColorSpaceType colorSpace = static_cast<GraphicCM_ColorSpaceType>(data.ReadInt32());
            int32_t result = SetScreenColorSpace(id, colorSpace);
            reply.WriteInt32(result);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_TYPE): {
            ScreenId id = data.ReadUint64();
            RSScreenType type;
            int32_t result = GetScreenType(id, type);
            reply.WriteInt32(result);
            if (result != StatusCode::SUCCESS) {
                break;
            }
            reply.WriteUint32(type);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_BITMAP): {
            NodeId id = data.ReadUint64();
            RS_PROFILER_PATCH_NODE_ID(data, id);
            Drawing::Bitmap bm;
            bool result = GetBitmap(id, bm);
            reply.WriteBool(result);
            if (result) {
                RSMarshallingHelper::Marshalling(reply, bm);
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PIXELMAP): {
            NodeId id = data.ReadUint64();
            RS_PROFILER_PATCH_NODE_ID(data, id);
            std::shared_ptr<Media::PixelMap> pixelmap =
                std::shared_ptr<Media::PixelMap>(data.ReadParcelable<Media::PixelMap>());
            Drawing::Rect rect;
            RSMarshallingHelper::Unmarshalling(data, rect);
            std::shared_ptr<Drawing::DrawCmdList> drawCmdList;
            RSMarshallingHelper::Unmarshalling(data, drawCmdList);
            bool result = GetPixelmap(id, pixelmap, &rect, drawCmdList);
            reply.WriteBool(result);
            if (result) {
                RSMarshallingHelper::Marshalling(reply, pixelmap);
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_TYPEFACE): {
            uint64_t uniqueId = data.ReadUint64();
            std::shared_ptr<Drawing::Typeface> typeface;
            bool result = false;
            result = RSMarshallingHelper::Unmarshalling(data, typeface);
            if (result) {
                RegisterTypeface(uniqueId, typeface);
            }
            reply.WriteBool(result);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_TYPEFACE): {
            uint64_t uniqueId = data.ReadUint64();
            UnRegisterTypeface(uniqueId);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_SKIP_FRAME_INTERVAL): {
            ScreenId id = data.ReadUint64();
            uint32_t skipFrameInterval = data.ReadUint32();
            int32_t result = SetScreenSkipFrameInterval(id, skipFrameInterval);
            reply.WriteInt32(result);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_OCCLUSION_CHANGE_CALLBACK): {
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            sptr<RSIOcclusionChangeCallback> callback = iface_cast<RSIOcclusionChangeCallback>(remoteObject);
            if (callback == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            int32_t status = RegisterOcclusionChangeCallback(callback);
            reply.WriteInt32(status);
            break;
        }
        case static_cast<uint32_t>(
            RSIRenderServiceConnectionInterfaceCode::REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK): {
            NodeId id = data.ReadUint64();
            RS_PROFILER_PATCH_NODE_ID(data, id);
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
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
                ret = ERR_TRANSACTION_FAILED;
                break;
            }
            int32_t status = RegisterSurfaceOcclusionChangeCallback(id, callback, partitionPoints);
            reply.WriteInt32(status);
            break;
        }
        case static_cast<uint32_t>(
            RSIRenderServiceConnectionInterfaceCode::UNREGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK): {
            NodeId id = data.ReadUint64();
            RS_PROFILER_PATCH_NODE_ID(data, id);
            int32_t status = UnRegisterSurfaceOcclusionChangeCallback(id);
            reply.WriteInt32(status);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_APP_WINDOW_NUM): {
            uint32_t num = data.ReadUint32();
            SetAppWindowNum(num);
            break;
        }
        case static_cast<uint32_t>(
            RSIRenderServiceConnectionInterfaceCode::SET_SYSTEM_ANIMATED_SCENES): {
            uint32_t systemAnimatedScenes = data.ReadUint32();
            bool result = SetSystemAnimatedScenes(static_cast<SystemAnimatedScenes>(systemAnimatedScenes));
            reply.WriteBool(result);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SHOW_WATERMARK): {
            std::shared_ptr<Media::PixelMap> watermarkImg =
                std::shared_ptr<Media::PixelMap>(data.ReadParcelable<Media::PixelMap>());
            bool isShow = data.ReadBool();
            ShowWatermark(watermarkImg, isShow);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::RESIZE_VIRTUAL_SCREEN): {
            ScreenId id = data.ReadUint64();
            uint32_t width = data.ReadUint32();
            uint32_t height = data.ReadUint32();
            int32_t status = ResizeVirtualScreen(id, width, height);
            reply.WriteInt32(status);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_JANK_STATS): {
            ReportJankStats();
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_RESPONSE): {
            DataBaseRs info;
            ReadDataBaseRs(info, data);
            ReportEventResponse(info);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_COMPLETE): {
            DataBaseRs info;
            ReadDataBaseRs(info, data);
            ReportEventComplete(info);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_JANK_FRAME): {
            DataBaseRs info;
            ReadDataBaseRs(info, data);
            ReportEventJankFrame(info);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_GAMESTATE): {
            GameStateData info;
            ReadGameStateDataRs(info, data);
            ReportGameStateData(info);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::EXECUTE_SYNCHRONOUS_TASK): {
            auto type = data.ReadInt16();
            auto subType = data.ReadInt16();
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
            ExecuteSynchronousTask(task);
            if (!task->Marshalling(reply)) {
                ret = ERR_INVALID_STATE;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_HARDWARE_ENABLED) : {
            auto id = data.ReadUint64();
            auto isEnabled = data.ReadBool();
            auto selfDrawingType = static_cast<SelfDrawingNodeType>(data.ReadUint8());
            SetHardwareEnabled(id, isEnabled, selfDrawingType);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_LIGHT_FACTOR_STATUS) : {
            auto isSafe = data.ReadBool();
            NotifyLightFactorStatus(isSafe);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_PACKAGE_EVENT) : {
            auto listSize = data.ReadUint32();
            const uint32_t MAX_LIST_SIZE = 50;
            if (listSize > MAX_LIST_SIZE) {
                ret = ERR_INVALID_STATE;
                break;
            }
            std::vector<std::string> packageList;
            for (uint32_t i = 0; i < listSize; i++) {
                packageList.push_back(data.ReadString());
            }
            NotifyPackageEvent(listSize, packageList);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_REFRESH_RATE_EVENT) : {
            EventInfo eventInfo = {
                data.ReadString(), data.ReadBool(), data.ReadUint32(), data.ReadUint32(), data.ReadString(),
            };
            NotifyRefreshRateEvent(eventInfo);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_DYNAMIC_MODE_EVENT) : {
            auto enableDynamicMode = data.ReadBool();
            NotifyDynamicModeEvent(enableDynamicMode);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_TOUCH_EVENT) : {
            auto touchStatus = data.ReadInt32();
            auto touchCnt = data.ReadInt32();
            NotifyTouchEvent(touchStatus, touchCnt);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_HGM_CFG_CALLBACK) : {
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            sptr<RSIHgmConfigChangeCallback> callback = iface_cast<RSIHgmConfigChangeCallback>(remoteObject);
            if (callback == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            int32_t status = RegisterHgmConfigChangeCallback(callback);
            reply.WriteInt32(status);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REFRESH_RATE_MODE_CHANGE_CALLBACK) : {
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
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
            reply.WriteInt32(status);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REFRESH_RATE_UPDATE_CALLBACK) : {
            sptr<RSIHgmConfigChangeCallback> callback = nullptr;
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject != nullptr) {
                callback = iface_cast<RSIHgmConfigChangeCallback>(remoteObject);
            }
            int32_t status = RegisterHgmRefreshRateUpdateCallback(callback);
            reply.WriteInt32(status);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_ROTATION_CACHE_ENABLED) : {
            auto isEnabled = data.ReadBool();
            SetCacheEnabledForRotation(isEnabled);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ACTIVE_DIRTY_REGION_INFO) : {
            uint64_t tokenId = OHOS::IPCSkeleton::GetCallingFullTokenID();
            if (Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(tokenId)) {
                ret = ERR_TRANSACTION_FAILED;
                break;
            }
            const auto& activeDirtyRegionInfos = GetActiveDirtyRegionInfo();
            reply.WriteInt32(activeDirtyRegionInfos.size());
            for (const auto& activeDirtyRegionInfo : activeDirtyRegionInfos) {
                reply.WriteInt64(activeDirtyRegionInfo.activeDirtyRegionArea);
                reply.WriteInt32(activeDirtyRegionInfo.activeFramesNumber);
                reply.WriteInt32(activeDirtyRegionInfo.pidOfBelongsApp);
                reply.WriteString(activeDirtyRegionInfo.windowName);
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_GLOBAL_DIRTY_REGION_INFO) : {
            const auto& globalDirtyRegionInfo = GetGlobalDirtyRegionInfo();
            reply.WriteInt64(globalDirtyRegionInfo.globalDirtyRegionAreas);
            reply.WriteInt32(globalDirtyRegionInfo.globalFramesNumber);
            reply.WriteInt32(globalDirtyRegionInfo.skipProcessFramesNumber);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_LAYER_COMPOSE_INFO) : {
            const auto& LayerComposeInfo = GetLayerComposeInfo();
            reply.WriteInt32(LayerComposeInfo.uniformRenderFrameNumber);
            reply.WriteInt32(LayerComposeInfo.offlineComposeFrameNumber);
            reply.WriteInt32(LayerComposeInfo.redrawFrameNumber);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::
            GET_HARDWARE_COMPOSE_DISABLED_REASON_INFO) : {
            uint64_t tokenId = OHOS::IPCSkeleton::GetCallingFullTokenID();
            if (Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(tokenId)) {
                ret = ERR_TRANSACTION_FAILED;
                break;
            }
            const auto& hwcDisabledReasonInfos = GetHwcDisabledReasonInfo();
            reply.WriteInt32(hwcDisabledReasonInfos.size());
            for (const auto& hwcDisabledReasonInfo : hwcDisabledReasonInfos) {
                for (const auto& disabledReasonCount : hwcDisabledReasonInfo.disabledReasonStatistics) {
                    reply.WriteInt32(disabledReasonCount);
                }
                reply.WriteInt32(hwcDisabledReasonInfo.pidOfBelongsApp);
                reply.WriteString(hwcDisabledReasonInfo.nodeName);
            }
            break;
        }
#ifdef TP_FEATURE_ENABLE
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_TP_FEATURE_CONFIG) : {
            int32_t feature = data.ReadInt32();
            auto config = data.ReadCString();
            SetTpFeatureConfig(feature, config);
            break;
        }
#endif
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_USING_STATUS) : {
            auto isVirtualScreenUsingStatus = data.ReadBool();
            SetVirtualScreenUsingStatus(isVirtualScreenUsingStatus);
            break;
        }

        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_CURTAIN_SCREEN_USING_STATUS) : {
            auto isCurtainScreenOn = data.ReadBool();
            SetCurtainScreenUsingStatus(isCurtainScreenOn);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_UIEXTENSION_CALLBACK): {
            if (!securityManager_.IsInterfaceCodeAccessible(code)) {
                RS_LOGE("RSRenderServiceConnectionStub::OnRemoteRequest no permission REGISTER_UIEXTENSION_CALLBACK");
                return ERR_INVALID_STATE;
            }
            uint64_t userId = data.ReadUint64();
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            sptr<RSIUIExtensionCallback> callback = iface_cast<RSIUIExtensionCallback>(remoteObject);
            if (callback == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            int32_t status = RegisterUIExtensionCallback(userId, callback);
            reply.WriteInt32(status);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_STATUS) : {
            if (!securityManager_.IsInterfaceCodeAccessible(code)) {
                RS_LOGE("RSRenderServiceConnectionStub::OnRemoteRequest no permission SET_VIRTUAL_SCREEN_STATUS");
                return ERR_INVALID_STATE;
            }
            ScreenId id = data.ReadUint64();
            VirtualScreenStatus screenStatus = static_cast<VirtualScreenStatus>(data.ReadUint8());
            bool result = SetVirtualScreenStatus(id, screenStatus);
            reply.WriteBool(result);
            break;
        }
        default: {
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }

    return ret;
}

void RSRenderServiceConnectionStub::ReadDataBaseRs(DataBaseRs& info, MessageParcel& data)
{
    info.appPid = data.ReadInt32();
    info.eventType =  data.ReadInt32();
    info.versionCode = data.ReadInt32();
    info.uniqueId = data.ReadInt64();
    info.inputTime = data.ReadInt64();
    info.beginVsyncTime = data.ReadInt64();
    info.endVsyncTime = data.ReadInt64();
    info.isDisplayAnimator = data.ReadBool();
    info.sceneId = data.ReadString();
    info.versionName = data.ReadString();
    info.bundleName = data.ReadString();
    info.processName = data.ReadString();
    info.abilityName = data.ReadString();
    info.pageUrl = data.ReadString();
    info.sourceType = data.ReadString();
    info.note = data.ReadString();
}

void RSRenderServiceConnectionStub::ReadGameStateDataRs(GameStateData& info, MessageParcel& data)
{
    info.pid = data.ReadInt32();
    info.uid =  data.ReadInt32();
    info.state = data.ReadInt32();
    info.renderTid = data.ReadInt32();
    info.bundleName = data.ReadString();
}

const RSInterfaceCodeSecurityManager RSRenderServiceConnectionStub::securityManager_ = \
    RSInterfaceCodeSecurityManager::CreateInstance<RSIRenderServiceConnectionInterfaceCodeAccessVerifier>();
} // namespace Rosen
} // namespace OHOS
