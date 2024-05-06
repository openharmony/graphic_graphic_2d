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
    pid_t callingPid = GetCallingPid();
#ifdef ENABLE_IPC_SECURITY_ACCESS_COUNTER
    auto accessCount = securityUtils_.GetCodeAccessCounter(code);
    if (!securityManager_.IsAccessTimesRestricted(code, accessCount)) {
        RS_LOGE("RSRenderServiceConnectionStub::OnRemoteRequest This Function[ID=%{public}u] invoke times:%{public}d"
            "by pid[%{public}d]", code, accessCount, callingPid);
        return ERR_INVALID_STATE;
    }
#endif
    int ret = ERR_NONE;
    switch (code) {
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::COMMIT_TRANSACTION): {
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
                RSUnmarshalThread::Instance().RecvParcel(parsedParcel);
            } else {
                // execute Unmarshalling immediately
                auto transactionData = RSBaseRenderUtil::ParseTransactionData(*parsedParcel);
                CommitTransaction(transactionData);
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_UNI_RENDER_ENABLED): {
            reply.WriteBool(GetUniRenderEnabled());
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_NODE): {
            auto nodeId = RS_PROFILER_READ_NODE_ID(data);
            auto surfaceName = data.ReadString();
            auto bundleName = data.ReadString();
            RSSurfaceRenderNodeConfig config = {.id = nodeId, .name = surfaceName, .bundleName = bundleName};
            reply.WriteBool(CreateNode(config));
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_NODE_AND_SURFACE): {
            auto nodeId = RS_PROFILER_READ_NODE_ID(data);
            auto surfaceName = data.ReadString();
            auto type = static_cast<RSSurfaceNodeType>(data.ReadUint8());
            auto bundleName = data.ReadString();
            bool isTextureExportNode = data.ReadBool();
            bool isSync = data.ReadBool();
            RSSurfaceRenderNodeConfig config = {
                .id = nodeId, .name = surfaceName, .bundleName = bundleName, .nodeType = type,
                .isTextureExportNode = isTextureExportNode, .isSync = isSync};
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
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }

            int32_t pid = RS_PROFILER_READ_PID(data);
            int32_t uid = data.ReadInt32();
            std::string bundleName = data.ReadString();
            std::string abilityName = data.ReadString();
            uint64_t focusNodeId = RS_PROFILER_READ_NODE_ID(data);
            int32_t status = SetFocusAppInfo(pid, uid, bundleName, abilityName, focusNodeId);
            reply.WriteInt32(status);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_DEFAULT_SCREEN_ID): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = GetDefaultScreenId();
            reply.WriteUint64(id);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ACTIVE_SCREEN_ID): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = GetActiveScreenId();
            reply.WriteUint64(id);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ALL_SCREEN_IDS): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            std::vector<ScreenId> ids = GetAllScreenIds();
            reply.WriteUint32(ids.size());
            for (uint32_t i = 0; i < ids.size(); i++) {
                reply.WriteUint64(ids[i]);
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }

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
            std::vector<NodeId> filteredAppVector;
            data.ReadUInt64Vector(&filteredAppVector);
            ScreenId id = CreateVirtualScreen(name, width, height, surface, mirrorId, flags, filteredAppVector);
            reply.WriteUint64(id);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_SURFACE): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }

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
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            RemoveVirtualScreen(id);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_CHANGE_CALLBACK): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }

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
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_ACTIVE_MODE): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            uint32_t modeId = data.ReadUint32();
            SetScreenActiveMode(id, modeId);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_REFRESH_RATE): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            int32_t sceneId = data.ReadInt32();
            int32_t rate = data.ReadInt32();
            SetScreenRefreshRate(id, sceneId, rate);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_REFRESH_RATE_MODE): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            if (!securityManager_.IsInterfaceCodeAccessible(code)) {
                RS_LOGE("RSRenderServiceConnectionStub::OnRemoteRequest no permission to access SET_REFRESH_RATE_MODE");
                return ERR_INVALID_STATE;
            }
            int32_t mode = data.ReadInt32();
            SetRefreshRateMode(mode);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SYNC_FRAME_RATE_RANGE): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            FrameRateLinkerId id = data.ReadUint64();
            uint32_t min = data.ReadUint32();
            uint32_t max = data.ReadUint32();
            uint32_t preferred = data.ReadUint32();
            SyncFrameRateRange(id, {min, max, preferred});
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_CURRENT_REFRESH_RATE): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            uint32_t refreshRate = GetScreenCurrentRefreshRate(id);
            reply.WriteUint32(refreshRate);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_CURRENT_REFRESH_RATE_MODE): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            int32_t refreshRateMode = GetCurrentRefreshRateMode();
            reply.WriteInt32(refreshRateMode);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_REFRESH_RATES): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            std::vector<int32_t> rates = GetScreenSupportedRefreshRates(id);
            reply.WriteUint64(static_cast<uint64_t>(rates.size()));
            for (auto ratesIter : rates) {
                reply.WriteInt32(ratesIter);
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SHOW_REFRESH_RATE_ENABLED): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
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
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            if (!securityManager_.IsInterfaceCodeAccessible(code)) {
                RS_LOGE("RSRenderServiceConnectionStub::OnRemoteRequest no permission to access"\
                    "SET_SHOW_REFRESH_RATE_ENABLED");
                return ERR_INVALID_STATE;
            }
            bool enable = data.ReadBool();
            SetShowRefreshRateEnabled(enable);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_RESOLUTION): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            uint32_t width = data.ReadUint32();
            uint32_t height = data.ReadUint32();
            int32_t status = SetVirtualScreenResolution(id, width, height);
            reply.WriteInt32(status);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_POWER_STATUS): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
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
            NodeId id = RS_PROFILER_READ_NODE_ID(data);
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
            float scaleX = data.ReadFloat();
            float scaleY = data.ReadFloat();
            SurfaceCaptureType surfaceCaptureType = static_cast<SurfaceCaptureType>(data.ReadUint8());
            bool isSync = data.ReadBool();
            if (RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode(id)->GetType() ==
                RSRenderNodeType::DISPLAY_NODE && !securityManager_.IsInterfaceCodeAccessible(code)) {
                RS_LOGE("RSRenderServiceConnectionStub::OnRemoteRequest no permission to access TAKE_SURFACE_CAPTURE");
                return ERR_INVALID_STATE;
            }
            TakeSurfaceCapture(id, cb, scaleX, scaleY, surfaceCaptureType, isSync);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_APPLICATION_AGENT): {
            uint32_t pid = static_cast<uint32_t>(RS_PROFILER_READ_PID(data));
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
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            RSVirtualScreenResolution virtualScreenResolution = GetVirtualScreenResolution(id);
            reply.WriteParcelable(&virtualScreenResolution);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_ACTIVE_MODE): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            RSScreenModeInfo screenModeInfo = GetScreenActiveMode(id);
            reply.WriteParcelable(&screenModeInfo);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_MODES): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            std::vector<RSScreenModeInfo> screenSupportedModes = GetScreenSupportedModes(id);
            reply.WriteUint64(static_cast<uint64_t>(screenSupportedModes.size()));
            for (uint32_t modeIndex = 0; modeIndex < screenSupportedModes.size(); modeIndex++) {
                reply.WriteParcelable(&screenSupportedModes[modeIndex]);
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_MEMORY_GRAPHIC): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            auto pid = RS_PROFILER_READ_PID(data);
            MemoryGraphic memoryGraphic = GetMemoryGraphic(pid);
            reply.WriteParcelable(&memoryGraphic);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_MEMORY_GRAPHICS): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
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
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            float cpuMemSize = 0.f;
            float gpuMemSize = 0.f;
            GetTotalAppMemSize(cpuMemSize, gpuMemSize);
            reply.WriteFloat(cpuMemSize);
            reply.WriteFloat(gpuMemSize);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_CAPABILITY): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            RSScreenCapability screenCapability = GetScreenCapability(id);
            reply.WriteParcelable(&screenCapability);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_POWER_STATUS): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            ScreenPowerStatus status = GetScreenPowerStatus(id);
            reply.WriteUint32(static_cast<uint32_t>(status));
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_DATA): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            RSScreenData screenData = GetScreenData(id);
            reply.WriteParcelable(&screenData);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_BACK_LIGHT): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            int32_t level = GetScreenBacklight(id);
            reply.WriteInt32(level);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_BACK_LIGHT): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            uint32_t level = data.ReadUint32();
            SetScreenBacklight(id, level);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_BUFFER_AVAILABLE_LISTENER): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            NodeId id = RS_PROFILER_READ_NODE_ID(data);
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
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            NodeId id = RS_PROFILER_READ_NODE_ID(data);
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
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
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
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
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
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
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
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            int32_t modeIdx = data.ReadInt32();
            int32_t result = SetScreenColorGamut(id, modeIdx);
            reply.WriteInt32(result);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_GAMUT_MAP): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            ScreenGamutMap mode = static_cast<ScreenGamutMap>(data.ReadInt32());
            int32_t result = SetScreenGamutMap(id, mode);
            reply.WriteInt32(result);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_CORRECTION): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            ScreenRotation screenRotation = static_cast<ScreenRotation>(data.ReadInt32());
            int32_t result = SetScreenCorrection(id, screenRotation);
            reply.WriteInt32(result);
            break;
        }
        case static_cast<uint32_t>(
            RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            bool canvasRotation = data.ReadBool();
            bool result = SetVirtualMirrorScreenCanvasRotation(id, canvasRotation);
            reply.WriteBool(result);
            break;
        }
        case static_cast<uint32_t>(
            RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            ScreenScaleMode scaleMode = static_cast<ScreenScaleMode>(data.ReadUint32());
            bool result = SetVirtualMirrorScreenScaleMode(id, scaleMode);
            reply.WriteBool(result);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_GAMUT_MAP): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
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
            if (data.ReadInterfaceToken() != RSIRenderServiceConnection::GetDescriptor()) {
                reply.WriteInt32(0);
                ret = ERR_INVALID_STATE;
                break;
            }
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
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
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
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
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
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            GraphicPixelFormat pixelFormat = static_cast<GraphicPixelFormat>(data.ReadInt32());
            int32_t result = SetPixelFormat(id, pixelFormat);
            reply.WriteInt32(result);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_HDR_FORMATS): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
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
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
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
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            int32_t modeIdx = data.ReadInt32();
            int32_t result = SetScreenHDRFormat(id, modeIdx);
            reply.WriteInt32(result);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_COLORSPACES): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
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
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
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
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            GraphicCM_ColorSpaceType colorSpace = static_cast<GraphicCM_ColorSpaceType>(data.ReadInt32());
            int32_t result = SetScreenColorSpace(id, colorSpace);
            reply.WriteInt32(result);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_TYPE): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
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
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            NodeId id = RS_PROFILER_READ_NODE_ID(data);
            Drawing::Bitmap bm;
            bool result = GetBitmap(id, bm);
            reply.WriteBool(result);
            if (result) {
                RSMarshallingHelper::Marshalling(reply, bm);
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PIXELMAP): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            NodeId id = RS_PROFILER_READ_NODE_ID(data);
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
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
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
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            uint64_t uniqueId = data.ReadUint64();
            UnRegisterTypeface(uniqueId);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_SKIP_FRAME_INTERVAL): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            uint32_t skipFrameInterval = data.ReadUint32();
            int32_t result = SetScreenSkipFrameInterval(id, skipFrameInterval);
            reply.WriteInt32(result);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_OCCLUSION_CHANGE_CALLBACK): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }

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
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }

            NodeId id = RS_PROFILER_READ_NODE_ID(data);
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
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }

            NodeId id = RS_PROFILER_READ_NODE_ID(data);
            int32_t status = UnRegisterSurfaceOcclusionChangeCallback(id);
            reply.WriteInt32(status);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_APP_WINDOW_NUM): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            uint32_t num = data.ReadUint32();
            SetAppWindowNum(num);
            break;
        }
        case static_cast<uint32_t>(
            RSIRenderServiceConnectionInterfaceCode::SET_SYSTEM_ANIMATED_SCENES): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            uint32_t systemAnimatedScenes = data.ReadUint32();
            bool result = SetSystemAnimatedScenes(static_cast<SystemAnimatedScenes>(systemAnimatedScenes));
            reply.WriteBool(result);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SHOW_WATERMARK): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            std::shared_ptr<Media::PixelMap> watermarkImg =
                std::shared_ptr<Media::PixelMap>(data.ReadParcelable<Media::PixelMap>());
            bool isShow = data.ReadBool();
            ShowWatermark(watermarkImg, isShow);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::RESIZE_VIRTUAL_SCREEN): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            uint32_t width = data.ReadUint32();
            uint32_t height = data.ReadUint32();
            int32_t status = ResizeVirtualScreen(id, width, height);
            reply.WriteInt32(status);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_JANK_STATS): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ReportJankStats();
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_RESPONSE): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            DataBaseRs info;
            ReadDataBaseRs(info, data);
            ReportEventResponse(info);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_COMPLETE): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            DataBaseRs info;
            ReadDataBaseRs(info, data);
            ReportEventComplete(info);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_JANK_FRAME): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            DataBaseRs info;
            ReadDataBaseRs(info, data);
            ReportEventJankFrame(info);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_GAMESTATE): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            GameStateData info;
            ReadGameStateDataRs(info, data);
            ReportGameStateData(info);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::EXECUTE_SYNCHRONOUS_TASK): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            auto type = data.ReadInt16();
            auto subType = data.ReadInt16();
            if (type != RS_NODE_SYNCHRONOUS_READ_PROPERTY) {
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
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            auto id = data.ReadUint64();
            auto isEnabled = data.ReadBool();
            auto selfDrawingType = static_cast<SelfDrawingNodeType>(data.ReadUint8());
            SetHardwareEnabled(id, isEnabled, selfDrawingType);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_LIGHT_FACTOR_STATUS) : {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            auto isSafe = data.ReadBool();
            NotifyLightFactorStatus(isSafe);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_PACKAGE_EVENT) : {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            auto listSize = data.ReadUint32();
            const uint32_t MAX_LIST_SIZE = 30;
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
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            EventInfo eventInfo = {
                data.ReadString(), data.ReadBool(), data.ReadUint32(), data.ReadUint32(), data.ReadString(),
            };
            NotifyRefreshRateEvent(eventInfo);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_TOUCH_EVENT) : {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            auto touchStatus = data.ReadInt32();
            NotifyTouchEvent(touchStatus);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_HGM_CFG_CALLBACK) : {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
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
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
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
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject != nullptr) {
                callback = iface_cast<RSIHgmConfigChangeCallback>(remoteObject);
            }
            int32_t status = RegisterHgmRefreshRateUpdateCallback(callback);
            reply.WriteInt32(status);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_ROTATION_CACHE_ENABLED) : {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            auto isEnabled = data.ReadBool();
            SetCacheEnabledForRotation(isEnabled);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_CURRENT_DIRTY_REGION_INFO) : {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            GpuDirtyRegionInfo gpuDirtyRegionInfo = GetCurrentDirtyRegionInfo(id);
            reply.WriteInt64(gpuDirtyRegionInfo.activeGpuDirtyRegionAreas);
            reply.WriteInt64(gpuDirtyRegionInfo.globalGpuDirtyRegionAreas);
            reply.WriteInt32(gpuDirtyRegionInfo.skipProcessFramesNumber);
            reply.WriteInt32(gpuDirtyRegionInfo.activeFramesNumber);
            reply.WriteInt32(gpuDirtyRegionInfo.globalFramesNumber);
            reply.WriteString("");
            break;
        }
#ifdef TP_FEATURE_ENABLE
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_TP_FEATURE_CONFIG) : {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            int32_t feature = data.ReadInt32();
            auto config = data.ReadCString();
            SetTpFeatureConfig(feature, config);
            break;
        }
#endif
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_USING_STATUS) : {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            auto isVirtualScreenUsingStatus = data.ReadBool();
            SetVirtualScreenUsingStatus(isVirtualScreenUsingStatus);
            break;
        }

        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_CURTAIN_SCREEN_USING_STATUS) : {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            auto isCurtainScreenOn = data.ReadBool();
            SetCurtainScreenUsingStatus(isCurtainScreenOn);
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
