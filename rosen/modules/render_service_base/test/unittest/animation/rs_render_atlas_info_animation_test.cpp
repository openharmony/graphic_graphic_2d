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

#include <gtest/gtest.h>

#include "animation/rs_render_atlas_info_animation.h"
#include "modifier/rs_render_property.h"
#include "transaction/rs_marshalling_helper.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSRenderAtlasInfoAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static constexpr uint64_t ANIMATION_ID = 12345;
    static constexpr uint64_t PROPERTY_ID = 54321;
};

void RSRenderAtlasInfoAnimationTest::SetUpTestCase() {}
void RSRenderAtlasInfoAnimationTest::TearDownTestCase() {}
void RSRenderAtlasInfoAnimationTest::SetUp() {}
void RSRenderAtlasInfoAnimationTest::TearDown() {}

namespace {
AtlasInfo MakeContextAtlasInfo(float frameIndex)
{
    AtlasInfo info;
    info.mode = 1;
    info.rows = 4; // 4 for rows
    info.cols = 4; // 4 for cols
    info.frameWidth = 360.0f;
    info.frameHeight = 360.0f;
    info.padding = 2.0f;
    info.totalFrame = 16; // 16 for total frames
    info.frameIndex = frameIndex;
    return info;
}

std::shared_ptr<RSRenderAnimatableProperty<AtlasInfo>> MakeAtlasProperty(const AtlasInfo& val)
{
    return std::make_shared<RSRenderAnimatableProperty<AtlasInfo>>(val);
}
} // namespace

/**
 * @tc.name: Construct001
 * @tc.desc: Verify construction extracts startValue/endValue
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderAtlasInfoAnimationTest, Construct001, TestSize.Level1)
{
    auto startProp = MakeAtlasProperty(MakeContextAtlasInfo(0.0f));
    auto endProp = MakeAtlasProperty(MakeContextAtlasInfo(15.0f));
    auto anim = std::make_shared<RSRenderAtlasInfoAnimation>(
        ANIMATION_ID, PROPERTY_ID, startProp, startProp, endProp);
    ASSERT_NE(anim, nullptr);
}

/**
 * @tc.name: ConstructNullStartValue
 * @tc.desc: Verify construction with null start/end does not crash
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderAtlasInfoAnimationTest, ConstructNullStartValue, TestSize.Level1)
{
    auto startProp = MakeAtlasProperty(MakeContextAtlasInfo(0.0f));
    auto anim = std::make_shared<RSRenderAtlasInfoAnimation>(
        ANIMATION_ID, PROPERTY_ID, startProp, nullptr, nullptr);
    ASSERT_NE(anim, nullptr);
}

/**
 * @tc.name: OnSetPropertyOnAdd001
 * @tc.desc: Verify OnSetPropertyOnAdd sets startValue to property
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderAtlasInfoAnimationTest, OnSetPropertyOnAdd001, TestSize.Level1)
{
    auto property = MakeAtlasProperty(MakeContextAtlasInfo(99.0f));
    auto startProp = MakeAtlasProperty(MakeContextAtlasInfo(0.0f));
    auto endProp = MakeAtlasProperty(MakeContextAtlasInfo(15.0f));
    auto anim = std::make_shared<RSRenderAtlasInfoAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, startProp, endProp);
    ASSERT_NE(anim, nullptr);

    anim->AttachRenderProperty(property);
    auto prop = std::static_pointer_cast<RSRenderProperty<AtlasInfo>>(anim->property_);
    ASSERT_NE(prop, nullptr);
    EXPECT_FLOAT_EQ(prop->Get().frameIndex, 0.0f);
}

/**
 * @tc.name: OnAnimateInterpolation001
 * @tc.desc: Verify OnAnimate linearly interpolates frameIndex at fraction 0.5
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderAtlasInfoAnimationTest, OnAnimateInterpolation001, TestSize.Level1)
{
    auto property = MakeAtlasProperty(MakeContextAtlasInfo(0.0f));
    auto startProp = MakeAtlasProperty(MakeContextAtlasInfo(0.0f));
    auto endProp = MakeAtlasProperty(MakeContextAtlasInfo(10.0f));
    auto anim = std::make_shared<RSRenderAtlasInfoAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, startProp, endProp);
    ASSERT_NE(anim, nullptr);

    anim->AttachRenderProperty(property);
    anim->OnAnimate(0.5f);
    auto prop = std::static_pointer_cast<RSRenderProperty<AtlasInfo>>(anim->property_);
    ASSERT_NE(prop, nullptr);
    EXPECT_FLOAT_EQ(prop->Get().frameIndex, 5.0f);
}

/**
 * @tc.name: OnAnimateInterpolationFraction0
 * @tc.desc: Verify OnAnimate at fraction 0 returns start frameIndex
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderAtlasInfoAnimationTest, OnAnimateInterpolationFraction0, TestSize.Level1)
{
    auto property = MakeAtlasProperty(MakeContextAtlasInfo(99.0f));
    auto startProp = MakeAtlasProperty(MakeContextAtlasInfo(3.0f));
    auto endProp = MakeAtlasProperty(MakeContextAtlasInfo(15.0f));
    auto anim = std::make_shared<RSRenderAtlasInfoAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, startProp, endProp);
    ASSERT_NE(anim, nullptr);

    anim->AttachRenderProperty(property);
    anim->OnAnimate(0.0f);
    auto prop = std::static_pointer_cast<RSRenderProperty<AtlasInfo>>(anim->property_);
    ASSERT_NE(prop, nullptr);
    EXPECT_FLOAT_EQ(prop->Get().frameIndex, 3.0f);
}

/**
 * @tc.name: OnAnimateInterpolationFraction1
 * @tc.desc: Verify OnAnimate at fraction 1 returns end frameIndex
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderAtlasInfoAnimationTest, OnAnimateInterpolationFraction1, TestSize.Level1)
{
    auto property = MakeAtlasProperty(MakeContextAtlasInfo(99.0f));
    auto startProp = MakeAtlasProperty(MakeContextAtlasInfo(0.0f));
    auto endProp = MakeAtlasProperty(MakeContextAtlasInfo(15.0f));
    auto anim = std::make_shared<RSRenderAtlasInfoAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, startProp, endProp);
    ASSERT_NE(anim, nullptr);

    anim->AttachRenderProperty(property);
    anim->OnAnimate(1.0f);
    auto prop = std::static_pointer_cast<RSRenderProperty<AtlasInfo>>(anim->property_);
    ASSERT_NE(prop, nullptr);
    EXPECT_FLOAT_EQ(prop->Get().frameIndex, 15.0f);
}

/**
 * @tc.name: OnAnimateContextCarryThrough
 * @tc.desc: Verify OnAnimate preserves context fields from startValue
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderAtlasInfoAnimationTest, OnAnimateContextCarryThrough, TestSize.Level1)
{
    auto property = MakeAtlasProperty(MakeContextAtlasInfo(0.0f));
    auto startProp = MakeAtlasProperty(MakeContextAtlasInfo(0.0f));
    auto endProp = MakeAtlasProperty(MakeContextAtlasInfo(15.0f));
    auto anim = std::make_shared<RSRenderAtlasInfoAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, startProp, endProp);
    ASSERT_NE(anim, nullptr);

    anim->AttachRenderProperty(property);
    anim->OnAnimate(0.5f);
    auto prop = std::static_pointer_cast<RSRenderProperty<AtlasInfo>>(anim->property_);
    ASSERT_NE(prop, nullptr);
    const auto& result = prop->Get();
    EXPECT_EQ(result.mode, 1);
    EXPECT_EQ(result.rows, 4);
    EXPECT_EQ(result.cols, 4);
    EXPECT_FLOAT_EQ(result.frameWidth, 360.0f);
    EXPECT_FLOAT_EQ(result.frameHeight, 360.0f);
    EXPECT_FLOAT_EQ(result.padding, 2.0f);
    EXPECT_EQ(result.totalFrame, 16);
}

/**
 * @tc.name: RebuildPropertyValueDelegatesToOnAnimate
 * @tc.desc: Verify RebuildPropertyValue calls OnAnimate
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderAtlasInfoAnimationTest, RebuildPropertyValueDelegatesToOnAnimate, TestSize.Level1)
{
    auto property = MakeAtlasProperty(MakeContextAtlasInfo(0.0f));
    auto startProp = MakeAtlasProperty(MakeContextAtlasInfo(0.0f));
    auto endProp = MakeAtlasProperty(MakeContextAtlasInfo(10.0f));
    auto anim = std::make_shared<RSRenderAtlasInfoAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, startProp, endProp);
    ASSERT_NE(anim, nullptr);

    anim->AttachRenderProperty(property);
    anim->RebuildPropertyValue(0.3f);
    auto prop = std::static_pointer_cast<RSRenderProperty<AtlasInfo>>(anim->property_);
    ASSERT_NE(prop, nullptr);
    EXPECT_FLOAT_EQ(prop->Get().frameIndex, 3.0f);
}

/**
 * @tc.name: InitValueEstimatorNoOp
 * @tc.desc: Verify InitValueEstimator is a no-op (does not crash)
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderAtlasInfoAnimationTest, InitValueEstimatorNoOp, TestSize.Level1)
{
    auto property = MakeAtlasProperty(MakeContextAtlasInfo(0.0f));
    auto startProp = MakeAtlasProperty(MakeContextAtlasInfo(0.0f));
    auto endProp = MakeAtlasProperty(MakeContextAtlasInfo(10.0f));
    auto anim = std::make_shared<RSRenderAtlasInfoAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, startProp, endProp);
    ASSERT_NE(anim, nullptr);
    anim->InitValueEstimator();
    SUCCEED();
}

/**
 * @tc.name: MarshallingRoundTrip
 * @tc.desc: Verify Marshalling/Unmarshalling round trip (null pixelMap)
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderAtlasInfoAnimationTest, MarshallingRoundTrip, TestSize.Level1)
{
    auto startProp = MakeAtlasProperty(MakeContextAtlasInfo(0.0f));
    auto endProp = MakeAtlasProperty(MakeContextAtlasInfo(15.0f));
    auto anim = std::make_shared<RSRenderAtlasInfoAnimation>(
        ANIMATION_ID, PROPERTY_ID, startProp, startProp, endProp);
    ASSERT_NE(anim, nullptr);

    Parcel parcel;
    EXPECT_TRUE(anim->Marshalling(parcel));
    auto recovered = RSRenderAtlasInfoAnimation::Unmarshalling(parcel);
    ASSERT_NE(recovered, nullptr);
}

/**
 * @tc.name: UnmarshallingEmptyParcel
 * @tc.desc: Verify Unmarshalling on empty parcel returns nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderAtlasInfoAnimationTest, UnmarshallingEmptyParcel, TestSize.Level1)
{
    Parcel parcel;
    auto result = RSRenderAtlasInfoAnimation::Unmarshalling(parcel);
    EXPECT_EQ(result, nullptr);
}
} // namespace Rosen
} // namespace OHOS
