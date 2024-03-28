/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "iconsumer_surface.h"
#include <iservice_registry.h>
#include "native_buffer.h"
#include "native_buffer_inner.h"
#include "native_window.h"
#include "surface_type.h"
#include "graphic_common_c.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class BufferConsumerListener : public IBufferConsumerListener {
public:
    void OnBufferAvailable() override
    {
    }
};

class NativeBufferTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline OH_NativeBuffer_Config config = {
        .width = 0x100,
        .height = 0x100,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
    };
    static inline OH_NativeBuffer_Config checkConfig = {};
    static inline OH_NativeBuffer* buffer = nullptr;
};

void NativeBufferTest::SetUpTestCase()
{
    buffer = nullptr;
}

void NativeBufferTest::TearDownTestCase()
{
    buffer = nullptr;
}

/*
* Function: OH_NativeBuffer_Alloc
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeBuffer_Alloc by abnormal input
*                  2. check ret
*/
HWTEST_F(NativeBufferTest, OHNativeBufferAlloc001, Function | MediumTest | Level2)
{
    buffer = OH_NativeBuffer_Alloc(nullptr);
    ASSERT_EQ(buffer, nullptr);
}

/*
* Function: OH_NativeBuffer_Alloc
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeBuffer_Alloc
*                  2. check ret
*/
HWTEST_F(NativeBufferTest, OHNativeBufferAlloc002, Function | MediumTest | Level2)
{
    buffer = OH_NativeBuffer_Alloc(&config);
    ASSERT_NE(buffer, nullptr);
}

/*
* Function: OH_NativeBuffer_GetSeqNum
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeBuffer_GetSeqNum by abnormal input
*                  2. check ret
*/
HWTEST_F(NativeBufferTest, OHNativeBufferGetSeqNum001, Function | MediumTest | Level2)
{
    uint32_t id = OH_NativeBuffer_GetSeqNum(nullptr);
    ASSERT_EQ(id, GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: OH_NativeBuffer_GetSeqNum
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeBuffer_GetSeqNum
*                  2. check ret
*/
HWTEST_F(NativeBufferTest, OHNativeBufferGetSeqNum002, Function | MediumTest | Level2)
{
    uint32_t id = OH_NativeBuffer_GetSeqNum(buffer);
    ASSERT_NE(id, GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: OH_NativeBuffer_GetConfig
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeBuffer_GetConfig
*                  2. check ret
 */
HWTEST_F(NativeBufferTest, OHNativeBufferGetConfig001, Function | MediumTest | Level2)
{
    OH_NativeBuffer_GetConfig(buffer, &checkConfig);
    ASSERT_NE(&checkConfig, nullptr);
}

/*
* Function: OH_NativeBuffer_GetConfig
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeBuffer_GetConfig by abnormal input
*                  2. check ret
*/
HWTEST_F(NativeBufferTest, OHNativeBufferGetConfig002, Function | MediumTest | Level2)
{
    checkConfig.width = 0x0;
    checkConfig.height = 0x0;
    checkConfig.format = 0x0;
    checkConfig.usage = 0x0;
    OH_NativeBuffer_GetConfig(nullptr, &checkConfig);
    ASSERT_EQ(checkConfig.width, 0x0);
    ASSERT_EQ(checkConfig.height, 0x0);
    ASSERT_EQ(checkConfig.format, 0x0);
    ASSERT_EQ(checkConfig.usage, 0x0);
}

/*
* Function: OH_NativeBuffer_Reference
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeBuffer_Reference by abnormal input
*                  2. check ret
*/
HWTEST_F(NativeBufferTest, OHNativeBufferReference001, Function | MediumTest | Level2)
{
    int32_t ret = OH_NativeBuffer_Reference(nullptr);
    ASSERT_NE(ret, GSERROR_OK);
}

/*
* Function: OH_NativeBuffer_Reference
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeBuffer_Reference
*                  2. check ret
*/
HWTEST_F(NativeBufferTest, OHNativeBufferReference002, Function | MediumTest | Level2)
{
    int32_t ret = OH_NativeBuffer_Reference(buffer);
    ASSERT_EQ(ret, GSERROR_OK);
}

/*
* Function: OH_NativeBuffer_Unreference
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeBuffer_Unreference by abnormal input
*                  2. check ret
*/
HWTEST_F(NativeBufferTest, OHNativeBufferUnreference001, Function | MediumTest | Level2)
{
    int32_t ret = OH_NativeBuffer_Unreference(nullptr);
    ASSERT_NE(ret, GSERROR_OK);
}

/*
* Function: OH_NativeBuffer_Unreference
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeBuffer_Unreference
*                  2. check ret
*/
HWTEST_F(NativeBufferTest, OHNativeBufferUnreference002, Function | MediumTest | Level2)
{
    int32_t ret = OH_NativeBuffer_Unreference(buffer);
    ASSERT_EQ(ret, GSERROR_OK);
}

/*
* Function: OH_NativeBuffer_GetSeqNum and OH_NativeBuffer_Unreference
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeBuffer_GetSeqNum
*                  2. call OH_NativeBuffer_Unreference
*                  3. OH_NativeBuffer_Alloc again
*                  4. check OH_NativeBuffer_GetSeqNum = oldSeq + 1
*/
HWTEST_F(NativeBufferTest, OHNativeBufferGetSeqNum003, Function | MediumTest | Level2)
{
    uint32_t oldSeq = OH_NativeBuffer_GetSeqNum(buffer);
    int32_t ret = OH_NativeBuffer_Unreference(buffer);
    ASSERT_EQ(ret, GSERROR_OK);
    buffer = OH_NativeBuffer_Alloc(&config);
    ASSERT_EQ(oldSeq + 1, OH_NativeBuffer_GetSeqNum(buffer));
}

/*
* Function: OH_NativeBuffer_GetBufferHandle
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeBuffer_GetBufferHandle
*                  2. check result
*/
HWTEST_F(NativeBufferTest, OHNativeBufferGetBufferHandle001, Function | MediumTest | Level2)
{
    const BufferHandle* handle = OH_NativeBuffer_GetBufferHandle(buffer);
    ASSERT_NE(handle, nullptr);
    int32_t ret = FreeBufferHandle(nullptr);
    ASSERT_EQ(ret, 0);
    BufferHandle* cloneHandle = CloneBufferHandle(nullptr);
    ASSERT_EQ(cloneHandle, nullptr);
    cloneHandle = CloneBufferHandle(handle);
    ASSERT_NE(cloneHandle, nullptr);
    ASSERT_NE(handle, nullptr);
    ASSERT_EQ(cloneHandle->width, handle->width);
}

/*
* Function: OH_NativeBuffer_GetNativeBufferConfig
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeBuffer_GetNativeBufferConfig
*                  2. check result
*/
HWTEST_F(NativeBufferTest, OHNativeBufferGetNativeBufferConfig001, Function | MediumTest | Level2)
{
    OH_NativeBuffer_Config testConfig = {};
    OH_NativeBuffer_GetNativeBufferConfig(buffer, &testConfig);
    ASSERT_EQ(testConfig.width, config.width);
    ASSERT_EQ(testConfig.height, config.height);
    ASSERT_EQ(testConfig.format, config.format);
    ASSERT_EQ(testConfig.usage, config.usage);
}

/*
* Function: OH_NativeBuffer_SetColorSpace
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeBuffer_SetColorSpace by abnormal input
*                  2. check ret
*/
HWTEST_F(NativeBufferTest, OHNativeBufferSetColorSpace001, Function | MediumTest | Level2)
{
    int32_t ret = OH_NativeBuffer_SetColorSpace(nullptr, OH_COLORSPACE_DISPLAY_BT2020_PQ);
    ASSERT_NE(ret, GSERROR_OK);
}

/*
* Function: OH_NativeBuffer_SetColorSpace
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeBuffer_SetColorSpace
*                  2. check ret
*/
HWTEST_F(NativeBufferTest, OHNativeBufferSetColorSpace002, Function | MediumTest | Level2)
{
    if (buffer == nullptr) {
        buffer = OH_NativeBuffer_Alloc(&config);
        ASSERT_NE(buffer, nullptr);
    }

    int32_t ret = OH_NativeBuffer_SetColorSpace(buffer, OH_COLORSPACE_BT709_LIMIT);
    if (ret != GSERROR_NOT_SUPPORT) { // some device not support set colorspace
        ASSERT_EQ(ret, GSERROR_OK);
    }
}

/*
* Function: OH_NativeBuffer_Map
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeBuffer_Map by abnormal input
*                  2. check ret
*/
HWTEST_F(NativeBufferTest, OHNativeBufferMap001, Function | MediumTest | Level2)
{
    void *virAddr = nullptr;
    int32_t ret = OH_NativeBuffer_Map(nullptr, &virAddr);
    ASSERT_NE(ret, GSERROR_OK);
}

/*
* Function: OH_NativeBuffer_Map
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeBuffer_Map
*                  2. check ret
*/
HWTEST_F(NativeBufferTest, OHNativeBufferMap002, Function | MediumTest | Level2)
{
    void *virAddr = nullptr;
    int32_t ret = OH_NativeBuffer_Map(buffer, &virAddr);
    ASSERT_EQ(ret, GSERROR_OK);
    ASSERT_NE(virAddr, nullptr);
}

/*
* Function: OH_NativeBuffer_Unmap
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeBuffer_Unmap by abnormal input
*                  2. check ret
*/
HWTEST_F(NativeBufferTest, OHNativeBufferUnmap001, Function | MediumTest | Level2)
{
    int32_t ret = OH_NativeBuffer_Unmap(nullptr);
    ASSERT_NE(ret, GSERROR_OK);
}

/*
* Function: OH_NativeBuffer_Unmap and OH_NativeBuffer_Unreference
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeBuffer_Unmap
*                  2. check ret
*                  3. call OH_NativeBuffer_Unreference
*/
HWTEST_F(NativeBufferTest, OHNativeBufferUnmap002, Function | MediumTest | Level2)
{
    int32_t ret = OH_NativeBuffer_Unmap(buffer);
    ASSERT_EQ(ret, GSERROR_OK);
    ret = OH_NativeBuffer_Unreference(buffer);
    ASSERT_EQ(ret, GSERROR_OK);
}

/*
* Function: OH_NativeBufferFromNativeWindowBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeBufferFromNativeWindowBuffer by abnormal input
*                  2. check ret
*/
HWTEST_F(NativeBufferTest, NativeBufferFromNativeWindowBuffer001, Function | MediumTest | Level2)
{
    OH_NativeBuffer* nativeBuffer = OH_NativeBufferFromNativeWindowBuffer(nullptr);
    ASSERT_EQ(nativeBuffer, nullptr);
}

/*
* Function: OH_NativeBufferFromNativeWindowBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeBufferFromNativeWindowBuffer
*                  2. check ret
*/
HWTEST_F(NativeBufferTest, NativeBufferFromNativeWindowBuffer002, Function | MediumTest | Level2)
{
    sptr<OHOS::IConsumerSurface> cSurface = IConsumerSurface::Create();
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    cSurface->RegisterConsumerListener(listener);
    sptr<OHOS::IBufferProducer> producer = cSurface->GetProducer();
    sptr<OHOS::Surface> pSurface = Surface::CreateSurfaceAsProducer(producer);
    int32_t fence;
    sptr<OHOS::SurfaceBuffer> sBuffer = nullptr;
    BufferRequestConfig requestConfig = {
        .width = 0x100,  // small
        .height = 0x100, // small
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    pSurface->RequestBuffer(sBuffer, fence, requestConfig);
    NativeWindow* nativeWindow = OH_NativeWindow_CreateNativeWindow(&pSurface);
    ASSERT_NE(nativeWindow, nullptr);
    NativeWindowBuffer* nativeWindowBuffer = OH_NativeWindow_CreateNativeWindowBufferFromSurfaceBuffer(&sBuffer);
    ASSERT_NE(nativeWindowBuffer, nullptr);
    OH_NativeBuffer* nativeBuffer = OH_NativeBufferFromNativeWindowBuffer(nativeWindowBuffer);
    ASSERT_NE(nativeBuffer, nullptr);

    int32_t ret = OH_NativeBuffer_FromNativeWindowBuffer(nativeWindowBuffer, nullptr);
    ASSERT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);
    OH_NativeBuffer* nativeBufferTmp = nullptr;
    ret = OH_NativeBuffer_FromNativeWindowBuffer(nativeWindowBuffer, &nativeBufferTmp);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_EQ(nativeBuffer, nativeBufferTmp);

    void *virAddr = nullptr;
    OH_NativeBuffer_Planes outPlanes;
    ret = OH_NativeBuffer_MapPlanes(nativeBuffer, &virAddr, &outPlanes);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    sBuffer = nullptr;
    cSurface = nullptr;
    producer = nullptr;
    pSurface = nullptr;
    nativeWindow = nullptr;
    nativeWindowBuffer = nullptr;
}

/*
* Function: OH_NativeBuffer_FromNativeWindowBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeBuffer_FromNativeWindowBuffer by abnormal input
*                  2. check ret
*/
HWTEST_F(NativeBufferTest, NativeBufferFromNativeWindowBuffer003, Function | MediumTest | Level2)
{
    int32_t ret = OH_NativeBuffer_FromNativeWindowBuffer(nullptr, nullptr);
    ASSERT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);

    NativeWindowBuffer nativeWindowBuffer;
    ret = OH_NativeBuffer_FromNativeWindowBuffer(&nativeWindowBuffer, nullptr);
    ASSERT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: OH_NativeBuffer_MapPlanes
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeBuffer_MapPlanes by abnormal input
*                  2. check ret
*/
HWTEST_F(NativeBufferTest, OHNativeBufferMapPlanes001, Function | MediumTest | Level2)
{
    int32_t ret = OH_NativeBuffer_MapPlanes(nullptr, nullptr, nullptr);
    ASSERT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);

    OH_NativeBuffer *buffer = (OH_NativeBuffer *)0xFFFFFFFF;
    ret = OH_NativeBuffer_MapPlanes(buffer, nullptr, nullptr);
    ASSERT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);

    void *virAddr = nullptr;
    ret = OH_NativeBuffer_MapPlanes(buffer, &virAddr, nullptr);
    ASSERT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: OH_NativeBuffer_MapPlanes
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeBuffer_MapPlanes
*                  2. check ret
*/
HWTEST_F(NativeBufferTest, OHNativeBufferMapPlanes002, Function | MediumTest | Level2)
{
    sptr<OHOS::IConsumerSurface> cSurface = IConsumerSurface::Create();
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    cSurface->RegisterConsumerListener(listener);
    sptr<OHOS::IBufferProducer> producer = cSurface->GetProducer();
    sptr<OHOS::Surface> pSurface = Surface::CreateSurfaceAsProducer(producer);
    int32_t fence;
    sptr<OHOS::SurfaceBuffer> sBuffer = nullptr;
    BufferRequestConfig requestConfig = {.width = 0x100, .height = 0x100, .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_YCBCR_420_SP,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, .timeout = 0};
    pSurface->SetQueueSize(4);
    int32_t formatType[] = {GRAPHIC_PIXEL_FMT_YCBCR_420_SP, GRAPHIC_PIXEL_FMT_YCRCB_420_SP,
        GRAPHIC_PIXEL_FMT_YCBCR_420_P, GRAPHIC_PIXEL_FMT_YCRCB_420_P};
    NativeWindow* nativeWindow;
    NativeWindowBuffer* nativeWindowBuffer;
    for (int32_t i = 0; i < sizeof(formatType) / sizeof(int32_t); i++) {
        requestConfig.format = formatType[i];
        pSurface->RequestBuffer(sBuffer, fence, requestConfig);
        nativeWindow = OH_NativeWindow_CreateNativeWindow(&pSurface);
        ASSERT_NE(nativeWindow, nullptr);
        nativeWindowBuffer = OH_NativeWindow_CreateNativeWindowBufferFromSurfaceBuffer(&sBuffer);
        ASSERT_NE(nativeWindowBuffer, nullptr);
        OH_NativeBuffer* nativeBuffer = OH_NativeBufferFromNativeWindowBuffer(nativeWindowBuffer);
        ASSERT_NE(nativeBuffer, nullptr);

        void *virAddr = nullptr;
        OH_NativeBuffer_Planes outPlanes;
        int32_t ret = OH_NativeBuffer_MapPlanes(nativeBuffer, &virAddr, &outPlanes);
        if (ret != 50001999) {
            ASSERT_EQ(ret, OHOS::GSERROR_OK);
            ASSERT_NE(virAddr, nullptr);
            ASSERT_EQ(outPlanes.planeCount, 3);
            ASSERT_EQ(outPlanes.planes[0].offset, 0);
            ASSERT_NE(outPlanes.planes[1].offset, 0);
            ASSERT_NE(outPlanes.planes[2].offset, 0);
            std::cout << "i:" << i << std::endl;
            for (int32_t j = 0; j < 3; j++) {
                std::cout << "offset:" << outPlanes.planes[j].offset <<
                    " rowStride:" << outPlanes.planes[j].rowStride << " columnStride:" <<
                    outPlanes.planes[j].columnStride << std::endl;
            }
        }
    }

    sBuffer = nullptr;
    cSurface = nullptr;
    producer = nullptr;
    pSurface = nullptr;
    nativeWindow = nullptr;
    nativeWindowBuffer = nullptr;
}
}