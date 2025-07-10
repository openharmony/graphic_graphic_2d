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
#include "pixel_map.h"
#include "render/rs_render_pixel_map_mask.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderPixelMapMaskTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderPixelMapMaskTest::SetUpTestCase() {}
void RSRenderPixelMapMaskTest::TearDownTestCase() {}
void RSRenderPixelMapMaskTest::SetUp() {}
void RSRenderPixelMapMaskTest::TearDown() {}

/**
 * @tc.name: CreateRenderProperty001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPixelMapMaskTest, CreateRenderProperty001, TestSize.Level1)
{
    auto renderProperty = RSRenderPixelMapMaskPara::CreateRenderProperty(RSUIFilterType::PIXEL_MAP_MASK_PIXEL_MAP);
    EXPECT_NE(renderProperty, nullptr);

    renderProperty = RSRenderPixelMapMaskPara::CreateRenderProperty(RSUIFilterType::PIXEL_MAP_MASK_SRC);
    EXPECT_NE(renderProperty, nullptr);

    renderProperty = RSRenderPixelMapMaskPara::CreateRenderProperty(RSUIFilterType::PIXEL_MAP_MASK_DST);
    EXPECT_NE(renderProperty, nullptr);

    renderProperty = RSRenderPixelMapMaskPara::CreateRenderProperty(RSUIFilterType::PIXEL_MAP_MASK_FILL_COLOR);
    EXPECT_NE(renderProperty, nullptr);

    renderProperty = RSRenderPixelMapMaskPara::CreateRenderProperty(RSUIFilterType::BLUR);
    EXPECT_EQ(renderProperty, nullptr);
}

/**
 * @tc.name: GetAnimatRenderProperty001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPixelMapMaskTest, GetAnimatRenderProperty001, TestSize.Level1)
{
    auto rsRenderPixelMapMaskPara = std::make_shared<RSRenderPixelMapMaskPara>(0);
    auto uiFilterType = RSUIFilterType::NONE;
    auto animatRenderProperty = rsRenderPixelMapMaskPara->GetRenderAnimatableProperty<Vector2f>(uiFilterType);
    EXPECT_EQ(animatRenderProperty, nullptr);
}

/**
 * @tc.name: GetDescription001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPixelMapMaskTest, GetDescription001, TestSize.Level1)
{
    auto rsRenderPixelMapMaskPara = std::make_shared<RSRenderPixelMapMaskPara>(0);
    std::string out;
    rsRenderPixelMapMaskPara->GetDescription(out);
    EXPECT_FALSE(out.empty());
}

/**
 * @tc.name: Parcel001
 * @tc.desc: test for WriteToParcel and ReadFromParcel
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPixelMapMaskTest, WriteToParcel001, TestSize.Level1)
{
    auto rsRenderPixelMapMaskPara = std::make_shared<RSRenderPixelMapMaskPara>(0);
    Parcel parcel;
    auto renderProperty = RSRenderPixelMapMaskPara::CreateRenderProperty(RSUIFilterType::PIXEL_MAP_MASK_PIXEL_MAP);
    ASSERT_NE(renderProperty, nullptr);
    rsRenderPixelMapMaskPara->properties_[RSUIFilterType::PIXEL_MAP_MASK_PIXEL_MAP] = renderProperty;

    renderProperty = RSRenderPixelMapMaskPara::CreateRenderProperty(RSUIFilterType::PIXEL_MAP_MASK_SRC);
    ASSERT_NE(renderProperty, nullptr);
    rsRenderPixelMapMaskPara->properties_[RSUIFilterType::PIXEL_MAP_MASK_SRC] = renderProperty;

    renderProperty = RSRenderPixelMapMaskPara::CreateRenderProperty(RSUIFilterType::PIXEL_MAP_MASK_DST);
    ASSERT_NE(renderProperty, nullptr);
    rsRenderPixelMapMaskPara->properties_[RSUIFilterType::PIXEL_MAP_MASK_DST] = renderProperty;

    renderProperty = RSRenderPixelMapMaskPara::CreateRenderProperty(RSUIFilterType::PIXEL_MAP_MASK_FILL_COLOR);
    ASSERT_NE(renderProperty, nullptr);
    rsRenderPixelMapMaskPara->properties_[RSUIFilterType::PIXEL_MAP_MASK_FILL_COLOR] = renderProperty;

    ASSERT_TRUE(rsRenderPixelMapMaskPara->WriteToParcel(parcel));

    // pixelmap value is nullptr, return false
    ASSERT_FALSE(rsRenderPixelMapMaskPara->ReadFromParcel(parcel));
}

/**
 * @tc.name: GetLeafRenderProperties001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPixelMapMaskTest, GetLeafRenderProperties001, TestSize.Level1)
{
    auto rsRenderPixelMapMaskPara = std::make_shared<RSRenderPixelMapMaskPara>(0);
    auto rsRenderPropertyBaseVec = rsRenderPixelMapMaskPara->GetLeafRenderProperties();
    EXPECT_TRUE(rsRenderPropertyBaseVec.empty());

    auto renderProperty = RSRenderPixelMapMaskPara::CreateRenderProperty(RSUIFilterType::PIXEL_MAP_MASK_PIXEL_MAP);
    ASSERT_NE(renderProperty, nullptr);
    rsRenderPixelMapMaskPara->properties_[RSUIFilterType::PIXEL_MAP_MASK_PIXEL_MAP] = renderProperty;
    rsRenderPropertyBaseVec = rsRenderPixelMapMaskPara->GetLeafRenderProperties();
    EXPECT_EQ(rsRenderPropertyBaseVec.size(), 1);

    renderProperty = RSRenderPixelMapMaskPara::CreateRenderProperty(RSUIFilterType::PIXEL_MAP_MASK_SRC);
    ASSERT_NE(renderProperty, nullptr);
    rsRenderPixelMapMaskPara->properties_[RSUIFilterType::PIXEL_MAP_MASK_SRC] = renderProperty;
    rsRenderPropertyBaseVec = rsRenderPixelMapMaskPara->GetLeafRenderProperties();
    EXPECT_EQ(rsRenderPropertyBaseVec.size(), 2);

    renderProperty = RSRenderPixelMapMaskPara::CreateRenderProperty(RSUIFilterType::PIXEL_MAP_MASK_DST);
    ASSERT_NE(renderProperty, nullptr);
    rsRenderPixelMapMaskPara->properties_[RSUIFilterType::PIXEL_MAP_MASK_DST] = renderProperty;
    rsRenderPropertyBaseVec = rsRenderPixelMapMaskPara->GetLeafRenderProperties();
    EXPECT_EQ(rsRenderPropertyBaseVec.size(), 3);

    renderProperty = RSRenderPixelMapMaskPara::CreateRenderProperty(RSUIFilterType::PIXEL_MAP_MASK_FILL_COLOR);
    ASSERT_NE(renderProperty, nullptr);
    rsRenderPixelMapMaskPara->properties_[RSUIFilterType::PIXEL_MAP_MASK_FILL_COLOR] = renderProperty;
    rsRenderPropertyBaseVec = rsRenderPixelMapMaskPara->GetLeafRenderProperties();
    EXPECT_EQ(rsRenderPropertyBaseVec.size(), 4);
}

/**
 * @tc.name: LimitedDeepCopy
 * @tc.desc: test for LimitedDeepCopy
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPixelMapMaskTest, LimitedDeepCopy001, TestSize.Level1)
{
    auto rsRenderPixelMapMaskPara = std::make_shared<RSRenderPixelMapMaskPara>(0);
    rsRenderPixelMapMaskPara->modifierType_ = RSModifierType::BACKGROUND_UI_FILTER;
    rsRenderPixelMapMaskPara->Setter(RSUIFilterType::PIXEL_MAP_MASK, nullptr);

    // pixel map
    auto pixelMap = std::make_shared<Media::PixelMap>();
    auto pixelMapProperty = std::make_shared<RSRenderProperty<std::shared_ptr<Media::PixelMap>>>(pixelMap, 0);
    rsRenderPixelMapMaskPara->Setter(RSUIFilterType::PIXEL_MAP_MASK_PIXEL_MAP, pixelMapProperty);

    // image
    rsRenderPixelMapMaskPara->cacheImage_ = std::make_shared<Drawing::Image>();

    // src
    auto src = Vector4f(0.1, 0.3, 0.9, 1);
    auto srcProperty = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(src, 0);
    rsRenderPixelMapMaskPara->Setter(RSUIFilterType::PIXEL_MAP_MASK_SRC, srcProperty);

    // dst
    auto dst = Vector4f(0.02, 0.35, 0.79, 0.98);
    auto dstProperty = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(dst, 0);
    rsRenderPixelMapMaskPara->Setter(RSUIFilterType::PIXEL_MAP_MASK_DST, dstProperty);

    // fill color
    auto fillColor = Vector4f(0.3, 0.4, 0.2, 0.66);
    auto fillColorProperty = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(fillColor, 0);
    rsRenderPixelMapMaskPara->Setter(RSUIFilterType::PIXEL_MAP_MASK_FILL_COLOR, fillColorProperty);

    rsRenderPixelMapMaskPara->Setter(RSUIFilterType::EDGE_LIGHT, fillColorProperty);

    auto para = rsRenderPixelMapMaskPara->LimitedDeepCopy();
    ASSERT_NE(para, nullptr);
    auto newPixelMapMaskPara = std::static_pointer_cast<RSRenderPixelMapMaskPara>(para);

    EXPECT_EQ(rsRenderPixelMapMaskPara->id_, newPixelMapMaskPara->id_);
    EXPECT_EQ(rsRenderPixelMapMaskPara->type_, newPixelMapMaskPara->type_);
    EXPECT_EQ(rsRenderPixelMapMaskPara->modifierType_, newPixelMapMaskPara->modifierType_);

    // pixel map
    ASSERT_NE(newPixelMapMaskPara->properties_[RSUIFilterType::PIXEL_MAP_MASK_PIXEL_MAP], nullptr);
    EXPECT_NE(rsRenderPixelMapMaskPara->properties_[RSUIFilterType::PIXEL_MAP_MASK_PIXEL_MAP],
        newPixelMapMaskPara->properties_[RSUIFilterType::PIXEL_MAP_MASK_PIXEL_MAP]);
    auto newPixelMap = std::static_pointer_cast<RSRenderProperty<std::shared_ptr<Media::PixelMap>>>(
        newPixelMapMaskPara->properties_[RSUIFilterType::PIXEL_MAP_MASK_PIXEL_MAP])->Get();
    EXPECT_EQ(pixelMap, newPixelMap);

    // image
    EXPECT_EQ(rsRenderPixelMapMaskPara->GetImage(), newPixelMapMaskPara->GetImage());

    // src
    ASSERT_NE(newPixelMapMaskPara->properties_[RSUIFilterType::PIXEL_MAP_MASK_SRC], nullptr);
    EXPECT_NE(rsRenderPixelMapMaskPara->properties_[RSUIFilterType::PIXEL_MAP_MASK_SRC],
        newPixelMapMaskPara->properties_[RSUIFilterType::PIXEL_MAP_MASK_SRC]);
    auto newSrc = std::static_pointer_cast<RSRenderAnimatableProperty<Vector4f>>(
        newPixelMapMaskPara->properties_[RSUIFilterType::PIXEL_MAP_MASK_SRC])->Get();
    EXPECT_EQ(src, newSrc);

    // dst
    ASSERT_NE(newPixelMapMaskPara->properties_[RSUIFilterType::PIXEL_MAP_MASK_DST], nullptr);
    EXPECT_NE(rsRenderPixelMapMaskPara->properties_[RSUIFilterType::PIXEL_MAP_MASK_DST],
        newPixelMapMaskPara->properties_[RSUIFilterType::PIXEL_MAP_MASK_DST]);
    auto newDst = std::static_pointer_cast<RSRenderAnimatableProperty<Vector4f>>(
        newPixelMapMaskPara->properties_[RSUIFilterType::PIXEL_MAP_MASK_DST])->Get();
    EXPECT_EQ(dst, newDst);

    // fill color
    ASSERT_NE(newPixelMapMaskPara->properties_[RSUIFilterType::PIXEL_MAP_MASK_FILL_COLOR], nullptr);
    EXPECT_NE(rsRenderPixelMapMaskPara->properties_[RSUIFilterType::PIXEL_MAP_MASK_FILL_COLOR],
        newPixelMapMaskPara->properties_[RSUIFilterType::PIXEL_MAP_MASK_FILL_COLOR]);
    auto newFillColor = std::static_pointer_cast<RSRenderAnimatableProperty<Vector4f>>(
        newPixelMapMaskPara->properties_[RSUIFilterType::PIXEL_MAP_MASK_FILL_COLOR])->Get();
    EXPECT_EQ(fillColor, newFillColor);
}
} // namespace OHOS::Rosen