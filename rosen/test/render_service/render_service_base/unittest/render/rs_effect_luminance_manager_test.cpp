/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <vector>
#include "render/rs_effect_luminance_manager.h"
#include "render/rs_render_filter_base.h"
#include "effect/rs_render_shader_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSEffectLuminanceManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSEffectLuminanceManagerTest::SetUpTestCase() {}
void RSEffectLuminanceManagerTest::TearDownTestCase() {}
void RSEffectLuminanceManagerTest::SetUp() {}
void RSEffectLuminanceManagerTest::TearDown() {}

/**
 * @tc.name: HeadroomSetGetTest
 * @tc.desc: test results of SetDisplayHeadroom & GetDisplayHeadroom
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectLuminanceManagerTest, HeadroomSetGetTest, TestSize.Level1)
{
    auto& manager = RSEffectLuminanceManager::GetInstance();
    EXPECT_EQ(manager.GetDisplayHeadroom(0), 1.0f);
    manager.SetDisplayHeadroom(0, 4.0f);
    EXPECT_EQ(manager.GetDisplayHeadroom(0), 4.0f);
}

/**
 * @tc.name: GetEnableHdrEffectEdgeLightTest
 * @tc.desc: test results of GetEnableHdrEffectEdgeLight
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectLuminanceManagerTest, GetEnableHdrEffectEdgeLight, TestSize.Level1)
{
    auto& manager = RSEffectLuminanceManager::GetInstance();
    EXPECT_FALSE(manager.GetEnableHdrEffect(std::shared_ptr<RSNGRenderFilterBase>(nullptr)));

    auto elFilter = std::make_shared<RSNGRenderEdgeLightFilter>();
    EXPECT_FALSE(manager.GetEnableHdrEffect(elFilter));

    Vector4f color{0.5f, 1.5f, 0.5f, 1.0f};
    elFilter->Setter<EdgeLightColorRenderTag>(color);
    EXPECT_TRUE(manager.GetEnableHdrEffect(elFilter));
}

/**
 * @tc.name: GetEnableHdrEffectColorGradientTest
 * @tc.desc: test results of GetEnableHdrEffectColorGradient
 * @tc.type: FUNC
*/
HWTEST_F(RSEffectLuminanceManagerTest, GetEnableHdrEffectColorGradient, TestSize.Level1)
{
    auto& manager = RSEffectLuminanceManager::GetInstance();
    auto cgFilter = std::make_shared<RSNGRenderColorGradientFilter>();
    EXPECT_FALSE(manager.GetEnableHdrEffect(cgFilter));

    std::vector<float> color = {0.5f, 1.0f, 0.5f, 1.0f, 0.5f, 1.5f, 0.5f, 1.0f};
    cgFilter->Setter<ColorGradientColorsRenderTag>(color);
    EXPECT_TRUE(manager.GetEnableHdrEffect(cgFilter));
}

/**
 * @tc.name: GetEnableHdrEffectSoundWaveTest
 * @tc.desc: test results of GetEnableHdrEffectSoundWave
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectLuminanceManagerTest, GetEnableHdrEffectSoundWave, TestSize.Level1)
{
    auto& manager = RSEffectLuminanceManager::GetInstance();
    auto swFilter = std::make_shared<RSNGRenderSoundWaveFilter>();
    EXPECT_FALSE(manager.GetEnableHdrEffect(swFilter));

    Vector4f colorA{0.5f, 1.5f, 0.5f, 1.0f};
    Vector4f colorB{0.5f, 1.5f, 0.5f, 1.0f};
    Vector4f colorC{0.5f, 1.5f, 0.5f, 1.0f};
    swFilter->Setter<SoundWaveColorARenderTag>(colorA);
    swFilter->Setter<SoundWaveColorBRenderTag>(colorB);
    swFilter->Setter<SoundWaveColorCRenderTag>(colorC);
    EXPECT_TRUE(manager.GetEnableHdrEffect(swFilter));
}

/**
 * @tc.name: GetEnableHdrShaderTest
 * @tc.desc: test results of GetEnableHdrShader
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectLuminanceManagerTest, GetEnableHdrShader, TestSize.Level1)
{
    auto& manager = RSEffectLuminanceManager::GetInstance();
    EXPECT_FALSE(manager.GetEnableHdrEffect(std::shared_ptr<RSNGRenderShaderBase>(nullptr)));

    auto cdflShader = RSNGRenderShaderBase::Create(RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT);
    EXPECT_FALSE(manager.GetEnableHdrEffect(cdflShader));
}

} // namespace OHOS::Rosen