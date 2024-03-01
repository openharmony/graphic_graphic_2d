/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "include/command/rs_canvas_node_command.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSCanvasNodeCommandTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSCanvasNodeCommandTest::SetUpTestCase() {}
void RSCanvasNodeCommandTest::TearDownTestCase() {}
void RSCanvasNodeCommandTest::SetUp() {}
void RSCanvasNodeCommandTest::TearDown() {}

/**
 * @tc.name: TestRSCanvasNodeCommand001
 * @tc.desc: UpdateRecording test.
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandTest, TestRSCanvasNodeCommand001, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    std::shared_ptr<Drawing::DrawCmdList> drawCmds = nullptr;
    RSModifierType type = RSModifierType::INVALID;
    RSCanvasNodeCommandHelper::UpdateRecording(context, nodeId, drawCmds, static_cast<uint16_t>(type));
}

/**
 * @tc.name: TestRSCanvasNodeCommand002
 * @tc.desc: ClearRecording test.
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandTest, TestRSCanvasNodeCommand002, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    RSCanvasNodeCommandHelper::ClearRecording(context, nodeId);
}

/**
 * @tc.name: Create001
 * @tc.desc: Create test.
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeCommandTest, Create001, TestSize.Level1)
{
    RSContext context;
    NodeId targetId = static_cast<NodeId>(-1);
    RSCanvasNodeCommandHelper::Create(context, targetId, false);
}
} // namespace OHOS::Rosen
