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

#include "buffer_queue_producer_test.h"

#include "buffer_consumer_listener.h"

using namespace OHOS;

namespace OHOS {
void BufferQueueProducerTest::SetUpTestCase()
{
    bq = new BufferQueue("test");
    bq->Init();
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    bq->RegisterConsumerListener(listener);
    bqp = new BufferQueueProducer(bq);
}

void BufferQueueProducerTest::TearDownTestCase()
{
    bq = nullptr;
    bqp = nullptr;
}

namespace {
HWTEST_F(BufferQueueProducerTest, QueueSize, testing::ext::TestSize.Level0)
{
    ASSERT_EQ(bqp->GetQueueSize(), (uint32_t)SURFACE_DEFAULT_QUEUE_SIZE);

    GSError ret = bqp->SetQueueSize(2);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = bqp->SetQueueSize(SURFACE_MAX_QUEUE_SIZE + 1);
    ASSERT_NE(ret, GSERROR_OK);

    ASSERT_EQ(bqp->GetQueueSize(), 2u);
    ASSERT_EQ(bq->queueSize_, 2u);
}

HWTEST_F(BufferQueueProducerTest, ReqCan, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval;
    GSError ret = bqp->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = bqp->CancelBuffer(retval.sequence, bedata);
    ASSERT_EQ(ret, GSERROR_OK);
}

HWTEST_F(BufferQueueProducerTest, ReqCanCan, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval;
    GSError ret = bqp->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = bqp->CancelBuffer(retval.sequence, bedata);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = bqp->CancelBuffer(retval.sequence, bedata);
    ASSERT_NE(ret, GSERROR_OK);
}

HWTEST_F(BufferQueueProducerTest, ReqReqReqCanCan, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval1;
    IBufferProducer::RequestBufferReturnValue retval2;
    IBufferProducer::RequestBufferReturnValue retval3;

    auto ret = bqp->RequestBuffer(requestConfig, bedata, retval1);
    ASSERT_EQ(ret, GSERROR_OK);
    ASSERT_NE(retval1.buffer, nullptr);

    ret = bqp->RequestBuffer(requestConfig, bedata, retval2);
    ASSERT_EQ(ret, GSERROR_OK);
    ASSERT_NE(retval2.buffer, nullptr);

    ret = bqp->RequestBuffer(requestConfig, bedata, retval3);
    ASSERT_NE(ret, GSERROR_OK);
    ASSERT_EQ(retval3.buffer, nullptr);

    ret = bqp->CancelBuffer(retval1.sequence, bedata);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = bqp->CancelBuffer(retval2.sequence, bedata);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = bqp->CancelBuffer(retval3.sequence, bedata);
    ASSERT_NE(ret, GSERROR_OK);
}

HWTEST_F(BufferQueueProducerTest, ReqFlu, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval;
    GSError ret = bqp->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = bqp->FlushBuffer(retval.sequence, bedata, -1, flushConfig);
    ASSERT_EQ(ret, GSERROR_OK);

    sptr<SurfaceBufferImpl> bufferImpl = SurfaceBufferImpl::FromBase(retval.buffer);
    ret = bq->AcquireBuffer(bufferImpl, retval.fence, timestamp, damage);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = bq->ReleaseBuffer(bufferImpl, -1);
    ASSERT_EQ(ret, GSERROR_OK);
}

HWTEST_F(BufferQueueProducerTest, ReqFluFlu, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval;
    GSError ret = bqp->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = bqp->FlushBuffer(retval.sequence, bedata, -1, flushConfig);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = bqp->FlushBuffer(retval.sequence, bedata, -1, flushConfig);
    ASSERT_NE(ret, GSERROR_OK);

    sptr<SurfaceBufferImpl> bufferImpl = SurfaceBufferImpl::FromBase(retval.buffer);
    ret = bq->AcquireBuffer(bufferImpl, retval.fence, timestamp, damage);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = bq->ReleaseBuffer(bufferImpl, -1);
    ASSERT_EQ(ret, GSERROR_OK);
}
}
} // namespace OHOS
