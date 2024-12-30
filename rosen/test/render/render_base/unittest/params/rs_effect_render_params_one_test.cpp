/*
 * Copyright (c) 2024-2024 Huawei Device Co., Ltd.
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
class RSEffectRenderParamsOneTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static void DisplayTestInfo();
};

void RSEffectRenderParamsOneTest::SetUpTestCase() {}
void RSEffectRenderParamsOneTest::TearDownTestCase() {}
void RSEffectRenderParamsOneTest::SetUp() {}
void RSEffectRenderParamsOneTest::TearDown() {}
void RSEffectRenderParamsOneTest::DisplayTestInfo()
{
    return;
}

/**
 * @tc.name: OnSyncTest001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require: issuesIB7RWT
 */
HWTEST_F(RSEffectRenderParamsOneTest, OnSyncTest001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[2];
    std::unique_ptr<RSRenderParams> target = nullptr;
    RSEffectRenderParams params(id);
    params.OnSync(target);
    EXPECT_FALSE(params.cacheValid_);
    EXPECT_FALSE(params.hasEffectChildren_);
}

/**
 * @tc.name: SetHasEffectChildrenTest001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require: issuesIB7RWT
 */
HWTEST_F(RSEffectRenderParamsOneTest, SetHasEffectChildrenTest001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSEffectRenderParams>(id);
    RSEffectRenderParams params(id);
    params.hasEffectChildren_ = true;
    params.SetHasEffectChildren(true);
    EXPECT_TRUE(params.GetHasEffectChildren());
}

/**
 * @tc.name: SetCacheValidTest001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require: issuesIB7RWT
 */
HWTEST_F(RSEffectRenderParamsOneTest, SetCacheValidTest001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[3];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSEffectRenderParams>(id);
    RSEffectRenderParams params(id);
    params.cacheValid_ = true;
    params.SetCacheValid(true);
    EXPECT_TRUE(params.GetCacheValid());
}
} // namespace OHOS::Rosen