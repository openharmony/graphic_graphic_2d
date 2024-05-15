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

#include "gtest/gtest.h"
#include "drawable/rs_canvas_drawing_render_node_drawable.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_context.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
class RSCanvasDrawingRenderNodeDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSCanvasDrawingRenderNodeDrawableTest::SetUpTestCase() {}
void RSCanvasDrawingRenderNodeDrawableTest::TearDownTestCase() {}
void RSCanvasDrawingRenderNodeDrawableTest::SetUp() {}
void RSCanvasDrawingRenderNodeDrawableTest::TearDown() {}

/**
 * @tc.name: CreateCanvasDrawingRenderNodeDrawableTest
 * @tc.desc: Test If CreateCanvasDrawingRenderNodeDrawable Can Be Created
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST(RSCanvasDrawingRenderNodeDrawableTest, CreateCanvasDrawingRenderNodeDrawable, TestSize.Level1)
{
    NodeId nodeId = 1;
    auto rsContext = std::make_shared<RSContext>();
    auto canvasDrawingNode = std::make_shared<RSCanvasDrawingRenderNode>(nodeId, rsContext->weak_from_this());
    auto drawable = RSCanvasDrawingRenderNodeDrawable::OnGenerate(canvasDrawingNode);
    ASSERT_NE(drawable, nullptr);
}
}
