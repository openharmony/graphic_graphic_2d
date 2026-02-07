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

#include "common/rs_common_def.h"
#include "common/rs_special_layer_manager.h"
#include "feature/special_layer/rs_special_layer_utils.h"
#include "gtest/gtest.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_render_node_map.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "screen_manager/rs_screen_property.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
NodeId GenerateNodeId()
{
    static NodeId nextId = 1;
    return nextId++;
}

ScreenId GenerateScreenId()
{
    static ScreenId nextId = 1;
    return nextId++;
}
}
class RSSpecialLayerUtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSpecialLayerUtilsTest::SetUpTestCase() {}
void RSSpecialLayerUtilsTest::TearDownTestCase() {}
void RSSpecialLayerUtilsTest::SetUp()
{
    ScreenSpecialLayerInfo::screenSpecialLayerInfoByNode_ = {};
    ScreenSpecialLayerInfo::SetGlobalBlackList({});
}
void RSSpecialLayerUtilsTest::TearDown() {}

/**
 * @tc.name: CheckCurrentTypeIntersectVisibleRect
 * @tc.desc: test CheckCurrentTypeIntersectVisibleRect with diverse node types
 * @tc.type: FUNC
 * @tc.require: issue21104
 */
HWTEST_F(RSSpecialLayerUtilsTest, CheckCurrentTypeIntersectVisibleRect, TestSize.Level1)
{
    NodeId nodeId = 1;
    RSSpecialLayerManager slManager;
    slManager.Set(SpecialLayerType::SKIP, true);
    int width = 100;
    int height = 100;
    RectI rect(0, 0, width, height);
    std::shared_ptr<RSSurfaceRenderNode> node1 = std::make_shared<RSSurfaceRenderNode>(nodeId++);
    ASSERT_NE(node1, nullptr);
    node1->absDrawRect_ = rect;
    node1->specialLayerManager_ = slManager;
    std::shared_ptr<RSSurfaceRenderNode> node2 = std::make_shared<RSSurfaceRenderNode>(nodeId++);
    ASSERT_NE(node2, nullptr);

    RSSurfaceRenderNodeConfig config = { .id = nodeId, .name = CAPTURE_WINDOW_NAME };
    std::shared_ptr<RSSurfaceRenderNode> node3 = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(node3, nullptr);
    slManager.Set(SpecialLayerType::SKIP, true);
    node3->specialLayerManager_ = slManager;
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(node1);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(node3);
    std::unordered_set<NodeId> nodeIds = {node1->GetId(), node2->GetId(), node3->GetId()};
    ASSERT_TRUE(RSSpecialLayerUtils::CheckCurrentTypeIntersectVisibleRect(nodeIds, SpecialLayerType::SKIP, rect));
    ASSERT_TRUE(RSSpecialLayerUtils::CheckCurrentTypeIntersectVisibleRect(nodeIds, SpecialLayerType::SECURITY, rect));
}

/**
 * @tc.name: CheckSpecialLayerIntersectMirrorDisplay
 * @tc.desc: test CheckSpecialLayerIntersectMirrorDisplay when enableVisibleRect is enabled
 * @tc.type: FUNC
 * @tc.require: issue21104
 */
HWTEST_F(RSSpecialLayerUtilsTest, CheckSpecialLayerIntersectMirrorDisplay, TestSize.Level1)
{
    NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    std::shared_ptr<RSLogicalDisplayRenderNode> mirrorNode =
        std::make_shared<RSLogicalDisplayRenderNode>(nodeId++, config);
    auto sourceNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    mirrorNode->isMirrorDisplay_ = true;

    mirrorNode->SetMirrorSource(sourceNode);
    RSSpecialLayerUtils::CheckSpecialLayerIntersectMirrorDisplay(*mirrorNode, *sourceNode);
    ASSERT_NE(mirrorNode->GetMirrorSource().lock(), nullptr);
}

/**
 * @tc.name: GetAllBlackList001
 * @tc.desc: test GetAllBlackList with empty nodeMap
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSSpecialLayerUtilsTest, GetAllBlackList001, TestSize.Level2)
{
    RSRenderNodeMap nodeMap;
    auto result = RSSpecialLayerUtils::GetAllBlackList(nodeMap);
    ASSERT_TRUE(result.empty());
}

/**
 * @tc.name: GetAllBlackList002
 * @tc.desc: test GetAllBlackList with screen nodes having black lists
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSSpecialLayerUtilsTest, GetAllBlackList002, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    auto& nodeMap = rsContext->GetMutableNodeMap();
    nodeMap.Initialize(rsContext->weak_from_this());

    // Create screen node with black list
    auto screenNode1 = std::make_shared<RSScreenRenderNode>(GenerateNodeId(), GenerateScreenId(), rsContext->weak_from_this());
    RSScreenProperty property;
    property.Set<ScreenPropertyType::BLACK_LIST>(std::unordered_set<NodeId>{GenerateNodeId()});
    screenNode1->SetScreenProperty(property);
    nodeMap.RegisterRenderNode(screenNode1);

    ScreenSpecialLayerInfo::SetGlobalBlackList(std::unordered_set<NodeId>{GenerateNodeId()});
    // Create screen node with black list and global blacklist enabled
    auto screenNode2 = std::make_shared<RSScreenRenderNode>(GenerateNodeId(), GenerateScreenId(), rsContext->weak_from_this());
    RSScreenProperty property2;
    property2.Set<ScreenPropertyType::BLACK_LIST>(std::unordered_set<NodeId>{GenerateNodeId()});
    property2.Set<ScreenPropertyType::ENABLE_SKIP_WINDOW>(true);
    screenNode2->SetScreenProperty(property2);
    nodeMap.RegisterRenderNode(screenNode2);

    auto result = RSSpecialLayerUtils::GetAllBlackList(nodeMap);
    ASSERT_FALSE(result.empty());

    // Clear
    ScreenSpecialLayerInfo::SetGlobalBlackList(std::unordered_set<NodeId>{});
}

/**
 * @tc.name: GetAllBlackList003
 * @tc.desc: test GetAllBlackList with null screen node
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSSpecialLayerUtilsTest, GetAllBlackList003, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    auto& nodeMap = rsContext->GetMutableNodeMap();
    nodeMap.Initialize(rsContext->weak_from_this());

    // Inject a null screen node to test null check branch
    NodeId nullNodeId = GenerateNodeId();
    nodeMap.screenNodeMap_[nullNodeId] = nullptr;

    auto result = RSSpecialLayerUtils::GetAllBlackList(nodeMap);
    ASSERT_TRUE(result.empty());
}

/**
 * @tc.name: GetAllWhiteList001
 * @tc.desc: test GetAllWhiteList with empty nodeMap
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSSpecialLayerUtilsTest, GetAllWhiteList001, TestSize.Level2)
{
    RSRenderNodeMap nodeMap;
    auto result = RSSpecialLayerUtils::GetAllWhiteList(nodeMap);
    ASSERT_TRUE(result.empty());
}

/**
 * @tc.name: GetAllWhiteList002
 * @tc.desc: test GetAllWhiteList with screen nodes having white lists
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSSpecialLayerUtilsTest, GetAllWhiteList002, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    auto& nodeMap = rsContext->GetMutableNodeMap();
    nodeMap.Initialize(rsContext->weak_from_this());

    // Create screen node with white list
    auto screenNode1 = std::make_shared<RSScreenRenderNode>(GenerateNodeId(), GenerateScreenId(), rsContext->weak_from_this());
    RSScreenProperty property;
    property.Set<ScreenPropertyType::WHITE_LIST>(std::unordered_set<NodeId>{GenerateNodeId()});
    screenNode1->SetScreenProperty(property);
    nodeMap.RegisterRenderNode(screenNode1);

    auto result = RSSpecialLayerUtils::GetAllWhiteList(nodeMap);
    ASSERT_FALSE(result.empty());
}

/**
 * @tc.name: GetAllWhiteList003
 * @tc.desc: test GetAllWhiteList with null screen node
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSSpecialLayerUtilsTest, GetAllWhiteList003, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    auto& nodeMap = rsContext->GetMutableNodeMap();
    nodeMap.Initialize(rsContext->weak_from_this());

    // Inject a null screen node to test null check branch
    NodeId nullNodeId = GenerateNodeId();
    nodeMap.screenNodeMap_[nullNodeId] = nullptr;

    auto result = RSSpecialLayerUtils::GetAllWhiteList(nodeMap);
    ASSERT_TRUE(result.empty());
}

/**
 * @tc.name: GetMergeBlackList001
 * @tc.desc: test GetMergeBlackList when EnableSkipWindow is false
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSSpecialLayerUtilsTest, GetMergeBlackList001, TestSize.Level2)
{
    RSScreenProperty property;
    property.Set<ScreenPropertyType::BLACK_LIST>(std::unordered_set<NodeId>{GenerateNodeId()});
    property.Set<ScreenPropertyType::ENABLE_SKIP_WINDOW>(false);

    auto result = RSSpecialLayerUtils::GetMergeBlackList(property);
    ASSERT_FALSE(result.empty());
}

/**
 * @tc.name: GetMergeBlackList002
 * @tc.desc: test GetMergeBlackList when EnableSkipWindow is true
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSSpecialLayerUtilsTest, GetMergeBlackList002, TestSize.Level2)
{
    RSScreenProperty property;
    property.Set<ScreenPropertyType::BLACK_LIST>(std::unordered_set<NodeId>{GenerateNodeId()});
    property.Set<ScreenPropertyType::ENABLE_SKIP_WINDOW>(true);

    ScreenSpecialLayerInfo::SetGlobalBlackList(std::unordered_set<NodeId>{GenerateNodeId()});
    auto result = RSSpecialLayerUtils::GetMergeBlackList(property);
    ASSERT_FALSE(result.empty());

    // Clean up
    ScreenSpecialLayerInfo::SetGlobalBlackList(std::unordered_set<NodeId>{});
}

/**
 * @tc.name: GetMergeBlackList003
 * @tc.desc: test GetMergeBlackList with empty black lists
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSSpecialLayerUtilsTest, GetMergeBlackList003, TestSize.Level2)
{
    RSScreenProperty property;
    property.Set<ScreenPropertyType::BLACK_LIST>(std::unordered_set<NodeId>{});
    property.Set<ScreenPropertyType::ENABLE_SKIP_WINDOW>(false);

    auto result = RSSpecialLayerUtils::GetMergeBlackList(property);
    ASSERT_TRUE(result.empty());
}

/**
 * @tc.name: GetMergeBlackList004
 * @tc.desc: test GetMergeBlackList with global blacklist but empty screen blacklist
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSSpecialLayerUtilsTest, GetMergeBlackList004, TestSize.Level2)
{
    RSScreenProperty property;
    property.Set<ScreenPropertyType::BLACK_LIST>(std::unordered_set<NodeId>{});
    property.Set<ScreenPropertyType::ENABLE_SKIP_WINDOW>(true);

    ScreenSpecialLayerInfo::SetGlobalBlackList(std::unordered_set<NodeId>{GenerateNodeId()});
    auto result = RSSpecialLayerUtils::GetMergeBlackList(property);
    ASSERT_FALSE(result.empty());

    // Clean up
    ScreenSpecialLayerInfo::SetGlobalBlackList(std::unordered_set<NodeId>{});
}

/**
 * @tc.name: UpdateInfoWithGlobalBlackList001
 * @tc.desc: test UpdateInfoWithGlobalBlackList with empty nodeMap
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSSpecialLayerUtilsTest, UpdateInfoWithGlobalBlackList001, TestSize.Level2)
{
    RSRenderNodeMap nodeMap;
    RSSpecialLayerUtils::UpdateInfoWithGlobalBlackList(nodeMap);
    ASSERT_TRUE(ScreenSpecialLayerInfo::screenSpecialLayerInfoByNode_.empty());
}

/**
 * @tc.name: UpdateInfoWithGlobalBlackList002
 * @tc.desc: test UpdateInfoWithGlobalBlackList with null screen node
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSSpecialLayerUtilsTest, UpdateInfoWithGlobalBlackList002, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    auto& nodeMap = rsContext->GetMutableNodeMap();
    nodeMap.Initialize(rsContext->weak_from_this());

    // Inject a null screen node to test null check branch
    NodeId nullNodeId = GenerateNodeId();
    nodeMap.screenNodeMap_[nullNodeId] = nullptr;

    RSSpecialLayerUtils::UpdateInfoWithGlobalBlackList(nodeMap);
    ASSERT_TRUE(ScreenSpecialLayerInfo::screenSpecialLayerInfoByNode_.empty());
}

/**
 * @tc.name: UpdateInfoWithGlobalBlackList003
 * @tc.desc: test UpdateInfoWithGlobalBlackList with screen node having EnableSkipWindow false
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSSpecialLayerUtilsTest, UpdateInfoWithGlobalBlackList003, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    auto& nodeMap = rsContext->GetMutableNodeMap();
    nodeMap.Initialize(rsContext->weak_from_this());

    auto screenId = GenerateScreenId();
    RSScreenProperty property;
    property.Set<ScreenPropertyType::BLACK_LIST>(std::unordered_set<NodeId>{GenerateNodeId()});
    property.Set<ScreenPropertyType::ENABLE_SKIP_WINDOW>(false);
    property.Set<ScreenPropertyType::ID>(screenId);
    auto screenNode = std::make_shared<RSScreenRenderNode>(GenerateNodeId(), screenId, rsContext->weak_from_this());
    screenNode->SetScreenProperty(property);
    nodeMap.RegisterRenderNode(screenNode);

    RSSpecialLayerUtils::UpdateInfoWithGlobalBlackList(nodeMap);
    ASSERT_TRUE(ScreenSpecialLayerInfo::screenSpecialLayerInfoByNode_.empty());
}

/**
 * @tc.name: UpdateInfoWithGlobalBlackList004
 * @tc.desc: test UpdateInfoWithGlobalBlackList with screen node having EnableSkipWindow true
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSSpecialLayerUtilsTest, UpdateInfoWithGlobalBlackList004, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    auto& nodeMap = rsContext->GetMutableNodeMap();
    nodeMap.Initialize(rsContext->weak_from_this());

    auto screenId = GenerateScreenId();
    RSScreenProperty screenProperty;
    screenProperty.Set<ScreenPropertyType::BLACK_LIST>(std::unordered_set<NodeId>{GenerateNodeId()});
    screenProperty.Set<ScreenPropertyType::ENABLE_SKIP_WINDOW>(true);
    screenProperty.Set<ScreenPropertyType::ID>(screenId);
    auto screenNode = std::make_shared<RSScreenRenderNode>(GenerateNodeId(), screenId, rsContext->weak_from_this());
    screenNode->SetScreenProperty(screenProperty);
    nodeMap.RegisterRenderNode(screenNode);

    RSSpecialLayerUtils::UpdateInfoWithGlobalBlackList(nodeMap);
    ASSERT_FALSE(ScreenSpecialLayerInfo::screenSpecialLayerInfoByNode_.empty());

    // Clean up
    ScreenSpecialLayerInfo::ClearByScreenId(screenId);
}

/**
 * @tc.name: DumpScreenSpecialLayer001
 * @tc.desc: test DumpScreenSpecialLayer with empty nodeIds
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSSpecialLayerUtilsTest, DumpScreenSpecialLayer001, TestSize.Level2)
{
    std::unordered_set<NodeId> nodeIds;
    RSSpecialLayerUtils::DumpScreenSpecialLayer(
        "TestFunc", SpecialLayerType::IS_BLACK_LIST, GenerateScreenId(), nodeIds);
}

/**
 * @tc.name: UpdateScreenSpecialLayer001
 * @tc.desc: test UpdateScreenSpecialLayer with basic properties
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSSpecialLayerUtilsTest, UpdateScreenSpecialLayer001, TestSize.Level2)
{
    auto screenId = GenerateScreenId();
    RSScreenProperty property;
    property.Set<ScreenPropertyType::BLACK_LIST>(std::unordered_set<NodeId>{GenerateNodeId()});
    property.Set<ScreenPropertyType::WHITE_LIST>(std::unordered_set<NodeId>{GenerateNodeId()});
    property.Set<ScreenPropertyType::ID>(screenId);

    RSSpecialLayerUtils::UpdateScreenSpecialLayer(property);
    ASSERT_FALSE(ScreenSpecialLayerInfo::screenSpecialLayerInfoByNode_.empty());

    // Clean up
    ScreenSpecialLayerInfo::ClearByScreenId(screenId);
}

/**
 * @tc.name: UpdateScreenSpecialLayer002
 * @tc.desc: test UpdateScreenSpecialLayer with ENABLE_SKIP_WINDOW type
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSSpecialLayerUtilsTest, UpdateScreenSpecialLayer002, TestSize.Level2)
{
    auto screenId = GenerateScreenId();
    RSScreenProperty property;
    property.Set<ScreenPropertyType::BLACK_LIST>(std::unordered_set<NodeId>{GenerateNodeId()});
    property.Set<ScreenPropertyType::ENABLE_SKIP_WINDOW>(true);
    property.Set<ScreenPropertyType::ID>(screenId);

    ScreenSpecialLayerInfo::SetGlobalBlackList(std::unordered_set<NodeId>{GenerateNodeId()});
    RSSpecialLayerUtils::UpdateScreenSpecialLayer(property, ScreenPropertyType::ENABLE_SKIP_WINDOW);
    ASSERT_FALSE(ScreenSpecialLayerInfo::screenSpecialLayerInfoByNode_.empty());

    // Clean up
    ScreenSpecialLayerInfo::ClearByScreenId(screenId);
    ScreenSpecialLayerInfo::SetGlobalBlackList(std::unordered_set<NodeId>{});
}

/**
 * @tc.name: UpdateScreenSpecialLayer003
 * @tc.desc: test UpdateScreenSpecialLayer with invalid type
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSSpecialLayerUtilsTest, UpdateScreenSpecialLayer003, TestSize.Level2)
{
    auto screenId = GenerateScreenId();
    RSScreenProperty property;
    property.Set<ScreenPropertyType::ID>(screenId);

    // Test with SCREEN_STATUS type which is not in SCREEN_SPECIAL_LAYER_PROPERTY
    RSSpecialLayerUtils::UpdateScreenSpecialLayer(property, ScreenPropertyType::SCREEN_STATUS);
    ASSERT_TRUE(ScreenSpecialLayerInfo::screenSpecialLayerInfoByNode_.empty());
}

/**
 * @tc.name: UpdateScreenSpecialLayersRecord001
 * @tc.desc: test UpdateScreenSpecialLayersRecord with needCalcScreenSpecialLayer false
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSSpecialLayerUtilsTest, UpdateScreenSpecialLayersRecord001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(GenerateNodeId(), rsContext->weak_from_this());
    RSDisplayNodeConfig displayConfig;
    displayConfig.screenId = GenerateScreenId();
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(GenerateNodeId(), displayConfig, rsContext->weak_from_this());

    // Update blacklist Info
    RSScreenProperty property;
    property.Set<ScreenPropertyType::BLACK_LIST>(std::unordered_set<NodeId>{surfaceNode->GetId()});
    ScreenId screenId = GenerateScreenId();
    property.Set<ScreenPropertyType::ID>(screenId);
    RSSpecialLayerUtils::UpdateScreenSpecialLayer(property);
    
    RSSpecialLayerUtils::UpdateScreenSpecialLayersRecord(*surfaceNode, *displayNode, false);
    ASSERT_FALSE(displayNode->GetSpecialLayerMgr().FindWithScreen(screenId, SpecialLayerType::HAS_BLACK_LIST));

    // Clean up
    ScreenSpecialLayerInfo::ClearByScreenId(screenId);
}

/**
 * @tc.name: UpdateScreenSpecialLayersRecord002
 * @tc.desc: test UpdateScreenSpecialLayersRecord with needCalcScreenSpecialLayer true
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSSpecialLayerUtilsTest, UpdateScreenSpecialLayersRecord002, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(GenerateNodeId(), rsContext->weak_from_this());
    RSDisplayNodeConfig displayConfig;
    displayConfig.screenId = GenerateScreenId();
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(GenerateNodeId(), displayConfig, rsContext->weak_from_this());

    // Update blacklist Info
    RSScreenProperty property;
    property.Set<ScreenPropertyType::BLACK_LIST>(std::unordered_set<NodeId>{surfaceNode->GetId()});
    ScreenId screenId = GenerateScreenId();
    property.Set<ScreenPropertyType::ID>(screenId);
    RSSpecialLayerUtils::UpdateScreenSpecialLayer(property);
    
    RSSpecialLayerUtils::UpdateScreenSpecialLayersRecord(*surfaceNode, *displayNode, true);
    ASSERT_TRUE(displayNode->GetSpecialLayerMgr().FindWithScreen(screenId, SpecialLayerType::HAS_BLACK_LIST));

    // Clean up
    ScreenSpecialLayerInfo::ClearByScreenId(screenId);
}

/**
 * @tc.name: UpdateSpecialLayersRecord001
 * @tc.desc: test UpdateSpecialLayersRecord with ShouldPaint false
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSSpecialLayerUtilsTest, UpdateSpecialLayersRecord001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    displayConfig.screenId = GenerateScreenId();
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(GenerateNodeId(), displayConfig, rsContext->weak_from_this());

    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(GenerateNodeId(), rsContext->weak_from_this());
    surfaceNode->SetSkipLayer(true);
    surfaceNode->shouldPaint_ = false;

    RSSpecialLayerUtils::UpdateSpecialLayersRecord(*surfaceNode, *displayNode);
    ASSERT_FALSE(displayNode->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SKIP));
}

/**
 * @tc.name: UpdateSpecialLayersRecord002
 * @tc.desc: test UpdateSpecialLayersRecord with ShouldPaint true
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSSpecialLayerUtilsTest, UpdateSpecialLayersRecord002, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    displayConfig.screenId = GenerateScreenId();
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(GenerateNodeId(), displayConfig, rsContext->weak_from_this());

    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(GenerateNodeId(), rsContext->weak_from_this());
    surfaceNode->SetSkipLayer(true);
    surfaceNode->shouldPaint_ = true;

    RSSpecialLayerUtils::UpdateSpecialLayersRecord(*surfaceNode, *displayNode);
    ASSERT_TRUE(displayNode->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SKIP));
}

/**
 * @tc.name: UpdateSpecialLayersRecord003
 * @tc.desc: test UpdateSpecialLayersRecord with HAS_SECURITY type
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSSpecialLayerUtilsTest, UpdateSpecialLayersRecord003, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    displayConfig.screenId = GenerateScreenId();
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(GenerateNodeId(), displayConfig, rsContext->weak_from_this());

    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(GenerateNodeId(), rsContext->weak_from_this());
    surfaceNode->SetSecurityLayer(true);
    surfaceNode->shouldPaint_ = true;

    RSSpecialLayerUtils::UpdateSpecialLayersRecord(*surfaceNode, *displayNode);
    ASSERT_TRUE(displayNode->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY));
}

/**
 * @tc.name: UpdateSpecialLayersRecord004
 * @tc.desc: test UpdateSpecialLayersRecord with capture window name
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSSpecialLayerUtilsTest, UpdateSpecialLayersRecord004, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    displayConfig.screenId = GenerateScreenId();
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(GenerateNodeId(), displayConfig, rsContext->weak_from_this());

    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(GenerateNodeId(), rsContext->weak_from_this());
    surfaceNode->SetSkipLayer(true);
    surfaceNode->shouldPaint_ = true;
    surfaceNode->name_ = CAPTURE_WINDOW_NAME;

    RSSpecialLayerUtils::UpdateSpecialLayersRecord(*surfaceNode, *displayNode);
    ASSERT_FALSE(displayNode->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SKIP));
}

/**
 * @tc.name: UpdateSpecialLayersRecord005
 * @tc.desc: test UpdateSpecialLayersRecord with special layer changed
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSSpecialLayerUtilsTest, UpdateSpecialLayersRecord005, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    displayConfig.screenId = GenerateScreenId();
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(GenerateNodeId(), displayConfig, rsContext->weak_from_this());

    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(GenerateNodeId(), rsContext->weak_from_this());
    surfaceNode->SetSkipLayer(true);
    surfaceNode->shouldPaint_ = true;
    surfaceNode->specialLayerChanged_ = true;

    RSSpecialLayerUtils::UpdateSpecialLayersRecord(*surfaceNode, *displayNode);
    ASSERT_TRUE(displayNode->displaySpecialSurfaceChanged_);
}

/**
 * @tc.name: DealWithSpecialLayer001
 * @tc.desc: test DealWithSpecialLayer with non-clone node
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSSpecialLayerUtilsTest, DealWithSpecialLayer001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    displayConfig.screenId = GenerateScreenId();
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(GenerateNodeId(), displayConfig, rsContext->weak_from_this());

    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(GenerateNodeId(), rsContext->weak_from_this());
    surfaceNode->isCloneCrossNode_ = false;
    surfaceNode->SetSkipLayer(true);
    surfaceNode->shouldPaint_ = true;

    RSSpecialLayerUtils::DealWithSpecialLayer(*surfaceNode, *displayNode, true);
    ASSERT_TRUE(displayNode->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SKIP));
}

/**
 * @tc.name: DealWithSpecialLayer002
 * @tc.desc: test DealWithSpecialLayer with clone node having valid source
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSSpecialLayerUtilsTest, DealWithSpecialLayer002, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    displayConfig.screenId = GenerateScreenId();
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(GenerateNodeId(), displayConfig, rsContext->weak_from_this());

    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(GenerateNodeId(), rsContext->weak_from_this());
    auto sourceNode = std::make_shared<RSSurfaceRenderNode>(GenerateNodeId(), rsContext->weak_from_this());
    surfaceNode->isCloneCrossNode_ = true;
    surfaceNode->sourceCrossNode_ = sourceNode;
    sourceNode->SetSkipLayer(true);
    sourceNode->shouldPaint_ = true;

    RSSpecialLayerUtils::DealWithSpecialLayer(*surfaceNode, *displayNode, true);
    ASSERT_TRUE(displayNode->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SKIP));
}

/**
 * @tc.name: DealWithSpecialLayer003
 * @tc.desc: test DealWithSpecialLayer with clone node having null source
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSSpecialLayerUtilsTest, DealWithSpecialLayer003, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    displayConfig.screenId = GenerateScreenId();
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(GenerateNodeId(), displayConfig, rsContext->weak_from_this());

    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(GenerateNodeId(), rsContext->weak_from_this());
    surfaceNode->isCloneCrossNode_ = true;
    surfaceNode->sourceCrossNode_ = std::weak_ptr<RSSurfaceRenderNode>();
    surfaceNode->SetSkipLayer(true);
    surfaceNode->shouldPaint_ = true;

    RSSpecialLayerUtils::DealWithSpecialLayer(*surfaceNode, *displayNode, true);
    ASSERT_FALSE(displayNode->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SKIP));
}
} // namespace Rosen

