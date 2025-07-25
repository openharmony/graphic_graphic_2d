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

#include "ge_ripple_shader_mask.h"
#include "platform/common/rs_log.h"
#include "render/rs_shader_mask.h"
#include "render/rs_render_pixel_map_mask.h"
#include "render/rs_render_ripple_mask.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSShaderMaskTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSShaderMaskTest::SetUpTestCase() {}
void RSShaderMaskTest::TearDownTestCase() {}
void RSShaderMaskTest::SetUp() {}
void RSShaderMaskTest::TearDown() {}

/**
* @tc.name: CalHashTest001
* @tc.desc: Verify function CalHash
* @tc.type: FUNC
*/
HWTEST_F(RSShaderMaskTest, CalHashTest001, TestSize.Level1)
{
    auto rsRenderMaskPara = std::make_shared<RSRenderMaskPara>(RSUIFilterType::RIPPLE_MASK);
    auto rsShaderMask = std::make_shared<RSShaderMask>(rsRenderMaskPara);
    rsShaderMask->CalHash();
    EXPECT_EQ(rsShaderMask->hash_, 0);
}

/**
* @tc.name: PixelMapMaskTest001
* @tc.desc: Verify pixel map mask
* @tc.type: FUNC
*/
HWTEST_F(RSShaderMaskTest, PixelMapMaskTest001, TestSize.Level1)
{
    auto rsRenderPixelMapMaskPara = std::make_shared<RSRenderPixelMapMaskPara>(0);
    auto rsShaderMask = std::make_shared<RSShaderMask>(rsRenderPixelMapMaskPara);
#ifdef USE_M133_SKIA
    EXPECT_NE(rsShaderMask->hash_, 0);
#else
    EXPECT_EQ(rsShaderMask->hash_, 0);
#endif
    EXPECT_EQ(rsShaderMask->GenerateGEShaderMask(), nullptr);
    ASSERT_NE(rsShaderMask->renderMask_, nullptr);
    rsRenderPixelMapMaskPara = std::static_pointer_cast<RSRenderPixelMapMaskPara>(rsShaderMask->renderMask_);

    // pixel map
    auto renderProperty = RSRenderPixelMapMaskPara::CreateRenderProperty(RSUIFilterType::PIXEL_MAP_MASK_PIXEL_MAP);
    ASSERT_NE(renderProperty, nullptr);
    rsRenderPixelMapMaskPara->properties_[RSUIFilterType::PIXEL_MAP_MASK_PIXEL_MAP] = renderProperty;
    rsRenderPixelMapMaskPara->cacheImage_ = std::make_shared<Drawing::Image>();
    rsShaderMask->CalHash();
#ifdef USE_M133_SKIA
    EXPECT_NE(rsShaderMask->hash_, 0);
#else
    EXPECT_EQ(rsShaderMask->hash_, 0);
#endif
    EXPECT_EQ(rsShaderMask->GenerateGEShaderMask(), nullptr);

    // src
    renderProperty = RSRenderPixelMapMaskPara::CreateRenderProperty(RSUIFilterType::PIXEL_MAP_MASK_SRC);
    ASSERT_NE(renderProperty, nullptr);
    auto prop = std::static_pointer_cast<RSRenderAnimatableProperty<Vector4f>>(renderProperty);
    prop->Set(Vector4f(0.5, 0.1, 0.3, 0.5));
    rsRenderPixelMapMaskPara->properties_[RSUIFilterType::PIXEL_MAP_MASK_SRC] = renderProperty;
    rsShaderMask->CalHash();
#ifdef USE_M133_SKIA
    EXPECT_NE(rsShaderMask->hash_, 0);
#else
    EXPECT_EQ(rsShaderMask->hash_, 0);
#endif
    EXPECT_EQ(rsShaderMask->GenerateGEShaderMask(), nullptr);

    // dst
    renderProperty = RSRenderPixelMapMaskPara::CreateRenderProperty(RSUIFilterType::PIXEL_MAP_MASK_DST);
    ASSERT_NE(renderProperty, nullptr);
    rsRenderPixelMapMaskPara->properties_[RSUIFilterType::PIXEL_MAP_MASK_DST] = renderProperty;
    rsShaderMask->CalHash();
#ifdef USE_M133_SKIA
    EXPECT_NE(rsShaderMask->hash_, 0);
#else
    EXPECT_EQ(rsShaderMask->hash_, 0);
#endif
    EXPECT_EQ(rsShaderMask->GenerateGEShaderMask(), nullptr);

    // fill color
    renderProperty = RSRenderPixelMapMaskPara::CreateRenderProperty(RSUIFilterType::PIXEL_MAP_MASK_FILL_COLOR);
    ASSERT_NE(renderProperty, nullptr);
    prop = std::static_pointer_cast<RSRenderAnimatableProperty<Vector4f>>(renderProperty);
    prop->Set(Vector4f(0.1, 0.3, 0.7, 0.5));
    rsRenderPixelMapMaskPara->properties_[RSUIFilterType::PIXEL_MAP_MASK_FILL_COLOR] = renderProperty;

    rsShaderMask->CalHash();
    EXPECT_NE(rsShaderMask->hash_, 0);
    EXPECT_NE(rsShaderMask->GenerateGEShaderMask(), nullptr);
}

/**
* @tc.name: RSShaderMaskInputNullTest
* @tc.desc: Verify RSShaderMask construct with nullptr
* @tc.type: FUNC
*/
HWTEST_F(RSShaderMaskTest, RSShaderMaskInputNullTest, TestSize.Level1)
{
    auto rsShaderMask = std::make_shared<RSShaderMask>(nullptr);
    EXPECT_EQ(rsShaderMask->renderMask_, nullptr);
    EXPECT_EQ(rsShaderMask->GenerateGEShaderMask(), nullptr);
    EXPECT_EQ(rsShaderMask->Hash(), 0);
}
} // namespace OHOS::Rosen