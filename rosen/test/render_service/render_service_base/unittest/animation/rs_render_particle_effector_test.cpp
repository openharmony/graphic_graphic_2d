/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "animation/rs_render_particle_effector.h"
#include "animation/rs_render_particle.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderParticleEffectorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderParticleEffectorTest::SetUpTestCase() {}
void RSRenderParticleEffectorTest::TearDownTestCase() {}
void RSRenderParticleEffectorTest::SetUp() {}
void RSRenderParticleEffectorTest::TearDown() {}

/**
 * @tc.name: UpdateOpacityTest
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleEffectorTest, UpdateOpacityTest, Level1)
{
    float deltaTime = 0.f;
    int64_t activeTime = 1;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticleEffector rsRenderParticleEffector(particleParams);
    rsRenderParticleEffector.UpdateOpacity(nullptr, deltaTime, activeTime);
    ASSERT_NE(activeTime, 0);
}

/**
 * @tc.name: UpdateScaleTest
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleEffectorTest, UpdateScaleTest, Level1)
{
    float deltaTime = 0.f;
    int64_t activeTime = 1;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticleEffector rsRenderParticleEffector(particleParams);
    rsRenderParticleEffector.UpdateScale(nullptr, deltaTime, activeTime);
    ASSERT_NE(activeTime, 0);
}

/**
 * @tc.name: UpdateSpinTest
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleEffectorTest, UpdateSpinTest, Level1)
{
    float deltaTime = 0.f;
    int64_t activeTime = 1;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticleEffector rsRenderParticleEffector(particleParams);
    rsRenderParticleEffector.UpdateSpin(nullptr, deltaTime, activeTime);
    ASSERT_NE(activeTime, 0);
}
}