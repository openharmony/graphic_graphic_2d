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

#include <cmath>

#include "gtest/gtest.h"

#include "common/rs_color.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSColorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSColorTest::SetUpTestCase() {}
void RSColorTest::TearDownTestCase() {}
void RSColorTest::SetUp() {}
void RSColorTest::TearDown() {}

/**
 * @tc.name: Operator001
 * @tc.desc: test results of operator/
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorTest, Operator001, TestSize.Level1)
{
    int16_t red = 10;
    int16_t green = 11;
    int16_t blue = 12;
    int16_t alpha = 13;
    RSColor color(red, green, blue, alpha);
    // operator/ is overridden to do nothing when scale is equal to 0
    float scale = 0;
    RSColor scaledColor = color / scale;
    EXPECT_EQ(scaledColor.GetRed(), color.GetRed());
    EXPECT_EQ(scaledColor.GetGreen(), color.GetGreen());
    EXPECT_EQ(scaledColor.GetBlue(), color.GetBlue());
    EXPECT_EQ(scaledColor.GetAlpha(), color.GetAlpha());

    scale = 3;
    scaledColor = color / scale;
    int16_t scaledRed = 3;
    int16_t scaledGreen = 4;
    int16_t scaledBlue = 4;
    int16_t scaledAlpha = 4;
    EXPECT_EQ(scaledColor.GetRed(), scaledRed);
    EXPECT_EQ(scaledColor.GetGreen(), scaledGreen);
    EXPECT_EQ(scaledColor.GetBlue(), scaledBlue);
    EXPECT_EQ(scaledColor.GetAlpha(), scaledAlpha);
}
} // namespace OHOS::Rosen