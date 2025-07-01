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

#include "feature/anco_manager/rs_anco_manager.h"
#include "feature/mock/mock_anco_manager.h"
#include "gtest/gtest.h"
#include "parameters.h"
#include "params/rs_surface_render_params.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSAncoManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSAncoManagerTest::SetUpTestCase() {}
void RSAncoManagerTest::TearDownTestCase() {}
void RSAncoManagerTest::SetUp() {}
void RSAncoManagerTest::TearDown() {}

/**
 * @tc.name: AncoOptimizeScreenNode
 * @tc.desc: test AncoOptimizeScreenNode
 * @tc.type: FUNC
 * @tc.require: issueIARZ3Q
 */
HWTEST_F(RSAncoManagerTest, IsAncoOptimize, TestSize.Level2)
{
    auto ancoManager = RSAncoManager::Instance();
    ASSERT_NE(ancoManager, nullptr);

    RSSurfaceRenderNode::SetAncoForceDoDirect(false);
    ASSERT_EQ(ancoManager->IsAncoOptimize(ScreenRotation::ROTATION_0), false);
}

/**
 * @tc.name: AncoOptimizeScreenNode
 * @tc.desc: test AncoOptimizeDisplayNode
 * @tc.type: FUNC
 * @tc.require: issueIARZ3Q
 */
HWTEST_F(RSAncoManagerTest, AncoOptimizeDisplayNode_01, TestSize.Level2)
{
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> hardwareEnabledNodes;
    std::shared_ptr<RSSurfaceHandler> surfaceHandler = nullptr;
    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create();

    sptr<SyncFence> fence = SyncFence::InvalidFence();

    std::unique_ptr<Mock::MockRSAncoManager> mock = std::make_unique<Mock::MockRSAncoManager>();
    EXPECT_CALL(*mock, IsAncoOptimize(_)).WillRepeatedly(testing::Return(true));

    ASSERT_EQ(mock->AncoOptimizeScreenNode(surfaceHandler, hardwareEnabledNodes,
        ScreenRotation::ROTATION_90, 0, 0), false);
}

/**
 * @tc.name: AncoOptimizeScreenNode
 * @tc.desc: test AncoOptimizeScreenNode
 * @tc.type: FUNC
 * @tc.require: issueIARZ3Q
 */
HWTEST_F(RSAncoManagerTest, AncoOptimizeScreenNode_02, TestSize.Level2)
{
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> hardwareEnabledNodes;
    std::shared_ptr<RSSurfaceHandler> surfaceHandler = std::make_shared<RSSurfaceHandler>(0);
    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create();

    sptr<SyncFence> fence = SyncFence::InvalidFence();
    Rect damage = {10, 10, 100, 100};
    int64_t timestamp = 0;
    surfaceHandler->SetBuffer(surfaceBuffer, fence, damage, timestamp);

    std::unique_ptr<Mock::MockRSAncoManager> mock = std::make_unique<Mock::MockRSAncoManager>();
    EXPECT_CALL(*mock, IsAncoOptimize(_)).WillRepeatedly(testing::Return(true));
    ASSERT_NE(surfaceHandler->GetBuffer(), nullptr);

    ASSERT_EQ(mock->AncoOptimizeScreenNode(surfaceHandler, hardwareEnabledNodes,
        ScreenRotation::ROTATION_90, 0, 0), false);
}

/**
 * @tc.name: AncoOptimizeScreenNode
 * @tc.desc: test AncoOptimizeScreenNode
 * @tc.type: FUNC
 * @tc.require: issueIARZ3Q
 */
HWTEST_F(RSAncoManagerTest, AncoOptimizeScreenNode_03, TestSize.Level2)
{
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> hardwareEnabledNodes;
    std::shared_ptr<RSSurfaceHandler> surfaceHandler = std::make_shared<RSSurfaceHandler>(0);
    ASSERT_NE(surfaceHandler, nullptr);
    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create();

    sptr<SyncFence> fence = SyncFence::InvalidFence();
    
    std::unique_ptr<Mock::MockRSAncoManager> mock = std::make_unique<Mock::MockRSAncoManager>();
    EXPECT_CALL(*mock, IsAncoOptimize(_)).WillRepeatedly(testing::Return(true));
    ASSERT_EQ(surfaceHandler->GetBuffer(), nullptr);

    ASSERT_EQ(mock->AncoOptimizeScreenNode(surfaceHandler, hardwareEnabledNodes,
        ScreenRotation::ROTATION_90, 1260, 2720), false);
}

/**
 * @tc.name: AncoOptimizeScreenNode
 * @tc.desc: test AncoOptimizeScreenNode
 * @tc.type: FUNC
 * @tc.require: issueIARZ3Q
 */
HWTEST_F(RSAncoManagerTest, AncoOptimizeScreenNode_04, TestSize.Level2)
{
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> hardwareEnabledNodes;
    std::shared_ptr<RSSurfaceHandler> surfaceHandler = std::make_shared<RSSurfaceHandler>(0);
    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create();

    sptr<SyncFence> fence = SyncFence::InvalidFence();
    Rect damage = {10, 10, 100, 100};
    int64_t timestamp = 0;
    surfaceHandler->SetBuffer(surfaceBuffer, fence, damage, timestamp);
    
    std::unique_ptr<Mock::MockRSAncoManager> mock = std::make_unique<Mock::MockRSAncoManager>();
    EXPECT_CALL(*mock, IsAncoOptimize(_)).WillRepeatedly(testing::Return(true));
    ASSERT_NE(surfaceHandler->GetBuffer(), nullptr);

    NodeId id = 1;
    RSSurfaceNodeType type = RSSurfaceNodeType::DEFAULT;
    RSSurfaceRenderNodeConfig config = { .id = id, .nodeType = type };
    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(config);
    surfaceNode1->SetAncoFlags(0);
    EXPECT_EQ(surfaceNode1->GetAncoFlags(), 0);

    auto surfaceNode2 = std::make_shared<RSSurfaceRenderNode>(config);
    EXPECT_NE(surfaceNode2, nullptr);
    surfaceNode2->SetAncoFlags(static_cast<uint32_t>(AncoFlags::IS_ANCO_NODE));
    surfaceNode2->SetGlobalAlpha(0.0f);
    surfaceNode2->GetRSSurfaceHandler()->SetBuffer(surfaceBuffer, fence, damage, timestamp);
    EXPECT_EQ(surfaceNode2->GetAncoFlags(), 1);

    auto surfaceNode3 = std::make_shared<RSSurfaceRenderNode>(config);
    surfaceNode3->SetAncoFlags(static_cast<uint32_t>(AncoFlags::IS_ANCO_NODE));
    surfaceNode3->SetGlobalAlpha(1.8f);
    surfaceNode3->GetRSSurfaceHandler()->SetBuffer(surfaceBuffer, fence, damage, timestamp);
    surfaceNode3->InitRenderParams();
    EXPECT_EQ(surfaceNode3->GetAncoFlags(), 1);

    hardwareEnabledNodes.push_back(surfaceNode1);
    hardwareEnabledNodes.push_back(surfaceNode2);
    hardwareEnabledNodes.push_back(surfaceNode3);

    ASSERT_EQ(mock->AncoOptimizeScreenNode(surfaceHandler, hardwareEnabledNodes,
              ScreenRotation::ROTATION_90, 1260, 2720), true);
}

/**
 * @tc.name: AncoOptimizeScreenNode
 * @tc.desc: test AncoOptimizeScreenNode
 * @tc.type: FUNC
 * @tc.require: issueIARZ3Q
 */
HWTEST_F(RSAncoManagerTest, AncoOptimizeScreenNode_05, TestSize.Level2)
{
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> hardwareEnabledNodes;
    std::shared_ptr<RSSurfaceHandler> surfaceHandler = std::make_shared<RSSurfaceHandler>(0);
    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create();

    sptr<SyncFence> fence = SyncFence::InvalidFence();
    Rect damage = {10, 10, 100, 100};
    int64_t timestamp = 0;
    surfaceHandler->SetBuffer(surfaceBuffer, fence, damage, timestamp);

    std::unique_ptr<Mock::MockRSAncoManager> mock = std::make_unique<Mock::MockRSAncoManager>();
    EXPECT_CALL(*mock, IsAncoOptimize(_)).WillRepeatedly(testing::Return(true));
    ASSERT_NE(surfaceHandler->GetBuffer(), nullptr);

    NodeId id = 1;
    RSSurfaceNodeType type = RSSurfaceNodeType::DEFAULT;
    RSSurfaceRenderNodeConfig config = { .id = id, .nodeType = type };
    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(config);
    surfaceNode1->SetAncoFlags(0);
    EXPECT_EQ(surfaceNode1->GetAncoFlags(), 0);

    auto surfaceNode2 = std::make_shared<RSSurfaceRenderNode>(config);
    EXPECT_NE(surfaceNode2, nullptr);
    surfaceNode2->SetAncoFlags(static_cast<uint32_t>(AncoFlags::IS_ANCO_NODE));
    surfaceNode2->SetGlobalAlpha(0.0f);
    surfaceNode2->GetRSSurfaceHandler()->SetBuffer(surfaceBuffer, fence, damage, timestamp);
    EXPECT_EQ(surfaceNode2->GetAncoFlags(), 1);

    auto surfaceNode3 = std::make_shared<RSSurfaceRenderNode>(config);
    surfaceNode3->SetAncoFlags(static_cast<uint32_t>(AncoFlags::IS_ANCO_NODE));
    surfaceNode3->SetGlobalAlpha(1.8f);
    surfaceNode3->GetRSSurfaceHandler()->SetBuffer(surfaceBuffer, fence, damage, timestamp);
    EXPECT_EQ(surfaceNode3->GetAncoFlags(), 1);

    hardwareEnabledNodes.push_back(surfaceNode1);
    hardwareEnabledNodes.push_back(surfaceNode2);
    hardwareEnabledNodes.push_back(surfaceNode3);

    ASSERT_EQ(mock->AncoOptimizeScreenNode(surfaceHandler, hardwareEnabledNodes,
              ScreenRotation::ROTATION_90, 1260, 2720), true);
}

/**
 * @tc.name: AncoOptimizeScreenNode
 * @tc.desc: test AncoOptimizeScreenNode
 * @tc.type: FUNC
 * @tc.require: issueIARZ3Q
 */
HWTEST_F(RSAncoManagerTest, AncoOptimizeScreenNode_06, TestSize.Level2)
{
    auto ancoManager = RSAncoManager::Instance();
    ASSERT_NE(ancoManager, nullptr);
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> hardwareEnabledNodes;
    std::shared_ptr<RSSurfaceHandler> surfaceHandler = nullptr;
    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create();

    sptr<SyncFence> fence = SyncFence::InvalidFence();
    ASSERT_EQ(ancoManager->AncoOptimizeScreenNode(surfaceHandler, hardwareEnabledNodes,
        ScreenRotation::ROTATION_90, 0, 0), false);
}

/**
 * @tc.name: SetAncoHebcStatus
 * @tc.desc: test SetAncoHebcStatus
 * @tc.type: FUNC
 * @tc.require: issueIARZ3Q
 */
HWTEST_F(RSAncoManagerTest, SetAncoHebcStatus, TestSize.Level2)
{
    auto hebc = system::GetParameter("persist.sys.graphic.anco.disableHebc", "0");
    system::SetParameter("persist.sys.graphic.anco.disableHebc", "1");
    auto ancoManager = RSAncoManager::Instance();
    ASSERT_NE(ancoManager, nullptr);
    ancoManager->SetAncoHebcStatus(AncoHebcStatus::INITIAL);
    ASSERT_EQ(ancoManager->GetAncoHebcStatus(), AncoHebcStatus::INITIAL);
    system::SetParameter("persist.sys.graphic.anco.disableHebc", hebc);
}

/**
 * @tc.name: GetAncoHebcStatus
 * @tc.desc: test GetAncoHebcStatus
 * @tc.type: FUNC
 * @tc.require: issueIARZ3Q
 */
HWTEST_F(RSAncoManagerTest, GetAncoHebcStatus, TestSize.Level2)
{
    auto ancoManager = RSAncoManager::Instance();
    ASSERT_NE(ancoManager, nullptr);
    ancoManager->SetAncoHebcStatus(AncoHebcStatus::INITIAL);
    ASSERT_EQ(ancoManager->GetAncoHebcStatus(), AncoHebcStatus::INITIAL);
}

/**
 * @tc.name: AncoOptimizeCheck
 * @tc.desc: test AncoOptimizeCheck
 * @tc.type: FUNC
 * @tc.require: issueIARZ3Q
 */
HWTEST_F(RSAncoManagerTest, AncoOptimizeCheck, TestSize.Level2)
{
    auto ancoManager = RSAncoManager::Instance();
    ASSERT_NE(ancoManager, nullptr);
    ASSERT_EQ(ancoManager->AncoOptimizeCheck(true, 3, 2), true);
    ASSERT_EQ(ancoManager->AncoOptimizeCheck(true, 4, 2), false);
    ASSERT_EQ(ancoManager->AncoOptimizeCheck(false, 3, 2), false);
    ASSERT_EQ(ancoManager->AncoOptimizeCheck(false, 4, 2), true);
}

/**
 * @tc.name: IsAncoSfv
 * @tc.desc: test IsAncoSfv
 * @tc.type: FUNC
 * @tc.require: issueICDOZS
 */
HWTEST_F(RSAncoManagerTest, IsAncoSfv, TestSize.Level2)
{
    uint32_t flag = 0;
    ASSERT_FALSE(RSAncoManager::IsAncoSfv(flag));
    flag = static_cast<uint32_t>(AncoFlags::IS_ANCO_NODE);
    ASSERT_FALSE(RSAncoManager::IsAncoSfv(flag));
    flag = static_cast<uint32_t>(AncoFlags::IS_ANCO_NODE) | static_cast<uint32_t>(AncoFlags::FORCE_REFRESH);
    ASSERT_FALSE(RSAncoManager::IsAncoSfv(flag));
    flag = static_cast<uint32_t>(AncoFlags::ANCO_SFV_NODE);
    ASSERT_TRUE(RSAncoManager::IsAncoSfv(flag));
    flag = static_cast<uint32_t>(AncoFlags::ANCO_SFV_NODE) | static_cast<uint32_t>(AncoFlags::FORCE_REFRESH);
    ASSERT_TRUE(RSAncoManager::IsAncoSfv(flag));
    flag = static_cast<uint32_t>(AncoFlags::ANCO_NATIVE_NODE);
    ASSERT_TRUE(RSAncoManager::IsAncoSfv(flag));
    flag = static_cast<uint32_t>(AncoFlags::ANCO_NATIVE_NODE) | static_cast<uint32_t>(AncoFlags::FORCE_REFRESH);
    ASSERT_TRUE(RSAncoManager::IsAncoSfv(flag));
}

/**
 * @tc.name: EnableCropRectForAnco_001
 * @tc.desc: test UpdateCropRectForAnco
 * @tc.type: FUNC
 * @tc.require: issueICDOZS
 */
HWTEST_F(RSAncoManagerTest, EnableCropRectForAnco_001, TestSize.Level2)
{
    uint32_t flag = 0;
    Rect cropRect{};
    Drawing::Rect srcRect{};
    RSAncoManager::UpdateCropRectForAnco(flag, cropRect, srcRect);
    flag = static_cast<uint32_t>(AncoFlags::ANCO_SFV_NODE);
    RSAncoManager::UpdateCropRectForAnco(flag, cropRect, srcRect);
    cropRect.w = 100;
    cropRect.h = 100;
    srcRect.right_ = 200;
    srcRect.bottom_ = 200;
    RSAncoManager::UpdateCropRectForAnco(flag, cropRect, srcRect);
    ASSERT_TRUE(srcRect == Drawing::Rect(0, 0, 100, 100));
    cropRect.w = 100;
    cropRect.h = 100;
    srcRect.right_ = 50;
    srcRect.bottom_ = 50;
    RSAncoManager::UpdateCropRectForAnco(flag, cropRect, srcRect);
    ASSERT_TRUE(srcRect == Drawing::Rect(0, 0, 50, 50));
}

/**
 * @tc.name: EnableCropRectForAnco_002
 * @tc.desc: test UpdateCropRectForAnco
 * @tc.type: FUNC
 * @tc.require: issueICDOZS
 */
HWTEST_F(RSAncoManagerTest, EnableCropRectForAnco_002, TestSize.Level2)
{
    uint32_t flag = 0;
    GraphicIRect cropRect{};
    Drawing::Rect srcRect{};
    RSAncoManager::UpdateCropRectForAnco(flag, cropRect, srcRect);
    flag = static_cast<uint32_t>(AncoFlags::ANCO_SFV_NODE);
    cropRect.w = -1;
    cropRect.h = -1;
    RSAncoManager::UpdateCropRectForAnco(flag, cropRect, srcRect);
    ASSERT_TRUE(srcRect == Drawing::Rect());
    cropRect.w = 100;
    cropRect.h = 100;
    srcRect.right_ = 50;
    srcRect.bottom_ = 50;
    RSAncoManager::UpdateCropRectForAnco(flag, cropRect, srcRect);
    ASSERT_TRUE(srcRect == Drawing::Rect(0, 0, 100, 100));
}

/**
 * @tc.name: UpdateLayerSrcRectForAnco
 * @tc.desc: test UpdateLayerSrcRectForAnco
 * @tc.type: FUNC
 * @tc.require: issueICDOZS
 */
HWTEST_F(RSAncoManagerTest, UpdateLayerSrcRectForAnco, TestSize.Level2)
{
    uint32_t flag = 0;
    GraphicIRect cropRect{};
    GraphicIRect srcRect{};
    RSAncoManager::UpdateLayerSrcRectForAnco(flag, cropRect, srcRect);
    flag = static_cast<uint32_t>(AncoFlags::ANCO_SFV_NODE);
    RSAncoManager::UpdateLayerSrcRectForAnco(flag, cropRect, srcRect);
    cropRect.w = 100;
    srcRect.w = 200;
    srcRect.h = 200;
    RSAncoManager::UpdateLayerSrcRectForAnco(flag, cropRect, srcRect);
    ASSERT_FALSE(srcRect == cropRect);
    cropRect.h = 100;
    RSAncoManager::UpdateLayerSrcRectForAnco(flag, cropRect, srcRect);
    ASSERT_TRUE(srcRect == cropRect);
}
} // namespace OHOS::Rosen