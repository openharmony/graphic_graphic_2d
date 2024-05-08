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
#include "modifier/rs_modifier_extractor.h"
#include "ui/rs_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
constexpr uint32_t SIZE_RSCOLOR = 255;
class RSModifierExtractorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSModifierExtractorTest::SetUpTestCase() {}
void RSModifierExtractorTest::TearDownTestCase() {}
void RSModifierExtractorTest::SetUp() {}
void RSModifierExtractorTest::TearDown() {}

/**
 * @tc.name: NodeTest001
 * @tc.desc: Node Test
 * @tc.type: FUNC
 * @tc.require:issueI9N22N
 */
HWTEST_F(RSModifierExtractorTest, NodeTest001, TestSize.Level1)
{
    RSModifierExtractor extractor;
    // Node is nullptr
    ASSERT_EQ(extractor.GetPivotZ(), 0.f);

    RSColor valTest1 { 0, 0, 0, 0 };

    ASSERT_EQ(extractor.GetSurfaceBgColor(), valTest1);
    ASSERT_EQ(extractor.GetBloom(), 0.f);

    // Node is not nullptr
    RSNode nodeTest(true);
    extractor.node_ = &nodeTest;
    ASSERT_EQ(extractor.GetBackgroundBlurRadius(), 0.f);

    ASSERT_EQ(extractor.GetBackgroundBlurSaturation(), 0.f);

    ASSERT_EQ(extractor.GetBackgroundBlurBrightness(), 0.f);

    ASSERT_EQ(extractor.GetBackgroundBlurMaskColor(), valTest1);

    ASSERT_EQ(extractor.GetBackgroundBlurColorMode(), BLUR_COLOR_MODE::DEFAULT);

    ASSERT_EQ(extractor.GetBackgroundBlurRadiusX(), 0.f);
    ASSERT_EQ(extractor.GetBackgroundBlurRadiusY(), 0.f);
    ASSERT_EQ(extractor.GetForegroundBlurRadius(), 0.f);
    ASSERT_EQ(extractor.GetForegroundBlurSaturation(), 0.f);
    ASSERT_EQ(extractor.GetForegroundBlurBrightness(), 0.f);

    ASSERT_EQ(extractor.GetForegroundBlurMaskColor(), valTest1);
    ASSERT_EQ(extractor.GetForegroundBlurColorMode(), BLUR_COLOR_MODE::DEFAULT);
    ASSERT_EQ(extractor.GetForegroundBlurRadiusX(), 0.f);
    ASSERT_EQ(extractor.GetForegroundBlurRadiusY(), 0.f);
    RSColor valTest2 { SIZE_RSCOLOR, SIZE_RSCOLOR, SIZE_RSCOLOR };
    ASSERT_EQ(extractor.GetLightColor(), valTest2);
}
} // namespace OHOS::Rosen