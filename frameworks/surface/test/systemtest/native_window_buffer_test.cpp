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

namespace OHOS::Rosen {
class NativeWindowBufferTest : public testing::Test,  public IBufferConsumerListenerClazz {
public:
    static void SetUpTestCase();
    virtual void OnBufferAvailable() override;
    pid_t ChildProcessMain();

    static inline sptr<OHOS::Surface> cSurface = nullptr;
    static inline int32_t pipeFd[2] = {};
    static inline int32_t ipcSystemAbilityID = 34156;
};

void NativeWindowBufferTest::SetUpTestCase() {}

void NativeWindowBufferTest::OnBufferAvailable() {}

pid_t NativeWindowBufferTest::ChildProcessMain()
{
    pipe(pipeFd);
    pid_t pid = fork();
    if (pid != 0) {
        return pid;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));  // wait 50ms
    int64_t data;
    read(pipeFd[0], &data, sizeof(data));

    sptr<IRemoteObject> robj = nullptr;
    while (true) {
        auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        robj = sam->GetSystemAbility(ipcSystemAbilityID);
        if (robj != nullptr) {
            break;
        }
        sleep(0);
    }

    auto producer = iface_cast<IBufferProducer>(robj);
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(producer);

    struct NativeWindow *nativeWindow = CreateNativeWindowFromSurface(&pSurface);
    struct NativeWindowBuffer *nativeWindowBuffer = nullptr;

    int code = SET_USAGE;
    int32_t usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA;
    NativeWindowHandleOpt(nativeWindow, code, usage);

    code = SET_BUFFER_GEOMETRY;
    int32_t height = 0x100;
    int32_t width = 0x100;
    NativeWindowHandleOpt(nativeWindow, code, height, width);

    code = SET_FORMAT;
    int32_t format = PIXEL_FMT_RGBA_8888;
    NativeWindowHandleOpt(nativeWindow, code, format);

    code = SET_STRIDE;
    int32_t stride = 0x8;
    NativeWindowHandleOpt(nativeWindow, code, stride);

    int32_t fenceFd;
    auto ret = NativeWindowRequestBuffer(nativeWindow, &nativeWindowBuffer, &fenceFd);
    if (ret != OHOS::GSERROR_OK) {
        data = ret;
        write(pipeFd[1], &data, sizeof(data));
        exit(0);
    }
    nativeWindowBuffer->sfbuffer->ExtraSet("123", 0x123);
    nativeWindowBuffer->sfbuffer->ExtraSet("345", (int64_t)0x345);
    nativeWindowBuffer->sfbuffer->ExtraSet("567", "567");

    struct Region *region = new Region();
    struct Region::Rect *rect = new Region::Rect();
    rect->w = 0x100;
    rect->h = 0x100;
    region->rects = rect;
    ret = NativeWindowFlushBuffer(nativeWindow, nativeWindowBuffer, -1, *region);

    data = ret;
    write(pipeFd[1], &data, sizeof(data));
    sleep(0);
    read(pipeFd[0], &data, sizeof(data));
    close(pipeFd[0]);
    close(pipeFd[1]);
    exit(0);
    return 0;
}

/*
* Function: produce and consumer surface of nativewindow
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. produce surface by nativewindow interface, fill buffer
*                  2. consume surface and check buffer
 */
HWTEST_F(NativeWindowBufferTest, Surface001, Function | MediumTest | Level2)
{
    auto pid = ChildProcessMain();
    ASSERT_GE(pid, 0);

    cSurface = Surface::CreateSurfaceAsConsumer("test");
    cSurface->RegisterConsumerListener(this);
    auto producer = cSurface->GetProducer();
    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sam->AddSystemAbility(ipcSystemAbilityID, producer->AsObject());

    int64_t data = 0;
    write(pipeFd[1], &data, sizeof(data));
    sleep(0);
    read(pipeFd[0], &data, sizeof(data));
    EXPECT_EQ(data, OHOS::GSERROR_OK);

    OHOS::sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence;
    int64_t timestamp;
    Rect damage;
    auto ret = cSurface->AcquireBuffer(buffer, fence, timestamp, damage);
    EXPECT_EQ(ret, OHOS::GSERROR_OK);
    EXPECT_NE(buffer, nullptr);
    if (buffer != nullptr) {
        int32_t int32;
        int64_t int64;
        std::string str;
        buffer->ExtraGet("123", int32);
        buffer->ExtraGet("345", int64);
        buffer->ExtraGet("567", str);

        EXPECT_EQ(int32, 0x123);
        EXPECT_EQ(int64, 0x345);
        EXPECT_EQ(str, "567");
    }

    ret = cSurface->ReleaseBuffer(buffer, -1);
    EXPECT_EQ(ret, OHOS::GSERROR_OK);

    write(pipeFd[1], &data, sizeof(data));
    close(pipeFd[0]);
    close(pipeFd[1]);
    sam->RemoveSystemAbility(ipcSystemAbilityID);
    waitpid(pid, nullptr, NULL);
}
}
