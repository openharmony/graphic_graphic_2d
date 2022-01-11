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
#include <gtest/gtest.h>
#include <display_type.h>
#include <surface.h>
#include <buffer_queue_consumer.h>
#include "buffer_consumer_listener.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class BufferQueueConsumerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
        .timeout = 0,
    };
    static inline BufferFlushConfig flushConfig = {
        .damage = {
            .w = 0x100,
            .h = 0x100,
        },
    };
    static inline int64_t timestamp = 0;
    static inline Rect damage = {};
    static inline sptr<BufferQueue> bq = nullptr;
    static inline sptr<BufferQueueConsumer> bqc = nullptr;
    static inline BufferExtraDataImpl bedata;
};

void BufferQueueConsumerTest::SetUpTestCase()
{
    bq = new BufferQueue("test");
    bq->Init();
    bqc = new BufferQueueConsumer(bq);
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    bqc->RegisterConsumerListener(listener);
}

void BufferQueueConsumerTest::TearDownTestCase()
{
    bq = nullptr;
    bqc = nullptr;
}

/*
* Function: AcquireBuffer and ReleaseBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBuffer and FlushBuffer
*                  2. call AcquireBuffer and ReleaseBuffer
*                  3. check ret
 */
HWTEST_F(BufferQueueConsumerTest, AcqRel001, Function | MediumTest | Level2)
{
    IBufferProducer::RequestBufferReturnValue retval;
    GSError ret = bq->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_GE(retval.sequence, 0);
    ASSERT_NE(retval.buffer, nullptr);

    uint8_t *addr1 = reinterpret_cast<uint8_t*>(retval.buffer->GetVirAddr());
    ASSERT_NE(addr1, nullptr);

    ret = bq->FlushBuffer(retval.sequence, bedata, -1, flushConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    sptr<SurfaceBufferImpl> bufferImpl = SurfaceBufferImpl::FromBase(retval.buffer);
    ret = bqc->AcquireBuffer(bufferImpl, retval.fence, timestamp, damage);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bqc->ReleaseBuffer(bufferImpl, -1);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: AcquireBuffer and ReleaseBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBuffer and FlushBuffer
*                  2. call AcquireBuffer and ReleaseBuffer
*                  3. call ReleaseBuffer again
*                  4. check ret
 */
HWTEST_F(BufferQueueConsumerTest, AcqRel002, Function | MediumTest | Level2)
{
    IBufferProducer::RequestBufferReturnValue retval;
    GSError ret = bq->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_GE(retval.sequence, 0);
    ASSERT_EQ(retval.buffer, nullptr);

    ret = bq->FlushBuffer(retval.sequence, bedata, -1, flushConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    sptr<SurfaceBufferImpl> bufferImpl = SurfaceBufferImpl::FromBase(retval.buffer);
    ret = bqc->AcquireBuffer(bufferImpl, retval.fence, timestamp, damage);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bqc->ReleaseBuffer(bufferImpl, -1);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bqc->ReleaseBuffer(bufferImpl, -1);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
}
}
