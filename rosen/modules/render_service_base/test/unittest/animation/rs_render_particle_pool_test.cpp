/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <cmath>
#include <vector>

#include "gtest/gtest.h"
#include "animation/rs_render_particle_pool.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSRenderParticlePoolTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderParticlePoolTest::SetUpTestCase() {}
void RSRenderParticlePoolTest::TearDownTestCase() {}
void RSRenderParticlePoolTest::SetUp() {}
void RSRenderParticlePoolTest::TearDown() {}

/**
 * @tc.name: Pool_Acquire001
 * @tc.desc: Test Acquire grows the pool, returns consecutive slots and clears their rows
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticlePoolTest, Pool_Acquire001, TestSize.Level1)
{
    RSRenderParticlePool pool;
    pool.Resize(8);
    EXPECT_EQ(pool.Capacity(), 8);
    EXPECT_EQ(pool.Count(), 0);

    const size_t first = pool.Acquire();
    const size_t second = pool.Acquire();
    EXPECT_EQ(first, 0);
    EXPECT_EQ(second, 1);
    EXPECT_EQ(pool.Count(), 2);
    EXPECT_FALSE(pool.IsAlive(first)); // a slot without a life time is not alive, as in the baseline
    pool.SetLifeTime(first, NS_TO_S);
    EXPECT_TRUE(pool.IsAlive(first));
    EXPECT_FLOAT_EQ(pool.GetScale(first), 1.f);
    EXPECT_FLOAT_EQ(pool.GetOpacity(first), 1.f);
    EXPECT_FLOAT_EQ(static_cast<float>(pool.GetColor(first).GetAlpha()), 255.f);
}

/**
 * @tc.name: Pool_Append001
 * @tc.desc: Test Append writes the kinematic rows
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticlePoolTest, Pool_Append001, TestSize.Level1)
{
    RSRenderParticlePool pool;
    const size_t slot = pool.Acquire();
    pool.SetPosition(slot, Vector2f(1.f, 2.f));
    pool.SetVelocity(slot, Vector2f(3.f, 4.f));
    pool.SetAcceleration(slot, Vector2f(0.5f, 0.25f));
    pool.SetLifeTime(slot, static_cast<int64_t>(10.f * NS_TO_S));
    EXPECT_EQ(slot, 0);
    EXPECT_FLOAT_EQ(pool.GetPosition(slot).x_, 1.f);
    EXPECT_FLOAT_EQ(pool.GetPosition(slot).y_, 2.f);
    EXPECT_FLOAT_EQ(pool.GetVelocity(slot).x_, 3.f);
    EXPECT_FLOAT_EQ(pool.GetVelocity(slot).y_, 4.f);
    EXPECT_FLOAT_EQ(pool.GetAcceleration(slot).x_, 0.5f);
    EXPECT_FLOAT_EQ(pool.GetAcceleration(slot).y_, 0.25f);
    EXPECT_EQ(pool.GetLifeTime(slot), static_cast<int64_t>(10.f * NS_TO_S));
}

/**
 * @tc.name: Pool_Update001
 * @tc.desc: Test Update integrates acceleration into velocity and velocity into position
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticlePoolTest, Pool_Update001, TestSize.Level1)
{
    RSRenderParticlePool pool;
    pool.Resize(16);
    for (size_t i = 0; i < 16; i++) {
        const float seed = static_cast<float>(i);
        const size_t slot = pool.Acquire();
        pool.SetPosition(slot, Vector2f(seed, -seed));
        pool.SetVelocity(slot, Vector2f(1.f + seed, 2.f - seed));
        pool.SetAcceleration(slot, Vector2f(0.5f, -0.25f));
        pool.SetLifeTime(slot, static_cast<int64_t>(100.f * NS_TO_S));
    }
    const float dt = 0.016f;
    pool.Update(dt);

    for (size_t i = 0; i < 16; i++) {
        const float seed = static_cast<float>(i);
        const float vx = (1.f + seed) + 0.5f * dt;
        const float vy = (2.f - seed) + (-0.25f) * dt;
        const float px = seed + vx * dt;
        const float py = -seed + vy * dt;
        EXPECT_NEAR(pool.GetVelocity(i).x_, vx, 1e-4f);
        EXPECT_NEAR(pool.GetVelocity(i).y_, vy, 1e-4f);
        EXPECT_NEAR(pool.GetPosition(i).x_, px, 1e-4f);
        EXPECT_NEAR(pool.GetPosition(i).y_, py, 1e-4f);
    }
}

/**
 * @tc.name: Pool_Force001
 * @tc.desc: Test the field force moves the position without changing the velocity
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticlePoolTest, Pool_Force001, TestSize.Level1)
{
    RSRenderParticlePool pool;
    const size_t slot = pool.Acquire();
    pool.SetPosition(slot, Vector2f(0.f, 0.f));
    pool.SetVelocity(slot, Vector2f(0.f, 0.f));
    pool.SetAcceleration(slot, Vector2f(0.f, 0.f));
    pool.SetLifeTime(slot, static_cast<int64_t>(100.f * NS_TO_S));
    pool.Data().forceX[slot] = 1.f;
    const float dt = 0.1f;
    pool.Update(dt);
    EXPECT_NEAR(pool.GetPosition(slot).x_, dt, 1e-5f);
    EXPECT_NEAR(pool.GetVelocity(slot).x_, 0.f, 1e-5f);
    EXPECT_FLOAT_EQ(pool.GetPosition(slot).y_, 0.f);
}

/**
 * @tc.name: Pool_Expire001
 * @tc.desc: Test a slot older than its life time dies and HasDead reports it
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticlePoolTest, Pool_Expire001, TestSize.Level1)
{
    RSRenderParticlePool pool;
    const size_t slot = pool.Acquire();
    pool.SetPosition(slot, Vector2f());
    pool.SetVelocity(slot, Vector2f());
    pool.SetAcceleration(slot, Vector2f());
    pool.SetLifeTime(slot, static_cast<int64_t>(0.05f * NS_TO_S));
    ASSERT_TRUE(pool.IsAlive(slot));
    pool.Update(0.1f);
    EXPECT_FALSE(pool.IsAlive(slot));
    EXPECT_TRUE(pool.HasDead());
}

/**
 * @tc.name: Pool_Compact001
 * @tc.desc: Test Compact swap-removes the dead slots and keeps the survivors contiguous
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticlePoolTest, Pool_Compact001, TestSize.Level1)
{
    RSRenderParticlePool pool;
    for (size_t i = 0; i < 4; i++) {
        const size_t slot = pool.Acquire();
        pool.SetPosition(slot, Vector2f(static_cast<float>(i), 0.f));
        pool.SetVelocity(slot, Vector2f());
        pool.SetAcceleration(slot, Vector2f());
        pool.SetLifeTime(slot, static_cast<int64_t>(100.f * NS_TO_S));
        EXPECT_EQ(slot, i);
    }
    pool.MarkDead(1);
    pool.MarkDead(3);
    pool.Compact();
    EXPECT_EQ(pool.Count(), 2);
    EXPECT_FALSE(pool.HasDead());
    EXPECT_FLOAT_EQ(pool.GetPosition(0).x_, 0.f);
    EXPECT_FLOAT_EQ(pool.GetPosition(1).x_, 2.f);
}

/**
 * @tc.name: Pool_Clear001
 * @tc.desc: Test Clear drops every slot
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticlePoolTest, Pool_Clear001, TestSize.Level1)
{
    RSRenderParticlePool pool;
    for (size_t i = 0; i < 2; i++) {
        const size_t slot = pool.Acquire();
        pool.SetPosition(slot, Vector2f());
        pool.SetVelocity(slot, Vector2f());
        pool.SetAcceleration(slot, Vector2f());
        pool.SetLifeTime(slot, static_cast<int64_t>(10.f * NS_TO_S));
    }
    pool.Clear();
    EXPECT_EQ(pool.Count(), 0);
    EXPECT_FALSE(pool.HasDead());
    EXPECT_EQ(pool.Acquire(), 0);
}

/**
 * @tc.name: Pool_Capacity001
 * @tc.desc: Test the pool grows past its initial capacity
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderParticlePoolTest, Pool_Capacity001, TestSize.Level1)
{
    RSRenderParticlePool pool;
    constexpr size_t extra = 40;
    const size_t target = pool.Capacity() + extra;
    for (size_t i = 0; i < target; i++) {
        pool.Acquire();
    }
    EXPECT_EQ(pool.Count(), target);
    EXPECT_GE(pool.Capacity(), target);
}

} // namespace Rosen
} // namespace OHOS
