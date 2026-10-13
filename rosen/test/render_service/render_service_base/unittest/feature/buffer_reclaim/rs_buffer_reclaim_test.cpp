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
#include <condition_variable>
#include <future>
#include <gmock/gmock.h>
#include <mutex>
#include <thread>
#include "gtest/gtest.h"
#include "feature/buffer_reclaim/rs_buffer_reclaim.h"
#include "surface_buffer_impl.h"
#include "ffrt.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
#ifndef ROSEN_CROSS_PLATFORM
constexpr uint32_t ADD_SAME_NODE_NUMS = 5;
constexpr uint64_t TEST_BUFFER_ID_BASE = 10000;
constexpr uint32_t MAX_RECLAIM_NUMS = 50;
constexpr int32_t RECLAIM_TIMEOUT1 = 1000;
constexpr int32_t RECLAIM_TIMEOUT2 = 50;

class MockSurfaceBuffer : public SurfaceBufferImpl {
public:
    MockSurfaceBuffer() : SurfaceBufferImpl() {}
    ~MockSurfaceBuffer() override = default;

    MOCK_METHOD0(TryReclaim, GSError());
    MOCK_METHOD0(TryResumeIfNeeded, GSError());
    MOCK_METHOD0(IsReclaimed, bool());
    MOCK_METHOD1(RegisterBufferDestructorCallbackFunc, bool(void (*)(uint64_t)));
    MOCK_METHOD1(UnRegisterBufferDestructorCallbackFunc, bool(void (*)(uint64_t)));

    void SetBufferId(uint64_t id) { customBufferId_ = id; }
    uint64_t GetBufferId() const override
    {
        if (customBufferId_ != 0) {
            return customBufferId_;
        }
        return SurfaceBufferImpl::GetBufferId();
    }

    void SetSeqNum(uint32_t seq) { customSeqNum_ = seq; }
    uint32_t GetSeqNum() const override
    {
        if (customSeqNum_ != 0) {
            return customSeqNum_;
        }
        return SurfaceBufferImpl::GetSeqNum();
    }

private:
    uint64_t customBufferId_ = 0;
    uint32_t customSeqNum_ = 0;
};

class RSBufferReclaimTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    RSBufferReclaim& GetReclaim() { return RSBufferReclaim::GetInstance(); }
private:
    sptr<MockSurfaceBuffer> CreateMockSurfaceBuffer(uint64_t bufferId = 0);
    void ClearReclaimMap();
    void TestReclaimBufferOnFfrtWrongState(uint64_t bufferId, ReclaimState state);
    void TestReclaimBufferOnFfrtRegisterFail(uint64_t bufferId);
    void TestReclaimBufferOnFfrtTryReclaimFail(uint64_t bufferId);
    void TestReclaimBufferOnFfrtEntryErasedDuringReclaim(uint64_t bufferId, GSError reclaimResult);
    void TestDoBufferResumeReclaimingResumeSuccess();
    void TestDoBufferResumeReclaimingTimeout();
    void TestDoBufferResumeAlreadyReclaimingResumeTimeout();
    static uint64_t nextBufferId_;
};

uint64_t RSBufferReclaimTest::nextBufferId_ = TEST_BUFFER_ID_BASE;

void RSBufferReclaimTest::ClearReclaimMap()
{
    auto& reclaim = GetReclaim();
    std::lock_guard<std::mutex> lock(reclaim.bufferReclaimMapMutex_);
    reclaim.bufferReclaimMap_.clear();
}

sptr<MockSurfaceBuffer> RSBufferReclaimTest::CreateMockSurfaceBuffer(uint64_t bufferId)
{
    auto buffer = new MockSurfaceBuffer();
    if (bufferId == 0) {
        bufferId = nextBufferId_++;
    }
    buffer->SetBufferId(bufferId);
    buffer->SetSeqNum(static_cast<uint32_t>(bufferId));
    return sptr<MockSurfaceBuffer>(buffer);
}

void RSBufferReclaimTest::TestReclaimBufferOnFfrtWrongState(uint64_t bufferId, ReclaimState state)
{
    auto& reclaim = GetReclaim();
    auto buffer = CreateMockSurfaceBuffer(bufferId);
    uint64_t id = buffer->GetBufferId();
    {
        std::lock_guard<std::mutex> lock(reclaim.bufferReclaimMapMutex_);
        reclaim.bufferReclaimMap_[id] = state;
    }
    ON_CALL(*buffer, TryReclaim()).WillByDefault(Return(GSERROR_OK));
    ON_CALL(*buffer, IsReclaimed()).WillByDefault(Return(false));
    ON_CALL(*buffer, RegisterBufferDestructorCallbackFunc(_)).WillByDefault(Return(true));
    reclaim.ReclaimBufferOnFfrt(buffer);
    std::lock_guard<std::mutex> lock(reclaim.bufferReclaimMapMutex_);
    EXPECT_FALSE(reclaim.bufferReclaimMap_.find(id) != reclaim.bufferReclaimMap_.end());
}

void RSBufferReclaimTest::TestReclaimBufferOnFfrtRegisterFail(uint64_t bufferId)
{
    auto& reclaim = GetReclaim();
    auto buffer = CreateMockSurfaceBuffer(bufferId);
    uint64_t id = buffer->GetBufferId();
    {
        std::lock_guard<std::mutex> lock(reclaim.bufferReclaimMapMutex_);
        reclaim.bufferReclaimMap_[id] = ReclaimState::PENDING;
    }
    ON_CALL(*buffer, TryReclaim()).WillByDefault(Return(GSERROR_OK));
    ON_CALL(*buffer, IsReclaimed()).WillByDefault(Return(false));
    ON_CALL(*buffer, RegisterBufferDestructorCallbackFunc(_)).WillByDefault(Return(false));
    reclaim.ReclaimBufferOnFfrt(buffer);
    std::lock_guard<std::mutex> lock(reclaim.bufferReclaimMapMutex_);
    EXPECT_FALSE(reclaim.bufferReclaimMap_.find(id) != reclaim.bufferReclaimMap_.end());
}

void RSBufferReclaimTest::TestReclaimBufferOnFfrtTryReclaimFail(uint64_t bufferId)
{
    auto& reclaim = GetReclaim();
    auto buffer = CreateMockSurfaceBuffer(bufferId);
    uint64_t id = buffer->GetBufferId();
    {
        std::lock_guard<std::mutex> lock(reclaim.bufferReclaimMapMutex_);
        reclaim.bufferReclaimMap_[id] = ReclaimState::PENDING;
    }
    ON_CALL(*buffer, TryReclaim()).WillByDefault(Return(GSERROR_API_FAILED));
    ON_CALL(*buffer, IsReclaimed()).WillByDefault(Return(false));
    ON_CALL(*buffer, RegisterBufferDestructorCallbackFunc(_)).WillByDefault(Return(true));
    ON_CALL(*buffer, UnRegisterBufferDestructorCallbackFunc(_)).WillByDefault(Return(true));
    reclaim.ReclaimBufferOnFfrt(buffer);
    std::lock_guard<std::mutex> lock(reclaim.bufferReclaimMapMutex_);
    EXPECT_FALSE(reclaim.bufferReclaimMap_.find(id) != reclaim.bufferReclaimMap_.end());
}

void RSBufferReclaimTest::TestReclaimBufferOnFfrtEntryErasedDuringReclaim(uint64_t bufferId, GSError reclaimResult)
{
    auto& reclaim = GetReclaim();
    auto buffer = CreateMockSurfaceBuffer(bufferId);
    uint64_t id = buffer->GetBufferId();
    ON_CALL(*buffer, TryReclaim())
        .WillByDefault(Invoke([&reclaim, id, reclaimResult]() {
            reclaim.RemoveBufferReclaim(id);
            return reclaimResult;
        }));
    ON_CALL(*buffer, IsReclaimed()).WillByDefault(Return(false));
    ON_CALL(*buffer, RegisterBufferDestructorCallbackFunc(_)).WillByDefault(Return(true));
    ON_CALL(*buffer, UnRegisterBufferDestructorCallbackFunc(_)).WillByDefault(Return(true));
    EXPECT_TRUE(reclaim.DoBufferReclaim(buffer));
    ffrt::wait();
    std::lock_guard<std::mutex> lock(reclaim.bufferReclaimMapMutex_);
    EXPECT_FALSE(reclaim.bufferReclaimMap_.find(id) != reclaim.bufferReclaimMap_.end());
}

void RSBufferReclaimTest::TestDoBufferResumeReclaimingResumeSuccess()
{
    const uint64_t bufferId = 5040;
    auto& reclaim = GetReclaim();
    auto buffer = CreateMockSurfaceBuffer(bufferId);
    std::atomic<bool> reclaimDone(false);
    std::condition_variable reclaimCv;
    std::mutex reclaimMtx;

    ON_CALL(*buffer, TryReclaim())
        .WillByDefault(Invoke([&reclaimMtx, &reclaimCv, &reclaimDone]() {
            std::unique_lock<std::mutex> lk(reclaimMtx);
            const int32_t timeout1 = 200;
            reclaimCv.wait_for(lk, std::chrono::milliseconds(timeout1),
                [&reclaimDone]() { return reclaimDone.load(); });
            return GSERROR_OK;
        }));
    ON_CALL(*buffer, IsReclaimed()).WillByDefault(Return(false));
    ON_CALL(*buffer, RegisterBufferDestructorCallbackFunc(_)).WillByDefault(Return(true));
    ON_CALL(*buffer, UnRegisterBufferDestructorCallbackFunc(_)).WillByDefault(Return(true));
    ON_CALL(*buffer, TryResumeIfNeeded()).WillByDefault(Return(GSERROR_OK));

    EXPECT_TRUE(reclaim.DoBufferReclaim(buffer));
        std::this_thread::sleep_for(std::chrono::milliseconds(RECLAIM_TIMEOUT2));
    auto resumeFuture = std::async(std::launch::async, [&reclaim, &buffer]() {
        return reclaim.DoBufferResume(buffer);
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(RECLAIM_TIMEOUT2));
    reclaimDone.store(true);
    reclaimCv.notify_all();
    ffrt::wait();
    EXPECT_TRUE(resumeFuture.get());
}

void RSBufferReclaimTest::TestDoBufferResumeReclaimingTimeout()
{
    const uint64_t bufferId = 5041;
    auto& reclaim = GetReclaim();
    auto buffer = CreateMockSurfaceBuffer(bufferId);
    ON_CALL(*buffer, TryReclaim())
        .WillByDefault(Invoke([]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(RECLAIM_TIMEOUT1));
            return GSERROR_OK;
        }));
    ON_CALL(*buffer, IsReclaimed()).WillByDefault(Return(false));
    ON_CALL(*buffer, RegisterBufferDestructorCallbackFunc(_)).WillByDefault(Return(true));
    ON_CALL(*buffer, UnRegisterBufferDestructorCallbackFunc(_)).WillByDefault(Return(true));
    ON_CALL(*buffer, TryResumeIfNeeded()).WillByDefault(Return(GSERROR_OK));

    EXPECT_TRUE(reclaim.DoBufferReclaim(buffer));
    std::this_thread::sleep_for(std::chrono::milliseconds(RECLAIM_TIMEOUT2));
    EXPECT_FALSE(reclaim.DoBufferResume(buffer));
    ffrt::wait();
}

void RSBufferReclaimTest::TestDoBufferResumeAlreadyReclaimingResumeTimeout()
{
    uint64_t bufferId = 5042;
    auto& reclaim = GetReclaim();
    auto buffer = CreateMockSurfaceBuffer(bufferId);
    bufferId = buffer->GetBufferId();
    ON_CALL(*buffer, TryReclaim()).WillByDefault(Return(GSERROR_OK));
    ON_CALL(*buffer, IsReclaimed()).WillByDefault(Return(false));
    ON_CALL(*buffer, RegisterBufferDestructorCallbackFunc(_)).WillByDefault(Return(true));
    ON_CALL(*buffer, UnRegisterBufferDestructorCallbackFunc(_)).WillByDefault(Return(true));
    ON_CALL(*buffer, TryResumeIfNeeded()).WillByDefault(Return(GSERROR_OK));
    {
        std::lock_guard<std::mutex> lock(reclaim.bufferReclaimMapMutex_);
        reclaim.bufferReclaimMap_[bufferId] = ReclaimState::RECLAIMING_RESUME;
    }
    EXPECT_FALSE(reclaim.DoBufferResume(buffer));
}

void RSBufferReclaimTest::SetUp()
{
    nextBufferId_ = TEST_BUFFER_ID_BASE;
    ffrt::wait();
    ClearReclaimMap();
}

void RSBufferReclaimTest::TearDown()
{
    ffrt::wait();
    ClearReclaimMap();
    auto& reclaim = GetReclaim();
    std::lock_guard<std::mutex> uiLock(reclaim.uiCaptureNodeMapMutex_);
    reclaim.uiCaptureNodeMap_.clear();
}
void RSBufferReclaimTest::SetUpTestCase() {}
void RSBufferReclaimTest::TearDownTestCase() {}

/**
 * @tc.name:  DoBufferReclaimAndResumeTest001
 * @tc.desc: test DoBufferReclaim + DoBufferResume full lifecycle (reclaim→RECLAIMED→resume)
 *           and duplicate reclaim/resume, null buffer, RemoveBufferReclaim, CheckBufferReclaim
 * @tc.type: FUNC
 * @tc.require: issue#913
 */
HWTEST_F(RSBufferReclaimTest, DoBufferReclaimAndResumeTest001, TestSize.Level2)
{
    auto& reclaim = GetReclaim();
    // null buffer
    sptr<SurfaceBuffer> nullBuffer = nullptr;
    EXPECT_FALSE(reclaim.DoBufferReclaim(nullBuffer));
    EXPECT_FALSE(reclaim.DoBufferResume(nullBuffer));

    // normal reclaim → resume lifecycle
    auto buffer = CreateMockSurfaceBuffer();
    EXPECT_NE(buffer, nullptr);
    uint64_t bufferId = buffer->GetBufferId();
    ON_CALL(*buffer, TryReclaim()).WillByDefault(Return(GSERROR_OK));
    ON_CALL(*buffer, TryResumeIfNeeded()).WillByDefault(Return(GSERROR_OK));
    ON_CALL(*buffer, IsReclaimed()).WillByDefault(Return(false));
    ON_CALL(*buffer, RegisterBufferDestructorCallbackFunc(_)).WillByDefault(Return(true));
    ON_CALL(*buffer, UnRegisterBufferDestructorCallbackFunc(_)).WillByDefault(Return(true));

    EXPECT_TRUE(reclaim.CheckBufferReclaim());
    EXPECT_TRUE(reclaim.DoBufferReclaim(buffer));
    ffrt::wait();
    // state should be RECLAIMED
    {
        std::lock_guard<std::mutex> lock(reclaim.bufferReclaimMapMutex_);
        auto iter = reclaim.bufferReclaimMap_.find(bufferId);
        ASSERT_NE(iter, reclaim.bufferReclaimMap_.end());
        EXPECT_EQ(iter->second, ReclaimState::RECLAIMED);
    }
    // duplicate DoBufferReclaim — should fail (bufferId already in map)
    EXPECT_FALSE(reclaim.DoBufferReclaim(buffer));
    // resume should succeed
    EXPECT_TRUE(reclaim.DoBufferResume(buffer));
    // double resume — should fail (entry was erased)
    EXPECT_FALSE(reclaim.DoBufferResume(buffer));

    // RemoveBufferReclaim: add then remove then remove-nonexistent
    auto buffer2 = CreateMockSurfaceBuffer();
    EXPECT_NE(buffer2, nullptr);
    uint64_t bufferId2 = buffer2->GetBufferId();
    ON_CALL(*buffer2, TryReclaim()).WillByDefault(Return(GSERROR_OK));
    ON_CALL(*buffer2, IsReclaimed()).WillByDefault(Return(false));
    ON_CALL(*buffer2, RegisterBufferDestructorCallbackFunc(_)).WillByDefault(Return(true));
    EXPECT_TRUE(reclaim.DoBufferReclaim(buffer2));
    ffrt::wait();
    {
        std::lock_guard<std::mutex> lock(reclaim.bufferReclaimMapMutex_);
        EXPECT_TRUE(reclaim.bufferReclaimMap_.find(bufferId2) != reclaim.bufferReclaimMap_.end());
    }
    reclaim.RemoveBufferReclaim(bufferId2);
    {
        std::lock_guard<std::mutex> lock(reclaim.bufferReclaimMapMutex_);
        EXPECT_FALSE(reclaim.bufferReclaimMap_.find(bufferId2) != reclaim.bufferReclaimMap_.end());
    }
    // remove non-existent — should not crash
    reclaim.RemoveBufferReclaim(bufferId2);
}

/**
 * @tc.name:  DoBufferReclaimMaxCapacityTest001
 * @tc.desc: test DoBufferReclaim max capacity and CheckBufferReclaim boundary
 * @tc.type: FUNC
 * @tc.require: issue#913
 */
HWTEST_F(RSBufferReclaimTest, DoBufferReclaimMaxCapacityTest001, TestSize.Level2)
{
    auto& reclaim = GetReclaim();
    for (uint32_t i = 0; i < MAX_RECLAIM_NUMS; i++) {
        auto buffer = CreateMockSurfaceBuffer();
        EXPECT_NE(buffer, nullptr);
        ON_CALL(*buffer, TryReclaim()).WillByDefault(Return(GSERROR_OK));
        ON_CALL(*buffer, RegisterBufferDestructorCallbackFunc(_)).WillByDefault(Return(true));
        EXPECT_TRUE(reclaim.CheckBufferReclaim());
        EXPECT_TRUE(reclaim.DoBufferReclaim(buffer));
    }
    ffrt::wait();
    EXPECT_FALSE(reclaim.CheckBufferReclaim());
    auto buffer = CreateMockSurfaceBuffer();
    EXPECT_NE(buffer, nullptr);
    EXPECT_FALSE(reclaim.DoBufferReclaim(buffer));
}

/**
 * @tc.name:  ReclaimBufferOnFfrtStateAndFailTest001
 * @tc.desc: test ReclaimBufferOnFfrt: not-PENDING erase, RegisterFail, TryReclaim fail,
 *           and entry erased during TryReclaim (success & fail paths)
 * @tc.type: FUNC
 * @tc.require: issue#913
 */
HWTEST_F(RSBufferReclaimTest, ReclaimBufferOnFfrtStateAndFailTest001, TestSize.Level2)
{
    uint64_t bufferId = 5001;
    // 1. state not PENDING (RECLAIMING) — should erase and return
    TestReclaimBufferOnFfrtWrongState(bufferId, ReclaimState::RECLAIMING);
    // 2. state not PENDING (RECLAIMED) — should erase and return
    TestReclaimBufferOnFfrtWrongState(++bufferId, ReclaimState::RECLAIMED);
    // 3. RegisterBufferDestructorCallbackFunc fails — entry erased
    TestReclaimBufferOnFfrtRegisterFail(++bufferId);
    // 4. TryReclaim fails — entry erased, UnRegister called
    TestReclaimBufferOnFfrtTryReclaimFail(++bufferId);
    // 5. TryReclaim succeeds but entry erased during execution (L66 false)
    TestReclaimBufferOnFfrtEntryErasedDuringReclaim(++bufferId, GSERROR_OK);
    // 6. TryReclaim fails and entry erased during execution (L76 false)
    TestReclaimBufferOnFfrtEntryErasedDuringReclaim(++bufferId, GSERROR_API_FAILED);
}

/**
 * @tc.name:  ReclaimBufferOnFfrtCancelledByRemoveTest001
 * @tc.desc: test ReclaimBufferOnFfrt when entry removed before ffrt runs
 * @tc.type: FUNC
 * @tc.require: issue#913
 */
HWTEST_F(RSBufferReclaimTest, ReclaimBufferOnFfrtCancelledByRemoveTest001, TestSize.Level2)
{
    uint64_t bufferId = 5010;
    auto& reclaim = GetReclaim();
    auto buffer = CreateMockSurfaceBuffer(bufferId);
    bufferId = buffer->GetBufferId();
    ON_CALL(*buffer, TryReclaim()).WillByDefault(Return(GSERROR_OK));
    ON_CALL(*buffer, IsReclaimed()).WillByDefault(Return(false));
        ON_CALL(*buffer, RegisterBufferDestructorCallbackFunc(_)).WillByDefault(Return(true));

    EXPECT_TRUE(reclaim.DoBufferReclaim(buffer));
    reclaim.RemoveBufferReclaim(bufferId);
    ffrt::wait();
    std::lock_guard<std::mutex> lock(reclaim.bufferReclaimMapMutex_);
    EXPECT_FALSE(reclaim.bufferReclaimMap_.find(bufferId) != reclaim.bufferReclaimMap_.end());
}

/**
 * @tc.name:  DoBufferResumeTest001
 * @tc.desc: test DoBufferResume: not-in-map, PENDING cancel, RECLAIMED resume,
 *           resume fail, unregister fail
 * @tc.type: FUNC
 * @tc.require: issue#913
 */
HWTEST_F(RSBufferReclaimTest, DoBufferResumeTest001, TestSize.Level2)
{
    uint64_t bufferId = 5020;
    auto& reclaim = GetReclaim();

    // 1. not in map — false
    {
        auto buffer = CreateMockSurfaceBuffer(bufferId);
        EXPECT_FALSE(reclaim.DoBufferResume(buffer));
    }

    // 2. PENDING — cancel and return true
    {
        auto buffer = CreateMockSurfaceBuffer(++bufferId);
        bufferId = buffer->GetBufferId();
        {
            std::lock_guard<std::mutex> lock(reclaim.bufferReclaimMapMutex_);
            reclaim.bufferReclaimMap_[bufferId] = ReclaimState::PENDING;
        }
        EXPECT_TRUE(reclaim.DoBufferResume(buffer));
        std::lock_guard<std::mutex> lock(reclaim.bufferReclaimMapMutex_);
        EXPECT_FALSE(reclaim.bufferReclaimMap_.find(bufferId) != reclaim.bufferReclaimMap_.end());
    }

    // 3. RECLAIMED + TryResumeIfNeeded fail — false, entry stays
    {
        auto buffer = CreateMockSurfaceBuffer(++bufferId);
        bufferId = buffer->GetBufferId();
        ON_CALL(*buffer, TryReclaim()).WillByDefault(Return(GSERROR_OK));
        ON_CALL(*buffer, IsReclaimed()).WillByDefault(Return(false));
        ON_CALL(*buffer, RegisterBufferDestructorCallbackFunc(_)).WillByDefault(Return(true));
        ON_CALL(*buffer, UnRegisterBufferDestructorCallbackFunc(_)).WillByDefault(Return(true));
        ON_CALL(*buffer, TryResumeIfNeeded()).WillByDefault(Return(GSERROR_API_FAILED));
        EXPECT_TRUE(reclaim.DoBufferReclaim(buffer));
        ffrt::wait();
        EXPECT_FALSE(reclaim.DoBufferResume(buffer));
        std::lock_guard<std::mutex> lock(reclaim.bufferReclaimMapMutex_);
        EXPECT_TRUE(reclaim.bufferReclaimMap_.find(bufferId) != reclaim.bufferReclaimMap_.end());
    }

    // 4. RECLAIMED + UnRegister fail — resume still succeeds (just warns)
    {
        auto buffer = CreateMockSurfaceBuffer(++bufferId);
        ON_CALL(*buffer, TryReclaim()).WillByDefault(Return(GSERROR_OK));
        ON_CALL(*buffer, IsReclaimed()).WillByDefault(Return(false));
        ON_CALL(*buffer, RegisterBufferDestructorCallbackFunc(_)).WillByDefault(Return(true));
        ON_CALL(*buffer, UnRegisterBufferDestructorCallbackFunc(_)).WillByDefault(Return(false));
        ON_CALL(*buffer, TryResumeIfNeeded()).WillByDefault(Return(GSERROR_OK));
        EXPECT_TRUE(reclaim.DoBufferReclaim(buffer));
        ffrt::wait();
        EXPECT_TRUE(reclaim.DoBufferResume(buffer));
    }
}

/**
 * @tc.name:  TryResumeBufferDirectTest001
 * @tc.desc: test TryResumeBuffer directly: not-in-map, PENDING, RECLAIMING,
 *           RECLAIMING_RESUME, RECLAIMED+success, RECLAIMED+fail
 * @tc.type: FUNC
 * @tc.require: issue#26422
 */
HWTEST_F(RSBufferReclaimTest, TryResumeBufferDirectTest001, TestSize.Level2)
{
    uint64_t bufferId = 5030;
    auto& reclaim = GetReclaim();
    auto buffer = CreateMockSurfaceBuffer(bufferId);
    bufferId = buffer->GetBufferId();
    ON_CALL(*buffer, TryResumeIfNeeded()).WillByDefault(Return(GSERROR_OK));
    ON_CALL(*buffer, UnRegisterBufferDestructorCallbackFunc(_)).WillByDefault(Return(true));

    // not in map
    EXPECT_FALSE(reclaim.TryResumeBuffer(buffer, bufferId));

    // PENDING
    {
        std::lock_guard<std::mutex> lock(reclaim.bufferReclaimMapMutex_);
        reclaim.bufferReclaimMap_[bufferId] = ReclaimState::PENDING;
    }
    EXPECT_FALSE(reclaim.TryResumeBuffer(buffer, bufferId));

    // RECLAIMING
    {
        std::lock_guard<std::mutex> lock(reclaim.bufferReclaimMapMutex_);
        reclaim.bufferReclaimMap_[bufferId] = ReclaimState::RECLAIMING;
    }
    EXPECT_FALSE(reclaim.TryResumeBuffer(buffer, bufferId));

    // RECLAIMING_RESUME
    {
        std::lock_guard<std::mutex> lock(reclaim.bufferReclaimMapMutex_);
        reclaim.bufferReclaimMap_[bufferId] = ReclaimState::RECLAIMING_RESUME;
    }
    EXPECT_FALSE(reclaim.TryResumeBuffer(buffer, bufferId));

    // RECLAIMED + success
    {
        std::lock_guard<std::mutex> lock(reclaim.bufferReclaimMapMutex_);
        reclaim.bufferReclaimMap_[bufferId] = ReclaimState::RECLAIMED;
    }
    EXPECT_TRUE(reclaim.TryResumeBuffer(buffer, bufferId));
    {
        std::lock_guard<std::mutex> lock(reclaim.bufferReclaimMapMutex_);
        EXPECT_FALSE(reclaim.bufferReclaimMap_.find(bufferId) != reclaim.bufferReclaimMap_.end());
    }

    // RECLAIMED + fail
    auto buffer2 = CreateMockSurfaceBuffer(++bufferId);
    uint64_t bufferId2 = buffer2->GetBufferId();
    ON_CALL(*buffer2, TryResumeIfNeeded()).WillByDefault(Return(GSERROR_API_FAILED));
    {
        std::lock_guard<std::mutex> lock(reclaim.bufferReclaimMapMutex_);
        reclaim.bufferReclaimMap_[bufferId2] = ReclaimState::RECLAIMED;
    }
    EXPECT_FALSE(reclaim.TryResumeBuffer(buffer2, bufferId2));
    {
        std::lock_guard<std::mutex> lock(reclaim.bufferReclaimMapMutex_);
        EXPECT_TRUE(reclaim.bufferReclaimMap_.find(bufferId2) != reclaim.bufferReclaimMap_.end());
    }
}

/**
 * @tc.name:  DoBufferResumeReclaimingResumeTest001
 * @tc.desc: test DoBufferResume RECLAIMING_RESUME: success path and timeout path
 * @tc.type: FUNC
 * @tc.require: issue#26422
 */
HWTEST_F(RSBufferReclaimTest, DoBufferResumeReclaimingResumeTest001, TestSize.Level2)
{
    // 1. RECLAIMING → RECLAIMING_RESUME → reclaim done → resume succeeds
    TestDoBufferResumeReclaimingResumeSuccess();
    // 2. RECLAIMING → timeout (TryReclaim hangs beyond RECLAIM_TIMEOUT_MS)
    TestDoBufferResumeReclaimingTimeout();
    // 3. already RECLAIMING_RESUME (no ffrt running) → timeout
    TestDoBufferResumeAlreadyReclaimingResumeTimeout();
}

/**
 * @tc.name:  UICaptureNodeTest001
 * @tc.desc: test AddUICaptureNode/RemoveUICaptureNode/CheckSameProcessUICaptureNode:
 *           single node, same-pid multi-node, same-node count, multi-pid,
 *           invalid pid/nodeId removal
 * @tc.type: FUNC
 * @tc.require: issue#913
 */
HWTEST_F(RSBufferReclaimTest, UICaptureNodeTest001, TestSize.Level1)
{
    auto& reclaim = GetReclaim();
    reclaim.uiCaptureNodeMap_.clear();
    pid_t pid1 = 1234;
    pid_t pid2 = 5678;
    uint32_t uid = 1000;

    // remove from empty map — no crash
    NodeId id0 = MakeNodeId(pid1, uid);
    reclaim.RemoveUICaptureNode(id0);
    EXPECT_TRUE(reclaim.uiCaptureNodeMap_.empty());

    // new pid + node
    NodeId id1 = MakeNodeId(pid1, uid++);
    reclaim.AddUICaptureNode(id1);
    EXPECT_TRUE(reclaim.CheckSameProcessUICaptureNode(id1));
    EXPECT_EQ(reclaim.uiCaptureNodeMap_.size(), 1u);
    EXPECT_EQ(reclaim.uiCaptureNodeMap_[pid1].size(), 1u);

    // same pid, new node
    NodeId id2 = MakeNodeId(pid1, uid++);
    reclaim.AddUICaptureNode(id2);
    EXPECT_EQ(reclaim.uiCaptureNodeMap_[pid1].size(), 2u);

    // same pid, same node — count increments
    reclaim.AddUICaptureNode(id1);
    EXPECT_EQ(reclaim.uiCaptureNodeMap_[pid1].size(), 2u);
    EXPECT_EQ(reclaim.uiCaptureNodeMap_[pid1][id1], 2u);

    // different pid
    NodeId id3 = MakeNodeId(pid2, uid++);
    reclaim.AddUICaptureNode(id3);
    EXPECT_EQ(reclaim.uiCaptureNodeMap_.size(), 2u);
    EXPECT_TRUE(reclaim.CheckSameProcessUICaptureNode(id3));
    EXPECT_FALSE(reclaim.CheckSameProcessUICaptureNode(MakeNodeId(9999, uid)));

    // remove same node (count 2→1, not erased)
    reclaim.RemoveUICaptureNode(id1);
    EXPECT_EQ(reclaim.uiCaptureNodeMap_[pid1][id1], 1u);

    // remove same node again (count 1→0, node erased from map)
    reclaim.RemoveUICaptureNode(id1);
    EXPECT_EQ(reclaim.uiCaptureNodeMap_[pid1].size(), 1u);

    // remove invalid nodeId (pid in map, node not in map)
    NodeId idInvalid = MakeNodeId(pid1, uid++);
    reclaim.RemoveUICaptureNode(idInvalid);
    EXPECT_EQ(reclaim.uiCaptureNodeMap_[pid1].size(), 1u);

    // remove remaining nodes — pid map shrinks
    reclaim.RemoveUICaptureNode(id2);
    EXPECT_EQ(reclaim.uiCaptureNodeMap_.size(), 1u);
    reclaim.RemoveUICaptureNode(id3);
    EXPECT_TRUE(reclaim.uiCaptureNodeMap_.empty());
}

/**
 * @tc.name:  UICaptureNodeSameNodeCountTest001
 * @tc.desc: test AddUICaptureNode/RemoveUICaptureNode with same node added N times
 * @tc.type: FUNC
 * @tc.require: issue#913
 */
HWTEST_F(RSBufferReclaimTest, UICaptureNodeSameNodeCountTest001, TestSize.Level1)
{
    auto& reclaim = GetReclaim();
    reclaim.uiCaptureNodeMap_.clear();
    pid_t pid = 1234;
    NodeId id = MakeNodeId(pid, 1000);

    for (uint32_t i = 1; i <= ADD_SAME_NODE_NUMS; i++) {
        reclaim.AddUICaptureNode(id);
        EXPECT_EQ(reclaim.uiCaptureNodeMap_[pid][id], i);
    }
    for (uint32_t i = 1; i <= ADD_SAME_NODE_NUMS; i++) {
        reclaim.RemoveUICaptureNode(id);
        if (i != ADD_SAME_NODE_NUMS) {
            EXPECT_EQ(reclaim.uiCaptureNodeMap_[pid][id], ADD_SAME_NODE_NUMS - i);
        } else {
            EXPECT_TRUE(reclaim.uiCaptureNodeMap_.empty());
        }
    }
}
#endif
} // namespace OHOS::Rosen
