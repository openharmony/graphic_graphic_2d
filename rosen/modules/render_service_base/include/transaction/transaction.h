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
#include "parcel.h"
#include "surface_tunnel_handle.h"
#include "surface_buffer.h"
#include "sync_fence.h"
#include "external_window.h"

namespace OHOS {
namespace Rosen {
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
    Transaction(sptr<SurfaceBuffer>& buffer) : buffer_(buffer)
    {
        if (buffer != nullptr) {
            bufferSeqNum_ = buffer->GetSeqNum();
        }
    }

    ~Transaction() = default;
    sptr<SurfaceBuffer> GetBuffer() const
    {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return buffer_;
    }
    uint32_t GetBufferSeqNum() const
    {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return bufferSeqNum_;
    }
    sptr<SyncFence> GetFence() const
    {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return fence_;
    }
    void SetFence(sptr<SyncFence> fence)
    {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        fence_ = fence;
    }
    const std::vector<OHOS::Rect>& GetDamages() const
    {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return damages_;
    }
    void SetDamages(std::vector<OHOS::Rect>& damages)
    {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        damages_ = damages;
    }
    int64_t GetUiTimestamp() const
    {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return uiTimestamp_;
    }
    void SetUiTimestamp(int64_t uiTimestamp)
    {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        uiTimestamp_ = uiTimestamp;
    }
    int64_t GetDesiredPresentTimestamp() const
    {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return desiredPresentTimestamp_;
    }
    void SetDesiredPresentTimestamp(int64_t desiredPresentTimestamp)
    {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        desiredPresentTimestamp_ = desiredPresentTimestamp;
    }
    bool GetAutoTimestamp() const
    {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return isAutoTimestamp_;
    }
    void SetAutoTimestamp(bool isAutoTimestamp)
    {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        isAutoTimestamp_ = isAutoTimestamp;
    }
    HDRMetaDataType GetHdrMetaDataType() const
    {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return hdrMetaDataType_;
    }
    const std::vector<GraphicHDRMetaData>& GetMetaData() const
    {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return metaData_;
    }
    const std::vector<uint8_t>& GetMetaDataSet() const
    {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return metaDataSet_;
    }
    GraphicHDRMetadataKey GetMetaDataKey() const
    {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return metaDataKey_;
    }
    void SetHdrMetaDataType(HDRMetaDataType type)
    {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        hdrMetaDataType_ = type;
    }
    void SetMetaData(const std::vector<GraphicHDRMetaData>& metaData)
    {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        metaData_ = metaData;
    }
    void SetMetaDataSet(const std::vector<uint8_t>& metaDataSet)
    {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        metaDataSet_ = metaDataSet;
    }
    void SetMetaDataKey(GraphicHDRMetadataKey key)
    {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        metaDataKey_ = key;
    }
    void SetlastAcquireTime(int64_t lastAcquireTime)
    {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        lastAcquireTime_ = lastAcquireTime;
    }
    int64_t GetLastAcquireTime()
    {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return lastAcquireTime_;
    }
    GSError WriteToMessageParcel(MessageParcel& parcel);
    static sptr<Transaction> ReadFromMessageParcel(MessageParcel& parcel);
    GSError ReadFromTransactionInfo(const TransactionInfo& info);
    static GSError ReadTransactionInfoFromMessageParcel(MessageParcel& parcel, TransactionInfo& info);
    static GSError ReadBufferInfoFromMessageParcel(MessageParcel& parcel, RSBufferInfo& info);

private:
    static GSError ReadMetaDataFromMessageParcel(MessageParcel& parcel, HDRMetaDataType& hdrMetaDataType,
        std::vector<GraphicHDRMetaData>& metaData, GraphicHDRMetadataKey& key, std::vector<uint8_t>& metaDataSet);
    GSError WriteMetaDataToMessageParcel(MessageParcel& parcel, HDRMetaDataType hdrMetaDataType,
        const std::vector<GraphicHDRMetaData>& metaData, const GraphicHDRMetadataKey key,
        const std::vector<uint8_t>& metaDataSet);

    mutable std::shared_mutex mutex_;  // 读写锁
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
     * lastAcquireTime is the time when this buffer was acquired last time through AcquireBuffer interface.
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
    sptr<Transaction> transaction = nullptr;
    std::vector<uint32_t> delList = {};
    TransactionInfo transactionInfo = {};
    int64_t requestTimeNs = 0;
    int64_t flushTimeNs = 0;
    int32_t pid = 0;
    uint32_t bufferSeqNum = 0;
    bool hasBuffer = false;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_RS_TRANSACTION_H