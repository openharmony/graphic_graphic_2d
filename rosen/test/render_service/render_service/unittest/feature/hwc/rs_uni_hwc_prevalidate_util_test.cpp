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

#include "feature/hwc/rs_uni_hwc_prevalidate_util.h"
#include "foundation/graphic/graphic_2d/rosen/test/render_service/render_service/unittest/pipeline/rs_test_util.h"
#include "feature/round_corner_display/rs_rcd_surface_render_node_drawable.h"
#include "gtest/gtest.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
constexpr uint64_t DEFAULT_FPS = 120;
constexpr uint32_t DEFAULT_Z_ORDER = 0;
constexpr int DEFAULT_POSITION = 0;
constexpr int DEFAULT_WIDTH = 100;
constexpr int DEFAULT_HEIGHT = 100;
class RSUniHwcPrevalidateUtilTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUniHwcPrevalidateUtilTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
void RSUniHwcPrevalidateUtilTest::TearDownTestCase() {}
void RSUniHwcPrevalidateUtilTest::SetUp() {}
void RSUniHwcPrevalidateUtilTest::TearDown() {}

/**
 * @tc.name: CreateSurfaceNodeLayerInfo001
 * @tc.desc: CreateSurfaceNodeLayerInfo, input nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAZAWR
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CreateSurfaceNodeLayerInfo001, TestSize.Level1)
{
    auto& uniHwcPrevalidateUtil = RSUniHwcPrevalidateUtil::GetInstance();
    RequestLayerInfo info;
    bool ret = uniHwcPrevalidateUtil.CreateSurfaceNodeLayerInfo(
        DEFAULT_Z_ORDER, nullptr, GraphicTransformType::GRAPHIC_ROTATE_180, DEFAULT_FPS, info);
    ASSERT_EQ(info.fps, DEFAULT_FPS);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: CreateSurfaceNodeLayerInfo002
 * @tc.desc: CreateSurfaceNodeLayerInfo, input surfaceNode
 * @tc.type: FUNC
 * @tc.require: issueIAZAWR
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CreateSurfaceNodeLayerInfo002, TestSize.Level1)
{
    auto& uniHwcPrevalidateUtil = RSUniHwcPrevalidateUtil::GetInstance();
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    RequestLayerInfo info;
    bool ret = uniHwcPrevalidateUtil.CreateSurfaceNodeLayerInfo(
        DEFAULT_Z_ORDER, surfaceNode, GraphicTransformType::GRAPHIC_ROTATE_180, DEFAULT_FPS, info);
    ASSERT_EQ(info.fps, DEFAULT_FPS);
    ASSERT_EQ(ret, true);
}

/**
 * @tc.name: CreateSurfaceNodeLayerInfo003
 * @tc.desc: CreateSurfaceNodeLayerInfo, input surfaceNode with metaData
 * @tc.type: FUNC
 * @tc.require: issueIBQDHZ
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CreateSurfaceNodeLayerInfo003, TestSize.Level1)
{
    auto& uniHwcPrevalidateUtil = RSUniHwcPrevalidateUtil::GetInstance();
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto buffer = surfaceNode->surfaceHandler_->buffer_.buffer;
    ASSERT_NE(buffer, nullptr);
    buffer->SetCropMetadata({DEFAULT_POSITION, DEFAULT_POSITION, DEFAULT_WIDTH, DEFAULT_HEIGHT});
    RequestLayerInfo info;
    bool ret = uniHwcPrevalidateUtil.CreateSurfaceNodeLayerInfo(
        DEFAULT_Z_ORDER, surfaceNode, GraphicTransformType::GRAPHIC_ROTATE_180, DEFAULT_FPS, info);
    ASSERT_EQ(ret, true);
}

/**
 * @tc.name: IsYUVBufferFormat001
 * @tc.desc: IsYUVBufferFormat, buffer is nullptr && format is invalid
 * @tc.type: FUNC
 * @tc.require: issueIAZAWR
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, IsYUVBufferFormat001, TestSize.Level1)
{
    auto& uniHwcPrevalidateUtil = RSUniHwcPrevalidateUtil::GetInstance();
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto surfaceHandler = surfaceNode1->surfaceHandler_;
    ASSERT_NE(surfaceHandler, nullptr);
    surfaceHandler->buffer_.buffer = nullptr;
    bool ret = uniHwcPrevalidateUtil.IsYUVBufferFormat(surfaceNode1);
    ASSERT_EQ(ret, false);

    auto surfaceNode2 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto bufferHandle = surfaceNode2->surfaceHandler_->buffer_.buffer->GetBufferHandle();
    ASSERT_NE(bufferHandle, nullptr);
    bufferHandle->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_1010102;
    ret = uniHwcPrevalidateUtil.IsYUVBufferFormat(surfaceNode2);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: IsYUVBufferFormat002
 * @tc.desc: IsYUVBufferFormat, format is valid
 * @tc.type: FUNC
 * @tc.require: issueIAZAWR
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, IsYUVBufferFormat002, TestSize.Level1)
{
    auto& uniHwcPrevalidateUtil = RSUniHwcPrevalidateUtil::GetInstance();
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto bufferHandle = surfaceNode->surfaceHandler_->buffer_.buffer->GetBufferHandle();
    ASSERT_NE(bufferHandle, nullptr);
    bufferHandle->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YUV_422_I;
    bool ret = uniHwcPrevalidateUtil.IsYUVBufferFormat(surfaceNode);
    ASSERT_EQ(ret, true);
}

/**
 * @tc.name: IsNeedDssRotate001
 * @tc.desc: IsNeedDssRotate, format is valid
 * @tc.type: FUNC
 * @tc.require: issueIBZZJT
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, IsNeedDssRotate001, TestSize.Level1)
{
    auto& uniHwcPrevalidateUtil = RSUniHwcPrevalidateUtil::GetInstance();
    bool ret = uniHwcPrevalidateUtil.IsNeedDssRotate(GraphicTransformType::GRAPHIC_ROTATE_NONE);
    ASSERT_EQ(ret, false);
    ret = uniHwcPrevalidateUtil.IsNeedDssRotate(GraphicTransformType::GRAPHIC_ROTATE_90);
    ASSERT_EQ(ret, true);
    ret = uniHwcPrevalidateUtil.IsNeedDssRotate(GraphicTransformType::GRAPHIC_ROTATE_180);
    ASSERT_EQ(ret, false);
    ret = uniHwcPrevalidateUtil.IsNeedDssRotate(GraphicTransformType::GRAPHIC_ROTATE_270);
    ASSERT_EQ(ret, true);
    ret = uniHwcPrevalidateUtil.IsNeedDssRotate(GraphicTransformType::GRAPHIC_FLIP_H);
    ASSERT_EQ(ret, false);
    ret = uniHwcPrevalidateUtil.IsNeedDssRotate(GraphicTransformType::GRAPHIC_FLIP_V);
    ASSERT_EQ(ret, false);
    ret = uniHwcPrevalidateUtil.IsNeedDssRotate(GraphicTransformType::GRAPHIC_FLIP_H_ROT90);
    ASSERT_EQ(ret, true);
    ret = uniHwcPrevalidateUtil.IsNeedDssRotate(GraphicTransformType::GRAPHIC_FLIP_V_ROT90);
    ASSERT_EQ(ret, true);
    ret = uniHwcPrevalidateUtil.IsNeedDssRotate(GraphicTransformType::GRAPHIC_FLIP_H_ROT180);
    ASSERT_EQ(ret, false);
    ret = uniHwcPrevalidateUtil.IsNeedDssRotate(GraphicTransformType::GRAPHIC_FLIP_V_ROT180);
    ASSERT_EQ(ret, false);
    ret = uniHwcPrevalidateUtil.IsNeedDssRotate(GraphicTransformType::GRAPHIC_FLIP_H_ROT270);
    ASSERT_EQ(ret, true);
    ret = uniHwcPrevalidateUtil.IsNeedDssRotate(GraphicTransformType::GRAPHIC_FLIP_V_ROT270);
    ASSERT_EQ(ret, true);
    ret = uniHwcPrevalidateUtil.IsNeedDssRotate(GraphicTransformType::GRAPHIC_ROTATE_BUTT);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: CreateDisplayNodeLayerInfo001
 * @tc.desc: CreateDisplayNodeLayerInfo, node is nullptr
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CreateDisplayNodeLayerInfo001, TestSize.Level1)
{
    auto& uniHwcPrevalidateUtil = RSUniHwcPrevalidateUtil::GetInstance();
    ScreenInfo screenInfo;
    RequestLayerInfo info;
    bool ret = uniHwcPrevalidateUtil.CreateDisplayNodeLayerInfo(
        DEFAULT_Z_ORDER, nullptr, screenInfo, DEFAULT_FPS, info);
    ASSERT_EQ(info.fps, DEFAULT_FPS);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: CreateDisplayNodeLayerInfo002
 * @tc.desc: CreateDisplayNodeLayerInfo, input displayNode
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CreateDisplayNodeLayerInfo002, TestSize.Level1)
{
    auto& uniHwcPrevalidateUtil = RSUniHwcPrevalidateUtil::GetInstance();
    ScreenInfo screenInfo;
    RSDisplayNodeConfig config;
    NodeId id = 0;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    RequestLayerInfo info;
    bool ret = uniHwcPrevalidateUtil.CreateDisplayNodeLayerInfo(
        DEFAULT_Z_ORDER, displayNode, screenInfo, DEFAULT_FPS, info);
    ASSERT_EQ(info.fps, DEFAULT_FPS);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: CreateRCDLayerInfo001
 * @tc.desc: CreateRCDLayerInfo, input nullptr
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CreateRCDLayerInfo001, TestSize.Level1)
{
    auto& uniHwcPrevalidateUtil = RSUniHwcPrevalidateUtil::GetInstance();
    ScreenInfo screenInfo;
    RequestLayerInfo info;
    bool ret = uniHwcPrevalidateUtil.CreateRCDLayerInfo(nullptr, screenInfo, DEFAULT_FPS, info);
    ASSERT_EQ(info.fps, DEFAULT_FPS);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: CreateRCDLayerInfo002
 * @tc.desc: CreateRCDLayerInfo, input RCDSurfaceNode
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CreateRCDLayerInfo002, TestSize.Level1)
{
    auto& uniHwcPrevalidateUtil = RSUniHwcPrevalidateUtil::GetInstance();
    NodeId id = 1;
    auto node = std::make_shared<RSRcdSurfaceRenderNode>(id, RCDSurfaceType::BOTTOM);
    ASSERT_NE(node, nullptr);
    ScreenInfo screenInfo;
    RequestLayerInfo info;
    bool ret = uniHwcPrevalidateUtil.CreateRCDLayerInfo(node, screenInfo, DEFAULT_FPS, info);
    ASSERT_EQ(info.fps, DEFAULT_FPS);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: IsPrevalidateEnable001
 * @tc.desc: IsPrevalidateEnable, input screen 0 and load success/fail
 * @tc.type: FUNC
 * @tc.require: issueIATEBN
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, IsPrevalidateEnable001, TestSize.Level1)
{
    auto& uniHwcPrevalidateUtil = RSUniHwcPrevalidateUtil::GetInstance();
    uniHwcPrevalidateUtil.isPrevalidateHwcNodeEnable_ = true;
    uniHwcPrevalidateUtil.loadSuccess_ = false;
    bool ret = uniHwcPrevalidateUtil.IsPrevalidateEnable();
    EXPECT_EQ(ret, false);
    uniHwcPrevalidateUtil.loadSuccess_ = true;
    ret = uniHwcPrevalidateUtil.IsPrevalidateEnable();
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: CheckHwcNodeAndGetPointerWindow001
 * @tc.desc: CheckHwcNodeAndGetPointerWindow, input nullptr or hwcNode not on the tree
 * @tc.type: FUNC
 * @tc.require: issueIATEBN
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CheckHwcNodeAndGetPointerWindow001, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> hwcNode = nullptr;
    std::shared_ptr<RSSurfaceRenderNode> pointerWindow = nullptr;
    bool ret = RSUniHwcPrevalidateUtil::CheckHwcNodeAndGetPointerWindow(hwcNode, pointerWindow);
    EXPECT_EQ(ret, false);
    hwcNode = RSTestUtil::CreateSurfaceNode();
    hwcNode->isOnTheTree_ = false;
    ret = RSUniHwcPrevalidateUtil::CheckHwcNodeAndGetPointerWindow(hwcNode, pointerWindow);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: CheckHwcNodeAndGetPointerWindow002
 * @tc.desc: CheckHwcNodeAndGetPointerWindow, input pointerWindow
 * @tc.type: FUNC
 * @tc.require: issueIATEBN
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CheckHwcNodeAndGetPointerWindow002, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> hwcNode = RSTestUtil::CreateSurfaceNode();
    std::shared_ptr<RSSurfaceRenderNode> pointerWindow = nullptr;
    hwcNode->isOnTheTree_ = true;
    hwcNode->nodeType_ = RSSurfaceNodeType::CURSOR_NODE;
    hwcNode->name_ = "pointer window";
    bool ret = RSUniHwcPrevalidateUtil::CheckHwcNodeAndGetPointerWindow(hwcNode, pointerWindow);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: CheckHwcNodeAndGetPointerWindow003
 * @tc.desc: CheckHwcNodeAndGetPointerWindow, input normal hwcNode
 * @tc.type: FUNC
 * @tc.require: issueIATEBN
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CheckHwcNodeAndGetPointerWindow003, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> hwcNode = RSTestUtil::CreateSurfaceNode();
    std::shared_ptr<RSSurfaceRenderNode> pointerWindow = nullptr;
    hwcNode->isOnTheTree_ = true;
    hwcNode->dstRect_ = { DEFAULT_POSITION, DEFAULT_POSITION, DEFAULT_WIDTH, DEFAULT_HEIGHT };
    bool ret = RSUniHwcPrevalidateUtil::CheckHwcNodeAndGetPointerWindow(hwcNode, pointerWindow);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: CheckHwcNodeAndGetPointerWindow003
 * @tc.desc: CheckHwcNodeAndGetPointerWindow, input surfaceNode without buffer
 * @tc.type: FUNC
 * @tc.require: issueIBQDHZ
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CheckHwcNodeAndGetPointerWindow004, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> hwcNode = RSTestUtil::CreateSurfaceNode();
    std::shared_ptr<RSSurfaceRenderNode> pointerWindow = nullptr;
    hwcNode->isOnTheTree_ = true;
    bool ret = RSUniHwcPrevalidateUtil::CheckHwcNodeAndGetPointerWindow(hwcNode, pointerWindow);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: CheckIfDoArsrPre001
 * @tc.desc: CheckIfDoArsrPre, input normal surfacenode
 * @tc.type: FUNC
 * @tc.require: issueIBA6PF
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CheckIfDoArsrPre001, TestSize.Level1)
{
    auto& uniHwcPrevalidateUtil = RSUniHwcPrevalidateUtil::GetInstance();
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto bufferHandle = surfaceNode->surfaceHandler_->buffer_.buffer->GetBufferHandle();
    ASSERT_NE(bufferHandle, nullptr);
    bufferHandle->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YUV_422_I;
    bool ret = uniHwcPrevalidateUtil.CheckIfDoArsrPre(surfaceNode);
    ASSERT_EQ(ret, true);
}

/**
 * @tc.name: CheckIfDoArsrPre001
 * @tc.desc: CheckIfDoArsrPre, surfaceNode buffer is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBQDHZ
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CheckIfDoArsrPre002, TestSize.Level1)
{
    auto& uniHwcPrevalidateUtil = RSUniHwcPrevalidateUtil::GetInstance();
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    bool ret = uniHwcPrevalidateUtil.CheckIfDoArsrPre(surfaceNode);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: CheckIfDoCopybit001
 * @tc.desc: CheckIfDoCopybit, input normal surfacenode
 * @tc.type: FUNC
 * @tc.require: issueIBZZJT
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CheckIfDoCopybit001, TestSize.Level1)
{
    auto& uniHwcPrevalidateUtil = RSUniHwcPrevalidateUtil::GetInstance();
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto bufferHandle = surfaceNode->surfaceHandler_->buffer_.buffer->GetBufferHandle();
    ASSERT_NE(bufferHandle, nullptr);
    bufferHandle->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YUV_422_I;
    RequestLayerInfo info;
    uniHwcPrevalidateUtil.CheckIfDoCopybit(surfaceNode, GraphicTransformType::GRAPHIC_ROTATE_90, info);
}

/**
 * @tc.name: CheckIfDoCopybit002
 * @tc.desc: CheckIfDoCopybit, input normal surfacenode
 * @tc.type: FUNC
 * @tc.require: issueIBZZJT
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CheckIfDoCopybit002, TestSize.Level1)
{
    auto& uniHwcPrevalidateUtil = RSUniHwcPrevalidateUtil::GetInstance();
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    RequestLayerInfo info;
    uniHwcPrevalidateUtil.CheckIfDoCopybit(surfaceNode, GraphicTransformType::GRAPHIC_ROTATE_90, info);
}

/**
 * @tc.name: CopyCldInfo001
 * @tc.desc: CopyCldInfo
 * @tc.type: FUNC
 * @tc.require: issueIBA6PF
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CopyCldInfo001, TestSize.Level1)
{
    auto& uniHwcPrevalidateUtil = RSUniHwcPrevalidateUtil::GetInstance();
    NodeId id = 1;
    auto node = std::make_shared<RSRcdSurfaceRenderNode>(id, RCDSurfaceType::BOTTOM);
    ASSERT_NE(node, nullptr);
    RequestLayerInfo info;
    uniHwcPrevalidateUtil.CopyCldInfo(node->GetCldInfo(), info);
}

/**
 * @tc.name: LayerRotate001
 * @tc.desc: LayerRotate
 * @tc.type: FUNC
 * @tc.require: issueIBA6PF
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, LayerRotate001, TestSize.Level1)
{
    auto& uniHwcPrevalidateUtil = RSUniHwcPrevalidateUtil::GetInstance();
    RequestLayerInfo info;
    info.dstRect = {50, 50, 100, 200};
    ScreenInfo screenInfo;
    sptr<IConsumerSurface> cSurface = nullptr;
    uniHwcPrevalidateUtil.LayerRotate(info, cSurface, screenInfo);
    ASSERT_EQ(info.dstRect.w, 100);
    ASSERT_EQ(info.dstRect.h, 200);
    
    cSurface = IConsumerSurface::Create();
    uniHwcPrevalidateUtil.LayerRotate(info, cSurface, screenInfo);
    ASSERT_EQ(info.dstRect.w, 100);
    ASSERT_EQ(info.dstRect.h, 200);

    screenInfo.rotation = ScreenRotation::ROTATION_90;
    uniHwcPrevalidateUtil.LayerRotate(info, cSurface, screenInfo);
    ASSERT_EQ(info.dstRect.w, 200);
    ASSERT_EQ(info.dstRect.h, 100);

    screenInfo.rotation = ScreenRotation::ROTATION_180;
    uniHwcPrevalidateUtil.LayerRotate(info, cSurface, screenInfo);
    ASSERT_EQ(info.dstRect.w, 200);
    ASSERT_EQ(info.dstRect.h, 100);

    screenInfo.rotation = ScreenRotation::ROTATION_270;
    uniHwcPrevalidateUtil.LayerRotate(info, cSurface, screenInfo);
    ASSERT_EQ(info.dstRect.w, 100);
    ASSERT_EQ(info.dstRect.h, 200);
}

/**
 * @tc.name: EmplaceSurfaceNodeLayer001
 * @tc.desc: EmplaceSurfaceNodeLayer
 * @tc.type: FUNC
 * @tc.require: issueIBA6PF
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, EmplaceSurfaceNodeLayer001, TestSize.Level1)
{
    auto& uniHwcPrevalidateUtil = RSUniHwcPrevalidateUtil::GetInstance();
    std::vector<RequestLayerInfo> prevalidLayers;
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    ScreenInfo screenInfo;
    uint32_t zOrder = DEFAULT_Z_ORDER;
    uniHwcPrevalidateUtil.EmplaceSurfaceNodeLayer(prevalidLayers, surfaceNode, DEFAULT_FPS, zOrder, screenInfo);
    ASSERT_EQ(prevalidLayers.size(), 1);
}

/**
 * @tc.name: CollectSurfaceNodeLayerInfo001
 * @tc.desc: CollectSurfaceNodeLayerInfo
 * @tc.type: FUNC
 * @tc.require: issueIBA6PF
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CollectSurfaceNodeLayerInfo001, TestSize.Level1)
{
    auto& uniHwcPrevalidateUtil = RSUniHwcPrevalidateUtil::GetInstance();
    std::vector<RequestLayerInfo> prevalidLayers;
    std::vector<RSBaseRenderNode::SharedPtr> surfaceNodes;
    surfaceNodes.push_back(nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNodes.push_back(surfaceNode);
    ScreenInfo screenInfo;
    uint32_t zOrder = DEFAULT_Z_ORDER;
    uniHwcPrevalidateUtil.CollectSurfaceNodeLayerInfo(prevalidLayers, surfaceNodes, DEFAULT_FPS, zOrder, screenInfo);
}

/**
 * @tc.name: Destructor001
 * @tc.desc: test for Destructor when handle is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBQDHZ
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, Destructor001, TestSize.Level1)
{
    auto& util = RSUniHwcPrevalidateUtil::GetInstance();
    util.preValidateHandle_ = nullptr;
    ASSERT_EQ(util.preValidateHandle_, nullptr);
}

/**
 * @tc.name: PreVaildate001
 * @tc.desc: test for Prevaildate
 * @tc.type: FUNC
 * @tc.require: issueIBQDHZ
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, PreVaildate001, TestSize.Level1)
{
    auto& util = RSUniHwcPrevalidateUtil::GetInstance();
    ScreenId id = 0;
    std::vector<RequestLayerInfo> infos;
    std::map<uint64_t, RequestCompositionType> strategy;
    if (util.preValidateFunc_) {
        bool ret = util.PreValidate(id, infos, strategy);
        ASSERT_EQ(ret, true);
    }
    util.preValidateFunc_ = nullptr;
    bool ret = util.PreValidate(id, infos, strategy);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: CheckIfDoCopybit003
 * @tc.desc: CheckIfDoCopybit, input normal surfacenode
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CheckIfDoCopybit003, TestSize.Level1)
{
    auto& uniHwcPrevalidateUtil = RSUniHwcPrevalidateUtil::GetInstance();
    uniHwcPrevalidateUtil.isCopybitSupported_ = true;
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto bufferHandle = surfaceNode->surfaceHandler_->buffer_.buffer->GetBufferHandle();
    ASSERT_NE(bufferHandle, nullptr);
    bufferHandle->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YUV_422_I;
    RequestLayerInfo info;
    uniHwcPrevalidateUtil.CheckIfDoCopybit(surfaceNode, GraphicTransformType::GRAPHIC_ROTATE_90, info);
    ASSERT_EQ(surfaceNode->GetCopybitTag(), true);
}

/**
 * @tc.name: CollectSurfaceNodeLayerInfo002
 * @tc.desc: CollectSurfaceNodeLayerInfo
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CollectSurfaceNodeLayerInfo002, TestSize.Level1)
{
    auto& uniHwcPrevalidateUtil = RSUniHwcPrevalidateUtil::GetInstance();
    std::vector<RequestLayerInfo> prevalidLayers;
    std::vector<RSBaseRenderNode::SharedPtr> surfaceNodes;
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto hwcNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(hwcNode, nullptr);

    hwcNode->isOnTheTree_ = true;
    hwcNode->dstRect_ = { DEFAULT_POSITION, DEFAULT_POSITION, DEFAULT_WIDTH, DEFAULT_HEIGHT };
    surfaceNode->AddChildHardwareEnabledNode(hwcNode);
    surfaceNodes.emplace_back(surfaceNode);

    ScreenInfo screenInfo;
    uint32_t zOrder = DEFAULT_Z_ORDER;
    uniHwcPrevalidateUtil.CollectSurfaceNodeLayerInfo(prevalidLayers, surfaceNodes, DEFAULT_FPS, zOrder, screenInfo);
    ASSERT_EQ(prevalidLayers.size(), 1);
}

/**
 * @tc.name: CollectSurfaceNodeLayerInfo003
 * @tc.desc: CollectSurfaceNodeLayerInfo
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CollectSurfaceNodeLayerInfo003, TestSize.Level1)
{
    auto& uniHwcPrevalidateUtil = RSUniHwcPrevalidateUtil::GetInstance();
    std::vector<RequestLayerInfo> prevalidLayers;
    std::vector<RSBaseRenderNode::SharedPtr> surfaceNodes;
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto hwcNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(hwcNode, nullptr);

    hwcNode->isOnTheTree_ = true;
    hwcNode->nodeType_ = RSSurfaceNodeType::CURSOR_NODE;
    hwcNode->name_ = "pointer window";
    surfaceNode->AddChildHardwareEnabledNode(hwcNode);
    surfaceNodes.emplace_back(surfaceNode);

    ScreenInfo screenInfo;
    uint32_t zOrder = DEFAULT_Z_ORDER;
    uniHwcPrevalidateUtil.CollectSurfaceNodeLayerInfo(prevalidLayers, surfaceNodes, DEFAULT_FPS, zOrder, screenInfo);
    ASSERT_EQ(prevalidLayers.size(), 1);
}
}