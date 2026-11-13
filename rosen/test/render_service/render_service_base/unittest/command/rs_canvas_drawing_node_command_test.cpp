/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "command/rs_canvas_drawing_node_command.h"
#include "command/rs_ui_director_command.h"
#include "common/rs_common_def.h"
#include "pipeline/rs_canvas_drawing_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSCanvasDrawingNodeCommandTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSCanvasDrawingNodeCommandTest::SetUpTestCase() {}
void RSCanvasDrawingNodeCommandTest::TearDownTestCase() {}
void RSCanvasDrawingNodeCommandTest::SetUp() {}
void RSCanvasDrawingNodeCommandTest::TearDown() {}
/**
 * @tc.name: CreateTest
 * @tc.desc: test.
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingNodeCommandTest, CreateTest, TestSize.Level1)
{
    RSContext context;
    int width = 1;
    int height = 1;
    NodeId targetId = static_cast<NodeId>(1);
    RSCanvasDrawingNodeCommandHelper::Create(context, targetId, false);
    EXPECT_NE(context.GetNodeMap().GetRenderNode<RSCanvasDrawingRenderNode>(targetId), nullptr);
    RSCanvasDrawingNodeCommandHelper::ResetSurface(context, targetId, width, height, 0);
    RSCanvasDrawingNodeCommandHelper::ResetSurface(context, 0, width, height, 0);
}


/**
 * @tc.name: CreateSceneBoardBranchTest001
 * @tc.desc: Create with a marked token covers the SceneBoard branch.
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingNodeCommandTest, CreateSceneBoardBranchTest001, TestSize.Level1)
{
    constexpr pid_t pid = 815;
    constexpr uint64_t token = 816;
    constexpr NodeId directorNodeId = MakeNodeId(pid, 0);
    constexpr NodeId nodeId = MakeNodeId(pid, 1);
    RSContext context;
    RSUIDirectorCommandHelper::GoCreate(context, directorNodeId, token);
    RSUIDirectorCommandHelper::MarkSceneBoard(context, directorNodeId, token);
    RSCanvasDrawingNodeCommandHelper::Create(context, nodeId, false, token);
    auto node = context.GetNodeMap().GetRenderNode<RSCanvasDrawingRenderNode>(nodeId);
    ASSERT_NE(node, nullptr);
    EXPECT_TRUE(node->IsSceneBoard());
    EXPECT_EQ(node->GetUIContextToken(), token);
}

} // namespace OHOS::Rosen
