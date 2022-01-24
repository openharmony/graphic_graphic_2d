/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "command/rs_command_factory.h"

namespace OHOS {
namespace Rosen {
int RSRenderServiceConnectionStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    int ret = ERR_NONE;
    switch (code) {
        case COMMIT_TRANSACTION: {
            auto token = data.ReadInterfaceToken();
            auto transactionData = data.ReadParcelable<RSTransactionData>();
            std::unique_ptr<RSTransactionData> transData(transactionData);
            CommitTransaction(transData);
            break;
        }
        case CREATE_NODE_AND_SURFACE: {
            auto nodeId = data.ReadUint64();
            RSSurfaceRenderNodeConfig config = {.id = nodeId};
            sptr<Surface> surface = CreateNodeAndSurface(config);
            auto producer = surface->GetProducer();
            reply.WriteRemoteObject(producer->AsObject());
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
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            auto bufferProducer = iface_cast<IBufferProducer>(remoteObject);
            sptr<Surface> surface = Surface::CreateSurfaceAsProducer(bufferProducer);
            ScreenId mirrorId = data.ReadUint64();
            int32_t flags = data.ReadInt32();

            ScreenId id = CreateVirtualScreen(name, width, height, surface, mirrorId, flags);
            reply.WriteUint64(id);
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
            SetScreenChangeCallback(cb);
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
            TakeSurfaceCapture(id, cb);
            break;
        }
        case REGISTER_APPLICATION_RENDER_THREAD: {
            uint32_t pid = data.ReadUint32();
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            sptr<IApplicationRenderThread> app = iface_cast<IApplicationRenderThread>(remoteObject);
            RegisterApplicationRenderThread(pid, app);
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
        default: {
            ret = ERR_UNKNOWN_TRANSACTION;
            break;
        }
    }

    return ret;
}
} // namespace Rosen
} // namespace OHOS
