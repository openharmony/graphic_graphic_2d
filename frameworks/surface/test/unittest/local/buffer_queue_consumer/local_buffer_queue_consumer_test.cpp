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

#include "local_buffer_queue_consumer_test.h"

#include <vector>

#include <display_type.h>
#include <gtest/gtest.h>
#include <surface_type.h>

#include "buffer_queue_consumer.h"
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
sptr<BufferQueue> bq;
sptr<BufferQueueConsumer> bqc;
std::vector<int32_t> deletingBuffers;

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

class LocalBufferQueueConsumerTest : public testing::Test {
public:
    static void SetUpTestCase(void)
    {
        bq = new BufferQueue();
        bq->Init();
        bqc = new BufferQueueConsumer(bq);
        sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
        bqc->RegisterConsumerListener(listener);
    }
    static void TearDownTestCase(void)
    {
        bq = nullptr;
        bqc = nullptr;
    }
};

HWTEST_F(LocalBufferQueueConsumerTest, AcqRel, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBufferImpl> buffer;
    int32_t releaseFence;
    int32_t flushFence;
    int32_t sequence;

    SurfaceError ret = bq->RequestBuffer(sequence,
                                         buffer, releaseFence, g_requestConfig, deletingBuffers);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_GE(sequence, 0);
    ASSERT_NE(buffer, nullptr);

    uint8_t* addr1 = reinterpret_cast<uint8_t*>(buffer->GetVirAddr());
    ASSERT_NE(addr1, nullptr);

    ret = bq->FlushBuffer(sequence, -1, g_flushConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bqc->AcquireBuffer(buffer, flushFence, timestamp, damage);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bqc->ReleaseBuffer(buffer, -1);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

HWTEST_F(LocalBufferQueueConsumerTest, AcqRelRel, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBufferImpl> buffer;
    int32_t releaseFence;
    int32_t flushFence;
    int32_t sequence;

    SurfaceError ret = bq->RequestBuffer(sequence,
                                         buffer, releaseFence, g_requestConfig, deletingBuffers);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_GE(sequence, 0);
    ASSERT_EQ(buffer, nullptr);

    ret = bq->FlushBuffer(sequence, -1, g_flushConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bqc->AcquireBuffer(buffer, flushFence, timestamp, damage);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bqc->ReleaseBuffer(buffer, -1);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bqc->ReleaseBuffer(buffer, -1);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}
}
