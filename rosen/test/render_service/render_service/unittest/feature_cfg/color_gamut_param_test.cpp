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
 * @tc.name: SetSLRCloseP3 IsSLRCloseP3
 * @tc.desc: Verify the SetSLRCloseP3 IsSLRCloseP3 function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(ColorGamutParamTest, SLRCloseP3, Function | SmallTest | Level1)
{
    ColorGamutParam::SetSLRCloseP3(true);
    EXPECT_EQ(ColorGamutParam::IsSLRCloseP3(), true);
    ColorGamutParam::SetSLRCloseP3(false);
    EXPECT_EQ(ColorGamutParam::IsSLRCloseP3(), false);
}

} // namespace Rosen
} // namespace OHOS