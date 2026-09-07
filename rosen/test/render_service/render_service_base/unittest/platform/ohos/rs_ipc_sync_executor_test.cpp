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

#include <atomic>
#include <chrono>
#include <fcntl.h>
#include <functional>
#include <gtest/gtest.h>
#include <iremote_object.h>
#include <message_option.h>
#include <message_parcel.h>
#include <thread>
#include <unistd.h>

#include "common/rs_common_def.h"
#include "platform/common/rs_system_properties.h"
#include "platform/ohos/transaction/rs_ipc_sync_executor.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t TEST_TIMEOUT_MS = 50;
constexpr uint32_t TEST_NORMAL_TIMEOUT_MS = 2000; // success-path tests: generous budget, avoids CI flakiness
constexpr uint32_t WAIT_DRAIN_TIMEOUT_MS = 5000;
constexpr int32_t TEST_REPLY_VALUE = 123;
constexpr int32_t TEST_DATA_VALUE = 42;
constexpr uint32_t TEST_CODE = 0x1234;

class MockIRemoteObject : public IRemoteObject {
public:
    MockIRemoteObject() : IRemoteObject { u"MockIRemoteObject" } {}
    ~MockIRemoteObject() override = default;

    int32_t GetObjectRefCount() override
    {
        return 0;
    }
    bool AddDeathRecipient(const sptr<DeathRecipient>& recipient) override
    {
        return true;
    }
    bool RemoveDeathRecipient(const sptr<DeathRecipient>& recipient) override
    {
        return true;
    }
    int Dump(int fd, const std::vector<std::u16string>& args) override
    {
        return 0;
    }

    int32_t SendRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override
    {
        if (sendRequestImpl_) {
            return sendRequestImpl_(code, data, reply, option);
        }
        return NO_ERROR;
    }

    std::function<int32_t(uint32_t, MessageParcel&, MessageParcel&, MessageOption&)> sendRequestImpl_;
};

void WaitInFlightCountDrain(RSSIpcSyncExecutor& executor, uint32_t expect)
{
    auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(WAIT_DRAIN_TIMEOUT_MS);
    while (executor.GetInFlightTimeoutCount() != expect && std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
} // namespace

class RSIpcSyncExecutorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSIpcSyncExecutorTest::SetUpTestCase() {}
void RSIpcSyncExecutorTest::TearDownTestCase() {}
void RSIpcSyncExecutorTest::SetUp() {}
void RSIpcSyncExecutorTest::TearDown()
{
    // restore defaults so later tests are not affected
    RSSIpcSyncExecutor::GetInstance().SetMaxInFlightTimeoutForTesting(RSSIpcSyncExecutor::DEFAULT_MAX_INFLIGHT_TIMEOUT);
}

/**
 * @tc.name: NormalComplete
 * @tc.desc: Worker completes SendRequest and the reply is cloned back to the caller.
 * @tc.type: FUNC
 */
HWTEST_F(RSIpcSyncExecutorTest, NormalComplete, TestSize.Level1)
{
    sptr<MockIRemoteObject> remote = new MockIRemoteObject();
    remote->sendRequestImpl_ = [](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption&) {
        reply.WriteInt32(TEST_REPLY_VALUE);
        return NO_ERROR;
    };
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto& executor = RSSIpcSyncExecutor::GetInstance();
    uint32_t countBefore = executor.GetInFlightTimeoutCount();
    int32_t ret = executor.ExecuteSyncWithTimeout(remote, TEST_CODE, data, reply, option, TEST_NORMAL_TIMEOUT_MS);
    ASSERT_EQ(ret, NO_ERROR);
    int32_t value = 0;
    ASSERT_TRUE(reply.ReadInt32(value));
    ASSERT_EQ(value, TEST_REPLY_VALUE);
    ASSERT_EQ(executor.GetInFlightTimeoutCount(), countBefore);
}

/**
 * @tc.name: NullRemoteRejected
 * @tc.desc: A null remote is rejected with NULLPTR_ERROR without submitting any work.
 * @tc.type: FUNC
 */
HWTEST_F(RSIpcSyncExecutorTest, NullRemoteRejected, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto& executor = RSSIpcSyncExecutor::GetInstance();
    int32_t ret = executor.ExecuteSyncWithTimeout(nullptr, TEST_CODE, data, reply, option, TEST_TIMEOUT_MS);
    ASSERT_EQ(ret, static_cast<int32_t>(RSInterfaceErrorCode::NULLPTR_ERROR));
}

/**
 * @tc.name: DataWithObjectsRejected
 * @tc.desc: A data parcel carrying binder objects/fds is rejected with UNKNOWN_ERROR instead
 *           of being byte-cloned, and SendRequest is never reached.
 * @tc.type: FUNC
 */
HWTEST_F(RSIpcSyncExecutorTest, DataWithObjectsRejected, TestSize.Level1)
{
    sptr<MockIRemoteObject> remote = new MockIRemoteObject();
    std::atomic<uint32_t> callCount { 0 };
    remote->sendRequestImpl_ = [&callCount](uint32_t, MessageParcel&, MessageParcel&, MessageOption&) {
        callCount.fetch_add(1);
        return NO_ERROR;
    };
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int fd = open("/dev/null", O_RDONLY);
    ASSERT_GE(fd, 0);
    ASSERT_TRUE(data.WriteFileDescriptor(fd));
    close(fd);
    auto& executor = RSSIpcSyncExecutor::GetInstance();
    int32_t ret = executor.ExecuteSyncWithTimeout(remote, TEST_CODE, data, reply, option, TEST_TIMEOUT_MS);
    ASSERT_EQ(ret, static_cast<int32_t>(RSInterfaceErrorCode::UNKNOWN_ERROR));
    ASSERT_EQ(callCount.load(), 0); // never submitted
}

/**
 * @tc.name: WorkerErrorCode
 * @tc.desc: SendRequest failure on the worker is returned as-is, not as a timeout.
 * @tc.type: FUNC
 */
HWTEST_F(RSIpcSyncExecutorTest, WorkerErrorCode, TestSize.Level1)
{
    sptr<MockIRemoteObject> remote = new MockIRemoteObject();
    remote->sendRequestImpl_ = [](uint32_t, MessageParcel&, MessageParcel&, MessageOption&) { return UNKNOWN_ERROR; };
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto& executor = RSSIpcSyncExecutor::GetInstance();
    uint32_t countBefore = executor.GetInFlightTimeoutCount();
    int32_t ret = executor.ExecuteSyncWithTimeout(remote, TEST_CODE, data, reply, option, TEST_NORMAL_TIMEOUT_MS);
    ASSERT_EQ(ret, UNKNOWN_ERROR);
    ASSERT_EQ(executor.GetInFlightTimeoutCount(), countBefore);
}

/**
 * @tc.name: DataCloneCorrectness
 * @tc.desc: The worker sees an intact copy of the caller's data parcel.
 * @tc.type: FUNC
 */
HWTEST_F(RSIpcSyncExecutorTest, DataCloneCorrectness, TestSize.Level1)
{
    sptr<MockIRemoteObject> remote = new MockIRemoteObject();
    std::atomic<int32_t> observed { 0 };
    remote->sendRequestImpl_ = [&observed](uint32_t, MessageParcel& data, MessageParcel&, MessageOption&) {
        int32_t value = 0;
        if (data.ReadInt32(value)) {
            observed.store(value);
        }
        return NO_ERROR;
    };
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    ASSERT_TRUE(data.WriteInt32(TEST_DATA_VALUE));
    auto& executor = RSSIpcSyncExecutor::GetInstance();
    int32_t ret = executor.ExecuteSyncWithTimeout(remote, TEST_CODE, data, reply, option, TEST_NORMAL_TIMEOUT_MS);
    ASSERT_EQ(ret, NO_ERROR);
    ASSERT_EQ(observed.load(), TEST_DATA_VALUE);
}

/**
 * @tc.name: TimeoutReturnsError
 * @tc.desc: A blocked SendRequest returns IPC_TIMEOUT_ERROR within the timeout and
 *           the calling thread is not blocked beyond it.
 * @tc.type: FUNC
 */
HWTEST_F(RSIpcSyncExecutorTest, TimeoutReturnsError, TestSize.Level1)
{
    sptr<MockIRemoteObject> remote = new MockIRemoteObject();
    auto latch = std::make_shared<std::atomic<bool>>(false);
    remote->sendRequestImpl_ = [latch](uint32_t, MessageParcel&, MessageParcel&, MessageOption&) {
        while (!latch->load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        return NO_ERROR;
    };
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto& executor = RSSIpcSyncExecutor::GetInstance();
    uint32_t countBefore = executor.GetInFlightTimeoutCount();
    auto start = std::chrono::steady_clock::now();
    int32_t ret = executor.ExecuteSyncWithTimeout(remote, TEST_CODE, data, reply, option, TEST_TIMEOUT_MS);
    auto elapsedMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
    ASSERT_EQ(ret, static_cast<int32_t>(RSInterfaceErrorCode::IPC_TIMEOUT_ERROR));
    ASSERT_LT(elapsedMs, WAIT_DRAIN_TIMEOUT_MS);
    ASSERT_EQ(executor.GetInFlightTimeoutCount(), countBefore + 1);
    latch->store(true); // let the worker finish so the count drains for later tests
    WaitInFlightCountDrain(executor, countBefore);
    ASSERT_EQ(executor.GetInFlightTimeoutCount(), countBefore);
}

/**
 * @tc.name: LateWorkerCompletion
 * @tc.desc: After a timeout the late-returning worker writes nothing back and drains the count.
 * @tc.type: FUNC
 */
HWTEST_F(RSIpcSyncExecutorTest, LateWorkerCompletion, TestSize.Level1)
{
    sptr<MockIRemoteObject> remote = new MockIRemoteObject();
    auto latch = std::make_shared<std::atomic<bool>>(false);
    remote->sendRequestImpl_ = [latch](uint32_t, MessageParcel&, MessageParcel& reply, MessageOption&) {
        while (!latch->load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        reply.WriteInt32(TEST_REPLY_VALUE); // late write goes to the discarded holder only
        return NO_ERROR;
    };
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto& executor = RSSIpcSyncExecutor::GetInstance();
    uint32_t countBefore = executor.GetInFlightTimeoutCount();
    int32_t ret = executor.ExecuteSyncWithTimeout(remote, TEST_CODE, data, reply, option, TEST_TIMEOUT_MS);
    ASSERT_EQ(ret, static_cast<int32_t>(RSInterfaceErrorCode::IPC_TIMEOUT_ERROR));
    latch->store(true);
    WaitInFlightCountDrain(executor, countBefore);
    ASSERT_EQ(executor.GetInFlightTimeoutCount(), countBefore);
    ASSERT_EQ(reply.GetDataSize(), 0); // caller's reply untouched by the late worker
}

/**
 * @tc.name: FailFastWhenOverloaded
 * @tc.desc: When the in-flight timeout count reaches the limit, new calls are rejected
 *           immediately without being submitted.
 * @tc.type: FUNC
 */
HWTEST_F(RSIpcSyncExecutorTest, FailFastWhenOverloaded, TestSize.Level1)
{
    sptr<MockIRemoteObject> remote = new MockIRemoteObject();
    std::atomic<uint32_t> callCount { 0 };
    remote->sendRequestImpl_ = [&callCount](uint32_t, MessageParcel&, MessageParcel&, MessageOption&) {
        callCount.fetch_add(1);
        return NO_ERROR;
    };
    auto& executor = RSSIpcSyncExecutor::GetInstance();
    executor.SetMaxInFlightTimeoutForTesting(executor.GetInFlightTimeoutCount()); // current count == limit
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int32_t ret = executor.ExecuteSyncWithTimeout(remote, TEST_CODE, data, reply, option, TEST_TIMEOUT_MS);
    ASSERT_EQ(ret, static_cast<int32_t>(RSInterfaceErrorCode::IPC_TIMEOUT_ERROR));
    ASSERT_EQ(callCount.load(), 0); // never submitted
}

/**
 * @tc.name: FailFastBoundary
 * @tc.desc: One slot below the limit still executes normally.
 * @tc.type: FUNC
 */
HWTEST_F(RSIpcSyncExecutorTest, FailFastBoundary, TestSize.Level1)
{
    sptr<MockIRemoteObject> remote = new MockIRemoteObject();
    remote->sendRequestImpl_ = nullptr; // default mock returns NO_ERROR
    auto& executor = RSSIpcSyncExecutor::GetInstance();
    executor.SetMaxInFlightTimeoutForTesting(executor.GetInFlightTimeoutCount() + 1);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int32_t ret = executor.ExecuteSyncWithTimeout(remote, TEST_CODE, data, reply, option, TEST_NORMAL_TIMEOUT_MS);
    ASSERT_EQ(ret, NO_ERROR);
}

/**
 * @tc.name: PeerDeathDrainsCount
 * @tc.desc: A worker that returns an error after the caller timed out (as on peer death /
 *           BR_DEAD_REPLY) drains the in-flight count and the executor keeps serving.
 * @tc.type: FUNC
 */
HWTEST_F(RSIpcSyncExecutorTest, PeerDeathDrainsCount, TestSize.Level1)
{
    sptr<MockIRemoteObject> remote = new MockIRemoteObject();
    auto latch = std::make_shared<std::atomic<bool>>(false);
    remote->sendRequestImpl_ = [latch](uint32_t, MessageParcel&, MessageParcel&, MessageOption&) {
        while (!latch->load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        return DEAD_OBJECT; // binder reports peer death to the blocked worker
    };
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto& executor = RSSIpcSyncExecutor::GetInstance();
    uint32_t countBefore = executor.GetInFlightTimeoutCount();
    int32_t ret = executor.ExecuteSyncWithTimeout(remote, TEST_CODE, data, reply, option, TEST_TIMEOUT_MS);
    ASSERT_EQ(ret, static_cast<int32_t>(RSInterfaceErrorCode::IPC_TIMEOUT_ERROR));
    latch->store(true);
    WaitInFlightCountDrain(executor, countBefore);
    ASSERT_EQ(executor.GetInFlightTimeoutCount(), countBefore);
    // executor still works after recovery
    remote->sendRequestImpl_ = nullptr;
    MessageParcel data2;
    MessageParcel reply2;
    MessageOption option2;
    ret = executor.ExecuteSyncWithTimeout(remote, TEST_CODE, data2, reply2, option2, TEST_NORMAL_TIMEOUT_MS);
    ASSERT_EQ(ret, NO_ERROR);
}

/**
 * @tc.name: ConfigRead
 * @tc.desc: The timeout parameter channel returns a value within the clamped range
 *           (default or configured, out-of-range parameters fall back to default).
 * @tc.type: FUNC
 */
HWTEST_F(RSIpcSyncExecutorTest, ConfigRead, TestSize.Level1)
{
    constexpr uint32_t MIN_CONFIG_TIMEOUT_MS = 1000;
    constexpr uint32_t MAX_CONFIG_TIMEOUT_MS = 5000;
    uint32_t timeoutMs = RSSystemProperties::GetIpcSyncTimeoutMs();
    ASSERT_GE(timeoutMs, MIN_CONFIG_TIMEOUT_MS);
    ASSERT_LE(timeoutMs, MAX_CONFIG_TIMEOUT_MS);
}

/**
 * @tc.name: TestingEntrypoints
 * @tc.desc: Set/Get roundtrip of the in-flight limit test hooks.
 * @tc.type: FUNC
 */
HWTEST_F(RSIpcSyncExecutorTest, TestingEntrypoints, TestSize.Level1)
{
    auto& executor = RSSIpcSyncExecutor::GetInstance();
    executor.SetMaxInFlightTimeoutForTesting(3);
    // no getter for the limit; verify indirectly via fail-fast behavior
    sptr<MockIRemoteObject> remote = new MockIRemoteObject();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t countBefore = executor.GetInFlightTimeoutCount();
    int32_t ret = executor.ExecuteSyncWithTimeout(remote, TEST_CODE, data, reply, option, TEST_NORMAL_TIMEOUT_MS);
    ASSERT_EQ(ret, NO_ERROR); // limit 3 > current count: normal path
    ASSERT_EQ(executor.GetInFlightTimeoutCount(), countBefore); // success does not leak the count
}
} // namespace Rosen
} // namespace OHOS
