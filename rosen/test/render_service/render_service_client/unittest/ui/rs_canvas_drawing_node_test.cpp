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

#include "ui/rs_canvas_drawing_node.h"
#include "command/rs_canvas_drawing_node_command.h"
#include "common/rs_obj_geometry.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_node_map.h"
#include "pipeline/rs_render_thread.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_render_service_client.h"
#include "transaction/rs_transaction_proxy.h"
#include "pipeline/rs_canvas_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSCanvasDrawingNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSCanvasDrawingNodeTest::SetUpTestCase() {}
void RSCanvasDrawingNodeTest::TearDownTestCase() {}
void RSCanvasDrawingNodeTest::SetUp() {}
void RSCanvasDrawingNodeTest::TearDown() {}

/**
 * @tc.name: CreateTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasDrawingNodeTest, CreateTest, TestSize.Level1)
{
    bool isRenderServiceNode = true;
    RSCanvasDrawingNode::SharedPtr canvasNode = RSCanvasDrawingNode::Create(isRenderServiceNode);
}

} // namespace OHOS::Rosen