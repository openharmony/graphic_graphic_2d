/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <test_header.h>

#include "color_gamut_param.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class ColorGamutParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ColorGamutParamTest::SetUpTestCase() {}
void ColorGamutParamTest::TearDownTestCase() {}
void ColorGamutParamTest::SetUp() {}
void ColorGamutParamTest::TearDown() {}

/**
 * @tc.name: DisableP3OnWiredExtendedScreen SetDisableP3OnWiredExtendedScreen
 * @tc.desc: Verify the DisableP3OnWiredExtendedScreen SetDisableP3OnWiredExtendedScreen function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(ColorGamutParamTest, DisableP3OnWiredExtendedScreen, Function | SmallTest | Level1)
{
    ColorGamutParam::SetDisableP3OnWiredExtendedScreen(true);
    EXPECT_EQ(ColorGamutParam::DisableP3OnWiredExtendedScreen(), true);
    ColorGamutParam::SetDisableP3OnWiredExtendedScreen(false);
    EXPECT_EQ(ColorGamutParam::DisableP3OnWiredExtendedScreen(), false);
}

/**
 * @tc.name: IsAdaptiveColorGamutEnabled SetAdaptiveColorGamutEnable
 * @tc.desc: Verify the IsAdaptiveColorGamutEnabled SetAdaptiveColorGamutEnable function 
 * @tc.type: FUNC
 * @tc.require: #IC82H3
 */
HWTEST_F(ColorGamutParamTest, AdaptiveColorGamutEnable, Function | SmallTest | Level1)
{
    ColorGamutParam::SetAdaptiveColorGamutEnable(true);
    EXPECT_EQ(ColorGamutParam::IsAdaptiveColorGamutEnabled(), true);
    ColorGamutParam::SetAdaptiveColorGamutEnable(false);
    EXPECT_EQ(ColorGamutParam::IsAdaptiveColorGamutEnabled(), false);
}

/**
 * @tc.name: SkipOccludedNodeDuringColorGamutCollection SetSkipOccludedNodeDuringColorGamutCollection
 * @tc.desc: Verify the SkipOccludedNodeDuringColorGamutCollection and
 *           SetSkipOccludedNodeDuringColorGamutCollection function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(ColorGamutParamTest, SkipOccludedNodeDuringColorGamutCollection, Function | SmallTest | Level1)
{
    ColorGamutParam::SetSkipOccludedNodeDuringColorGamutCollection(true);
    EXPECT_EQ(ColorGamutParam::SkipOccludedNodeDuringColorGamutCollection(), true);
    ColorGamutParam::SetSkipOccludedNodeDuringColorGamutCollection(false);
    EXPECT_EQ(ColorGamutParam::SkipOccludedNodeDuringColorGamutCollection(), false);
}

} // namespace Rosen
} // namespace OHOS