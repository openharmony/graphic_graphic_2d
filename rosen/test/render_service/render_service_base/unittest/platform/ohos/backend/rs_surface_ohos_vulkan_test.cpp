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

#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#include "render_context/render_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
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
 * @tc.name: ClearBuffer001
 * @tc.desc: test results of ClearBuffer
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceOhosVulkanTest, ClearBuffer001, TestSize.Level1)
{
    sptr<Surface> producer = nullptr;
    RSSurfaceOhosVulkan rsSurface(producer);
    rsSurface.ClearBuffer();
}

/**
 * @tc.name: ClearBuffer002
 * @tc.desc: test results of ClearBuffer
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceOhosVulkanTest, ClearBuffer002, TestSize.Level1)
{
    RSSurfaceOhosVulkan rsSurface(IConsumerSurface::Create());
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        int32_t width = 1;
        int32_t height = 1;
        uint64_t uiTimestamp = 1;
        rsSurface.RequestFrame(width, height, uiTimestamp);
    }
#endif
    rsSurface.ClearBuffer();
}

/**
 * @tc.name: ResetBufferAge001
 * @tc.desc: test results of ResetBufferAge
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceOhosVulkanTest, ResetBufferAge001, TestSize.Level1)
{
    sptr<Surface> producer = nullptr;
    RSSurfaceOhosVulkan rsSurface(producer);
    rsSurface.ResetBufferAge();
}

/**
 * @tc.name: ResetBufferAge002
 * @tc.desc: test results of ResetBufferAge
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceOhosVulkanTest, ResetBufferAge002, TestSize.Level1)
{
    RSSurfaceOhosVulkan rsSurface(IConsumerSurface::Create());
    rsSurface = std::make_shared<RSSurfaceOhosVulkan>(targetSurface);
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        rsSurface.SetRenderContext(renderContext);
        int32_t width = 1;
        int32_t height = 1;
        uint64_t uiTimestamp = 1;
        rsSurface.RequestFrame(width, height, uiTimestamp);
    }
#endif
    rsSurface.ResetBufferAge();
}

/**
 * @tc.name: SetNativeWindowInfo001
 * @tc.desc: test results of SetNativeWindowInfo
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceOhosVulkanTest, SetNativeWindowInfo001, TestSize.Level1)
{
    sptr<Surface> producer = nullptr;
    RSSurfaceOhosVulkan rsSurface(producer);
    int32_t width = 1;
    int32_t height = -1;
    rsSurface.SetNativeWindowInfo(width, height, false, false); // true

    width = -1;
    height = 1;
    rsSurface.SetNativeWindowInfo(width, height, true, true); // true

    width = 1;
    height = 1;
    rsSurface.SetNativeWindowInfo(width, height, true, false); // true

    width = -1;
    height = -1;
    rsSurface.SetNativeWindowInfo(width, height, true, true); // false

    EXPECT_TRUE(rsSurface.mSurfaceMap.empty());
}
/**
 * @tc.name: RequestNativeWindowBuffer001
 * @tc.desc: test results of RequestNativeWindowBuffer
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceOhosVulkanTest, RequestNativeWindowBuffer001, TestSize.Level1)
{
    sptr<Surface> producer = nullptr;
    RSSurfaceOhosVulkan rsSurface(producer);

    int32_t width = 1;
    int32_t height = 5;
    int fenceFd = -1;
    bool useAFBC = true;
    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    auto res = rsSurface.RequestNativeWindowBuffer(&nativeWindowBuffer, width, height, fenceFd, useAFBC);
    EXPECT_TRUE(ret != GSERROR_OK);
}
/**
 * @tc.name: RequestFrame001
 * @tc.desc: test results of RequestFrame
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceOhosVulkanTest, RequestFrame001, TestSize.Level1)
{
    RSSurfaceOhosVulkan rsSurface(IConsumerSurface::Create());

    int32_t width = 1;
    int32_t height = 1;
    uint64_t uiTimestamp = 1;
    std::unique_ptr<RSSurfaceFrame> ret = rsSurface.RequestFrame(width, height, uiTimestamp, true, true);
    EXPECT_TRUE(ret == nullptr);
}
/**
 * @tc.name: GetCurrentBuffer001
 * @tc.desc: test results of GetCurrentBuffer
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceOhosVulkanTest, GetCurrentBuffer001, TestSize.Level1)
{
    RSSurfaceOhosVulkan rsSurface(IConsumerSurface::Create());
    sptr<SurfaceBuffer> ret = rsSurface.GetCurrentBuffer();
    EXPECT_TRUE(ret == nullptr);

    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    rsSurface.mSurfaceList.emplace_back(nativeWindowBuffer);
    ret = rsSurface.GetCurrentBuffer();
    EXPECT_TRUE(ret != nullptr);
}

/**
 * @tc.name: SetColorSpace001
 * @tc.desc: test results of SetColorSpace
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceOhosVulkanTest, SetColorSpace001, TestSize.Level1)
{
    RSSurfaceOhosVulkan rsSurface(IConsumerSurface::Create());
    rsSurface.SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_ADOBE_RGB);
    ASSERT_EQ(rsSurface->colorSpace_, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_ADOBE_RGB);
}

/**
 * @tc.name: SetSurfacePixelFormat001
 * @tc.desc: test results of SetSurfacePixelFormat
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceOhosVulkanTest, SetSurfacePixelFormat001, TestSize.Level1)
{
    RSSurfaceOhosVulkan rsSurface(IConsumerSurface::Create());
    rsSurface.SetSurfacePixelFormat(11);
    ASSERT_EQ(rsSurface->pixelFormat_, GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBX_8888);
}

/**
 * @tc.name: ClearBuffer001
 * @tc.desc: test results of ClearBuffer
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceOhosVulkanTest, ClearBuffer001, TestSize.Level1)
{
    RSSurfaceOhosVulkan rsSurface(IConsumerSurface::Create());
    rsSurface.ClearBuffer();
    EXPECT_TRUE(rsSurface.producer_ != nullptr);
}

/**
 * @tc.name: FlushFrame001
 * @tc.desc: test results of FlushFrame
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceOhosVulkanTest, FlushFrame001, TestSize.Level1)
{
    sptr<Surface> producer = nullptr;
    RSSurfaceOhosVulkan rsSurface(producer);
    uint64_t uiTimestamp = 1;
    std::unique_ptr<RSSurfaceFrame> frame = nullptr;
    rsSurface.SetUiTimeStamp(frame, uiTimestamp);
    EXPECT_FALSE(rsSurface.FlushFrame(frame, uiTimestamp));
    {
        sptr<Surface> producer = nullptr;
        RSSurfaceOhosGl rsSurface(producer);
        RenderContext renderContext;
        rsSurface.SetRenderContext(&renderContext);
        ASSERT_TRUE(rsSurface.FlushFrame(frame, uiTimestamp));
    }
}
} // namespace Rosen
} // namespace OHOS