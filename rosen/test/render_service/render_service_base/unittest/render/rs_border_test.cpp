/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "include/render/rs_border.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSBorderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSBorderTest::SetUpTestCase() {}
void RSBorderTest::TearDownTestCase() {}
void RSBorderTest::SetUp() {}
void RSBorderTest::TearDown() {}

/**
 * @tc.name: LifeCycle001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, LifeCycle001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by Gradient
     */
    RSColor color(1, 1, 1);
    std::shared_ptr<RSBorder>border = std::make_shared<RSBorder>();
    ASSERT_TRUE(border->GetColor(0) == RgbPalette::Transparent());
    ASSERT_TRUE(border->GetStyle(0) == BorderStyle::NONE);
    border->SetColor(color);
    border->GetColor(0);
    RSColor color2(1, 1, 1);
    border->SetColor(color2);
    border->GetColor(1);
    border->SetWidth(1.f);
    border->SetWidth(2.f);
    ASSERT_TRUE(border->GetWidth(1) == 2.f);
    border->SetStyle(BorderStyle::DOTTED);
    border->SetStyle(BorderStyle::DOTTED);
    ASSERT_TRUE(border->GetStyle(1) == BorderStyle::DOTTED);
}

/**
 * @tc.name: LifeCycle002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, LifeCycle002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by Gradient
     */
    Color color1(1, 1, 1);
    Color color2(2, 2, 2);
    Color color3(3, 3, 3);
    Color color4(4, 4, 4);
    Vector4<Color> vectorColor(color1, color2, color3, color4);
    std::shared_ptr<RSBorder>border = std::make_shared<RSBorder>();
    border->GetColorFour();
    border->SetColorFour(vectorColor);
    border->GetColorFour();
    Vector4<uint32_t>vectorStyle(1, 2, 3, 4);
    border->GetStyleFour();
    border->SetStyleFour(vectorStyle);
    border->GetStyleFour();
}
}

