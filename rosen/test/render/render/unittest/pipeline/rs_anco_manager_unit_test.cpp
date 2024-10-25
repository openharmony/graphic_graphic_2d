/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"
#include "parameters.h"
#include "pipeline/rs_anco_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RsAncoManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsAncoManagerTest::SetUpTestCase() {}
void RsAncoManagerTest::TearDownTestCase() {}
void RsAncoManagerTest::SetUp() {}
void RsAncoManagerTest::TearDown() {}

/**
 * @tc.name: SetAncoHebcStatus
 * @tc.desc: test SetAncoHebcStatus
 * @tc.type: FUNC
 * @tc.require: issueIARZ3Q
 */
HWTEST_F(RsAncoManagerTest, SetAncoHebcStatus01, TestSize.Level2)
{
    auto ancoManager = RSAncoManager::Instance();
    ASSERT_NE(ancoManager, nullptr);
    ancoManager->SetAncoHebcStatus(AncoHebcStatus::USE_HEBC);
    ASSERT_EQ(ancoManager->GetAncoHebcStatus(), AncoHebcStatus::USE_HEBC);
}

/**
 * @tc.name: AncoOptimizeDisplayNode
 * @tc.desc: test AncoOptimizeDisplayNode
 * @tc.type: FUNC
 * @tc.require: issueIARZ3Q
 */
HWTEST_F(RsAncoManagerTest, AncoOptimizeDisplayNode01, TestSize.Level2)
{
    auto ancoManager = RSAncoManager::Instance();
    ASSERT_NE(ancoManager, nullptr);
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> hardwareEnabledNodes;
    std::shared_ptr<RSSurfaceHandler> surfaceHandler = std::make_shared<RSSurfaceHandler>(0);

    RSSurfaceRenderNode::SetAncoForceDoDirect(false);
    ASSERT_EQ(ancoManager->AncoOptimizeDisplayNode(surfaceHandler, hardwareEnabledNodes,
        ScreenRotation::ROTATION_0, 0, 0), false);
    
    RSSurfaceRenderNode::SetAncoForceDoDirect(true);
    auto deviceTypeStr = system::GetParameter("const.product.devicetype", "phone");
    system::SetParameter("const.product.devicetype", "phone");
    ASSERT_EQ(ancoManager->AncoOptimizeDisplayNode(surfaceHandler, hardwareEnabledNodes,
        ScreenRotation::ROTATION_0, 0, 0), false);

    system::SetParameter("const.product.devicetype", "tablet");
    ASSERT_EQ(ancoManager->AncoOptimizeDisplayNode(surfaceHandler, hardwareEnabledNodes,
        ScreenRotation::ROTATION_90, 0, 0), false);

    surfaceHandler->buffer_.buffer = SurfaceBuffer::Create();
    ASSERT_NE(surfaceHandler->buffer_.buffer, nullptr);
    ASSERT_EQ(ancoManager->AncoOptimizeDisplayNode(surfaceHandler, hardwareEnabledNodes,
        ScreenRotation::ROTATION_90, 0, 0), false);
    ASSERT_EQ(ancoManager->AncoOptimizeDisplayNode(surfaceHandler, hardwareEnabledNodes,
        ScreenRotation::ROTATION_90, 10, 10), false);
    system::SetParameter("const.product.devicetype", deviceTypeStr);
}

/**
 * @tc.name: AncoOptimizeCheck
 * @tc.desc: test AncoOptimizeCheck
 * @tc.type: FUNC
 * @tc.require: issueIARZ3Q
 */
HWTEST_F(RsAncoManagerTest, AncoOptimizeCheck01, TestSize.Level2)
{
    auto ancoManager = RSAncoManager::Instance();
    ASSERT_NE(ancoManager, nullptr);
    ASSERT_EQ(ancoManager->AncoOptimizeCheck(true, 3, 2), true);
    ASSERT_EQ(ancoManager->AncoOptimizeCheck(true, 4, 2), false);
    ASSERT_EQ(ancoManager->AncoOptimizeCheck(false, 3, 2), false);
    ASSERT_EQ(ancoManager->AncoOptimizeCheck(false, 4, 2), true);
}

/**
 * @tc.name: GetAncoHebcStatus
 * @tc.desc: test GetAncoHebcStatus
 * @tc.type: FUNC
 * @tc.require: issueIARZ3Q
 */
HWTEST_F(RsAncoManagerTest, GetAncoHebcStatus01, TestSize.Level2)
{
    auto ancoManager = RSAncoManager::Instance();
    ASSERT_NE(ancoManager, nullptr);
    ancoManager->SetAncoHebcStatus(AncoHebcStatus::INITIAL);
    ASSERT_EQ(ancoManager->GetAncoHebcStatus(), AncoHebcStatus::INITIAL);
}

} // namespace OHOS::Rosen