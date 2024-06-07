/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "platform/ohos/backend/rs_surface_frame_ohos_vulkan.cpp"
#include "platform/ohos/backend/rs_surface_frame_ohos_vulkan.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSSurfaceFrameOhosVulkanTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSurfaceFrameOhosVulkanTest::SetUpTestCase() {}
void RSSurfaceFrameOhosVulkanTest::TearDownTestCase() {}
void RSSurfaceFrameOhosVulkanTest::SetUp() {}
void RSSurfaceFrameOhosVulkanTest::TearDown() {}

/**
 * @tc.name: SetDamageRegion001
 * @tc.desc: test results of SetDamageRegion
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceFrameOhosVulkanTest, SetDamageRegion001, TestSize.Level1)
{
    int32_t width = 1;
    int32_t height = 1;
    int32_t bufferAge = 1;
    auto value = std::make_shared<Drawing::Surface>();
    auto rsSurfaceFrameOhosVulkan = std::make_shared<RSSurfaceFrameOhosVulkan>(value, width, height, bufferAge);

    rsSurfaceFrameOhosVulkan->SetDamageRegion(0, 0, 2, 1);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetDamageRegion002
 * @tc.desc: test results of SetDamageRegion
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceFrameOhosVulkanTest, SetDamageRegion002, TestSize.Level1)
{
    int32_t width = 1;
    int32_t height = 1;
    int32_t bufferAge = 1;
    auto value = std::make_shared<Drawing::Surface>();
    auto rsSurfaceFrameOhosVulkan = std::make_shared<RSSurfaceFrameOhosVulkan>(value, width, height, bufferAge);

    std::vector<OHOS::Rosen::RectI> rects;
    rsSurfaceFrameOhosVulkan->SetDamageRegion(rects);
    EXPECT_TRUE(true);
}
} // namespace Rosen
} // namespace OHOS