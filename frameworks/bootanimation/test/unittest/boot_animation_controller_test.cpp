/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "boot_animation_controller.h"
#include <if_system_ability_manager.h>
#include <iservice_registry.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class BootAnimationControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void BootAnimationControllerTest::SetUpTestCase() {}
void BootAnimationControllerTest::TearDownTestCase() {}
void BootAnimationControllerTest::SetUp() {}
void BootAnimationControllerTest::TearDown() {}

/**
 * @tc.name: BootAnimationControllerTest_001
 * @tc.desc: Verify the CreateDefaultBootConfig
 * @tc.type:FUNC
 */
HWTEST_F(BootAnimationControllerTest, BootAnimationControllerTest_001, TestSize.Level1)
{
    BootAnimationController controller;
    controller.CreateDefaultBootConfig();
    EXPECT_EQ(1, controller.animationConfigs_.size());
}

/**
 * @tc.name: BootAnimationControllerTest_002
 * @tc.desc: Verify the GetBootType
 * @tc.type:FUNC
 */
HWTEST_F(BootAnimationControllerTest, BootAnimationControllerTest_002, TestSize.Level0)
{
    BootAnimationController controller;
    controller.isCompatible_ = true;
    BootStrategyType type = controller.GetBootType();
    EXPECT_EQ(type, BootStrategyType::COMPATIBLE);
}

/**
 * @tc.name: BootAnimationControllerTest_003
 * @tc.desc: Verify the GetBootType
 * @tc.type:FUNC
 */
HWTEST_F(BootAnimationControllerTest, BootAnimationControllerTest_003, TestSize.Level0)
{
    BootAnimationController controller;
    controller.isMultiDisplay_ = true;
    BootStrategyType type = controller.GetBootType();
    EXPECT_EQ(type, BootStrategyType::INDEPENDENT);
}

/**
 * @tc.name: BootAnimationControllerTest_004
 * @tc.desc: Verify the GetBootType
 * @tc.type:FUNC
 */
HWTEST_F(BootAnimationControllerTest, BootAnimationControllerTest_004, TestSize.Level0)
{
    BootAnimationController controller;
    BootAnimationConfig config;
    controller.animationConfigs_.emplace_back(config);
    BootStrategyType type = controller.GetBootType();
    EXPECT_EQ(type, BootStrategyType::INDEPENDENT);
}

/**
 * @tc.name: BootAnimationControllerTest_005
 * @tc.desc: Verify the GetBootType
 * @tc.type:FUNC
 */
HWTEST_F(BootAnimationControllerTest, BootAnimationControllerTest_005, TestSize.Level0)
{
    BootAnimationController controller;
    BootAnimationConfig first_config;
    controller.animationConfigs_.emplace_back(first_config);
    BootAnimationConfig second_config;
    controller.animationConfigs_.emplace_back(second_config);
    BootStrategyType type = controller.GetBootType();
    EXPECT_EQ(type, BootStrategyType::ASSOCIATIVE);
}

/**
 * @tc.name: BootAnimationControllerTest_006
 * @tc.desc: Verify the GetBootType
 * @tc.type:FUNC
 */
HWTEST_F(BootAnimationControllerTest, BootAnimationControllerTest_006, TestSize.Level1)
{
    std::shared_ptr<BootAnimationController> controller = std::make_shared<BootAnimationController>();
    std::string path = controller->GetConfigFilePath();
    std::string filePath = "/sys_prod/etc/bootanimation/bootanimation_custom_config.json";
    bool isFileExist = OHOS::IsFileExisted(filePath);
    EXPECT_EQ(path.empty(), isFileExist ? false : true);
}

/**
 * @tc.name: BootAnimationControllerTest_007
 * @tc.desc: Verify the WaitRenderServiceInit
 * @tc.type:FUNC
 */
HWTEST_F(BootAnimationControllerTest, BootAnimationControllerTest_007, TestSize.Level1)
{
    std::shared_ptr<BootAnimationController> controller = std::make_shared<BootAnimationController>();
    controller->WaitRenderServiceInit();
    sptr<ISystemAbilityManager> saMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_TRUE(saMgr);
}
}
