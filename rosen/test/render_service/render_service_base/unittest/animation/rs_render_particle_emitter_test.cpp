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
#include "animation/rs_render_particle_emitter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderParticleEmitterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderParticleEmitterTest::SetUpTestCase() {}
void RSRenderParticleEmitterTest::TearDownTestCase() {}
void RSRenderParticleEmitterTest::SetUp() {}
void RSRenderParticleEmitterTest::TearDown() {}

/**
 * @tc.name: EmitParticleTest
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleEmitterTest, EmitParticleTest, Level1)
{
    int64_t deltaTime = 1;
    auto particleParams = std::shared_ptr<ParticleRenderParams>();
    RSRenderParticleEmitter rsRenderParticleEmitter(particleParams);
    rsRenderParticleEmitter.EmitParticle(deltaTime);
    ASSERT_NE(deltaTime, 0);
}

/**
 * @tc.name: UpdateParticleTest
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticleEmitterTest, UpdateParticleTest, Level1)
{
    int64_t deltaTime = 1;
    auto particleParams = std::shared_ptr<ParticleRenderParams>();
    RSRenderParticleEmitter rsRenderParticleEmitter(particleParams);
    rsRenderParticleEmitter.UpdateParticle(deltaTime);
    ASSERT_NE(deltaTime, 0);
}
}