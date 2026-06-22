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

#include "transaction/transaction.h"
#include "transaction/rs_marshalling_helper.h"

#include <string>

#include "buffer_utils.h"
#include "parameters.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"
#include "sandbox_utils.h"
#include "surface_buffer_impl.h"
#include "surface_utils.h"
#include "pipeline/rs_surface_render_node.h"
#include "feature/delegate_composite/rs_delegate_composite_callback_manager.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t DUMP_BUFFER_WAIT_FENCE_TIMEOUT_MS = 3000;
}

GSError Transaction::WriteToMessageParcel(MessageParcel& parcel, bool hasBuffer) const
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

    GSError ret = GSERROR_OK;
    if (hasBuffer) {
        ret = WriteSurfaceBufferImpl(parcel, buffer_->GetSeqNum(), buffer_);
    } else {
        ret = WriteSurfaceBufferImpl(parcel, buffer_->GetSeqNum(), nullptr);
    }

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
    if (parcel.ReadBool()) {
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

sptr<SurfaceBuffer> Transaction::ReadBufferFromMessageParcel(MessageParcel& parcel)
{
    uint32_t sequence = 0;
    sptr<SurfaceBuffer> buffer = nullptr;
    GSError ret = ReadSurfaceBufferImpl(parcel, sequence, buffer);
    if (ret != GSERROR_OK || buffer == nullptr) {
        RS_LOGE("Transaction ReadSurfaceBufferImpl ret:%{public}d", ret);
        return nullptr;
    }
    ret = buffer->ReadBufferProperty(parcel);
    if (ret != GSERROR_OK) {
        RS_LOGE("Transaction ReadBufferProperty ret:%{public}d", ret);
        return nullptr;
    }
    return buffer;
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
        OHOS::Rect rect = { .x = parcel.ReadInt32(), .y = parcel.ReadInt32(),
            .w = parcel.ReadInt32(), .h = parcel.ReadInt32(), };
        damages.emplace_back(rect);
    }
    sptr<SurfaceBuffer> buffer = ReadBufferFromMessageParcel(parcel);
    if (!buffer) {
        return nullptr;
    }
    sptr<SyncFence> fence = SyncFence::ReadFromMessageParcel(parcel);
    int64_t uiTimestamp = 0;
    int64_t desiredPresentTimestamp = 0;
    bool isAutoTimestamp = false;
    if (!parcel.ReadInt64(uiTimestamp) || !parcel.ReadInt64(desiredPresentTimestamp) ||
        !parcel.ReadBool(isAutoTimestamp)) {
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
    GSError ret = ReadMetaDataFromMessageParcel(parcel, hdrMetaDataType, metaData, metaDataKey, metaDataSet);
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
    const std::vector<uint8_t>& metaDataSet) const
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

GSError Transaction::ReadTransactionConfigFromMessageParcel(MessageParcel& parcel, RSTransactionConfig& config)
{
    if (!parcel.ReadUint64(config.nodeId)) {
        return GSERROR_BINDER;
    }
    if (!parcel.ReadBool(config.hasBuffer)) {
        return GSERROR_BINDER;
    }
    if (config.hasBuffer) {
        config.transaction = Transaction::ReadFromMessageParcel(parcel);
        if (config.transaction == nullptr) {
            return GSERROR_BINDER;
        }
    } else {
        if (Transaction::ReadTransactionInfoFromMessageParcel(parcel, config.transactionInfo) != GSERROR_OK) {
            return GSERROR_BINDER;
        }
    }
    std::vector<uint32_t> delList;
    uint32_t size = 0;
    if (!parcel.ReadUint32(size)) {
        return GSERROR_BINDER;
    }
    for (uint32_t i = 0; i < size; ++i) {
        uint32_t value = 0;
        if (!parcel.ReadUint32(value)) {
            return GSERROR_BINDER;
        }
        config.delList.push_back(value);
    }
    if (!parcel.ReadInt64(config.requestTimeNs) || !parcel.ReadInt64(config.flushTimeNs) ||
        !parcel.ReadInt32(config.pid)) {
        return GSERROR_BINDER;
    }
    return GSERROR_OK;
}

GSError Transaction::WriteTransactionConfigToMessageParcel(MessageParcel& parcel, const RSTransactionConfig& config)
{
    if (!parcel.WriteUint64(config.nodeId)) {
        return GSERROR_BINDER;
    }
    if (!parcel.WriteBool(config.hasBuffer)) {
        return GSERROR_BINDER;
    }
    if (config.transaction == nullptr ||
        config.transaction->WriteToMessageParcel(parcel, config.hasBuffer) != GSERROR_OK) {
        return GSERROR_BINDER;
    }
    uint32_t delListSize = config.delList.size();
    if (!parcel.WriteUint32(delListSize)) {
        return GSERROR_BINDER;
    }
    for (uint32_t i = 0; i < delListSize; i++) {
        if (!parcel.WriteUint32(config.delList[i])) {
            return GSERROR_BINDER;
        }
    }
    if (!parcel.WriteInt64(config.requestTimeNs) || !parcel.WriteInt64(config.flushTimeNs)) {
        return GSERROR_BINDER;
    }
    if (!parcel.WriteInt32(config.pid)) {
        return GSERROR_BINDER;
    }
    return GSERROR_OK;
}

Transaction::Transaction(const sptr<SurfaceBuffer>& buffer) : buffer_(buffer)
{
    if (buffer) {
        bufferSeqNum_ = buffer->GetSeqNum();
    }
}

sptr<SurfaceBuffer> Transaction::GetBuffer() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return buffer_;
}

uint32_t Transaction::GetBufferSeqNum() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return bufferSeqNum_;
}

sptr<SyncFence> Transaction::GetFence() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return fence_;
}

void Transaction::SetFence(const sptr<SyncFence>& fence)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    fence_ = fence;
}

const std::vector<OHOS::Rect>& Transaction::GetDamages() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return damages_;
}

void Transaction::SetDamages(const std::vector<OHOS::Rect>& damages)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    damages_ = damages;
}

int64_t Transaction::GetUiTimestamp() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return uiTimestamp_;
}

void Transaction::SetUiTimestamp(int64_t uiTimestamp)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    uiTimestamp_ = uiTimestamp;
}

int64_t Transaction::GetDesiredPresentTimestamp() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return desiredPresentTimestamp_;
}

void Transaction::SetDesiredPresentTimestamp(int64_t desiredPresentTimestamp)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    desiredPresentTimestamp_ = desiredPresentTimestamp;
}

bool Transaction::GetAutoTimestamp() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return isAutoTimestamp_;
}

void Transaction::SetAutoTimestamp(bool isAutoTimestamp)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    isAutoTimestamp_ = isAutoTimestamp;
}

HDRMetaDataType Transaction::GetHdrMetaDataType() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return hdrMetaDataType_;
}

const std::vector<GraphicHDRMetaData>& Transaction::GetMetaData() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return metaData_;
}

const std::vector<uint8_t>& Transaction::GetMetaDataSet() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return metaDataSet_;
}

GraphicHDRMetadataKey Transaction::GetMetaDataKey() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return metaDataKey_;
}

void Transaction::SetHdrMetaDataType(HDRMetaDataType type)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    hdrMetaDataType_ = type;
}

void Transaction::SetMetaData(const std::vector<GraphicHDRMetaData>& metaData)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    metaData_ = metaData;
}

void Transaction::SetMetaDataSet(const std::vector<uint8_t>& metaDataSet)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    metaDataSet_ = metaDataSet;
}

void Transaction::SetMetaDataKey(GraphicHDRMetadataKey key)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    metaDataKey_ = key;
}

void Transaction::SetLastAcquireTime(int64_t lastAcquireTime)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    lastAcquireTime_ = lastAcquireTime;
}

int64_t Transaction::GetLastAcquireTime() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return lastAcquireTime_;
}
} // namespace Rosen
} // namespace OHOS