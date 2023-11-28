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
#include "modifier/rs_extended_modifier.h"
#include <memory>
#include "modifier/rs_render_modifier.h"
#include "modifier/rs_modifier_type.h"
#include "pipeline/rs_node_map.h"
#include "pipeline/rs_recording_canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSExtendedModifierTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSExtendedModifierTest::SetUpTestCase() {}
void RSExtendedModifierTest::TearDownTestCase() {}
void RSExtendedModifierTest::SetUp() {}
void RSExtendedModifierTest::TearDown() {}

/**
 * @tc.name: CreateDrawingContextTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSExtendedModifierTest, CreateDrawingContextTest, TestSize.Level1)
{
    NodeId nodeId = -1;
    RSExtendedModifierHelper::CreateDrawingContext(nodeId);
    auto node = RSNodeMap::Instance().GetNode<RSCanvasNode>(nodeId);
    ASSERT_EQ(node, nullptr);
}

/**
 * @tc.name: CreateRenderModifierTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSExtendedModifierTest, CreateRenderModifierTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto ctx = RSExtendedModifierHelper::CreateDrawingContext(nodeId);
    PropertyId id = 1;
    RSModifierType type = RSModifierType::EXTENDED;
    RSExtendedModifierHelper::CreateRenderModifier(ctx, id, type);
}

/**
 * @tc.name: FinishDrawingTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSExtendedModifierTest, FinishDrawingTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto ctx = RSExtendedModifierHelper::CreateDrawingContext(nodeId);
    RSExtendedModifierHelper::FinishDrawing(ctx);
    ASSERT_EQ(ctx.canvas, nullptr);
}
} // namespace OHOS::Rosen
