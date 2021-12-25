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

#include "consumer_surface_test.h"

#include <securec.h>

#include "buffer_consumer_listener.h"
#include "buffer_queue_producer.h"
#include "consumer_surface.h"

namespace OHOS {
void ConsumerSurfaceTest::SetUpTestCase()
{
    cs = Surface::CreateSurfaceAsConsumer();
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    cs->RegisterConsumerListener(listener);
    auto p = cs->GetProducer();
    ps = Surface::CreateSurfaceAsProducer(p);
}

void ConsumerSurfaceTest::TearDownTestCase()
{
    cs = nullptr;
}

namespace {
HWTEST_F(ConsumerSurfaceTest, ConsumerSurface, testing::ext::TestSize.Level0)
{
    ASSERT_NE(cs, nullptr);

    sptr<ConsumerSurface> qs = static_cast<ConsumerSurface*>(cs.GetRefPtr());
    ASSERT_NE(qs, nullptr);
    ASSERT_NE(qs->producer_, nullptr);
    ASSERT_NE(qs->consumer_, nullptr);
}

HWTEST_F(ConsumerSurfaceTest, QueueSize, testing::ext::TestSize.Level0)
{
    ASSERT_EQ(cs->GetQueueSize(), (uint32_t)SURFACE_DEFAULT_QUEUE_SIZE);
    GSError ret = cs->SetQueueSize(2);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = cs->SetQueueSize(SURFACE_MAX_QUEUE_SIZE + 1);
    ASSERT_NE(ret, GSERROR_OK);

    ASSERT_EQ(cs->GetQueueSize(), 2u);
}

HWTEST_F(ConsumerSurfaceTest, ReqFluReqFlu, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;

    GSError ret = cs->RequestBufferNoFence(buffer, requestConfig);
    ASSERT_NE(ret, GSERROR_OK);

    ret = ps->RequestBufferNoFence(buffer, requestConfig);
    ASSERT_EQ(ret, GSERROR_OK);
    ASSERT_NE(buffer, nullptr);

    ret = cs->FlushBuffer(buffer, -1, flushConfig);
    ASSERT_NE(ret, GSERROR_OK);

    ret = ps->FlushBuffer(buffer, -1, flushConfig);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = ps->RequestBufferNoFence(buffer, requestConfig);
    ASSERT_EQ(ret, GSERROR_OK);
    ret = ps->FlushBuffer(buffer, -1, flushConfig);
    ASSERT_EQ(ret, GSERROR_OK);
}

HWTEST_F(ConsumerSurfaceTest, AcqRel, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;
    int32_t flushFence;

    GSError ret = cs->AcquireBuffer(buffer, flushFence, timestamp, damage);
    ASSERT_EQ(ret, GSERROR_OK);
    ASSERT_NE(buffer, nullptr);

    ret = cs->ReleaseBuffer(buffer, -1);
    ASSERT_EQ(ret, GSERROR_OK);
}

HWTEST_F(ConsumerSurfaceTest, AcqRelRel, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;
    int32_t flushFence;

    GSError ret = cs->AcquireBuffer(buffer, flushFence, timestamp, damage);
    ASSERT_EQ(ret, GSERROR_OK);
    ASSERT_NE(buffer, nullptr);

    ret = cs->ReleaseBuffer(buffer, -1);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = cs->ReleaseBuffer(buffer, -1);
    ASSERT_NE(ret, GSERROR_OK);
}

HWTEST_F(ConsumerSurfaceTest, ReqCan, testing::ext::TestSize.Level0)
{
    sptr<SurfaceBuffer> buffer;
    GSError ret = cs->RequestBufferNoFence(buffer, requestConfig);
    ASSERT_NE(ret, GSERROR_OK);

    ret = ps->RequestBufferNoFence(buffer, requestConfig);
    ASSERT_EQ(ret, GSERROR_OK);

    ret = cs->CancelBuffer(buffer);
    ASSERT_NE(ret, GSERROR_OK);

    ret = ps->CancelBuffer(buffer);
    ASSERT_EQ(ret, GSERROR_OK);
}

HWTEST_F(ConsumerSurfaceTest, SetQueueSizeDeleting, testing::ext::TestSize.Level0)
{
    sptr<ConsumerSurface> qs = static_cast<ConsumerSurface*>(cs.GetRefPtr());
    sptr<BufferQueueProducer> bqp = static_cast<BufferQueueProducer*>(qs->producer_.GetRefPtr());
    ASSERT_EQ(bqp->bufferQueue_->queueSize_, 2u);
    ASSERT_EQ(bqp->bufferQueue_->freeList_.size(), 2u);

    GSError ret = cs->SetQueueSize(1);
    ASSERT_EQ(ret, GSERROR_OK);
    ASSERT_EQ(bqp->bufferQueue_->freeList_.size(), 1u);

    ret = cs->SetQueueSize(2);
    ASSERT_EQ(ret, GSERROR_OK);
    ASSERT_EQ(bqp->bufferQueue_->freeList_.size(), 1u);
}

HWTEST_F(ConsumerSurfaceTest, UserData, testing::ext::TestSize.Level0)
{
    GSError ret;

    std::string strs[SURFACE_MAX_USER_DATA_COUNT];
    constexpr int32_t stringLengthMax = 32;
    char str[stringLengthMax] = {};
    for (int i = 0; i < SURFACE_MAX_USER_DATA_COUNT; i++) {
        auto secRet = snprintf_s(str, sizeof(str), sizeof(str) - 1, "%d", i);
        ASSERT_GT(secRet, 0);

        strs[i] = str;
        ret = cs->SetUserData(strs[i], "magic");
        ASSERT_EQ(ret, GSERROR_OK);
    }

    ret = cs->SetUserData("-1", "error");
    ASSERT_NE(ret, GSERROR_OK);

    std::string retStr;
    for (int i = 0; i < SURFACE_MAX_USER_DATA_COUNT; i++) {
        retStr = cs->GetUserData(strs[i]);
        ASSERT_EQ(retStr, "magic");
    }
}

HWTEST_F(ConsumerSurfaceTest, RegisterConsumerListener, testing::ext::TestSize.Level0)
{
    class TestConsumerListener : public IBufferConsumerListener {
    public:
        void OnBufferAvailable() override
        {
            sptr<SurfaceBuffer> buffer;
            int32_t flushFence;

            cs->AcquireBuffer(buffer, flushFence, timestamp, damage);
            int32_t *p = (int32_t*)buffer->GetVirAddr();
            if (p != nullptr) {
                for (int32_t i = 0; i < 128; i++) {
                    ASSERT_EQ(p[i], i);
                }
            }

            cs->ReleaseBuffer(buffer, -1);
        }
    };
    sptr<IBufferConsumerListener> listener = new TestConsumerListener();
    GSError ret = cs->RegisterConsumerListener(listener);
    ASSERT_EQ(ret, GSERROR_OK);

    sptr<SurfaceBuffer> buffer;
    ret = ps->RequestBufferNoFence(buffer, requestConfig);
    ASSERT_EQ(ret, GSERROR_OK);
    ASSERT_NE(buffer, nullptr);

    int32_t *p = (int32_t*)buffer->GetVirAddr();
    if (p != nullptr) {
        for (int32_t i = 0; i < 128; i++) {
            p[i] = i;
        }
    }

    ret = ps->FlushBuffer(buffer, -1, flushConfig);
    ASSERT_EQ(ret, GSERROR_OK);
}

HWTEST_F(ConsumerSurfaceTest, RegisterConsumerListenerWithParam, testing::ext::TestSize.Level0)
{
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    GSError ret = cs->RegisterConsumerListener(listener);
    ASSERT_EQ(ret, GSERROR_OK);

    sptr<SurfaceBuffer> buffer;
    ret = ps->RequestBufferNoFence(buffer, requestConfig);
    ASSERT_EQ(ret, GSERROR_OK);
    ASSERT_NE(buffer, nullptr);

    int32_t *p = (int32_t*)buffer->GetVirAddr();
    if (p != nullptr) {
        for (int32_t i = 0; i < requestConfig.width * requestConfig.height; i++) {
            p[i] = i;
        }
    }

    ret = ps->FlushBuffer(buffer, -1, flushConfig);
    ASSERT_EQ(ret, GSERROR_OK);
}
}
} // namespace OHOS
