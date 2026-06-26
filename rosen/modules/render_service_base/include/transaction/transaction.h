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

#ifndef ROSEN_RENDER_SERVICE_BASE_RS_TRANSACTION_H
#define ROSEN_RENDER_SERVICE_BASE_RS_TRANSACTION_H

#include <deque>
#include <mutex>
#include <shared_mutex>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "iconsumer_surface.h"
#include "ibuffer_consumer_listener.h"
#include "ipc_callbacks/rs_transaction_callback.h"
#include "ipc_callbacks/rs_delegate_composite_callback.h"
#include "parcel.h"
#include "surface_tunnel_handle.h"
#include "surface_buffer.h"
#include "sync_fence.h"
#include "external_window.h"
#include "command/rs_command.h"
#include "command/rs_command_factory.h"

namespace OHOS {
namespace Rosen {
struct RSTransactionConfig;
struct TransactionInfo {
    RSBufferInfo bufferInfo = {};
    sptr<SyncFence> fence = nullptr;
    int64_t uiTimestamp = 0;
    int64_t desiredPresentTimestamp = 0;
    std::vector<OHOS::Rect> damages = {};
    std::vector<GraphicHDRMetaData> metaData = {};
    std::vector<uint8_t> metaDataSet = {};
    HDRMetaDataType hdrMetaDataType = HDRMetaDataType::HDR_NOT_USED;
    GraphicHDRMetadataKey metaDataKey = GraphicHDRMetadataKey::GRAPHIC_MATAKEY_RED_PRIMARY_X;
    bool isAutoTimestamp = false;
};

class RSB_EXPORT Transaction : public RefBase {
public:
    Transaction() = default;
    Transaction(const sptr<SurfaceBuffer>& buffer);
    ~Transaction() = default;
    sptr<SurfaceBuffer> GetBuffer() const;
    uint32_t GetBufferSeqNum() const;
    sptr<SyncFence> GetFence() const;
    void SetFence(const sptr<SyncFence>& fence);
    const std::vector<OHOS::Rect>& GetDamages() const;
    void SetDamages(const std::vector<OHOS::Rect>& damages);
    int64_t GetUiTimestamp() const;
    void SetUiTimestamp(int64_t uiTimestamp);
    int64_t GetDesiredPresentTimestamp() const;
    void SetDesiredPresentTimestamp(int64_t desiredPresentTimestamp);
    bool GetAutoTimestamp() const;
    void SetAutoTimestamp(bool isAutoTimestamp);
    HDRMetaDataType GetHdrMetaDataType() const;
    const std::vector<GraphicHDRMetaData>& GetMetaData() const;
    const std::vector<uint8_t>& GetMetaDataSet() const;
    GraphicHDRMetadataKey GetMetaDataKey() const;
    void SetHdrMetaDataType(HDRMetaDataType type);
    void SetMetaData(const std::vector<GraphicHDRMetaData>& metaData);
    void SetMetaDataSet(const std::vector<uint8_t>& metaDataSet);
    void SetMetaDataKey(GraphicHDRMetadataKey key);
    void SetLastAcquireTime(int64_t lastAcquireTime);
    int64_t GetLastAcquireTime() const;
    GSError WriteToMessageParcel(MessageParcel& parcel, bool hasBuffer) const;
    static sptr<Transaction> ReadFromMessageParcel(MessageParcel& parcel);
    GSError ReadFromTransactionInfo(const TransactionInfo& info);
    static GSError ReadTransactionInfoFromMessageParcel(MessageParcel& parcel, TransactionInfo& info);
    static GSError ReadBufferInfoFromMessageParcel(MessageParcel& parcel, RSBufferInfo& info);
    static GSError ReadTransactionConfigFromMessageParcel(MessageParcel& parcel, RSTransactionConfig& config);
    static GSError WriteTransactionConfigToMessageParcel(MessageParcel& parcel, const RSTransactionConfig& config);

private:
    static GSError ReadMetaDataFromMessageParcel(MessageParcel& parcel, HDRMetaDataType& hdrMetaDataType,
        std::vector<GraphicHDRMetaData>& metaData, GraphicHDRMetadataKey& key, std::vector<uint8_t>& metaDataSet);
    GSError WriteMetaDataToMessageParcel(MessageParcel& parcel, HDRMetaDataType hdrMetaDataType,
        const std::vector<GraphicHDRMetaData>& metaData, GraphicHDRMetadataKey key,
        const std::vector<uint8_t>& metaDataSet) const;
    static sptr<SurfaceBuffer> ReadBufferFromMessageParcel(MessageParcel& parcel);

    mutable std::shared_mutex mutex_;
    sptr<SurfaceBuffer> buffer_ = nullptr;
    uint32_t bufferSeqNum_ = 0;
    sptr<SyncFence> fence_ = nullptr;
    std::vector<OHOS::Rect> damages_ = {};
    int64_t uiTimestamp_ = 0;
    int64_t desiredPresentTimestamp_ = 0;
    bool isAutoTimestamp_ = false;
    HDRMetaDataType hdrMetaDataType_ = HDRMetaDataType::HDR_NOT_USED;
    std::vector<GraphicHDRMetaData> metaData_ = {};
    GraphicHDRMetadataKey metaDataKey_ = GraphicHDRMetadataKey::GRAPHIC_MATAKEY_RED_PRIMARY_X;
    std::vector<uint8_t> metaDataSet_ = {};
    /**
     * lastAcquireTime_ is used to record the last acquire time of the buffer
     */
    int64_t lastAcquireTime_ = 0;
};

struct RSTransactionManagerConfig {
    int64_t lastFlushedDesiredPresentTimeStamp = 0;
    uint32_t bufferQueueSize = SURFACE_DEFAULT_QUEUE_SIZE;
    uint32_t rotatingBufferNumber = 0;
    GraphicTransformType transform = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    OHSurfaceSource sourceType = OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT;
    bool isBufferHold = false;
    bool bufferSupportFastCompose = false;
};

struct RSTransactionConfig {
    uint64_t nodeId = 0;
    sptr<Transaction> transaction = nullptr;
    std::vector<uint32_t> delList = {};
    TransactionInfo transactionInfo = {};
    int64_t requestTimeNs = 0;
    int64_t flushTimeNs = 0;
    int32_t pid = 0;
    uint32_t bufferSequenceNum = 0;
    bool hasBuffer = false;
};

using BufferAndFence = std::pair<sptr<SurfaceBuffer>, sptr<SyncFence>>;
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_RENDER_SERVICE_BASE_RS_TRANSACTION_H
