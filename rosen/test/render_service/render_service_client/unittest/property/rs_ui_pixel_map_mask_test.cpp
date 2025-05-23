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
#include "platform/image_native/pixel_map.h"
#include "render/rs_render_pixel_map_mask.h"
#include "ui_effect/property/include/rs_ui_pixel_map_mask.h"
#include "ui_effect/mask/include/pixel_map_mask_para.h"
#include "ui_effect/property/include/rs_ui_ripple_mask.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUIPixelMapMaskTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUIPixelMapMaskTest::SetUpTestCase() {}
void RSUIPixelMapMaskTest::TearDownTestCase() {}
void RSUIPixelMapMaskTest::SetUp() {}
void RSUIPixelMapMaskTest::TearDown() {}

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
 * @tc.name: Equal001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIPixelMapMaskTest, Equal001, TestSize.Level1)
{
    auto rsUIPixelMapMaskPara1 = std::make_shared<RSUIPixelMapMaskPara>();
    auto rsUIPixelMapMaskPara2 = std::make_shared<RSUIPixelMapMaskPara>();
    auto rsUIRippleMaskPara2 = std::make_shared<RSUIRippleMaskPara>();

    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIPixelMapMaskPara2);
    EXPECT_TRUE(rsUIPixelMapMaskPara1->Equals(rsUIFilterParaBase));

    rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIRippleMaskPara2);
    EXPECT_FALSE(rsUIPixelMapMaskPara1->Equals(rsUIFilterParaBase));

    EXPECT_FALSE(rsUIPixelMapMaskPara1->Equals(nullptr));
}

/**
 * @tc.name: Set001
 * @tc.desc: test SetPara and SetProperty, check by Get properties
 * @tc.type:FUNC
 */
HWTEST_F(RSUIPixelMapMaskTest, Set001, TestSize.Level1)
{
    auto pixelMapMaskPara = std::make_shared<PixelMapMaskPara>();

    auto pixelMap = CreatePixelMap(50, 50);
    EXPECT_NE(pixelMap, nullptr);
    EXPECT_EQ(pixelMapMaskPara->GetPixelMap(), nullptr);
    pixelMapMaskPara->SetPixelMap(pixelMap);
    EXPECT_NE(pixelMapMaskPara->GetPixelMap(), nullptr);

    Vector4f src(0.1, 0.2, 0.6, 0.9);
    EXPECT_EQ(pixelMapMaskPara->GetSrc(), Vector4f());
    pixelMapMaskPara->SetSrc(src);
    EXPECT_NE(pixelMapMaskPara->GetSrc(), src);

    Vector4f dst(0.4, 0.3, 0.7, 0.5);
    EXPECT_EQ(pixelMapMaskPara->GetDst(), Vector4f());
    pixelMapMaskPara->SetDst(dst);
    EXPECT_NE(pixelMapMaskPara->GetDst(), dst);

    Vector4f fillColor(0.3, 0.6, 0.1, 0.7);
    EXPECT_EQ(pixelMapMaskPara->GetFillColor(), Vector4f());
    pixelMapMaskPara->SetFillColor(fillColor);
    EXPECT_NE(pixelMapMaskPara->GetFillColor(), fillColor);

    auto maskPara = std::static_pointer_cast<MaskPara>(pixelMapMaskPara);

    auto rsUIPixelMapMaskPara1 = std::make_shared<RSUIPixelMapMaskPara>();
    auto rsUIPixelMapMaskPara2 = std::make_shared<RSUIPixelMapMaskPara>();
    rsUIPixelMapMaskPara2->SetPara(maskPara);

    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIPixelMapMaskPara2);
    rsUIPixelMapMaskPara1->SetProperty(rsUIFilterParaBase);

    {
        auto iter = rsUIPixelMapMaskPara1->properties_.find(RSUIFilterType::PIXEL_MAP_MASK_PIXEL_MAP);
        ASSERT_NE(iter, rsUIPixelMapMaskPara1->properties_.end());
        auto property = std::static_pointer_cast<RSProperty<std::shared_ptr<Media::PixelMap>>>(iter->second);
        ASSERT_NE(property, nullptr);
        EXPECT_NE(property->Get(), nullptr);
    }

    {
        auto iter = rsUIPixelMapMaskPara1->properties_.find(RSUIFilterType::PIXEL_MAP_MASK_SRC);
        ASSERT_NE(iter, rsUIPixelMapMaskPara1->properties_.end());
        auto property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(iter->second);
        ASSERT_NE(property, nullptr);
        EXPECT_EQ(property->Get(), src);
    }

    {
        auto iter = rsUIPixelMapMaskPara1->properties_.find(RSUIFilterType::PIXEL_MAP_MASK_DST);
        ASSERT_NE(iter, rsUIPixelMapMaskPara1->properties_.end());
        auto property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(iter->second);
        ASSERT_NE(property, nullptr);
        EXPECT_EQ(property->Get(), dst);
    }

    {
        auto iter = rsUIPixelMapMaskPara1->properties_.find(RSUIFilterType::PIXEL_MAP_MASK_FILL_COLOR);
        ASSERT_NE(iter, rsUIPixelMapMaskPara1->properties_.end());
        auto property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(iter->second);
        ASSERT_NE(property, nullptr);
        EXPECT_EQ(property->Get(), fillColor);
    }
}

/**
 * @tc.name: Set002
 * @tc.desc: test SetPara and SetProperty for abnormal input
 * @tc.type:FUNC
 */
HWTEST_F(RSUIPixelMapMaskTest, Set002, TestSize.Level1)
{
    auto rsUIPixelMapMaskPara = std::make_shared<RSUIPixelMapMaskPara>();
    auto pixelMapMaskPara = std::make_shared<PixelMapMaskPara>();
    auto rsUIMaskPara = std::make_shared<RSUIMaskPara>(RSUIFilterType::RADIAL_GRADIENT_MASK);
    auto maskPara = std::make_shared<MaskPara>();

    rsUIPixelMapMaskPara->SetPara(nullptr);
    rsUIPixelMapMaskPara->SetPara(maskPara);

    rsUIPixelMapMaskPara->SetProperty(nullptr);
    rsUIPixelMapMaskPara->SetProperty(rsUIMaskPara);

    auto iter = rsUIPixelMapMaskPara->properties_.find(RSUIFilterType::PIXEL_MAP_MASK_PIXEL_MAP);
    EXPECT_EQ(iter, rsUIPixelMapMaskPara->properties_.end());

    iter = rsUIPixelMapMaskPara->properties_.find(RSUIFilterType::PIXEL_MAP_MASK_SRC);
    EXPECT_EQ(iter, rsUIPixelMapMaskPara->properties_.end());

    iter = rsUIPixelMapMaskPara->properties_.find(RSUIFilterType::PIXEL_MAP_MASK_DST);
    EXPECT_EQ(iter, rsUIPixelMapMaskPara->properties_.end());

    iter = rsUIPixelMapMaskPara->properties_.find(RSUIFilterType::PIXEL_MAP_MASK_FILL_COLOR);
    EXPECT_EQ(iter, rsUIPixelMapMaskPara->properties_.end());
}

/**
 * @tc.name: CreateRSRenderFilter001
 * @tc.desc: test for CreateRSRenderFilter
 * @tc.type:FUNC
 */
HWTEST_F(RSUIPixelMapMaskTest, CreateRSRenderFilter001, TestSize.Level1)
{
    auto pixelMapMaskPara = std::make_shared<PixelMapMaskPara>();

    auto pixelMap = CreatePixelMap(50, 50);
    EXPECT_NE(pixelMap, nullptr);
    pixelMapMaskPara->SetPixelMap(pixelMap);

    Vector4f src(0.1, 0.2, 0.6, 0.9);
    pixelMapMaskPara->SetSrc(src);

    Vector4f dst(0.4, 0.3, 0.7, 0.5);
    pixelMapMaskPara->SetDst(dst);

    Vector4f fillColor(0.3, 0.6, 0.1, 0.7);
    pixelMapMaskPara->SetFillColor(fillColor);

    auto maskPara = std::static_pointer_cast<MaskPara>(pixelMapMaskPara);

    auto rsUIPixelMapMaskPara = std::make_shared<RSUIPixelMapMaskPara>();
    rsUIPixelMapMaskPara->SetPara(maskPara);

    EXPECT_NE(rsUIPixelMapMaskPara->CreateRSRenderFilter(), nullptr);
}

/**
 * @tc.name: CreateRSRenderFilter002
 * @tc.desc: test for CreateRSRenderFilter
 * @tc.type:FUNC
 */
HWTEST_F(RSUIPixelMapMaskTest, CreateRSRenderFilter002, TestSize.Level1)
{
    auto rsUIPixelMapMaskPara = std::make_shared<RSUIPixelMapMaskPara>();
    ASSERT_EQ(rsUIPixelMapMaskPara->CreateRSRenderFilter(), nullptr);

    auto pixelMap = CreatePixelMap(50, 50);
    rsUIPixelMapMaskPara->SetPixelMap(pixelMap);
    ASSERT_EQ(rsUIPixelMapMaskPara->CreateRSRenderFilter(), nullptr);

    Vector4f src(0.1, 0.2, 0.6, 0.9);
    rsUIPixelMapMaskPara->SetSrc(src);
    ASSERT_EQ(rsUIPixelMapMaskPara->CreateRSRenderFilter(), nullptr);

    Vector4f dst(0.4, 0.3, 0.7, 0.5);
    rsUIPixelMapMaskPara->SetDst(dst);
    ASSERT_EQ(rsUIPixelMapMaskPara->CreateRSRenderFilter(), nullptr);

    Vector4f fillColor(0.3, 0.6, 0.1, 0.7);
    rsUIPixelMapMaskPara->SetFillColor(fillColor);
    EXPECT_NE(rsUIPixelMapMaskPara->CreateRSRenderFilter(), nullptr);
}

/**
 * @tc.name: GetLeafProperties001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIPixelMapMaskTest, GetLeafProperties001, TestSize.Level1)
{
    auto rsUIPixelMapMaskPara = std::make_shared<RSUIPixelMapMaskPara>();

    auto rsPropertyBaseVector = rsUIPixelMapMaskPara->GetLeafProperties();
    ASSERT_EQ(rsPropertyBaseVector.size(), 0);

    auto pixelMap = CreatePixelMap(50, 50);
    rsUIPixelMapMaskPara->SetPixelMap(pixelMap);
    rsPropertyBaseVector = rsUIPixelMapMaskPara->GetLeafProperties();
    ASSERT_EQ(rsPropertyBaseVector.size(), 1);

    Vector4f src(0.1, 0.2, 0.6, 0.9);
    rsUIPixelMapMaskPara->SetSrc(src);
    Vector4f dst(0.4, 0.3, 0.7, 0.5);
    rsUIPixelMapMaskPara->SetDst(dst);
    rsPropertyBaseVector = rsUIPixelMapMaskPara->GetLeafProperties();
    ASSERT_EQ(rsPropertyBaseVector.size(), 3);

    Vector4f fillColor(0.3, 0.6, 0.1, 0.7);
    rsUIPixelMapMaskPara->SetFillColor(fillColor);
    ASSERT_EQ(rsPropertyBaseVector.size(), 4);
}
} // namespace OHOS::Rosen