/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "render_context/render_context.h"

#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"

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
 * @tc.name: FlushFrame001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceOhosVulkanTest, FlushFrame001, TestSize.Level1)
{
    sptr<Surface> producer = nullptr;
    RSSurfaceOhosVulkan rsSurface(producer);
    uint64_t uiTimestamp = 1;
    std::unique_ptr<RSSurfaceFrame> frame = nullptr;
    rsSurface.SetUiTimeStamp(frame, uiTimestamp);
    EXPECT_FALSE(rsSurface.FlushFrame(frame, uiTimestamp));
}

/**
 * @tc.name: ClearBuffer001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceOhosVulkanTest, ClearBuffer001, TestSize.Level1)
{
    sptr<Surface> producer = nullptr;
    RSSurfaceOhosVulkan rsSurface(producer);
    rsSurface.ClearBuffer();
}

/**
 * @tc.name: ClearBuffer002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
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
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceOhosVulkanTest, ResetBufferAge001, TestSize.Level1)
{
    sptr<Surface> producer = nullptr;
    RSSurfaceOhosVulkan rsSurface(producer);
    rsSurface.ResetBufferAge();
}

/**
 * @tc.name: ResetBufferAge002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
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
} // namespace Rosen
} // namespace OHOS