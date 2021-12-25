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

#include "producer_surface_test.h"

#include <securec.h>

#include "buffer_consumer_listener.h"
#include "consumer_surface.h"

namespace OHOS {
void ProducerSurfaceTest::SetUpTestCase()
{
    csurf = Surface::CreateSurfaceAsConsumer();
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    csurf->RegisterConsumerListener(listener);
    producer = csurf->GetProducer();
    psurf = Surface::CreateSurfaceAsProducer(producer);
}

void ProducerSurfaceTest::TearDownTestCase()
{
    csurf = nullptr;
    producer = nullptr;
    psurf = nullptr;
}

namespace {
HWTEST_F(ProducerSurfaceTest, ProducerSurface, testing::ext::TestSize.Level0)
{
    ASSERT_NE(psurf, nullptr);
}

HWTEST_F(ProducerSurfaceTest, QueueSize, testing::ext::TestSize.Level0)
{
    ASSERT_EQ(psurf->GetQueueSize(), (uint32_t)SURFACE_DEFAULT_QUEUE_SIZE);
    GSError ret = psurf->SetQueueSize(2);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = psurf->SetQueueSize(SURFACE_MAX_QUEUE_SIZE + 1);
    ASSERT_NE(ret, GSERROR_OK);

    ASSERT_EQ(psurf->GetQueueSize(), 2u);
}

HWTEST_F(ProducerSurfaceTest, ReqFlu, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;

    GSError ret = psurf->RequestBufferNoFence(buffer, requestConfig);
    ASSERT_EQ(ret, GSERROR_OK);
    ASSERT_NE(buffer, nullptr);

    ret = psurf->FlushBuffer(buffer, -1, flushConfig);
    ASSERT_EQ(ret, GSERROR_OK);
}

HWTEST_F(ProducerSurfaceTest, ReqFluFlu, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;

    GSError ret = psurf->RequestBufferNoFence(buffer, requestConfig);
    ASSERT_EQ(ret, GSERROR_OK);
    ASSERT_NE(buffer, nullptr);

    ret = psurf->FlushBuffer(buffer, -1, flushConfig);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = psurf->FlushBuffer(buffer, -1, flushConfig);
    ASSERT_NE(ret, GSERROR_OK);
}

HWTEST_F(ProducerSurfaceTest, AcqRel, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;
    int32_t flushFence;

    GSError ret = psurf->AcquireBuffer(buffer, flushFence, timestamp, damage);
    ASSERT_NE(ret, GSERROR_OK);

    ret = psurf->ReleaseBuffer(buffer, -1);
    ASSERT_NE(ret, GSERROR_OK);

    ret = csurf->AcquireBuffer(buffer, flushFence, timestamp, damage);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = csurf->ReleaseBuffer(buffer, -1);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = csurf->AcquireBuffer(buffer, flushFence, timestamp, damage);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = csurf->ReleaseBuffer(buffer, -1);
    ASSERT_EQ(ret, GSERROR_OK);
}

HWTEST_F(ProducerSurfaceTest, ReqCan, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;

    GSError ret = psurf->RequestBufferNoFence(buffer, requestConfig);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = psurf->CancelBuffer(buffer);
    ASSERT_EQ(ret, GSERROR_OK);
}

HWTEST_F(ProducerSurfaceTest, ReqCanCan, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;

    GSError ret = psurf->RequestBufferNoFence(buffer, requestConfig);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = psurf->CancelBuffer(buffer);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = psurf->CancelBuffer(buffer);
    ASSERT_NE(ret, GSERROR_OK);
}

HWTEST_F(ProducerSurfaceTest, ReqReqReqCanCan, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;
    sptr<SurfaceBuffer> buffer1;
    sptr<SurfaceBuffer> buffer2;

    GSError ret = psurf->RequestBufferNoFence(buffer, requestConfig);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = psurf->RequestBufferNoFence(buffer1, requestConfig);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = psurf->RequestBufferNoFence(buffer2, requestConfig);
    ASSERT_NE(ret, GSERROR_OK);

    ret = psurf->CancelBuffer(buffer);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = psurf->CancelBuffer(buffer1);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = psurf->CancelBuffer(buffer2);
    ASSERT_NE(ret, GSERROR_OK);
}

HWTEST_F(ProducerSurfaceTest, SetQueueSizeDeleting, testing::ext::TestSize.Level0)
{
    sptr<ConsumerSurface> cs = static_cast<ConsumerSurface*>(csurf.GetRefPtr());
    sptr<BufferQueueProducer> bqp = static_cast<BufferQueueProducer*>(cs->producer_.GetRefPtr());
    ASSERT_EQ(bqp->bufferQueue_->queueSize_, 2u);
    ASSERT_EQ(bqp->bufferQueue_->freeList_.size(), 2u);

    GSError ret = psurf->SetQueueSize(1);
    ASSERT_EQ(ret, GSERROR_OK);
    ASSERT_EQ(bqp->bufferQueue_->freeList_.size(), 1u);

    ret = psurf->SetQueueSize(2);
    ASSERT_EQ(ret, GSERROR_OK);
    ASSERT_EQ(bqp->bufferQueue_->freeList_.size(), 1u);
}

HWTEST_F(ProducerSurfaceTest, ReqRel, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;

    GSError ret = psurf->RequestBufferNoFence(buffer, requestConfig);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = psurf->ReleaseBuffer(buffer, -1);
    ASSERT_NE(ret, GSERROR_OK);

    ret = psurf->CancelBuffer(buffer);
    ASSERT_EQ(ret, GSERROR_OK);
}

HWTEST_F(ProducerSurfaceTest, UserData, testing::ext::TestSize.Level0)
{
    GSError ret;

    std::string strs[SURFACE_MAX_USER_DATA_COUNT];
    constexpr int32_t stringLengthMax = 32;
    char str[stringLengthMax] = {};
    for (int i = 0; i < SURFACE_MAX_USER_DATA_COUNT; i++) {
        auto secRet = snprintf_s(str, sizeof(str), sizeof(str) - 1, "%d", i);
        ASSERT_GT(secRet, 0);

        strs[i] = str;
        ret = psurf->SetUserData(strs[i], "magic");
        ASSERT_EQ(ret, GSERROR_OK);
    }

    ret = psurf->SetUserData("-1", "error");
    ASSERT_NE(ret, GSERROR_OK);

    std::string retStr;
    for (int i = 0; i < SURFACE_MAX_USER_DATA_COUNT; i++) {
        retStr = psurf->GetUserData(strs[i]);
        ASSERT_EQ(retStr, "magic");
    }
}

HWTEST_F(ProducerSurfaceTest, RegisterConsumerListener, testing::ext::TestSize.Level0)
{
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    GSError ret = psurf->RegisterConsumerListener(listener);
    ASSERT_NE(ret, GSERROR_OK);
}
}
} // namespace OHOS
