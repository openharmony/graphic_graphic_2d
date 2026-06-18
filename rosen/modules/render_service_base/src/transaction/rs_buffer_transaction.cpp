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

#include "transaction/rs_buffer_transaction.h"

#include "buffer_utils.h"
#include "rs_trace.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSBufferTransaction::RSBufferTransaction(sptr<SurfaceBuffer>& buffer) : buffer_(buffer) {}

sptr<SurfaceBuffer> RSBufferTransaction::GetBuffer() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return buffer_;
}

sptr<SyncFence> RSBufferTransaction::GetFence() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return fence_;
}

void RSBufferTransaction::SetFence(sptr<SyncFence> fence)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    fence_ = fence;
}

const std::vector<OHOS::Rect>& RSBufferTransaction::GetDamages() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return damages_;
}

void RSBufferTransaction::SetDamages(std::vector<OHOS::Rect>& damages)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    damages_ = damages;
}

int64_t RSBufferTransaction::GetUiTimestamp() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return uiTimestamp_;
}

void RSBufferTransaction::SetUiTimestamp(int64_t uiTimestamp)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    uiTimestamp_ = uiTimestamp;
}

int64_t RSBufferTransaction::GetDesiredPresentTimestamp() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return desiredPresentTimestamp_;
}

void RSBufferTransaction::SetDesiredPresentTimestamp(int64_t desiredPresentTimestamp)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    desiredPresentTimestamp_ = desiredPresentTimestamp;
}

GSError RSBufferTransaction::WriteToMessageParcel(MessageParcel& parcel)
{
    if (buffer_ == nullptr || fence_ == nullptr) {
        RS_LOGE("RSBufferTransaction::WriteToMessageParcel buffer_ == nullptr:%{public}d, fence_ == nullptr:%{public}d",
            buffer_ == nullptr, fence_ == nullptr);
        return GSERROR_INTERNAL;
    }
    uint32_t size = damages_.size();
    if (size > SURFACE_PARCEL_SIZE_LIMIT) {
        RS_LOGE("RSBufferTransaction::WriteToMessageParcel damages size more than limit");
        return GSERROR_INVALID_ARGUMENTS;
    }
    if (!parcel.WriteUint32(size)) {
        RS_LOGE("RSBufferTransaction::WriteToMessageParcel !parcel.Write size");
        return GSERROR_BINDER;
    }
    for (const auto& rect : damages_) {
        if (!parcel.WriteInt32(rect.x) || !parcel.WriteInt32(rect.y) || !parcel.WriteInt32(rect.w) ||
            !parcel.WriteInt32(rect.h)) {
            RS_LOGE("RSBufferTransaction::WriteToMessageParcel !parcel.Write damages_");
            return GSERROR_BINDER;
        }
    }
    GSError ret = WriteSurfaceBufferImpl(parcel, buffer_->GetSeqNum(), buffer_);
    if (ret != GSERROR_OK) {
        RS_LOGE("RSBufferTransaction::WriteToMessageParcel WriteSurfaceBufferImpl failed, ret:%{public}d", ret);
        return ret;
    }
    if (!fence_->WriteToMessageParcel(parcel) || !parcel.WriteInt64(uiTimestamp_) ||
        !parcel.WriteInt64(desiredPresentTimestamp_)) {
        RS_LOGE("RSBufferTransaction !fence_->WriteToMessageParcel || !parcel.Write timestamp");
        return GSERROR_BINDER;
    }
    return GSERROR_OK;
}

sptr<RSBufferTransaction> RSBufferTransaction::ReadFromMessageParcel(MessageParcel& parcel)
{
    uint32_t size = parcel.ReadUint32();
    if (size > SURFACE_PARCEL_SIZE_LIMIT) {
        RS_LOGE("RSBufferTransaction::ReadFromMessageParcel more than limit");
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
        RS_LOGE("RSBufferTransaction ReadSurfaceBufferImpl ret:%{public}d", ret);
        buffer = nullptr;
        return nullptr;
    }
    sptr<SyncFence> fence = SyncFence::ReadFromMessageParcel(parcel);
    int64_t uiTimestamp = 0;
    int64_t desiredPresentTimestamp = 0;
    if (!parcel.ReadInt64(uiTimestamp) || !parcel.ReadInt64(desiredPresentTimestamp)) {
        buffer = nullptr;
        return nullptr;
    }
    auto transaction = new RSBufferTransaction(buffer);
    transaction->SetFence(fence);
    transaction->SetDamages(damages);
    transaction->SetUiTimestamp(uiTimestamp);
    transaction->SetDesiredPresentTimestamp(desiredPresentTimestamp);
    return transaction;
}

GSError RSBufferTransaction::ReadTransactionConfigFromMessageParcel(MessageParcel& parcel, RSTransactionConfig& config)
{
    if (!parcel.ReadUint64(config.nodeId)) {
        return GSERROR_BINDER;
    }
    config.transaction = RSBufferTransaction::ReadFromMessageParcel(parcel);
    if (config.transaction == nullptr) {
        return GSERROR_BINDER;
    }
    return GSERROR_OK;
}

GSError RSBufferTransaction::WriteTransactionConfigToMessageParcel(
    MessageParcel& parcel, const RSTransactionConfig& config)
{
    if (!parcel.WriteUint64(config.nodeId)) {
        ROSEN_LOGE("QueueTransaction: WriteUint64 id err.");
        return GSERROR_BINDER;
    }
    if (config.transaction == nullptr || config.transaction->WriteToMessageParcel(parcel) != GSERROR_OK) {
        ROSEN_LOGE("QueueTransaction: Write buffer err.");
        return GSERROR_BINDER;
    }
    return GSERROR_OK;
}
} // namespace Rosen
} // namespace OHOS