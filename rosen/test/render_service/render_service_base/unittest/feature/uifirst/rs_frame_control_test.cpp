/*
* Copyright (c) 2026-2026 Huawei Device Co., Ltd.
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

#include "feature/uifirst/rs_frame_control.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class RSFrameControlToolTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSFrameControlToolTest::SetUpTestCase() {}
void RSFrameControlToolTest::TearDownTestCase() {}
void RSFrameControlToolTest::SetUp() {}
void RSFrameControlToolTest::TearDown() {}

/**
* @tc.name: CheckAppWindowNodeId001
* @tc.desc: test single frame modifier add to list
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(RSFrameControlToolTest, CheckAppWindowNodeId001, TestSize.Level1)
{
    NodeId id1 = 100;
    NodeId id2 = 200;
    // case1
    RSFrameControlTool::Instance().SetAppWindowNodeId(id1);
    EXPECT_EQ(RSFrameControlTool::Instance().CheckAppWindowNodeId(id1), true);
    // case2
    EXPECT_EQ(RSFrameControlTool::Instance().CheckAppWindowNodeId(id2), false);
}

/**
* @tc.name: CheckAppWindowNodeId002
* @tc.desc: test single frame modifier add to list
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(RSFrameControlToolTest, CheckAppWindowNodeId002, TestSize.Level1)
{
    NodeId id1 = 100;
    NodeId id2 = 200;
    std::shared_ptr<RSSurfaceRenderNode> node1 = std::make_shared<RSSurfaceRenderNode>(id1);
    RSFrameControlTool::Instance().SetNodeIdForFrameControl(*node1);

    system::SetParameter("const.graphic.subthread.control.framerate", true);
    auto rsContext = std::make_shared<RSContext>();
    std::shared_ptr<RSSurfaceRenderNode> node2 = std::make_shared<RSSurfaceRenderNode>(id1, rsContext);
    node2->firstLevelNodeId_ = id2;
    RSFrameControlTool::Instance().SetNodeIdForFrameControl(*node2);

    auto& nodeMap = rsContext->GetMutableNodeMap();
    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(id2, std::make_shared<RSContext>());
    bool res = nodeMap.RegisterRenderNode(surfaceRenderNode);
    ASSERT_EQ(res, true);
    std::shared_ptr<RSSurfaceRenderNode> node3 = std::make_shared<RSSurfaceRenderNode>(id1, rsContext);
    node3->firstLevelNodeId_ = id2;
    RSFrameControlTool::Instance().SetNodeIdForFrameControl(*node3);
    system::SetParameter("const.graphic.subthread.control.framerate", false);
    EXPECT_EQ(RSFrameControlTool::Instance().CheckAppWindowNodeId(id2), true);
}
}
}