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

#include "rs_composer_to_render_connection_proxy.h"

#include <cstdint>
#include <message_option.h>
#include <message_parcel.h>
#include <vector>
#include "buffer_utils.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"

#undef LOG_TAG
#define LOG_TAG "RSComposerToRenderConnectionProxy"

namespace OHOS {
namespace Rosen {
RSComposerToRenderConnectionProxy::RSComposerToRenderConnectionProxy(const sptr<IRemoteObject>& impl) :
    IRemoteProxy<IRSComposerToRenderConnection>(impl) {}

int32_t RSComposerToRenderConnectionProxy::ReleaseLayerBuffers(ReleaseLayerBuffersInfo& releaseLayerInfo)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint64_t screenId = releaseLayerInfo.screenId;
    auto timestampVec = releaseLayerInfo.timestampVec;
    auto releaseBufferFenceVec = releaseLayerInfo.releaseBufferFenceVec;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(IRSComposerToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s write InterfaceToken failed", __func__);
        return -1;
    }
    if (!data.WriteUint64(screenId)) {
        ROSEN_LOGE("%{public}s: write screenId failed", __func__);
        return -1;
    }
    uint32_t vecSize = timestampVec.size();
    if (!data.WriteUint32(vecSize)) {
        ROSEN_LOGE("%{public}s write vecSize failed", __func__);
        return -1;
    }
    for (uint32_t i = 0; i < vecSize; i++) {
        if(!data.WriteUint64(std::get<0>(timestampVec[i]))) {
            ROSEN_LOGE("%{public}s write RSLayerId failed", __func__);
            return -1;
        }
        if(!data.WriteBool(std::get<1>(timestampVec[i]))) {
            ROSEN_LOGE("%{public}s write bool failed", __func__);
            return -1;
        }
        if(!data.WriteUint32((std::get<2>(timestampVec[i])).type)) {
            ROSEN_LOGE("%{public}s write timestamp failed", __func__);
            return -1;
        }
        if(!data.WriteInt64((std::get<2>(timestampVec[i])).time)) {
            ROSEN_LOGE("%{public}s write timestamp failed", __func__);
            return -1;
        }
    }
    vecSize = releaseBufferFenceVec.size();
    if (!data.WriteUint32(vecSize)) {
        ROSEN_LOGE("%{public}s write vecSiz failed", __func__);
        return -1;
    }
    for (uint32_t i = 0; i < vecSize; i++) {
        if(!data.WriteUint64(std::get<0>(releaseBufferFenceVec[i]))) {
            ROSEN_LOGE("%{public}s write RSLayerId failed", __func__);
            return -1;
        }
        if (std::get<1>(releaseBufferFenceVec[i]) != nullptr) {
            data.WriteBool(true);
            if (WriteSurfaceBufferImpl(data, std::get<1>(releaseBufferFenceVec[i])->GetSeqNum(),
                std::get<1>(releaseBufferFenceVec[i])) != GSERROR_OK) {
                ROSEN_LOGE("%{public}s write surface buffer failed", __func__);
                return -1;
            }
        } else {
            data.WriteBool(false);
        }
        if (!std::get<2>(releaseBufferFenceVec[i])->WriteToMessageParcel(data)) {
            ROSEN_LOGE("%{public}s write Fence failed", __func__);
            return -1;
        }
    }
    if (!data.WriteInt64(releaseLayerInfo.lastSwapBufferTime)) {
        ROSEN_LOGE("%{public}s write lastSwapBufferTime failed", __func__);
        return -1;
    }
    uint32_t code = static_cast<uint32_t>(ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: SendRequest failed, err is %{public}d", __func__, err);
        return -1;
    }

    auto replyMessage = reply.ReadInt32();
    RS_LOGD("%{public}s reply received successfully", __func__);

    return replyMessage;
}

int32_t RSComposerToRenderConnectionProxy::NotifyLppLayerToRender(
    uint64_t vsyncId, const std::unordered_set<uint64_t>& lppNodeIds)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(IRSComposerToRenderConnection::GetDescriptor())) {
        ROSEN_LOGE("%{public}s: write InterfaceToken failed", __func__);
        return -1;
    }
    if (!data.WriteUint64(vsyncId)) {
        ROSEN_LOGE("%{public}s write vsyncId failed", __func__);
        return -1;
    }

    int32_t size = static_cast<int32_t>(lppNodeIds.size());
    if (!data.WriteInt32(size)) {
        ROSEN_LOGE("%{public}s write lppNodeIds size failed", __func__);
        return -1;
    }

    for (uint64_t nodeId : lppNodeIds) {
        if (!data.WriteUint64(nodeId)) {
            ROSEN_LOGE("%{public}s write nodeId failed", __func__);
            return -1;
        }
    }

    RS_LOGI("%{public}s conn write vsyncId = %{public}" PRIu64 " remoteObj successfully", __func__, vsyncId);
    uint32_t code = static_cast<uint32_t>(NOTIFY_LPP_LAYER_TO_RENDER);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("%{public}s: SendRequest failed, err is %{public}d", __func__, err);
        return -1;
    }
    auto replyMessage = reply.ReadInt32();
    return replyMessage;
}
} // namespace Rosen
} // namespace OHOS