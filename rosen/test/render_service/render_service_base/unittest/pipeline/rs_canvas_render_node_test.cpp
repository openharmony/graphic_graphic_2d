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

#include "pipeline/rs_canvas_render_node.h"
#include "platform/common/rs_log.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSCanvasRenderNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id;
    static inline std::weak_ptr<RSContext> context = {};
};

void RSCanvasRenderNodeTest::SetUpTestCase() {}
void RSCanvasRenderNodeTest::TearDownTestCase() {}
void RSCanvasRenderNodeTest::SetUp() {}
void RSCanvasRenderNodeTest::TearDown() {}

/**
 * @tc.name: UpdateRecording001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, UpdateRecording001, TestSize.Level1)
{
    auto canvasRenderNode = std::make_shared<RSCanvasRenderNode>(id, context);
    canvasRenderNode->UpdateRecording(nullptr, RSModifierType::INVALID);
}

/**
 * @tc.name: UpdateRecording002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, UpdateRecording002, TestSize.Level1)
{
    int w;
    int h;
    auto canvasRenderNode = std::make_shared<RSCanvasRenderNode>(id + 1);
    auto drawCmds = std::make_shared<DrawCmdList>(w, h);
    canvasRenderNode->UpdateRecording(drawCmds, RSModifierType::INVALID);
}

} // namespace OHOS::Rosen