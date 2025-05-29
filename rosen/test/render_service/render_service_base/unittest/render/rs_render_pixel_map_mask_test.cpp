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
} // namespace OHOS::Rosen