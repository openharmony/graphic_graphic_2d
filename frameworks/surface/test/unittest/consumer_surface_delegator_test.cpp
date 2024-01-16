/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <buffer_extra_data_impl.h>
#include <gtest/gtest.h>
#include <message_option.h>
#include <message_parcel.h>
#include <native_window.h>
#include <securec.h>
#include <surface.h>

#include "buffer_queue_producer.h"
#include "consumer_surface.h"
#include "consumer_surface_delegator.h"
#include "sync_fence.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class ConsumerSurfaceDelegatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline sptr<ConsumerSurfaceDelegator> consumerDelegator = nullptr;
    static inline sptr<BufferExtraData> bedata = nullptr;
    static inline sptr<SurfaceBuffer> buffer = nullptr;
    static inline sptr<BufferQueue> bq = nullptr;

    static inline BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
};

void ConsumerSurfaceDelegatorTest::SetUpTestCase()
{
    consumerDelegator = ConsumerSurfaceDelegator::Create();
    bedata = new OHOS::BufferExtraDataImpl;
    buffer = SurfaceBuffer::Create();
    bq = new BufferQueue("test");
}

void ConsumerSurfaceDelegatorTest::TearDownTestCase()
{
    consumerDelegator = nullptr;
    bedata = nullptr;
    buffer = nullptr;
    bq = nullptr;
}

/*
* Function: DequeueBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call DequeueBuffer
*                  2. check ret
 */
HWTEST_F(ConsumerSurfaceDelegatorTest, DequeueBuffer001, Function | MediumTest | Level2)
{
    IBufferProducer::RequestBufferReturnValue retval;
    GSError ret = consumerDelegator->DequeueBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, GSERROR_OK);
}


/*
* Function: QueueBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call QueueBuffer
*                  2. check ret
 */
HWTEST_F(ConsumerSurfaceDelegatorTest, QueueBuffer001, Function | MediumTest | Level2)
{
    int32_t fenceFd = 3;
    GSError ret = consumerDelegator->QueueBuffer(buffer, fenceFd);
    ASSERT_EQ(ret, GSERROR_OK);
}

/*
* Function: ReleaseBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call ReleaseBuffer
*                  2. check ret
 */
HWTEST_F(ConsumerSurfaceDelegatorTest, ReleaseBuffer001, Function | MediumTest | Level2)
{
    int slot = 0;
    int32_t releaseFenceFd = 3;
    GSError ret = consumerDelegator->ReleaseBuffer(slot, releaseFenceFd);
    ASSERT_EQ(ret, GSERROR_OK);
}

/*
* Function: CancelBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call CancelBuffer
*                  2. check ret
 */
HWTEST_F(ConsumerSurfaceDelegatorTest, CancelBuffer001, Function | MediumTest | Level2)
{
    GSError ret = consumerDelegator->CancelBuffer(buffer);
    ASSERT_EQ(ret, GSERROR_OK);
}

/*
* Function: DetachBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call DetachBuffer
*                  2. check ret
 */
HWTEST_F(ConsumerSurfaceDelegatorTest, DetachBuffer001, Function | MediumTest | Level2)
{
    GSError ret = consumerDelegator->DetachBuffer(buffer);
    ASSERT_EQ(ret, GSERROR_OK);
}

/*
* Function: SetBufferQueue
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetBufferQueue
*                  2. check ret
 */
HWTEST_F(ConsumerSurfaceDelegatorTest, SetBufferQueue001, Function | MediumTest | Level2)
{
    bool ret = consumerDelegator->SetBufferQueue(bq);
    ASSERT_EQ(ret, true);
}

/*
* Function: OnRemoteRequest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OnRemoteRequest
*                  2. check ret
 */
HWTEST_F(ConsumerSurfaceDelegatorTest, OnRemoteRequest001, Function | MediumTest | Level2)
{
    uint32_t code = 1; // QUEUEBUFFER
    MessageParcel reply;
    MessageOption option;
    MessageParcel dataQueue;
    dataQueue.WriteInt32(10);
    dataQueue.WriteFileDescriptor(20);
    int ret = consumerDelegator->OnRemoteRequest(code, dataQueue, reply, option);
    ASSERT_EQ(ret, ERR_NONE);
}
}
