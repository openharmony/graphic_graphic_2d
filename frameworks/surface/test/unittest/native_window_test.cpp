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
#include <iservice_registry.h>
#include <native_window.h>
#include <display_type.h>
#include <surface_type.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class BufferConsumerListener : public IBufferConsumerListener {
public:
    void OnBufferAvailable() override
    {
    }
};

class NativeWindowTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline BufferRequestConfig requestConfig = {};
    static inline BufferFlushConfig flushConfig = {};
    static inline sptr<OHOS::Surface> cSurface = nullptr;
    static inline sptr<OHOS::IBufferProducer> producer = nullptr;
    static inline sptr<OHOS::Surface> pSurface = nullptr;
    static inline sptr<OHOS::SurfaceBuffer> sBuffer = nullptr;
    static inline NativeWindow* nativeWindow = nullptr;
    static inline NativeWindowBuffer* nativeWindowBuffer = nullptr;
};

void NativeWindowTest::SetUpTestCase()
{
    requestConfig = {
        .width = 0x100,  // small
        .height = 0x100, // small
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
        .timeout = 0,
    };

    cSurface = Surface::CreateSurfaceAsConsumer();
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    cSurface->RegisterConsumerListener(listener);
    producer = cSurface->GetProducer();
    pSurface = Surface::CreateSurfaceAsProducer(producer);
    int32_t fence;
    pSurface->RequestBuffer(sBuffer, fence, requestConfig);
}

void NativeWindowTest::TearDownTestCase()
{
    flushConfig = { .damage = {
        .w = 0x100,
        .h = 0x100,
    } };
    pSurface->FlushBuffer(sBuffer, -1, flushConfig);
    sBuffer = nullptr;
    cSurface = nullptr;
    producer = nullptr;
    pSurface = nullptr;
    nativeWindow = nullptr;
    nativeWindowBuffer = nullptr;
}

/*
* Function: CreateNativeWindowFromSurface
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call CreateNativeWindowFromSurface by abnormal input
*                  2. check ret
 */
HWTEST_F(NativeWindowTest, CreateNativeWindow001, Function | MediumTest | Level2)
{
    ASSERT_EQ(CreateNativeWindowFromSurface(nullptr), nullptr);
}

/*
* Function: CreateNativeWindowFromSurface
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call CreateNativeWindowFromSurface
*                  2. check ret
 */
HWTEST_F(NativeWindowTest, CreateNativeWindow002, Function | MediumTest | Level2)
{
    nativeWindow = CreateNativeWindowFromSurface(&pSurface);
    ASSERT_NE(nativeWindow, nullptr);
}

/*
* Function: NativeWindowHandleOpt
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call NativeWindowHandleOpt by abnormal input
*                  2. check ret
 */
HWTEST_F(NativeWindowTest, HandleOpt001, Function | MediumTest | Level2)
{
    int code = SET_USAGE;
    int32_t usage = HBM_USE_CPU_READ;
    ASSERT_EQ(NativeWindowHandleOpt(nullptr, code, usage), OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: NativeWindowHandleOpt
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call NativeWindowHandleOpt by different param
*                  2. check ret
 */
HWTEST_F(NativeWindowTest, HandleOpt002, Function | MediumTest | Level2)
{
    int code = SET_USAGE;
    int32_t usage = HBM_USE_CPU_READ;
    ASSERT_EQ(NativeWindowHandleOpt(nativeWindow, code, usage), OHOS::GSERROR_OK);

    code = GET_USAGE;
    ASSERT_EQ(NativeWindowHandleOpt(nativeWindow, code, &usage), OHOS::GSERROR_OK);
}

/*
* Function: NativeWindowHandleOpt
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call NativeWindowHandleOpt by different param
*                  2. check ret
 */
HWTEST_F(NativeWindowTest, HandleOpt003, Function | MediumTest | Level2)
{
    int code = SET_BUFFER_GEOMETRY;
    int32_t height = 0x100;
    int32_t width = 0x100;
    ASSERT_EQ(NativeWindowHandleOpt(nativeWindow, code, height, width), OHOS::GSERROR_OK);

    code = GET_BUFFER_GEOMETRY;
    ASSERT_EQ(NativeWindowHandleOpt(nativeWindow, code, &height, &width), OHOS::GSERROR_OK);
}

/*
* Function: NativeWindowHandleOpt
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call NativeWindowHandleOpt by different param
*                  2. check ret
 */
HWTEST_F(NativeWindowTest, HandleOpt004, Function | MediumTest | Level2)
{
    int code = SET_FORMAT;
    int32_t format = PIXEL_FMT_RGBA_8888;
    ASSERT_EQ(NativeWindowHandleOpt(nativeWindow, code, format), OHOS::GSERROR_OK);

    code = GET_FORMAT;
    ASSERT_EQ(NativeWindowHandleOpt(nativeWindow, code, &format), OHOS::GSERROR_OK);
}

/*
* Function: NativeWindowHandleOpt
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call NativeWindowHandleOpt by different param
*                  2. check ret
 */
HWTEST_F(NativeWindowTest, HandleOpt005, Function | MediumTest | Level2)
{
    int code = SET_STRIDE;
    int32_t stride = 0x8;
    ASSERT_EQ(NativeWindowHandleOpt(nativeWindow, code, stride), OHOS::GSERROR_OK);

    code = GET_STRIDE;
    ASSERT_EQ(NativeWindowHandleOpt(nativeWindow, code, &stride), OHOS::GSERROR_OK);
}

/*
* Function: CreateNativeWindowBufferFromSurfaceBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call CreateNativeWindowBufferFromSurfaceBuffer by abnormal input
*                  2. check ret
 */
HWTEST_F(NativeWindowTest, CreateNativeWindowBuffer001, Function | MediumTest | Level2)
{
    ASSERT_EQ(CreateNativeWindowBufferFromSurfaceBuffer(nullptr), nullptr);
}

/*
* Function: CreateNativeWindowBufferFromSurfaceBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call CreateNativeWindowBufferFromSurfaceBuffer
*                  2. check ret
 */
HWTEST_F(NativeWindowTest, CreateNativeWindowBuffer002, Function | MediumTest | Level2)
{
    nativeWindowBuffer = CreateNativeWindowBufferFromSurfaceBuffer(&sBuffer);
    ASSERT_NE(nativeWindowBuffer, nullptr);
}

/*
* Function: NativeWindowRequestBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call NativeWindowRequestBuffer by abnormal input
*                  2. check ret
 */
HWTEST_F(NativeWindowTest, RequestBuffer001, Function | MediumTest | Level2)
{
    ASSERT_EQ(NativeWindowRequestBuffer(nullptr, &nativeWindowBuffer, nullptr), OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: NativeWindowRequestBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call NativeWindowRequestBuffer by abnormal input
*                  2. check ret
 */
HWTEST_F(NativeWindowTest, RequestBuffer002, Function | MediumTest | Level2)
{
    ASSERT_EQ(NativeWindowRequestBuffer(nativeWindow, nullptr, nullptr), OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: NativeWindowRequestBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call NativeWindowRequestBuffer
*                  2. check ret
 */
HWTEST_F(NativeWindowTest, RequestBuffer003, Function | MediumTest | Level2)
{
    int fenceFd = -1;
    ASSERT_EQ(NativeWindowRequestBuffer(nativeWindow, &nativeWindowBuffer, &fenceFd), OHOS::GSERROR_OK);
}

/*
* Function: GetBufferHandleFromNative
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetBufferHandleFromNative by abnormal input
*                  2. check ret
 */
HWTEST_F(NativeWindowTest, GetBufferHandle001, Function | MediumTest | Level2)
{
    ASSERT_EQ(GetBufferHandleFromNative(nullptr), nullptr);
}

/*
* Function: GetBufferHandleFromNative
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetBufferHandleFromNative
*                  2. check ret
 */
HWTEST_F(NativeWindowTest, GetBufferHandle002, Function | MediumTest | Level2)
{
    struct NativeWindowBuffer *buffer = new NativeWindowBuffer();
    buffer->sfbuffer = sBuffer;
    ASSERT_NE(GetBufferHandleFromNative(nativeWindowBuffer), nullptr);
    delete buffer;
}

/*
* Function: NativeWindowFlushBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call NativeWindowFlushBuffer by abnormal input
*                  2. check ret
 */
HWTEST_F(NativeWindowTest, FlushBuffer001, Function | MediumTest | Level2)
{
    int fenceFd = -1;
    struct Region *region = new Region();
    struct Region::Rect * rect = new Region::Rect();
    rect->x = 0x100;
    rect->y = 0x100;
    rect->w = 0x100;
    rect->h = 0x100;
    region->rects = rect;

    ASSERT_EQ(NativeWindowFlushBuffer(nullptr, nullptr, fenceFd, *region), OHOS::GSERROR_INVALID_ARGUMENTS);
    delete region;
}

/*
* Function: NativeWindowFlushBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call NativeWindowFlushBuffer by abnormal input
*                  2. check ret
 */
HWTEST_F(NativeWindowTest, FlushBuffer002, Function | MediumTest | Level2)
{
    int fenceFd = -1;
    struct Region *region = new Region();
    struct Region::Rect * rect = new Region::Rect();
    rect->x = 0x100;
    rect->y = 0x100;
    rect->w = 0x100;
    rect->h = 0x100;
    region->rects = rect;

    ASSERT_EQ(NativeWindowFlushBuffer(nativeWindow, nullptr, fenceFd, *region), OHOS::GSERROR_INVALID_ARGUMENTS);
    delete region;
}

/*
* Function: NativeWindowFlushBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call NativeWindowFlushBuffer
*                  2. check ret
 */
HWTEST_F(NativeWindowTest, FlushBuffer003, Function | MediumTest | Level2)
{
    int fenceFd = -1;
    struct Region *region = new Region();
    struct Region::Rect * rect = new Region::Rect();
    rect->x = 0x100;
    rect->y = 0x100;
    rect->w = 0x100;
    rect->h = 0x100;
    region->rects = rect;

    ASSERT_EQ(NativeWindowFlushBuffer(nativeWindow, nativeWindowBuffer, fenceFd, *region), OHOS::GSERROR_OK);
    delete region;
}

/*
* Function: NativeWindowCancelBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call NativeWindowCancelBuffer by abnormal input
*                  2. check ret
 */
HWTEST_F(NativeWindowTest, CancelBuffer001, Function | MediumTest | Level2)
{
    ASSERT_EQ(NativeWindowCancelBuffer(nullptr, nullptr), OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: NativeWindowCancelBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call NativeWindowCancelBuffer by abnormal input
*                  2. check ret
 */
HWTEST_F(NativeWindowTest, CancelBuffer002, Function | MediumTest | Level2)
{
    ASSERT_EQ(NativeWindowCancelBuffer(nativeWindow, nullptr), OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: NativeWindowCancelBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call NativeWindowCancelBuffer
*                  2. check ret
 */
HWTEST_F(NativeWindowTest, CancelBuffer003, Function | MediumTest | Level2)
{
    ASSERT_EQ(NativeWindowCancelBuffer(nativeWindow, nativeWindowBuffer), OHOS::GSERROR_OK);
}

/*
* Function: NativeObjectReference
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call NativeObjectReference
*                  2. check ret
 */
HWTEST_F(NativeWindowTest, Reference001, Function | MediumTest | Level2)
{
    struct NativeWindowBuffer *buffer = new NativeWindowBuffer();
    buffer->sfbuffer = sBuffer;
    ASSERT_EQ(NativeObjectReference(reinterpret_cast<void *>(buffer)), OHOS::GSERROR_OK);
    delete buffer;
}

/*
* Function: NativeObjectUnreference
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call NativeObjectUnreference
*                  2. check ret
 */
HWTEST_F(NativeWindowTest, Unreference001, Function | MediumTest | Level2)
{
    struct NativeWindowBuffer *buffer = new NativeWindowBuffer();
    buffer->sfbuffer = sBuffer;
    ASSERT_EQ(NativeObjectUnreference(reinterpret_cast<void *>(buffer)), OHOS::GSERROR_OK);
    delete buffer;
}

/*
* Function: DestoryNativeWindow
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call DestoryNativeWindow by abnormal input
*                  2. check ret
 */
HWTEST_F(NativeWindowTest, DestoryNativeWindow001, Function | MediumTest | Level2)
{
    DestoryNativeWindow(nullptr);
}

/*
* Function: DestoryNativeWindow
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call DestoryNativeWindow
*                  2. check ret
 */
HWTEST_F(NativeWindowTest, DestoryNativeWindow002, Function | MediumTest | Level2)
{
    DestoryNativeWindow(nativeWindow);
}

/*
* Function: DestoryNativeWindowBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call DestoryNativeWindowBuffer by abnormal and normal input
*                  2. check ret
 */
HWTEST_F(NativeWindowTest, DestoryNativeWindowBuffer001, Function | MediumTest | Level2)
{
    DestoryNativeWindowBuffer(nullptr);
    DestoryNativeWindowBuffer(nativeWindowBuffer);
}

/*
* Function: DestoryNativeWindowBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call DestoryNativeWindowBuffer again
*                  2. check ret
 */
HWTEST_F(NativeWindowTest, DestoryNativeWindowBuffer002, Function | MediumTest | Level2)
{
    DestoryNativeWindowBuffer(nativeWindowBuffer);
}
}
