/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <ostream>
#include <securec.h>
#include <cstdint>

#include "refbase.h"
#include "sync_fence.h"
#include "sync_fence_timeline.h"
#include "sync_fence_tracker.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
class SyncFenceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
};

void SyncFenceTest::SetUpTestCase()
{
}

void SyncFenceTest::TearDownTestCase()
{
}

/*
* Function: IsValid
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call IsValid
*                  2. check ret
*/
HWTEST_F(SyncFenceTest, IsValid001, Function | MediumTest | Level2)
{
    sptr<SyncTimeline> syncTimeline_ = new SyncTimeline();
    bool valid = syncTimeline_->IsValid();
    if (valid) {
        ASSERT_EQ(valid, true);
    } else {
        ASSERT_EQ(valid, false);
    }
}

/*
* Function: IsValid, GenerateFence
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GenerateFence
*                  2. check ret
*/
HWTEST_F(SyncFenceTest, GenerateFence001, Function | MediumTest | Level2)
{
    sptr<SyncTimeline> syncTimeline_ = new SyncTimeline();
    bool valid = syncTimeline_->IsValid();
    if (valid) {
        ASSERT_EQ(valid, true);
        int32_t fd = syncTimeline_->GenerateFence("test sw_sync_fence", 1);
        SyncFence syncFence(fd);
        ASSERT_GE(fd, 0);
    } else {
        ASSERT_EQ(valid, false);
        int32_t fd = syncTimeline_->GenerateFence("test sw_sync_fence", 1);
        ASSERT_EQ(fd, -1);
        int32_t ret = syncTimeline_->IncreaseSyncPoint(1);
        ASSERT_EQ(ret, -1);
    }
}

/*
* Function: IsValid, GenerateFence, SyncFileReadTimestamp
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GenerateFence
*                  2. check ret
*/
HWTEST_F(SyncFenceTest, GetFenceInfo001, Function | MediumTest | Level2)
{
    sptr<SyncTimeline> syncTimeline_ = new SyncTimeline();
    bool valid = syncTimeline_->IsValid();
    if (valid) {
        ASSERT_EQ(valid, true);
        int32_t fd = syncTimeline_->GenerateFence("test sw_sync_fence", 1);
        SyncFence syncFence(fd);
        ASSERT_GE(fd, 0);
        int64_t timestamp = syncFence.SyncFileReadTimestamp();
        ASSERT_EQ(timestamp, SyncFence::FENCE_PENDING_TIMESTAMP);
        auto ret = syncTimeline_->IncreaseSyncPoint(1);
        ASSERT_EQ(ret, 0);
        timestamp = syncFence.SyncFileReadTimestamp();
        ASSERT_GT(timestamp, 0);
    } else {
        ASSERT_EQ(valid, false);
    }
}

/*
* Function: IsValid, GenerateFence, IncreaseSyncPoint, GetStatus
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call IsValid, GenerateFence, IncreaseSyncPoint, GetStatus
*                  2. check ret
*/
HWTEST_F(SyncFenceTest, OneTimeLineGetStatus001, Function | MediumTest | Level2)
{
    sptr<SyncTimeline> syncTimeline_ = new SyncTimeline();
    bool valid = syncTimeline_->IsValid();
    if (valid) {
        ASSERT_EQ(valid, true);
        int32_t fd = syncTimeline_->GenerateFence("test sw_sync_fence", 1);
        SyncFence syncFence(fd);
        ASSERT_GE(fd, 0);
        FenceStatus st = syncFence.GetStatus();
        ASSERT_EQ(st, FenceStatus::ACTIVE);
        auto ret = syncTimeline_->IncreaseSyncPoint(1);
        ASSERT_EQ(ret, 0);
        st = syncFence.GetStatus();
        ASSERT_EQ(st, FenceStatus::SIGNALED);
        // go 2 even futher, still signal status
        syncTimeline_->IncreaseSyncPoint(2);
        ASSERT_EQ(ret, 0);
        st = syncFence.GetStatus();
        ASSERT_EQ(st, FenceStatus::SIGNALED);
    } else {
        ASSERT_EQ(valid, false);
    }
}

/*
* Function: IsValid, GenerateFence, Wait, GetStatus
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call IsValid, GenerateFence, Wait, GetStatus
*                  2. check ret
*/
HWTEST_F(SyncFenceTest, OneTimeLineWait001, Function | MediumTest | Level2)
{
    sptr<SyncTimeline> syncTimeline_ = new SyncTimeline();
    bool valid = syncTimeline_->IsValid();
    if (valid) {
        ASSERT_EQ(valid, true);
        int32_t fd = syncTimeline_->GenerateFence("test sw_sync_fence", 3);
        SyncFence syncFence(fd);
        ASSERT_GE(fd, 0);

        // increase timeline from 0 -> 1
        auto ret = syncTimeline_->IncreaseSyncPoint(1);
        ASSERT_EQ(ret, 0);
        // wait on fence until timeout
        ret = syncFence.Wait(0);
        ASSERT_LT(ret, 0);
        ASSERT_EQ(errno, ETIME);
        auto st = syncFence.GetStatus();
        ASSERT_EQ(st, FenceStatus::ACTIVE);
        // increase 2 timeline from 1 -> 3, signal the fence
        ret = syncTimeline_->IncreaseSyncPoint(2);
        ASSERT_EQ(ret, 0);
        // wait succ
        ret = syncFence.Wait(0);
        ASSERT_EQ(ret, 0);
        st = syncFence.GetStatus();
        ASSERT_EQ(st, FenceStatus::SIGNALED);
        // go 5 even futher, still signal status
        ret = syncTimeline_->IncreaseSyncPoint(5);
        ASSERT_EQ(ret, 0);
        ret = syncFence.Wait(0);
        ASSERT_EQ(ret, 0);
        st = syncFence.GetStatus();
        ASSERT_EQ(st, FenceStatus::SIGNALED);
    } else {
        ASSERT_EQ(valid, false);
    }
}

/*
* Function: IsValid, GenerateFence, MergeFence
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call IsValid, GenerateFence, MergeFence
*                  2. check ret
*/
HWTEST_F(SyncFenceTest, OneTimeLineMerge001, Function | MediumTest | Level2)
{
    sptr<SyncTimeline> syncTimeline_ = new SyncTimeline();
    bool valid = syncTimeline_->IsValid();
    if (valid) {
        ASSERT_EQ(valid, true);
        int32_t fd1 = syncTimeline_->GenerateFence("test sw_sync_fence1", 1);
        sptr<SyncFence> syncFence1 = new SyncFence(fd1);
        ASSERT_GE(fd1, 0);
        // timeline generate fence2 target 5 seq.no
        int32_t fd2 = syncTimeline_->GenerateFence("test sw_sync_fence2", 5);
        sptr<SyncFence> syncFence2 = new SyncFence(fd2);
        ASSERT_GE(fd2, 0);

        sptr<SyncFence> fdMerged = SyncFence::MergeFence("merged sw_fence", syncFence1, syncFence2);
        ASSERT_GE(fdMerged->Get(), 0);
        auto st = fdMerged->GetStatus();
        ASSERT_EQ(st, FenceStatus::ACTIVE);

        // increase 1 timeline from 0 -> 1
        auto ret = syncTimeline_->IncreaseSyncPoint(1);
        ASSERT_EQ(ret, 0);
        st = fdMerged->GetStatus();
        ASSERT_EQ(st, FenceStatus::ACTIVE);
        // increase 4 timeline from 1 -> 5, signal fence
        ret = syncTimeline_->IncreaseSyncPoint(4);
        ASSERT_EQ(ret, 0);
        st = fdMerged->GetStatus();
        ASSERT_EQ(st, FenceStatus::SIGNALED);
    } else {
        ASSERT_EQ(valid, false);
    }
}

/*
* Function: IsValid, GenerateFence, MergeFence
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call IsValid, GenerateFence, MergeFence
*                  2. check ret
*/
HWTEST_F(SyncFenceTest, MultiTimeLineMerge001, Function | MediumTest | Level2)
{
    sptr<SyncTimeline> syncTimeline1_ = new SyncTimeline();
    bool valid = syncTimeline1_->IsValid();
    if (!valid) {
        ASSERT_EQ(valid, false);
        return;
    }
    sptr<SyncTimeline> syncTimeline2_ = new SyncTimeline();
    valid = syncTimeline2_->IsValid();
    if (valid) {
        ASSERT_EQ(valid, true);
        // timeline1 generate fence1 target 1 seq.no
        int32_t fd1 = syncTimeline1_->GenerateFence("test sw_sync_fence1", 1);
        sptr<SyncFence> syncFence1 = new SyncFence(fd1);
        ASSERT_GE(fd1, 0);
        // timeline2 generate fence2 target 5 seq.no
        int32_t fd2 = syncTimeline2_->GenerateFence("test sw_sync_fence2", 5);
        sptr<SyncFence> syncFence2 = new SyncFence(fd2);
        ASSERT_GE(fd2, 0);

        sptr<SyncFence> fdMerged = SyncFence::MergeFence("merged sw_fence", syncFence1, syncFence2);
        ASSERT_GE(fdMerged->Get(), 0);
        auto st = fdMerged->GetStatus();
        ASSERT_EQ(st, FenceStatus::ACTIVE);
        // timeline1 increase 1 timeline from 0 -> 1, signal fence fence1
        auto ret = syncTimeline1_->IncreaseSyncPoint(1);
        ASSERT_EQ(ret, 0);
        st = fdMerged->GetStatus();
        ASSERT_EQ(st, FenceStatus::ACTIVE);
        // timeline2 increase 5 timeline from 0 -> 5, signal fence fence2
        ret = syncTimeline2_->IncreaseSyncPoint(5);
        ASSERT_EQ(ret, 0);
        st = fdMerged->GetStatus();
        ASSERT_EQ(st, FenceStatus::SIGNALED);
    } else {
        ASSERT_EQ(valid, false);
    }
}

/*
* Function: IsValid, Dup, Get
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GenerateFence, IsValid, Dup, Get
*                  2. check ret
*/
HWTEST_F(SyncFenceTest, IsValidGetDup001, Function | MediumTest | Level2)
{
    sptr<SyncTimeline> syncTimeline = new SyncTimeline();
    bool valid = syncTimeline->IsValid();
    if (valid) {
        ASSERT_EQ(valid, true);
        int32_t fd = syncTimeline->GenerateFence("test sw_sync_fence", 1);
        SyncFence syncFence(fd);
        ASSERT_GE(fd, 0);
        bool fenceValid = syncFence.IsValid();
        ASSERT_EQ(fenceValid, true);
        int32_t getFd = syncFence.Get();
        ASSERT_EQ(getFd, fd);
        int32_t dupFenceFd = syncFence.Dup();
        ASSERT_GE(dupFenceFd, 0);
    } else {
        ASSERT_EQ(valid, false);
    }
}

/*
* Function: IsValid
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call IsValid
*                  2. check ret
*/
HWTEST_F(SyncFenceTest, IsValid002, Function | MediumTest | Level2)
{
    sptr<SyncTimeline> syncTimeline_ = new SyncTimeline();
    bool valid = syncTimeline_->IsValid();
    if (valid) {
        ASSERT_EQ(valid, true);
        int32_t fd = syncTimeline_->GenerateFence("test sw_sync_fence", 1);
        sptr<SyncFence> syncFence_ = new SyncFence(fd);
        ASSERT_GE(fd, 0);
        auto syncFenceTime_ = std::make_shared<SyncFenceTime>(syncFence_);
        ASSERT_EQ(true, syncFenceTime_->IsValid());
    } else {
        ASSERT_EQ(valid, false);
    }
}

/*
* Function: IsValid, GetSignalTimestamp
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetSignalTimestamp
*                  2. check ret
*/
HWTEST_F(SyncFenceTest, GetSignalTimestamp001, Function | MediumTest | Level2)
{
    sptr<SyncTimeline> syncTimeline_ = new SyncTimeline();
    bool valid = syncTimeline_->IsValid();
    if (valid) {
        ASSERT_EQ(valid, true);
        int32_t fd = syncTimeline_->GenerateFence("test sw_sync_fence", 1);
        sptr<SyncFence> syncFence_ = new SyncFence(fd);
        ASSERT_GE(fd, 0);
        auto syncFenceTime_ = std::make_shared<SyncFenceTime>(syncFence_);
        ns_sec_t timestamp = syncFenceTime_->GetSignalTimestamp();
        ASSERT_EQ(syncFenceTime_->GetCachedSignalTimestamp(), timestamp);
    } else {
        ASSERT_EQ(valid, false);
    }
}

/*
* Function: Push
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call Push
*                  2. check ret
*/
HWTEST_F(SyncFenceTest, Push001, Function | MediumTest | Level2)
{
    sptr<SyncTimeline> syncTimeline_ = new SyncTimeline();
    bool valid = syncTimeline_->IsValid();
    if (valid) {
        ASSERT_EQ(valid, true);
        int32_t fd = syncTimeline_->GenerateFence("test sw_sync_fence", 1);
        sptr<SyncFence> syncFence_ = new SyncFence(fd);
        ASSERT_GE(fd, 0);
        auto syncFenceTime_ = std::make_shared<SyncFenceTime>(syncFence_);
        auto syncFenceTimeline_ = std::make_shared<SyncFenceTimeline>();
        syncFenceTimeline_->Push(syncFenceTime_);
    } else {
        ASSERT_EQ(valid, false);
    }
}

/*
* Function: UpdateFenceTimeline
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call UpdateFenceTimeline
*                  2. check ret
*/
HWTEST_F(SyncFenceTest, UpdateFenceTimeline001, Function | MediumTest | Level2)
{
    sptr<SyncTimeline> syncTimeline_ = new SyncTimeline();
    bool valid = syncTimeline_->IsValid();
    if (valid) {
        ASSERT_EQ(valid, true);
        int32_t fd = syncTimeline_->GenerateFence("test sw_sync_fence", 1);
        sptr<SyncFence> syncFence_ = new SyncFence(fd);
        ASSERT_GE(fd, 0);
        auto syncFenceTime_ = std::make_shared<SyncFenceTime>(syncFence_);
        auto syncFenceTimeline_ = std::make_shared<SyncFenceTimeline>();
        syncFenceTimeline_->UpdateFenceTimeline();
        syncFenceTimeline_->Push(syncFenceTime_);
        syncFenceTimeline_->UpdateFenceTimeline();

    } else {
        ASSERT_EQ(valid, false);
    }
}

/*
* Function: SyncFenceTracker
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SyncFenceTracker
*                  2. check ret
*/
HWTEST_F(SyncFenceTest, SyncFenceTrackerTest, Function | MediumTest | Level2)
{
    auto tracker = std::make_shared<SyncFenceTracker>("test sw_sync_fence1");
}

/*
* Function: TrackFence
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call TrackFence, call Loop
*                  2. check ret
*/
HWTEST_F(SyncFenceTest, TrackFenceTest, Function | MediumTest | Level2)
{
    sptr<SyncTimeline> syncTimeline_ = new SyncTimeline();
    bool valid = syncTimeline_->IsValid();
    ASSERT_EQ(true, valid);
    int32_t fd = syncTimeline_->GenerateFence("test sw_sync_fence_timeline", 1);
    sptr<SyncFence> syncFence = new SyncFence(fd);
    ASSERT_GE(fd, 0);

    auto tracker = std::make_shared<SyncFenceTracker>("test sw_sync_fence");
    tracker->TrackFence(syncFence);

    // increase timeline from 0 -> 1
    auto ret = syncTimeline_->IncreaseSyncPoint(1);
    ASSERT_EQ(ret, 0);
    tracker->TrackFence(syncFence);
}

/*
* Function: TrackFenceTestAcquireFence
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call TrackFence, call Loop
*                  2. check ret
*/
HWTEST_F(SyncFenceTest, TrackFenceTestAcquireFence, Function | MediumTest | Level2)
{
    sptr<SyncTimeline> syncTimeline_ = new SyncTimeline();
    bool valid = syncTimeline_->IsValid();
    if (valid) {
        int32_t fd = syncTimeline_->GenerateFence("Acquire Fence", 20);
        ASSERT_GE(fd, 0);
        sptr<SyncFence> syncFence = new SyncFence(fd);

        auto ret = syncTimeline_->IncreaseSyncPoint(15);
        ASSERT_EQ(ret, 0);
        auto st = syncFence->GetStatus();
        ASSERT_EQ(st, FenceStatus::ACTIVE);

        auto tracker = std::make_shared<SyncFenceTracker>("Acquire Fence");
        tracker->TrackFence(syncFence);

        ret = syncTimeline_->IncreaseSyncPoint(15);
        ASSERT_EQ(ret, 0);
        st = syncFence->GetStatus();
        ASSERT_EQ(st, FenceStatus::SIGNALED);
    } else {
        ASSERT_EQ(valid, false);
    }
}
} // namespace OHOS