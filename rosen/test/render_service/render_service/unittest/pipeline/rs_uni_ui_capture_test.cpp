/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "pipeline/rs_uni_ui_capture.h"
#include "pipeline/rs_main_thread.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUniUiCaptureTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUniUiCaptureTest::SetUpTestCase() {}
void RSUniUiCaptureTest::TearDownTestCase() {}
void RSUniUiCaptureTest::SetUp() {}
void RSUniUiCaptureTest::TearDown() {}

/**
 * @tc.name: TakeLocalCapture001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSUniUiCaptureTest, TakeLocalCapture001, TestSize.Level1)
{
    NodeId nodeId = 0;
    float scaleX = 0.0;
    float scaleY = 0.0;
    RSUniUICapture rsUniUICapture(nodeId, scaleX, scaleY);
    EXPECT_EQ(nullptr, rsUniUICapture.TakeLocalCapture());
}

/**
 * @tc.name: SetCanvasTest
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSUniUiCaptureTest, SetCanvasTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    float scaleX = 0.0;
    float scaleY = 0.0;
    RSUniUICapture::RSUniUICaptureVisitor rsUniUICaptureVisitor(nodeId, scaleX, scaleY);
    std::shared_ptr<RSRecordingCanvas> canvas = nullptr;
    rsUniUICaptureVisitor.SetCanvas(canvas);
}

/**
 * @tc.name: ProcessRootRenderNodeTest
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSUniUiCaptureTest, ProcessRootRenderNodeTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    float scaleX = 0.0;
    float scaleY = 0.0;
    std::weak_ptr<RSContext> context;
    RSUniUICapture::RSUniUICaptureVisitor rsUniUICaptureVisitor(nodeId, scaleX, scaleY);
    RSRootRenderNode node(nodeId, context);
    rsUniUICaptureVisitor.ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessCanvasRenderNodeTest
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSUniUiCaptureTest, ProcessCanvasRenderNodeTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    float scaleX = 0.0;
    float scaleY = 0.0;
    std::weak_ptr<RSContext> context;
    RSUniUICapture::RSUniUICaptureVisitor rsUniUICaptureVisitor(nodeId, scaleX, scaleY);
    RSRootRenderNode node(nodeId, context);
    rsUniUICaptureVisitor.ProcessCanvasRenderNode(node);
}

/**
 * @tc.name: ProcessSurfaceRenderNodeTest
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSUniUiCaptureTest, ProcessSurfaceRenderNodeTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    float scaleX = 0.0;
    float scaleY = 0.0;
    std::weak_ptr<RSContext> context;
    RSUniUICapture::RSUniUICaptureVisitor rsUniUICaptureVisitor(nodeId, scaleX, scaleY);
    RSSurfaceRenderNode node(nodeId, context);
    rsUniUICaptureVisitor.ProcessSurfaceRenderNode(node);
}
} // namespace OHOS::Rosen