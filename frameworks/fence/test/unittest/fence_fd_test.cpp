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
    csurf = nullptr;
    producer = nullptr;
    psurf = nullptr;
}

void FenceFdTest::OnBufferAvailable()
{
}

namespace {
HWTEST_F(FenceFdTest, BufferQueueFenceItem, testing::ext::TestSize.Level0) {
    PART("EnvConditions") {
        STEP("surf create success.") {
            csurf = Surface::CreateSurfaceAsConsumer();
            STEP_ASSERT_NE(csurf, nullptr);
            csurf->RegisterConsumerListener(this);
            producer = csurf->GetProducer();
            STEP_ASSERT_NE(producer, nullptr);
            psurf = Surface::CreateSurfaceAsProducer(producer);
            STEP_ASSERT_NE(psurf, nullptr);
        }
    }

    PART("CaseDescription") {
        sptr<SurfaceBuffer> buffer = nullptr;
        int32_t releaseFence = 0;
        GSError ret = GSERROR_INTERNEL;

        STEP("1. Check release fence fd") {
            ret = psurf->RequestBuffer(buffer, releaseFence, requestConfig);
            STEP_ASSERT_EQ(ret, GSERROR_OK);
            STEP_ASSERT_EQ(releaseFence, -1);
            STEP_ASSERT_NE(buffer, nullptr);
        }

        STEP("2. Check acquire fence from FlushBuffer to AcquireBuffer") {
            int32_t acquireFence = 1;
            ret = psurf->FlushBuffer(buffer, acquireFence, flushConfig);
            STEP_ASSERT_EQ(ret, GSERROR_OK);

            int32_t outAcquireFence = 0;
            ret = csurf->AcquireBuffer(buffer, outAcquireFence, timestamp, damage);
            STEP_ASSERT_EQ(ret, GSERROR_OK);
            STEP_ASSERT_EQ(outAcquireFence, acquireFence);
        }

        STEP("3. Check this release fence and the release fence of the next RequestBuffer") {
            int32_t newReleaseFence = 2;
            ret = csurf->ReleaseBuffer(buffer, newReleaseFence);
            STEP_ASSERT_EQ(ret, GSERROR_OK);

            int32_t outReleaseFence = 0;
            ret = psurf->RequestBuffer(buffer, outReleaseFence, requestConfig);
            STEP_ASSERT_EQ(ret, GSERROR_OK);
            STEP_ASSERT_NE(buffer, nullptr);
            STEP_ASSERT_EQ(outReleaseFence, newReleaseFence);
        }
    }
}
} // namespace
} // namespace OHOS
