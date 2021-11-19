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
    csurface = Surface::CreateSurfaceAsConsumer();
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    csurface->RegisterConsumerListener(listener);
    producer = csurface->GetProducer();
    psurface = Surface::CreateSurfaceAsProducer(producer);
}

void ProducerSurfaceTest::TearDownTestCase()
{
    csurface = nullptr;
    producer = nullptr;
    psurface = nullptr;
}

namespace {
HWTEST_F(ProducerSurfaceTest, ProducerSurface, testing::ext::TestSize.Level0)
{
    ASSERT_NE(psurface, nullptr);
}

HWTEST_F(ProducerSurfaceTest, QueueSize, testing::ext::TestSize.Level0)
{
    ASSERT_EQ(psurface->GetQueueSize(), (uint32_t)SURFACE_DEFAULT_QUEUE_SIZE);
    SurfaceError ret = psurface->SetQueueSize(2);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = psurface->SetQueueSize(SURFACE_MAX_QUEUE_SIZE + 1);
    ASSERT_NE(ret, SURFACE_ERROR_OK);

    ASSERT_EQ(psurface->GetQueueSize(), 2u);
}

HWTEST_F(ProducerSurfaceTest, ReqFlu, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;

    SurfaceError ret = psurface->RequestBufferNoFence(buffer, requestConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_NE(buffer, nullptr);

    ret = psurface->FlushBuffer(buffer, -1, flushConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

HWTEST_F(ProducerSurfaceTest, ReqFluFlu, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;

    SurfaceError ret = psurface->RequestBufferNoFence(buffer, requestConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_NE(buffer, nullptr);

    ret = psurface->FlushBuffer(buffer, -1, flushConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = psurface->FlushBuffer(buffer, -1, flushConfig);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}

HWTEST_F(ProducerSurfaceTest, AcqRel, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;
    int32_t flushFence;

    SurfaceError ret = psurface->AcquireBuffer(buffer, flushFence, timestamp, damage);
    ASSERT_NE(ret, SURFACE_ERROR_OK);

    ret = psurface->ReleaseBuffer(buffer, -1);
    ASSERT_NE(ret, SURFACE_ERROR_OK);

    ret = csurface->AcquireBuffer(buffer, flushFence, timestamp, damage);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = csurface->ReleaseBuffer(buffer, -1);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = csurface->AcquireBuffer(buffer, flushFence, timestamp, damage);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = csurface->ReleaseBuffer(buffer, -1);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

HWTEST_F(ProducerSurfaceTest, ReqCan, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;

    SurfaceError ret = psurface->RequestBufferNoFence(buffer, requestConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = psurface->CancelBuffer(buffer);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

HWTEST_F(ProducerSurfaceTest, ReqCanCan, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;

    SurfaceError ret = psurface->RequestBufferNoFence(buffer, requestConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = psurface->CancelBuffer(buffer);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = psurface->CancelBuffer(buffer);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}

HWTEST_F(ProducerSurfaceTest, ReqReqReqCanCan, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;
    sptr<SurfaceBuffer> buffer1;
    sptr<SurfaceBuffer> buffer2;

    SurfaceError ret = psurface->RequestBufferNoFence(buffer, requestConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = psurface->RequestBufferNoFence(buffer1, requestConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = psurface->RequestBufferNoFence(buffer2, requestConfig);
    ASSERT_NE(ret, SURFACE_ERROR_OK);

    ret = psurface->CancelBuffer(buffer);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = psurface->CancelBuffer(buffer1);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = psurface->CancelBuffer(buffer2);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}

HWTEST_F(ProducerSurfaceTest, SetQueueSizeDeleting, testing::ext::TestSize.Level0)
{
    sptr<ConsumerSurface> cs = static_cast<ConsumerSurface*>(csurface.GetRefPtr());
    sptr<BufferQueueProducer> bqp = static_cast<BufferQueueProducer*>(cs->producer_.GetRefPtr());
    ASSERT_EQ(bqp->bufferQueue_->queueSize_, 2u);
    ASSERT_EQ(bqp->bufferQueue_->freeList_.size(), 2u);

    SurfaceError ret = psurface->SetQueueSize(1);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_EQ(bqp->bufferQueue_->freeList_.size(), 1u);

    ret = psurface->SetQueueSize(2);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_EQ(bqp->bufferQueue_->freeList_.size(), 1u);
}

HWTEST_F(ProducerSurfaceTest, ReqRel, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;

    SurfaceError ret = psurface->RequestBufferNoFence(buffer, requestConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = psurface->ReleaseBuffer(buffer, -1);
    ASSERT_NE(ret, SURFACE_ERROR_OK);

    ret = psurface->CancelBuffer(buffer);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

HWTEST_F(ProducerSurfaceTest, UserData, testing::ext::TestSize.Level0)
{
    SurfaceError ret;

    std::string strs[SURFACE_MAX_USER_DATA_COUNT];
    constexpr int32_t stringLengthMax = 32;
    char str[stringLengthMax] = {};
    for (int i = 0; i < SURFACE_MAX_USER_DATA_COUNT; i++) {
        auto secRet = snprintf_s(str, sizeof(str), sizeof(str) - 1, "%d", i);
        ASSERT_GT(secRet, 0);

        strs[i] = str;
        ret = psurface->SetUserData(strs[i], "magic");
        ASSERT_EQ(ret, SURFACE_ERROR_OK);
    }

    ret = psurface->SetUserData("-1", "error");
    ASSERT_NE(ret, SURFACE_ERROR_OK);

    std::string retStr;
    for (int i = 0; i < SURFACE_MAX_USER_DATA_COUNT; i++) {
        retStr = psurface->GetUserData(strs[i]);
        ASSERT_EQ(retStr, "magic");
    }
}

HWTEST_F(ProducerSurfaceTest, RegisterConsumerListener, testing::ext::TestSize.Level0)
{
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    SurfaceError ret = psurface->RegisterConsumerListener(listener);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}
}
} // namespace OHOS
