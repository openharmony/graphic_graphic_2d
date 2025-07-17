/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "feature/composite_layer/rs_composite_layer_uitls.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_surface_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSCompositeLayerUtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSCompositeLayerUtilsTest::SetUpTestCase() {}
void RSCompositeLayerUtilsTest::TearDownTestCase() {}
void RSCompositeLayerUtilsTest::SetUp() {}
void RSCompositeLayerUtilsTest::TearDown() {}

/**
 * @tc.name: CreateCompositeLayerTest
 * @tc.desc: Test CreateCompositeLayer
 * @tc.type: FUNC
 */
HWTEST_F(RSCompositeLayerUtilsTest, CreateCompositeLayerTest, TestSize.Level1)
{
    std::shared_ptr<RSCanvasNode> node = nullptr;
    auto clUtils =
        std::make_shared<RSCompositeLayerUtils>(node, static_cast<uint32_t>(TopLayerZOrder::CHARGE_ACTION_TEXT));
    EXPECT_FALSE(clUtils->CreateCompositeLayer());

    node = RSCanvasNode::Create();
    clUtils->rootNode_ = node;
    EXPECT_FALSE(clUtils->CreateCompositeLayer());

    RSSurfaceNodeConfig config;
    auto surfaceNode = RSSurfaceNode::Create(config);
    clUtils->rootNode_ = surfaceNode;
    EXPECT_FALSE(clUtils->CreateCompositeLayer());
}

/**
 * @tc.name: UpdateVirtualNodeBoundsTest
 * @tc.desc: Test UpdateVirtualNodeBounds
 * @tc.type: FUNC
 */
HWTEST_F(RSCompositeLayerUtilsTest, UpdateVirtualNodeBoundsTest, TestSize.Level1)
{
    RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> node = RSSurfaceNode::Create(config);
    auto clUtils =
        std::make_shared<RSCompositeLayerUtils>(node, static_cast<uint32_t>(TopLayerZOrder::CHARGE_ACTION_TEXT));
    ASSERT_NE(clUtils, nullptr);
    clUtils->textureExport_ = nullptr;
    clUtils->UpdateVirtualNodeBounds({0, 0, 0, 0});
    clUtils->textureExport_ = std::make_shared<RSTextureExport>(node, 0);
    clUtils->textureExport_->virtualRootNode_ = RSSurfaceNode::Create(config);
    clUtils->UpdateVirtualNodeBounds({0, 0, 0, 0});

    clUtils->compositeNode_ = node;
    clUtils->UpdateVirtualNodeBounds({0, 0, 0, 0});
}

/**
 * @tc.name: GetTopLayerZOrderTest
 * @tc.desc: Test GetTopLayerZOrder
 * @tc.type: FUNC
 */
HWTEST_F(RSCompositeLayerUtilsTest, GetTopLayerZOrderTest, TestSize.Level1)
{
    RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> node = RSSurfaceNode::Create(config);
    auto clUtils =
        std::make_shared<RSCompositeLayerUtils>(node, static_cast<uint32_t>(TopLayerZOrder::CHARGE_ACTION_TEXT));
    ASSERT_NE(clUtils, nullptr);
    EXPECT_EQ(clUtils->GetTopLayerZOrder(), static_cast<uint32_t>(TopLayerZOrder::CHARGE_ACTION_TEXT));
}

/**
 * @tc.name: GetCompositeNodeTest
 * @tc.desc: Test GetCompositeNode
 * @tc.type: FUNC
 */
HWTEST_F(RSCompositeLayerUtilsTest, GetCompositeNodeTest, TestSize.Level1)
{
    RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> node = RSSurfaceNode::Create(config);
    auto clUtils =
        std::make_shared<RSCompositeLayerUtils>(node, static_cast<uint32_t>(TopLayerZOrder::CHARGE_ACTION_TEXT));
    ASSERT_NE(clUtils, nullptr);
    EXPECT_EQ(clUtils->GetCompositeNode(), nullptr);
}

/**
 * @tc.name: DealWithSelfDrawCompositeNodeTest
 * @tc.desc: Test DealWithSelfDrawCompositeNode
 * @tc.type: FUNC
 */
HWTEST_F(RSCompositeLayerUtilsTest, DealWithSelfDrawCompositeNodeTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceNode> node = nullptr;
    auto clUtils =
        std::make_shared<RSCompositeLayerUtils>(node, static_cast<uint32_t>(TopLayerZOrder::CHARGE_ACTION_TEXT));
    EXPECT_FALSE(
        clUtils->DealWithSelfDrawCompositeNode(node, static_cast<uint32_t>(TopLayerZOrder::CHARGE_ACTION_TEXT)));

    RSSurfaceNodeConfig config;
    node = RSSurfaceNode::Create(config);
    ASSERT_NE(clUtils, nullptr);
    EXPECT_FALSE(
        clUtils->DealWithSelfDrawCompositeNode(node, static_cast<uint32_t>(TopLayerZOrder::CHARGE_ACTION_TEXT)));
    
    auto canvasChild = RSCanvasNode::Create();
    node->AddChild(canvasChild, -1);
    EXPECT_FALSE(
        clUtils->DealWithSelfDrawCompositeNode(node, static_cast<uint32_t>(TopLayerZOrder::CHARGE_ACTION_TEXT)));
    
    canvasChild->RemoveFromTree();

    auto surfaceChild = RSSurfaceNode::Create(config);
    node->AddChild(surfaceChild, -1);
    EXPECT_FALSE(
        clUtils->DealWithSelfDrawCompositeNode(node, static_cast<uint32_t>(TopLayerZOrder::CHARGE_ACTION_TEXT)));
    surfaceChild->RemoveFromTree();

    auto surfaceChild1 = RSSurfaceNode::Create(config, false);
    node->AddChild(surfaceChild1, -1);
    EXPECT_TRUE(
        clUtils->DealWithSelfDrawCompositeNode(node, static_cast<uint32_t>(TopLayerZOrder::CHARGE_ACTION_TEXT)));
    surfaceChild->RemoveFromTree();
}
}