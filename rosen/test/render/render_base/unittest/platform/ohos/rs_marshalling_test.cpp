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
#include "animation/rs_render_particle.h"
#include "animation/rs_render_transition.h"
#include "animation/rs_render_curve_animation.h"
#include "common/rs_common_def.h"
#include "common/rs_matrix3.h"
#include "common/rs_vector4.h"
#include "pipeline/rs_draw_cmd.h"
#include "platform/common/rs_log.h"
#include "render/rs_blur_filter.h"
#include "render/rs_filter.h"
#include "memory/rs_memory_track.h"
#include "animation/rs_render_keyframe_animation.h"
#include "animation/rs_render_particle_animation.h"
#include "animation/rs_render_path_animation.h"
#include "animation/rs_render_spring_animation.h"
#include "render/rs_light_up_effect_filter.h"
#include "render/rs_mask.h"
#include "animation/rs_render_interpolating_spring_animation.h"
#include "common/rs_color.h"
#include "modifier/rs_render_modifier.h"
#include "render/rs_gradient_blur_para.h"
#include "render/rs_material_filter.h"
#include "render/rs_motion_blur_filter.h"
#include "render/rs_image.h"
#include "render/rs_image_base.h"
#include "render/rs_path.h"
#include "render/rs_pixel_map_shader.h"
#include "render/rs_shader.h"
#include "transaction/rs_ashmem_helper.h"

#ifdef ROSEN_OHOS
#include "buffer_utils.h"
#endif
#include "recording/mask_cmd_list.h"
#include "property/rs_properties_def.h"


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
 * @tc.name: Marshalling001
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Marshalling001, TestSize.Level1)
{
    Parcel parcel1;
    std::shared_ptr<Drawing::Data> val1;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel1, val1));
    val1 = std::make_shared<Drawing::Data>();
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel1, val1));
    val1->BuildUninitialized(1);
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel1, val1));
}

/**
 * @tc.name: Unmarshalling001
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Unmarshalling001, TestSize.Level1)
{
    Parcel parcel1;
    std::shared_ptr<Drawing::Data> val1;
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel1, val1));
    parcel1.WriteInt32(-1);
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel1, val1));
    parcel1.WriteInt32(1);
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel1, val1));
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel1, val1));
}

/**
 * @tc.name: UnmarshallingWithCopyTest001
 * @tc.desc: Verify function UnmarshallingWithCopy
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingWithCopyTest001, TestSize.Level1)
{
    Parcel parcel1;
    auto val1 = std::make_shared<Drawing::Data>();
    EXPECT_TRUE(RSMarshallingHelper::UnmarshallingWithCopy(parcel1, val1));
    val1 = nullptr;
    EXPECT_TRUE(RSMarshallingHelper::UnmarshallingWithCopy(parcel1, val1));
}

/**
 * @tc.name: SkipDataTest001
 * @tc.desc: Verify function SkipData
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, SkipDataTest001, TestSize.Level1)
{
    Parcel parcel1;
    EXPECT_TRUE(RSMarshallingHelper::SkipData(parcel1));
    parcel1.WriteInt32(1);
    EXPECT_FALSE(RSMarshallingHelper::SkipData(parcel1));
}

/**
 * @tc.name: Marshalling002
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Marshalling002, TestSize.Level1)
{
    Parcel parcel2;
    Drawing::Bitmap val2;
    EXPECT_FALSE(RSMarshallingHelper::Marshalling(parcel2, val2));
}

/**
 * @tc.name: Unmarshalling002
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Unmarshalling002, TestSize.Level1)
{
    Parcel parcel2;
    Drawing::Bitmap val2;
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel2, val2));
}

/**
 * @tc.name: Marshalling003
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Marshalling003, TestSize.Level1)
{
    Parcel parcel3;
    std::shared_ptr<Drawing::Typeface> typeface3;
    EXPECT_FALSE(RSMarshallingHelper::Marshalling(parcel3, typeface3));
    typeface = Drawing::Typeface::MakeDefault();
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel3, typeface3));
}

/**
 * @tc.name: Unmarshalling003
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Unmarshalling003, TestSize.Level1)
{
    Parcel parcel3;
    std::shared_ptr<Drawing::Typeface> typeface3;
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel3, typeface3));
}

/**
 * @tc.name: Marshalling004
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Marshalling004, TestSize.Level1)
{
    Parcel parcel4;
    std::shared_ptr<Drawing::Image> val4;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, val4));
    val4 = std::make_shared<Drawing::Image>();
    EXPECT_FALSE(RSMarshallingHelper::Marshalling(parcel4, val4));
}

/**
 * @tc.name: ReadColorSpaceFromParcelTest001
 * @tc.desc: Verify function ReadColorSpaceFromParcel
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, ReadColorSpaceFromParcelTest001, TestSize.Level1)
{
    Parcel parcel1;
    auto colorSpace1 = std::make_shared<Drawing::ColorSpace>();
    EXPECT_TRUE(RSMarshallingHelper::ReadColorSpaceFromParcel(parcel1, colorSpace1));
    parcel1.WriteInt32(RSMarshallingHelper::MIN_DATA_SIZE);
    EXPECT_FALSE(RSMarshallingHelper::ReadColorSpaceFromParcel(parcel1, colorSpace1));
    parcel1.WriteInt32(1);
    EXPECT_FALSE(RSMarshallingHelper::ReadColorSpaceFromParcel(parcel1, colorSpace1));
}

/**
 * @tc.name: UnmarshallingNoLazyGeneratedImageTest001
 * @tc.desc: Verify function UnmarshallingNoLazyGeneratedImage
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, UnmarshallingNoLazyGeneratedImageTest001, TestSize.Level1)
{
    Parcel parcel1;
    void* imagepixelAddr1 = nullptr;
    auto val1 = std::make_shared<Drawing::Image>();
    EXPECT_FALSE(RSMarshallingHelper::UnmarshallingNoLazyGeneratedImage(parcel1, val1, imagepixelAddr1));
}

/**
 * @tc.name: Unmarshalling004
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Unmarshalling004, TestSize.Level1)
{
    Parcel parcel4;
    auto val4 = std::make_shared<Drawing::Image>();
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel4, val4));
}

/**
 * @tc.name: Unmarshalling005
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Unmarshalling005, TestSize.Level1)
{
    Parcel parcel5;
    void* imagepixelAddr5 = nullptr;
    std::shared_ptr<Drawing::Image> val5;
    parcel5.WriteInt32(-1);
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel5, val5, imagepixelAddr5));
    parcel5.WriteInt32(1);
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel5, val5, imagepixelAddr5));
}

/**
 * @tc.name: SkipImageTest001
 * @tc.desc: Verify function SkipImage
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, SkipImageTest001, TestSize.Level1)
{
    Parcel parcel1;
    parcel1.WriteInt32(-1);
    EXPECT_TRUE(RSMarshallingHelper::SkipImage(parcel1));
    parcel1.WriteInt32(1);
    EXPECT_TRUE(RSMarshallingHelper::SkipImage(parcel1));
    parcel1.WriteInt32(0);
    EXPECT_TRUE(RSMarshallingHelper::SkipImage(parcel1));
    parcel1.WriteInt32(RSMarshallingHelper::MIN_DATA_SIZE);
    EXPECT_TRUE(RSMarshallingHelper::SkipImage(parcel1));
}

/**
 * @tc.name: Marshalling005
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Marshalling005, TestSize.Level1)
{
    Parcel parcel5;
    std::shared_ptr<RSShader> val5;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel5, val5));
    val5 = RSShader::CreateRSShader();
    Drawing::Color color;
    auto drShader = Drawing::ShaderEffect::CreateColorShader(color.COLOR_TRANSPARENT);
    val5->SetDrawingShader(drShader);
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel5, val5));
}

/**
 * @tc.name: Unmarshalling006
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Unmarshalling006, TestSize.Level1)
{
    std::shared_ptr<RSShader> val6;
    Parcel parcel6;
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel6, val6));
    parcel6.WriteInt32(-1);
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel6, val6));
    parcel6.WriteInt32(1);
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel6, val6));
}

/**
 * @tc.name: MarshallingTest006
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, MarshallingTest006, TestSize.Level1)
{
    Drawing::Matrix val6;
    Parcel parcel6;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel6, val6));
}

/**
 * @tc.name: Unmarshalling007
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Unmarshalling007, TestSize.Level1)
{
    Drawing::Matrix val7;
    Parcel parcel7;
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel7, val7));
    parcel.WriteInt32(RSMarshallingHelper::MIN_DATA_SIZE);
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel7, val7));
}

/**
 * @tc.name: Marshalling007
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Marshalling007, TestSize.Level1)
{
    float blurRadius7 = 0.0f;
    Parcel parcel7;
    std::pair<float, float> fractionStop;
    std::vector<std::pair<float, float>> fractionStops;
    fractionStop.first = 0.0f;
    fractionStop.second = 0.0f;
    fractionStops.push_back(fractionStop);
    auto val = std::make_shared<RSLinearGradientBlurPara>(blurRadius7, fractionStops, GradientDirection::LEFT);
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel7, val));
}

/**
 * @tc.name: Unmarshalling008
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Unmarshalling008, TestSize.Level1)
{
    std::shared_ptr<RSLinearGradientBlurPara> val8;
    Parcel parcel8;
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel8, val8));
    parcel8.WriteInt32(1);
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel8, val8));
    parcel8.WriteInt32(RSMarshallingHelper::MIN_DATA_SIZE);
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel8, val8));
}

/**
 * @tc.name: Marshalling008
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Marshalling008, TestSize.Level1)
{
    uint32_t emitterIndex8 = 1;
    Parcel parcel8;
    auto val8 = std::make_shared<EmitterUpdater>(emitterIndex8);
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel8, val8));
}

/**
 * @tc.name: Unmarshalling009
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Unmarshalling009, TestSize.Level1)
{
    std::shared_ptr<EmitterUpdater> val9;
    Parcel parcel9;
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel9, val9));
}

/**
 * @tc.name: Marshalling009
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Marshalling009, TestSize.Level1)
{
    Parcel parcel9;
    uint32_t emitterIndex = 1;
    std::vector<std::shared_ptr<EmitterUpdater>> val9;
    auto emitterUpdater = std::make_shared<EmitterUpdater>(emitterIndex);
    val9.push_back(emitterUpdater);
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel9, val9));
}

/**
 * @tc.name: Unmarshalling010
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Unmarshalling010, TestSize.Level1)
{
    Parcel parcel10;
    std::vector<std::shared_ptr<EmitterUpdater>> val10;
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel10, val10));
    parcel10.WriteInt32(RSMarshallingHelper::MAX_DATA_SIZE);
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel10, val10));
}

/**
 * @tc.name: Marshalling010
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Marshalling010, TestSize.Level1)
{
    Parcel parcel10;
    Vector2f fieldCenter10;
    Vector2f fieldSize10;
    auto val =
        std::make_shared<ParticleNoiseField>(1, ShapeType::RECT, fieldSize10, fieldCenter10, 1, 1.0f, 1.0f, 1.0f);
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel10, val));
}

/**
 * @tc.name: Unmarshalling011
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Unmarshalling011, TestSize.Level1)
{
    std::shared_ptr<ParticleNoiseField> val11;
    Parcel parcel11;
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel11, val11));
}

/**
 * @tc.name: Marshalling011
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Marshalling011, TestSize.Level1)
{
    Vector2f anchor11;
    Parcel parcel11;
    auto val11 = std::make_shared<MotionBlurParam>(1.0f, anchor11);
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel11, val11));
}

/**
 * @tc.name: Unmarshalling012
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Unmarshalling012, TestSize.Level1)
{
    std::shared_ptr<MotionBlurParam> val12;
    Parcel parcel12;
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel, val12));
}

/**
 * @tc.name: Marshalling012
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Marshalling012, TestSize.Level1)
{
    Parcel parcel12;
    Vector2f fieldSize12;
    Vector2f fieldCenter;
    auto val = std::make_shared<ParticleNoiseFields>();
    auto field =
        std::make_shared<ParticleNoiseField>(1, ShapeType::RECT, fieldSize12, fieldCenter, 1, 1.0f, 1.0f, 1.0f);
    val->AddField(field);
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel12, val));
}

/**
 * @tc.name: Unmarshalling013
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Unmarshalling013, TestSize.Level1)
{
    std::shared_ptr<ParticleNoiseFields> val13;
    Parcel parcel13;
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel13, val13));
    parcel13.WriteInt32(RSMarshallingHelper::MAX_DATA_SIZE);
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel13, val13));
}

/**
 * @tc.name: Marshalling013
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Marshalling013, TestSize.Level1)
{
    EmitterConfig val13;
    Parcel parcel13;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel13, val13));
}

/**
 * @tc.name: Unmarshalling014
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Unmarshalling014, TestSize.Level1)
{
    EmitterConfig val14;
    Parcel parcel14;
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel14, val14));
}

/**
 * @tc.name: Marshalling014
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Marshalling014, TestSize.Level1)
{
    ParticleVelocity val14;
    Parcel parcel14;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel14, val14));
}

/**
 * @tc.name: Unmarshalling015
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Unmarshalling015, TestSize.Level1)
{
    ParticleVelocity val15;
    Parcel parcel15;
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel15, val15));
}

/**
 * @tc.name: Marshalling015
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Marshalling015, TestSize.Level1)
{
    Parcel parcel15;
    Range<float> random(0.0f, 0.0f);
    Range<float> value(0.0f, 0.0f);
    std::vector<std::shared_ptr<ChangeInOverLife<float>>> valChangeOverLife;
    parcel15.WriteInt16(1);
    parcel15.WriteUint64(1);
    std::shared_ptr<RSInterpolator> interpolator = RSInterpolator::Unmarshalling(parcel15);
    auto changeInOverLife = std::make_shared<ChangeInOverLife<float>>(0.f, 0.f, 0, 0, interpolator);
    valChangeOverLife.push_back(changeInOverLife);
    RenderParticleParaType<float> val1(value, ParticleUpdator::RANDOM, random, valChangeOverLife);
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel15, val1));
    RenderParticleParaType<float> val2(value, ParticleUpdator::CURVE, random, valChangeOverLife);
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel15, val2));
}

/**
 * @tc.name: Unmarshalling016
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Unmarshalling016, TestSize.Level1)
{
    RenderParticleParaType<float> val16;
    Parcel parcel16;
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel16, val16));
}

/**
 * @tc.name: Unmarshalling017
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Unmarshalling017, TestSize.Level1)
{
    RenderParticleColorParaType val17;
    Parcel parcel17;
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel17, val17));
}

/**
 * @tc.name: Marshalling017
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Marshalling017, TestSize.Level1)
{
    EmitterConfig emitterConfig;
    Parcel parcel17;
    ParticleVelocity velocity17;
    RenderParticleParaType<float> accelerationValue;
    RenderParticleParaType<float> accelerationAngle;
    RenderParticleAcceleration acceleration = RenderParticleAcceleration(accelerationValue, accelerationAngle);
    RenderParticleParaType<float> opacity;
    RenderParticleParaType<float> scale;
    RenderParticleParaType<float> spin;
    RenderParticleColorParaType color;
    auto val17 =
        std::make_shared<ParticleRenderParams>(emitterConfig, velocity17, acceleration, color, opacity, scale, spin);
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel17, val17));
}

/**
 * @tc.name: Unmarshalling018
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Unmarshalling018, TestSize.Level1)
{
    std::shared_ptr<ParticleRenderParams> val18;
    Parcel parcel18;
    EXPECT_FALSE(RSMarshallingHelper::Unmarshalling(parcel18, val18));
}

/**
 * @tc.name: Marshalling018
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9NIKQ
 */
HWTEST_F(RSMarshallingHelperTest, Marshalling018, TestSize.Level1)
{
    std::vector<std::shared_ptr<ParticleRenderParams>> val18;
    Parcel parcel18;
    auto particleRenderParams18 = std::make_shared<ParticleRenderParams>();
    val18.push_back(particleRenderParams18);
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel18, val18));
}
} // namespace Rosen
} // namespace OHOS