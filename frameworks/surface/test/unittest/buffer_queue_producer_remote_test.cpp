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
#include <iservice_registry.h>
#include <display_type.h>
#include <surface.h>
#include <buffer_extra_data_impl.h>
#include <buffer_queue_producer.h>
#include "buffer_consumer_listener.h"
#include "sync_fence.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class BufferQueueProducerRemoteTest : public testing::Test {
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
    static inline std::vector<int32_t> deletingBuffers;
    static inline int64_t timestamp = 0;
    static inline Rect damage = {};
    static inline sptr<IRemoteObject> robj = nullptr;
    static inline sptr<IBufferProducer> bp = nullptr;
    static inline sptr<BufferQueue> bq = nullptr;
    static inline sptr<BufferQueueProducer> bqp = nullptr;
    static inline sptr<BufferExtraData> bedata = nullptr;
    static inline int32_t systemAbilityID = 345154;
};

void BufferQueueProducerRemoteTest::SetUpTestCase()
{
    bq = new BufferQueue("test");
    bqp = new BufferQueueProducer(bq);
    bq->Init();
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    bq->RegisterConsumerListener(listener);

    auto sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sm->AddSystemAbility(systemAbilityID, bqp);

    robj = sm->GetSystemAbility(systemAbilityID);
    bp = iface_cast<IBufferProducer>(robj);

    bedata = new OHOS::BufferExtraDataImpl;
}

void BufferQueueProducerRemoteTest::TearDownTestCase()
{
    bp = nullptr;
    robj = nullptr;

    auto sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sm->RemoveSystemAbility(systemAbilityID);

    bqp = nullptr;
    bq = nullptr;
}

/*
* Function: IsProxyObject
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call IsProxyObject and check ret
 */
HWTEST_F(BufferQueueProducerRemoteTest, IsProxy001, Function | MediumTest | Level2)
{
    ASSERT_FALSE(robj->IsProxyObject());
}

/*
* Function: SetQueueSize and GetQueueSize
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetQueueSize
*                  2. call SetQueueSize again with abnormal input
*                  3. check ret and call GetQueueSize
 */
HWTEST_F(BufferQueueProducerRemoteTest, QueueSize001, Function | MediumTest | Level2)
{
    GSError ret = bp->SetQueueSize(2);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bp->SetQueueSize(SURFACE_MAX_QUEUE_SIZE + 1);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ASSERT_EQ(bp->GetQueueSize(), 2u);
}

/*
* Function: RequestBuffer, CancelBuffer and AcquireBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBuffer
*                  2. call CancelBuffer
*                  3. call AcquireBuffer and check ret
 */
HWTEST_F(BufferQueueProducerRemoteTest, ReqCan001, Function | MediumTest | Level2)
{
    IBufferProducer::RequestBufferReturnValue retval;
    GSError ret = bp->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bp->CancelBuffer(retval.sequence, bedata);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bq->AcquireBuffer(retval.buffer, retval.fence, timestamp, damage);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
}

/*
* Function: RequestBuffer, CancelBuffer and AcquireBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBuffer
*                  2. call CancelBuffer
*                  3. call CancelBuffer again
*                  4. call AcquireBuffer and check ret
 */
HWTEST_F(BufferQueueProducerRemoteTest, ReqCan002, Function | MediumTest | Level2)
{
    IBufferProducer::RequestBufferReturnValue retval;
    GSError ret = bp->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bp->CancelBuffer(retval.sequence, bedata);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bp->CancelBuffer(retval.sequence, bedata);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ret = bq->AcquireBuffer(retval.buffer, retval.fence, timestamp, damage);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
}

/*
* Function: RequestBuffer, CancelBuffer and AcquireBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBuffer and CancelBuffer by different retval
*                  2. call AcquireBuffer and check ret
 */
HWTEST_F(BufferQueueProducerRemoteTest, ReqCan003, Function | MediumTest | Level2)
{
    IBufferProducer::RequestBufferReturnValue retval1;
    IBufferProducer::RequestBufferReturnValue retval2;
    IBufferProducer::RequestBufferReturnValue retval3;
    GSError ret;

    ret = bp->RequestBuffer(requestConfig, bedata, retval1);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(retval1.buffer, nullptr);

    ret = bp->RequestBuffer(requestConfig, bedata, retval2);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(retval2.buffer, nullptr);

    ret = bp->RequestBuffer(requestConfig, bedata, retval3);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
    ASSERT_EQ(retval3.buffer, nullptr);

    ret = bp->CancelBuffer(retval1.sequence, bedata);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bp->CancelBuffer(retval2.sequence, bedata);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bp->CancelBuffer(retval3.sequence, bedata);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ret = bq->AcquireBuffer(retval1.buffer, retval1.fence, timestamp, damage);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
}

/*
* Function: RequestBuffer, FlushBuffer, AcquireBuffer and ReleaseBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBuffer and FlushBuffer
*                  2. call AcquireBuffer and ReleaseBuffer
*                  3. call AcquireBuffer again
*                  4. check ret
 */
HWTEST_F(BufferQueueProducerRemoteTest, ReqFlu001, Function | MediumTest | Level2)
{
    IBufferProducer::RequestBufferReturnValue retval;
    GSError ret = bp->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    ret = bp->FlushBuffer(retval.sequence, bedata, acquireFence, flushConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bq->AcquireBuffer(retval.buffer, retval.fence, timestamp, damage);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    sptr<SyncFence> releaseFence = SyncFence::INVALID_FENCE;
    ret = bq->ReleaseBuffer(retval.buffer, releaseFence);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bq->AcquireBuffer(retval.buffer, retval.fence, timestamp, damage);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
}

/*
* Function: RequestBuffer, FlushBuffer, AcquireBuffer and ReleaseBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBuffer and FlushBuffer
*                  2. call FlushBuffer again
*                  3. call AcquireBuffer and ReleaseBuffer
*                  4. call AcquireBuffer again
*                  5. check ret
 */
HWTEST_F(BufferQueueProducerRemoteTest, ReqFlu002, Function | MediumTest | Level2)
{
    IBufferProducer::RequestBufferReturnValue retval;
    GSError ret = bp->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    ret = bp->FlushBuffer(retval.sequence, bedata, acquireFence, flushConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bp->FlushBuffer(retval.sequence, bedata, acquireFence, flushConfig);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ret = bq->AcquireBuffer(retval.buffer, retval.fence, timestamp, damage);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    sptr<SyncFence> releaseFence = SyncFence::INVALID_FENCE;
    ret = bq->ReleaseBuffer(retval.buffer, releaseFence);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bq->AcquireBuffer(retval.buffer, retval.fence, timestamp, damage);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
}
}
