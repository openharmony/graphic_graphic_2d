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
#include <map>
#include <gtest/gtest.h>
#include <surface.h>
#include <buffer_extra_data_impl.h>
#include <buffer_queue.h>
#include "buffer_consumer_listener.h"
#include "sync_fence.h"
#include "consumer_surface.h"
#include "producer_surface_delegator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class BufferQueueTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    static inline BufferFlushConfigWithDamages flushConfig = {
        .damages = {
            {
                .w = 0x100,
                .h = 0x100,
            }
        },
    };
    static inline int64_t timestamp = 0;
    static inline std::vector<Rect> damages = {};
    static inline sptr<BufferQueue> bq = nullptr;
    static inline std::map<int32_t, sptr<SurfaceBuffer>> cache;
    static inline sptr<BufferExtraData> bedata = nullptr;
    static inline sptr<ProducerSurfaceDelegator> surfaceDelegator = nullptr;
    static inline sptr<IConsumerSurface> csurface1 = nullptr;
};

void BufferQueueTest::SetUpTestCase()
{
    bq = new BufferQueue("test");
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    bq->RegisterConsumerListener(listener);
    bedata = new OHOS::BufferExtraDataImpl;
    csurface1 = IConsumerSurface::Create();
}

void BufferQueueTest::TearDownTestCase()
{
    bq = nullptr;
}

/*
* Function: Init
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call Init and check ret
 */
HWTEST_F(BufferQueueTest, Init001, Function | MediumTest | Level2)
{
    GSError ret = bq->Init();
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: GetUsedSize
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetUsedSize and check ret
 */
HWTEST_F(BufferQueueTest, GetUsedSize001, Function | MediumTest | Level2)
{
    uint32_t usedSize = bq->GetUsedSize();
    ASSERT_NE(usedSize, -1);
}

/*
* Function: SetQueueSize and GetQueueSize
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetQueueSize for default
*                  2. call SetQueueSize
*                  3. call SetQueueSize again with abnormal input
*                  4. check ret and call GetQueueSize
 */
HWTEST_F(BufferQueueTest, QueueSize001, Function | MediumTest | Level2)
{
    ASSERT_EQ(bq->GetQueueSize(), (uint32_t)SURFACE_DEFAULT_QUEUE_SIZE);

    GSError ret = bq->SetQueueSize(2);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bq->SetQueueSize(SURFACE_MAX_QUEUE_SIZE + 1);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ASSERT_EQ(bq->GetQueueSize(), 2u);
    BufferQueue *bqTmp = new BufferQueue("testTmp", true);
    EXPECT_EQ(bqTmp->SetQueueSize(2), GSERROR_INVALID_ARGUMENTS);
    EXPECT_EQ(bqTmp->SetQueueSize(1), GSERROR_OK);
    bqTmp = nullptr;
}

/*
* Function: SetQueueSize and GetQueueSize
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetQueueSize 2 times both with abnormal input
*                  2. call GetQueueSize
*                  3. check ret
 */
HWTEST_F(BufferQueueTest, QueueSize002, Function | MediumTest | Level2)
{
    GSError ret = bq->SetQueueSize(-1);
    ASSERT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);
    ASSERT_EQ(bq->GetQueueSize(), 2u);

    ret = bq->SetQueueSize(0);
    ASSERT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);
    ASSERT_EQ(bq->GetQueueSize(), 2u);
}

/*
* Function: RequestBuffer, FlushBuffer, AcquireBuffer and ReleaseBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBuffer and FlushBuffer
*                  2. call AcquireBuffer and ReleaseBuffer
*                  3. check ret
 */
HWTEST_F(BufferQueueTest, ReqCanFluAcqRel001, Function | MediumTest | Level2)
{
    IBufferProducer::RequestBufferReturnValue retval;

    // first request
    GSError ret = bq->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(retval.buffer, nullptr);
    ASSERT_GE(retval.sequence, 0);

    // add cache
    cache[retval.sequence] = retval.buffer;

    // buffer queue will map
    uint8_t *addr1 = reinterpret_cast<uint8_t*>(retval.buffer->GetVirAddr());
    ASSERT_NE(addr1, nullptr);
    addr1[0] = 5;

    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    ret = bq->FlushBuffer(retval.sequence, bedata, acquireFence, flushConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bq->AcquireBuffer(retval.buffer, retval.fence, timestamp, damages);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(retval.buffer, nullptr);

    uint8_t *addr2 = reinterpret_cast<uint8_t*>(retval.buffer->GetVirAddr());
    ASSERT_NE(addr2, nullptr);
    if (addr2 != nullptr) {
        ASSERT_EQ(addr2[0], 5u);
    }

    sptr<SyncFence> releaseFence = SyncFence::INVALID_FENCE;
    ret = bq->ReleaseBuffer(retval.buffer, releaseFence);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: RequestBuffer and CancelBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBuffer
*                  2. call CancelBuffer
*                  3. check ret
 */
HWTEST_F(BufferQueueTest, ReqCanFluAcqRel002, Function | MediumTest | Level2)
{
    IBufferProducer::RequestBufferReturnValue retval;

    // not first request
    GSError ret = bq->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_GE(retval.sequence, 0);
    ASSERT_EQ(retval.buffer, nullptr);

    ret = bq->CancelBuffer(retval.sequence, bedata);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: RequestBuffer and CancelBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBuffer
*                  2. call CancelBuffer 2 times
*                  3. check ret
 */
HWTEST_F(BufferQueueTest, ReqCanFluAcqRel003, Function | MediumTest | Level2)
{
    IBufferProducer::RequestBufferReturnValue retval;

    // not first request
    GSError ret = bq->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_GE(retval.sequence, 0);
    ASSERT_EQ(retval.buffer, nullptr);

    ret = bq->CancelBuffer(retval.sequence, bedata);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bq->CancelBuffer(retval.sequence, bedata);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
}

/*
* Function: RequestBuffer and FlushBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBuffer
*                  2. call FlushBuffer 2 times
*                  3. check ret
 */
HWTEST_F(BufferQueueTest, ReqCanFluAcqRel004, Function | MediumTest | Level2)
{
    IBufferProducer::RequestBufferReturnValue retval;

    // not first request
    GSError ret = bq->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_GE(retval.sequence, 0);
    ASSERT_EQ(retval.buffer, nullptr);

    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    ret = bq->FlushBuffer(retval.sequence, bedata, acquireFence, flushConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bq->FlushBuffer(retval.sequence, bedata, acquireFence, flushConfig);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
}

/*
* Function: AcquireBuffer and ReleaseBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call AcquireBuffer
*                  2. call ReleaseBuffer 2 times
*                  3. check ret
 */
HWTEST_F(BufferQueueTest, ReqCanFluAcqRel005, Function | MediumTest | Level2)
{
    sptr<SurfaceBuffer> buffer;

    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    GSError ret = bq->AcquireBuffer(buffer, acquireFence, timestamp, damages);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    sptr<SyncFence> ReleaseFence = SyncFence::INVALID_FENCE;
    ret = bq->ReleaseBuffer(buffer, ReleaseFence);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bq->ReleaseBuffer(buffer, ReleaseFence);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
}

/*
* Function: RequestBuffer, and CancelBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBuffer and CancelBuffer by different retval
*                  2. check ret
 */
HWTEST_F(BufferQueueTest, ReqCanFluAcqRel006, Function | MediumTest | Level2)
{
    IBufferProducer::RequestBufferReturnValue retval1;
    IBufferProducer::RequestBufferReturnValue retval2;
    IBufferProducer::RequestBufferReturnValue retval3;
    GSError ret;

    // not alloc
    ret = bq->RequestBuffer(requestConfig, bedata, retval1);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_GE(retval1.sequence, 0);
    ASSERT_EQ(retval1.buffer, nullptr);

    // alloc
    ret = bq->RequestBuffer(requestConfig, bedata, retval2);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_GE(retval2.sequence, 0);
    ASSERT_NE(retval2.buffer, nullptr);

    cache[retval2.sequence] = retval2.buffer;

    // no buffer
    ret = bq->RequestBuffer(requestConfig, bedata, retval3);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
    ASSERT_EQ(retval3.buffer, nullptr);

    ret = bq->CancelBuffer(retval1.sequence, bedata);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bq->CancelBuffer(retval2.sequence, bedata);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bq->CancelBuffer(retval3.sequence, bedata);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
}

/*
* Function: RequestBuffer, ReleaseBuffer and FlushBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBuffer
*                  2. call ReleaseBuffer
*                  3. call FlushBuffer
*                  4. check ret
 */
HWTEST_F(BufferQueueTest, ReqCanFluAcqRel007, Function | MediumTest | Level2)
{
    IBufferProducer::RequestBufferReturnValue retval;

    // not alloc
    GSError ret = bq->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_GE(retval.sequence, 0);
    ASSERT_EQ(retval.buffer, nullptr);

    retval.buffer = cache[retval.sequence];

    sptr<SyncFence> releaseFence = SyncFence::INVALID_FENCE;
    ret = bq->ReleaseBuffer(retval.buffer, releaseFence);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    ret = bq->FlushBuffer(retval.sequence, bedata, acquireFence, flushConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: AcquireBuffer, FlushBuffer and ReleaseBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call AcquireBuffer
*                  2. call FlushBuffer
*                  3. call ReleaseBuffer
*                  4. check ret
 */
HWTEST_F(BufferQueueTest, ReqCanFluAcqRel008, Function | MediumTest | Level2)
{
    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;

    // acq from last test
    GSError ret = bq->AcquireBuffer(buffer, acquireFence, timestamp, damages);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    uint32_t sequence;
    for (auto it = cache.begin(); it != cache.end(); it++) {
        if (it->second.GetRefPtr() == buffer.GetRefPtr()) {
            sequence = it->first;
        }
    }
    ASSERT_GE(sequence, 0);

    ret = bq->FlushBuffer(sequence, bedata, acquireFence, flushConfig);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    sptr<SyncFence> releaseFence = SyncFence::INVALID_FENCE;
    ret = bq->ReleaseBuffer(buffer, releaseFence);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: RequestBuffer and CancelBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBuffer
*                  2. call CancelBuffer
*                  3. check retval and ret
 */
HWTEST_F(BufferQueueTest, ReqCanFluAcqRel009, Function | MediumTest | Level2)
{
    IBufferProducer::RequestBufferReturnValue retval;
    BufferRequestConfig deleteconfig = requestConfig;
    deleteconfig.width = 1921;

    GSError ret = bq->RequestBuffer(deleteconfig, bedata, retval);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_EQ(retval.deletingBuffers.size(), 1u);
    ASSERT_GE(retval.sequence, 0);
    ASSERT_NE(retval.buffer, nullptr);

    ret = bq->CancelBuffer(retval.sequence, bedata);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: RequestBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. set BufferRequestConfig with abnormal value
*                  2. call RequestBuffer
*                  3. check ret
 */
HWTEST_F(BufferQueueTest, RequestBuffer001, Function | MediumTest | Level2)
{
    IBufferProducer::RequestBufferReturnValue retval;
    BufferRequestConfig config = requestConfig;
    config.width = -1;

    GSError ret = bq->RequestBuffer(config, bedata, retval);
    ASSERT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: RequestBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. set BufferRequestConfig with abnormal value
*                  2. call RequestBuffer
*                  3. check ret
 */
HWTEST_F(BufferQueueTest, RequestBuffer002, Function | MediumTest | Level2)
{
    IBufferProducer::RequestBufferReturnValue retval;
    BufferRequestConfig config = requestConfig;
    config.height = -1;

    GSError ret = bq->RequestBuffer(config, bedata, retval);
    ASSERT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);
}


/*
* Function: RequestBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. set BufferRequestConfig with abnormal value
*                  2. call RequestBuffer
*                  3. check ret
 */
HWTEST_F(BufferQueueTest, RequestBuffer003, Function | MediumTest | Level2)
{
    IBufferProducer::RequestBufferReturnValue retval;
    BufferRequestConfig config = requestConfig;
    config.strideAlignment = SURFACE_MIN_STRIDE_ALIGNMENT - 1;

    GSError ret = bq->RequestBuffer(config, bedata, retval);
    ASSERT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: RequestBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. set BufferRequestConfig with abnormal value
*                  2. call RequestBuffer
*                  3. check ret
 */
HWTEST_F(BufferQueueTest, RequestBuffer004, Function | MediumTest | Level2)
{
    IBufferProducer::RequestBufferReturnValue retval;
    BufferRequestConfig config = requestConfig;
    config.strideAlignment = SURFACE_MAX_STRIDE_ALIGNMENT + 1;

    GSError ret = bq->RequestBuffer(config, bedata, retval);
    ASSERT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: RequestBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. set BufferRequestConfig with abnormal value
*                  2. call RequestBuffer
*                  3. check ret
 */
HWTEST_F(BufferQueueTest, RequestBuffer005, Function | MediumTest | Level2)
{
    IBufferProducer::RequestBufferReturnValue retval;
    BufferRequestConfig config = requestConfig;
    config.strideAlignment = 3;

    GSError ret = bq->RequestBuffer(config, bedata, retval);
    ASSERT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: RequestBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. set BufferRequestConfig with abnormal value
*                  2. call RequestBuffer
*                  3. check ret
 */
HWTEST_F(BufferQueueTest, RequestBuffer006, Function | MediumTest | Level2)
{
    IBufferProducer::RequestBufferReturnValue retval;
    BufferRequestConfig config = requestConfig;
    config.format = -1;

    GSError ret = bq->RequestBuffer(config, bedata, retval);
    ASSERT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: QueryIfBufferAvailable
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call QueryIfBufferAvailable and check ret
 */
HWTEST_F(BufferQueueTest, QueryIfBufferAvailable001, Function | MediumTest | Level2)
{
    bq->CleanCache();
    bool ret = bq->QueryIfBufferAvailable();
    ASSERT_EQ(ret, true);

    GSError reqRet = OHOS::GSERROR_OK;
    IBufferProducer::RequestBufferReturnValue retval;
    BufferRequestConfig config = requestConfig;
    while (reqRet != OHOS::GSERROR_NO_BUFFER) {
        reqRet = bq->RequestBuffer(config, bedata, retval);
    }

    ret = bq->QueryIfBufferAvailable();
    ASSERT_EQ(ret, false);
}

/*
* Function: GetName
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetName and check ret
 */
HWTEST_F(BufferQueueTest, GetName001, Function | MediumTest | Level2)
{
    std::string name("na");
    GSError ret = bq->GetName(name);
    ASSERT_EQ(ret, GSERROR_OK);
    ASSERT_NE(name, "na");
}

/*
* Function: RegisterConsumerListener
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RegisterConsumerListener and check ret
 */
HWTEST_F(BufferQueueTest, RegisterConsumerListener001, Function | MediumTest | Level2)
{
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    GSError ret = bq->RegisterConsumerListener(listener);
    ASSERT_EQ(ret, GSERROR_OK);
}

/*
* Function: SetDefaultWidthAndHeight
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetDefaultWidthAndHeight and check ret
 */
HWTEST_F(BufferQueueTest, SetDefaultWidthAndHeight001, Function | MediumTest | Level2)
{
    int width = 0;
    int height = 0;
    GSError ret = bq->SetDefaultWidthAndHeight(width, height);
    ASSERT_EQ(ret, GSERROR_INVALID_ARGUMENTS);

    width = 1;
    ret = bq->SetDefaultWidthAndHeight(width, height);
    ASSERT_EQ(ret, GSERROR_INVALID_ARGUMENTS);

    width = 80;
    height = 80;
    ret = bq->SetDefaultWidthAndHeight(width, height);
    ASSERT_EQ(ret, GSERROR_OK);
}

/*
* Function: GetDefaultWidth and GetDefaultHeight
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetDefaultWidth and check ret
 */
HWTEST_F(BufferQueueTest, GetDefaultWidth001, Function | MediumTest | Level2)
{
    int32_t width = 80;
    int32_t height = 80;
    GSError ret = bq->SetDefaultWidthAndHeight(width, height);
    ASSERT_EQ(ret, GSERROR_OK);

    ASSERT_EQ(width, bq->GetDefaultWidth());
    ASSERT_EQ(height, bq->GetDefaultHeight());
}

/*
* Function: SetDefaultUsage and GetDefaultUsage
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetDefaultUsage and check ret
 */
HWTEST_F(BufferQueueTest, SetDefaultUsage001, Function | MediumTest | Level2)
{
    uint64_t usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA;
    GSError ret = bq->SetDefaultUsage(usage);
    ASSERT_EQ(ret, GSERROR_OK);
    ASSERT_EQ(usage, bq->GetDefaultUsage());
}

/*
* Function: CleanCache
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call CleanCache and check ret
 */
HWTEST_F(BufferQueueTest, CleanCache001, Function | MediumTest | Level2)
{
    GSError ret = bq->CleanCache();
    ASSERT_EQ(ret, GSERROR_OK);
}
/*
* Function: AttachBufferUpdateStatus
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call AttachBufferUpdateStatus and check ret
 */
HWTEST_F(BufferQueueTest, AttachBufferUpdateStatus, Function | MediumTest | Level2)
{
    uint32_t sequence = 2;
    int32_t timeOut = 6;
    std::mutex mutex_;
    std::unique_lock<std::mutex> lock(mutex_);
    GSError ret = bq->AttachBufferUpdateStatus(lock, sequence, timeOut);
    ASSERT_EQ(ret, GSERROR_OK);
}

/*
* Function: AttachBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call AttachBuffer, DetachBuffer and check ret
 */
HWTEST_F(BufferQueueTest, AttachBufferAndDetachBuffer001, Function | MediumTest | Level2)
{
    bq->CleanCache();
    int32_t timeOut = 6;
    IBufferProducer::RequestBufferReturnValue retval;
    GSError ret = bq->AttachBuffer(retval.buffer, timeOut);
    ASSERT_EQ(ret, GSERROR_INVALID_OPERATING);
    EXPECT_EQ(bq->DetachBuffer(retval.buffer), GSERROR_INVALID_ARGUMENTS);
    sptr<SurfaceBuffer> buffer = nullptr;
    EXPECT_EQ(bq->DetachBuffer(buffer), GSERROR_INVALID_ARGUMENTS);
    BufferQueue *bqTmp = new BufferQueue("testTmp", true);
    EXPECT_EQ(bqTmp->DetachBuffer(buffer), GSERROR_INVALID_OPERATING);
    bqTmp = nullptr;
}

/*
* Function: AttachBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call AttachBuffer, DetachBuffer and check ret
 */
HWTEST_F(BufferQueueTest, AttachBufferAndDetachBuffer002, Function | MediumTest | Level2)
{
    bq->CleanCache();
    int32_t timeOut = 6;
    EXPECT_EQ(bq->SetQueueSize(SURFACE_MAX_QUEUE_SIZE), GSERROR_OK);
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    ASSERT_NE(buffer, nullptr);
    GSError ret = bq->AttachBuffer(buffer, timeOut);
    sptr<SurfaceBuffer> buffer1 = SurfaceBuffer::Create();
    EXPECT_EQ(bq->GetUsedSize(), 1);
    ASSERT_EQ(ret, GSERROR_OK);
    EXPECT_EQ(bq->AttachBuffer(buffer1, timeOut), GSERROR_OK);
    ret= bq->DetachBuffer(buffer);
    EXPECT_EQ(ret, GSERROR_NO_ENTRY);
    EXPECT_EQ(bq->DetachBuffer(buffer1), GSERROR_NO_ENTRY);
    EXPECT_EQ(bq->AllocBuffer(buffer1, requestConfig), GSERROR_OK);
    EXPECT_EQ(bq->DetachBuffer(buffer1), GSERROR_OK);
}

/*
* Function: RegisterSurfaceDelegator
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RegisterSurfaceDelegator and check ret
 */
HWTEST_F(BufferQueueTest, RegisterSurfaceDelegator001, Function | MediumTest | Level2)
{
    surfaceDelegator = ProducerSurfaceDelegator::Create();
    GSError ret = bq->RegisterSurfaceDelegator(surfaceDelegator->AsObject(), csurface1);
    ASSERT_EQ(ret, GSERROR_OK);
}

/*
* Function: RegisterDeleteBufferListener
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RegisterDeleteBufferListener and check ret
 */
HWTEST_F(BufferQueueTest, RegisterDeleteBufferListener001, Function | MediumTest | Level2)
{
    surfaceDelegator = ProducerSurfaceDelegator::Create();
    GSError ret = bq->RegisterDeleteBufferListener(nullptr, true);
    ASSERT_EQ(ret, GSERROR_OK);
}

/*
* Function: DumpToFile
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call DumpToFile and check ret
 */
HWTEST_F(BufferQueueTest, DumpToFile001, Function | MediumTest | Level2)
{
    bq->CleanCache();
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    ASSERT_NE(buffer, nullptr);
    GSError ret = bq->AttachBuffer(buffer, 6);
    sptr<SurfaceBuffer> buffer1 = SurfaceBuffer::Create();
    ASSERT_EQ(ret, GSERROR_OK);
    bq->DumpToFile(0);
    bq->DumpToFile(1);
}
}
