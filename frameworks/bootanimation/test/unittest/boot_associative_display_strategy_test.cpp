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
 
#include <gtest/gtest.h>
 
#include "boot_associative_display_strategy.h"
#include <parameters.h>
 
using namespace testing;
using namespace testing::ext;
 
namespace OHOS::Rosen {
class BootAssociativeDisplayStrategyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};
 
void BootAssociativeDisplayStrategyTest::SetUpTestCase() {}
void BootAssociativeDisplayStrategyTest::TearDownTestCase() {}
void BootAssociativeDisplayStrategyTest::SetUp() {}
void BootAssociativeDisplayStrategyTest::TearDown() {}
 
/**
 * @tc.name: BootAssociativeDisplayStrategyTest_001
 * @tc.desc: Verify the IsExtraVideoExist
 * @tc.type:FUNC
 */
HWTEST_F(BootAssociativeDisplayStrategyTest, BootAssociativeDisplayStrategyTest_001, TestSize.Level1)
{
    std::shared_ptr<BootAssociativeDisplayStrategy> strategy = std::make_shared<BootAssociativeDisplayStrategy>();
    std::vector<BootAnimationConfig> animationConfigs;
    BootAnimationConfig config1;
    config1.screenId = 0;
    animationConfigs.emplace_back(config1);
    bool result = strategy->IsExtraVideoExist(animationConfigs);
    EXPECT_FALSE(result);
    BootAnimationConfig config2;
    config2.videoExtPath.emplace("abc", "abc");
    animationConfigs.emplace_back(config2);
    result = strategy->IsExtraVideoExist(animationConfigs);
    EXPECT_TRUE(result);
}
 
/**
 * @tc.name: BootAssociativeDisplayStrategyTest_002
 * @tc.desc: Verify the IsSupportCoordination
 * @tc.type:FUNC
 */
HWTEST_F(BootAssociativeDisplayStrategyTest, BootAssociativeDisplayStrategyTest_002, TestSize.Level1)
{
    std::shared_ptr<BootAssociativeDisplayStrategy> strategy = std::make_shared<BootAssociativeDisplayStrategy>();
    bool result = strategy->IsSupportCoordination();
    EXPECT_TRUE(result);
}
}