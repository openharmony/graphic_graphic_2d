/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include <iservice_registry.h>
#include <native_image.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <sys/time.h>
#include <securec.h>
#include "graphic_common_c.h"
#include "surface_type.h"
#include "window.h"
#include "GLES/gl.h"
#include "surface.h"
#include "surface_image.h"

using namespace testing;
using namespace testing::ext;
using namespace std;

namespace OHOS::Rosen {
struct TEST_IMAGE {
    int a;
    bool b;
};

typedef struct OH_NativeImage_Tmp {
    OHOS::sptr<OHOS::SurfaceImage> consumer;
    OHOS::sptr<OHOS::IBufferProducer> producer;
    OHOS::sptr<OHOS::Surface> pSurface = nullptr;
    struct NativeWindow* nativeWindow = nullptr;
} OH_NativeImage_Tmp;


class NativeImageSystemTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static void OnFrameAvailable(void *context);
    static inline bool isOnFrameAvailabled_ = false;

    // OH_ConsumerSurface_Create001
    void SetData(NativeWindowBuffer *nativeWindowBuffer, NativeWindow *nativeWindow);
    bool GetData(sptr<SurfaceBuffer> &buffer);
    int32_t CreateNativeWindowAndRequestBuffer001(uint64_t uniqueId, NativeWindow **nativeWindow);
    int32_t ThreadNativeWindowProcess001(int32_t *pipeFd, uint64_t uniqueId);
    static inline int32_t g_onBufferAvailable_ = 0;
};

void NativeImageSystemTest::OnFrameAvailable(void *context)
{
    (void) context;
    isOnFrameAvailabled_ = true;
    g_onBufferAvailable_++;
}

void NativeImageSystemTest::SetUpTestCase()
{
}

void NativeImageSystemTest::TearDownTestCase()
{
}

void ProducerThread(OHNativeWindow* nativeWindow)
{
    if (nativeWindow == nullptr) {
        return;
    }
    int32_t code = SET_BUFFER_GEOMETRY;
    int32_t width = 0x100;
    int32_t height = 0x100;
    auto ret = NativeWindowHandleOpt(nativeWindow, code, width, height);

    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    int fenceFd = -1;
    struct Region *region = new Region();
    struct Region::Rect *rect = new Region::Rect();
    rect->x = 0x100;
    rect->y = 0x100;
    rect->w = 0x100;
    rect->h = 0x100;
    region->rects = rect;
    ret = OH_NativeWindow_NativeWindowRequestBuffer(nativeWindow, &nativeWindowBuffer, &fenceFd);
    if (ret != GSERROR_OK) {
        return;
    }
    ret = OH_NativeWindow_NativeWindowFlushBuffer(nativeWindow, nativeWindowBuffer, fenceFd, *region);
    delete rect;
    delete region;
    return;
}

/*
* Function: NativeImageSystemTestWithAcquireBuffer
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. run NativeImageSystemTestWithAcquireBuffer
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageSystemTest, NativeImageSystemTestWithAcquireBuffer, Function | MediumTest | Level1)
{
    OH_NativeImage* newImage = OH_NativeImage_Create(0, 0);
    ASSERT_NE(newImage, nullptr);
    OHNativeWindow* nativeWindow = OH_NativeImage_AcquireNativeWindow(newImage);
    std::thread producerThread(ProducerThread, nativeWindow);
    producerThread.join();
    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    int fenceFd = -1;
    nativeWindowBuffer = nullptr;
    auto ret = OH_NativeImage_AcquireNativeWindowBuffer(newImage, &nativeWindowBuffer, &fenceFd);
    ASSERT_EQ(ret, GSERROR_OK);
    ASSERT_NE(nativeWindowBuffer, nullptr);

    ret = OH_NativeImage_ReleaseNativeWindowBuffer(newImage, nativeWindowBuffer, fenceFd);
    ASSERT_EQ(ret, GSERROR_OK);

    OH_NativeImage_Destroy(&newImage);
}

/*
* Function: NativeImageSystemTestWithListener
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. run NativeImageSystemTestWithListener
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageSystemTest, NativeImageSystemTestWithListener, Function | MediumTest | Level1)
{
    OH_NativeImage* newImage = OH_NativeImage_Create(0, 0);
    ASSERT_NE(newImage, nullptr);
    OHNativeWindow* nativeWindow = OH_NativeImage_AcquireNativeWindow(newImage);
    OH_OnFrameAvailableListener listener;
    listener.context = this;
    listener.onFrameAvailable = NativeImageSystemTest::OnFrameAvailable;
    auto ret = OH_NativeImage_SetOnFrameAvailableListener(newImage, listener);
    ASSERT_EQ(ret, GSERROR_OK);


    isOnFrameAvailabled_ = false;
    std::thread producerThread(ProducerThread, nativeWindow);
    producerThread.join();
    EXPECT_TRUE(isOnFrameAvailabled_);
    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    int fenceFd = -1;
    nativeWindowBuffer = nullptr;
    ret = OH_NativeImage_AcquireNativeWindowBuffer(newImage, &nativeWindowBuffer, &fenceFd);
    ASSERT_EQ(ret, GSERROR_OK);
    ASSERT_NE(nativeWindowBuffer, nullptr);

    ret = OH_NativeImage_ReleaseNativeWindowBuffer(newImage, nativeWindowBuffer, fenceFd);
    ASSERT_EQ(ret, GSERROR_OK);

    OH_NativeImage_Destroy(&newImage);
}

/*
* Function: NativeImageSystemTestWithGetTimeStamp
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. run NativeImageSystemTestWithGetTimeStamp
*                  2. check ret
* @tc.require: issueI5KG61
*/
HWTEST_F(NativeImageSystemTest, NativeImageSystemTestWithGetTimeStamp, Function | MediumTest | Level1)
{
    OH_NativeImage* newImage = OH_NativeImage_Create(0, 0);
    ASSERT_NE(newImage, nullptr);

    OHNativeWindow* nativeWindow = OH_NativeImage_AcquireNativeWindow(newImage);
    std::thread producerThread(ProducerThread, nativeWindow);
    producerThread.join();
    int64_t timeStamp = OH_NativeImage_GetTimestamp(newImage);
    ASSERT_NE(timeStamp, SURFACE_ERROR_ERROR);
    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    int fenceFd = -1;
    nativeWindowBuffer = nullptr;
    auto ret = OH_NativeImage_AcquireNativeWindowBuffer(newImage, &nativeWindowBuffer, &fenceFd);
    ASSERT_EQ(ret, GSERROR_OK);
    ASSERT_NE(nativeWindowBuffer, nullptr);

    ret = OH_NativeImage_ReleaseNativeWindowBuffer(newImage, nativeWindowBuffer, fenceFd);
    ASSERT_EQ(ret, GSERROR_OK);

    OH_NativeImage_Destroy(&newImage);
}

void NativeImageSystemTest::SetData(NativeWindowBuffer *nativeWindowBuffer, NativeWindow *nativeWindow)
{
    nativeWindowBuffer->sfbuffer->GetExtraData()->ExtraSet("123", 0x123);
    nativeWindowBuffer->sfbuffer->GetExtraData()->ExtraSet("345", (int64_t)0x345);
    nativeWindowBuffer->sfbuffer->GetExtraData()->ExtraSet("567", "567");
}

bool NativeImageSystemTest::GetData(sptr<SurfaceBuffer> &buffer)
{
    int32_t int32;
    int64_t int64;
    std::string str;
    buffer->GetExtraData()->ExtraGet("123", int32);
    buffer->GetExtraData()->ExtraGet("345", int64);
    buffer->GetExtraData()->ExtraGet("567", str);
    if ((int32 != 0x123) || (int64 != 0x345) || (str != "567")) {
        return false;
    }

    return true;
}

int32_t NativeImageSystemTest::CreateNativeWindowAndRequestBuffer001(uint64_t uniqueId, NativeWindow **nativeWindow)
{
    int32_t ret = OH_NativeWindow_CreateNativeWindowFromSurfaceId(uniqueId, nativeWindow);
    if (ret != GSERROR_OK) {
        return ret;
    }
    struct NativeWindowBuffer *nativeWindowBuffer = nullptr;

    int32_t code = SET_BUFFER_GEOMETRY;
    int32_t height = 0x100;
    int32_t width = 0x100;
    OH_NativeWindow_NativeWindowHandleOpt(*nativeWindow, code, height, width);
    code = SET_FORMAT;
    int32_t format = GRAPHIC_PIXEL_FMT_RGBA_8888;
    OH_NativeWindow_NativeWindowHandleOpt(*nativeWindow, code, format);

    int32_t fenceFd = -1;
    struct Region *region = new Region();
    struct Region::Rect *rect = new Region::Rect();
    rect->w = 0x100;
    rect->h = 0x100;
    region->rects = rect;
    region->rectNumber = 1;
    for (int32_t i = 0; i < 3; i++) {  // 3 : queue size
        ret = OH_NativeWindow_NativeWindowRequestBuffer(*nativeWindow, &nativeWindowBuffer, &fenceFd);
        if (ret != GSERROR_OK) {
            delete rect;
            delete region;
            return ret;
        }
        SetData(nativeWindowBuffer, *nativeWindow);

        ret = OH_NativeWindow_NativeWindowFlushBuffer(*nativeWindow, nativeWindowBuffer, -1, *region);
        if (ret != GSERROR_OK) {
            delete rect;
            delete region;
            return ret;
        }
    }
    delete rect;
    delete region;
    return GSERROR_OK;
}

int32_t NativeImageSystemTest::ThreadNativeWindowProcess001(int32_t *pipeFd, uint64_t uniqueId)
{
    int64_t data;
    NativeWindow *nativeWindow = nullptr;
    int32_t ret = CreateNativeWindowAndRequestBuffer001(uniqueId, &nativeWindow);
    if (ret != GSERROR_OK) {
        data = ret;
        write(pipeFd[1], &data, sizeof(data));
        return -1;
    }

    data = ret;
    write(pipeFd[1], &data, sizeof(data));
    usleep(1000); // sleep 1000 microseconds (equals 1 milliseconds)
    read(pipeFd[0], &data, sizeof(data));
    OH_NativeWindow_DestroyNativeWindow(nativeWindow);
    return 0;
}

HWTEST_F(NativeImageSystemTest, OH_ConsumerSurface_Create001, Function | MediumTest | Level2)
{
    int32_t pipeFd[2] = {};
    pipe(pipeFd);

    g_onBufferAvailable_ = 0;
    OH_NativeImage* consumerSurface = OH_ConsumerSurface_Create();
    ASSERT_NE(consumerSurface, nullptr);
    OHNativeWindow* newNativeWindow = OH_NativeImage_AcquireNativeWindow(consumerSurface);
    ASSERT_NE(newNativeWindow, nullptr);
    uint64_t uniqueId;
    int32_t ret = OH_NativeImage_GetSurfaceId(consumerSurface, &uniqueId);
    EXPECT_EQ(ret, GSERROR_OK);

    OH_OnFrameAvailableListener listener;
    listener.context = this;
    listener.onFrameAvailable = NativeImageSystemTest::OnFrameAvailable;
    ret = OH_NativeImage_SetOnFrameAvailableListener(consumerSurface, listener);
    ASSERT_EQ(ret, GSERROR_OK);

    std::thread thread([this, pipeFd, uniqueId]() {
        int32_t ret = this->ThreadNativeWindowProcess001((int32_t*)(pipeFd), uniqueId);
        EXPECT_EQ(ret, GSERROR_OK);
    });

    int64_t data = 0;
    read(pipeFd[0], &data, sizeof(data));
    EXPECT_EQ(data, GSERROR_OK);

    OHNativeWindowBuffer *nativeWindowBuffer = nullptr;
    int32_t fence = -1;
    EXPECT_EQ(g_onBufferAvailable_, 3);  // 3 : queue size
    while (g_onBufferAvailable_ > 0) {
        ret = OH_NativeImage_AcquireNativeWindowBuffer(consumerSurface, &nativeWindowBuffer, &fence);
        EXPECT_EQ(ret, GSERROR_OK);
        EXPECT_NE(nativeWindowBuffer, nullptr);
        EXPECT_EQ(GetData(nativeWindowBuffer->sfbuffer), true);

        ret = OH_NativeImage_ReleaseNativeWindowBuffer(consumerSurface, nativeWindowBuffer, -1);
        EXPECT_EQ(ret, GSERROR_OK);
        g_onBufferAvailable_--;
    }

    g_onBufferAvailable_ = 0;
    write(pipeFd[1], &data, sizeof(data));
    close(pipeFd[0]);
    close(pipeFd[1]);
    if (thread.joinable()) {
        thread.join();
    }
    OH_NativeImage_Destroy(&consumerSurface);
}
}