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
    auto effect = RSRenderTransitionEffect::Unmarshalling(parcel1);
    EXPECT_EQ(effect, nullptr);

    Parcel parcel2;
    parcel2.WriteUint16(10);
    effect = RSRenderTransitionEffect::Unmarshalling(parcel2);
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
    auto effect = RSTransitionScale::Unmarshalling(parcel1);
    EXPECT_EQ(effect, nullptr);

    Parcel parcel2;
    parcel2.WriteFloat(1.0f);
    effect = RSTransitionScale::Unmarshalling(parcel2);
    EXPECT_EQ(effect, nullptr);

    Parcel parcel3;
    parcel3.WriteFloat(1.0f);
    parcel3.WriteFloat(1.0f);
    effect = RSTransitionScale::Unmarshalling(parcel3);
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
    auto effect = RSTransitionTranslate::Unmarshalling(parcel1);
    EXPECT_EQ(effect, nullptr);

    Parcel parcel2;
    parcel2.WriteFloat(1.0f);
    effect = RSTransitionTranslate::Unmarshalling(parcel2);
    EXPECT_EQ(effect, nullptr);

    Parcel parcel3;
    parcel3.WriteFloat(1.0f);
    parcel3.WriteFloat(1.0f);
    effect = RSTransitionTranslate::Unmarshalling(parcel3);
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
    auto effect = RSTransitionRotate::Unmarshalling(parcel1);
    EXPECT_EQ(effect, nullptr);

    Parcel parcel2;
    parcel2.WriteFloat(1.0f);
    effect = RSTransitionRotate::Unmarshalling(parcel2);
    EXPECT_EQ(effect, nullptr);

    Parcel parcel3;
    parcel3.WriteFloat(1.0f);
    parcel3.WriteFloat(1.0f);
    effect = RSTransitionRotate::Unmarshalling(parcel3);
    EXPECT_EQ(effect, nullptr);

    Parcel parcel4;
    parcel4.WriteFloat(1.0f);
    parcel4.WriteFloat(1.0f);
    parcel4.WriteFloat(1.0f);
    effect = RSTransitionRotate::Unmarshalling(parcel4);
    EXPECT_EQ(effect, nullptr);
    GTEST_LOG_(INFO) << "RSRenderTransitionEffectTest TRSTransitionRotateUnmarshallingTest001 end";
}

} // namespace Rosen
} // namespace OHOS