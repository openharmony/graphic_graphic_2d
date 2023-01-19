/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "ivsync_connection.h"
#include "securec.h"
#include "sys_binder.h"

#include "command/rs_command_factory.h"
#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "pipeline/rs_unmarshal_thread.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_ashmem_helper.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t MAX_DATA_SIZE_FOR_UNMARSHALLING_IN_PLACE = 1024 * 30; // 30kB

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

        if (flat->hdr.type == BINDER_TYPE_FD && flat->handle > 0) {
            int32_t val = dup(flat->handle);
            if (val < 0) {
                ROSEN_LOGW("CopyFileDescriptor dup failed, fd:%d", val);
            }
            copiedFlat->handle = static_cast<uint32_t>(val);
        }
    }
}

std::shared_ptr<MessageParcel> CopyParcelIfNeed(MessageParcel& old)
{
    auto dataSize = old.GetDataSize();
    if (dataSize <= MAX_DATA_SIZE_FOR_UNMARSHALLING_IN_PLACE) {
        return nullptr;
    }
    RS_TRACE_NAME("CopyParcelForUnmarsh: size:" + std::to_string(dataSize));
    void* base = malloc(dataSize);
    if (memcpy_s(base, dataSize, reinterpret_cast<void*>(old.GetData()), dataSize) != 0) {
        RS_LOGE("RSRenderServiceConnectionStub::CopyParcelIfNeed copy parcel data failed");
        free(base);
        return nullptr;
    }

    auto parcelCopied = std::make_shared<MessageParcel>();
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
    int ret = ERR_NONE;
    switch (code) {
        case COMMIT_TRANSACTION: {
            RS_ASYNC_TRACE_END("RSProxySendRequest", data.GetDataSize());
            static bool isUniRender = RSUniRenderJudgement::IsUniRender();
            std::shared_ptr<MessageParcel> parsedParcel;
            if (data.ReadInt32() == 0) { // indicate normal parcel
                if (isUniRender) {
                    // in uni render mode, if parcel size over threshold,
                    // Unmarshalling task will be post to RSUnmarshalThread,
                    // copy the origin parcel to maintain the parcel lifetime
                    parsedParcel = CopyParcelIfNeed(data);
                }
                if (parsedParcel == nullptr) {
                    // no need to copy or copy failed, use original parcel
                    // execute Unmarshalling immediately
                    RSMainThread::Instance()->RequestNextVSync();
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
                RS_LOGE("RSRenderServiceConnectionStub::COMMIT_TRANSACTION failed");
                return ERR_INVALID_DATA;
            }
            if (RSMainThread::Instance()->QueryIfUseUniVisitor()) {
                // post Unmarshalling task to RSUnmarshalThread
                RSUnmarshalThread::Instance().RecvParcel(parsedParcel);
            } else {
                // execute Unmarshalling immediately
                auto transactionData = RSBaseRenderUtil::ParseTransactionData(*parsedParcel);
                CommitTransaction(transactionData);
            }
            break;
        }
        case SET_RENDER_MODE_CHANGE_CALLBACK: {
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
            sptr<RSIRenderModeChangeCallback> cb = iface_cast<RSIRenderModeChangeCallback>(remoteObject);
            int32_t status = SetRenderModeChangeCallback(cb);
            reply.WriteInt32(status);
            break;
        }
        case UPDATE_RENDER_MODE: {
            bool isUniRender = data.ReadBool();
            UpdateRenderMode(isUniRender);
            break;
        }
        case GET_UNI_RENDER_ENABLED: {
            reply.WriteBool(GetUniRenderEnabled());
            break;
        }
        case QUERY_RT_NEED_RENDER: {
            reply.WriteBool(QueryIfRTNeedRender());
            break;
        }
        case CREATE_NODE: {
            auto nodeId = data.ReadUint64();
            auto surfaceName = data.ReadString();
            RSSurfaceRenderNodeConfig config = {.id = nodeId, .name = surfaceName};
            reply.WriteBool(CreateNode(config));
            break;
        }
        case CREATE_NODE_AND_SURFACE: {
            auto nodeId = data.ReadUint64();
            auto surfaceName = data.ReadString();
            auto type = static_cast<RSSurfaceNodeType>(data.ReadUint8());
            RSSurfaceRenderNodeConfig config = {.id = nodeId, .name = surfaceName, .nodeType = type };
            sptr<Surface> surface = CreateNodeAndSurface(config);
            auto producer = surface->GetProducer();
            reply.WriteRemoteObject(producer->AsObject());
            break;
        }
        case SET_FOCUS_APP_INFO: {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }

            int32_t pid = data.ReadInt32();
            int32_t uid = data.ReadInt32();
            std::string bundleName = data.ReadString();
            std::string abilityName = data.ReadString();
            int32_t status = SetFocusAppInfo(pid, uid, bundleName, abilityName);
            reply.WriteInt32(status);
            break;
        }
        case GET_DEFAULT_SCREEN_ID: {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = GetDefaultScreenId();
            reply.WriteUint64(id);
            break;
        }
        case GET_ALL_SCREEN_IDS: {
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
        case CREATE_VIRTUAL_SCREEN: {
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

            ScreenId id = CreateVirtualScreen(name, width, height, surface, mirrorId, flags);
            reply.WriteUint64(id);
            break;
        }
        case SET_VIRTUAL_SCREEN_SURFACE: {
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

            int32_t status = SetVirtualScreenSurface(id, surface);
            reply.WriteInt32(status);
            break;
        }
        case REMOVE_VIRTUAL_SCREEN: {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            RemoveVirtualScreen(id);
            break;
        }
        case SET_SCREEN_CHANGE_CALLBACK: {
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
            int32_t status = SetScreenChangeCallback(cb);
            reply.WriteInt32(status);
            break;
        }
        case SET_SCREEN_ACTIVE_MODE: {
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
        case SET_VIRTUAL_SCREEN_RESOLUTION: {
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
        case SET_SCREEN_POWER_STATUS: {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            uint32_t status = data.ReadUint32();
            SetScreenPowerStatus(id, static_cast<ScreenPowerStatus>(status));
            break;
        }
        case TAKE_SURFACE_CAPTURE: {
            NodeId id = data.ReadUint64();
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            sptr<RSISurfaceCaptureCallback> cb = iface_cast<RSISurfaceCaptureCallback>(remoteObject);
            float scaleX = data.ReadFloat();
            float scaleY = data.ReadFloat();
            TakeSurfaceCapture(id, cb, scaleX, scaleY);
            break;
        }
        case REGISTER_APPLICATION_AGENT: {
            uint32_t pid = data.ReadUint32();
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            sptr<IApplicationAgent> app = iface_cast<IApplicationAgent>(remoteObject);
            RegisterApplicationAgent(pid, app);
            break;
        }
        case GET_VIRTUAL_SCREEN_RESOLUTION: {
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
        case GET_SCREEN_ACTIVE_MODE: {
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
        case GET_SCREEN_SUPPORTED_MODES: {
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
        case GET_SCREEN_CAPABILITY: {
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
        case GET_SCREEN_POWER_STATUS: {
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
        case GET_SCREEN_DATA: {
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
        case EXECUTE_SYNCHRONOUS_TASK: {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            auto type = data.ReadUint16();
            auto subType = data.ReadUint16();
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
        case GET_SCREEN_BACK_LIGHT: {
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
        case SET_SCREEN_BACK_LIGHT: {
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
        case SET_BUFFER_AVAILABLE_LISTENER: {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            NodeId id = data.ReadUint64();
            auto remoteObject = data.ReadRemoteObject();
            bool isFromRenderThread = data.ReadBool();
            if (remoteObject == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            sptr<RSIBufferAvailableCallback> cb = iface_cast<RSIBufferAvailableCallback>(remoteObject);
            RegisterBufferAvailableListener(id, cb, isFromRenderThread);
            break;
        }
        case GET_SCREEN_SUPPORTED_GAMUTS: {
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
        case GET_SCREEN_SUPPORTED_METADATAKEYS: {
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
        case GET_SCREEN_GAMUT: {
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
        case SET_SCREEN_GAMUT: {
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
        case SET_SCREEN_GAMUT_MAP: {
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
        case GET_SCREEN_GAMUT_MAP: {
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
        case CREATE_VSYNC_CONNECTION: {
            std::string name = data.ReadString();
            sptr<IVSyncConnection> conn = CreateVSyncConnection(name);
            reply.WriteRemoteObject(conn->AsObject());
            break;
        }
        case GET_SCREEN_HDR_CAPABILITY: {
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
        case GET_SCREEN_TYPE: {
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
        case SET_SCREEN_SKIP_FRAME_INTERVAL: {
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
        case REGISTER_OCCLUSION_CHANGE_CALLBACK: {
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
            int32_t status = RegisterOcclusionChangeCallback(callback);
            reply.WriteInt32(status);
            break;
        }
        case UNREGISTER_OCCLUSION_CHANGE_CALLBACK: {
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
            int32_t status = UnRegisterOcclusionChangeCallback(callback);
            reply.WriteInt32(status);
            break;
        }
        case SET_APP_WINDOW_NUM: {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            uint32_t num = data.ReadUint32();
            SetAppWindowNum(num);
        }
        default: {
            ret = ERR_UNKNOWN_TRANSACTION;
            break;
        }
    }

    return ret;
}
} // namespace Rosen
} // namespace OHOS
