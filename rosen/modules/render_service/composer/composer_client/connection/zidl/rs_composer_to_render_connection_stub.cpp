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

#include "rs_composer_to_render_connection_stub.h"
#include <unistd.h>
#include "accesstoken_kit.h"
#include "buffer_utils.h"
#include "graphic_common.h"
#include "ipc_skeleton.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"

#undef LOG_TAG
#define LOG_TAG "RSComposerToRenderConnectionStub"

namespace OHOS {
namespace Rosen {
int32_t RSComposerToRenderConnectionStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    int ret = COMPOSITOR_ERROR_OK;
    switch (code) {
        case ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS: {
            auto interfaceToken = data.ReadInterfaceToken();
            if (interfaceToken != IRSComposerToRenderConnection::GetDescriptor()) {
                RS_LOGE("RSComposerToRenderConnectionStub::CREATE_CONNECTION Read interfaceToken failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            ReleaseLayerBuffersInfo releaseLayerInfo;
            releaseLayerInfo.screenId = data.ReadUint64();
            auto vecSize = data.ReadUint32();
            for (uint32_t i = 0; i < vecSize; i++) {
                auto layerId = data.ReadUint64();
                bool isSupportedPresentTimestamp = data.ReadBool();
                GraphicPresentTimestamp timestamp;
                timestamp.type = static_cast<GraphicPresentTimestampType>(data.ReadUint32());
                timestamp.time = data.ReadInt64();
                releaseLayerInfo.timestampVec.push_back(std::tuple(layerId, isSupportedPresentTimestamp, timestamp));
            }
            vecSize = data.ReadUint32();
            for (uint32_t i = 0; i < vecSize; i++) {
                auto layerId = data.ReadUint64();
                uint32_t sequence;
                sptr<SurfaceBuffer> buffer = nullptr;
                bool hasBuffer = data.ReadBool();
                if (hasBuffer) {
                    auto readSafeFdFunc = [](OHOS::MessageParcel& parcel, std::function<int(OHOS::MessageParcel&)> readFdDefaultFunc) -> int {
                        return parcel.ReadFileDescriptor();
                    };
                    ret = ReadSurfaceBufferImpl(data, sequence, buffer, readSafeFdFunc);
                    if (ret != GSERROR_OK) {
                        ret = ERR_INVALID_DATA;
                        break;
                    }
                }
                sptr<SyncFence> fence = SyncFence::ReadFromMessageParcel(data);
                releaseLayerInfo.releaseBufferFenceVec.push_back(std::tuple(layerId, buffer, fence));
            }
            data.ReadInt64(releaseLayerInfo.lastSwapBufferTime);
            auto replyMessage = ReleaseLayerBuffers(releaseLayerInfo);
            reply.WriteInt32(replyMessage);
            break;
        }
        case NOTIFY_LPP_LAYER_TO_RENDER: {
            auto interfaceToken = data.ReadInterfaceToken();
            if (interfaceToken != IRSComposerToRenderConnection::GetDescriptor()) {
                RS_LOGE("RSComposerToRenderConnectionStub::CREATE_CONNECTION Read interfaceToken failed!");
                ret = ERR_INVALID_DATA;
                break;
            }
            uint64_t vsyncId = data.ReadUint64();
            int32_t size = data.ReadInt32();
            std::set<uint64_t> lppNodeIds;
            for (int32_t i = 0; i < size; i++) {
                uint64_t nodeId = data.ReadUint64();
                lppNodeIds.insert(nodeId);
            }
            auto replyMessage = NotifyLppLayerToRender(vsyncId, lppNodeIds);
            reply.WriteInt32(replyMessage);
            break;
        }

        default:
            ret = COMPOSITOR_ERROR_INVALID_ARGUMENTS;
            break;
    }
    return ret;
}

} // namespace Rosen
} // namespace OHOS