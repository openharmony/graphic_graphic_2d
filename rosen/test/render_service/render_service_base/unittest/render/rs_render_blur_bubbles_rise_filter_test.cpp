/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "effect/rs_render_filter_base.h"
#include "ge_visual_effect.h"
#include "ge_visual_effect_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderBlurBubblesRiseFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderBlurBubblesRiseFilterTest::SetUpTestCase() {}
void RSRenderBlurBubblesRiseFilterTest::TearDownTestCase() {}
void RSRenderBlurBubblesRiseFilterTest::SetUp() {}
void RSRenderBlurBubblesRiseFilterTest::TearDown() {}

/**
 * @tc.name: BlurBubblesRiseCreateAndType001
 * @tc.desc: Verify blur bubbles rise filter type and factory create
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderBlurBubblesRiseFilterTest, BlurBubblesRiseCreateAndType001, TestSize.Level1)
{
    auto filter = std::make_shared<RSNGRenderBlurBubblesRiseFilter>();
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->GetType(), RSNGEffectType::BLUR_BUBBLES_RISE);

    auto factoryFilter = RSNGRenderFilterBase::Create(RSNGEffectType::BLUR_BUBBLES_RISE);
    ASSERT_NE(factoryFilter, nullptr);
    EXPECT_EQ(factoryFilter->GetType(), RSNGEffectType::BLUR_BUBBLES_RISE);
}

/**
 * @tc.name: BlurBubblesRiseSetterGetter001
 * @tc.desc: Verify setter/getter for blur bubbles rise properties
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderBlurBubblesRiseFilterTest, BlurBubblesRiseSetterGetter001, TestSize.Level1)
{
    auto filter = std::make_shared<RSNGRenderBlurBubblesRiseFilter>();
    ASSERT_NE(filter, nullptr);

    constexpr float blurRadius = 5.0f;
    constexpr float mixStrength = 0.6f;
    constexpr int invertMask = 1;
    constexpr int maskChannel = 2;
    constexpr float maskScrollSpeed = 0.18f;
    constexpr float timePhase = 0.62f;

    filter->Setter<BlurBubblesRiseBlurRadiusRenderTag>(blurRadius);
    filter->Setter<BlurBubblesRiseMixStrengthRenderTag>(mixStrength);
    filter->Setter<BlurBubblesRiseInvertMaskRenderTag>(invertMask);
    filter->Setter<BlurBubblesRiseMaskChannelRenderTag>(maskChannel);
    filter->Setter<BlurBubblesRiseMaskScrollSpeedRenderTag>(maskScrollSpeed);
    filter->Setter<BlurBubblesRiseTimePhaseRenderTag>(timePhase);
    filter->Setter<BlurBubblesRiseMaskImageRenderTag>(nullptr);

    EXPECT_FLOAT_EQ(filter->Getter<BlurBubblesRiseBlurRadiusRenderTag>()->Get(), blurRadius);
    EXPECT_FLOAT_EQ(filter->Getter<BlurBubblesRiseMixStrengthRenderTag>()->Get(), mixStrength);
    EXPECT_EQ(filter->Getter<BlurBubblesRiseInvertMaskRenderTag>()->Get(), invertMask);
    EXPECT_EQ(filter->Getter<BlurBubblesRiseMaskChannelRenderTag>()->Get(), maskChannel);
    EXPECT_FLOAT_EQ(filter->Getter<BlurBubblesRiseMaskScrollSpeedRenderTag>()->Get(), maskScrollSpeed);
    EXPECT_FLOAT_EQ(filter->Getter<BlurBubblesRiseTimePhaseRenderTag>()->Get(), timePhase);
    EXPECT_EQ(filter->Getter<BlurBubblesRiseMaskImageRenderTag>()->Get(), nullptr);
}

/**
 * @tc.name: BlurBubblesRiseGenerateGEVisualEffect001
 * @tc.desc: Verify blur bubbles rise can generate GE visual effect
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderBlurBubblesRiseFilterTest, BlurBubblesRiseGenerateGEVisualEffect001, TestSize.Level1)
{
    auto filter = std::make_shared<RSNGRenderBlurBubblesRiseFilter>();
    ASSERT_NE(filter, nullptr);

    filter->Setter<BlurBubblesRiseBlurRadiusRenderTag>(3.5f);
    filter->Setter<BlurBubblesRiseMixStrengthRenderTag>(0.8f);
    filter->Setter<BlurBubblesRiseInvertMaskRenderTag>(1);
    filter->Setter<BlurBubblesRiseMaskChannelRenderTag>(4);
    filter->Setter<BlurBubblesRiseMaskScrollSpeedRenderTag>(0.2f);
    filter->Setter<BlurBubblesRiseTimePhaseRenderTag>(0.5f);
    filter->Setter<BlurBubblesRiseMaskImageRenderTag>(nullptr);

    std::shared_ptr<RSNGRenderFilterBase> baseFilter = filter;
    RSNGRenderFilterHelper::GenerateGEVisualEffect(baseFilter);

    ASSERT_NE(baseFilter->geFilter_, nullptr);
    ASSERT_NE(baseFilter->geFilter_->GetImpl(), nullptr);
    EXPECT_EQ(baseFilter->geFilter_->GetImpl()->GetFilterType(),
        Drawing::GEVisualEffectImpl::FilterType::BLUR_BUBBLES_RISE);
}
} // namespace OHOS::Rosen
