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
 * @tc.desc: test AncoOptimizesScreenNode
 * @tc.type: FUNC
 * @tc.require: issueIARZ3Q
 */
HWTEST_F(RSAncoManagerTest, AncoOptimizeScreenNode_01, TestSize.Level2)
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
 * @tc.name: UpdateCropRectForAnco_001
 * @tc.desc: test UpdateCropRectForAnco_001
 * @tc.type: FUNC
 * @tc.require: issueICPWO9
 */
HWTEST_F(RSAncoManagerTest, UpdateCropRectForAnco_001, TestSize.Level2)
{
    uint32_t flag = 0;
    Rect cropRect{ 0, 0, 100, 100 };
    Drawing::Rect srcRect{ 0, 0, 100, 100 };
    AncoBufferInfo ancoInfo{};
    RSAncoManager::UpdateCropRectForAnco(flag, cropRect, ancoInfo, srcRect);
    ASSERT_TRUE(srcRect == Drawing::Rect(0, 0, 100, 100));

    flag = static_cast<uint32_t>(AncoFlags::ANCO_SFV_NODE);
    cropRect.w = 50;
    cropRect.h = 50;
    RSAncoManager::UpdateCropRectForAnco(flag, cropRect, ancoInfo, srcRect);
    ASSERT_TRUE(srcRect == Drawing::Rect(0, 0, 50, 50));
    cropRect.w = 100;
    cropRect.h = 100;
    RSAncoManager::UpdateCropRectForAnco(flag, cropRect, ancoInfo, srcRect);
    ASSERT_TRUE(srcRect == Drawing::Rect(0, 0, 50, 50));

    ancoInfo = { 100, 100, GRAPHIC_PIXEL_FMT_BGRA_8888 };
    cropRect.w = 50;
    cropRect.h = 50;
    srcRect = Drawing::Rect(0, 0, 100, 100);
    RSAncoManager::UpdateCropRectForAnco(flag, cropRect, ancoInfo, srcRect);
    ASSERT_TRUE(srcRect == Drawing::Rect(0.5, 0.5, 49.5, 49.5));
    cropRect.w = 100;
    cropRect.h = 100;
    srcRect = Drawing::Rect(0, 0, 50, 50);
    RSAncoManager::UpdateCropRectForAnco(flag, cropRect, ancoInfo, srcRect);
    ASSERT_TRUE(srcRect == Drawing::Rect(0.5, 0.5, 49.5, 49.5));
}

/**
 * @tc.name: UpdateCropRectForAnco_002
 * @tc.desc: test UpdateCropRectForAnco_002
 * @tc.type: FUNC
 * @tc.require: issueICPWO9
 */
HWTEST_F(RSAncoManagerTest, UpdateCropRectForAnco_002, TestSize.Level2)
{
    uint32_t flag = 0;
    GraphicIRect cropRect{ 0, 0, 100, 100 };
    Drawing::Rect srcRect{ 0, 0, 100, 100 };
    AncoBufferInfo ancoInfo{};
    RSAncoManager::UpdateCropRectForAnco(flag, cropRect, ancoInfo, srcRect);
    ASSERT_TRUE(srcRect == Drawing::Rect(0, 0, 100, 100));

    flag = static_cast<uint32_t>(AncoFlags::ANCO_SFV_NODE);
    cropRect.w = 50;
    cropRect.h = 50;
    RSAncoManager::UpdateCropRectForAnco(flag, cropRect, ancoInfo, srcRect);
    ASSERT_TRUE(srcRect == Drawing::Rect(0, 0, 50, 50));
    cropRect.w = 100;
    cropRect.h = 100;
    RSAncoManager::UpdateCropRectForAnco(flag, cropRect, ancoInfo, srcRect);
    ASSERT_TRUE(srcRect == Drawing::Rect(0, 0, 100, 100));

    ancoInfo = { 100, 100, GRAPHIC_PIXEL_FMT_BUTT };
    cropRect.w = 50;
    cropRect.h = 50;
    RSAncoManager::UpdateCropRectForAnco(flag, cropRect, ancoInfo, srcRect);
    ASSERT_TRUE(srcRect == Drawing::Rect(1, 1, 49, 49));
}

/**
 * @tc.name: UpdateLayerSrcRectForAnco
 * @tc.desc: test UpdateLayerSrcRectForAnco
 * @tc.type: FUNC
 * @tc.require: issueICPWO9
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
    cropRect.h = 100;
    srcRect.w = 200;
    srcRect.h = 200;
    RSAncoManager::UpdateLayerSrcRectForAnco(flag, cropRect, srcRect);
    ASSERT_TRUE(srcRect == cropRect);
    cropRect.w = 300;
    cropRect.h = 300;
    GraphicIRect res = srcRect;
    RSAncoManager::UpdateLayerSrcRectForAnco(flag, cropRect, srcRect);
    ASSERT_TRUE(srcRect == res);
}

/**
 * @tc.name: ValidCropRect
 * @tc.desc: test ValidCropRect
 * @tc.type: FUNC
 * @tc.require: issueICPWO9
 */
HWTEST_F(RSAncoManagerTest, ValidCropRect, TestSize.Level2)
{
    GraphicIRect cropRect{ -1, -1, -1, -1 };
    ASSERT_FALSE(RSAncoManager::ValidCropRect(cropRect));
    cropRect.w = 1;
    ASSERT_FALSE(RSAncoManager::ValidCropRect(cropRect));
    cropRect.h = 1;
    ASSERT_FALSE(RSAncoManager::ValidCropRect(cropRect));
    cropRect.x = 1;
    ASSERT_FALSE(RSAncoManager::ValidCropRect(cropRect));
    cropRect.y = 1;
    ASSERT_TRUE(RSAncoManager::ValidCropRect(cropRect));
}

/**
 * @tc.name: IntersectCrop
 * @tc.desc: test IntersectCrop
 * @tc.type: FUNC
 * @tc.require: issueICPWO9
 */
HWTEST_F(RSAncoManagerTest, IntersectCrop, TestSize.Level2)
{
    GraphicIRect cropRect{ 200, 200, 50, 50 };
    GraphicIRect srcRect{ 0, 0, 100, 100 };
    GraphicIRect res = srcRect;
    RSAncoManager::IntersectCrop(cropRect, srcRect);
    ASSERT_TRUE(srcRect == res);
    cropRect.x = 0;
    RSAncoManager::IntersectCrop(cropRect, srcRect);
    ASSERT_TRUE(srcRect == res);
    cropRect.y = 0;
    RSAncoManager::IntersectCrop(cropRect, srcRect);
    ASSERT_TRUE(srcRect == cropRect);
}

/**
 * @tc.name: CalculateShrinkAmount
 * @tc.desc: test CalculateShrinkAmount
 * @tc.type: FUNC
 * @tc.require: issueICPWO9
 */
HWTEST_F(RSAncoManagerTest, CalculateShrinkAmount, TestSize.Level2)
{
    int32_t format = GRAPHIC_PIXEL_FMT_BUTT;
    float shrinkAmount = RSAncoManager::CalculateShrinkAmount(format);
    ASSERT_FLOAT_EQ(shrinkAmount, 1.0f);
    format = GRAPHIC_PIXEL_FMT_RGBA_8888;
    shrinkAmount = RSAncoManager::CalculateShrinkAmount(format);
    ASSERT_FLOAT_EQ(shrinkAmount, 0.5f);
}

/**
 * @tc.name: ShrinkAmountIfNeed
 * @tc.desc: test ShrinkAmountIfNeed
 * @tc.type: FUNC
 * @tc.require: issueICPWO9
 */
HWTEST_F(RSAncoManagerTest, ShrinkAmountIfNeed, TestSize.Level2)
{
    AncoBufferInfo ancoInfo;
    Drawing::Rect srcRect{ 0, 0, 100, 100 };
    RSAncoManager::ShrinkAmountIfNeed(ancoInfo, srcRect);
    ASSERT_TRUE(srcRect == Drawing::Rect(0, 0, 100, 100));
    ancoInfo = { 200, 200, GRAPHIC_PIXEL_FMT_BGRA_8888 };
    RSAncoManager::ShrinkAmountIfNeed(ancoInfo, srcRect);
    ASSERT_TRUE(srcRect == Drawing::Rect(0.5, 0.5, 99.5, 99.5));
    srcRect = Drawing::Rect(0, 0, 1, 1);
    RSAncoManager::ShrinkAmountIfNeed(ancoInfo, srcRect);
    ASSERT_TRUE(srcRect == Drawing::Rect(0, 0, 1, 1));

    ancoInfo.format_ = GRAPHIC_PIXEL_FMT_BUTT;
    srcRect = Drawing::Rect(0, 0, 100, 100);
    RSAncoManager::ShrinkAmountIfNeed(ancoInfo, srcRect);
    ASSERT_TRUE(srcRect == Drawing::Rect(1, 1, 99, 99));
    srcRect = Drawing::Rect(0, 0, 2, 2);
    RSAncoManager::ShrinkAmountIfNeed(ancoInfo, srcRect);
    ASSERT_TRUE(srcRect == Drawing::Rect(0, 0, 2, 2));
}
} // namespace OHOS::Rosen