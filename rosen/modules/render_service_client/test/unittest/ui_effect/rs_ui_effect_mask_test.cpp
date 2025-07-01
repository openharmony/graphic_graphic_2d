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

#include "pixel_map.h"
#include "mask/include/mask_para.h"
#include "mask/include/pixel_map_mask_para.h"
#include "mask/include/radial_gradient_mask_para.h"
#include "image/bitmap.h"
#include "draw/surface.h"
#include "mask/include/mask_unmarshalling_singleton.h"

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
    PixelMapMaskPara::RegisterUnmarshallingCallback();
    RadialGradientMaskPara::RegisterUnmarshallingCallback();
}

void RSUIEffectMaskTest::TearDown() {}

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
 * @tc.require:
 * @tc.author:
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
}

/**
 * @tc.name: RSUIEffectRadialGradientMaskParaTest
 * @tc.desc: Verify the RadialGradientMaskPara func
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
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
 * @tc.name: RSUIEffectMaskUnmarshallingSingletonTest
 * @tc.desc: Verify the MaskUnmarshallingSingleton func
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
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

} // namespace Rosen
} // namespace OHOS