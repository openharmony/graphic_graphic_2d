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

#include "buffer_queue_producer_remote_test.h"

#include "buffer_consumer_listener.h"

namespace OHOS {
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

namespace {
HWTEST_F(BufferQueueProducerRemoteTest, IsProxy, testing::ext::TestSize.Level0)
{
    ASSERT_FALSE(robj->IsProxyObject());
}

HWTEST_F(BufferQueueProducerRemoteTest, QueueSize, testing::ext::TestSize.Level0)
{
    SurfaceError ret = bp->SetQueueSize(2);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bp->SetQueueSize(SURFACE_MAX_QUEUE_SIZE + 1);
    ASSERT_NE(ret, SURFACE_ERROR_OK);

    ASSERT_EQ(bp->GetQueueSize(), 2u);
}

HWTEST_F(BufferQueueProducerRemoteTest, ReqCan, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval;
    SurfaceError ret = bp->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bp->CancelBuffer(retval.sequence, bedata);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    sptr<SurfaceBufferImpl> bufferImpl = static_cast<SurfaceBufferImpl*>(retval.buffer.GetRefPtr());
    ret = bq->AcquireBuffer(bufferImpl, retval.fence, timestamp, damage);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}

HWTEST_F(BufferQueueProducerRemoteTest, ReqCanCan, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval;
    SurfaceError ret = bp->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bp->CancelBuffer(retval.sequence, bedata);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bp->CancelBuffer(retval.sequence, bedata);
    ASSERT_NE(ret, SURFACE_ERROR_OK);

    sptr<SurfaceBufferImpl> bufferImpl = static_cast<SurfaceBufferImpl*>(retval.buffer.GetRefPtr());
    ret = bq->AcquireBuffer(bufferImpl, retval.fence, timestamp, damage);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}

HWTEST_F(BufferQueueProducerRemoteTest, ReqReqReqCanCan, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval1;
    IBufferProducer::RequestBufferReturnValue retval2;
    IBufferProducer::RequestBufferReturnValue retval3;
    SurfaceError ret;

    ret = bp->RequestBuffer(requestConfig, bedata, retval1);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_NE(retval1.buffer, nullptr);

    ret = bp->RequestBuffer(requestConfig, bedata, retval2);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_NE(retval2.buffer, nullptr);

    ret = bp->RequestBuffer(requestConfig, bedata, retval3);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
    ASSERT_EQ(retval3.buffer, nullptr);

    ret = bp->CancelBuffer(retval1.sequence, bedata);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bp->CancelBuffer(retval2.sequence, bedata);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bp->CancelBuffer(retval3.sequence, bedata);
    ASSERT_NE(ret, SURFACE_ERROR_OK);

    sptr<SurfaceBufferImpl> bufferImpl = static_cast<SurfaceBufferImpl*>(retval1.buffer.GetRefPtr());
    ret = bq->AcquireBuffer(bufferImpl, retval1.fence, timestamp, damage);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}

HWTEST_F(BufferQueueProducerRemoteTest, SetQueueSizeDeleting, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;

    sptr<BufferQueueProducer> bqp = static_cast<BufferQueueProducer*>(bp.GetRefPtr());
    ASSERT_EQ(bqp->bufferQueue_->freeList_.size(), 2u);

    SurfaceError ret = bp->SetQueueSize(1);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_EQ(bqp->bufferQueue_->freeList_.size(), 1u);

    ret = bp->SetQueueSize(2);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_EQ(bqp->bufferQueue_->freeList_.size(), 1u);
}

HWTEST_F(BufferQueueProducerRemoteTest, ReqFlu, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval;
    SurfaceError ret = bp->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bp->FlushBuffer(retval.sequence, bedata, -1, flushConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    sptr<SurfaceBufferImpl> bufferImpl = static_cast<SurfaceBufferImpl*>(retval.buffer.GetRefPtr());
    ret = bq->AcquireBuffer(bufferImpl, retval.fence, timestamp, damage);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bq->ReleaseBuffer(bufferImpl, -1);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bq->AcquireBuffer(bufferImpl, retval.fence, timestamp, damage);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}

HWTEST_F(BufferQueueProducerRemoteTest, ReqFluFlu, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval;
    SurfaceError ret = bp->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bp->FlushBuffer(retval.sequence, bedata, -1, flushConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bp->FlushBuffer(retval.sequence, bedata, -1, flushConfig);
    ASSERT_NE(ret, SURFACE_ERROR_OK);

    sptr<SurfaceBufferImpl> bufferImpl = static_cast<SurfaceBufferImpl*>(retval.buffer.GetRefPtr());
    ret = bq->AcquireBuffer(bufferImpl, retval.fence, timestamp, damage);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bq->ReleaseBuffer(bufferImpl, -1);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bq->AcquireBuffer(bufferImpl, retval.fence, timestamp, damage);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}
}
} // namespace OHOS
