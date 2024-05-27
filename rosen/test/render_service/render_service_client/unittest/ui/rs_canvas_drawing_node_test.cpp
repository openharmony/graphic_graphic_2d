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
#include "pipeline/rs_display_render_node.h"

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

/**
 * @tc.name: ResetSurfaceTest
 * @tc.desc: test results of ResetSurface
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingNodeTest, ResetSurfaceTest, TestSize.Level1)
{
    bool isRenderServiceNode = true;
    RSCanvasDrawingNode::SharedPtr canvasNode = RSCanvasDrawingNode::Create(isRenderServiceNode);
    bool res = canvasNode->ResetSurface();
    EXPECT_EQ(res, true);

    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    res = canvasNode->ResetSurface();
    EXPECT_EQ(res, false);
    RSTransactionProxy::instance_ = new RSTransactionProxy();
}

/**
 * @tc.name: CreateTextureExportRenderNodeInRTTest
 * @tc.desc: test results of CreateTextureExportRenderNodeInRT
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSCanvasDrawingNodeTest, CreateTextureExportRenderNodeInRTTest, TestSize.Level1)
{
    bool isRenderServiceNode = true;
    RSCanvasDrawingNode::SharedPtr canvasNode = RSCanvasDrawingNode::Create(isRenderServiceNode);
    canvasNode->CreateTextureExportRenderNodeInRT();
    EXPECT_NE(RSTransactionProxy::GetInstance(), nullptr);

    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    canvasNode->CreateTextureExportRenderNodeInRT();
    EXPECT_EQ(RSTransactionProxy::GetInstance(), nullptr);
    RSTransactionProxy::instance_ = new RSTransactionProxy();
}

/**
 * @tc.name: GetBitmapTest
 * @tc.desc: test results of GetBitmap
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSCanvasDrawingNodeTest, GetBitmapTest, TestSize.Level1)
{
    bool isRenderServiceNode = true;
    auto drawingNode = std::make_shared<RSCanvasDrawingNode>(isRenderServiceNode);
    Drawing::Bitmap bitmap;
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList;
    Drawing::Rect rect;
    bool res = drawingNode->GetBitmap(bitmap, drawCmdList, &rect);
    EXPECT_EQ(res, false);
    ASSERT_TRUE(!RSSystemProperties::isUniRenderEnabled_);

    RSSystemProperties::GetUniRenderEnabled();
    RSSystemProperties::isUniRenderEnabled_ = true;
    drawingNode->GetBitmap(bitmap, drawCmdList, &rect);
    ASSERT_TRUE(RSSystemProperties::isUniRenderEnabled_);

    drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    res = drawingNode->GetBitmap(bitmap, drawCmdList, &rect);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name: GetPixelmapTest
 * @tc.desc: test results of GetPixelmap
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSCanvasDrawingNodeTest, GetPixelmapTest, TestSize.Level1)
{
    bool isRenderServiceNode = true;
    auto drawingNode = std::make_shared<RSCanvasDrawingNode>(isRenderServiceNode);
    std::shared_ptr<Media::PixelMap> pixelmap;
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList;
    Drawing::Rect rect;
    bool res = drawingNode->GetPixelmap(pixelmap, drawCmdList, &rect);
    EXPECT_EQ(res, false);

    pixelmap = std::make_shared<Media::PixelMap>();
    drawingNode->GetPixelmap(pixelmap, drawCmdList, &rect);
    EXPECT_EQ(res, false);

    pixelmap = nullptr;
    res = drawingNode->GetPixelmap(pixelmap, drawCmdList, &rect);
    EXPECT_EQ(res, false);

    RSSystemProperties::GetUniRenderEnabled();
    RSSystemProperties::isUniRenderEnabled_ = true;
    drawingNode->GetPixelmap(pixelmap, drawCmdList, &rect);
    ASSERT_TRUE(RSSystemProperties::isUniRenderEnabled_);

    pixelmap = std::make_shared<Media::PixelMap>();
    res = drawingNode->GetPixelmap(pixelmap, drawCmdList, &rect);
    EXPECT_EQ(res, false);
}
} // namespace OHOS::Rosen