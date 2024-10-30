/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <parameter.h>
#include <parameters.h>

#include "gtest/gtest.h"
#include "system/rs_system_parameters.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSSystemParametersUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSystemParametersUnitTest::SetUpTestCase() {}
void RSSystemParametersUnitTest::TearDownTestCase() {}
void RSSystemParametersUnitTest::SetUp() {}
void RSSystemParametersUnitTest::TearDown() {}

/**
 * @tc.name: IsSkipProcessingTest
 * @tc.desc: Test RsNodeCostManagerTest.IsSkipProcessingTest
 * @tc.type: FUNC
 * @tc.require: issueI6P8EE
 */
HWTEST_F(RSSystemParametersUnitTest, GetCalcCostEnabledTest2, TestSize.Level1)
{
    auto result = RSSystemParameters::GetCalcCostEnabled();
    ASSERT_FALSE(result);
    system::SetParameter("rosen.calcCost.enabled", "1");
    result = RSSystemParameters::GetCalcCostEnabled();
    ASSERT_TRUE(result);
    system::SetParameter("rosen.calcCost.enabled", "0");
}

/**
 * @tc.name: GetDrawingCacheEnabledTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI6P8EE
 */
HWTEST_F(RSSystemParametersUnitTest, GetDrawingCacheEnabledTest1, TestSize.Level1)
{
    auto result = RSSystemParameters::GetDrawingCacheEnabled();
    system::SetParameter("rosen.drawingCache.enabled", "1");
    ASSERT_TRUE(result);
}

/**
 * @tc.name: GetDrawingCacheEnabledDfxTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI6P8EE
 */
HWTEST_F(RSSystemParametersUnitTest, GetDrawingCacheEnabledDfxTest1, TestSize.Level1)
{
    auto result = RSSystemParameters::GetDrawingCacheEnabledDfx();
    system::SetParameter("rosen.drawingCache.enabledDfx", "0");
    ASSERT_FALSE(result);
}

/**
 * @tc.name: GetQuickSkipPrepareTypeTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI6P8EE
 */
HWTEST_F(RSSystemParametersUnitTest, GetQuickSkipPrepareTypeTest1, TestSize.Level1)
{
    system::SetParameter("rosen.quickskipprepare.enabled", "2");
    auto result = RSSystemParameters::GetQuickSkipPrepareType();
    ASSERT_EQ(result, QuickSkipPrepareType::STATIC_APP_INSTANCE);
}

/**
 * @tc.name: IsNeedScRGBForP3Test
 * @tc.desc: Test ScRGB For P3 Controller
 * @tc.type: FUNC
 * @tc.require: issueIAWIC7
 */
HWTEST_F(RSSystemParametersUnitTest, IsNeedScRGBForP3Test1, TestSize.Level2)
{
    auto original = system::GetBoolParameter("persist.sys.graphic.scrgb.enabled", false);
    auto currentGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    auto result = RSSystemParameters::IsNeedScRGBForP3(currentGamut);
    EXPECT_EQ(original, result);
}
} // namespace OHOS::Rosen