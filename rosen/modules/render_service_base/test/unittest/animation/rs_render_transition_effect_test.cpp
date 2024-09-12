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

#include "animation/rs_render_transition_effect.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSRenderTransitionEffectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderTransitionEffectTest::SetUpTestCase() {}
void RSRenderTransitionEffectTest::TearDownTestCase() {}
void RSRenderTransitionEffectTest::SetUp() {}
void RSRenderTransitionEffectTest::TearDown() {}

/**
 * @tc.name: RenderTransitionUnmarshallingTest001
 * @tc.desc: Verify the RenderTransition Unmarshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderTransitionEffectTest, RenderTransitionUnmarshallingTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderTransitionEffectTest RenderTransitionUnmarshallingTest001 start";

    Parcel parcel1;
    std::shared_ptr<RSRenderTransitionEffect> effect(RSRenderTransitionEffect::Unmarshalling(parcel1));
    EXPECT_EQ(effect, nullptr);

    Parcel parcel2;
    parcel2.WriteUint16(10);
    effect.reset(RSRenderTransitionEffect::Unmarshalling(parcel2));
    EXPECT_EQ(effect, nullptr);

    GTEST_LOG_(INFO) << "RSRenderTransitionEffectTest RenderTransitionUnmarshallingTest001 end";
}

/**
 * @tc.name: TransitionScaleUnmarshallingTest001
 * @tc.desc: Verify the TransitionScale Unmarshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderTransitionEffectTest, TransitionScaleUnmarshallingTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderTransitionEffectTest TransitionScaleUnmarshallingTest001 start";

    Parcel parcel1;
    std::shared_ptr<RSRenderTransitionEffect> effect(RSTransitionScale::Unmarshalling(parcel1));
    EXPECT_EQ(effect, nullptr);

    Parcel parcel2;
    parcel2.WriteFloat(1.0f);
    effect.reset(RSTransitionScale::Unmarshalling(parcel2));
    EXPECT_EQ(effect, nullptr);

    Parcel parcel3;
    parcel3.WriteFloat(1.0f);
    parcel3.WriteFloat(1.0f);
    effect.reset(RSTransitionScale::Unmarshalling(parcel3));
    EXPECT_EQ(effect, nullptr);

    GTEST_LOG_(INFO) << "RSRenderTransitionEffectTest TransitionScaleUnmarshallingTest001 end";
}

/**
 * @tc.name: TransitionTranslateUnmarshallingTest001
 * @tc.desc: Verify the TransitionTranslate Unmarshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderTransitionEffectTest, TransitionTranslateUnmarshallingTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderTransitionEffectTest TransitionTranslateUnmarshallingTest001 start";

    Parcel parcel1;
    std::shared_ptr<RSRenderTransitionEffect> effect(RSTransitionTranslate::Unmarshalling(parcel1));
    EXPECT_EQ(effect, nullptr);

    Parcel parcel2;
    parcel2.WriteFloat(1.0f);
    effect.reset(RSTransitionTranslate::Unmarshalling(parcel2));
    EXPECT_EQ(effect, nullptr);

    Parcel parcel3;
    parcel3.WriteFloat(1.0f);
    parcel3.WriteFloat(1.0f);
    effect.reset(RSTransitionTranslate::Unmarshalling(parcel3));
    EXPECT_EQ(effect, nullptr);

    GTEST_LOG_(INFO) << "RSRenderTransitionEffectTest TransitionTranslateUnmarshallingTest001 end";
}

/**
 * @tc.name: TRSTransitionRotateUnmarshallingTest001
 * @tc.desc: Verify the RSTransitionRotate Unmarshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderTransitionEffectTest, TRSTransitionRotateUnmarshallingTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderTransitionEffectTest TRSTransitionRotateUnmarshallingTest001 start";

    Parcel parcel1;
    std::shared_ptr<RSRenderTransitionEffect> effect(RSTransitionRotate::Unmarshalling(parcel1));
    EXPECT_EQ(effect, nullptr);

    Parcel parcel2;
    parcel2.WriteFloat(1.0f);
    effect.reset(RSTransitionRotate::Unmarshalling(parcel2));
    EXPECT_EQ(effect, nullptr);

    Parcel parcel3;
    parcel3.WriteFloat(1.0f);
    parcel3.WriteFloat(1.0f);
    effect.reset(RSTransitionRotate::Unmarshalling(parcel3));
    EXPECT_EQ(effect, nullptr);

    Parcel parcel4;
    parcel4.WriteFloat(1.0f);
    parcel4.WriteFloat(1.0f);
    parcel4.WriteFloat(1.0f);
    effect.reset(RSTransitionRotate::Unmarshalling(parcel4));
    EXPECT_EQ(effect, nullptr);
    GTEST_LOG_(INFO) << "RSRenderTransitionEffectTest TRSTransitionRotateUnmarshallingTest001 end";
}

/**
 * @tc.name: RSTransitionFadeUnmarshallingTest001
 * @tc.desc: Verify the RSTransitionFade Unmarshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderTransitionEffectTest, RSTransitionFadeUnmarshallingTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderTransitionEffectTest RSTransitionFadeUnmarshallingTest001 start";

    Parcel parcel1;
    std::shared_ptr<RSRenderTransitionEffect> effect(RSTransitionFade::Unmarshalling(parcel1));
    EXPECT_EQ(effect, nullptr);

    Parcel parcel2;
    parcel2.WriteUint16(1);
    parcel2.WriteFloat(1.0f);
    effect.reset(RSTransitionFade::Unmarshalling(parcel2));
    EXPECT_TRUE(effect != nullptr);
    GTEST_LOG_(INFO) << "RSRenderTransitionEffectTest RSTransitionFadeUnmarshallingTest001 end";
}

/**
 * @tc.name: UpdateFractionTest001
 * @tc.desc: Verify the RSRenderTransitionEffect UpdateFraction
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderTransitionEffectTest, UpdateFractionTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderTransitionEffectTest UpdateFractionTest001 start";

    auto fadeEffect = std::make_shared<RSTransitionFade>(1.0f);
    EXPECT_TRUE(fadeEffect != nullptr);
    fadeEffect->UpdateFraction(0.0f);
    auto modifier = fadeEffect->GetModifier();
    fadeEffect->UpdateFraction(0.0f);
    EXPECT_TRUE(modifier != nullptr);

    auto scaleEffect = std::make_shared<RSTransitionScale>(1.0f, 1.0f, 1.0f);
    EXPECT_TRUE(scaleEffect != nullptr);
    scaleEffect->UpdateFraction(0.0f);
    modifier = scaleEffect->GetModifier();
    scaleEffect->UpdateFraction(0.0f);
    EXPECT_TRUE(modifier != nullptr);

    auto translateEffect = std::make_shared<RSTransitionTranslate>(1.0f, 1.0f, 1.0f);
    EXPECT_TRUE(translateEffect != nullptr);
    translateEffect->UpdateFraction(0.0f);
    modifier = translateEffect->GetModifier();
    translateEffect->UpdateFraction(0.0f);
    EXPECT_TRUE(modifier != nullptr);

    auto rotateEffect = std::make_shared<RSTransitionRotate>(1.0f, 1.0f, 1.0f, 1.0f);
    EXPECT_TRUE(rotateEffect != nullptr);
    rotateEffect->UpdateFraction(0.0f);
    modifier = rotateEffect->GetModifier();
    rotateEffect->UpdateFraction(0.0f);
    EXPECT_TRUE(modifier != nullptr);

    GTEST_LOG_(INFO) << "RSRenderTransitionEffectTest UpdateFractionTest001 end";
}

/**
 * @tc.name: RSTransitionCustomTest001
 * @tc.desc: Verify the RSTransitionFade Unmarshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderTransitionEffectTest, RSTransitionCustomTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderTransitionEffectTest RSTransitionCustomTest001 start";

    auto property = std::make_shared<RSRenderPropertyBase>();
    auto startProp = std::make_shared<RSRenderPropertyBase>();
    auto endProp = std::make_shared<RSRenderPropertyBase>();
    RSTransitionCustom custom(property, startProp, endProp);
    custom.property_ = property;
    custom.InitValueEstimator();
    EXPECT_TRUE(custom.valueEstimator_ == nullptr);

    GTEST_LOG_(INFO) << "RSRenderTransitionEffectTest RSTransitionCustomTest001 end";
}
} // namespace Rosen
} // namespace OHOS