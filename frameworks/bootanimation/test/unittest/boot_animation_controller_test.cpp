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
 * @tc.name: CreateDefaultBootConfig_Normal_Success
 * @tc.desc: Verify the CreateDefaultBootConfig function creates default config successfully.
 * @tc.type: FUNC
 */
HWTEST_F(BootAnimationControllerTest, CreateDefaultBootConfig_Normal_Success, TestSize.Level1)
{
    BootAnimationController controller;
    controller.CreateDefaultBootConfig();
    EXPECT_EQ(controller.animationConfigs_.size(), 1);
}

/**
 * @tc.name: GetBootType_CompatibleMode_ReturnCompatibleType
 * @tc.desc: Verify the GetBootType function returns COMPATIBLE type when isCompatible_ is true.
 * @tc.type: FUNC
 */
HWTEST_F(BootAnimationControllerTest, GetBootType_CompatibleMode_ReturnCompatibleType, TestSize.Level0)
{
    BootAnimationController controller;
    controller.isCompatible_ = true;
    BootStrategyType type = controller.GetBootType();
    EXPECT_EQ(type, BootStrategyType::COMPATIBLE);
}

/**
 * @tc.name: GetBootType_MultiDisplay_ReturnIndependentType
 * @tc.desc: Verify the GetBootType function returns INDEPENDENT type when isMultiDisplay_ is true.
 * @tc.type: FUNC
 */
HWTEST_F(BootAnimationControllerTest, GetBootType_MultiDisplay_ReturnIndependentType, TestSize.Level0)
{
    BootAnimationController controller;
    controller.isMultiDisplay_ = true;
    BootStrategyType type = controller.GetBootType();
    EXPECT_EQ(type, BootStrategyType::INDEPENDENT);
}

/**
 * @tc.name: GetBootType_SingleConfig_ReturnIndependentType
 * @tc.desc: Verify the GetBootType function returns INDEPENDENT type when there is single animation config.
 * @tc.type: FUNC
 */
HWTEST_F(BootAnimationControllerTest, GetBootType_SingleConfig_ReturnIndependentType, TestSize.Level0)
{
    BootAnimationController controller;
    BootAnimationConfig config;
    controller.animationConfigs_.emplace_back(config);
    BootStrategyType type = controller.GetBootType();
    EXPECT_EQ(type, BootStrategyType::INDEPENDENT);
}

/**
 * @tc.name: GetBootType_MultipleConfigs_ReturnAssociativeType
 * @tc.desc: Verify the GetBootType function returns ASSOCIATIVE type when there are multiple animation configs.
 * @tc.type: FUNC
 */
HWTEST_F(BootAnimationControllerTest, GetBootType_MultipleConfigs_ReturnAssociativeType, TestSize.Level0)
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
 * @tc.name: GetConfigFilePath_FileExists_ReturnCorrectPath
 * @tc.desc: Verify the GetConfigFilePath function returns correct path when config file exists.
 * @tc.type: FUNC
 */
HWTEST_F(BootAnimationControllerTest, GetConfigFilePath_FileExists_ReturnCorrectPath, TestSize.Level1)
{
    std::shared_ptr<BootAnimationController> controller = std::make_shared<BootAnimationController>();
    std::string path = controller->GetConfigFilePath();
    std::string filePath = "/sys_prod/etc/bootanimation/bootanimation_custom_config.json";
    bool isFileExist = OHOS::IsFileExisted(filePath);
    EXPECT_EQ(path.empty(), isFileExist ? false : true);
}
}
