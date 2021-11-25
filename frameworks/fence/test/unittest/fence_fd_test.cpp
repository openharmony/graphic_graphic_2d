/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "fence_fd_test.h"

#include <chrono>
#include <sys/stat.h>
#include <unistd.h>
#include <thread>

#include <linux/sync_file.h>
#include "test_header.h"

namespace OHOS {
void FenceFdTest::SetUpTestCase()
{
}

void FenceFdTest::TearDownTestCase()
{
    csurface = nullptr;
    producer = nullptr;
    psurface = nullptr;
}

void FenceFdTest::OnBufferAvailable()
{
}

namespace {
HWTEST_F(FenceFdTest, BufferQueueFenceItem, testing::ext::TestSize.Level0) {
    PART("EnvConditions") {
        STEP("surface create success.") {
            csurface = Surface::CreateSurfaceAsConsumer();
            STEP_ASSERT_NE(csurface, nullptr);
            csurface->RegisterConsumerListener(this);
            producer = csurface->GetProducer();
            STEP_ASSERT_NE(producer, nullptr);
            psurface = Surface::CreateSurfaceAsProducer(producer);
            STEP_ASSERT_NE(psurface, nullptr);
        }
    }

    PART("CaseDescription") {
        sptr<SurfaceBuffer> buffer = nullptr;
        int32_t releaseFence = 0;
        SurfaceError ret = SURFACE_ERROR_ERROR;

        STEP("1. Check release fence fd") {
            ret = psurface->RequestBuffer(buffer, releaseFence, requestConfig);
            STEP_ASSERT_EQ(ret, SURFACE_ERROR_OK);
            STEP_ASSERT_EQ(releaseFence, -1);
            STEP_ASSERT_NE(buffer, nullptr);
        }

        STEP("2. Check acquire fence from FlushBuffer to AcquireBuffer") {
            int32_t acquireFence = 1;
            ret = psurface->FlushBuffer(buffer, acquireFence, flushConfig);
            STEP_ASSERT_EQ(ret, SURFACE_ERROR_OK);

            int32_t outAcquireFence = 0;
            ret = csurface->AcquireBuffer(buffer, outAcquireFence, timestamp, damage);
            STEP_ASSERT_EQ(ret, SURFACE_ERROR_OK);
            STEP_ASSERT_EQ(outAcquireFence, acquireFence);
        }

        STEP("3. Check this release fence and the release fence of the next RequestBuffer") {
            int32_t newReleaseFence = 2;
            ret = csurface->ReleaseBuffer(buffer, newReleaseFence);
            STEP_ASSERT_EQ(ret, SURFACE_ERROR_OK);

            int32_t outReleaseFence = 0;
            ret = psurface->RequestBuffer(buffer, outReleaseFence, requestConfig);
            STEP_ASSERT_EQ(ret, SURFACE_ERROR_OK);
            STEP_ASSERT_NE(buffer, nullptr);
            STEP_ASSERT_EQ(outReleaseFence, newReleaseFence);
        }
    }
}

HWTEST_F(FenceFdTest, FenceHoldTest, testing::ext::TestSize.Level0) {
    PART("EnvConditions") {
        STEP("Support fence success") {
            STEP_ASSERT_EQ(IsSupportSwSync(), true);
        }
    }

    PART("CaseDescription") {
        int32_t ret = 0;
        int32_t status = 0;
        int32_t fenceFd = -1;
        int32_t timeline = 0;

        STEP("1. Invalid fd") {
            ret = FenceHold(-1, 1 * 1000);
            STEP_ASSERT_EQ(ret, -1);
        }

        STEP("2. Create timeline") {
            timeline = CreateTimeline();
            STEP_ASSERT_NE(timeline, -1);
        }

        STEP("3. Create fence fd") {
            fenceFd = CreateFenceFromTimeline(timeline, "createFence", 2);
            status = fcntl(fenceFd, F_GETFD, 0);
            STEP_ASSERT_GE(status, 0);
        }

        STEP("4. Hold and active") {
            std::thread t([] (int32_t fd, int32_t timeline) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                TimelineActivate(timeline, 1);
                std::this_thread::sleep_for(std::chrono::seconds(1));
                TimelineActivate(timeline, 1);
            }, fenceFd, timeline);

            ret = FenceHold(fenceFd, -1);
            STEP_ASSERT_GT(ret, 0);
            STEP_ASSERT_EQ(FenceGetStatus(fenceFd), SIGNALED);

            t.join();
            close(fenceFd);
            close(timeline);
        }
    }
}

HWTEST_F(FenceFdTest, FenceMergeTest, testing::ext::TestSize.Level0) {
    PART("EnvConditions") {
        STEP("Support fence success") {
            STEP_ASSERT_EQ(IsSupportSwSync(), true);
        }
    }

    PART("CaseDescription") {
        int32_t ret = 0;
        int32_t status = 0;
        int32_t fenceFd = -1;
        int32_t timeline = 0;

        STEP("1. Invalid fence fd") {
            ret = FenceMerge("merge", -1, -1);
            STEP_ASSERT_EQ(ret, -1);
        }

        STEP("2. Merge fence") {
            timeline = CreateTimeline();
            STEP_ASSERT_NE(timeline, -1);

            fenceFd = CreateFenceFromTimeline(timeline, "createFence", 2);
            status = fcntl(fenceFd, F_GETFD, 0);
            STEP_ASSERT_GE(status, 0);

            int32_t fenceMerged = FenceMerge("merge", fenceFd, fenceFd);
            status = fcntl(fenceMerged, F_GETFD, 0);
            STEP_ASSERT_GE(status, 0);
            close(fenceMerged);
        }
        close(fenceFd);
        close(timeline);
    }
}
}
} // namespace OHOS
