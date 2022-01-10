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
#include <display_type.h>
#include <surface.h>
#include <buffer_extra_data_impl.h>
#include <buffer_queue.h>
#include <buffer_manager.h>
#include "buffer_consumer_listener.h"

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
        .format = PIXEL_FMT_RGBA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
        .timeout = 0,
    };
    static inline BufferFlushConfig flushConfig = {
        .damage = {
            .w = 0x100,
            .h = 0x100,
        },
    };
    static inline int64_t timestamp = 0;
    static inline Rect damage = {};
    static inline sptr<BufferQueue> bq = nullptr;
    static inline std::map<int32_t, sptr<SurfaceBuffer>> cache;
    static inline BufferExtraDataImpl bedata;
};

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

    ret = bq->FlushBuffer(retval.sequence, bedata, -1, flushConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    sptr<SurfaceBufferImpl> bufferImpl = SurfaceBufferImpl::FromBase(retval.buffer);
    ret = bq->AcquireBuffer(bufferImpl, retval.fence, timestamp, damage);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(retval.buffer, nullptr);

    uint8_t *addr2 = reinterpret_cast<uint8_t*>(retval.buffer->GetVirAddr());
    ASSERT_NE(addr2, nullptr);
    if (addr2 != nullptr) {
        ASSERT_EQ(addr2[0], 5u);
    }

    ret = bq->ReleaseBuffer(bufferImpl, -1);
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

    ret = bq->FlushBuffer(retval.sequence, bedata, -1, flushConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bq->FlushBuffer(retval.sequence, bedata, -1, flushConfig);
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
    sptr<SurfaceBufferImpl> buffer;
    int32_t flushFence;

    GSError ret = bq->AcquireBuffer(buffer, flushFence, timestamp, damage);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bq->ReleaseBuffer(buffer, -1);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bq->ReleaseBuffer(buffer, -1);
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

    sptr<SurfaceBufferImpl> bufferImpl = SurfaceBufferImpl::FromBase(retval.buffer);
    ret = bq->ReleaseBuffer(bufferImpl, -1);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ret = bq->FlushBuffer(retval.sequence, bedata, -1, flushConfig);
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
    sptr<SurfaceBufferImpl> bufferImpl;
    int32_t flushFence;

    // acq from last test
    GSError ret = bq->AcquireBuffer(bufferImpl, flushFence, timestamp, damage);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    int32_t sequence;
    for (auto it = cache.begin(); it != cache.end(); it++) {
        if (it->second.GetRefPtr() == bufferImpl.GetRefPtr()) {
            sequence = it->first;
        }
    }
    ASSERT_GE(sequence, 0);

    ret = bq->FlushBuffer(sequence, bedata, -1, flushConfig);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ret = bq->ReleaseBuffer(bufferImpl, -1);
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
    config.width = SURFACE_MAX_WIDTH + 1;

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
HWTEST_F(BufferQueueTest, RequestBuffer004, Function | MediumTest | Level2)
{
    IBufferProducer::RequestBufferReturnValue retval;
    BufferRequestConfig config = requestConfig;
    config.height = SURFACE_MAX_HEIGHT + 1;

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
HWTEST_F(BufferQueueTest, RequestBuffer006, Function | MediumTest | Level2)
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
HWTEST_F(BufferQueueTest, RequestBuffer007, Function | MediumTest | Level2)
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
HWTEST_F(BufferQueueTest, RequestBuffer008, Function | MediumTest | Level2)
{
    IBufferProducer::RequestBufferReturnValue retval;
    BufferRequestConfig config = requestConfig;
    config.format = -1;

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
HWTEST_F(BufferQueueTest, RequestBuffer009, Function | MediumTest | Level2)
{
    IBufferProducer::RequestBufferReturnValue retval;
    BufferRequestConfig config = requestConfig;
    config.format = PIXEL_FMT_BUTT + 1;

    GSError ret = bq->RequestBuffer(config, bedata, retval);
    ASSERT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);
}
}
