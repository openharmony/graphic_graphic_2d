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

#include "buffer_queue_test.h"

#include "buffer_consumer_listener.h"
#include "buffer_manager.h"

namespace OHOS {
void BufferQueueTest::SetUpTestCase()
{
    bq = new BufferQueue("test");
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    bq->RegisterConsumerListener(listener);
}

void BufferQueueTest::TearDownTestCase()
{
    bq = nullptr;
}

namespace {
HWTEST_F(BufferQueueTest, Init, testing::ext::TestSize.Level0)
{
    SurfaceError ret = bq->Init();
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

HWTEST_F(BufferQueueTest, QueueSize1, testing::ext::TestSize.Level0)
{
    ASSERT_EQ(bq->GetQueueSize(), (uint32_t)SURFACE_DEFAULT_QUEUE_SIZE);

    SurfaceError ret = bq->SetQueueSize(2);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bq->SetQueueSize(SURFACE_MAX_QUEUE_SIZE + 1);
    ASSERT_NE(ret, SURFACE_ERROR_OK);

    ASSERT_EQ(bq->GetQueueSize(), 2u);
    ASSERT_EQ(bq->queueSize_, 2u);
}

HWTEST_F(BufferQueueTest, QueueSize2, testing::ext::TestSize.Level0)
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

HWTEST_F(BufferQueueTest, ReqFluAcqRel, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval;

    // first request
    SurfaceError ret = bq->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_NE(retval.buffer, nullptr);
    ASSERT_GE(retval.sequence, 0);

    // add cache
    cache[retval.sequence] = retval.buffer;

    // buffer queue will map
    uint8_t *addr1 = reinterpret_cast<uint8_t*>(retval.buffer->GetVirAddr());
    ASSERT_NE(addr1, nullptr);
    addr1[0] = 5;

    ret = bq->FlushBuffer(retval.sequence, bedata, -1, flushConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    sptr<SurfaceBufferImpl> bufferImpl = SurfaceBufferImpl::FromBase(retval.buffer);
    ret = bq->AcquireBuffer(bufferImpl, retval.fence, timestamp, damage);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_NE(retval.buffer, nullptr);

    uint8_t *addr2 = reinterpret_cast<uint8_t*>(retval.buffer->GetVirAddr());
    ASSERT_NE(addr2, nullptr);
    if (addr2 != nullptr) {
        ASSERT_EQ(addr2[0], 5u);
    }

    ret = bq->ReleaseBuffer(bufferImpl, -1);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

HWTEST_F(BufferQueueTest, ReqCan, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval;

    // not first request
    SurfaceError ret = bq->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_GE(retval.sequence, 0);
    ASSERT_EQ(retval.buffer, nullptr);

    ret = bq->CancelBuffer(retval.sequence, bedata);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

HWTEST_F(BufferQueueTest, ReqCanCan, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval;

    // not first request
    SurfaceError ret = bq->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_GE(retval.sequence, 0);
    ASSERT_EQ(retval.buffer, nullptr);

    ret = bq->CancelBuffer(retval.sequence, bedata);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bq->CancelBuffer(retval.sequence, bedata);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}

HWTEST_F(BufferQueueTest, ReqFluFlu, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval;

    // not first request
    SurfaceError ret = bq->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_GE(retval.sequence, 0);
    ASSERT_EQ(retval.buffer, nullptr);

    ret = bq->FlushBuffer(retval.sequence, bedata, -1, flushConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bq->FlushBuffer(retval.sequence, bedata, -1, flushConfig);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}

HWTEST_F(BufferQueueTest, AcqRelRel, testing::ext::TestSize.Level0)
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

HWTEST_F(BufferQueueTest, ReqReqReqCanCan, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval1;
    IBufferProducer::RequestBufferReturnValue retval2;
    IBufferProducer::RequestBufferReturnValue retval3;
    SurfaceError ret;

    // not alloc
    ret = bq->RequestBuffer(requestConfig, bedata, retval1);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_GE(retval1.sequence, 0);
    ASSERT_EQ(retval1.buffer, nullptr);

    // alloc
    ret = bq->RequestBuffer(requestConfig, bedata, retval2);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_GE(retval2.sequence, 0);
    ASSERT_NE(retval2.buffer, nullptr);

    cache[retval2.sequence] = retval2.buffer;

    // no buffer
    ret = bq->RequestBuffer(requestConfig, bedata, retval3);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
    ASSERT_EQ(retval3.buffer, nullptr);

    ret = bq->CancelBuffer(retval1.sequence, bedata);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bq->CancelBuffer(retval2.sequence, bedata);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bq->CancelBuffer(retval3.sequence, bedata);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}

HWTEST_F(BufferQueueTest, ReqRel, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval;

    // not alloc
    SurfaceError ret = bq->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_GE(retval.sequence, 0);
    ASSERT_EQ(retval.buffer, nullptr);

    retval.buffer = cache[retval.sequence];

    sptr<SurfaceBufferImpl> bufferImpl = SurfaceBufferImpl::FromBase(retval.buffer);
    ret = bq->ReleaseBuffer(bufferImpl, -1);
    ASSERT_NE(ret, SURFACE_ERROR_OK);

    ret = bq->FlushBuffer(retval.sequence, bedata, -1, flushConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

HWTEST_F(BufferQueueTest, AcqFlu, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBufferImpl> bufferImpl;
    int32_t flushFence;

    // acq from last test
    SurfaceError ret = bq->AcquireBuffer(bufferImpl, flushFence, timestamp, damage);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    int32_t sequence;
    for (auto it = cache.begin(); it != cache.end(); it++) {
        if (it->second.GetRefPtr() == bufferImpl.GetRefPtr()) {
            sequence = it->first;
        }
    }
    ASSERT_GE(sequence, 0);

    ret = bq->FlushBuffer(sequence, bedata, -1, flushConfig);
    ASSERT_NE(ret, SURFACE_ERROR_OK);

    ret = bq->ReleaseBuffer(bufferImpl, -1);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

HWTEST_F(BufferQueueTest, ReqDeleteing, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval;
    BufferRequestConfig deleteconfig = requestConfig;
    deleteconfig.width = 1921;

    SurfaceError ret = bq->RequestBuffer(deleteconfig, bedata, retval);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_EQ(retval.deletingBuffers.size(), 1u);
    ASSERT_GE(retval.sequence, 0);
    ASSERT_NE(retval.buffer, nullptr);

    ret = bq->CancelBuffer(retval.sequence, bedata);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

HWTEST_F(BufferQueueTest, ConfigWidth_LE_Min, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval;
    BufferRequestConfig config = requestConfig;
    config.width = -1;

    SurfaceError ret = bq->RequestBuffer(config, bedata, retval);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);
}

HWTEST_F(BufferQueueTest, ConfigWidth_GE_Max, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval;
    BufferRequestConfig config = requestConfig;
    config.width = SURFACE_MAX_WIDTH + 1;

    SurfaceError ret = bq->RequestBuffer(config, bedata, retval);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);
}

HWTEST_F(BufferQueueTest, ConfigHeight_LE_Min, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval;
    BufferRequestConfig config = requestConfig;
    config.height = -1;

    SurfaceError ret = bq->RequestBuffer(config, bedata, retval);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);
}

HWTEST_F(BufferQueueTest, ConfigHeight_GE_Max, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval;
    BufferRequestConfig config = requestConfig;
    config.height = SURFACE_MAX_HEIGHT + 1;

    SurfaceError ret = bq->RequestBuffer(config, bedata, retval);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);
}

HWTEST_F(BufferQueueTest, ConfigStrideAlignment_LE_Min, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval;
    BufferRequestConfig config = requestConfig;
    config.strideAlignment = SURFACE_MIN_STRIDE_ALIGNMENT - 1;

    SurfaceError ret = bq->RequestBuffer(config, bedata, retval);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);
}

HWTEST_F(BufferQueueTest, ConfigStrideAlignment_GE_Max, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval;
    BufferRequestConfig config = requestConfig;
    config.strideAlignment = SURFACE_MAX_STRIDE_ALIGNMENT + 1;

    SurfaceError ret = bq->RequestBuffer(config, bedata, retval);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);
}

HWTEST_F(BufferQueueTest, ConfigStrideAlignment_NOT_POW_2, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval;
    BufferRequestConfig config = requestConfig;
    config.strideAlignment = 3;

    SurfaceError ret = bq->RequestBuffer(config, bedata, retval);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);
}

HWTEST_F(BufferQueueTest, ConfigFormat_LE_Min, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval;
    BufferRequestConfig config = requestConfig;
    config.format = -1;

    SurfaceError ret = bq->RequestBuffer(config, bedata, retval);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);
}

HWTEST_F(BufferQueueTest, ConfigFormat_GE_Max, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval;
    BufferRequestConfig config = requestConfig;
    config.format = PIXEL_FMT_BUTT + 1;

    SurfaceError ret = bq->RequestBuffer(config, bedata, retval);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);
}

HWTEST_F(BufferQueueTest, ConfigUsage_LE_Min, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval;
    BufferRequestConfig config = requestConfig;
    config.usage = -1;

    SurfaceError ret = bq->RequestBuffer(config, bedata, retval);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);
}

HWTEST_F(BufferQueueTest, ConfigUsage_GE_Max, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval;
    BufferRequestConfig config = requestConfig;
    config.usage = HBM_USE_MEM_DMA * 2;

    SurfaceError ret = bq->RequestBuffer(config, bedata, retval);
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_PARAM);
}
}
} // namespace OHOS
