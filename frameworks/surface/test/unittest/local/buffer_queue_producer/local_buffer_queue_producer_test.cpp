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

#include "local_buffer_queue_producer_test.h"

#include <vector>

#include <display_type.h>
#include <gtest/gtest.h>
#include <surface_type.h>

#include "buffer_queue_producer.h"
#include "environments.h"

using namespace OHOS;

namespace {
BufferRequestConfig g_requestConfig = {
    .width = 1920,
    .height = 1080,
    .strideAlignment = 8,
    .format = PIXEL_FMT_RGBA_8888,
    .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
    .timeout = 0,
};
BufferFlushConfig g_flushConfig = {
    .damage = {
        .x = 0,
        .y = 0,
        .w = 1920,
        .h = 1080
    },
    .timestamp = 0
};
static int64_t timestamp;
static Rect damage;
static sptr<BufferQueue> bq;
static sptr<BufferQueueProducer> bqp;
static std::vector<int32_t> deletingBuffers;

class BufferConsumerListener: public IBufferConsumerListener {
public:
    BufferConsumerListener()
    {
    }

    ~BufferConsumerListener()
    {
    }

    void OnBufferAvailable()
    {
    }
};

class LocalBufferQueueProducerTest : public testing::Test {
public:
    static void SetUpTestCase(void)
    {
        bq = new BufferQueue();
        bq->Init();
        sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
        bq->RegisterConsumerListener(listener);
        bqp = new BufferQueueProducer(bq);
    }
    static void TearDownTestCase(void)
    {
        bq = nullptr;
        bqp = nullptr;
    }
};

HWTEST_F(LocalBufferQueueProducerTest, QueueSize, testing::ext::TestSize.Level0)
{
    ASSERT_EQ(bqp->GetQueueSize(), (uint32_t)SURFACE_DEFAULT_QUEUE_SIZE);

    SurfaceError ret = bqp->SetQueueSize(2);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bqp->SetQueueSize(SURFACE_MAX_QUEUE_SIZE + 1);
    ASSERT_NE(ret, SURFACE_ERROR_OK);

    ASSERT_EQ(bqp->GetQueueSize(), 2u);
    ASSERT_EQ(bq->queueSize_, 2u);
}

HWTEST_F(LocalBufferQueueProducerTest, ReqCan, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;
    int32_t releaseFence;
    int32_t sequence;

    SurfaceError ret = bqp->RequestBuffer(sequence,
                                          buffer, releaseFence, g_requestConfig, deletingBuffers);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bqp->CancelBuffer(sequence);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

HWTEST_F(LocalBufferQueueProducerTest, ReqCanCan, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;
    int32_t releaseFence;
    int32_t sequence;

    SurfaceError ret = bqp->RequestBuffer(sequence,
                                          buffer, releaseFence, g_requestConfig, deletingBuffers);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bqp->CancelBuffer(sequence);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bqp->CancelBuffer(sequence);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}

HWTEST_F(LocalBufferQueueProducerTest, ReqReqReqCanCan, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer1;
    sptr<SurfaceBuffer> buffer2;
    sptr<SurfaceBuffer> buffer3;
    int32_t releaseFence;
    int32_t sequence1;
    int32_t sequence2;
    int32_t sequence3;
    SurfaceError ret;

    ret = bqp->RequestBuffer(sequence1, buffer1, releaseFence, g_requestConfig, deletingBuffers);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_EQ(buffer1, nullptr);

    ret = bqp->RequestBuffer(sequence2, buffer2, releaseFence, g_requestConfig, deletingBuffers);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_NE(buffer2, nullptr);

    ret = bqp->RequestBuffer(sequence3, buffer3, releaseFence, g_requestConfig, deletingBuffers);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
    ASSERT_EQ(buffer3, nullptr);

    ret = bqp->CancelBuffer(sequence1);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bqp->CancelBuffer(sequence2);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bqp->CancelBuffer(sequence3);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}

HWTEST_F(LocalBufferQueueProducerTest, ReqFlu, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;
    int32_t releaseFence;
    int32_t flushFence;
    int32_t sequence;

    SurfaceError ret = bqp->RequestBuffer(sequence, buffer, releaseFence, g_requestConfig, deletingBuffers);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bqp->FlushBuffer(sequence, -1, g_flushConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    sptr<SurfaceBufferImpl> bufferImpl = static_cast<SurfaceBufferImpl*>(buffer.GetRefPtr());
    ret = bq->AcquireBuffer(bufferImpl, flushFence, timestamp, damage);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bq->ReleaseBuffer(bufferImpl, -1);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

HWTEST_F(LocalBufferQueueProducerTest, ReqFluFlu, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;
    int32_t releaseFence;
    int32_t flushFence;
    int32_t sequence;

    SurfaceError ret = bqp->RequestBuffer(sequence, buffer, releaseFence, g_requestConfig, deletingBuffers);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bqp->FlushBuffer(sequence, -1, g_flushConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bqp->FlushBuffer(sequence, -1, g_flushConfig);
    ASSERT_NE(ret, SURFACE_ERROR_OK);

    sptr<SurfaceBufferImpl> bufferImpl = static_cast<SurfaceBufferImpl*>(buffer.GetRefPtr());
    ret = bq->AcquireBuffer(bufferImpl, flushFence, timestamp, damage);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bq->ReleaseBuffer(bufferImpl, -1);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}
}
