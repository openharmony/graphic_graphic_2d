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

#include "platform/ohos/transaction/rs_ipc_sync_executor.h"

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <ffrt.h>

#include "securec.h"

#include "common/rs_common_def.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {

// Upper bound for cloning a parcel's raw bytes, aligned with the IPC layer's raw data cap
// (MessageParcel::MAX_RAWDATA_SIZE, which is private). Sizes above this are treated as
// corrupted and rejected before allocation.
constexpr size_t MAX_CLONE_PARCEL_SIZE = 128 * 1024 * 1024; // 128M

// Deep-copies the raw bytes of a plain-data parcel into a heap-held parcel.
// Caller contract: src must not contain binder objects or fds (no object offsets);
// such calls are filtered out by the proxy wrappers before reaching the executor.
std::unique_ptr<MessageParcel> CloneDataParcel(const MessageParcel& src)
{
    auto holder = std::make_unique<MessageParcel>();
    size_t size = src.GetDataSize();
    if (size == 0) {
        return holder;
    }
    if (size > MAX_CLONE_PARCEL_SIZE) {
        ROSEN_LOGE("RSSIpcSyncExecutor: invalid data parcel size %{public}zu, refuse to clone", size);
        return nullptr;
    }
    void* buffer = malloc(size);
    if (buffer == nullptr) {
        return nullptr;
    }
    errno_t err = memcpy_s(buffer, size, reinterpret_cast<const void*>(src.GetData()), size);
    if (err != EOK) {
        free(buffer);
        return nullptr;
    }
    // ParseFrom takes ownership of buffer; the parcel frees it on destruction.
    if (!holder->ParseFrom(reinterpret_cast<uintptr_t>(buffer), size)) {
        free(buffer);
        return nullptr;
    }
    return holder;
}
} // namespace

RSSIpcSyncExecutor& RSSIpcSyncExecutor::GetInstance()
{
    static RSSIpcSyncExecutor instance;
    return instance;
}

void RSSIpcSyncExecutor::RunWorker(
    const sptr<IRemoteObject>& remote, uint32_t code, const std::shared_ptr<SyncIpcContext>& ctx, MessageOption option)
{
    int32_t ret = remote->SendRequest(code, *ctx->dataHolder_, *ctx->replyHolder_, option);
    {
        std::lock_guard<std::mutex> lock(ctx->mutex_);
        if (ctx->discarded_) {
            // The business thread already returned on timeout; nothing may be written back.
            GetInstance().inFlightTimeoutCount_.fetch_sub(1, std::memory_order_relaxed);
            return;
        }
        ctx->result_ = ret;
        ctx->done_ = true;
    }
    ctx->cv_.notify_one();
}

int32_t RSSIpcSyncExecutor::ExecuteSyncWithTimeout(const sptr<IRemoteObject>& remote, uint32_t code,
    MessageParcel& data, MessageParcel& reply, const MessageOption& option, uint32_t timeoutMs)
{
    if (inFlightTimeoutCount_.load(std::memory_order_relaxed) >= maxInFlightTimeout_.load(std::memory_order_relaxed)) {
        ROSEN_LOGE("RSSIpcSyncExecutor: too many in-flight timeouts, reject sync IPC code %{public}u", code);
        return static_cast<int32_t>(RSInterfaceErrorCode::IPC_TIMEOUT_ERROR);
    }
    auto ctx = std::make_shared<SyncIpcContext>();
    ctx->dataHolder_ = CloneDataParcel(data);
    ctx->replyHolder_ = std::make_unique<MessageParcel>();
    if (ctx->dataHolder_ == nullptr) {
        ROSEN_LOGE("RSSIpcSyncExecutor: clone data parcel failed, code %{public}u", code);
        return static_cast<int32_t>(RSInterfaceErrorCode::UNKNOWN_ERROR);
    }
    ffrt::submit([remote, code, ctx, option]() { RunWorker(remote, code, ctx, option); });

    std::unique_lock<std::mutex> lock(ctx->mutex_);
    if (!ctx->cv_.wait_for(lock, std::chrono::milliseconds(timeoutMs), [&ctx]() { return ctx->done_; })) {
        ctx->discarded_ = true;
        inFlightTimeoutCount_.fetch_add(1, std::memory_order_relaxed);
        ROSEN_LOGE("RSSIpcSyncExecutor: sync IPC timeout, code %{public}u, timeout %{public}u ms", code, timeoutMs);
        return static_cast<int32_t>(RSInterfaceErrorCode::IPC_TIMEOUT_ERROR);
    }
    int32_t result = ctx->result_;
    lock.unlock();
    if (result != NO_ERROR) {
        return result;
    }
    if (ctx->replyHolder_->GetOffsetsSize() > 0) {
        // Defensive guard: replies carrying binder objects/fds cannot be cloned back safely;
        // the proxy wrappers are expected to keep such interfaces on the direct path.
        ROSEN_LOGE("RSSIpcSyncExecutor: reply of code %{public}u carries objects, discard", code);
        return static_cast<int32_t>(RSInterfaceErrorCode::UNKNOWN_ERROR);
    }
    if (!CloneReplyToCaller(*ctx->replyHolder_, reply)) {
        ROSEN_LOGE("RSSIpcSyncExecutor: clone reply parcel failed, code %{public}u", code);
        return static_cast<int32_t>(RSInterfaceErrorCode::UNKNOWN_ERROR);
    }
    return NO_ERROR;
}

bool RSSIpcSyncExecutor::CloneReplyToCaller(const MessageParcel& replyHolder, MessageParcel& reply)
{
    size_t size = replyHolder.GetDataSize();
    if (size == 0) {
        return true;
    }
    if (size > MAX_CLONE_PARCEL_SIZE) {
        ROSEN_LOGE("RSSIpcSyncExecutor: invalid reply parcel size %{public}zu, refuse to clone", size);
        return false;
    }
    void* buffer = malloc(size);
    if (buffer == nullptr) {
        return false;
    }
    errno_t err = memcpy_s(buffer, size, reinterpret_cast<const void*>(replyHolder.GetData()), size);
    if (err != EOK) {
        free(buffer);
        return false;
    }
    // ParseFrom takes ownership of buffer; the caller's reply parcel frees it on destruction.
    if (!reply.ParseFrom(reinterpret_cast<uintptr_t>(buffer), size)) {
        free(buffer);
        return false;
    }
    return true;
}

void RSSIpcSyncExecutor::SetMaxInFlightTimeoutForTesting(uint32_t limit)
{
    maxInFlightTimeout_.store(limit, std::memory_order_relaxed);
}

uint32_t RSSIpcSyncExecutor::GetInFlightTimeoutCount() const
{
    return inFlightTimeoutCount_.load(std::memory_order_relaxed);
}
} // namespace Rosen
} // namespace OHOS
