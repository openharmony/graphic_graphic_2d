/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#include "params/rs_effect_render_params.h"
#include "limit_number.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSEffectRenderParamsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static void DisplayTestInfo();
};

void RSEffectRenderParamsTest::SetUpTestCase() {}
void RSEffectRenderParamsTest::TearDownTestCase() {}
void RSEffectRenderParamsTest::SetUp() {}
void RSEffectRenderParamsTest::TearDown() {}
void RSEffectRenderParamsTest::DisplayTestInfo()
{
    return;
}

/**
 * @tc.name: OnSync001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectRenderParamsTest, OnSync001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[2];
    std::unique_ptr<RSRenderParams> target = nullptr;
    RSEffectRenderParams params(id);
    params.OnSync(target);
    EXPECT_FALSE(params.cacheValid_);
    EXPECT_FALSE(params.hasEffectChildren_);
}

/**
 * @tc.name: OnSync002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectRenderParamsTest, OnSync002, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[2];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSEffectRenderParams>(id);
    auto targetEffectRenderParam = static_cast<RSEffectRenderParams*>(target.get());
    RSEffectRenderParams params(id);
    params.cacheValid_ = true;
    params.hasEffectChildren_ = true;
    params.OnSync(target);
    EXPECT_EQ(params.cacheValid_, targetEffectRenderParam->cacheValid_);
    EXPECT_EQ(params.hasEffectChildren_, targetEffectRenderParam->hasEffectChildren_);
}

/**
 * @tc.name: SetCacheValid001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectRenderParamsTest, SetCacheValid001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[3];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSEffectRenderParams>(id);
    RSEffectRenderParams params(id);
    params.cacheValid_ = true;
    params.SetCacheValid(true);
    EXPECT_TRUE(params.GetCacheValid());
}

/**
 * @tc.name: SetHasEffectChildren001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectRenderParamsTest, SetHasEffectChildren001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSEffectRenderParams>(id);
    RSEffectRenderParams params(id);
    params.hasEffectChildren_ = true;
    params.SetHasEffectChildren(true);
    EXPECT_TRUE(params.GetHasEffectChildren());
}

/**
 * @tc.name: SetEffectIntersectWithDRM001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectRenderParamsTest, SetEffectIntersectWithDRM001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[5];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSEffectRenderParams>(id);
    RSEffectRenderParams params(id);
    params.isIntersectWithDRM_ = false;
    EXPECT_FALSE(params.GetEffectIntersectWithDRM());
    params.SetEffectIntersectWithDRM(true);
    EXPECT_TRUE(params.GetEffectIntersectWithDRM());
}
} // namespace OHOS::Rosen