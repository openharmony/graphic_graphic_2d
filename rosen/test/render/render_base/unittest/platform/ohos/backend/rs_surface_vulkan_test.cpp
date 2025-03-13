/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "iconsumer_surface.h"

#include "platform/common/rs_system_properties.h"
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#include "render_context/render_context.h"
#include "platform/ohos/backend/rs_surface_ohos_gl.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class BufferConsumerListener : public IBufferConsumerListener {
public:
    void OnBufferAvailable() override
    {
    }
};

class RSSurfaceOhosVulkanTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSurfaceOhosVulkanTest::SetUpTestCase() {}
void RSSurfaceOhosVulkanTest::TearDownTestCase() {}
void RSSurfaceOhosVulkanTest::SetUp() {}
void RSSurfaceOhosVulkanTest::TearDown() {}

/**
 * @tc.name: ClearBufferTest002
 * @tc.desc: test results of ClearBuffer
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceOhosVulkanTest, ClearBufferTest002, TestSize.Level1)
{
    sptr<IConsumerSurface> cSurface2 = IConsumerSurface::Create("DisplayNode");
    ASSERT_TRUE(cSurface2 != nullptr);
    sptr<IBufferProducer> bp = cSurface2->GetProducer();
    sptr<Surface> pSurface2 = Surface::CreateSurfaceAsProducer(bp);
    RSSurfaceOhosVulkan rsSurface(pSurface2);
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::IsUseVulkan()) {
        int32_t height = 1;
        int32_t width = 1;
        uint64_t uiTimestamp = 1;
        rsSurface.RequestFrame(width, height, uiTimestamp);
    }
#endif
    rsSurface.ClearBuffer();
}

/**
 * @tc.name: ResetBufferAgeTest002
 * @tc.desc: test results of ResetBufferAge
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceOhosVulkanTest, ResetBufferAgeTest002, TestSize.Level1)
{
    sptr<IConsumerSurface> cSurface2 = IConsumerSurface::Create("DisplayNode");
    ASSERT_TRUE(cSurface2 != nullptr);
    sptr<IBufferProducer> bp = cSurface2->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    RSSurfaceOhosVulkan rsSurface(pSurface);
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::IsUseVulkan()) {
        std::unique_ptr<RenderContext> renderContext = std::make_unique<RenderContext>();
        rsSurface.SetRenderContext(renderContext.get());
        int32_t width2 = 1;
        int32_t height2 = 1;
        uint64_t uiTimestamp2 = 1;
        rsSurface.RequestFrame(width2, height2, uiTimestamp2);
    }
#endif
    rsSurface.ResetBufferAge();
}

/**
 * @tc.name: RequestNativeWindowBufferTest001
 * @tc.desc: test results of RequestNativeWindowBuffer
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceOhosVulkanTest, RequestNativeWindowBufferTest001, TestSize.Level1)
{
    sptr<Surface> producer1 = nullptr;
    RSSurfaceOhosVulkan rsSurface(producer1);

    int32_t width1 = 1;
    int32_t height1 = 5;
    int fenceFd = -1;
    bool useAFBC = true;
    NativeWindowBuffer* nativeWindowBuffer1 = nullptr;
    auto res = rsSurface.RequestNativeWindowBuffer(&nativeWindowBuffer1, width1, height1, fenceFd, useAFBC);
    EXPECT_TRUE(res != GSERROR_OK);
}

/**
 * @tc.name: SetNativeWindowInfoTest001
 * @tc.desc: test results of SetNativeWindowInfo
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceOhosVulkanTest, SetNativeWindowInfoTest001, TestSize.Level1)
{
    sptr<Surface> producer = nullptr;
    RSSurfaceOhosVulkan rsSurface(producer);
    int32_t height1 = -1;
    int32_t width1 = 1;
    rsSurface.SetNativeWindowInfo(width1, height1, false, false); // true

    width1 = -1;
    height1 = 1;
    rsSurface.SetNativeWindowInfo(width1, height1, true, true); // true

    width1 = 1;
    height1 = 1;
    rsSurface.SetNativeWindowInfo(width1, height1, true, false); // true

    width1 = -1;
    height1 = -1;
    rsSurface.SetNativeWindowInfo(width1, height1, true, true); // false

    EXPECT_TRUE(rsSurface.mSurfaceMap.empty());
}

/**
 * @tc.name: GetCurrentBufferTest001
 * @tc.desc: test results of GetCurrentBuffer
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceOhosVulkanTest, GetCurrentBufferTest001, TestSize.Level1)
{
    RSSurfaceOhosVulkan rsSurface1(IConsumerSurface::Create());
    sptr<SurfaceBuffer> ret = rsSurface1.GetCurrentBuffer();
    EXPECT_TRUE(ret == nullptr);

    sptr<OHOS::IConsumerSurface> cSurface = IConsumerSurface::Create();
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    cSurface->RegisterConsumerListener(listener);
    sptr<OHOS::IBufferProducer> producer1 = cSurface->GetProducer();
    sptr<OHOS::Surface> pSurface = Surface::CreateSurfaceAsProducer(producer1);
    int32_t fence1;
    sptr<OHOS::SurfaceBuffer> sBuffer1 = nullptr;
    BufferRequestConfig requestConfig1 = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    pSurface->RequestBuffer(sBuffer1, fence1, requestConfig1);
    NativeWindowBuffer* nativeWindowBuffer = OH_NativeWindow_CreateNativeWindowBufferFromSurfaceBuffer(&sBuffer1);
    ASSERT_NE(nativeWindowBuffer, nullptr);
    rsSurface1.mSurfaceList.emplace_back(nativeWindowBuffer);
    ret = rsSurface1.GetCurrentBuffer();
    EXPECT_TRUE(ret != nullptr);
}

/**
 * @tc.name: RequestFrameTest001
 * @tc.desc: test results of RequestFrame
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceOhosVulkanTest, RequestFrameTest001, TestSize.Level1)
{
    sptr<IConsumerSurface> cSurface1 = IConsumerSurface::Create("DisplayNode");
    ASSERT_TRUE(cSurface1 != nullptr);
    sptr<IBufferProducer> bp = cSurface1->GetProducer();
    sptr<Surface> pSurface1 = Surface::CreateSurfaceAsProducer(bp);
    RSSurfaceOhosVulkan rsSurface(pSurface1);
    int32_t width1 = 1;
    int32_t height1 = 1;
    uint64_t uiTimestamp1 = 1;
    std::unique_ptr<RSSurfaceFrame> ret = rsSurface.RequestFrame(width1, height1, uiTimestamp1, true, true);
    EXPECT_TRUE(ret == nullptr);
}

/**
 * @tc.name: SetSurfacePixelFormatTest001
 * @tc.desc: test results of SetSurfacePixelFormat
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceOhosVulkanTest, SetSurfacePixelFormatTest001, TestSize.Level1)
{
    RSSurfaceOhosVulkan rsSurface1(IConsumerSurface::Create());
    rsSurface1.SetSurfacePixelFormat(11);
    rsSurface1.SetSurfacePixelFormat(11);
    ASSERT_EQ(rsSurface1.pixelFormat_, GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBX_8888);
}

/**
 * @tc.name: SetColorSpaceTest001
 * @tc.desc: test results of SetColorSpace
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceOhosVulkanTest, SetColorSpaceTest001, TestSize.Level1)
{
    RSSurfaceOhosVulkan rsSurface1(IConsumerSurface::Create());
    rsSurface1.SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_ADOBE_RGB);
    ASSERT_EQ(rsSurface1.colorSpace_, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_ADOBE_RGB);
    rsSurface1.SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_ADOBE_RGB);
    ASSERT_EQ(rsSurface1.colorSpace_, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_ADOBE_RGB);
}

/**
 * @tc.name: FlushFrameTest001
 * @tc.desc: test results of FlushFrame
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceOhosVulkanTest, FlushFrameTest001, TestSize.Level1)
{
    sptr<IConsumerSurface> cSurface1 = IConsumerSurface::Create("DisplayNode");
    ASSERT_TRUE(cSurface1 != nullptr);
    sptr<IBufferProducer> bp1 = cSurface1->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp1);
    RSSurfaceOhosVulkan rsSurface(pSurface);
    uint64_t uiTimestamp1 = 1;
    std::unique_ptr<RSSurfaceFrame> frame = nullptr;
    rsSurface.SetUiTimeStamp(frame, uiTimestamp1);

    int32_t width1 = 1;
    int32_t height1 = 1;
    std::unique_ptr<RSSurfaceFrame> ret = rsSurface.RequestFrame(width1, height1, uiTimestamp1, true, true);
    EXPECT_FALSE(rsSurface.FlushFrame(frame, uiTimestamp1));
    {
        sptr<Surface> producer = nullptr;
        RSSurfaceOhosGl rsSurface(producer);
        RenderContext renderContext;
        rsSurface.SetRenderContext(&renderContext);
        ASSERT_TRUE(rsSurface.FlushFrame(frame, uiTimestamp1));
    }
}
} // namespace Rosen
} // namespace OHOS