/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#include "rs_render_to_composer_connection_stub.h"
#include <unistd.h>
#include "accesstoken_kit.h"
#include "graphic_common.h"
#include "ipc_skeleton.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
int32_t RSRenderToComposerConnectionStub::OnRemoteRequest(uint32_t code, OHOS::MessageParcel& data,
    OHOS::MessageParcel& reply, MessageOption& option)
{
    auto remoteDescriptor = data.ReadInterfaceToken();
    if (GetDescriptor() != remoteDescriptor) {
        RS_LOGE("RSRenderToComposerConnectionStub::OnRemoteRequest ReadInterfaceToken failed.");
        return ERR_INVALID_STATE;
    }
    int32_t ret = COMPOSITOR_ERROR_OK;
    switch (code) {
        case IRENDER_TO_COMPOSER_CONNECTION_COMMIT_LAYERS: {
            auto transactionData = ParseTransactionData(data);
            CommitLayers(transactionData);
            break;
        }
        case IRENDER_TO_COMPOSER_CONNECTION_CLEAR_FRAME_BUFFERS: {
            ClearFrameBuffers();
            break;
        }
        case IRENDER_TO_COMPOSER_CONNECTION_CLEAN_LAYER_BUFFER_BY_SURFACE_ID: {
            uint64_t surfaceId = 0;
            if (GetCleanLayerBufferSurfaceId(data, surfaceId) == COMPOSITOR_ERROR_OK) {
                CleanLayerBufferBySurfaceId(surfaceId);
            }
            break;
        }
        case IRENDER_TO_COMPOSER_CONNECTION_CLEAR_REDRAW_GPU_COMPOSITION_CACHE: {
            const auto& bufferIds = ParseClearRedrawCacheBufferIds(data);
            ClearRedrawGPUCompositionCache(bufferIds);
            break;
        }
        case IRENDER_TO_COMPOSER_CONNECTION_SET_BACKLIGHT_LEVEL: {
            uint32_t level = 0;
            if (GetBacklightLevel(data, level) == COMPOSITOR_ERROR_OK) {
                SetScreenBacklight(level);
            }
            break;
        }        
        default: {
            ret = COMPOSITOR_ERROR_BINDER_ERROR;
            break;
        }
    }
    return ret;
}

std::unique_ptr<RSLayerTransactionData> RSRenderToComposerConnectionStub::ParseTransactionData(OHOS::MessageParcel& parcel)
{
    RS_TRACE_NAME("ParseTransactionData data size:" + std::to_string(parcel.GetDataSize()));
    auto transactionData = RSLayerTransactionData::Unmarshalling(parcel);
    if (!transactionData) {
        RS_LOGE("RSLayerTransactionData::Unmarshalling failed.");
        return nullptr;
    }
    lastSendingPid_ = transactionData->GetSendingPid();
    std::unique_ptr<RSLayerTransactionData> transData(transactionData);
    return transData;
}

int32_t RSRenderToComposerConnectionStub::GetCleanLayerBufferSurfaceId(OHOS::MessageParcel& parcel, uint64_t& surfaceId)
{
    if (!parcel.ReadUint64(surfaceId)) {
        RS_LOGE("RSRenderToComposerConnectionStub::GetCleanLayerBufferSurfaceId read surface id failed.");
        return COMPOSITOR_ERROR_BINDER_ERROR;
    }
    return COMPOSITOR_ERROR_OK;
}

std::set<uint64_t> RSRenderToComposerConnectionStub::ParseClearRedrawCacheBufferIds(OHOS::MessageParcel& parcel)
{
    std::vector<uint64_t> bufferIdsVector;
    std::set<uint64_t> bufferIds;
    if (!parcel.ReadUInt64Vector(&bufferIdsVector)) {
        RS_LOGE("RSRenderToComposerConnectionStub::ParseClearRedrawCacheBufferIds read buffer ids failed.");
        return bufferIds;
    }
    bufferIds.insert(bufferIdsVector.begin(), bufferIdsVector.end());
    return bufferIds;
}

int32_t RSRenderToComposerConnectionStub::GetBacklightLevel(OHOS::MessageParcel& parcel, uint32_t& level)
{
    if (!parcel.ReadUint32(level)) {
        RS_LOGE("RSRenderToComposerConnectionStub::GetBacklightLevel failed.");
        return COMPOSITOR_ERROR_BINDER_ERROR;
    }
    return COMPOSITOR_ERROR_OK;
}
} // namespace Rosen
} // namespace OHOS