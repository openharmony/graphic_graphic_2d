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
#include "animation/rs_render_particle.h"
#include <cstdint>
#include <random>
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderParticleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderParticleTest::SetUpTestCase() {}
void RSRenderParticleTest::TearDownTestCase() {}
void RSRenderParticleTest::SetUp() {}
void RSRenderParticleTest::TearDown() {}

/**
 * @tc.name: CalculateParticlePositionTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleTest, CalculateParticlePositionTest, TestSize.Level1)
{
    ShapeType emitShape_ = ShapeType::RECT;
    Vector2f position_;
    Vector2f emitSize_;
    auto particleParams = std::make_shared<ParticleRenderParams>();
    RSRenderParticle rsRenderParticle(particleParams);
    bool la = rsRenderParticle.IsAlive();
    AnnulusRegion region;
    std::shared_ptr<AnnulusRegion> regionPtr = std::make_shared<AnnulusRegion>(region);
    rsRenderParticle.CalculateParticlePosition(emitShape_, position_, emitSize_, regionPtr);
    ASSERT_EQ(la, false);
}
} // namespace OHOS::Rosen
