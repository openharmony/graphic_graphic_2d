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

#include "gtest/gtest.h"
#include "ui_effect/property/include/rs_ui_mask_para.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUIMaskParaTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUIMaskParaTest::SetUpTestCase() {}
void RSUIMaskParaTest::TearDownTestCase() {}
void RSUIMaskParaTest::SetUp() {}
void RSUIMaskParaTest::TearDown() {}

/**
 * @tc.name: Equal001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIMaskParaTest, Equal001, TestSize.Level1)
{
    auto rsUIMaskPara1 = std::make_shared<RSUIMaskPara>(RSUIFilterType::RADIAL_GRADIENT_MASK);
    auto rsUIMaskPara2 = std::make_shared<RSUIMaskPara>(RSUIFilterType::RADIAL_GRADIENT_MASK);

    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIMaskPara2);
    EXPECT_TRUE(rsUIMaskPara1->Equals(rsUIFilterParaBase));
}

/**
 * @tc.name: ConvertMaskType001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIMaskParaTest, ConvertMaskType001, TestSize.Level1)
{
    auto type = RSUIMaskPara::ConvertMaskType(MaskPara::Type::NONE);
    EXPECT_EQ(type, RSUIFilterType::NONE);

    type = RSUIMaskPara::ConvertMaskType(MaskPara::Type::RADIAL_GRADIENT_MASK);
    EXPECT_EQ(type, RSUIFilterType::RADIAL_GRADIENT_MASK);

    type = RSUIMaskPara::ConvertMaskType(MaskPara::Type::RIPPLE_MASK);
    EXPECT_EQ(type, RSUIFilterType::RIPPLE_MASK);

    type = RSUIMaskPara::ConvertMaskType(MaskPara::Type::PIXEL_MAK_MASK);
    EXPECT_EQ(type, RSUIFilterType::NONE);
}
} // namespace OHOS::Rosen