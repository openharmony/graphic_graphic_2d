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

#include <cstdlib>
#include <cstring>
#include <memory>
#include <securec.h>
#include <string.h>

#include "gtest/gtest.h"
#include "gtest/hwext/gtest-tag.h"
#include "message_parcel.h"

#include "common/rs_vector4.h"
#include "modifier_ng/rs_modifier_ng_type.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "property/rs_properties.h"
#include "modifier_ng/appearance/rs_particle_effect_render_modifier.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;
namespace OHOS::Rosen {
class RSParticleEffectRenderModifierNGTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSParticleEffectRenderModifierNGTest::SetUpTestCase() {}
void RSParticleEffectRenderModifierNGTest::TearDownTestCase() {}
void RSParticleEffectRenderModifierNGTest::SetUp() {}
void RSParticleEffectRenderModifierNGTest::TearDown() {}

/**
 * @tc.name: RSParticleEffectRenderModifierTest
 * @tc.desc:GetType & ResetProperties
 * @tc.type:FUNC
 */
HWTEST_F(RSParticleEffectRenderModifierNGTest, RSParticleEffectRenderModifierTest, TestSize.Level1)
{
    RSParticleEffectRenderModifier modifier;
    EXPECT_EQ(modifier.GetType(), ModifierNG::RSModifierType::PARTICLE_EFFECT);
    RSProperties props;
    modifier.ResetProperties(props);
    EXPECT_TRUE(props.GetEmitterUpdater().empty());
    EXPECT_EQ(props.GetParticleNoiseFields(), nullptr);
}
} // namespace OHOS::Rosen