/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
namespace {
constexpr size_t RELEASE_LAYER_MAX_SIZE = 1000; // upper bound of parcel capacity
constexpr int32_t MAX_LPP_LAYER_SIZE = 5;
}
int32_t RSComposerToRenderConnectionStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    auto interfaceToken = data.ReadInterfaceToken();
    if (interfaceToken != IRSComposerToRenderConnection::GetDescriptor()) {
        RS_LOGE("Read interfaceToken failed!");
        return ERR_INVALID_DATA;
    }
    int ret = COMPOSITOR_ERROR_OK;
    switch (code) {
        case ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS: {
            ret = ReleaseLayerBuffersStub(data, reply, option);
            break;
        }
        case NOTIFY_LPP_LAYER_TO_RENDER: {
            ret = NotifyLppLayerToRenderStub(data, reply, option);
            break;
        }

        default:
            ret = COMPOSITOR_ERROR_INVALID_ARGUMENTS;
            break;
    }
    return ret;
}

int32_t RSComposerToRenderConnectionStub::ReleaseLayerBuffersStub(MessageParcel& data,
    MessageParcel& reply, MessageOption& option)
{
    ReleaseLayerBuffersInfo releaseLayerInfo;
    releaseLayerInfo.screenId = data.ReadUint64();
    uint32_t vecSize;
    if (!data.ReadUint32(vecSize)) {
        RS_LOGE("%{public}s read vecSize error", __func__);
        return COMPOSITOR_ERROR_BINDER_ERROR;
    }
    if (vecSize > RELEASE_LAYER_MAX_SIZE) {
        RS_LOGE("Release layer size invalid: %{public}d", vecSize);
        return ERR_INVALID_DATA;
    }
    for (uint32_t i = 0; i < vecSize; i++) {
        uint64_t layerId;
        if (!data.ReadUint64(layerId)) {
            RS_LOGE("%{public}s read layerId error", __func__);
            return COMPOSITOR_ERROR_BINDER_ERROR;
        }
        bool isSupportedPresentTimestamp;
        if (!data.ReadBool(isSupportedPresentTimestamp)) {
            RS_LOGE("%{public}s read isSupportedPresentTimestamp error", __func__);
            return COMPOSITOR_ERROR_BINDER_ERROR;
        }
        GraphicPresentTimestamp timestamp;
        uint32_t presentTimestampType;
        if (!data.ReadUint32(presentTimestampType)) {
            RS_LOGE("%{public}s read presentTimestampType error", __func__);
            return COMPOSITOR_ERROR_BINDER_ERROR;
        }
        timestamp.type = static_cast<GraphicPresentTimestampType>(presentTimestampType);
        if (!data.ReadInt64(timestamp.time)) {
            RS_LOGE("%{public}s read timestamp.time error", __func__);
            return COMPOSITOR_ERROR_BINDER_ERROR;
        }
        releaseLayerInfo.timestampVec.push_back(std::tuple(layerId, isSupportedPresentTimestamp, timestamp));
    }
    if (!data.ReadUint32(vecSize)) {
        RS_LOGE("%{public}s read vecSize error", __func__);
        return COMPOSITOR_ERROR_BINDER_ERROR;
    }
    if (vecSize > RELEASE_LAYER_MAX_SIZE) {
        RS_LOGE("Release layer size invalid: %{public}d", vecSize);
        return ERR_INVALID_DATA;
    }
    for (uint32_t i = 0; i < vecSize; i++) {
        uint64_t layerId;
        if (!data.ReadUint64(layerId)) {
            RS_LOGE("%{public}s read layerId error", __func__);
            return COMPOSITOR_ERROR_BINDER_ERROR;
        }
        bool hasBuffer;
        if (!data.ReadBool(hasBuffer)) {
            RS_LOGE("%{public}s read hasBuffer error", __func__);
            return COMPOSITOR_ERROR_BINDER_ERROR;
        }
        sptr<SurfaceBuffer> buffer = nullptr;
        if (hasBuffer) {
            auto readSafeFdFunc = [](OHOS::MessageParcel& parcel,
                std::function<int(OHOS::MessageParcel&)> readFdDefaultFunc) -> int {
                return parcel.ReadFileDescriptor();
            };
            uint32_t sequence;
            auto ret = ReadSurfaceBufferImpl(data, sequence, buffer, readSafeFdFunc);
            if (ret != GSERROR_OK) {
                return ERR_INVALID_DATA;
            }
        }
        sptr<SyncFence> fence = SyncFence::ReadFromMessageParcel(data);
        releaseLayerInfo.releaseBufferFenceVec.push_back(std::tuple(layerId, buffer, fence));
    }
    data.ReadInt64(releaseLayerInfo.lastSwapBufferTime);
    auto replyMessage = ReleaseLayerBuffers(releaseLayerInfo);
    reply.WriteInt32(replyMessage);
    return COMPOSITOR_ERROR_OK;
}

int32_t RSComposerToRenderConnectionStub::NotifyLppLayerToRenderStub(MessageParcel& data,
    MessageParcel& reply, MessageOption& option)
{
    uint64_t vsyncId;
    if (!data.ReadUint64(vsyncId)) {
        ROSEN_LOGE("%{public}s read vsyncId failed", __func__);
        return ERR_INVALID_DATA;
    }
    int32_t size;
    if (!data.ReadInt32(size) || size > MAX_LPP_LAYER_SIZE) {
        ROSEN_LOGE("%{public}s read size failed", __func__);
        return ERR_INVALID_DATA;
    }
    std::unordered_set<uint64_t> lppNodeIds;
    for (int32_t i = 0; i < size; i++) {
        uint64_t nodeId;
        if (!data.ReadUint64(nodeId)) {
            ROSEN_LOGE("%{public}s read nodeId failed", __func__);
            return ERR_INVALID_DATA;
        }
        lppNodeIds.insert(nodeId);
    }
    auto replyMessage = NotifyLppLayerToRender(vsyncId, lppNodeIds);
    reply.WriteInt32(replyMessage);
    return COMPOSITOR_ERROR_OK;
}
} // namespace Rosen
} // namespace OHOS