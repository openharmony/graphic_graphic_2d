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
#include "render/rs_render_ripple_mask.h"
#include "ui_effect/property/include/rs_ui_ripple_mask.h"
#include "ui_effect/mask/include/ripple_mask_para.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUIRippleMaskTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUIRippleMaskTest::SetUpTestCase() {}
void RSUIRippleMaskTest::TearDownTestCase() {}
void RSUIRippleMaskTest::SetUp() {}
void RSUIRippleMaskTest::TearDown() {}

/**
 * @tc.name: Equal001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIRippleMaskTest, Equal001, TestSize.Level1)
{
    auto rsUIRippleMaskPara1 = std::make_shared<RSUIRippleMaskPara>();
    auto rsUIRippleMaskPara2 = std::make_shared<RSUIRippleMaskPara>();

    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIRippleMaskPara2);
    EXPECT_TRUE(rsUIRippleMaskPara1->Equals(rsUIFilterParaBase));
}

/**
 * @tc.name: SetProperty001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIRippleMaskTest, SetProperty001, TestSize.Level1)
{
    auto rippleMaskPara = std::make_shared<RippleMaskPara>();
    Vector2f center = Vector2f(0.0f, 0.0f);
    rippleMaskPara->SetCenter(center);
    float radius = 1.0f;
    rippleMaskPara->SetRadius(radius);
    float width = 0.5f;
    rippleMaskPara->SetWidth(width);

    auto maskPara = std::static_pointer_cast<MaskPara>(rippleMaskPara);

    auto rsUIRippleMaskPara1 = std::make_shared<RSUIRippleMaskPara>();
    auto rsUIRippleMaskPara2 = std::make_shared<RSUIRippleMaskPara>();
    rsUIRippleMaskPara2->SetPara(maskPara);

    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIRippleMaskPara2);
    rsUIRippleMaskPara1->SetProperty(rsUIFilterParaBase);

    auto iter = rsUIRippleMaskPara1->properties_.find(RSUIFilterType::RIPPLE_MASK_CENTER);
    ASSERT_NE(iter, rsUIRippleMaskPara1->properties_.end());

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second);
    ASSERT_NE(property, nullptr);
    EXPECT_EQ(property->Get(), center);
}

/**
 * @tc.name: SetPara001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIRippleMaskTest, SetPara001, TestSize.Level1)
{
    auto rippleMaskPara = std::make_shared<RippleMaskPara>();
    Vector2f center = Vector2f(0.0f, 0.0f);
    rippleMaskPara->SetCenter(center);
    float radius = 1.0f;
    rippleMaskPara->SetRadius(radius);
    float width = 0.5f;
    rippleMaskPara->SetWidth(width);

    auto maskPara = std::static_pointer_cast<MaskPara>(rippleMaskPara);

    auto rsUIRippleMaskPara = std::make_shared<RSUIRippleMaskPara>();

    rsUIRippleMaskPara->SetPara(maskPara);

    auto iter = rsUIRippleMaskPara->properties_.find(RSUIFilterType::RIPPLE_MASK_CENTER);
    ASSERT_NE(iter, rsUIRippleMaskPara->properties_.end());

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second);
    ASSERT_NE(property, nullptr);
    EXPECT_EQ(property->Get(), center);
}

/**
 * @tc.name: SetRadius001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIRippleMaskTest, SetRadius001, TestSize.Level1)
{
    auto rsUIRippleMaskPara = std::make_shared<RSUIRippleMaskPara>();
    float radius = 1.0f;
    rsUIRippleMaskPara->SetRadius(radius);

    auto iter = rsUIRippleMaskPara->properties_.find(RSUIFilterType::RIPPLE_MASK_RADIUS);
    ASSERT_NE(iter, rsUIRippleMaskPara->properties_.end());

    auto property = std::static_pointer_cast<RSAnimatableProperty<float>>(iter->second);
    ASSERT_NE(property, nullptr);
    EXPECT_EQ(property->Get(), radius);
}

/**
 * @tc.name: SetWidth001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIRippleMaskTest, SetWidth001, TestSize.Level1)
{
    auto rsUIRippleMaskPara = std::make_shared<RSUIRippleMaskPara>();
    float width = 0.5f;
    rsUIRippleMaskPara->SetWidth(width);

    auto iter = rsUIRippleMaskPara->properties_.find(RSUIFilterType::RIPPLE_MASK_WIDTH);
    ASSERT_NE(iter, rsUIRippleMaskPara->properties_.end());

    auto property = std::static_pointer_cast<RSAnimatableProperty<float>>(iter->second);
    ASSERT_NE(property, nullptr);
    EXPECT_EQ(property->Get(), width);
}

/**
 * @tc.name: SetCenter001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIRippleMaskTest, SetCenter001, TestSize.Level1)
{
    auto rsUIRippleMaskPara = std::make_shared<RSUIRippleMaskPara>();
    Vector2f center = Vector2f(0.0f, 0.0f);
    rsUIRippleMaskPara->SetCenter(center);

    auto iter = rsUIRippleMaskPara->properties_.find(RSUIFilterType::RIPPLE_MASK_CENTER);
    ASSERT_NE(iter, rsUIRippleMaskPara->properties_.end());

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second);
    ASSERT_NE(property, nullptr);
    EXPECT_EQ(property->Get(), center);
}

/**
 * @tc.name: CreateRSRenderFilter001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIRippleMaskTest, CreateRSRenderFilter001, TestSize.Level1)
{
    auto rsUIRippleMaskPara = std::make_shared<RSUIRippleMaskPara>();
    float radius = 1.0f;
    rsUIRippleMaskPara->SetRadius(radius);
    float width = 0.5f;
    rsUIRippleMaskPara->SetWidth(width);
    Vector2f center = Vector2f(0.0f, 0.0f);
    rsUIRippleMaskPara->SetCenter(center);

    auto rsRenderFilterParaBase = rsUIRippleMaskPara->CreateRSRenderFilter();
    EXPECT_NE(rsRenderFilterParaBase, nullptr);
}

/**
 * @tc.name: GetLeafProperties001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIRippleMaskTest, GetLeafProperties001, TestSize.Level1)
{
    auto rsUIRippleMaskPara = std::make_shared<RSUIRippleMaskPara>();
    float radius = 1.0f;
    rsUIRippleMaskPara->SetRadius(radius);
    float width = 0.5f;
    rsUIRippleMaskPara->SetWidth(width);
    Vector2f center = Vector2f(0.0f, 0.0f);
    rsUIRippleMaskPara->SetCenter(center);

    auto rsPropertyBaseVector = rsUIRippleMaskPara->GetLeafProperties();
    EXPECT_NE(rsPropertyBaseVector.size(), 0);
}
} // namespace OHOS::Rosen