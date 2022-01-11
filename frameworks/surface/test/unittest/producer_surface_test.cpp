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
#include <securec.h>
#include <gtest/gtest.h>
#include <display_type.h>
#include <surface.h>
#include <consumer_surface.h>
#include "buffer_consumer_listener.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class ProducerSurfaceTest : public testing::Test {
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
    static inline sptr<Surface> csurf = nullptr;
    static inline sptr<IBufferProducer> producer = nullptr;
    static inline sptr<Surface> psurf = nullptr;
};

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

/*
* Function: ProducerSurface
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. check psurf
 */
HWTEST_F(ProducerSurfaceTest, ProducerSurface001, Function | MediumTest | Level2)
{
    ASSERT_NE(psurf, nullptr);
}

/*
* Function: SetQueueSize and GetQueueSize
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetQueueSize and get default value
*                  2. call SetQueueSize
*                  3. call SetQueueSize again with abnormal value
*                  4. call GetQueueSize
*                  5. check ret
 */
HWTEST_F(ProducerSurfaceTest, QueueSize001, Function | MediumTest | Level2)
{
    ASSERT_EQ(psurf->GetQueueSize(), (uint32_t)SURFACE_DEFAULT_QUEUE_SIZE);
    GSError ret = psurf->SetQueueSize(2);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = psurf->SetQueueSize(SURFACE_MAX_QUEUE_SIZE + 1);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ASSERT_EQ(psurf->GetQueueSize(), 2u);
}

/*
* Function: RequestBufferNoFence and FlushBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBufferNoFence
*                  2. call FlushBuffer
*                  3. check ret
 */
HWTEST_F(ProducerSurfaceTest, ReqCanFluAcqRel001, Function | MediumTest | Level2)
{
    sptr<SurfaceBuffer> buffer;

    GSError ret = psurf->RequestBufferNoFence(buffer, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(buffer, nullptr);

    ret = psurf->FlushBuffer(buffer, -1, flushConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: RequestBufferNoFence and FlushBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBufferNoFence
*                  2. call FlushBuffer 2 times
*                  3. check ret
 */
HWTEST_F(ProducerSurfaceTest, ReqCanFluAcqRel002, Function | MediumTest | Level2)
{
    sptr<SurfaceBuffer> buffer;

    GSError ret = psurf->RequestBufferNoFence(buffer, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(buffer, nullptr);

    ret = psurf->FlushBuffer(buffer, -1, flushConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = psurf->FlushBuffer(buffer, -1, flushConfig);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
}

/*
* Function: AcquireBuffer and ReleaseBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call AcquireBuffer and ReleaseBuffer many times
*                  2. check ret
 */
HWTEST_F(ProducerSurfaceTest, ReqCanFluAcqRel003, Function | MediumTest | Level2)
{
    sptr<SurfaceBuffer> buffer;
    int32_t flushFence;

    GSError ret = psurf->AcquireBuffer(buffer, flushFence, timestamp, damage);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ret = psurf->ReleaseBuffer(buffer, -1);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ret = csurf->AcquireBuffer(buffer, flushFence, timestamp, damage);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = csurf->ReleaseBuffer(buffer, -1);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = csurf->AcquireBuffer(buffer, flushFence, timestamp, damage);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = csurf->ReleaseBuffer(buffer, -1);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: RequestBufferNoFence and CancelBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBufferNoFence
*                  2. call CancelBuffer
*                  3. check ret
 */
HWTEST_F(ProducerSurfaceTest, ReqCanFluAcqRel004, Function | MediumTest | Level2)
{
    sptr<SurfaceBuffer> buffer;

    GSError ret = psurf->RequestBufferNoFence(buffer, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = psurf->CancelBuffer(buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: RequestBufferNoFence and CancelBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBufferNoFence
*                  2. call CancelBuffer 2 times
*                  3. check ret
 */
HWTEST_F(ProducerSurfaceTest, ReqCanFluAcqRel005, Function | MediumTest | Level2)
{
    sptr<SurfaceBuffer> buffer;

    GSError ret = psurf->RequestBufferNoFence(buffer, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = psurf->CancelBuffer(buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = psurf->CancelBuffer(buffer);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
}

/*
* Function: RequestBufferNoFence and CancelBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBufferNoFence and CancelBuffer many times
*                  2. check ret
 */
HWTEST_F(ProducerSurfaceTest, ReqCanFluAcqRel006, Function | MediumTest | Level2)
{
    sptr<SurfaceBuffer> buffer;
    sptr<SurfaceBuffer> buffer1;
    sptr<SurfaceBuffer> buffer2;

    GSError ret = psurf->RequestBufferNoFence(buffer, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = psurf->RequestBufferNoFence(buffer1, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = psurf->RequestBufferNoFence(buffer2, requestConfig);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ret = psurf->CancelBuffer(buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = psurf->CancelBuffer(buffer1);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = psurf->CancelBuffer(buffer2);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
}

/*
* Function: GetQueueSize and SetQueueSize
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetQeueSize
*                  2. call SetQueueSize 2 times
*                  3. check ret
 */
HWTEST_F(ProducerSurfaceTest, SetQueueSizeDeleting001, Function | MediumTest | Level2)
{
    sptr<ConsumerSurface> cs = static_cast<ConsumerSurface*>(csurf.GetRefPtr());
    sptr<BufferQueueProducer> bqp = static_cast<BufferQueueProducer*>(cs->producer_.GetRefPtr());
    ASSERT_EQ(bqp->bufferQueue_->GetQueueSize(), 2u);

    GSError ret = psurf->SetQueueSize(1);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = psurf->SetQueueSize(2);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: RequestBufferNoFence, ReleaseBuffer and CancelBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBufferNoFence
*                  2. call ReleaseBuffer
*                  3. call CancelBuffer
*                  4. check ret
 */
HWTEST_F(ProducerSurfaceTest, ReqCanFluAcqRel007, Function | MediumTest | Level2)
{
    sptr<SurfaceBuffer> buffer;

    GSError ret = psurf->RequestBufferNoFence(buffer, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = psurf->ReleaseBuffer(buffer, -1);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ret = psurf->CancelBuffer(buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: SetUserData and GetUserData
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetUserData and GetUserData many times
*                  2. check ret
 */
HWTEST_F(ProducerSurfaceTest, UserData001, Function | MediumTest | Level2)
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
        ASSERT_EQ(ret, OHOS::GSERROR_OK);
    }

    ret = psurf->SetUserData("-1", "error");
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    std::string retStr;
    for (int i = 0; i < SURFACE_MAX_USER_DATA_COUNT; i++) {
        retStr = psurf->GetUserData(strs[i]);
        ASSERT_EQ(retStr, "magic");
    }
}

/*
* Function: RegisterConsumerListener
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RegisterConsumerListener
*                  2. check ret
 */
HWTEST_F(ProducerSurfaceTest, RegisterConsumerListener001, Function | MediumTest | Level2)
{
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    GSError ret = psurf->RegisterConsumerListener(listener);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
}

/*
* Function: GetUniqueId
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetUniqueId
*                  2. check ret
 */
HWTEST_F(ProducerSurfaceTest, UniqueId001, Function | MediumTest | Level2)
{
    uint64_t uniqueId = psurf->GetUniqueId();
    ASSERT_NE(uniqueId, 0);
}
}
