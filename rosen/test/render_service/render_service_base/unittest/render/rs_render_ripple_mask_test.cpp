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

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderRippleMaskTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderRippleMaskTest::SetUpTestCase() {}
void RSRenderRippleMaskTest::TearDownTestCase() {}
void RSRenderRippleMaskTest::SetUp() {}
void RSRenderRippleMaskTest::TearDown() {}

/**
 * @tc.name: CreateRenderPropert001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderRippleMaskTest, CreateRenderPropert001, TestSize.Level1)
{
    auto renderPropert = RSRenderRippleMaskPara::CreateRenderProperty(RSUIFilterType::RIPPLE_MASK_RADIUS);
    EXPECT_NE(renderPropert, nullptr);

    renderPropert = RSRenderRippleMaskPara::CreateRenderProperty(RSUIFilterType::RIPPLE_MASK_WIDTH);
    EXPECT_NE(renderPropert, nullptr);

    renderPropert = RSRenderRippleMaskPara::CreateRenderProperty(RSUIFilterType::RIPPLE_MASK_CENTER);
    EXPECT_NE(renderPropert, nullptr);

    renderPropert = RSRenderRippleMaskPara::CreateRenderProperty(RSUIFilterType::RIPPLE_MASK_WIDTH_CENTER_OFFSET);
    EXPECT_NE(renderPropert, nullptr);

    renderPropert = RSRenderRippleMaskPara::CreateRenderProperty(RSUIFilterType::BLUR);
    EXPECT_EQ(renderPropert, nullptr);
}

/**
 * @tc.name: GetAnimatRenderProperty001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderRippleMaskTest, GetAnimatRenderProperty001, TestSize.Level1)
{
    auto rsRenderRippleMaskPara = std::make_shared<RSRenderRippleMaskPara>(0);
    auto uiFilterType = RSUIFilterType::RIPPLE_MASK;
    auto animatRenderProperty = rsRenderRippleMaskPara->GetAnimatRenderProperty<Vector2f>(uiFilterType);
    EXPECT_EQ(animatRenderProperty, nullptr);
}

/**
 * @tc.name: GetDescription001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderRippleMaskTest, GetDescription001, TestSize.Level1)
{
    auto rsRenderRippleMaskPara = std::make_shared<RSRenderRippleMaskPara>(0);
    std::string out;
    rsRenderRippleMaskPara->GetDescription(out);
    EXPECT_FALSE(out.empty());
}

/**
 * @tc.name: WriteToParcel001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderRippleMaskTest, WriteToParcel001, TestSize.Level1)
{
    auto rsRenderRippleMaskPara = std::make_shared<RSRenderRippleMaskPara>(0);
    Parcel parcel;
    auto ret = rsRenderRippleMaskPara->WriteToParcel(parcel);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: ReadFromParcel001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderRippleMaskTest, ReadFromParcel001, TestSize.Level1)
{
    auto rsRenderRippleMaskPara = std::make_shared<RSRenderRippleMaskPara>(0);
    Parcel parcel;
    auto ret = rsRenderRippleMaskPara->ReadFromParcel(parcel);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: GetLeafRenderProperties001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderRippleMaskTest, GetLeafRenderProperties001, TestSize.Level1)
{
    auto rsRenderRippleMaskPara = std::make_shared<RSRenderRippleMaskPara>(0);
    auto rsRenderPropertyBaseVec = rsRenderRippleMaskPara->GetLeafRenderProperties();
    EXPECT_TRUE(rsRenderPropertyBaseVec.empty());
}

/**
 * @tc.name: LimitedDeepCopy
 * @tc.desc: test for LimitedDeepCopy
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderRippleMaskTest, LimitedDeepCopy001, TestSize.Level1)
{
    auto rsRenderRippleMaskPara = std::make_shared<RSRenderRippleMaskPara>(0);
    rsRenderRippleMaskPara->modifierType_ = RSModifierType::BACKGROUND_UI_FILTER;
    rsRenderRippleMaskPara->Setter(RSUIFilterType::RIPPLE_MASK, nullptr);

    // center
    Vector2f center = {5.f, 2.f};
    auto centerProperty = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(center, 0);
    rsRenderRippleMaskPara->Setter(RSUIFilterType::RIPPLE_MASK_CENTER, centerProperty);

    // radius
    float radius = 7.2;
    auto radiusProperty = std::make_shared<RSRenderAnimatableProperty<float>>(radius, 0);
    rsRenderRippleMaskPara->Setter(RSUIFilterType::RIPPLE_MASK_RADIUS, radiusProperty);

    // width
    float width = 3.4;
    auto widthProperty = std::make_shared<RSRenderAnimatableProperty<float>>(width, 0);
    rsRenderRippleMaskPara->Setter(RSUIFilterType::RIPPLE_MASK_WIDTH, widthProperty);

    // center offset
    float centerOffset = 2.2;
    auto centerOffsetProperty = std::make_shared<RSRenderAnimatableProperty<float>>(centerOffset, 0);
    rsRenderRippleMaskPara->Setter(RSUIFilterType::RIPPLE_MASK_WIDTH_CENTER_OFFSET, centerOffsetProperty);

    rsRenderRippleMaskPara->Setter(RSUIFilterType::EDGE_LIGHT, centerOffsetProperty);

    auto para = rsRenderRippleMaskPara->LimitedDeepCopy();
    ASSERT_NE(para, nullptr);
    auto newRippleMaskPara = std::static_pointer_cast<RSRenderRippleMaskPara>(para);

    EXPECT_EQ(rsRenderRippleMaskPara->id_, newRippleMaskPara->id_);
    EXPECT_EQ(rsRenderRippleMaskPara->type_, newRippleMaskPara->type_);
    EXPECT_EQ(rsRenderRippleMaskPara->modifierType_, newRippleMaskPara->modifierType_);

    // center
    ASSERT_NE(newRippleMaskPara->properties_[RSUIFilterType::RIPPLE_MASK_CENTER], nullptr);
    EXPECT_NE(rsRenderRippleMaskPara->properties_[RSUIFilterType::RIPPLE_MASK_CENTER],
        newRippleMaskPara->properties_[RSUIFilterType::RIPPLE_MASK_CENTER]);
    auto newCenter = std::static_pointer_cast<RSRenderAnimatableProperty<Vector2f>>(
        newRippleMaskPara->properties_[RSUIFilterType::RIPPLE_MASK_CENTER])->Get();
    EXPECT_EQ(center, newCenter);

    // radius
    ASSERT_NE(newRippleMaskPara->properties_[RSUIFilterType::RIPPLE_MASK_RADIUS], nullptr);
    EXPECT_NE(rsRenderRippleMaskPara->properties_[RSUIFilterType::RIPPLE_MASK_RADIUS],
        newRippleMaskPara->properties_[RSUIFilterType::RIPPLE_MASK_RADIUS]);
    auto newRadius = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(
        newRippleMaskPara->properties_[RSUIFilterType::RIPPLE_MASK_RADIUS])->Get();
    EXPECT_EQ(radius, newRadius);

    // width
    ASSERT_NE(newRippleMaskPara->properties_[RSUIFilterType::RIPPLE_MASK_WIDTH], nullptr);
    EXPECT_NE(rsRenderRippleMaskPara->properties_[RSUIFilterType::RIPPLE_MASK_WIDTH],
        newRippleMaskPara->properties_[RSUIFilterType::RIPPLE_MASK_WIDTH]);
    auto newWidth = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(
        newRippleMaskPara->properties_[RSUIFilterType::RIPPLE_MASK_WIDTH])->Get();
    EXPECT_EQ(width, newWidth);

    // center offset
    ASSERT_NE(newRippleMaskPara->properties_[RSUIFilterType::RIPPLE_MASK_WIDTH_CENTER_OFFSET], nullptr);
    EXPECT_NE(rsRenderRippleMaskPara->properties_[RSUIFilterType::RIPPLE_MASK_WIDTH_CENTER_OFFSET],
        newRippleMaskPara->properties_[RSUIFilterType::RIPPLE_MASK_WIDTH_CENTER_OFFSET]);
    auto newCenterOffset = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(
        newRippleMaskPara->properties_[RSUIFilterType::RIPPLE_MASK_WIDTH_CENTER_OFFSET])->Get();
    EXPECT_EQ(centerOffset, newCenterOffset);
}
} // namespace OHOS::Rosen