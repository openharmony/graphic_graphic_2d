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

#include "local_buffer_queue_test.h"

#include <map>
#include <vector>

#include <display_gralloc.h>
#include <gtest/gtest.h>
#include <surface_type.h>

#include "buffer_manager.h"
#include "buffer_queue.h"
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
static std::map<int32_t, sptr<SurfaceBufferImpl>> cache;
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

class LocalBufferQueueTest : public testing::Test {
public:
    static void SetUpTestCase(void)
    {
        bq = new BufferQueue();
        sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
        bq->RegisterConsumerListener(listener);
    }
    static void TearDownTestCase(void)
    {
        bq = nullptr;
    }
};

HWTEST_F(LocalBufferQueueTest, Init, testing::ext::TestSize.Level0)
{
    SurfaceError ret = bq->Init();
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

HWTEST_F(LocalBufferQueueTest, QueueSize1, testing::ext::TestSize.Level0)
{
    ASSERT_EQ(bq->GetQueueSize(), (uint32_t)SURFACE_DEFAULT_QUEUE_SIZE);

    SurfaceError ret = bq->SetQueueSize(2);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bq->SetQueueSize(SURFACE_MAX_QUEUE_SIZE + 1);
    ASSERT_NE(ret, SURFACE_ERROR_OK);

    ASSERT_EQ(bq->GetQueueSize(), 2u);
    ASSERT_EQ(bq->queueSize_, 2u);
}

HWTEST_F(LocalBufferQueueTest, QueueSize2, testing::ext::TestSize.Level0)
{
    SurfaceError ret = bq->SetQueueSize(-1);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);
    ASSERT_EQ(bq->GetQueueSize(), 2u);
    ASSERT_EQ(bq->queueSize_, 2u);

    ret = bq->SetQueueSize(0);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);

    ASSERT_EQ(bq->GetQueueSize(), 2u);
    ASSERT_EQ(bq->queueSize_, 2u);
}

HWTEST_F(LocalBufferQueueTest, ReqFluAcqRel, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBufferImpl> buffer;
    int32_t releaseFence;
    int32_t flushFence;
    int32_t sequence;

    // first request
    SurfaceError ret = bq->RequestBuffer(sequence,
                                         buffer, releaseFence, g_requestConfig, deletingBuffers);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_NE(buffer, nullptr);
    ASSERT_GE(sequence, 0);

    // add cache
    cache[sequence] = buffer;

    // buffer queue will map
    uint8_t* addr1 = reinterpret_cast<uint8_t*>(buffer->GetVirAddr());
    ASSERT_NE(addr1, nullptr);
    addr1[0] = 5;

    ret = bq->FlushBuffer(sequence, -1, g_flushConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bq->AcquireBuffer(buffer, flushFence, timestamp, damage);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_NE(buffer, nullptr);

    uint8_t* addr2 = reinterpret_cast<uint8_t*>(buffer->GetVirAddr());
    ASSERT_NE(addr2, nullptr);
    if (addr2 != nullptr) {
        ASSERT_EQ(addr2[0], 5u);
    }

    ret = bq->ReleaseBuffer(buffer, -1);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

HWTEST_F(LocalBufferQueueTest, ReqCan, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBufferImpl> buffer;
    int32_t releaseFence;
    int32_t sequence;

    // not first request
    SurfaceError ret = bq->RequestBuffer(sequence,
                                         buffer, releaseFence, g_requestConfig, deletingBuffers);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_GE(sequence, 0);
    ASSERT_EQ(buffer, nullptr);

    ret = bq->CancelBuffer(sequence);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

HWTEST_F(LocalBufferQueueTest, ReqCanCan, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBufferImpl> buffer;
    int32_t releaseFence;
    int32_t sequence;

    // not first request
    SurfaceError ret = bq->RequestBuffer(sequence,
                                         buffer, releaseFence, g_requestConfig, deletingBuffers);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_GE(sequence, 0);
    ASSERT_EQ(buffer, nullptr);

    ret = bq->CancelBuffer(sequence);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bq->CancelBuffer(sequence);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}

HWTEST_F(LocalBufferQueueTest, ReqFluFlu, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBufferImpl> buffer;
    int32_t releaseFence;
    int32_t sequence;

    // not first request
    SurfaceError ret = bq->RequestBuffer(sequence,
                                         buffer, releaseFence, g_requestConfig, deletingBuffers);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_GE(sequence, 0);
    ASSERT_EQ(buffer, nullptr);

    ret = bq->FlushBuffer(sequence, -1, g_flushConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bq->FlushBuffer(sequence, -1, g_flushConfig);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}

HWTEST_F(LocalBufferQueueTest, AcqRelRel, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBufferImpl> buffer;
    int32_t flushFence;

    SurfaceError ret = bq->AcquireBuffer(buffer, flushFence, timestamp, damage);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bq->ReleaseBuffer(buffer, -1);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bq->ReleaseBuffer(buffer, -1);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}

HWTEST_F(LocalBufferQueueTest, ReqReqReqCanCan, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBufferImpl> buffer1;
    sptr<SurfaceBufferImpl> buffer2;
    sptr<SurfaceBufferImpl> buffer3;
    int32_t releaseFence;
    int32_t sequence1 = -1;
    int32_t sequence2 = -1;
    int32_t sequence3 = -1;
    SurfaceError ret;

    // not alloc
    ret = bq->RequestBuffer(sequence1, buffer1, releaseFence, g_requestConfig, deletingBuffers);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_GE(sequence1, 0);
    ASSERT_EQ(buffer1, nullptr);

    // alloc
    ret = bq->RequestBuffer(sequence2, buffer2, releaseFence, g_requestConfig, deletingBuffers);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_GE(sequence2, 0);
    ASSERT_NE(buffer2, nullptr);

    cache[sequence2] = buffer2;

    // no buffer
    ret = bq->RequestBuffer(sequence3, buffer3, releaseFence, g_requestConfig, deletingBuffers);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
    ASSERT_EQ(sequence3, -1);
    ASSERT_EQ(buffer3, nullptr);

    ret = bq->CancelBuffer(sequence1);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bq->CancelBuffer(sequence2);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bq->CancelBuffer(sequence3);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}

HWTEST_F(LocalBufferQueueTest, ReqRel, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBufferImpl> buffer;
    int32_t releaseFence;
    int32_t sequence;

    // not alloc
    SurfaceError ret = bq->RequestBuffer(sequence, buffer, releaseFence, g_requestConfig, deletingBuffers);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_GE(sequence, 0);
    ASSERT_EQ(buffer, nullptr);

    buffer = cache[sequence];

    ret = bq->ReleaseBuffer(buffer, -1);
    ASSERT_NE(ret, SURFACE_ERROR_OK);

    ret = bq->FlushBuffer(sequence, -1, g_flushConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

HWTEST_F(LocalBufferQueueTest, AcqFlu, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBufferImpl> buffer;
    int32_t flushFence;

    // acq from last test
    SurfaceError ret = bq->AcquireBuffer(buffer, flushFence, timestamp, damage);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    int32_t sequence;
    for (auto it = cache.begin(); it != cache.end(); it++) {
        if (it->second == buffer) {
            sequence = it->first;
        }
    }
    ASSERT_GE(sequence, 0);

    ret = bq->FlushBuffer(sequence, -1, g_flushConfig);
    ASSERT_NE(ret, SURFACE_ERROR_OK);

    ret = bq->ReleaseBuffer(buffer, -1);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

HWTEST_F(LocalBufferQueueTest, ReqDeleteing, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBufferImpl> buffer;
    int32_t releaseFence;
    int32_t sequence;
    BufferRequestConfig deleteconfig = g_requestConfig;
    deleteconfig.width = 1921;

    SurfaceError ret = bq->RequestBuffer(sequence,
                                         buffer, releaseFence, deleteconfig, deletingBuffers);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_EQ(deletingBuffers.size(), 1u);
    ASSERT_GE(sequence, 0);
    ASSERT_NE(buffer, nullptr);

    ret = bq->CancelBuffer(sequence);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

HWTEST_F(LocalBufferQueueTest, ConfigWidth_LE_Min, testing::ext::TestSize.Level0)
{
    BufferRequestConfig config = g_requestConfig;
    config.width = -1;
    sptr<SurfaceBufferImpl> buffer;
    int32_t releaseFence;
    int32_t sequence;

    SurfaceError ret = bq->RequestBuffer(sequence,
                                         buffer, releaseFence, config, deletingBuffers);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);
}

HWTEST_F(LocalBufferQueueTest, ConfigWidth_GE_Max, testing::ext::TestSize.Level0)
{
    BufferRequestConfig config = g_requestConfig;
    config.width = SURFACE_MAX_WIDTH + 1;
    sptr<SurfaceBufferImpl> buffer;
    int32_t releaseFence;
    int32_t sequence;

    SurfaceError ret = bq->RequestBuffer(sequence,
                                         buffer, releaseFence, config, deletingBuffers);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);
}

HWTEST_F(LocalBufferQueueTest, ConfigHeight_LE_Min, testing::ext::TestSize.Level0)
{
    BufferRequestConfig config = g_requestConfig;
    config.height = -1;
    sptr<SurfaceBufferImpl> buffer;
    int32_t releaseFence;
    int32_t sequence;

    SurfaceError ret = bq->RequestBuffer(sequence,
                                         buffer, releaseFence, config, deletingBuffers);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);
}

HWTEST_F(LocalBufferQueueTest, ConfigHeight_GE_Max, testing::ext::TestSize.Level0)
{
    BufferRequestConfig config = g_requestConfig;
    config.height = SURFACE_MAX_HEIGHT + 1;
    sptr<SurfaceBufferImpl> buffer;
    int32_t releaseFence;
    int32_t sequence;

    SurfaceError ret = bq->RequestBuffer(sequence,
                                         buffer, releaseFence, config, deletingBuffers);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);
}

HWTEST_F(LocalBufferQueueTest, ConfigStrideAlignment_LE_Min, testing::ext::TestSize.Level0)
{
    BufferRequestConfig config = g_requestConfig;
    config.strideAlignment = SURFACE_MIN_STRIDE_ALIGNMENT - 1;
    sptr<SurfaceBufferImpl> buffer;
    int32_t releaseFence;
    int32_t sequence;

    SurfaceError ret = bq->RequestBuffer(sequence,
                                         buffer, releaseFence, config, deletingBuffers);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);
}

HWTEST_F(LocalBufferQueueTest, ConfigStrideAlignment_GE_Max, testing::ext::TestSize.Level0)
{
    BufferRequestConfig config = g_requestConfig;
    config.strideAlignment = SURFACE_MAX_STRIDE_ALIGNMENT + 1;
    sptr<SurfaceBufferImpl> buffer;
    int32_t releaseFence;
    int32_t sequence;

    SurfaceError ret = bq->RequestBuffer(sequence,
                                         buffer, releaseFence, config, deletingBuffers);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);
}

HWTEST_F(LocalBufferQueueTest, ConfigStrideAlignment_NOT_POW_2, testing::ext::TestSize.Level0)
{
    BufferRequestConfig config = g_requestConfig;
    config.strideAlignment = 3;
    sptr<SurfaceBufferImpl> buffer;
    int32_t releaseFence;
    int32_t sequence;

    SurfaceError ret = bq->RequestBuffer(sequence,
                                         buffer, releaseFence, config, deletingBuffers);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);
}

HWTEST_F(LocalBufferQueueTest, ConfigFormat_LE_Min, testing::ext::TestSize.Level0)
{
    BufferRequestConfig config = g_requestConfig;
    config.format = -1;
    sptr<SurfaceBufferImpl> buffer;
    int32_t releaseFence;
    int32_t sequence;

    SurfaceError ret = bq->RequestBuffer(sequence,
                                         buffer, releaseFence, config, deletingBuffers);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);
}

HWTEST_F(LocalBufferQueueTest, ConfigFormat_GE_Max, testing::ext::TestSize.Level0)
{
    BufferRequestConfig config = g_requestConfig;
    config.format = PIXEL_FMT_BUTT + 1;
    sptr<SurfaceBufferImpl> buffer;
    int32_t releaseFence;
    int32_t sequence;

    SurfaceError ret = bq->RequestBuffer(sequence,
                                         buffer, releaseFence, config, deletingBuffers);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);
}

HWTEST_F(LocalBufferQueueTest, ConfigUsage_LE_Min, testing::ext::TestSize.Level0)
{
    BufferRequestConfig config = g_requestConfig;
    config.usage = -1;
    sptr<SurfaceBufferImpl> buffer;
    int32_t releaseFence;
    int32_t sequence;

    SurfaceError ret = bq->RequestBuffer(sequence,
                                         buffer, releaseFence, config, deletingBuffers);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);
}

HWTEST_F(LocalBufferQueueTest, ConfigUsage_GE_Max, testing::ext::TestSize.Level0)
{
    BufferRequestConfig config = g_requestConfig;
    config.usage = HBM_USE_MEM_DMA * 2;
    sptr<SurfaceBufferImpl> buffer;
    int32_t releaseFence;
    int32_t sequence;

    SurfaceError ret = bq->RequestBuffer(sequence,
                                         buffer, releaseFence, config, deletingBuffers);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);
}
}
