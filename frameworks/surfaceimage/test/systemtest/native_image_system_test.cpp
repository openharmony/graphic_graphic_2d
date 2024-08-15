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
};

void NativeImageSystemTest::OnFrameAvailable(void *context)
{
    (void) context;
    isOnFrameAvailabled_ = true;
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
}