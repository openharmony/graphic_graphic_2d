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

#include "buffer_queue_consumer_test.h"

#include "buffer_consumer_listener.h"

namespace OHOS {
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

namespace {
HWTEST_F(BufferQueueConsumerTest, AcqRel, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval;
    GSError ret = bq->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, GSERROR_OK);
    ASSERT_GE(retval.sequence, 0);
    ASSERT_NE(retval.buffer, nullptr);

    uint8_t *addr1 = reinterpret_cast<uint8_t*>(retval.buffer->GetVirAddr());
    ASSERT_NE(addr1, nullptr);

    ret = bq->FlushBuffer(retval.sequence, bedata, -1, flushConfig);
    ASSERT_EQ(ret, GSERROR_OK);

    sptr<SurfaceBufferImpl> bufferImpl = SurfaceBufferImpl::FromBase(retval.buffer);
    ret = bqc->AcquireBuffer(bufferImpl, retval.fence, timestamp, damage);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = bqc->ReleaseBuffer(bufferImpl, -1);
    ASSERT_EQ(ret, GSERROR_OK);
}

HWTEST_F(BufferQueueConsumerTest, AcqRelRel, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval;
    GSError ret = bq->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, GSERROR_OK);
    ASSERT_GE(retval.sequence, 0);
    ASSERT_EQ(retval.buffer, nullptr);

    ret = bq->FlushBuffer(retval.sequence, bedata, -1, flushConfig);
    ASSERT_EQ(ret, GSERROR_OK);

    sptr<SurfaceBufferImpl> bufferImpl = SurfaceBufferImpl::FromBase(retval.buffer);
    ret = bqc->AcquireBuffer(bufferImpl, retval.fence, timestamp, damage);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = bqc->ReleaseBuffer(bufferImpl, -1);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = bqc->ReleaseBuffer(bufferImpl, -1);
    ASSERT_NE(ret, GSERROR_OK);
}
}
} // namespace OHOS
