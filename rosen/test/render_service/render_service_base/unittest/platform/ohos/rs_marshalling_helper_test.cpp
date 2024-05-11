/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "animation/rs_particle_noise_field.h"
#include "animation/rs_render_curve_animation.h"
#include "animation/rs_render_interpolating_spring_animation.h"
#include "animation/rs_render_keyframe_animation.h"
#include "animation/rs_render_particle.h"
#include "animation/rs_render_particle_animation.h"
#include "animation/rs_render_path_animation.h"
#include "animation/rs_render_spring_animation.h"
#include "animation/rs_render_transition.h"
#include "common/rs_color.h"
#include "common/rs_common_def.h"
#include "common/rs_matrix3.h"
#include "common/rs_vector4.h"
#include "memory/rs_memory_track.h"
#include "modifier/rs_render_modifier.h"
#include "pipeline/rs_draw_cmd.h"
#include "platform/common/rs_log.h"
#include "render/rs_blur_filter.h"
#include "render/rs_filter.h"
#include "render/rs_gradient_blur_para.h"
#include "render/rs_image.h"
#include "render/rs_image_base.h"
#include "render/rs_light_up_effect_filter.h"
#include "render/rs_mask.h"
#include "render/rs_material_filter.h"
#include "render/rs_motion_blur_filter.h"
#include "render/rs_path.h"
#include "render/rs_pixel_map_shader.h"
#include "render/rs_shader.h"
#include "transaction/rs_ashmem_helper.h"

#ifdef ROSEN_OHOS
#include "buffer_utils.h"
#endif
#include "recording/mask_cmd_list.h"
#include "property/rs_properties_def.h"

#ifdef RS_ENABLE_RECORDING
#include "benchmarks/rs_recording_thread.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSMarshallingHelperTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMarshallingHelperTest::SetUpTestCase() {}
void RSMarshallingHelperTest::TearDownTestCase() {}
void RSMarshallingHelperTest::SetUp() {}
void RSMarshallingHelperTest::TearDown() {}

/**
 * @tc.name: MarshallingTest001
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest001, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<Drawing::Data> val;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
    val = std::make_shared<Drawing::Data>();
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
    val->BuildUninitialized(1);
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest001
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest001, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<Drawing::Data> val;
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, val));
    parcel.WriteInt32(-1);
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, val));
    parcel.WriteInt32(1);
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: SkipDataTest
 * @tc.desc: Verify function SkipData
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, SkipDataTest, TestSize.Level1)
{
    Parcel parcel;
    EXPECT_TRUE(RSMarshallingHelper::SkipData(parcel));
    parcel.WriteInt32(1);
    EXPECT_FALSE(RSMarshallingHelper::SkipData(parcel));
}

/**
 * @tc.name: UnmarshallingWithCopyTest
 * @tc.desc: Verify function UnmarshallingWithCopy
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingWithCopyTest, TestSize.Level1)
{
    Parcel parcel;
    auto val = std::make_shared<Drawing::Data>();
    EXPECT_TRUE(RSMarshallingHelper::UnmarshallingWithCopy(parcel, val));
}

/**
 * @tc.name: MarshallingTest002
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest002, TestSize.Level1)
{
    Parcel parcel;
    Drawing::Bitmap val;
    EXPECT_FALSE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest002
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest002, TestSize.Level1)
{
    Parcel parcel;
    Drawing::Bitmap val;
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: MarshallingTest003
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest003, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<Drawing::Typeface> typeface;
    EXPECT_FALSE(RSMarshallingHelper::Marshalling(parcel, typeface));
    typeface = Drawing::Typeface::MakeDefault();
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, typeface));
}

/**
 * @tc.name: UnmarshallingTest003
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest003, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<Drawing::Typeface> typeface;
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, typeface));
}

/**
 * @tc.name: MarshallingTest004
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest004, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<Drawing::Image> val;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
    val = std::make_shared<Drawing::Image>();
    EXPECT_FALSE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: ReadColorSpaceFromParcelTest
 * @tc.desc: Verify function ReadColorSpaceFromParcel
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, ReadColorSpaceFromParcelTest, TestSize.Level1)
{
    Parcel parcel;
    auto colorSpace = std::make_shared<Drawing::ColorSpace>();
    EXPECT_TRUE(RSMarshallingHelper::ReadColorSpaceFromParcel(parcel, colorSpace));
    parcel.WriteInt32(RSMarshallingHelper::MIN_DATA_SIZE);
    EXPECT_FALSE(RSMarshallingHelper::ReadColorSpaceFromParcel(parcel, colorSpace));
    parcel.WriteInt32(1);
    EXPECT_FALSE(RSMarshallingHelper::ReadColorSpaceFromParcel(parcel, colorSpace));
}

/**
 * @tc.name: UnmarshallingNoLazyGeneratedImageTest
 * @tc.desc: Verify function UnmarshallingNoLazyGeneratedImage
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingNoLazyGeneratedImageTest, TestSize.Level1)
{
    Parcel parcel;
    auto val = std::make_shared<Drawing::Image>();
    void* imagepixelAddr = nullptr;
    EXPECT_FALSE(RSMarshallingHelper::UnmarshallingNoLazyGeneratedImage(parcel, val, imagepixelAddr));
}

/**
 * @tc.name: UnmarshallingTest004
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest004, TestSize.Level1)
{
    Parcel parcel;
    auto val = std::make_shared<Drawing::Image>();
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest005
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest005, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<Drawing::Image> val;
    void* imagepixelAddr = nullptr;
    parcel.WriteInt32(-1);
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, val, imagepixelAddr));
    parcel.WriteInt32(1);
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val, imagepixelAddr));
}

/**
 * @tc.name: SkipImageTest
 * @tc.desc: Verify function SkipImage
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, SkipImageTest, TestSize.Level1)
{
    Parcel parcel;
    parcel.WriteInt32(-1);
    EXPECT_TRUE(RSMarshallingHelper::SkipImage(parcel));
    parcel.WriteInt32(1);
    EXPECT_TRUE(RSMarshallingHelper::SkipImage(parcel));
    parcel.WriteInt32(0);
    EXPECT_TRUE(RSMarshallingHelper::SkipImage(parcel));
    parcel.WriteInt32(RSMarshallingHelper::MIN_DATA_SIZE);
    EXPECT_TRUE(RSMarshallingHelper::SkipImage(parcel));
}

/**
 * @tc.name: MarshallingTest005
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest005, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<RSShader> val;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
    val = RSShader::CreateRSShader();
    Drawing::Color color;
    auto drShader = Drawing::ShaderEffect::CreateColorShader(color.COLOR_TRANSPARENT);
    val->SetDrawingShader(drShader);
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest006
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest006, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<RSShader> val;
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, val));
    parcel.WriteInt32(-1);
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, val));
    parcel.WriteInt32(1);
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: MarshallingTest006
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest006, TestSize.Level1)
{
    Parcel parcel;
    Drawing::Matrix val;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest007
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest007, TestSize.Level1)
{
    Parcel parcel;
    Drawing::Matrix val;
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
    parcel.WriteInt32(RSMarshallingHelper::MIN_DATA_SIZE);
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: MarshallingTest007
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest007, TestSize.Level1)
{
    Parcel parcel;
    float blurRadius = 0.0f;
    std::vector<std::pair<float, float>> fractionStops;
    std::pair<float, float> fractionStop;
    fractionStop.first = 0.0f;
    fractionStop.second = 0.0f;
    fractionStops.push_back(fractionStop);
    auto val = std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, GradientDirection::LEFT);
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest008
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest008, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<RSLinearGradientBlurPara> val;
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
    parcel.WriteInt32(1);
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
    parcel.WriteInt32(RSMarshallingHelper::MIN_DATA_SIZE);
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: MarshallingTest008
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest008, TestSize.Level1)
{
    Parcel parcel;
    uint32_t emitterIndex = 1;
    auto val = std::make_shared<EmitterUpdater>(emitterIndex);
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest009
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest009, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<EmitterUpdater> val;
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: MarshallingTest009
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest009, TestSize.Level1)
{
    Parcel parcel;
    std::vector<std::shared_ptr<EmitterUpdater>> val;
    uint32_t emitterIndex = 1;
    auto emitterUpdater = std::make_shared<EmitterUpdater>(emitterIndex);
    val.push_back(emitterUpdater);
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest010
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest010, TestSize.Level1)
{
    Parcel parcel;
    std::vector<std::shared_ptr<EmitterUpdater>> val;
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, val));
    parcel.WriteInt32(RSMarshallingHelper::MAX_DATA_SIZE);
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: MarshallingTest010
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest010, TestSize.Level1)
{
    Parcel parcel;
    Vector2f fieldSize;
    Vector2f fieldCenter;
    auto val = std::make_shared<ParticleNoiseField>(1, ShapeType::RECT, fieldSize, fieldCenter, 1, 1.0f, 1.0f, 1.0f);
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest011
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest011, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<ParticleNoiseField> val;
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: MarshallingTest011
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest011, TestSize.Level1)
{
    Parcel parcel;
    Vector2f anchor;
    auto val = std::make_shared<MotionBlurParam>(1.0f, anchor);
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest012
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest012, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<MotionBlurParam> val;
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: MarshallingTest012
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest012, TestSize.Level1)
{
    Parcel parcel;
    auto val = std::make_shared<ParticleNoiseFields>();
    Vector2f fieldSize;
    Vector2f fieldCenter;
    auto field = std::make_shared<ParticleNoiseField>(1, ShapeType::RECT, fieldSize, fieldCenter, 1, 1.0f, 1.0f, 1.0f);
    val->AddField(field);
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest013
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest013, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<ParticleNoiseFields> val;
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, val));
    parcel.WriteInt32(RSMarshallingHelper::MAX_DATA_SIZE);
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: MarshallingTest013
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest013, TestSize.Level1)
{
    Parcel parcel;
    EmitterConfig val;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest014
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest014, TestSize.Level1)
{
    Parcel parcel;
    EmitterConfig val;
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: MarshallingTest014
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest014, TestSize.Level1)
{
    Parcel parcel;
    ParticleVelocity val;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest015
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest015, TestSize.Level1)
{
    Parcel parcel;
    ParticleVelocity val;
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: MarshallingTest015
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest015, TestSize.Level1)
{
    Parcel parcel;
    Range<float> value(0.0f, 0.0f);
    Range<float> random(0.0f, 0.0f);
    std::vector<std::shared_ptr<ChangeInOverLife<float>>> valChangeOverLife;
    parcel.WriteInt16(1);
    parcel.WriteUint64(1);
    std::shared_ptr<RSInterpolator> interpolator = RSInterpolator::Unmarshalling(parcel);
    auto changeInOverLife = std::make_shared<ChangeInOverLife<float>>(0.f, 0.f, 0, 0, interpolator);
    valChangeOverLife.push_back(changeInOverLife);
    RenderParticleParaType<float> val(value, ParticleUpdator::RANDOM, random, std::move(valChangeOverLife));
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
    RenderParticleParaType<float> val2(value, ParticleUpdator::CURVE, random, std::move(valChangeOverLife));
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val2));
}

/**
 * @tc.name: UnmarshallingTest016
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest016, TestSize.Level1)
{
    Parcel parcel;
    RenderParticleParaType<float> val;
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: MarshallingTest016
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest016, TestSize.Level1)
{
    Parcel parcel;
    Range<Color> colorVal(RSColor(0, 0, 0), RSColor(0, 0, 0));
    Range<float> redRandom(0.f, 0.f);
    Range<float> greenRandom(0.f, 0.f);
    Range<float> blueRandom(0.f, 0.f);
    Range<float> alphaRandom(0.f, 0.f);
    std::vector<std::shared_ptr<ChangeInOverLife<Color>>> valChangeOverLife;
    parcel.WriteInt16(1);
    parcel.WriteUint64(1);
    std::shared_ptr<RSInterpolator> interpolator = RSInterpolator::Unmarshalling(parcel);
    auto changeInOverLife =
        std::make_shared<ChangeInOverLife<Color>>(RSColor(0, 0, 0), RSColor(0, 0, 0), 0, 0, interpolator);
    valChangeOverLife.push_back(changeInOverLife);
    RenderParticleColorParaType val(colorVal, DistributionType::UNIFORM, ParticleUpdator::RANDOM, redRandom,
        greenRandom, blueRandom, alphaRandom, valChangeOverLife);
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
    RenderParticleColorParaType val2(colorVal, DistributionType::UNIFORM, ParticleUpdator::CURVE, redRandom,
        greenRandom, blueRandom, alphaRandom, valChangeOverLife);
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val2));
}

/**
 * @tc.name: UnmarshallingTest017
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest017, TestSize.Level1)
{
    Parcel parcel;
    RenderParticleColorParaType val;
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: MarshallingTest017
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest017, TestSize.Level1)
{
    Parcel parcel;
    EmitterConfig emitterConfig;
    ParticleVelocity velocity;
    RenderParticleParaType<float> accelerationValue;
    RenderParticleParaType<float> accelerationAngle;
    RenderParticleAcceleration acceleration = RenderParticleAcceleration(accelerationValue, accelerationAngle);
    RenderParticleColorParaType color;
    RenderParticleParaType<float> opacity;
    RenderParticleParaType<float> scale;
    RenderParticleParaType<float> spin;
    auto val =
        std::make_shared<ParticleRenderParams>(emitterConfig, velocity, acceleration, color, opacity, scale, spin);
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest018
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest018, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<ParticleRenderParams> val;
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: MarshallingTest018
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest018, TestSize.Level1)
{
    Parcel parcel;
    std::vector<std::shared_ptr<ParticleRenderParams>> val;
    auto particleRenderParams = std::make_shared<ParticleRenderParams>();
    val.push_back(particleRenderParams);
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest019
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest019, TestSize.Level1)
{
    Parcel parcel;
    std::vector<std::shared_ptr<ParticleRenderParams>> val;
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, val));
    parcel.WriteUint32(RSMarshallingHelper::MAX_DATA_SIZE);
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: MarshallingTest019
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest019, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<RSPath> val;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
    Drawing::Path path;
    val = RSPath::CreateRSPath(path);
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest020
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest020, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<RSPath> val;
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, val));
    parcel.WriteInt32(1);
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, val));
    parcel.WriteInt32(-1);
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: MarshallingTest020
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest020, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<RSMask> val;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
    Drawing::Brush maskBrush;
    val = RSMask::CreateGradientMask(maskBrush);
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest021
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest021, TestSize.Level1)
{
    Parcel parcel;
    Drawing::Brush maskBrush;
    std::shared_ptr<RSMask> val = RSMask::CreateGradientMask(maskBrush);
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
    parcel.WriteInt32(-1);
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: MarshallingTest021
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest021, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<RSFilter> val;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
    val = RSFilter::CreateBlurFilter(1.0f, 1.0f);
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
    val->type_ = RSFilter::BLUR;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
    val->type_ = RSFilter::MATERIAL;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
    val->type_ = RSFilter::LIGHT_UP_EFFECT;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest022
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest022, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<RSFilter> val;
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: MarshallingTest022
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest022, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<RSImageBase> val;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
    val = std::make_shared<RSImageBase>();
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest023
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest023, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<RSImageBase> val = std::make_shared<RSImageBase>();
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
    parcel.WriteInt32(-1);
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: MarshallingTest023
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest023, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<RSImage> val;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
    val = std::make_shared<RSImage>();
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest024
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest024, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<RSImage> val = std::make_shared<RSImage>();
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
    parcel.WriteInt32(-1);
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: MarshallingTest024
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest024, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<Media::PixelMap> val;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
    val = std::make_shared<Media::PixelMap>();
    EXPECT_FALSE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest025
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest025, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<Media::PixelMap> val = std::make_shared<Media::PixelMap>();
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
    parcel.WriteInt32(-1);
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: SkipPixelMapTest
 * @tc.desc: Verify function SkipPixelMap
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, SkipPixelMapTest, TestSize.Level1)
{
    Parcel parcel;
    EXPECT_TRUE(RSMarshallingHelper::SkipPixelMap(parcel));
    parcel.WriteInt32(-1);
    EXPECT_TRUE(RSMarshallingHelper::SkipPixelMap(parcel));
}

/**
 * @tc.name: MarshallingTest025
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest025, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<RectT<float>> val;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
    val = std::make_shared<RectT<float>>();
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest026
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest026, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<RectT<float>> val = std::make_shared<RectT<float>>();
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
    parcel.WriteInt32(-1);
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: MarshallingTest026
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest026, TestSize.Level1)
{
    Parcel parcel;
    RRectT<float> val;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest027
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest027, TestSize.Level1)
{
    Parcel parcel;
    RRectT<float> val;
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: MarshallingTest027
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest027, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<Drawing::DrawCmdList> val;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
    val = std::make_shared<Drawing::DrawCmdList>(1, 1);
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
    size_t size = 0;
    val->opAllocator_.size_ = size;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest028
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest028, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<Drawing::DrawCmdList> val;
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, val));
    parcel.WriteInt32(-1);
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, val));
    parcel.WriteInt32(1);
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: MarshallingTest028
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest028, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<RSExtendImageObject> val;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
    val = std::make_shared<RSExtendImageObject>();
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest029
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest029, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<RSExtendImageObject> val = std::make_shared<RSExtendImageObject>();
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
    parcel.WriteInt32(-1);
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: MarshallingTest029
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest029, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<RSExtendImageBaseObj> val;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
    val = std::make_shared<RSExtendImageBaseObj>();
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest030
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest030, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<RSExtendImageBaseObj> val = std::make_shared<RSExtendImageBaseObj>();
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
    parcel.WriteInt32(-1);
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: MarshallingTest030
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest030, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<Drawing::MaskCmdList> val;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
    std::vector<uint8_t> testData = { 0x01, 0x01, 0x01, 0x01 };
    Drawing::CmdListData data(testData.data(), testData.size());
    val = Drawing::MaskCmdList::CreateFromData(data, true);
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
    size_t size = 0;
    val->opAllocator_.size_ = size;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest031
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest031, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<Drawing::MaskCmdList> val;
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, val));
    parcel.WriteInt32(-1);
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, val));
    parcel.WriteInt32(1);
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: MarshallingTest031
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest031, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<RSRenderModifier> val;
    EXPECT_FALSE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest032
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest032, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<RSRenderModifier> val;
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: WriteToParcelTest
 * @tc.desc: Verify function WriteToParcel
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, WriteToParcelTest, TestSize.Level1)
{
    Parcel parcel;
    EXPECT_FALSE(RSMarshallingHelper::WriteToParcel(parcel, nullptr, 1));
    int data = 1;
    EXPECT_TRUE(RSMarshallingHelper::WriteToParcel(parcel, &data, 1));
}

/**
 * @tc.name: ReadFromParcelTest
 * @tc.desc: Verify function ReadFromParcel
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, ReadFromParcelTest, TestSize.Level1)
{
    Parcel parcel;
    bool isMalloc = false;
    EXPECT_FALSE(RSMarshallingHelper::ReadFromParcel(parcel, 1, isMalloc));
    EXPECT_FALSE(RSMarshallingHelper::ReadFromParcel(parcel, 0, isMalloc));
    parcel.WriteUint32(RSMarshallingHelper::MAX_DATA_SIZE);
    EXPECT_FALSE(RSMarshallingHelper::ReadFromParcel(parcel, 0, isMalloc));
}

/**
 * @tc.name: SkipFromParcelTest
 * @tc.desc: Verify function SkipFromParcel
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, SkipFromParcelTest, TestSize.Level1)
{
    Parcel parcel;
    EXPECT_TRUE(RSMarshallingHelper::SkipFromParcel(parcel, 0));
    parcel.WriteUint32(1);
    EXPECT_FALSE(RSMarshallingHelper::SkipFromParcel(parcel, 0));
    parcel.WriteUint32(RSMarshallingHelper::MAX_DATA_SIZE);
    EXPECT_FALSE(RSMarshallingHelper::SkipFromParcel(parcel, 0));
}

/**
 * @tc.name: SharedMemTest
 * @tc.desc: Verify function BeginNoSharedMem EndNoSharedMem GetUseSharedMem
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, SharedMemTest, TestSize.Level1)
{
    std::thread::id tid = 1;
    RSMarshallingHelper::BeginNoSharedMem(tid);
    RSMarshallingHelper::EndNoSharedMem();
    EXPECT_TRUE(RSMarshallingHelper::GetUseSharedMem(tid));
    RSMarshallingHelper::BeginNoSharedMem(tid);
    EXPECT_FALSE(RSMarshallingHelper::GetUseSharedMem(tid));
}

/**
 * @tc.name: MarshallingTest032
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest032, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> val;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest033
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest033, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<RSRenderPropertyBase> val;
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: MarshallingTest033
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest033, TestSize.Level1)
{
    Parcel parcel;
    int val = 0;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest034
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest034, TestSize.Level1)
{
    Parcel parcel;
    int val = 0;
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: MarshallingTest034
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest034, TestSize.Level1)
{
    Parcel parcel;
    std::string val = "0";
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest035
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest035, TestSize.Level1)
{
    Parcel parcel;
    std::string val = "0";
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: MarshallingArrayTest
 * @tc.desc: Verify function MarshallingArray
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingArrayTest, TestSize.Level1)
{
    Parcel parcel;
    int val = 0;
    EXPECT_TRUE(RSMarshallingHelper::MarshallingArray(parcel, &val, 0));
    EXPECT_TRUE(RSMarshallingHelper::MarshallingArray(parcel, &val, 1));
}

/**
 * @tc.name: UnmarshallingArrayTest
 * @tc.desc: Verify function UnmarshallingArray
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingArrayTest, TestSize.Level1)
{
    Parcel parcel;
    const int* val = nullptr;
    EXPECT_FALSE(RSMarshallingHelper::UnmarshallingArray(parcel, val, 0));
    EXPECT_FALSE(RSMarshallingHelper::UnmarshallingArray(parcel, val, 1));
}

/**
 * @tc.name: MarshallingVecTest
 * @tc.desc: Verify function MarshallingVec
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingVecTest, TestSize.Level1)
{
    Parcel parcel;
    std::vector<int> val;
    EXPECT_TRUE(RSMarshallingHelper::MarshallingVec(parcel, val));
    val.emplace_back(1);
    EXPECT_TRUE(RSMarshallingHelper::MarshallingVec(parcel, val));
}

/**
 * @tc.name: UnmarshallingVecTest
 * @tc.desc: Verify function UnmarshallingVec
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingVecTest, TestSize.Level1)
{
    Parcel parcel;
    std::vector<int> val;
    EXPECT_TRUE(RSMarshallingHelper::UnmarshallingVec(parcel, val));
}

/**
 * @tc.name: MarshallingVec2Test
 * @tc.desc: Verify function MarshallingVec2
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingVec2Test, TestSize.Level1)
{
    Parcel parcel;
    std::vector<std::vector<int>> val;
    EXPECT_TRUE(RSMarshallingHelper::MarshallingVec2(parcel, val));
    std::vector<int> emptyVector;
    val.emplace_back(emptyVector);
    EXPECT_TRUE(RSMarshallingHelper::MarshallingVec2(parcel, val));
}

/**
 * @tc.name: UnmarshallingVec2Test
 * @tc.desc: Verify function UnmarshallingVec2
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingVec2Test, TestSize.Level1)
{
    Parcel parcel;
    std::vector<std::vector<int>> val;
    EXPECT_TRUE(RSMarshallingHelper::UnmarshallingVec2(parcel, val));
}

/**
 * @tc.name: MarshallingTest035
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest035, TestSize.Level1)
{
    Parcel parcel;
    std::map<int, std::string> val;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
    val[0] = "0";
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest036
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest036, TestSize.Level1)
{
    Parcel parcel;
    std::map<int, std::string> val;
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: MarshallingTest036
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest036, TestSize.Level1)
{
    Parcel parcel;
    std::vector<int> val;
    val.emplace_back(1);
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest037
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest037, TestSize.Level1)
{
    Parcel parcel;
    std::vector<int> val;
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: MarshallingTest037
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest037, TestSize.Level1)
{
    Parcel parcel;
    std::optional<int> val;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
    val = 1;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest038
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest038, TestSize.Level1)
{
    Parcel parcel;
    std::optional<int> val;
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, val));
    parcel.WriteBool(true);
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: MarshallingTest038
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest038, TestSize.Level1)
{
    Parcel parcel;
    std::pair<int, int> val = { 1, 2 };
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val));
}

/**
 * @tc.name: UnmarshallingTest039
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest039, TestSize.Level1)
{
    Parcel parcel;
    std::pair<int, int> val = { 1, 2 };
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val));
}

/**
 * @tc.name: MarshallingTest039
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest039, TestSize.Level1)
{
    Parcel parcel;
    int first = 1;
    std::string args = "1";
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, first, args));
}

/**
 * @tc.name: UnmarshallingTest040
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9O78C
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingTest040, TestSize.Level1)
{
    Parcel parcel;
    int first = 1;
    std::string args = "1";
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, first, args));
}
} // namespace Rosen
} // namespace OHOS