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
#include <gtest/gtest.h>
#include <surface.h>
#include <buffer_extra_data_impl.h>
#include <buffer_queue_producer.h>
#include "buffer_consumer_listener.h"
#include "consumer_surface.h"
#include "frame_report.h"
#include "sync_fence.h"
#include "producer_surface_delegator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class BufferQueueProducerTest : public testing::Test {
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
    static inline std::vector<int32_t> deletingBuffers;
    static inline int64_t timestamp = 0;
    static inline std::vector<Rect> damages = {};
    static inline sptr<BufferQueue> bq = nullptr;
    static inline sptr<BufferQueueProducer> bqp = nullptr;
    static inline sptr<BufferExtraData> bedata = nullptr;
};

void BufferQueueProducerTest::SetUpTestCase()
{
    bq = new BufferQueue("test");
    bq->Init();
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    bq->RegisterConsumerListener(listener);
    bqp = new BufferQueueProducer(bq);
    bedata = new OHOS::BufferExtraDataImpl;
}

void BufferQueueProducerTest::TearDownTestCase()
{
    bq = nullptr;
    bqp = nullptr;
}

/*
* Function: SetQueueSize and GetQueueSize
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetQueueSize and get default value
*                  2. call SetQueueSize
*                  3. call SetQueueSize again with abnormal value
*                  4. call GetQueueSize for BufferQueueProducer and BufferQueue
*                  5. check ret
 */
HWTEST_F(BufferQueueProducerTest, QueueSize001, Function | MediumTest | Level2)
{
    ASSERT_EQ(bqp->GetQueueSize(), (uint32_t)SURFACE_DEFAULT_QUEUE_SIZE);

    GSError ret = bqp->SetQueueSize(2);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bqp->SetQueueSize(SURFACE_MAX_QUEUE_SIZE + 1);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ASSERT_EQ(bqp->GetQueueSize(), 2u);
    ASSERT_EQ(bq->GetQueueSize(), 2u);
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
HWTEST_F(BufferQueueProducerTest, ReqCan001, Function | MediumTest | Level2)
{
    IBufferProducer::RequestBufferReturnValue retval;
    GSError ret = bqp->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bqp->CancelBuffer(retval.sequence, bedata);
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
HWTEST_F(BufferQueueProducerTest, ReqCan002, Function | MediumTest | Level2)
{
    IBufferProducer::RequestBufferReturnValue retval;
    GSError ret = bqp->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bqp->CancelBuffer(retval.sequence, bedata);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bqp->CancelBuffer(retval.sequence, bedata);
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
HWTEST_F(BufferQueueProducerTest, ReqCan003, Function | MediumTest | Level2)
{
    IBufferProducer::RequestBufferReturnValue retval1;
    IBufferProducer::RequestBufferReturnValue retval2;
    IBufferProducer::RequestBufferReturnValue retval3;

    auto ret = bqp->RequestBuffer(requestConfig, bedata, retval1);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_EQ(retval1.buffer, nullptr);

    ret = bqp->RequestBuffer(requestConfig, bedata, retval2);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(retval2.buffer, nullptr);

    ret = bqp->RequestBuffer(requestConfig, bedata, retval3);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
    ASSERT_EQ(retval3.buffer, nullptr);

    ret = bqp->CancelBuffer(retval1.sequence, bedata);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bqp->CancelBuffer(retval2.sequence, bedata);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bqp->CancelBuffer(retval3.sequence, bedata);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
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
HWTEST_F(BufferQueueProducerTest, ReqFlu001, Function | MediumTest | Level2)
{
    Rosen::FrameReport::GetInstance().SetGameScene(true);
    IBufferProducer::RequestBufferReturnValue retval;
    GSError ret = bqp->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    ret = bqp->FlushBuffer(retval.sequence, bedata, acquireFence, flushConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bq->AcquireBuffer(retval.buffer, retval.fence, timestamp, damages);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    sptr<SyncFence> releaseFence = SyncFence::INVALID_FENCE;
    ret = bq->ReleaseBuffer(retval.buffer, releaseFence);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: RequestBuffer, FlushBuffer, AcquireBuffer and ReleaseBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBuffer and FlushBuffer
*                  2. call FlushBuffer again
*                  3. call AcquireBuffer and ReleaseBuffer
*                  4. check ret
 */
HWTEST_F(BufferQueueProducerTest, ReqFlu002, Function | MediumTest | Level2)
{
    IBufferProducer::RequestBufferReturnValue retval;
    GSError ret = bqp->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    ret = bqp->FlushBuffer(retval.sequence, bedata, acquireFence, flushConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bqp->FlushBuffer(retval.sequence, bedata, acquireFence, flushConfig);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ret = bq->AcquireBuffer(retval.buffer, retval.fence, timestamp, damages);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    sptr<SyncFence> releaseFence = SyncFence::INVALID_FENCE;
    ret = bq->ReleaseBuffer(retval.buffer, releaseFence);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    int32_t timeOut = 1;
    ret = bqp->AttachBuffer(retval.buffer, timeOut);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: AttachBuffer and DetachBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call AttachBuffer and DetachBuffer
* 4. check ret
*/
HWTEST_F(BufferQueueProducerTest, AttachAndDetachBuffer001, Function | MediumTest | Level2)
{
    IBufferProducer::RequestBufferReturnValue retVal;
    sptr<SurfaceBuffer> &buffer = retVal.buffer;

    sptr<BufferQueue> bufferQueue = nullptr;
    auto bqpTmp = new BufferQueueProducer(bufferQueue);

    GSError ret = bqpTmp->AttachBuffer(buffer);
    ASSERT_EQ(ret, GSERROR_INVALID_ARGUMENTS);
    ret = bqpTmp->DetachBuffer(buffer);
    ASSERT_EQ(ret, GSERROR_INVALID_ARGUMENTS);
    bqpTmp = nullptr;
}

/*
* Function: AttachAndDetachBufferRemote
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call AttachBufferRemote, DetachBufferRemote
* 4. check ret
*/
HWTEST_F(BufferQueueProducerTest, AttachAndDetachBufferRemote, Function | MediumTest | Level2)
{
    MessageParcel arguments;
    arguments.WriteInt32(5);
    MessageParcel reply;
    reply.WriteInt32(6);
    MessageOption option;
    int32_t ret = bqp->AttachBufferRemote(arguments, reply, option);
    ASSERT_EQ(ret, 0);
    ret = bqp->DetachBufferRemote(arguments, reply, option);
    ASSERT_EQ(ret, 0);
}

/*
* Function: SetTunnelHandle
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetTunnelHandle
* 4. check ret
*/
HWTEST_F(BufferQueueProducerTest, SetTunnelHandle, Function | MediumTest | Level2)
{
    EXPECT_EQ(bqp->SetTunnelHandle(nullptr), GSERROR_OK);
    GraphicExtDataHandle *handle = static_cast<GraphicExtDataHandle *>(
        malloc(sizeof(GraphicExtDataHandle) + sizeof(int32_t)));
    handle->fd = -1;
    handle->reserveInts = 1;
    handle->reserve[0] = 0;
    GSError ret = bqp->SetTunnelHandle(handle);
    EXPECT_EQ(ret, GSERROR_NO_ENTRY);
    free(handle);
}

/*
* Function: SetTunnelHandleRemote
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetTunnelHandleRemote
* 4. check ret
*/
HWTEST_F(BufferQueueProducerTest, SetTunnelHandleRemote, Function | MediumTest | Level2)
{
    MessageParcel arguments;
    arguments.WriteInt32(5);
    MessageParcel reply;
    reply.WriteInt32(6);
    MessageOption option;
    int32_t ret = bqp->SetTunnelHandleRemote(arguments, reply, option);
    ASSERT_EQ(ret, 0);
}

/*
* Function: GetPresentTimestampRemote
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetPresentTimestampRemote
* 4. check ret
*/
HWTEST_F(BufferQueueProducerTest, GetPresentTimestampRemote, Function | MediumTest | Level2)
{
    MessageParcel arguments;
    arguments.WriteInt32(5);
    MessageParcel reply;
    reply.WriteInt32(6);
    MessageOption option;
    int32_t ret = bqp->GetPresentTimestampRemote(arguments, reply, option);
    ASSERT_EQ(ret, 0);
}

/*
* Function: BufferQueueProducer member function nullptr test
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: BufferQueueProducer member function nullptr test
 */
HWTEST_F(BufferQueueProducerTest, NullTest, Function | MediumTest | Level2)
{
    sptr<BufferQueue> bqTmp = nullptr;
    auto bqpTmp = new BufferQueueProducer(bqTmp);
    IBufferProducer::RequestBufferReturnValue retval;
    GSError ret = bqpTmp->RequestBuffer(requestConfig, bedata, retval);
    EXPECT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);
    ret = bqpTmp->CancelBuffer(retval.sequence, bedata);
    EXPECT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    ret = bqpTmp->FlushBuffer(retval.sequence, bedata, acquireFence, flushConfig);
    EXPECT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);
    ret = bqpTmp->GetLastFlushedBuffer(retval.buffer, acquireFence, nullptr, 0);
    EXPECT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);
    EXPECT_EQ(bqpTmp->AttachBuffer(retval.buffer), OHOS::GSERROR_INVALID_ARGUMENTS);
    EXPECT_EQ(bqpTmp->DetachBuffer(retval.buffer), OHOS::GSERROR_INVALID_ARGUMENTS);
    EXPECT_EQ(bqpTmp->GetQueueSize(), 0);
    EXPECT_EQ(bqpTmp->SetQueueSize(0), OHOS::GSERROR_INVALID_ARGUMENTS);
    std::string name = "test";
    EXPECT_EQ(bqpTmp->GetName(name), OHOS::GSERROR_INVALID_ARGUMENTS);
    EXPECT_EQ(bqpTmp->GetDefaultWidth(), 0);
    EXPECT_EQ(bqpTmp->GetDefaultHeight(), 0);
    EXPECT_EQ(bqpTmp->GetDefaultUsage(), 0);
    EXPECT_EQ(bqpTmp->GetUniqueId(), 0);
    sptr<IProducerListener> listener = nullptr;
    EXPECT_EQ(bqpTmp->RegisterReleaseListener(listener), OHOS::GSERROR_INVALID_ARGUMENTS);
    EXPECT_EQ(bqpTmp->UnRegisterReleaseListener(), OHOS::GSERROR_INVALID_ARGUMENTS);
    EXPECT_EQ(bqpTmp->SetTransform(GraphicTransformType::GRAPHIC_FLIP_H), OHOS::GSERROR_INVALID_ARGUMENTS);
    std::vector<BufferVerifyAllocInfo> infos;
    std::vector<bool> supporteds;
    EXPECT_EQ(bqpTmp->IsSupportedAlloc(infos, supporteds), OHOS::GSERROR_INVALID_ARGUMENTS);
    uint64_t uniqueId;
    EXPECT_EQ(bqpTmp->GetNameAndUniqueId(name, uniqueId), OHOS::GSERROR_INVALID_ARGUMENTS);
    EXPECT_EQ(bqpTmp->Disconnect(), OHOS::GSERROR_INVALID_ARGUMENTS);
    EXPECT_EQ(bqpTmp->SetScalingMode(0, ScalingMode::SCALING_MODE_FREEZE), OHOS::GSERROR_INVALID_ARGUMENTS);
    std::vector<GraphicHDRMetaData> meta;
    EXPECT_EQ(bqpTmp->SetMetaData(0, meta), OHOS::GSERROR_INVALID_ARGUMENTS);
    std::vector<uint8_t> metaData;
    EXPECT_EQ(bqpTmp->SetMetaDataSet(0, GraphicHDRMetadataKey::GRAPHIC_MATAKEY_BLUE_PRIMARY_X,
        metaData), OHOS::GSERROR_INVALID_ARGUMENTS);
    sptr<SurfaceTunnelHandle> handle = nullptr;
    EXPECT_EQ(bqpTmp->SetTunnelHandle(handle), OHOS::GSERROR_INVALID_ARGUMENTS);
    int64_t time = 0;
    EXPECT_EQ(bqpTmp->GetPresentTimestamp(0, GraphicPresentTimestampType::GRAPHIC_DISPLAY_PTS_DELAY,
        time), OHOS::GSERROR_INVALID_ARGUMENTS);
    EXPECT_EQ(bqpTmp->GetStatus(), false);
    bqpTmp->SetStatus(false);
    bqTmp = nullptr;
    bqpTmp = nullptr;
}
}
