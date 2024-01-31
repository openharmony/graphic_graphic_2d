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

#include <map>
#include <gtest/gtest.h>

#include <surface.h>
#include "buffer_extra_data_impl.h"
#include "buffer_queue.h"
#include "buffer_consumer_listener.h"
#include "buffer_log.h"
#include "iconsumer_surface.h"
#include "test_header.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class BufferSharedTest : public testing::Test, public IBufferConsumerListenerClazz {
public:
    static void SetUpTestCase();
    void OnBufferAvailable() override;
    static void TearDownTestCase();

    static inline sptr<IConsumerSurface> surf = nullptr;
    static inline sptr<Surface> producerSurface1 = nullptr;
    static inline sptr<Surface> producerSurface2 = nullptr;
    static inline sptr<SurfaceBuffer> buffer1 = nullptr;
    static inline sptr<SurfaceBuffer> buffer2 = nullptr;
    static inline sptr<SurfaceBuffer> sbuffer1 = nullptr;
    static inline sptr<SurfaceBuffer> sbuffer2 = nullptr;
};

void BufferSharedTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << getpid() << std::endl;
    surf = IConsumerSurface::Create("shared", true);
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    surf->RegisterConsumerListener(listener);
    auto producer1 = surf->GetProducer();
    producerSurface1 = Surface::CreateSurfaceAsProducer(producer1);
    auto producer2 = surf->GetProducer();
    producerSurface2 = Surface::CreateSurfaceAsProducer(producer2);
}

void BufferSharedTest::OnBufferAvailable() {}

void BufferSharedTest::TearDownTestCase()
{
}

/*
* Function: RequestBuffer
* Type: Reliability
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBuffer with buffer=buffer1, buffer2，the param is same
*                  2. check ret1 and ret2 are OHOS::GSERROR_OK, check buffer1 and buffer2 is not nullptr
*                  3. check the addr of buffer1 EQ buffer2
* */

HWTEST_F(BufferSharedTest, RequestBuffer001, Function | MediumTest | Level2)
{
    PART("REQUEST BUFFER TWO TIMES") {
        GSError ret1, ret2;
        STEP("1: request buffer") {
            BufferRequestConfig requestConfig = {
                .width = 0x100,
                .height = 0x100,
                .strideAlignment = 0x8,
                .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
                .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
                .timeout = 0,
            };
            int releaseFence = -1;
            ret1 = producerSurface1->RequestBuffer(buffer1, releaseFence, requestConfig);
            ret2 = producerSurface2->RequestBuffer(buffer2, releaseFence, requestConfig);
        }
        STEP("2: check ret1 ret2 buffer1 buffer2") {
            STEP_ASSERT_EQ(ret1, OHOS::GSERROR_OK);
            STEP_ASSERT_NE(buffer1, nullptr);
            STEP_ASSERT_EQ(ret2, OHOS::GSERROR_OK);
            STEP_ASSERT_NE(buffer2, nullptr);
        }
        STEP("3: check buffer addr") {
            STEP_ASSERT_EQ(buffer2, buffer1);
        }
    }
}
/*
* Function: RequestBuffer with different requestconfig
* Type: Reliability
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBuffer with buffer=bufferDiff,
*                     the requestconfig is not same with buffer1
*                  2. check ret1 is GSERROR_INVALID_ARGUMENTS
* */

HWTEST_F(BufferSharedTest, RequestBufferDiff001, Function | MediumTest | Level2)
{
    PART("REQUEST BUFFER with different requestconfig") {
        GSError ret1;
        sptr<SurfaceBuffer> bufferDiff = nullptr;
        STEP("1: request buffer") {
            BufferRequestConfig diffRequestConfig = {
                .width = 0x200,
                .height = 0x100,
                .strideAlignment = 0x8,
                .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
                .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
                .timeout = 0,
            };
            int releaseFence = -1;
            ret1 = producerSurface1->RequestBuffer(bufferDiff, releaseFence, diffRequestConfig);
        }
        STEP("2: check ret1") {
            STEP_ASSERT_EQ(ret1, GSERROR_INVALID_ARGUMENTS);
        }
    }
}
/*
* Function: FlushBuffer
* Type: Reliability
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call FlushBuffer with buffer=buffer1, buffer2
*                  2. check ret1 and ret2 is OHOS::GSERROR_OK
* */
HWTEST_F(BufferSharedTest, FlushBuffer001,  Function | MediumTest | Level2)
{
    PART("FlushBuffer") {
        GSError ret1, ret2;
        STEP("1: FlushBuffer two times") {
            BufferFlushConfig flushConfig = { .damage = { .w = 0x100, .h = 0x100, }, };
            ret1 = producerSurface1->FlushBuffer(buffer1, -1, flushConfig);
            ret2 = producerSurface2->FlushBuffer(buffer2, -1, flushConfig);
        }
        STEP("2: check ret1 ret2") {
            STEP_ASSERT_EQ(ret1, OHOS::GSERROR_OK);
            STEP_ASSERT_EQ(ret2, OHOS::GSERROR_OK);
        }
    }
}
/*
* Function: AquiredBuffer
* Type: Reliability
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call AcquireBuffer with buffer=sbuffer1, sbuffer2
*                  2. check ret1 and ret2 are GSERROR_INVALID_ARGUMENTS
* */
HWTEST_F(BufferSharedTest, AquiredBuffer001, Function | MediumTest | Level2)
{
    PART("AquiredBuffer") {
        GSError ret1, ret2;
        STEP("1: AcquireBuffer two times") {
            int64_t timestamp = 0;
            Rect damage = {};
            int32_t fence = -1;

            ret1 = surf->AcquireBuffer(sbuffer1, fence, timestamp, damage);
            ret2 = surf->AcquireBuffer(sbuffer2, fence, timestamp, damage);
        }
        STEP("2: check ret1 ret2") {
            STEP_ASSERT_EQ(ret1, OHOS::GSERROR_OK);
            STEP_ASSERT_EQ(ret2, OHOS::GSERROR_OK);
        }
        STEP("3: check addr sbuffer1 and sbuffer2") {
            STEP_ASSERT_EQ(sbuffer1, sbuffer2);
        }
    }
}
/*
* Function: CancelBuffer
* Type: Reliability
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call cancelBuffer with buffer=buffer1
*                  2. check ret1 is GSERROR_INVALID_OPERATING
*                  3. call cancelBuffer with buffer=buffer2
*                  4. check ret2 is GSERROR_INVALID_OPERATING
* */
HWTEST_F(BufferSharedTest, CancelBuffer001, Function | MediumTest | Level2)
{
    PART("CancelBuffer") {
        GSError ret1, ret2;
        STEP("1: Cancel buffer1") {
            ret1 = producerSurface1->CancelBuffer(buffer1);
        }
        STEP("2: check ret1") {
            STEP_ASSERT_EQ(ret1, GSERROR_INVALID_OPERATING);
        }
        STEP("3: Cancel buffer2") {
            ret2 = producerSurface2->CancelBuffer(buffer2);
        }
        STEP("4: check ret2") {
            STEP_ASSERT_EQ(ret2, GSERROR_INVALID_OPERATING);
        }
    }
}
/*
* Function: RelaseBuffer
* Type: Reliability
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. releaseBuffer two times
*                  2. check ret1 is GSERROR_INVALID_OPERATING, check ret1 is OHOS::GSERROR_OK
* */
HWTEST_F(BufferSharedTest, ReleaseBuffer001, Function | MediumTest | Level2)
{
    PART("ReleaseBuffer") {
        GSError ret1, ret2;
        STEP("1: releaseBuffer two times") {
            ret1 = surf->ReleaseBuffer(sbuffer1, -1);
            ret2 = surf->ReleaseBuffer(sbuffer2, -1);
        }
        STEP("2: check ret1, ret2") {
            STEP_ASSERT_EQ(ret1, OHOS::GSERROR_OK);
            STEP_ASSERT_EQ(ret2, OHOS::GSERROR_OK);
        }
    }
}
}
