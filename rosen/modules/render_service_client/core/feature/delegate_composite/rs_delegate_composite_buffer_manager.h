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

#ifndef RS_DELEGATE_COMPOSITE_BUFFER_MANAGER_H
#define RS_DELEGATE_COMPOSITE_BUFFER_MANAGER_H
#ifndef ROSEN_CROSS_PLATFORM
#include <mutex>
#include "iconsumer_surface.h"
#include "surface.h"
#include "surface_type.h"
#include "transaction/transaction.h"
#include "ui/rs_ui_context.h"
#include "transaction/rs_transaction_handler.h"
#include "common/rs_common_def.h"
namespace OHOS {
namespace Rosen {

enum class TransactionState : uint8_t {
    TRANSACTION_STATE_RELEASED,
    TRANSACTION_STATE_QUEUED,
};

struct SetBufferParams {
    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> fence;
    std::vector<Rect> damages;
    int64_t timestamp;
    int64_t desiredPresentTimestamp;
    bool isAutoTimestamp;
    int64_t requestTimeNs;
    int64_t flushTimeNs;
    uint32_t bufferSequence;
    HDRMetaDataType hdrMetaDataType = HDRMetaDataType::HDR_NOT_USED;
    std::vector<GraphicHDRMetaData> metaData;
    GraphicHDRMetadataKey metaDataKey = GraphicHDRMetadataKey::GRAPHIC_MATAKEY_RED_PRIMARY_X;
    std::vector<uint8_t> metaDataSet;
};

struct SetBufferCacheInfo {
    sptr<SurfaceBuffer> buffer;
    TransactionState state;
};

class RSDelegateCompositeBufferManager : public RefBase {
public:
    using BufferReleaseCallback = std::function<void(int release_fence_fd)>;
    bool SetBuffer(sptr<SurfaceBuffer> buffer, UniqueFd fence_fd, const BufferReleaseCallback& callback);

    RSDelegateCompositeBufferManager(std::shared_ptr<RSUIContext> rsUIContext, sptr<Surface> psurface,
        const NodeId& nodeId, const std::string& name);
    ~RSDelegateCompositeBufferManager() override;

    GSError ReleaseBuffer(uint32_t sequence, sptr<SyncFence> fence);
    void CleanBuffer(bool cleanAll);
    void SetDesiredPresentTime(int64_t desiredPresentTime);
    void SetDamageRegion(std::vector<Rect> rects);
    void SetBufferTransform(GraphicTransformType transform);
    void SetDelegateDstRect(float positionX, float positionY, float positionZ, float positionW);
    void SetDelegateSrcRect(float positionX, float positionY, float positionZ, float positionW);
    bool SetDelegateMode(bool isDelegateMode);

private:
    void GenerateTransactionConfigInner(SetBufferParams& params, RSTransactionConfig& config);
    RSTransactionConfig GenerateTransactionConfig(SetBufferParams params, BufferReleaseCallback releaseCallback);
    void GenerateBufferParams(SetBufferParams &params, const int64_t &desiredPresentTime);
    RSTransactionConfig GenerateTransactionConfigIfNecessary(int64_t desiredPresentTime);
    void GenerateTransactionConfigDelList(RSTransactionConfig& config);

    std::shared_ptr<RSUIContext> rsUIContext_ = nullptr;
    std::shared_ptr<RSTransactionHandler> rsTransactionHandle_ = nullptr;
    sptr<Surface> psurface_ = nullptr;

    sptr<IConsumerSurface> consumerSurface_;
    std::map<uint32_t, TransactionState> transactionCache_;
    std::map<uint32_t, SetBufferCacheInfo> transactionCacheForSetBuffer_;
    int32_t setBufferCountAfterCleanCache_ = -1;
    sptr<SurfaceBuffer> preCachebuffer_ = nullptr;
    sptr<IBufferConsumerListener> listener_ = nullptr;
    bool isTransactionConfigSent_ = false;
    RSTransactionManagerConfig transactionConfig_{};
    // This map should only contain several callbacks, it is better to use a lightweight container.
    mutable std::mutex bufferReleaseCallbacksMutex_;
    std::map<uint32_t, BufferReleaseCallback> bufferReleaseCallbacks_;
    sptr<SurfaceBuffer> buffer_;
    std::vector<Rect> damage_region_;
    bool needSyncBufferToNode_ = false;
    int64_t desiredPresentTime_ = 0;
    BufferReleaseCallback release_callback_;
    GraphicTransformType buffer_transform_ = GRAPHIC_ROTATE_NONE;
    UniqueFd fence_fd_;
    NodeId nodeId_ = 0;
    std::string name_ = "Unnamed";
    mutable std::mutex cacheMutex_;
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_CROSS_PLATFORM
#endif
