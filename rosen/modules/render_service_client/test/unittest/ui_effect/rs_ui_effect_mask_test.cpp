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

#include <gtest/gtest.h>

#include "mask/include/atlas_frame_mask_para.h"
#include "mask/include/binocular_mask_para.h"
#include "mask/include/fractal_glass_mask_para.h"
#include "mask/include/image_mask_para.h"
#include "mask/include/mask.h"
#include "mask/include/mask_para.h"
#include "mask/include/mask_unmarshalling_singleton.h"
#include "mask/include/pixel_map_mask_para.h"
#include "mask/include/sweep_refraction_mask_para.h"
#include "mask/include/radial_gradient_mask_para.h"
#include "mask/include/use_effect_mask_para.h"
#include "mask/include/warped_ring_mask_para.h"
#include "pixel_map.h"

#include "draw/surface.h"
#include "image/bitmap.h"

namespace OHOS {
namespace Rosen {

using namespace testing;
using namespace testing::ext;

class RSUIEffectMaskTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUIEffectMaskTest::SetUpTestCase() {}
void RSUIEffectMaskTest::TearDownTestCase() {}

void RSUIEffectMaskTest::SetUp()
{
    Mask::RegisterUnmarshallingCallback();
}

void RSUIEffectMaskTest::TearDown() {}

/**
 * @tc.name: RSUIEffectMaskRegisterUnmarshallingCallbackTest
 * @tc.desc: Verify the Mask::RegisterUnmarshallingCallback static func
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectMaskTest, RSUIEffectMaskRegisterUnmarshallingCallbackTest, TestSize.Level1)
{
    auto& singleton = MaskUnmarshallingSingleton::GetInstance();
    EXPECT_NE(nullptr, singleton.GetCallback(static_cast<uint16_t>(MaskPara::Type::PIXEL_MAP_MASK)));
    EXPECT_NE(nullptr, singleton.GetCallback(static_cast<uint16_t>(MaskPara::Type::RADIAL_GRADIENT_MASK)));
    EXPECT_NE(nullptr, singleton.GetCallback(static_cast<uint16_t>(MaskPara::Type::IMAGE_MASK)));
}

static std::shared_ptr<Media::PixelMap> CreatePixelMap(int width, int height)
{
    Media::InitializationOptions opts;
    opts.size.width = width;
    opts.size.height = height;
    auto pixelMap = Media::PixelMap::Create(opts);
    if (pixelMap == nullptr) {
        return nullptr;
    }
    auto address = const_cast<uint32_t*>(pixelMap->GetPixel32(0, 0));
    if (address == nullptr) {
        return nullptr;
    }
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bitmap.Build(pixelMap->GetWidth(), pixelMap->GetHeight(), format);
    auto surface = std::make_shared<Drawing::Surface>();
    surface->Bind(bitmap);
    auto canvas = surface->GetCanvas();
    if (canvas == nullptr) {
        return nullptr;
    }
    canvas->Clear(Drawing::Color::COLOR_YELLOW);
    return pixelMap;
}

/**
 * @tc.name: RSUIEffectPixelMapMaskParaTest
 * @tc.desc: Verify the PixelMapMaskPara func
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectMaskTest, RSUIEffectPixelMapMaskParaTest, TestSize.Level1)
{
    auto pixelMapMaskPara = std::make_shared<PixelMapMaskPara>();
    auto pixelMap = CreatePixelMap(50, 50);
    EXPECT_NE(nullptr, pixelMap);
    pixelMapMaskPara->SetPixelMap(pixelMap);
    Vector4f src { 0.1f, 0.2f, 0.3f, 0.4f };
    pixelMapMaskPara->SetSrc(src);
    Vector4f dst { 0.5f, 0.6f, 0.7f, 0.8f };
    pixelMapMaskPara->SetDst(dst);
    Vector4f fillColor { 0.9f, 1.0f, 1.1f, 1.2f };
    pixelMapMaskPara->SetFillColor(fillColor);

    Parcel parcel;
    EXPECT_EQ(true, pixelMapMaskPara->Marshalling(parcel));
    std::shared_ptr<MaskPara> val = nullptr;
    EXPECT_EQ(true, MaskPara::Unmarshalling(parcel, val));
    EXPECT_NE(nullptr, val);
    auto clonePara = pixelMapMaskPara->Clone();
    EXPECT_NE(nullptr, clonePara);

    // test create
    pixelMapMaskPara = std::make_shared<PixelMapMaskPara>();
    EXPECT_NE(pixelMapMaskPara, nullptr);
    auto pixelMapMaskPara1 = std::make_shared<PixelMapMaskPara>(*pixelMapMaskPara);

    // test OnUnmarshalling
    std::shared_ptr<MaskPara> valTest = nullptr;
    Parcel parcelTest;
    EXPECT_EQ(false, PixelMapMaskPara::OnUnmarshalling(parcelTest, valTest));

    parcelTest.FlushBuffer();
    parcelTest.WriteUint16(666);
    EXPECT_EQ(false, PixelMapMaskPara::OnUnmarshalling(parcelTest, valTest));

    parcelTest.FlushBuffer();
    parcelTest.WriteUint16(static_cast<uint16_t>(MaskPara::Type::PIXEL_MAP_MASK));
    EXPECT_EQ(false, PixelMapMaskPara::OnUnmarshalling(parcelTest, valTest));
}

/**
 * @tc.name: RSUIEffectRadialGradientMaskParaTest
 * @tc.desc: Verify the RadialGradientMaskPara func
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectMaskTest, RSUIEffectRadialGradientMaskParaTest, TestSize.Level1)
{
    auto radialGradientMaskPara = std::make_shared<RadialGradientMaskPara>();
    Vector2f center { 0.1f, 0.2f };
    radialGradientMaskPara->SetCenter(center);
    float radiusX = 0.3f;
    radialGradientMaskPara->SetRadiusX(radiusX);
    float radiusY = 0.3f;
    radialGradientMaskPara->SetRadiusY(radiusY);
    std::vector<float> colors { 0.1f, 0.2f, 0.3f };
    radialGradientMaskPara->SetColors(colors);
    std::vector<float> positions { 0.4f, 0.5f, 0.6f };
    radialGradientMaskPara->SetPositions(positions);

    Parcel parcel;
    EXPECT_EQ(true, radialGradientMaskPara->Marshalling(parcel));
    std::shared_ptr<MaskPara> val = nullptr;
    EXPECT_EQ(true, MaskPara::Unmarshalling(parcel, val));
    EXPECT_NE(nullptr, val);
    EXPECT_NE(nullptr, radialGradientMaskPara->Clone());
}

/**
 * @tc.name: RSUIEffectRadialGradientMaskParaAbnormalTest001
 * @tc.desc: Verify the RadialGradientMaskPara func
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectMaskTest, RSUIEffectRadialGradientMaskParaAbnormalTest001, TestSize.Level1)
{
    Parcel parcelTest;
    // test Marshalling
    auto radialGradientMaskPara = std::make_shared<RadialGradientMaskPara>();
    std::vector<float> colors;
    for (int i = 0; i < (Rosen::MaskPara::UNMARSHALLING_MAX_VECTOR_SIZE + 1); i++) {
        colors.push_back(0.1f);
    }
    radialGradientMaskPara->SetColors(colors);
    EXPECT_EQ(false, radialGradientMaskPara->Marshalling(parcelTest));

    radialGradientMaskPara = std::make_shared<RadialGradientMaskPara>();
    std::vector<float> positions;
    for (int i = 0; i < (Rosen::MaskPara::UNMARSHALLING_MAX_VECTOR_SIZE + 1); i++) {
        positions.push_back(0.1f);
    }
    radialGradientMaskPara->SetPositions(positions);
    EXPECT_EQ(false, radialGradientMaskPara->Marshalling(parcelTest));
}

/**
 * @tc.name: RSUIEffectRadialGradientMaskParaAbnormalTest002
 * @tc.desc: Verify the RadialGradientMaskPara func
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectMaskTest, RSUIEffectRadialGradientMaskParaAbnormalTest002, TestSize.Level1)
{
    Parcel parcelTest;
    // test OnUnmarshalling
    std::shared_ptr<MaskPara> valTest = nullptr;
    parcelTest.FlushBuffer();
    EXPECT_EQ(false, RadialGradientMaskPara::OnUnmarshalling(parcelTest, valTest));

    parcelTest.FlushBuffer();
    parcelTest.WriteUint16(666);
    EXPECT_EQ(false, RadialGradientMaskPara::OnUnmarshalling(parcelTest, valTest));

    parcelTest.FlushBuffer();
    parcelTest.WriteUint16(static_cast<uint16_t>(MaskPara::Type::RADIAL_GRADIENT_MASK));
    EXPECT_EQ(false, RadialGradientMaskPara::OnUnmarshalling(parcelTest, valTest));

    parcelTest.FlushBuffer();
    parcelTest.WriteUint16(static_cast<uint16_t>(MaskPara::Type::RADIAL_GRADIENT_MASK));
    parcelTest.WriteFloat(0.1f);
    EXPECT_EQ(false, RadialGradientMaskPara::OnUnmarshalling(parcelTest, valTest));

    parcelTest.FlushBuffer();
    parcelTest.WriteUint16(static_cast<uint16_t>(MaskPara::Type::RADIAL_GRADIENT_MASK));
    parcelTest.WriteFloat(0.1f);
    parcelTest.WriteFloat(0.1f);
    EXPECT_EQ(false, RadialGradientMaskPara::OnUnmarshalling(parcelTest, valTest));

    parcelTest.FlushBuffer();
    parcelTest.WriteUint16(static_cast<uint16_t>(MaskPara::Type::RADIAL_GRADIENT_MASK));
    parcelTest.WriteFloat(0.1f);
    parcelTest.WriteFloat(0.1f);
    parcelTest.WriteFloat(0.1f);
    EXPECT_EQ(false, RadialGradientMaskPara::OnUnmarshalling(parcelTest, valTest));

    parcelTest.FlushBuffer();
    parcelTest.WriteUint16(static_cast<uint16_t>(MaskPara::Type::RADIAL_GRADIENT_MASK));
    parcelTest.WriteFloat(0.1f);
    parcelTest.WriteFloat(0.1f);
    parcelTest.WriteFloat(0.1f);
    parcelTest.WriteFloat(0.1f);
    EXPECT_EQ(false, RadialGradientMaskPara::OnUnmarshalling(parcelTest, valTest));
}

/**
 * @tc.name: RSUIEffectRadialGradientMaskParaAbnormalTest003
 * @tc.desc: Verify the RadialGradientMaskPara func
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectMaskTest, RSUIEffectRadialGradientMaskParaAbnormalTest003, TestSize.Level1)
{
    Parcel parcelTest;
    // test OnUnmarshalling
    std::shared_ptr<MaskPara> valTest = nullptr;
    parcelTest.FlushBuffer();
    parcelTest.WriteUint16(static_cast<uint16_t>(MaskPara::Type::RADIAL_GRADIENT_MASK));
    parcelTest.WriteFloat(0.1f);
    parcelTest.WriteFloat(0.1f);
    parcelTest.WriteFloat(0.1f);
    parcelTest.WriteFloat(0.1f);
    parcelTest.WriteUint32(MaskPara::UNMARSHALLING_MAX_VECTOR_SIZE + 1);
    EXPECT_EQ(false, RadialGradientMaskPara::OnUnmarshalling(parcelTest, valTest));

    parcelTest.FlushBuffer();
    parcelTest.WriteUint16(static_cast<uint16_t>(MaskPara::Type::RADIAL_GRADIENT_MASK));
    parcelTest.WriteFloat(0.1f);
    parcelTest.WriteFloat(0.1f);
    parcelTest.WriteFloat(0.1f);
    parcelTest.WriteFloat(0.1f);
    parcelTest.WriteUint32(1);
    EXPECT_EQ(false, RadialGradientMaskPara::OnUnmarshalling(parcelTest, valTest));

    parcelTest.FlushBuffer();
    parcelTest.WriteUint16(static_cast<uint16_t>(MaskPara::Type::RADIAL_GRADIENT_MASK));
    parcelTest.WriteFloat(0.1f);
    parcelTest.WriteFloat(0.1f);
    parcelTest.WriteFloat(0.1f);
    parcelTest.WriteFloat(0.1f);
    parcelTest.WriteUint32(0);
    EXPECT_EQ(false, RadialGradientMaskPara::OnUnmarshalling(parcelTest, valTest));

    parcelTest.FlushBuffer();
    parcelTest.WriteUint16(static_cast<uint16_t>(MaskPara::Type::RADIAL_GRADIENT_MASK));
    parcelTest.WriteFloat(0.1f);
    parcelTest.WriteFloat(0.1f);
    parcelTest.WriteFloat(0.1f);
    parcelTest.WriteFloat(0.1f);
    parcelTest.WriteUint32(0);
    parcelTest.WriteUint32(MaskPara::UNMARSHALLING_MAX_VECTOR_SIZE + 1);
    EXPECT_EQ(false, RadialGradientMaskPara::OnUnmarshalling(parcelTest, valTest));

    parcelTest.FlushBuffer();
    parcelTest.WriteUint16(static_cast<uint16_t>(MaskPara::Type::RADIAL_GRADIENT_MASK));
    parcelTest.WriteFloat(0.1f);
    parcelTest.WriteFloat(0.1f);
    parcelTest.WriteFloat(0.1f);
    parcelTest.WriteFloat(0.1f);
    parcelTest.WriteUint32(0);
    parcelTest.WriteUint32(1);
    EXPECT_EQ(false, RadialGradientMaskPara::OnUnmarshalling(parcelTest, valTest));
}

/**
 * @tc.name: RSUIEffectMaskUnmarshallingSingletonTest
 * @tc.desc: Verify the MaskUnmarshallingSingleton func
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectMaskTest, RSUIEffectMaskUnmarshallingSingletonTest, TestSize.Level1)
{
    uint16_t type = 666;
    auto unmarshallingFunc = [](Parcel& parcel, std::shared_ptr<MaskPara>& val) -> bool { return true; };
    MaskUnmarshallingSingleton::GetInstance().RegisterCallback(type, unmarshallingFunc);
    auto retFunc = MaskUnmarshallingSingleton::GetInstance().GetCallback(type);
    EXPECT_NE(nullptr, retFunc);
    MaskUnmarshallingSingleton::GetInstance().UnregisterCallback(type);
    retFunc = MaskUnmarshallingSingleton::GetInstance().GetCallback(type);
    EXPECT_EQ(nullptr, retFunc);
}

/**
 * @tc.name: RSUIEffectMaskParamTest
 * @tc.desc: Verify the MaskParam func
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectMaskTest, RSUIEffectMaskParamTest, TestSize.Level1)
{
    EXPECT_EQ(false, MaskPara::RegisterUnmarshallingCallback(MaskPara::Type::NONE, nullptr));

    EXPECT_EQ(false, MaskPara::RegisterUnmarshallingCallback(MaskPara::Type::PIXEL_MAP_MASK, nullptr));

    auto maskParam = std::make_shared<MaskPara>();
    EXPECT_NE(maskParam, nullptr);
    Parcel parcel;
    EXPECT_EQ(false, maskParam->Marshalling(parcel));

    std::shared_ptr<MaskPara> val = nullptr;
    EXPECT_EQ(false, maskParam->Unmarshalling(parcel, val));

    parcel.WriteUint16(static_cast<uint16_t>(MaskPara::Type::NONE));
    EXPECT_EQ(false, maskParam->Unmarshalling(parcel, val));

    Parcel parcel1;
    parcel1.WriteUint16(666);
    EXPECT_EQ(false, maskParam->Unmarshalling(parcel1, val));

    EXPECT_EQ(maskParam->Clone(), nullptr);
}

/**
 * @tc.name: RSUIEffectImageMaskParaTest
 * @tc.desc: Verify the ImageMaskPara func
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectMaskTest, RSUIEffectImageMaskParaTest, TestSize.Level1)
{
    auto maskPara = std::make_shared<ImageMaskPara>();
    auto pixelMap = CreatePixelMap(50, 50);
    EXPECT_NE(nullptr, pixelMap);
    maskPara->SetPixelMap(pixelMap);

    Parcel parcel;
    EXPECT_EQ(true, maskPara->Marshalling(parcel));
    std::shared_ptr<MaskPara> val = nullptr;
    EXPECT_EQ(false, ImageMaskPara::OnUnmarshalling(parcel, val));
    EXPECT_EQ(nullptr, val);
    auto clonePara = maskPara->Clone();
    EXPECT_NE(nullptr, clonePara);

    // test create
    maskPara = std::make_shared<ImageMaskPara>();
    EXPECT_NE(maskPara, nullptr);
    auto maskPara1 = std::make_shared<ImageMaskPara>(*maskPara);

    // test OnUnmarshalling
    std::shared_ptr<MaskPara> valTest = nullptr;
    Parcel parcelTest;
    EXPECT_EQ(false, ImageMaskPara::OnUnmarshalling(parcelTest, valTest));
    parcelTest.FlushBuffer();
    parcelTest.WriteUint16(666);
    EXPECT_EQ(false, ImageMaskPara::OnUnmarshalling(parcelTest, valTest));

    parcelTest.FlushBuffer();
    parcelTest.WriteUint16(static_cast<uint16_t>(MaskPara::Type::IMAGE_MASK));
    EXPECT_EQ(false, ImageMaskPara::OnUnmarshalling(parcelTest, valTest));
}

/**
 * @tc.name: RSUIEffectFractalGlassMaskParaTest
 * @tc.desc: Verify the FractalGlassMaskPara Clone, copy constructor, getters/setters
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectMaskTest, RSUIEffectFractalGlassMaskParaTest, TestSize.Level1)
{
    auto para = std::make_shared<FractalGlassMaskPara>();
    auto pixelMap = CreatePixelMap(50, 50);
    EXPECT_NE(nullptr, pixelMap);
    para->SetPixelMap(pixelMap);
    Vector4f src { 0.0f, 0.0f, 1.0f, 1.0f };
    para->SetSrc(src);
    Vector4f dst { 0.0f, 0.0f, 0.5f, 0.5f };
    para->SetDst(dst);
    para->SetGlassNum(25.0f);
    para->SetGlassStrength(1.0f);
    para->SetGlassSoftness(0.001f);
    para->SetIsSymmetric(true);

    auto clonePara = std::static_pointer_cast<FractalGlassMaskPara>(para->Clone());
    EXPECT_NE(nullptr, clonePara);
    EXPECT_FLOAT_EQ(clonePara->GetSrc().x_, src.x_);
    EXPECT_FLOAT_EQ(clonePara->GetSrc().z_, src.z_);
    EXPECT_FLOAT_EQ(clonePara->GetDst().x_, dst.x_);
    EXPECT_FLOAT_EQ(clonePara->GetDst().z_, dst.z_);
    EXPECT_FLOAT_EQ(clonePara->GetGlassNum(), 25.0f);
    EXPECT_FLOAT_EQ(clonePara->GetGlassStrength(), 1.0f);
    EXPECT_FLOAT_EQ(clonePara->GetGlassSoftness(), 0.001f);
    EXPECT_EQ(clonePara->GetIsSymmetric(), true);

    // Copy constructor
    auto copyPara = std::make_shared<FractalGlassMaskPara>(*para);
    EXPECT_FLOAT_EQ(copyPara->GetGlassNum(), 25.0f);
    EXPECT_EQ(copyPara->GetIsSymmetric(), true);

    // IsValid
    EXPECT_EQ(para->IsValid(), true);
    para->SetSrc(Vector4f { 1.0f, 1.0f, 0.0f, 0.0f }); // invalid: x > z
    EXPECT_EQ(para->IsValid(), false);
}

/**
 * @tc.name: RSUIEffectFractalGlassMaskParaIsSymmetricTest
 * @tc.desc: Verify FractalGlassMaskPara isSymmetric false path via clone
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectMaskTest, RSUIEffectFractalGlassMaskParaIsSymmetricTest, TestSize.Level1)
{
    auto para = std::make_shared<FractalGlassMaskPara>();
    auto pixelMap = CreatePixelMap(50, 50);
    EXPECT_NE(nullptr, pixelMap);
    para->SetPixelMap(pixelMap);
    para->SetSrc(Vector4f { 0.0f, 0.0f, 1.0f, 1.0f });
    para->SetDst(Vector4f { 0.0f, 0.0f, 1.0f, 1.0f });
    para->SetIsSymmetric(false);

    auto clonePara = std::static_pointer_cast<FractalGlassMaskPara>(para->Clone());
    EXPECT_EQ(clonePara->GetIsSymmetric(), false);
}

/**
 * @tc.name: RSUIEffectBinocularMaskParaTest
 * @tc.desc: Verify the BinocularMaskPara Clone, copy constructor, getters/setters
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectMaskTest, RSUIEffectBinocularMaskParaTest, TestSize.Level1)
{
    auto para = std::make_shared<BinocularMaskPara>();
    para->SetRadiusX(0.5f);
    para->SetRadiusY(0.8f);
    para->SetGap(0.3f);
    para->SetSoftness(0.4f);

    auto clonePara = std::static_pointer_cast<BinocularMaskPara>(para->Clone());
    EXPECT_NE(nullptr, clonePara);
    EXPECT_FLOAT_EQ(clonePara->GetRadiusX(), 0.5f);
    EXPECT_FLOAT_EQ(clonePara->GetRadiusY(), 0.8f);
    EXPECT_FLOAT_EQ(clonePara->GetGap(), 0.3f);
    EXPECT_FLOAT_EQ(clonePara->GetSoftness(), 0.4f);

    // Copy constructor
    auto copyPara = std::make_shared<BinocularMaskPara>(*para);
    EXPECT_FLOAT_EQ(copyPara->GetRadiusX(), 0.5f);
}

/**
* @tc.name: RSUIEffectSweepRefractionMaskParaTest
* @tc.desc: Verify the SweepRefractionMaskPara clone preserves all fields
* @tc.type: FUNC
*/
HWTEST_F(RSUIEffectMaskTest, RSUIEffectSweepRefractionMaskParaTest, TestSize.Level1)
{
    auto sweepRefractionMaskPara = std::make_shared<SweepRefractionMaskPara>();
    sweepRefractionMaskPara->SetMaskRadius(1.5f);
    sweepRefractionMaskPara->SetEdgeThickness(500.0f);
    sweepRefractionMaskPara->SetRefractAmount(0.3f);
    sweepRefractionMaskPara->SetRippleWidth(0.4f);
    sweepRefractionMaskPara->SetSweepOffset(0.25f);
    sweepRefractionMaskPara->SetChromaDelta(0.08f);
    sweepRefractionMaskPara->SetShapeType(1);
    sweepRefractionMaskPara->SetCornerRadius(0.16f);
    sweepRefractionMaskPara->SetPrismWidth(1.0f);
    sweepRefractionMaskPara->SetPrismHeight(1.0f);
    Vector2f center(0.5f, 0.5f);
    sweepRefractionMaskPara->SetSweepCenter(center);

    auto clonePara = std::static_pointer_cast<SweepRefractionMaskPara>(sweepRefractionMaskPara->Clone());
    EXPECT_NE(nullptr, clonePara);
    EXPECT_FLOAT_EQ(clonePara->GetMaskRadius(), 1.5f);
    EXPECT_FLOAT_EQ(clonePara->GetEdgeThickness(), 500.0f);
    EXPECT_FLOAT_EQ(clonePara->GetRefractAmount(), 0.3f);
    EXPECT_FLOAT_EQ(clonePara->GetRippleWidth(), 0.4f);
    EXPECT_FLOAT_EQ(clonePara->GetSweepOffset(), 0.25f);
    EXPECT_FLOAT_EQ(clonePara->GetChromaDelta(), 0.08f);
    EXPECT_EQ(clonePara->GetShapeType(), 1);
    EXPECT_FLOAT_EQ(clonePara->GetCornerRadius(), 0.16f);
    EXPECT_FLOAT_EQ(clonePara->GetPrismWidth(), 1.0f);
    EXPECT_FLOAT_EQ(clonePara->GetPrismHeight(), 1.0f);
    EXPECT_FLOAT_EQ(clonePara->GetSweepCenterX(), 0.5f);
    EXPECT_FLOAT_EQ(clonePara->GetSweepCenterY(), 0.5f);
}

/**
* @tc.name: RSUIEffectSweepRefractionMaskParaCopyTest
* @tc.desc: Verify SweepRefractionMaskPara copy constructor
* @tc.type: FUNC
*/
HWTEST_F(RSUIEffectMaskTest, RSUIEffectSweepRefractionMaskParaCopyTest, TestSize.Level1)
{
    auto original = std::make_shared<SweepRefractionMaskPara>();
    original->SetMaskRadius(2.0f);
    original->SetEdgeThickness(800.0f);
    original->SetRefractAmount(0.5f);
    original->SetRippleWidth(0.3f);
    original->SetSweepOffset(0.1f);
    original->SetChromaDelta(0.05f);
    original->SetShapeType(0);
    original->SetCornerRadius(0.5f);
    original->SetPrismWidth(1.5f);
    original->SetPrismHeight(1.2f);
    Vector2f center(0.3f, 0.7f);
    original->SetSweepCenter(center);

    auto copy = std::make_shared<SweepRefractionMaskPara>(*original);
    EXPECT_FLOAT_EQ(copy->GetMaskRadius(), 2.0f);
    EXPECT_FLOAT_EQ(copy->GetEdgeThickness(), 800.0f);
    EXPECT_FLOAT_EQ(copy->GetRefractAmount(), 0.5f);
    EXPECT_FLOAT_EQ(copy->GetRippleWidth(), 0.3f);
    EXPECT_FLOAT_EQ(copy->GetSweepOffset(), 0.1f);
    EXPECT_FLOAT_EQ(copy->GetChromaDelta(), 0.05f);
    EXPECT_EQ(copy->GetShapeType(), 0);
    EXPECT_FLOAT_EQ(copy->GetCornerRadius(), 0.5f);
    EXPECT_FLOAT_EQ(copy->GetPrismWidth(), 1.5f);
    EXPECT_FLOAT_EQ(copy->GetPrismHeight(), 1.2f);
    EXPECT_FLOAT_EQ(copy->GetSweepCenterX(), 0.3f);
    EXPECT_FLOAT_EQ(copy->GetSweepCenterY(), 0.7f);
}

/**
 * @tc.name: RSUIEffectWarpedRingMaskParaStoresNewArguments
 * @tc.desc: Verify WarpedRingMaskPara stores progress and every ring parameter
 * @tc.type: FUNC
 */
HWTEST_F(RSUIEffectMaskTest, RSUIEffectWarpedRingMaskParaStoresNewArguments, TestSize.Level1)
{
    auto warpedRingMaskPara = std::make_shared<WarpedRingMaskPara>();
    WarpedRingParam ringParam;
    ringParam.radius = 0.8f;
    ringParam.baseHalfWidth = 0.12f;
    ringParam.widthVariation = 0.05f;
    ringParam.rotate3DProgress = 0.6f;

    warpedRingMaskPara->SetRingParam(ringParam);
    const auto& storedRingParam = warpedRingMaskPara->GetRingParam();

    EXPECT_EQ(warpedRingMaskPara->GetMaskParaType(), MaskPara::Type::WARPED_RING_MASK);
    EXPECT_FLOAT_EQ(storedRingParam.radius, ringParam.radius);
    EXPECT_FLOAT_EQ(storedRingParam.baseHalfWidth, ringParam.baseHalfWidth);
    EXPECT_FLOAT_EQ(storedRingParam.widthVariation, ringParam.widthVariation);
    EXPECT_FLOAT_EQ(storedRingParam.rotate3DProgress, ringParam.rotate3DProgress);
}

} // namespace Rosen
} // namespace OHOS