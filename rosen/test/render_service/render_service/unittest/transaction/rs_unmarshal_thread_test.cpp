/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "ffrt_inner.h"
#include "gtest/gtest.h"

#include <chrono>
#include <fcntl.h>
#include <future>
#include <unistd.h>

#include "command/rs_command.h"
#include "platform/common/rs_system_properties.h"
#include "transaction/rs_ashmem_helper.h"
#include "transaction/rs_transaction_data.h"
#include "transaction/rs_unmarshal_thread.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
#define USLEEP_TIME (110 * 1000)
const int MAX_CONCURRENCY = 3;
class RSUnmarshalThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUnmarshalThreadTest::SetUpTestCase() {}
void RSUnmarshalThreadTest::TearDownTestCase() {}
void RSUnmarshalThreadTest::SetUp() {}
void RSUnmarshalThreadTest::TearDown()
{
    usleep(USLEEP_TIME);
}

/**
 * @tc.name: PostTask001
 * @tc.desc: Test PostTask
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSUnmarshalThreadTest, PostTask001, TestSize.Level1)
{
    std::function<void()> func = []() -> void {};
    RSUnmarshalThread::Instance().queue_ = nullptr;
    RSUnmarshalThread::Instance().PostTask(func);

    RSUnmarshalThread::Instance().Start();
    ASSERT_NE(RSUnmarshalThread::Instance().queue_, nullptr);
    RSUnmarshalThread::Instance().PostTask(func);
}

/**
 * @tc.name: PostTask002
 * @tc.desc: Test PostTask
 * @tc.type: FUNC
 * @tc.require: issueICLG1E
 */
HWTEST_F(RSUnmarshalThreadTest, PostTask002, TestSize.Level1)
{
    RSUnmarshalThread& instance = RSUnmarshalThread::Instance();
    std::function<void()> func = []() -> void {};
    std::string name = "test";

    instance.queue_ = nullptr;
    instance.PostTask(func, name);

    instance.Start();
    EXPECT_NE(instance.queue_, nullptr);
    instance.PostTask(func, name);
}

/**
 * @tc.name: RemoveTask001
 * @tc.desc: Test RemoveTask
 * @tc.type: FUNC
 * @tc.require: issueICLG1E
 */
HWTEST_F(RSUnmarshalThreadTest, RemoveTask001, TestSize.Level1)
{
    RSUnmarshalThread& instance = RSUnmarshalThread::Instance();
    std::function<void()> func = []() -> void {};
    std::string name = "test";

    instance.queue_ = nullptr;
    instance.RemoveTask(name);

    instance.Start();
    EXPECT_NE(instance.queue_, nullptr);
    instance.PostTask(func, name);
    instance.RemoveTask(name);
    EXPECT_EQ(instance.queue_->get_task_cnt(), 0);
}

/**
 * @tc.name: RecvParcel001
 * @tc.desc: Test RecvParcel
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSUnmarshalThreadTest, RecvParcel001, TestSize.Level1)
{
    RSUnmarshalThread& instance = RSUnmarshalThread::Instance();
    instance.Start();
    ASSERT_NE(instance.queue_, nullptr);
    
    std::shared_ptr<RSTransactionData> transactionData = std::make_shared<RSTransactionData>();
    std::shared_ptr<MessageParcel> data = std::make_shared<MessageParcel>();

    instance.RecvParcel(data);

    bool success = data->WriteParcelable(transactionData.get());
    ASSERT_EQ(success, true);
    instance.RecvParcel(data);

    instance.queue_ = nullptr;
    instance.RecvParcel(data);

    data = nullptr;
    instance.RecvParcel(data);

    instance.Start();
    ASSERT_NE(instance.queue_, nullptr);
    instance.RecvParcel(data);
}

/**
 * @tc.name: RecvParcel002
 * @tc.desc: Test RecvParcel
 * @tc.type: FUNC
 * @tc.require: issueIAI1VN
 */
HWTEST_F(RSUnmarshalThreadTest, RecvParcel002, TestSize.Level1)
{
    RSUnmarshalThread::Instance().Start();
    ASSERT_NE(RSUnmarshalThread::Instance().queue_, nullptr);

    std::shared_ptr<RSTransactionData> transactionData = std::make_shared<RSTransactionData>();
    std::shared_ptr<MessageParcel> data = std::make_shared<MessageParcel>();

    RSUnmarshalThread::Instance().RecvParcel(data);

    bool success = data->WriteParcelable(transactionData.get());
    ASSERT_EQ(success, true);
    bool isNonSystemAppCalling = true;
    pid_t callingPid = 1111;
    RSUnmarshalThread::Instance().RecvParcel(data, isNonSystemAppCalling, callingPid);
}

/**
 * @tc.name: RecvParcel003
 * @tc.desc: Test RecvParcel
 * @tc.type: FUNC
 * @tc.require: issueIAI1VN
 */
HWTEST_F(RSUnmarshalThreadTest, RecvParcel003, TestSize.Level1)
{
    RSUnmarshalThread::Instance().Start();
    ASSERT_NE(RSUnmarshalThread::Instance().queue_, nullptr);

    std::shared_ptr<RSTransactionData> transactionData = std::make_shared<RSTransactionData>();
    std::shared_ptr<MessageParcel> data = std::make_shared<MessageParcel>();
    transactionData->SetDVSyncUpdate(true);

    RSUnmarshalThread::Instance().RecvParcel(data);

    bool success = data->WriteParcelable(transactionData.get());
    ASSERT_EQ(success, true);
    RSUnmarshalThread::Instance().RecvParcel(data);
    bool isNonSystemAppCalling = true;
    pid_t callingPid = 1111;
    RSUnmarshalThread::Instance().RecvParcel(data, isNonSystemAppCalling, callingPid);
}

#ifdef RS_ENABLE_UNI_RENDER
/**
 * @tc.name: RecvParcelSetSendingPid
 * @tc.desc: Test RecvParcel sets sendingPid so that the parsed transaction data is filed
 *           under callingPid in cachedTransactionDataMap_.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUnmarshalThreadTest, RecvParcelSetSendingPid, TestSize.Level1)
{
    RSUnmarshalThread& instance = RSUnmarshalThread::Instance();
    instance.Start();
    ASSERT_NE(instance.queue_, nullptr);

    instance.GetCachedTransactionData();

    std::shared_ptr<RSTransactionData> transactionData = std::make_shared<RSTransactionData>();
    std::shared_ptr<MessageParcel> data = std::make_shared<MessageParcel>();
    bool success = data->WriteParcelable(transactionData.get());
    ASSERT_EQ(success, true);

    pid_t callingPid = 9999;
    instance.RecvParcel(data, false, callingPid);

    usleep(USLEEP_TIME);

    auto cachedData = instance.GetCachedTransactionData();
    EXPECT_EQ(cachedData.count(callingPid), 1u);
}
#endif

/**
 * @tc.name: RecvParcelFdWorkerQueueNull
 * @tc.desc: Test RecvParcel closes the collected ashmem fd when the queue is unavailable
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUnmarshalThreadTest, RecvParcelFdWorkerQueueNull, TestSize.Level1)
{
    RSUnmarshalThread& instance = RSUnmarshalThread::Instance();
    instance.queue_ = nullptr;

    int ownedFd = open("/dev/null", O_RDONLY);
    ASSERT_GE(ownedFd, 0);
    auto ashmemFdWorker = std::make_unique<AshmemFdWorker>(0);
    ashmemFdWorker->InsertFdWithOffset(ownedFd, 0, true);

    std::shared_ptr<MessageParcel> data = std::make_shared<MessageParcel>();
    instance.RecvParcel(data, false, 0, std::move(ashmemFdWorker));
    // early return on the null queue destroys the worker, which closes the fd unconditionally
    EXPECT_EQ(fcntl(ownedFd, F_GETFD), -1);

    instance.Start();
}

/**
 * @tc.name: RecvParcelFdWorkerParcelNull
 * @tc.desc: Test RecvParcel closes the collected ashmem fd when the parcel is null
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUnmarshalThreadTest, RecvParcelFdWorkerParcelNull, TestSize.Level1)
{
    RSUnmarshalThread& instance = RSUnmarshalThread::Instance();
    instance.Start();
    ASSERT_NE(instance.queue_, nullptr);

    int ownedFd = open("/dev/null", O_RDONLY);
    ASSERT_GE(ownedFd, 0);
    auto ashmemFdWorker = std::make_unique<AshmemFdWorker>(0);
    ashmemFdWorker->InsertFdWithOffset(ownedFd, 0, true);

    std::shared_ptr<MessageParcel> data = nullptr;
    instance.RecvParcel(data, false, 0, std::move(ashmemFdWorker));
    // early return on the null parcel destroys the worker, which closes the fd unconditionally
    EXPECT_EQ(fcntl(ownedFd, F_GETFD), -1);
}

/**
 * @tc.name: RecvParcelFdWorkerTaskExecuted
 * @tc.desc: Test RecvParcel closes the collected ashmem fd once the posted task has run
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUnmarshalThreadTest, RecvParcelFdWorkerTaskExecuted, TestSize.Level1)
{
    RSUnmarshalThread& instance = RSUnmarshalThread::Instance();
    instance.Start();
    ASSERT_NE(instance.queue_, nullptr);

    // probe whether the unmarshal queue actually runs tasks in this environment
    std::promise<void> probe;
    auto probeFuture = probe.get_future();
    instance.PostTask([&probe]() { probe.set_value(); }, "probe");
    if (probeFuture.wait_for(std::chrono::seconds(1)) != std::future_status::ready) {
        GTEST_SKIP() << "unmarshal queue does not run tasks in this environment";
    }

    int ownedFd = open("/dev/null", O_RDONLY);
    ASSERT_GE(ownedFd, 0);
    auto ashmemFdWorker = std::make_unique<AshmemFdWorker>(0);
    ashmemFdWorker->InsertFdWithOffset(ownedFd, 0, true);

    // an empty parcel makes ParseTransactionData fail; the worker is still reset (and its
    // fd closed) before the task returns
    std::shared_ptr<MessageParcel> data = std::make_shared<MessageParcel>();
    instance.RecvParcel(data, false, 0, std::move(ashmemFdWorker));

    constexpr int maxWaitMs = 1000;
    constexpr int pollIntervalMs = 10;
    constexpr int msPerSecond = 1000;
    bool closed = false;
    for (int waitedMs = 0; waitedMs < maxWaitMs; waitedMs += pollIntervalMs) {
        if (fcntl(ownedFd, F_GETFD) == -1) {
            closed = true;
            break;
        }
        usleep(pollIntervalMs * msPerSecond);
    }
    EXPECT_TRUE(closed);
}

/**
 * @tc.name: TransactionDataStatistics001
 * @tc.desc: Test ReportTransactionDataStatistics and ClearTransactionDataStatistics
 * @tc.type: FUNC
 * @tc.require: issueIAPMUF
 */
HWTEST_F(RSUnmarshalThreadTest, TransactionDataStatistics001, TestSize.Level1)
{
    constexpr pid_t callingPid = -1; // invalid pid
    std::shared_ptr<RSTransactionData> transactionData = std::make_shared<RSTransactionData>();
    constexpr bool isSystemCall = false;
    auto& instance = RSUnmarshalThread::Instance();
    bool terminateEnabled = RSSystemProperties::GetTransactionTerminateEnabled();

    RSUnmarshalThread::Instance().ClearTransactionDataStatistics();
    ASSERT_EQ(instance.ReportTransactionDataStatistics(callingPid, transactionData.get(), !isSystemCall), false);
    ASSERT_EQ(instance.ReportTransactionDataStatistics(callingPid, transactionData.get(), !isSystemCall), false);
    ASSERT_EQ(instance.ReportTransactionDataStatistics(callingPid, transactionData.get(), !isSystemCall), false);

    RSUnmarshalThread::Instance().ClearTransactionDataStatistics();
    ASSERT_EQ(instance.ReportTransactionDataStatistics(callingPid, transactionData.get(), isSystemCall), false);
    ASSERT_EQ(instance.ReportTransactionDataStatistics(callingPid, transactionData.get(), isSystemCall), false);
    ASSERT_EQ(instance.ReportTransactionDataStatistics(callingPid, transactionData.get(), isSystemCall), false);
}

/**
 * @tc.name: WaitUntilParallelTasksFinished001
 * @tc.desc: Test WaitUntilParallelTasksFinished
 * @tc.type: FUNC
 * @tc.require: issue20966
 */
HWTEST_F(RSUnmarshalThreadTest, WaitUntilParallelTasksFinished001, TestSize.Level1)
{
    auto& instance = RSUnmarshalThread::Instance();
    instance.parallelQueue_ = nullptr;
    std::function<void()> func = []() -> void { sleep(2); };
    instance.PostParallelTask(func);
    ASSERT_TRUE(instance.cachedHandles_.empty());
    instance.WaitUntilParallelTasksFinished();

    instance.parallelQueue_ =
        std::make_shared<ffrt::queue>(
            ffrt::queue_concurrent, "RSUnmarshalThreadParallel",
            ffrt::queue_attr().qos(ffrt::qos_user_interactive).max_concurrency(MAX_CONCURRENCY));
    instance.PostParallelTask(func);
    ASSERT_FALSE(instance.cachedHandles_.empty());
    instance.WaitUntilParallelTasksFinished();
}
}