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

#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"
#include "render/rs_render_displacement_distort_filter.h"
#include "render/rs_shader_mask.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderDisplacementDistortFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderDisplacementDistortFilterTest::SetUpTestCase() {}
void RSRenderDisplacementDistortFilterTest::TearDownTestCase() {}
void RSRenderDisplacementDistortFilterTest::SetUp() {}
void RSRenderDisplacementDistortFilterTest::TearDown() {}

/**
 * @tc.name: GetDescription001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderDisplacementDistortFilterTest, GetDescription001, TestSize.Level1)
{
    auto rsRenderDispDistortFilterPara = std::make_shared<RSRenderDispDistortFilterPara>(0);
    std::string out;
    rsRenderDispDistortFilterPara->GetDescription(out);
    EXPECT_FALSE(out.empty());
}

/**
 * @tc.name: WriteToParcel001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderDisplacementDistortFilterTest, WriteToParcel001, TestSize.Level1)
{
    auto rsRenderDispDistortFilterPara = std::make_shared<RSRenderDispDistortFilterPara>(0);
    Parcel parcel;
    auto ret = rsRenderDispDistortFilterPara->WriteToParcel(parcel);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: ReadFromParcel001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderDisplacementDistortFilterTest, ReadFromParcel001, TestSize.Level1)
{
    auto rsRenderDispDistortFilterPara = std::make_shared<RSRenderDispDistortFilterPara>(0);
    Parcel parcel;
    auto ret = rsRenderDispDistortFilterPara->ReadFromParcel(parcel);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: CreateRenderPropert001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderDisplacementDistortFilterTest, CreateRenderPropert001, TestSize.Level1)
{
    auto rsRenderDispDistortFilterPara = std::make_shared<RSRenderDispDistortFilterPara>(0);

    auto renderPropert = rsRenderDispDistortFilterPara->CreateRenderPropert(RSUIFilterType::RIPPLE_MASK);
    EXPECT_NE(renderPropert, nullptr);

    renderPropert = rsRenderDispDistortFilterPara->CreateRenderPropert(RSUIFilterType::RADIAL_GRADIENT_MASK);
    EXPECT_EQ(renderPropert, nullptr);
}

/**
 * @tc.name: GetLeafRenderProperties001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderDisplacementDistortFilterTest, GetLeafRenderProperties001, TestSize.Level1)
{
    auto rsRenderDispDistortFilterPara = std::make_shared<RSRenderDispDistortFilterPara>(0);
    auto rsRenderPropertyBaseVec = rsRenderDispDistortFilterPara->GetLeafRenderProperties();
    EXPECT_TRUE(rsRenderPropertyBaseVec.empty());
}

/**
 * @tc.name: GetRenderMask001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderDisplacementDistortFilterTest, GetRenderMask001, TestSize.Level1)
{
    auto rsRenderDispDistortFilterPara = std::make_shared<RSRenderDispDistortFilterPara>(0);
    auto renderMask = rsRenderDispDistortFilterPara->GetRenderMask();
    EXPECT_EQ(renderMask, nullptr);
}

/**
 * @tc.name: GenerateGEVisualEffect001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderDisplacementDistortFilterTest, GenerateGEVisualEffect001, TestSize.Level1)
{
    auto rsDisplacementDistortFilter = std::make_shared<RSRenderDispDistortFilterPara>(0);

    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    rsDisplacementDistortFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_TRUE(visualEffectContainer->filterVec_.empty());

    rsDisplacementDistortFilter->mask_ =
        std::make_shared<RSShaderMask>(std::make_shared<RSRenderMaskPara>(RSUIFilterType::RIPPLE_MASK));
    rsDisplacementDistortFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GetFactor001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderDisplacementDistortFilterTest, GetFactor001, TestSize.Level1)
{
    auto rsDisplacementDistortFilter = std::make_shared<RSRenderDispDistortFilterPara>(0);
    // 1.f, 1.f is default factor
    EXPECT_EQ(rsDisplacementDistortFilter->GetFactor(), Vector2f(1.f, 1.f));
    rsDisplacementDistortFilter->factor_ = Vector2f(2.f, 1.f);
    EXPECT_EQ(rsDisplacementDistortFilter->GetFactor(), Vector2f(2.f, 1.f));
}

/**
 * @tc.name: GetMask001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderDisplacementDistortFilterTest, GetMask001, TestSize.Level1)
{
    auto rsDisplacementDistortFilter = std::make_shared<RSRenderDispDistortFilterPara>(0);
    auto mask = std::make_shared<RSShaderMask>(std::make_shared<RSRenderMaskPara>(RSUIFilterType::RIPPLE_MASK));
    rsDisplacementDistortFilter->mask_ = mask;
    auto tempMask = rsDisplacementDistortFilter->GetMask();
    EXPECT_EQ(tempMask, mask);
}

/**
 * @tc.name: GetMask002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderDisplacementDistortFilterTest, GetMask002, TestSize.Level1)
{
    auto rsDisplacementDistortFilter = std::make_shared<RSRenderDispDistortFilterPara>(0);
    rsDisplacementDistortFilter->mask_ = nullptr;
    auto tempMask = rsDisplacementDistortFilter->GetMask();
    EXPECT_EQ(tempMask, nullptr);
}

/**
 * @tc.name: ParseFilterValuesTest001
 * @tc.desc: Verify function ParseFilterValues
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderDisplacementDistortFilterTest, ParseFilterValuesTest001, TestSize.Level1)
{
    auto filter = std::make_shared<RSRenderDispDistortFilterPara>(0, RSUIFilterType::RIPPLE_MASK);

    EXPECT_FALSE(filter->ParseFilterValues());

    auto factProperty = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(Vector2f(1.f, 1.f), 0);
    filter->Setter(RSUIFilterType::DISPLACEMENT_DISTORT_FACTOR, factProperty);
    EXPECT_FALSE(filter->ParseFilterValues());

    auto maskRenderProperty = std::make_shared<RSRenderMaskPara>(RSUIFilterType::RIPPLE_MASK);
    filter->Setter(RSUIFilterType::RIPPLE_MASK, maskRenderProperty);
    EXPECT_TRUE(filter->ParseFilterValues());
}
} // namespace OHOS::Rosen