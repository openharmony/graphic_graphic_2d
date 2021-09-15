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

#include "buffer_client_producer_remote_test.h"

#include <vector>

#include <sys/wait.h>
#include <unistd.h>

#include "buffer_client_producer.h"
#include "buffer_consumer_listener.h"
#include "buffer_queue_producer.h"

namespace OHOS {
void BufferClientProducerRemoteTest::SetUpTestCase()
{
    pipe(pipeFd);

    pid = fork();
    if (pid < 0) {
        exit(1);
    }

    if (pid == 0) {
        sptr<BufferQueue> bq = new BufferQueue("test");
        ASSERT_NE(bq, nullptr);

        sptr<BufferQueueProducer> bqp = new BufferQueueProducer(bq);
        ASSERT_NE(bqp, nullptr);

        bq->Init();
        sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
        bq->RegisterConsumerListener(listener);

        auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        sam->AddSystemAbility(systemAbilityID, bqp);

        char buf[10] = "start";
        write(pipeFd[1], buf, sizeof(buf));

        sleep(0);

        read(pipeFd[0], buf, sizeof(buf));

        sam->RemoveSystemAbility(systemAbilityID);

        exit(0);
    } else {
        char buf[10];
        read(pipeFd[0], buf, sizeof(buf));

        auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        robj = sam->GetSystemAbility(systemAbilityID);
        bp = iface_cast<IBufferProducer>(robj);
    }
}

void BufferClientProducerRemoteTest::TearDownTestCase()
{
    bp = nullptr;
    robj = nullptr;

    char buf[10] = "over";
    write(pipeFd[1], buf, sizeof(buf));

    waitpid(pid, nullptr, 0);
}

namespace {
HWTEST_F(BufferClientProducerRemoteTest, IsProxy, testing::ext::TestSize.Level0)
{
    ASSERT_TRUE(robj->IsProxyObject());
}

HWTEST_F(BufferClientProducerRemoteTest, QueueSize, testing::ext::TestSize.Level0)
{
    ASSERT_EQ(bp->GetQueueSize(), (uint32_t)SURFACE_DEFAULT_QUEUE_SIZE);

    SurfaceError ret = bp->SetQueueSize(2);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bp->SetQueueSize(SURFACE_MAX_QUEUE_SIZE + 1);
    ASSERT_NE(ret, SURFACE_ERROR_OK);

    ASSERT_EQ(bp->GetQueueSize(), 2u);
}

HWTEST_F(BufferClientProducerRemoteTest, ReqCan, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval;
    SurfaceError ret = bp->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_NE(retval.buffer, nullptr);

    ret = bp->CancelBuffer(retval.sequence, bedata);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

HWTEST_F(BufferClientProducerRemoteTest, ReqCanCan, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval;
    SurfaceError ret = bp->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_EQ(retval.buffer, nullptr);

    ret = bp->CancelBuffer(retval.sequence, bedata);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bp->CancelBuffer(retval.sequence, bedata);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}

HWTEST_F(BufferClientProducerRemoteTest, ReqReqReqCanCan, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval1;
    IBufferProducer::RequestBufferReturnValue retval2;
    IBufferProducer::RequestBufferReturnValue retval3;
    SurfaceError ret;

    ret = bp->RequestBuffer(requestConfig, bedata, retval1);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_EQ(retval1.buffer, nullptr);

    ret = bp->RequestBuffer(requestConfig, bedata, retval2);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_NE(retval2.buffer, nullptr);

    ret = bp->RequestBuffer(requestConfig, bedata, retval3);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
    ASSERT_EQ(retval3.buffer, nullptr);

    ret = bp->CancelBuffer(retval1.sequence, bedata);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bp->CancelBuffer(retval2.sequence, bedata);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bp->CancelBuffer(retval3.sequence, bedata);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}

HWTEST_F(BufferClientProducerRemoteTest, SetQueueSizeDeleting, testing::ext::TestSize.Level0)
{
    SurfaceError ret = bp->SetQueueSize(1);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    IBufferProducer::RequestBufferReturnValue retval;
    ret = bp->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_EQ(retval.buffer, nullptr);

    ret = bp->CancelBuffer(retval.sequence, bedata);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bp->SetQueueSize(2);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

HWTEST_F(BufferClientProducerRemoteTest, ReqFlu, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval;
    SurfaceError ret = bp->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bp->FlushBuffer(retval.sequence, bedata, -1, flushConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}

HWTEST_F(BufferClientProducerRemoteTest, ReqFluFlu, testing::ext::TestSize.Level0)
{
    IBufferProducer::RequestBufferReturnValue retval;
    SurfaceError ret = bp->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bp->FlushBuffer(retval.sequence, bedata, -1, flushConfig);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = bp->FlushBuffer(retval.sequence, bedata, -1, flushConfig);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
}
}
} // namespace OHOS
