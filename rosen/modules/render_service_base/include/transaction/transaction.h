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
    GSError WriteToMessageParcel(MessageParcel& parcel, bool hasBuffer);
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

using BufferAndFence = std::pair<sptr<SurfaceBuffer>, sptr<SyncFence>>;

class RSB_EXPORT RSTransactionManager : public IConsumerSurface {
public:
    static constexpr uint32_t BUFFER_MATRIX_SIZE = 16;
    static constexpr uint32_t ONE_SECOND_TIMESTAMP = 1e9;
    /**
     * @brief Disable copy construction.
     */
    RSTransactionManager(const RSTransactionManager&) = delete;
    /**
     * @brief Disable copy assignment.
     */
    RSTransactionManager& operator=(const RSTransactionManager&) = delete;
    /**
     * @brief Disable move construction.
     */
    RSTransactionManager(RSTransactionManager&&) = delete;
    /**
     * @brief Disable move assignment.
     */
    RSTransactionManager& operator=(RSTransactionManager&&) = delete;
    /**
     * @brief Factory method to create a new RSTransactionManager.
     * @param uniqueId A unique identifier for this manager instance.
     * @param name A human-readable name for logging/tracing.
     * @param maxQueueSize Maximum number of pending transactions to hold.
     * @param transactionCallback Callback invoked when a transaction completes.
     * @param transactionListener Listener for consumer-surface buffer events.
     * @return A sptr<RSTransactionManager> on success, or nullptr on failure.
     */
    static sptr<RSTransactionManager> Create(uint64_t uniqueId, std::string name, uint32_t maxQueueSize,
        sptr<RSITransactionCallback> transactionCallback, sptr<IBufferConsumerListener> transactionListener);
    /**
     * @brief Destructor. Cleans up internal resources.
     */
    ~RSTransactionManager();
    /**
     * @brief Enqueue a new transaction for later processing.
     * @param config Configuration details for the new transaction.
     * @return {@link GSERROR_OK} 0 - Success.
     *         {@link GSERROR_INVALID_ARGUMENTS} 40001000 - invaild arguments.
     */
    GSError QueueTransaction(const RSTransactionConfig& config);
    /**
     * @brief Get the current number of pending transactions.
     * @return The size of the pending transaction queue.
     */
    uint32_t GetPendingTransactionQueueSize() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return pendingTransactionQueue_.size();
    }
    /**
     * @brief Update manager configuration (e.g., queue size, transform).
     * @param config New configuration parameters to apply.
     * @return {@link GSERROR_OK} 0 - Success.
     *         {@link GSERROR_NO_BUFFER} 40601000 - no buffer.
     */
    GSError UpdateTransactionConfig(const RSTransactionManagerConfig& config);
    
    void OnGoBackground();
    /**
     * @brief Clear all internal caches and pending transactions.
     *        Used when a full reset of surface state is required.
     */
    void OnCleanCache();
    /**
     * @brief Called when the transform hint for this surface changes.
     *        Adjusts internal processing to honor the new transform.
     */
    void OnTransformChange();
    /**
     * @brief Notify the manager that the surface tunnel handle has changed.
     * @param handle New tunnel handle for buffer routing.
     */
    void OnTunnelHandleChange(const sptr<SurfaceTunnelHandle>& handle);
    /**
     * @brief Handle a buffer deletion event from the producer side.
     * @param sequence The sequence number of the buffer to delete.
     * @param isForUniRedraw True for a RSHardwareThread, flase for RSMainThread.
     */
    void OnDeleteBuffer(uint32_t sequence, bool isForUniRedraw);
    /**
     * @brief Acquire buffer for data consumed.
     * When the fenceFd is used up, you need to close it.
     *
     * @param buffer [out] The buffer for data consumed.
     * @param fence [out] fence fd for asynchronous waiting mechanism.
     * @param timestamp [out] The timestamp of the produced data.
     * @param damage [out] The dirty buffer area set by the producer.
     * @return {@link GSERROR_OK} 0 - Success.
     *         {@link GSERROR_NO_BUFFER} 40601000 - no buffer.
     */
    GSError AcquireBuffer(sptr<SurfaceBuffer>& buffer, sptr<SyncFence>& fence, int64_t& timestamp,
                          std::vector<OHOS::Rect>& damages_) override;
    /**
     * @brief Acquire buffer for data consumed.
     *
     * @param buffer [out] The buffer for data consumed.
     * @param fence [out] fence fd for asynchronous waiting mechanism.
     * @param timestamp [out] The timestamp of the produced data.
     * @param damage [out] The dirty buffer area set by the producer.
     * @return {@link GSERROR_OK} 0 - Success.
     *         {@link GSERROR_NO_BUFFER} 40601000 - no buffer.
     *         {@link GSERROR_NO_BUFFER_READY} 40605000 - buffer's desirePresentTimestamp has not arrived.
     *
     */
    GSError AcquireBuffer(AcquireBufferReturnValue& returnValue, int64_t expectPresentTimestamp,
                          bool isUsingAutoTimestamp) override;
    /**
     * @brief Release buffer for data production.
     *
     * @param buffer [in] Consumed data buffer.
     * @param fence [in] fence fd for asynchronous waiting mechanism.
     * @return {@link GSERROR_OK} 0 - Success.
     * {@link GSERROR_INVALID_ARGUMENTS} 40001000 - Param invalid.
     * {@link SURFACE_ERROR_BUFFER_NOT_INCACHE} 41210000 - Buffer not in cache.
     * {@link SURFACE_ERROR_BUFFER_STATE_INVALID} 41207000 - Buffer state invalid.
     */
    GSError ReleaseBuffer(sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& releaseFence) override;
    /**
     * @brief Clean the surface buffer cache and inform consumer.
     * This interface will empty all caches of the current process.
     *
     * @return {@link GSERROR_OK} 0 - Success.
     * {@link GSERROR_INVALID_ARGUMENTS} 40001000 - Param invalid.
     */
    GSError GoBackground() override;
    /**
     * @brief Determine whether a buffer is ready for use
     *
     * @return true There has available buffer.
     * @return false There has not available buffer.
     */
    bool QueryIfBufferAvailable() override;
    /**
     * @brief Get the Surface Source Type of the surface.
     *
     * @return OHSurfaceSource The source type.
     */
    OHSurfaceSource GetSurfaceSourceType() const override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return sourceType_;
    }
    /**
     * @brief Set the Surface Source Type for the surface.
     *
     * @param sourceType [in] The source type.
     * @return {@link GSERROR_OK} 0 - Success.
     * {@link GSERROR_INVALID_ARGUMENTS} 40001000 - Param invalid.
     */
    GSError SetSurfaceSourceType(OHSurfaceSource sourceType) override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        sourceType_ = sourceType;
        return GSERROR_OK;
    }
    /**
     * @brief Get the Unique Id of the surface.
     *
     * @return uint64_t The Unique Id of the surface.
     */
    uint64_t GetUniqueId() const override
    {
        return uniqueId_;
    }
    /**
     * @brief Get the Max Queue Size form the surface.
     *
     * @param queueSize [out] the Max Queue Size.
     * @return {@link GSERROR_OK} 0 - Success.
     * {@link SURFACE_ERROR_UNKOWN} 50002000 - Inner error.
     */
    GSError GetMaxQueueSize(uint32_t& queueSize) const override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queueSize = maxQueueSize_;
        return GSERROR_OK;
    }
    /**
     * @brief Get the Name of the surface.
     *
     * @return const std::string& The name of The surface.
     */
    const std::string& GetName() override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return name_;
    }
    /**
     * @brief Get the Tunnel Handle of the surface.
     *
     * @return sptr<SurfaceTunnelHandle> Tunnel handle.
     */
    sptr<SurfaceTunnelHandle> GetTunnelHandle() const override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return tunnelHandle_;
    }
    /**
     * @brief Get the Transform Hint from the surface
     *
     * @return GraphicTransformType The type of transform hint.
     */
    GraphicTransformType GetTransform() const override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return transform_;
    }
    /**
     * @brief Get the buffer hold state.
     *
     * @return true The buffer hold is on.
     * @return false The buffer hold is off.
     */
    bool IsBufferHold() override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return isBufferHold_;
    }
    /**
     * @brief Set the Buffer Hold for the surface.
     *
     * @param hold [in] Indicates the switch to bool instance.
     */
    void SetBufferHold(bool isHold) override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        isBufferHold_ = isHold;
    }
    /**
     * @brief Register consumer listener function.
     *
     * @param listener The callback of consumer function.
     * @return {@link GSERROR_OK} 0 - Success.
     * {@link GSERROR_INVALID_ARGUMENTS} 40001000 - Param invalid.
     */
    GSError RegisterConsumerListener(sptr<IBufferConsumerListener>& listener) override;
    /**
     * @brief Register delete buffer listener function.
     *
     * @param func The callback of delete buffer function.
     * @param isForUniRedraw Is for uni redraw.
     * @return {@link GSERROR_OK} 0 - Success.
     * {@link GSERROR_INVALID_ARGUMENTS} 40001000 - Param invalid.
     */
    GSError RegisterDeleteBufferListener(OHOS::OnDeleteBufferFunc func, bool isForUniRedraw = false) override;
    /**
     * @brief Set the Default Width And Height for the surface.
     *
     * @param width [in] The default width for the surface.
     * @param height [in] The default height for the surface.
     * @return {@link GSERROR_OK} 0 - Success.
     * {@link GSERROR_INVALID_ARGUMENTS} 40001000 - Param invalid.
     */
    GSError SetDefaultWidthAndHeight(int32_t width, int32_t height) override;
    /**
     * @brief Get the Last Flushed Desired Present Time Stamp from the surface.
     *
     * @param lastFlushedDesiredPresentTimeStamp [out] The Last Flushed Desired Present Time Stamp
     * @return {@link GSERROR_OK} 0 - Success.
     * {@link SURFACE_ERROR_UNKOWN} 50002000 - Internal error.
     */
    GSError GetLastFlushedDesiredPresentTimeStamp(int64_t& lastFlushedDesiredPresentTimeStamp) const override;
    /**
     * @brief Get the Buffer Support Fast Compose from the surface.
     *
     * @param bufferSupportFastCompose [out] The Buffer Support Fast Compose.
     * @return {@link GSERROR_OK} 0 - Success.
     * {@link SURFACE_ERROR_UNKOWN} 50002000 - Internal error.
     */
    GSError GetBufferSupportFastCompose(bool& bufferSupportFastCompose) override;
    /**
     * @brief Get the Cycle Buffers Number from the surface.
     *
     * @param cycleBuffersNumber [out] The Cycle Buffers Number of the surface.
     * @return {@link GSERROR_OK} 0 - Success.
     * {@link SURFACE_ERROR_UNKOWN} 50002000 - Inner error.
     */
    GSError GetCycleBuffersNumber(uint32_t& cycleBuffersNumber) override;
    /**
     * @brief Get the meta data type of the surface buffer.
     *
     * @param sequence [in] The number of surface buffer.
     * @param type [out] The meta data type.
     * @return {@link GSERROR_OK} 0 - Success.
     * {@link GSERROR_INVALID_ARGUMENTS} 40001000 - Param invalid.
     */
    GSError QueryMetaDataType(uint32_t sequence, HDRMetaDataType& type) const override;
    /**
     * @brief Get the Meta Data of the surface buffer
     *
     * @param sequence [in] The number of surface buffer.
     * @param metaData [out] Meta data info.
     * @return {@link GSERROR_OK} 0 - Success.
     * {@link GSERROR_INVALID_ARGUMENTS} 40001000 - Param invalid.
     */
    GSError GetMetaData(uint32_t sequence, std::vector<GraphicHDRMetaData>& metaData) const override;
    /**
     * @brief Get the Meta Data of the surface buffer
     *
     * @param sequence [in] The number of surface buffer.
     * @param key [in] The key of meta data.
     * @param metaData [out] Meta data info.
     * @return {@link GSERROR_OK} 0 - Success.
     * {@link GSERROR_INVALID_ARGUMENTS} 40001000 - Param invalid.
     */
    GSError GetMetaDataSet(uint32_t sequence, GraphicHDRMetadataKey& key,
        std::vector<uint8_t>& metaDataSet) const override;
    /**
     * @brief Dump info of the surface.
     *
     * @param result The info of the surface.
     */
    void Dump(std::string& result) const override;
    /**
     * @brief Dump current frame layer info of the surface.
     *
     */
    void DumpCurrentFrameLayer() const override;
    /**
     * @brief Get the Last Consume Time from the surface.
     *
     * @param lastConsumeTime [out] The Last Consume Time.
     * @return {@link GSERROR_OK} 0 - Success.
     * {@link SURFACE_ERROR_UNKOWN} 50002000 - Inner error.
     */
    GSError GetLastConsumeTime(int64_t& lastConsumeTime) const override;
    /**
     * @brief Get the Surface Buffer Transform Type from the surface.
     *
     * @param buffer [in] Indicates the pointer to a SurfaceBuffer instance.
     * @param transformType [out] The Transform type.
     * @return {@link GSERROR_OK} 0 - Success.
     * {@link GSERROR_INVALID_ARGUMENTS} 40001000 - Param invalid.
     */
    GSError GetSurfaceBufferTransformType(sptr<SurfaceBuffer> buffer, GraphicTransformType *transformType) override;
    /**
     * @brief Get the Available Buffer Count from the surface.
     *
     * @return uint32_t The Available Buffer Count of the surface.
     */
    uint32_t GetAvailableBufferCount() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return pendingTransactionQueue_.size();
    }
private:
    RSTransactionManager(uint64_t uniqueId, std::string name, uint32_t maxQueueSize,
        sptr<RSITransactionCallback> transactionCallback, sptr<IBufferConsumerListener> transactionListener);
    GSError ReleaseBufferLocked(uint32_t bufferSeqNum, const sptr<SyncFence>& releaseFence);
    sptr<Transaction> AcquireBufferLocked();
    void OnBufferDeleteForRS(uint32_t sequence);
    void ReleaseDropBuffers(std::vector<uint32_t>& dropBuffers);
    void LogAndTraceAllBufferInPendingTransactionQueueLocked();

    mutable std::mutex mutex_;
    uint64_t uniqueId_ = 0;
    std::string name_ = "";
    uint32_t maxQueueSize_ = SURFACE_DEFAULT_QUEUE_SIZE;
    std::vector<sptr<Transaction>> pendingTransactionQueue_ = {};
    sptr<RSITransactionCallback> transactionCallback_ = nullptr;
    sptr<IBufferConsumerListener> transactionListener_ = nullptr;
    sptr<SurfaceTunnelHandle> tunnelHandle_ = nullptr;
    OnDeleteBufferFunc onBufferDeleteForRSMainThread_ = nullptr;
    OnDeleteBufferFunc onBufferDeleteForRSHardwareThread_ = nullptr;
    GraphicTransformType transform_ = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    bool isBufferHold_ = false;
    OHSurfaceSource sourceType_ = OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT;
    int64_t lastFlushedDesiredPresentTimeStamp_ = 0;
    bool bufferSupportFastCompose_ = false;
    uint32_t rotatingBufferNumber_ = 0;
    std::unordered_map<uint32_t, sptr<Transaction>> transactionQueueCache_ = {};
    int64_t lastConsumeTime_ = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_RS_TRANSACTION_H