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


#include "gtest/gtest.h"
#include "render/rs_colorful_shadow_filter.h"

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSColorfulShadowFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSColorfulShadowFilterTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}
void RSColorfulShadowFilterTest::TearDownTestCase() {}
void RSColorfulShadowFilterTest::SetUp() {}
void RSColorfulShadowFilterTest::TearDown() {}

/**
 * @tc.name: GetDescriptionTest
 * @tc.desc: test results of GetDescription
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorfulShadowFilterTest, GetDescriptionTest, TestSize.Level1)
{
    float blurRadius = 1.f;
    float offsetX = 0.1f;
    float offsetY = 0.1f;
    Drawing::Path path;
    Drawing::Rect rect(2.0f, 2.0f, 5.0f, 5.0f);
    path.AddRect(rect);
    RSColorfulShadowFilter shadowFilter(blurRadius, offsetX, offsetY, path, false);
    std::string expectRes = "RSColorfulShadowFilter " + std::to_string(blurRadius);
    EXPECT_EQ(shadowFilter.GetDescription(), expectRes);
}

/**
 * @tc.name: DrawImageRectTest001
 * @tc.desc: test DrawImageRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorfulShadowFilterTest, DrawImageRectTest001, TestSize.Level1)
{
    float blurRadius = 1.f;
    float offsetX = 0.1f;
    float offsetY = 0.1f;
    Drawing::Path path;
    Drawing::Rect rect(2.0f, 2.0f, 5.0f, 5.0f);
    path.AddRect(rect);
    RSColorfulShadowFilter shadowFilter(blurRadius, offsetX, offsetY, path, false);

    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    Drawing::Rect src;
    Drawing::Rect dst;
    shadowFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_EQ(image, nullptr);
}

/**
 * @tc.name: DrawImageRectTest002
 * @tc.desc: test DrawImageRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorfulShadowFilterTest, DrawImageRectTest002, TestSize.Level1)
{
    float blurRadius = 1.f;
    float offsetX = 0.1f;
    float offsetY = 0.1f;
    Drawing::Path path;
    Drawing::Rect rect(2.0f, 2.0f, 5.0f, 5.0f);
    path.AddRect(rect);
    RSColorfulShadowFilter shadowFilter(blurRadius, offsetX, offsetY, path, false);

    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();;
    Drawing::Rect src;
    Drawing::Rect dst;
    shadowFilter.DrawImageRect(canvas, image, src, dst);
    shadowFilter.blurRadius_ = 0.998f;
    EXPECT_FALSE(shadowFilter.IsValid());
    shadowFilter.DrawImageRect(canvas, image, src, dst);
}

/**
 * @tc.name: DrawImageRectTest003
 * @tc.desc: test DrawImageRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorfulShadowFilterTest, DrawImageRectTest003, TestSize.Level1)
{
    float blurRadius = 1.f;
    float offsetX = 0.1f;
    float offsetY = 0.1f;
    Drawing::Path path;
    Drawing::Rect rect(2.0f, 2.0f, 5.0f, 5.0f);
    path.AddRect(rect);
    RSColorfulShadowFilter shadowFilter(blurRadius, offsetX, offsetY, path, true);

    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();;
    Drawing::Rect src;
    Drawing::Rect dst;
    shadowFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(shadowFilter.IsValid());
}

} // namespace Rosen
} // namespace OHOS