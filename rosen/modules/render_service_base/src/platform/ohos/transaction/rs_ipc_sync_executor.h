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

#ifndef RENDER_SERVICE_BASE_RS_IPC_SYNC_EXECUTOR_H
#define RENDER_SERVICE_BASE_RS_IPC_SYNC_EXECUTOR_H

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <iremote_object.h>
#include <memory>
#include <message_option.h>
#include <message_parcel.h>
#include <mutex>
#include <refbase.h>

namespace OHOS {
namespace Rosen {

// Shared state of one offloaded synchronous IPC call. Both the business thread and the worker
// thread hold a shared_ptr to it, so either side may leave first safely. The parcels must be
// heap-held inside this context: after a timeout the worker may still be blocked inside
// SendRequest and accessing them, while the business thread's stack parcels are already gone.
struct SyncIpcContext {
    std::mutex mutex_;
    std::condition_variable cv_;
    bool done_ = false;      // worker finished SendRequest (no matter success or failure)
    bool discarded_ = false; // business thread gave up after timeout; worker must not write back
    int32_t result_ = 0;
    std::unique_ptr<MessageParcel> dataHolder_;
    std::unique_ptr<MessageParcel> replyHolder_;
};

// Offloads synchronous binder SendRequest calls to an FFRT worker and bounds the caller's
// waiting time with condition_variable::wait_for, so a frozen RS process returns an error
// to the client business thread instead of blocking it forever.
class RSSIpcSyncExecutor final {
public:
    static constexpr uint32_t DEFAULT_MAX_INFLIGHT_TIMEOUT = 16;

    static RSSIpcSyncExecutor& GetInstance();

    // Runs remote->SendRequest on a worker thread and waits at most timeoutMs on the calling
    // thread. Returns the SendRequest result code when the worker completes, or
    // RSInterfaceErrorCode::IPC_TIMEOUT_ERROR on timeout or when too many timed-out calls are
    // still in flight (fail-fast).
    // Caller contract: sync option only, and both parcels carry plain data only (no binder
    // objects or fds); callers are responsible for filtering such calls out beforehand.
    int32_t ExecuteSyncWithTimeout(const sptr<IRemoteObject>& remote, uint32_t code, MessageParcel& data,
        MessageParcel& reply, const MessageOption& option, uint32_t timeoutMs);

    void SetMaxInFlightTimeoutForTesting(uint32_t limit);
    uint32_t GetInFlightTimeoutCount() const;

private:
    RSSIpcSyncExecutor() = default;

    static void RunWorker(const sptr<IRemoteObject>& remote, uint32_t code, const std::shared_ptr<SyncIpcContext>& ctx,
        MessageOption option);
    static bool CloneReplyToCaller(const MessageParcel& replyHolder, MessageParcel& reply);

    std::atomic<uint32_t> inFlightTimeoutCount_ { 0 };
    std::atomic<uint32_t> maxInFlightTimeout_ { DEFAULT_MAX_INFLIGHT_TIMEOUT };
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_RS_IPC_SYNC_EXECUTOR_H
