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
#include <chrono>
#include <thread>
#include <unistd.h>
#include <gtest/gtest.h>
#include <iservice_registry.h>
#include <display_type.h>
#include <native_window.h>

using namespace testing;
using namespace testing::ext;
using namespace std::chrono_literals;

namespace OHOS::Rosen {
class SurfaceRevertIPCTest : public testing::Test,  public IBufferConsumerListenerClazz {
public:
    static void SetUpTestCase();
    virtual void OnBufferAvailable() override;

    pid_t ChildProcessMain();

    static inline int32_t pipeFd[2] = {};
    static inline int32_t ipcSystemAbilityID = 34156;
    static inline BufferRequestConfig requestConfig = {};
    static inline BufferFlushConfig flushConfig = {};
};

void SurfaceRevertIPCTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << getpid();
    requestConfig = {
        .width = 0x100,  // small
        .height = 0x100, // small
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
        .timeout = 0,
    };
    flushConfig = { .damage = {
        .w = 0x100,
        .h = 0x100,
    } };
}

void SurfaceRevertIPCTest::OnBufferAvailable()
{
}

pid_t SurfaceRevertIPCTest::ChildProcessMain()
{
    pipe(pipeFd);
    auto pid = fork();
    if (pid != 0) {
        return pid;
    }

    GTEST_LOG_(INFO) << getpid();
    auto cSurface = Surface::CreateSurfaceAsConsumer("test");
    cSurface->RegisterConsumerListener(this);
    auto producer = cSurface->GetProducer();
    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sam->AddSystemAbility(ipcSystemAbilityID, producer->AsObject());
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);

    int64_t data;
    sptr<SurfaceBuffer> buffer = nullptr;
    int releaseFence = -1;
    auto sret = pSurface->RequestBuffer(buffer, releaseFence, requestConfig);
    if (sret != OHOS::GSERROR_OK) {
        data = sret;
        write(pipeFd[1], &data, sizeof(data));
        exit(0);
    }

    buffer->GetExtraData()->ExtraSet("123", 0x123);
    buffer->GetExtraData()->ExtraSet("345", (int64_t)0x345);
    buffer->GetExtraData()->ExtraSet("567", "567");

    sret = pSurface->FlushBuffer(buffer, -1, flushConfig);
    if (sret != OHOS::GSERROR_OK) {
        data = sret;
        write(pipeFd[1], &data, sizeof(data));
        exit(0);
    }

    Rect damage;
    int32_t fence;
    sret = cSurface->AcquireBuffer(buffer, fence, data, damage);
    if (sret != OHOS::GSERROR_OK) {
        data = sret;
        write(pipeFd[1], &data, sizeof(data));
        exit(0);
    }

    sret = cSurface->ReleaseBuffer(buffer, -1);
    data = sret;
    write(pipeFd[1], &data, sizeof(data));
    sleep(0);
    read(pipeFd[0], &data, sizeof(data));
    sam->RemoveSystemAbility(ipcSystemAbilityID);
    exit(0);
    return pid;
}

/*
* Function: RequestBuffer and flush buffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. fork thread for produce surface, fill buffer data
*                  2. produce surface, fill buffer data in other thread, check sret and buffer
 */
HWTEST_F(SurfaceRevertIPCTest, Fork001, Function | MediumTest | Level2)
{
    auto pid = ChildProcessMain();
    ASSERT_GE(pid, 0);
    int64_t data;
    read(pipeFd[0], &data, sizeof(data));
    GTEST_LOG_(INFO) << getpid();
    ASSERT_EQ(data, OHOS::GSERROR_OK);

    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto robj = sam->GetSystemAbility(ipcSystemAbilityID);
    auto producer = iface_cast<IBufferProducer>(robj);
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);

    sptr<SurfaceBuffer> buffer = nullptr;
    int releaseFence = -1;
    auto sRet = pSurface->RequestBuffer(buffer, releaseFence, requestConfig);
    EXPECT_EQ(sRet, OHOS::GSERROR_OK);
    EXPECT_NE(buffer, nullptr);
    if (buffer != nullptr) {
        int32_t int32;
        int64_t int64;
        std::string str;
        buffer->GetExtraData()->ExtraGet("123", int32);
        buffer->GetExtraData()->ExtraGet("345", int64);
        buffer->GetExtraData()->ExtraGet("567", str);

        EXPECT_EQ(int32, 0x123);
        EXPECT_EQ(int64, 0x345);
        EXPECT_EQ(str, "567");
    }
    write(pipeFd[1], &data, sizeof(data));
    int32_t ret = 0;
    do {
        waitpid(pid, nullptr, 0);
    } while (ret == -1 && errno == EINTR);
}
}
