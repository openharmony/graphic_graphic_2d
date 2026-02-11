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

#include "common/rs_special_layer_manager.h"
#include "feature/special_layer/rs_special_layer_utils.h"
#include "gtest/gtest.h"
#include "params/rs_screen_render_params.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_logical_display_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSSpecialLayerUtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSpecialLayerUtilsTest::SetUpTestCase() {}
void RSSpecialLayerUtilsTest::TearDownTestCase() {}
void RSSpecialLayerUtilsTest::SetUp() {}
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
    RSSpecialLayerUtils::CheckSpecialLayerIntersectMirrorDisplay(*mirrorNode, *sourceNode, true);
    ASSERT_NE(mirrorNode->GetMirrorSource().lock(), nullptr);
}

/**
 * @tc.name: NeedProcessSecLayerInDisplay_SecurityExemptionTest
 * @tc.desc: test NeedProcessSecLayerInDisplay when mirrorParam has security exemption
 * @tc.type: FUNC
 * @tc.require: issue22049
 */
HWTEST_F(RSSpecialLayerUtilsTest, NeedProcessSecLayerInDisplay_SecurityExemptionTest, TestSize.Level2)
{
    NodeId nodeId = 1;
    RSScreenRenderParams mirrorScreenParam(nodeId);
    RSLogicalDisplayRenderParams mirrorParam(nodeId++);
    RSLogicalDisplayRenderParams sourceParam(nodeId++);

    mirrorParam.SetSecurityExemption(true);

    bool result = RSSpecialLayerUtils::NeedProcessSecLayerInDisplay(false, mirrorScreenParam, mirrorParam, sourceParam);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: NeedProcessSecLayerInDisplay_VirtualSecLayerOptionTest
 * @tc.desc: test NeedProcessSecLayerInDisplay when virtual sec layer option is not zero
 * @tc.type: FUNC
 * @tc.require: issue22049
 */
HWTEST_F(RSSpecialLayerUtilsTest, NeedProcessSecLayerInDisplay_VirtualSecLayerOptionTest, TestSize.Level2)
{
    NodeId nodeId = 1;
    RSScreenRenderParams mirrorScreenParam(nodeId);
    RSLogicalDisplayRenderParams mirrorParam(nodeId++);
    RSLogicalDisplayRenderParams sourceParam(nodeId++);

    mirrorParam.isSecurityDisplay_ = true;
    // Set virtualSecLayerOption_ to non-zero value to trigger early return false
    mirrorScreenParam.screenProperty_.virtualSecLayerOption_ = 1;

    bool result = RSSpecialLayerUtils::NeedProcessSecLayerInDisplay(false, mirrorScreenParam, mirrorParam, sourceParam);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: NeedProcessSecLayerInDisplay_SecurityDisplayTest
 * @tc.desc: test NeedProcessSecLayerInDisplay when isn't security display
 * @tc.type: FUNC
 * @tc.require: issue22049
 */
HWTEST_F(RSSpecialLayerUtilsTest, NeedProcessSecLayerInDisplay_SecurityDisplayTest, TestSize.Level2)
{
    NodeId nodeId = 1;
    RSScreenRenderParams mirrorScreenParam(nodeId);
    RSLogicalDisplayRenderParams mirrorParam(nodeId++);
    RSLogicalDisplayRenderParams sourceParam(nodeId++);

    bool result = RSSpecialLayerUtils::NeedProcessSecLayerInDisplay(false, mirrorScreenParam, mirrorParam, sourceParam);
    ASSERT_FALSE(result);
}
} // namespace Rosen

