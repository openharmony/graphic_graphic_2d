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

#include <memory>

#include "gtest/gtest.h"

#include "animation/rs_render_particle_pool.h"
#include "common/rs_vector2.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
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
 * @tc.desc: Verify an uninitialized slot never reports alive
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleTest, CalculateParticlePositionTest, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    const size_t slot = pool->Acquire();
    EXPECT_FALSE(pool->IsAlive(slot));
    AnnulusRegion region;
    std::shared_ptr<AnnulusRegion> regionPtr = std::make_shared<AnnulusRegion>(region);
    float positionX = 0.f;
    float positionY = 0.f;
    regionPtr->CalculatePosition(positionX, positionY);
    EXPECT_GE(positionX, 0.f);
}
} // namespace Rosen
} // namespace OHOS
