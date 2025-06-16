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
#include "modifier_ng/appearance/rs_particle_effect_modifier.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen {
class RSParticleEffectModifierNGTypeTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: RSBorderModifierTest
 * @tc.desc: Test Set/Get functions of RSBorderModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSParticleEffectModifierNGTypeTest, RSParticleEffectModifierTest, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSParticleEffectModifier> modifier = std::make_shared<ModifierNG::RSParticleEffectModifier>();

    EXPECT_EQ(modifier->GetType(), ModifierNG::RSModifierType::PARTICLE_EFFECT);

    std::vector<std::shared_ptr<EmitterUpdater>> nullEmitters = { nullptr, nullptr };
    modifier->SetEmitterUpdater(nullEmitters);
    EXPECT_EQ(modifier->GetEmitterUpdater(), nullEmitters);

    std::shared_ptr<ParticleNoiseFields> particleNoiseFields = std::make_shared<ParticleNoiseFields>();
    modifier->SetParticleNoiseFields(particleNoiseFields);
    EXPECT_EQ(modifier->GetParticleNoiseFields(), particleNoiseFields);
}
} // namespace OHOS::Rosen