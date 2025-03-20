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
 * @tc.name: SetCoveredSurfaceCloseP3
 * @tc.desc: Verify the SetCoveredSurfaceCloseP3 function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(ColorGamutParamTest, SetCoveredSurfaceCloseP3, Function | SmallTest | Level1)
{
    ColorGamutParam::SetCoveredSurfaceCloseP3(true);
    EXPECT_EQ(ColorGamutParam::isCoveredSurfaceCloseP3_, true);
    ColorGamutParam::SetCoveredSurfaceCloseP3(false);
    EXPECT_EQ(ColorGamutParam::isCoveredSurfaceCloseP3_, false);
}

/**
 * @tc.name: SetSLRCloseP3
 * @tc.desc: Verify the SetSLRCloseP3 function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(ColorGamutParamTest, SetSLRCloseP3, Function | SmallTest | Level1)
{
    ColorGamutParam::SetSLRCloseP3(true);
    EXPECT_EQ(ColorGamutParam::isSLRCloseP3_, true);
    ColorGamutParam::SetSLRCloseP3(false);
    EXPECT_EQ(ColorGamutParam::isSLRCloseP3_, false);
}

/**
 * @tc.name: IsCoveredSurfaceCloseP3
 * @tc.desc: Verify the result of IsCoveredSurfaceCloseP3 function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(ColorGamutParamTest, IsCoveredSurfaceCloseP3, Function | SmallTest | Level1)
{
    ColorGamutParam::isCoveredSurfaceCloseP3_ = true;
    EXPECT_TRUE(ColorGamutParam::IsCoveredSurfaceCloseP3());
    ColorGamutParam::isCoveredSurfaceCloseP3_ = false;
    EXPECT_FALSE(ColorGamutParam::IsCoveredSurfaceCloseP3());
}

/**
 * @tc.name: IsSLRCloseP3
 * @tc.desc: Verify the result of IsSLRCloseP3 function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(ColorGamutParamTest, IsSLRCloseP3, Function | SmallTest | Level1)
{
    ColorGamutParam::isSLRCloseP3_ = true;
    EXPECT_TRUE(ColorGamutParam::IsSLRCloseP3());
    ColorGamutParam::isSLRCloseP3_ = false;
    EXPECT_FALSE(ColorGamutParam::IsSLRCloseP3());
}
} // namespace Rosen
} // namespace OHOS