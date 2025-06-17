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
  
#include "transaction/transaction.h"

#include <string>

#include "buffer_utils.h"
#include "parameters.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"
#include "sandbox_utils.h"
#include "surface_buffer_impl.h"
#include "surface_utils.h"


namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t DUMP_BUFFER_WAIT_FENCE_TIMEOUT_MS = 3000;
}

GSError Transaction::WriteToMessageParcel(MessageParcel& parcel)
{
    if (buffer_ == nullptr || fence_ == nullptr) {
        RS_LOGE("Transaction::WriteToMessageParcel buffer_ == nullptr:%{public}d, fence_ == nullptr:%{public}d",
            buffer_ == nullptr, fence_ == nullptr);
        return GSERROR_INTERNAL;
    }
    uint32_t size = damages_.size();
    if (size > SURFACE_PARCEL_SIZE_LIMIT) {
        RS_LOGE("Transaction::WriteToMessageParcel damages size more than limit");
        return GSERROR_INVALID_ARGUMENTS;
    }
    if (!parcel.WriteUint32(size)) {
        RS_LOGE("Transaction::WriteToMessageParcel !parcel.Write size");
        return GSERROR_BINDER;
    }
    for (const auto& rect : damages_) {
        if (!parcel.WriteInt32(rect.x) || !parcel.WriteInt32(rect.y) || !parcel.WriteInt32(rect.w) ||
            !parcel.WriteInt32(rect.h)) {
            RS_LOGE("Transaction::WriteToMessageParcel !parcel.Write damages_");
            return GSERROR_BINDER;
        }
    }
    GSError ret = WriteSurfaceBufferImpl(parcel, buffer_->GetSeqNum(), buffer_);
    if (ret != GSERROR_OK) {
        RS_LOGE("Transaction::WriteToMessageParcel WriteSurfaceBufferImpl failed, ret:%{public}d", ret);
        return ret;
    }
    ret = buffer_->WriteBufferProperty(parcel);
    if (ret != GSERROR_OK) {
        RS_LOGE("Transaction::WriteToMessageParcel WriteBufferProperty failed, ret:%{public}d", ret);
        return ret;
    }
    if (!fence_->WriteToMessageParcel(parcel) || !parcel.WriteInt64(uiTimestamp_) ||
        !parcel.WriteInt64(desiredPresentTimestamp_) || !parcel.WriteBool(isAutoTimestamp_)) {
        RS_LOGE("Transaction !fence_->WriteToMessageParcel || !parcel.Write timestamp");
        return GSERROR_BINDER;
    }
    ret = WriteMetaDataToMessageParcel(parcel, hdrMetaDataType_, metaData_, metaDataKey_, metaDataSet_);
    if (ret != GSERROR_OK) {
        RS_LOGE("Transaction WriteMetaDataToMessageParcel ret:%{public}d", ret);
        return ret;
    }
    return GSERROR_OK;
}

GSError Transaction::ReadFromTransactionInfo(const TransactionInfo& info)
{
    buffer_->ReadFromBufferInfo(info.bufferInfo);
    damages_ = info.damages;
    fence_ = info.fence;
    uiTimestamp_ = info.uiTimestamp;
    desiredPresentTimestamp_ = info.desiredPresentTimestamp;
    isAutoTimestamp_ = info.isAutoTimestamp;
    hdrMetaDataType_ = info.hdrMetaDataType;
    metaData_ = info.metaData;
    metaDataKey_ = info.metaDataKey;
    metaDataSet_ = info.metaDataSet;
    return GSERROR_OK;
}

GSError Transaction::ReadTransactionInfoFromMessageParcel(MessageParcel& parcel, TransactionInfo& info)
{
    uint32_t size = parcel.ReadUint32();
    if (size > SURFACE_PARCEL_SIZE_LIMIT) {
        RS_LOGE("Transaction::ReadTransactionInfoFromMessageParcel more than limit");
        return GSERROR_INVALID_ARGUMENTS;
    }
    info.damages.reserve(size);
    for (uint32_t i = 0; i < size; i++) {
        OHOS::Rect rect = {
            .x = parcel.ReadInt32(),
            .y = parcel.ReadInt32(),
            .w = parcel.ReadInt32(),
            .h = parcel.ReadInt32(),
        };
        info.damages.emplace_back(rect);
    }
    auto ret = ReadBufferInfoFromMessageParcel(parcel, info.bufferInfo);
    if (ret != GSERROR_OK) {
        RS_LOGE("Transaction ReadBufferInfoFromMessageParcel ret:%{public}d", ret);
        return ret;
    }
    info.fence = SyncFence::ReadFromMessageParcel(parcel);
    info.uiTimestamp = parcel.ReadInt64();
    info.desiredPresentTimestamp = parcel.ReadInt64();
    info.isAutoTimestamp = parcel.ReadBool();
    ret =
        ReadMetaDataFromMessageParcel(parcel, info.hdrMetaDataType, info.metaData, info.metaDataKey, info.metaDataSet);
    if (ret != GSERROR_OK) {
        RS_LOGE("Transaction ReadMetaDataFromMessageParcel ret:%{public}d", ret);
    }
    return ret;
}

GSError Transaction::ReadBufferInfoFromMessageParcel(MessageParcel& parcel, RSBufferInfo& info)
{
    info.sequence = parcel.ReadUint32();
    if (!parcel.ReadBool()) {
        RS_LOGE("Transaction ReadBufferInfoFromMessageParcel no buffer, sequence:%{public}u", info.sequence);
        return GSERROR_INVALID_ARGUMENTS;
    }
    uint32_t colorGamut = 0;
    uint32_t transform = 0;
    int32_t scalingMode = 0;
    uint32_t configTransform = 0;
    if (!parcel.ReadInt32(info.bufferRequestConfig.width) || !parcel.ReadInt32(info.bufferRequestConfig.height) ||
        !parcel.ReadInt32(info.bufferRequestConfig.strideAlignment) ||
        !parcel.ReadInt32(info.bufferRequestConfig.format) || !parcel.ReadUint64(info.bufferRequestConfig.usage) ||
        !parcel.ReadInt32(info.bufferRequestConfig.timeout) || !parcel.ReadUint32(colorGamut) ||
        !parcel.ReadUint32(configTransform) || !parcel.ReadInt32(scalingMode) || !parcel.ReadUint32(transform)) {
        RS_LOGE("parcel read fail, seq: %{public}u.", info.sequence);
        return GSERROR_INVALID_ARGUMENTS;
    }
    info.surfaceBufferColorGamut = static_cast<GraphicColorGamut>(colorGamut);
    info.transform = static_cast<GraphicTransformType>(transform);
    info.surfaceBufferWidth = info.bufferRequestConfig.width;
    info.surfaceBufferHeight = info.bufferRequestConfig.height;
    info.scalingMode = static_cast<ScalingMode>(scalingMode);
    return GSERROR_OK;
}

sptr<Transaction> Transaction::ReadFromMessageParcel(MessageParcel& parcel)
{
    uint32_t size = parcel.ReadUint32();
    if (size > SURFACE_PARCEL_SIZE_LIMIT) {
        RS_LOGE("Transaction::ReadFromMessageParcel more than limit");
        return nullptr;
    }
    std::vector<OHOS::Rect> damages;
    damages.reserve(size);
    for (uint32_t i = 0; i < size; i++) {
        OHOS::Rect rect = {
            .x = parcel.ReadInt32(),
            .y = parcel.ReadInt32(),
            .w = parcel.ReadInt32(),
            .h = parcel.ReadInt32(),
        };
        damages.emplace_back(rect);
    }
    uint32_t sequence = 0;
    sptr<SurfaceBuffer> buffer = nullptr;
    GSError ret = ReadSurfaceBufferImpl(parcel, sequence, buffer);
    if (ret != GSERROR_OK || buffer == nullptr) {
        RS_LOGE("Transaction ReadSurfaceBufferImpl ret:%{public}d", ret);
        buffer = nullptr;
        return nullptr;
    }
    ret = buffer->ReadBufferProperty(parcel);
    if (ret != GSERROR_OK) {
        RS_LOGE("Transaction ReadBufferProperty ret:%{public}d", ret);
        buffer = nullptr;
        return nullptr;
    }
    sptr<SyncFence> fence = SyncFence::ReadFromMessageParcel(parcel);
    int64_t uiTimestamp = 0;
    int64_t desiredPresentTimestamp = 0;
    bool isAutoTimestamp = false;
    if (!parcel.ReadInt64(uiTimestamp) || !parcel.ReadInt64(desiredPresentTimestamp) ||
        !parcel.ReadBool(isAutoTimestamp)) {
        buffer = nullptr;
        return nullptr;
    }
    auto transaction = new Transaction(buffer);
    transaction->SetFence(fence);
    transaction->SetDamages(damages);
    transaction->SetUiTimestamp(uiTimestamp);
    transaction->SetDesiredPresentTimestamp(desiredPresentTimestamp);
    transaction->SetAutoTimestamp(isAutoTimestamp);
    HDRMetaDataType hdrMetaDataType = HDRMetaDataType::HDR_NOT_USED;
    std::vector<GraphicHDRMetaData> metaData{};
    GraphicHDRMetadataKey metaDataKey = GraphicHDRMetadataKey::GRAPHIC_MATAKEY_RED_PRIMARY_X;
    std::vector<uint8_t> metaDataSet{};
    ret = ReadMetaDataFromMessageParcel(parcel, hdrMetaDataType, metaData, metaDataKey, metaDataSet);
    if (hdrMetaDataType == HDRMetaDataType::HDR_NOT_USED) {
        return transaction;
    }
    transaction->SetHdrMetaDataType(hdrMetaDataType);
    transaction->SetMetaData(metaData);
    transaction->SetMetaDataKey(metaDataKey);
    transaction->SetMetaDataSet(metaDataSet);
    return transaction;
}

GSError Transaction::ReadMetaDataFromMessageParcel(MessageParcel& parcel, HDRMetaDataType& hdrMetaDataType,
    std::vector<GraphicHDRMetaData>& metaData, GraphicHDRMetadataKey& key, std::vector<uint8_t>& metaDataSet)
{
    hdrMetaDataType = static_cast<HDRMetaDataType>(parcel.ReadUint32());
    if (hdrMetaDataType == HDRMetaDataType::HDR_NOT_USED) {
        return GSERROR_OK;
    }
    if (ReadHDRMetaData(parcel, metaData) != GSERROR_OK) {
        return GSERROR_BINDER;
    }
    key = static_cast<GraphicHDRMetadataKey>(parcel.ReadUint32());
    if (ReadHDRMetaDataSet(parcel, metaDataSet) != GSERROR_OK) {
        return GSERROR_BINDER;
    }
    return GSERROR_OK;
}

GSError Transaction::WriteMetaDataToMessageParcel(MessageParcel& parcel, HDRMetaDataType hdrMetaDataType,
    const std::vector<GraphicHDRMetaData>& metaData, const GraphicHDRMetadataKey key,
    const std::vector<uint8_t>& metaDataSet)
{
    if (!parcel.WriteUint32(static_cast<uint32_t>(hdrMetaDataType))) {
        return GSERROR_BINDER;
    }
    if (hdrMetaDataType == HDRMetaDataType::HDR_NOT_USED) {
        return GSERROR_OK;
    }
    auto ret = WriteHDRMetaData(parcel, metaData);
    if (ret != GSERROR_OK) {
        return ret;
    }
    if (!parcel.WriteUint32(static_cast<uint32_t>(key))) {
        return GSERROR_BINDER;
    }
    ret = WriteHDRMetaDataSet(parcel, metaDataSet);
    if (ret != GSERROR_OK) {
        return ret;
    }
    return GSERROR_OK;
}

sptr<RSTransactionManager> RSTransactionManager::Create(
    uint64_t uniqueId, std::string name, uint32_t maxQueueSize,
    sptr<RSITransactionCallback> transactionCallback,
    sptr<IBufferConsumerListener> transactionListener)
{
    if (transactionCallback == nullptr || transactionListener == nullptr) {
        RS_LOGE("transactionCallback or transactionListener is nullptr");
        return nullptr;
    }
    return new RSTransactionManager(
        uniqueId, std::move(name), maxQueueSize, transactionCallback, transactionListener);
}

// 私有构造函数
RSTransactionManager::RSTransactionManager(uint64_t uniqueId, std::string name, uint32_t maxQueueSize,
    sptr<RSITransactionCallback> transactionCallback, sptr<IBufferConsumerListener> transactionListener)
    : mutex_(), uniqueId_(uniqueId), name_(std::move(name)), maxQueueSize_(maxQueueSize),
      transactionCallback_(transactionCallback), transactionListener_(transactionListener)
{
    pendingTransactionQueue_.reserve(maxQueueSize);
}

RSTransactionManager::~RSTransactionManager()
{
    RS_LOGD("~RSTransactionManager dtor, uniqueId: %{public}" PRIu64 ".", uniqueId_);
    for (auto& [id, _] : transactionQueueCache_) {
        OnBufferDeleteForRS(id);
    }
}

GSError RSTransactionManager::GoBackground()
{
    transactionCallback_->GoBackground(GetUniqueId());
    return GSERROR_OK;
}

GSError RSTransactionManager::QueueTransaction(const RSTransactionConfig& config)
{
    sptr<Transaction> transaction = nullptr;
    sptr<IBufferConsumerListener> listener = nullptr;
    {
        if (config.hasBuffer) {
            transaction = config.transaction;
        } else {
            auto iter = transactionQueueCache_.find(config.transactionInfo.bufferInfo.sequence);
            if (iter != transactionQueueCache_.end()) {
                iter->second->ReadFromTransactionInfo(config.transactionInfo);
                transaction = iter->second;
            }
        }
        if (!transaction || !transaction->GetBuffer() || !transaction->GetFence()) {
            RS_LOGE("RSTransactionManager::QueueTransaction invaild argument in config, hasBuffer: %{public}d, "
                "seq: %{public}u", config.hasBuffer, config.transactionInfo.bufferInfo.sequence);
            return GSERROR_INVALID_ARGUMENTS;
        }
        RS_TRACE_NAME_FMT("RSTransactionManager::QueueTransaction sequence: %u, fence:%d, queueId:%" PRId64
            ",queueSize:%" PRIu64 ".", transaction->GetBufferSeqNum(), transaction->GetFence()->Get(),
            uniqueId_, pendingTransactionQueue_.size());
    
        std::lock_guard<std::mutex> lock(mutex_);
        transactionQueueCache_[transaction->GetBufferSeqNum()] = transaction;
        for (auto& bufferSeqNum : config.delList) {
            transactionQueueCache_.erase(bufferSeqNum);
        }
        sptr<Transaction> transactionCopy = transaction;
        pendingTransactionQueue_.push_back(transactionCopy);
        listener = transactionListener_;
    }
    
    if (listener) {
        listener->OnBufferAvailable();
    }
    // if you need dump SurfaceBuffer to file, you should execute hdc shell param set persist.dumpbuffer.enabled 1
    // and reboot your device
    static bool dumpBufferEnabled = system::GetParameter("persist.dumpbuffer.enabled", "0") != "0";
    if (dumpBufferEnabled) {
        // Wait for the status of the fence to change to SIGNALED.
        transaction->GetFence()->Wait(DUMP_BUFFER_WAIT_FENCE_TIMEOUT_MS);
        sptr<SurfaceBuffer> buffer = transaction->GetBuffer();
        DumpToFileAsync(GetRealPid(), name_, buffer);
        RS_LOGW("RSTransactionManager::QueueTransaction dump buffer, queueSize:%{public}u, queueId: %{public}" PRIu64
            ", name: %{public}s", pendingTransactionQueue_.size(), uniqueId_, name_.c_str());
    }
    return GSERROR_OK;
}

GSError RSTransactionManager::AcquireBuffer(sptr<SurfaceBuffer>& buffer, sptr<SyncFence>& fence, int64_t& timestamp,
    std::vector<OHOS::Rect>& damages)
{
    sptr<Transaction> rsTransaction = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        rsTransaction = AcquireBufferLocked();
    }
    if (rsTransaction == nullptr) {
        return GSERROR_NO_BUFFER;
    }
    buffer = rsTransaction->GetBuffer();
    fence = rsTransaction->GetFence();
    timestamp = rsTransaction->GetUiTimestamp();
    damages = rsTransaction->GetDamages();
    return GSERROR_OK;
}

void RSTransactionManager::LogAndTraceAllBufferInPendingTransactionQueueLocked()
{
    for (auto& transaction : pendingTransactionQueue_) {
        RS_TRACE_NAME_FMT("acquire buffer id: %d desiredPresentTimestamp: %" PRId64
            " isAotuTimestamp: %d", transaction->GetBufferSeqNum(), transaction->GetDesiredPresentTimestamp(),
            transaction->GetAutoTimestamp());
        RS_LOGE("acquire buffer id: %{public}d desiredPresentTimestamp: %{public}" PRId64
            " isAotuTimestamp: %{public}d", transaction->GetBufferSeqNum(), transaction->GetDesiredPresentTimestamp(),
            transaction->GetAutoTimestamp());
    }
}

GSError RSTransactionManager::AcquireBuffer(AcquireBufferReturnValue& returnValue, int64_t expectPresentTimestamp,
                                            bool isUsingAutoTimestamp)
{
    RS_TRACE_NAME_FMT("AcquireBuffer with PresentTimestamp name: %s queueId: %" PRIu64 " queueSize: %u"
        "expectPresentTimestamp: %" PRId64, name_.c_str(), uniqueId_, maxQueueSize_, expectPresentTimestamp);
    if (expectPresentTimestamp <= 0) {
        return AcquireBuffer(returnValue.buffer, returnValue.fence, returnValue.timestamp, returnValue.damages);
    }
    std::vector<uint32_t> dropBuffers;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto iterTransaction = pendingTransactionQueue_.begin();
        if (iterTransaction == pendingTransactionQueue_.end()) {
            LogAndTraceAllBufferInPendingTransactionQueueLocked();
            return GSERROR_NO_BUFFER;
        }
        int64_t frontDesiredPresentTimestamp = (*iterTransaction)->GetDesiredPresentTimestamp();
        bool frontIsAutoTimestamp = (*iterTransaction)->GetAutoTimestamp();
        // If front is not auto-timestamp, and it’s just barely in the future (<1s ahead),
        // then buffer isn’t ready yet—log state and return “not ready” error.
        if (!frontIsAutoTimestamp && (frontDesiredPresentTimestamp > expectPresentTimestamp) &&
            (frontDesiredPresentTimestamp - ONE_SECOND_TIMESTAMP <= expectPresentTimestamp)) {
            LogAndTraceAllBufferInPendingTransactionQueueLocked();
            // Notify caller that no buffer is ready yet
            return GSERROR_NO_BUFFER_READY;
        }
        // Continue dropping the front buffer while both of the following are true:
        // 1) We are not in the special case where the front buffer uses a system (auto) timestamp
        //    but the consumer is only accepting producer (manual) timestamps.
        // 2) The front buffer’s desired present timestamp is already ≤ our expected timestamp
        while (!(frontIsAutoTimestamp && !isUsingAutoTimestamp) &&
            frontDesiredPresentTimestamp <= expectPresentTimestamp) {
            sptr<Transaction> frontTransaction = *iterTransaction;
            if (++iterTransaction == pendingTransactionQueue_.end()) {
                RS_LOGD("Buffer seq(%{public}d) is the last buffer, do acquire.", frontTransaction->GetBufferSeqNum());
                break;
            }
            int64_t curDesiredPresentTimestamp = (*iterTransaction)->GetDesiredPresentTimestamp();
            if (((*iterTransaction)->GetAutoTimestamp() && !isUsingAutoTimestamp) ||
                curDesiredPresentTimestamp > expectPresentTimestamp) {
                RS_LOGD("Next dirty buffer desiredPresentTimestamp: %{public}" PRId64
                    " not match expectPresentTimestamp"
                    ": %{public}" PRId64 ".",
                    frontDesiredPresentTimestamp, expectPresentTimestamp);
                break;
            }
            RS_TRACE_NAME_FMT("DropBuffer name: %s queueId: %" PRIu64 " ,buffer seq: %u , buffer "
                "desiredPresentTimestamp: %" PRId64 " acquire expectPresentTimestamp: %" PRId64,
                name_.c_str(), uniqueId_, (*iterTransaction)->GetBufferSeqNum(), frontDesiredPresentTimestamp,
                expectPresentTimestamp);
            dropBuffers.emplace_back(frontTransaction->GetBufferSeqNum());
            pendingTransactionQueue_.erase(pendingTransactionQueue_.begin());
            iterTransaction--;
            frontDesiredPresentTimestamp = curDesiredPresentTimestamp;
            frontIsAutoTimestamp = (*iterTransaction)->GetAutoTimestamp();
        }
        if (!frontIsAutoTimestamp &&
            !isBufferUtilPresentTimestampReady(frontDesiredPresentTimestamp, expectPresentTimestamp)) {
            LogAndTraceAllBufferInPendingTransactionQueueLocked();
            return GSERROR_NO_BUFFER_READY;
        }
    }
    ReleaseDropBuffers(dropBuffers);
    return AcquireBuffer(returnValue.buffer, returnValue.fence, returnValue.timestamp, returnValue.damages);
}

void RSTransactionManager::ReleaseDropBuffers(std::vector<uint32_t>& dropBuffers)
{
    if (dropBuffers.size() == 0) {
        return;
    }
    OnDropBuffersRet onDropBuffersRet;
    onDropBuffersRet.queueId = GetUniqueId();
    onDropBuffersRet.bufferSeqNums = dropBuffers;
    GSError ret = transactionCallback_->OnDropBuffers(onDropBuffersRet);
    if (ret != GSERROR_OK) {
        RS_LOGE("RSTransactionManager::ReleaseDropBuffers failed, ret:%{public}d", ret);
    }
}

GSError RSTransactionManager::SetDefaultWidthAndHeight(int32_t width, int32_t height)
{
    OnSetDefaultWidthAndHeightRet onSetDefaultWidthAndHeightRet;
    onSetDefaultWidthAndHeightRet.queueId = GetUniqueId();
    onSetDefaultWidthAndHeightRet.width = width;
    onSetDefaultWidthAndHeightRet.height = height;
    GSError ret = transactionCallback_->SetDefaultWidthAndHeight(onSetDefaultWidthAndHeightRet);
    if (ret != GSERROR_OK) {
        RS_LOGE("RSTransactionManager::SetDefaultWidthAndHeight failed, ret:%{public}d", ret);
    }
    return ret;
}

void RSTransactionManager::Dump(std::string& result) const
{
    RS_TRACE_NAME_FMT("RSTransactionManager::Dump");
    std::lock_guard<std::mutex> lock(mutex_);
    result += "RSTransactionManager [uniqueId_=" + std::to_string(uniqueId_) +
              ", name_=" + name_ +
              ", maxQueueSize_" + std::to_string(maxQueueSize_) +
              ", transform_=" + std::to_string(transform_) +
              ", sourceType_=" + std::to_string(sourceType_) +
              ", bufferSupportFastCompose_=" + std::to_string(bufferSupportFastCompose_) +
              ", rotatingBufferNumber_=" + std::to_string(rotatingBufferNumber_) +
              "]";
    for (auto& [id, transaction] : transactionQueueCache_) {
        result += "Transaction [bufferSeqNum_=" + std::to_string(transaction->GetBufferSeqNum()) + "]";
        result += "\n";
    }
    OnSurfaceDumpRet onSurfaceDumpRet;
    onSurfaceDumpRet.queueId = GetUniqueId();
    GSError ret = transactionCallback_->OnSurfaceDump(onSurfaceDumpRet);
    if (ret != GSERROR_OK) {
        RS_LOGE("RSTransactionManager::Dump failed, ret:%{public}d", ret);
    }
    result += onSurfaceDumpRet.dumpString;
}

sptr<Transaction> RSTransactionManager::AcquireBufferLocked()
{
    if (pendingTransactionQueue_.empty()) {
        return nullptr;
    }
    sptr<Transaction> transaction = pendingTransactionQueue_.front();
    pendingTransactionQueue_.erase(pendingTransactionQueue_.begin());
    transaction->SetlastAcquireTime(std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());
    RS_TRACE_NAME_FMT("AcquireBuffer success, buffer sequence: %u, desirePresentTimestamp: %" PRId64
        ", isAutoTimestamp: %d, queueId: %" PRId64 ", pendingQueueSize: %" PRIu64 ", name: %s.",
        transaction->GetBufferSeqNum(), transaction->GetDesiredPresentTimestamp(), transaction->GetAutoTimestamp(),
        uniqueId_, pendingTransactionQueue_.size(), name_.c_str());
    return transaction;
}

GSError RSTransactionManager::ReleaseBufferLocked(uint32_t bufferSeqNum, const sptr<SyncFence>& releaseFence)
{
    RS_TRACE_NAME_FMT("RSTransactionManager::ReleaseBufferLocked sequence:%u, uniqueId:%" PRId64 ".",
        bufferSeqNum, uniqueId_);
    auto mapIter = transactionQueueCache_.find(bufferSeqNum);
    if (mapIter != transactionQueueCache_.end()) {
        int64_t now = std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
        lastConsumeTime_ = now - mapIter->second->GetLastAcquireTime();
    }
    OnCompletedRet onCompletedRet = {
        .queueId = GetUniqueId(),
        .bufferSeqNum = bufferSeqNum,
        .releaseFence = releaseFence,
    };
    GSError ret = transactionCallback_->OnCompleted(onCompletedRet);
    if (ret != GSERROR_OK) {
        RS_LOGE("RSTransactionManager::ReleaseBufferLocked failed, ret:%{public}d, sequence:%{public}u,"
            "uniqueId:%{public}" PRIu64 ".", ret, bufferSeqNum, uniqueId_);
    }
    return ret;
}

GSError RSTransactionManager::ReleaseBuffer(sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& releaseFence)
{
    if (buffer == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto ret = ReleaseBufferLocked(buffer->GetSeqNum(), releaseFence);
        if (ret != GSERROR_OK) {
            return ret;
        }
    }
    OnDeleteBuffer(buffer->GetSeqNum(), true);
    return GSERROR_OK;
}

bool RSTransactionManager::QueryIfBufferAvailable()
{
    return !pendingTransactionQueue_.empty();
}

void RSTransactionManager::OnTunnelHandleChange(const sptr<SurfaceTunnelHandle>& handle)
{
    sptr<IBufferConsumerListener> listener = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        bool tunnelHandleChange = false;
        if (tunnelHandle_ == nullptr) {
            if (handle == nullptr) {
                RS_LOGW("tunnel handle is nullptr, uniqueId: %{public}" PRIu64 ".", uniqueId_);
                return;
            }
            tunnelHandleChange = true;
        } else {
            tunnelHandleChange = tunnelHandle_->Different(handle);
        }
        if (!tunnelHandleChange) {
            RS_LOGW("same tunnel handle, uniqueId: %{public}" PRIu64 ".", uniqueId_);
            return;
        }
        tunnelHandle_ = handle;
        listener = transactionListener_;
    }
    if (listener != nullptr) {
        RS_TRACE_NAME_FMT("OnTunnelHandleChange");
        listener->OnTunnelHandleChange();
    }
}

void RSTransactionManager::OnGoBackground()
{
    sptr<IBufferConsumerListener> listener = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        listener = transactionListener_;
    }
    if (listener != nullptr) {
        listener->OnGoBackground();
    }
}

void RSTransactionManager::OnCleanCache()
{
    sptr<IBufferConsumerListener> listener = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        listener = transactionListener_;
    }
    if (listener != nullptr) {
        listener->OnCleanCache();
    }
}

void RSTransactionManager::OnTransformChange()
{
    sptr<IBufferConsumerListener> listener = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        listener = transactionListener_;
    }
    if (listener != nullptr) {
        listener->OnTransformChange();
    }
}

void RSTransactionManager::OnDeleteBuffer(uint32_t sequence, bool isForUniRedraw)
{
    OnDeleteBufferFunc onDeleteBufferFunc = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (isForUniRedraw) {
            onDeleteBufferFunc = onBufferDeleteForRSHardwareThread_;
        } else {
            onDeleteBufferFunc = onBufferDeleteForRSMainThread_;
        }
    }
    if (onDeleteBufferFunc == nullptr) {
        return;
    }
    onDeleteBufferFunc(sequence);
}

GSError RSTransactionManager::RegisterConsumerListener(sptr<IBufferConsumerListener>& listener)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (listener == nullptr) {
        return GSERROR_INTERNAL;
    }
    transactionListener_ = listener;
    return GSERROR_OK;
}

GSError RSTransactionManager::RegisterDeleteBufferListener(OnDeleteBufferFunc func, bool isForUniRedraw)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (isForUniRedraw) {
        onBufferDeleteForRSHardwareThread_ = func;
    } else {
        onBufferDeleteForRSMainThread_ = func;
    }
    return GSERROR_OK;
}

GSError RSTransactionManager::QueryMetaDataType(uint32_t sequence, HDRMetaDataType& type) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto mapIter = transactionQueueCache_.find(sequence);
    if (mapIter == transactionQueueCache_.end()) {
        return GSERROR_NO_ENTRY;
    }
    type = mapIter->second->GetHdrMetaDataType();
    return GSERROR_OK;
}

GSError RSTransactionManager::GetMetaData(uint32_t sequence, std::vector<GraphicHDRMetaData>& metaData) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto mapIter = transactionQueueCache_.find(sequence);
    if (mapIter == transactionQueueCache_.end()) {
        return GSERROR_NO_ENTRY;
    }
    metaData.clear();
    metaData = mapIter->second->GetMetaData();
    return GSERROR_OK;
}

GSError RSTransactionManager::GetMetaDataSet(uint32_t sequence, GraphicHDRMetadataKey& key,
    std::vector<uint8_t>& metaDataSet) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto mapIter = transactionQueueCache_.find(sequence);
    if (mapIter == transactionQueueCache_.end()) {
        return GSERROR_NO_ENTRY;
    }
    metaDataSet.clear();
    key = mapIter->second->GetMetaDataKey();
    metaDataSet = mapIter->second->GetMetaDataSet();
    return GSERROR_OK;
}

GSError RSTransactionManager::UpdateTransactionConfig(const RSTransactionManagerConfig& config)
{
    std::lock_guard<std::mutex> lock(mutex_);
    maxQueueSize_ = config.bufferQueueSize;
    transform_ = config.transform;
    isBufferHold_ = config.isBufferHold;
    sourceType_ = config.sourceType;
    lastFlushedDesiredPresentTimeStamp_ = config.lastFlushedDesiredPresentTimeStamp;
    bufferSupportFastCompose_ = config.bufferSupportFastCompose;
    rotatingBufferNumber_ = config.rotatingBufferNumber;
    return GSERROR_OK;
}

GSError RSTransactionManager::GetLastFlushedDesiredPresentTimeStamp(int64_t& lastFlushedDesiredPresentTimeStamp) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    lastFlushedDesiredPresentTimeStamp = lastFlushedDesiredPresentTimeStamp_;
    return GSERROR_OK;
}

GSError RSTransactionManager::GetBufferSupportFastCompose(bool& bufferSupportFastCompose)
{
    std::lock_guard<std::mutex> lock(mutex_);
    bufferSupportFastCompose = bufferSupportFastCompose_;
    return GSERROR_OK;
}

GSError RSTransactionManager::GetCycleBuffersNumber(uint32_t& cycleBuffersNumber)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (rotatingBufferNumber_ == 0) {
        cycleBuffersNumber = maxQueueSize_;
    } else {
        cycleBuffersNumber = rotatingBufferNumber_;
    }
    return GSERROR_OK;
}

void RSTransactionManager::OnBufferDeleteForRS(uint32_t sequence)
{
    if (onBufferDeleteForRSMainThread_ != nullptr) {
        onBufferDeleteForRSMainThread_(sequence);
    }
    if (onBufferDeleteForRSHardwareThread_ != nullptr) {
        onBufferDeleteForRSHardwareThread_(sequence);
    }
}

void RSTransactionManager::DumpCurrentFrameLayer() const
{
    bool dumpStaticFrameEnabled = GetBoolParameter("debug.dumpstaticframe.enabled", "0");
    if (!dumpStaticFrameEnabled) {
        RS_LOGI("debug.dumpstaticframe.enabled not exist or not enable!");
        return;
    }

    std::lock_guard<std::mutex> lockGuard(mutex_);
    for (auto it = pendingTransactionQueue_.begin(); it != pendingTransactionQueue_.end(); it++) {
        if (*it != nullptr) {
            sptr<SurfaceBuffer> buffer = (*it)->GetBuffer();
            DumpToFileAsync(GetRealPid(), name_, buffer);
        }
    }
    RS_LOGD("RSTransactionManager::DumpCurrentFrameLayer dump %{public}d buffer",
        static_cast<uint32_t>(pendingTransactionQueue_.size()));
}

GSError RSTransactionManager::GetLastConsumeTime(int64_t& lastConsumeTime) const
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    lastConsumeTime = lastConsumeTime_;
    return GSERROR_OK;
}

GSError RSTransactionManager::GetSurfaceBufferTransformType(sptr<SurfaceBuffer> buffer,
    GraphicTransformType *transformType)
{
    if (buffer == nullptr || transformType == nullptr) {
        return SURFACE_ERROR_INVALID_PARAM;
    }
    *transformType = buffer->GetSurfaceBufferTransform();
    return GSERROR_OK;
}
} // namespace Rosen
} // namespace OHOS