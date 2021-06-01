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

#include "local_producer_surface_test.h"

#include <cstdlib>

#include <display_type.h>
#include <gtest/gtest.h>
#include <securec.h>
#include <surface.h>
#include <surface_buffer.h>

#include "buffer_queue_producer.h"
#include "consumer_surface.h"
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
int64_t timestamp;
Rect damage;
sptr<Surface> surface_;
sptr<IBufferProducer> producer;
sptr<Surface> surface;

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

class LocalProducerSurfaceTest : public testing::Test {
public:
    static void SetUpTestCase(void)
    {
        surface_ = Surface::CreateSurfaceAsConsumer();
        sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
        surface_->RegisterConsumerListener(listener);
        producer = surface_->GetProducer();
        surface = Surface::CreateSurfaceAsProducer(producer);
    }
    static void TearDownTestCase(void)
    {
        surface_ = nullptr;
        producer = nullptr;
        surface = nullptr;
    }
};

HWTEST_F(LocalProducerSurfaceTest, ProducerSurface, testing::ext::TestSize.Level0)
{
    ASSERT_NE(surface, nullptr);
}

HWTEST_F(LocalProducerSurfaceTest, QueueSize, testing::ext::TestSize.Level0)
{
    ASSERT_EQ(surface->GetQueueSize(), (uint32_t)SURFACE_DEFAULT_QUEUE_SIZE);
    SurfaceError ret = surface->SetQueueSize(2);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = surface->SetQueueSize(SURFACE_MAX_QUEUE_SIZE + 1);
    ASSERT_NE(ret, SURFACE_ERROR_OK);

    ASSERT_EQ(surface->GetQueueSize(), 2u);
}

HWTEST_F(LocalProducerSurfaceTest, ReqFlu, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;
    int32_t releaseFence;

    SurfaceError ret = surface->RequestBuffer(buffer, releaseFence, g_requestConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_NE(buffer, nullptr);

    ret = surface->FlushBuffer(buffer, -1, g_flushConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

HWTEST_F(LocalProducerSurfaceTest, ReqFluFlu, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;
    int32_t releaseFence;

    SurfaceError ret = surface->RequestBuffer(buffer, releaseFence, g_requestConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_NE(buffer, nullptr);

    ret = surface->FlushBuffer(buffer, -1, g_flushConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = surface->FlushBuffer(buffer, -1, g_flushConfig);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}

HWTEST_F(LocalProducerSurfaceTest, AcqRel, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;
    int32_t flushFence;

    SurfaceError ret = surface->AcquireBuffer(buffer, flushFence, timestamp, damage);
    ASSERT_NE(ret, SURFACE_ERROR_OK);

    ret = surface->ReleaseBuffer(buffer, -1);
    ASSERT_NE(ret, SURFACE_ERROR_OK);

    ret = surface_->AcquireBuffer(buffer, flushFence, timestamp, damage);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = surface_->ReleaseBuffer(buffer, -1);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = surface_->AcquireBuffer(buffer, flushFence, timestamp, damage);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = surface_->ReleaseBuffer(buffer, -1);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

HWTEST_F(LocalProducerSurfaceTest, ReqCan, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;
    int releaseFence;

    SurfaceError ret = surface->RequestBuffer(buffer, releaseFence, g_requestConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = surface->CancelBuffer(buffer);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

HWTEST_F(LocalProducerSurfaceTest, ReqCanCan, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;
    int releaseFence;

    SurfaceError ret = surface->RequestBuffer(buffer, releaseFence, g_requestConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = surface->CancelBuffer(buffer);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = surface->CancelBuffer(buffer);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}

HWTEST_F(LocalProducerSurfaceTest, ReqReqReqCanCan, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;
    sptr<SurfaceBuffer> buffer1;
    sptr<SurfaceBuffer> buffer2;
    int releaseFence;

    SurfaceError ret = surface->RequestBuffer(buffer, releaseFence, g_requestConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = surface->RequestBuffer(buffer1, releaseFence, g_requestConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = surface->RequestBuffer(buffer2, releaseFence, g_requestConfig);
    ASSERT_NE(ret, SURFACE_ERROR_OK);

    ret = surface->CancelBuffer(buffer);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = surface->CancelBuffer(buffer1);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = surface->CancelBuffer(buffer2);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}

HWTEST_F(LocalProducerSurfaceTest, SetQueueSizeDeleting, testing::ext::TestSize.Level0)
{
    sptr<ConsumerSurface> cs = static_cast<ConsumerSurface*>(surface_.GetRefPtr());
    sptr<BufferQueueProducer> bqp = static_cast<BufferQueueProducer*>(cs->producer_.GetRefPtr());
    ASSERT_EQ(bqp->bufferQueue_->queueSize_, 2u);
    ASSERT_EQ(bqp->bufferQueue_->freeList_.size(), 2u);

    SurfaceError ret = surface->SetQueueSize(1);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_EQ(bqp->bufferQueue_->freeList_.size(), 1u);

    ret = surface->SetQueueSize(2);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_EQ(bqp->bufferQueue_->freeList_.size(), 1u);
}

HWTEST_F(LocalProducerSurfaceTest, ReqRel, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;
    int releaseFence;

    SurfaceError ret = surface->RequestBuffer(buffer, releaseFence, g_requestConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = surface->ReleaseBuffer(buffer, -1);
    ASSERT_NE(ret, SURFACE_ERROR_OK);

    ret = surface->CancelBuffer(buffer);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

HWTEST_F(LocalProducerSurfaceTest, UserData, testing::ext::TestSize.Level0)
{
    SurfaceError ret;

    std::string strs[SURFACE_MAX_USER_DATA_COUNT];
    constexpr int32_t stringLengthMax = 32;
    char str[stringLengthMax] = {};
    for (int i = 0; i < SURFACE_MAX_USER_DATA_COUNT; i++) {
        auto secRet = snprintf_s(str, sizeof(str), sizeof(str) - 1, "%d", i);
        ASSERT_GT(secRet, 0);

        strs[i] = str;
        ret = surface->SetUserData(strs[i], "magic");
        ASSERT_EQ(ret, SURFACE_ERROR_OK);
    }

    ret = surface->SetUserData("-1", "error");
    ASSERT_NE(ret, SURFACE_ERROR_OK);

    std::string retStr;
    for (int i = 0; i < SURFACE_MAX_USER_DATA_COUNT; i++) {
        retStr = surface->GetUserData(strs[i]);
        ASSERT_EQ(retStr, "magic");
    }
}

HWTEST_F(LocalProducerSurfaceTest, RegisterConsumerListener, testing::ext::TestSize.Level0)
{
    class TestConsumerListener : public IBufferConsumerListener {
    public:
        void OnBufferAvailable() override {}
    };
    sptr<IBufferConsumerListener> listener = new TestConsumerListener();
    SurfaceError ret = surface->RegisterConsumerListener(listener);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}
}
