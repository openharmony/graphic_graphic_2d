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

#include "drawable/rs_screen_render_node_drawable.h"
#include "feature/hwc/hpae_offline/rs_offline_processor.h"
#include "feature/hwc/rs_uni_hwc_prevalidate_param_util.h"
#include "feature/hwc/rs_uni_hwc_prevalidate_util.h"
#include "feature/round_corner_display/rs_rcd_render_manager.h"
#include "feature/round_corner_display/rs_round_corner_display_manager.h"
#include "foundation/graphic/graphic_2d/rosen/test/render_service/render_service/unittest/pipeline/rs_test_util.h"
#include "common/rs_common_hook.h"
#include "common/rs_singleton.h"
#include "gtest/gtest.h"
#include "hwc_param.h"
#include "params/rs_surface_render_params.h"
#include "platform/common/rs_system_properties.h"
#include <parameters.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
constexpr uint64_t DEFAULT_FPS = 120;
constexpr uint32_t DEFAULT_Z_ORDER = 0;
constexpr int DEFAULT_POSITION = 0;
constexpr int DEFAULT_WIDTH = 100;
constexpr int DEFAULT_HEIGHT = 100;
constexpr uint64_t USAGE_UNI_LAYER_VAL = 1ULL << 60;
constexpr uint64_t USAGE_DUAL_PREVIEW_VAL_VAL = 1ULL << 50;
constexpr uint64_t USAGE_UNI_RENDER_LAYER_NULL = 1ULL << 51;

class RSUniHwcPrevalidateUtilTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    RSScreenProperty defaultProperty_;
};

void RSUniHwcPrevalidateUtilTest::SetUpTestCase()
{
    auto& util = RSUniHwcPrevalidateUtil::GetInstance();
    if (!util.IsPrevalidateEnable()) {
        GTEST_SKIP() << "Skip all tests: prevalidate not enabled on this device";
    }
    RSTestUtil::InitRenderNodeGC();
    OHOS::system::SetParameter("const.display.support_copybit", "true");
}

void RSUniHwcPrevalidateUtilTest::TearDownTestCase()
{
    OHOS::system::SetParameter("const.display.support_copybit", "false");
}

void RSUniHwcPrevalidateUtilTest::SetUp()
{
    defaultProperty_.Set<ScreenPropertyType::PHYSICAL_RESOLUTION_REFRESHRATE>(
        std::tuple<uint32_t, uint32_t, uint32_t>{0u, 0u, DEFAULT_FPS});
}
void RSUniHwcPrevalidateUtilTest::TearDown() {}

/**
 * @tc.name: CreateSurfaceNodeLayerInfo001
 * @tc.desc: CreateSurfaceNodeLayerInfo, input nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAZAWR
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CreateSurfaceNodeLayerInfo001, TestSize.Level1)
{
    RequestLayerInfo info;
    uint32_t zOrder = DEFAULT_Z_ORDER;
    bool ret = RSUniHwcPrevalidateUtil::CreateSurfaceNodeLayerInfo(
        zOrder, nullptr, defaultProperty_, info);
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
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    RequestLayerInfo info;
    uint32_t zOrder = DEFAULT_Z_ORDER;
    bool ret = RSUniHwcPrevalidateUtil::CreateSurfaceNodeLayerInfo(
        zOrder, surfaceNode, defaultProperty_, info);
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
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto buffer = surfaceNode->surfaceHandler_->buffer_.buffer;
    ASSERT_NE(buffer, nullptr);
    buffer->SetCropMetadata({DEFAULT_POSITION, DEFAULT_POSITION, DEFAULT_WIDTH, DEFAULT_HEIGHT});
    RequestLayerInfo info;
    uint32_t zOrder = DEFAULT_Z_ORDER;
    bool ret = RSUniHwcPrevalidateUtil::CreateSurfaceNodeLayerInfo(
        zOrder, surfaceNode, defaultProperty_, info);
    ASSERT_EQ(ret, true);
}

/**
 * @tc.name: CreateSurfaceNodeLayerInfoLayerLinearMatrix
 * @tc.desc: CreateSurfaceNodeLayerInfo, layer linear matrix branches
 * @tc.type: FUNC
 * @tc.require: issueIBQDHZ
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CreateSurfaceNodeLayerInfoLayerLinearMatrix, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams *>(surfaceNode->GetStagingRenderParams().get());
    ASSERT_NE(stagingSurfaceParams, nullptr);

    std::vector<float> layerLinearMatrix = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    stagingSurfaceParams->SetLayerLinearMatrix(layerLinearMatrix);
    RequestLayerInfo info;
    uint32_t zOrder = DEFAULT_Z_ORDER;
    bool ret = RSUniHwcPrevalidateUtil::CreateSurfaceNodeLayerInfo(
        zOrder, surfaceNode, defaultProperty_, info);
    ASSERT_EQ(ret, true);
    EXPECT_NE(info.perFrameParameters.find("LayerLinearMatrix"), info.perFrameParameters.end());

    std::vector<float> smallMatrix = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f};
    stagingSurfaceParams->SetLayerLinearMatrix(smallMatrix);
    RequestLayerInfo infoSmall;
    ret = RSUniHwcPrevalidateUtil::CreateSurfaceNodeLayerInfo(
        zOrder, surfaceNode, defaultProperty_,
        infoSmall);
    ASSERT_EQ(ret, true);
    EXPECT_EQ(infoSmall.perFrameParameters.find("LayerLinearMatrix"), infoSmall.perFrameParameters.end());
}

/**
 * @tc.name: CreateSurfaceNodeLayerInfoVcldInfo
 * @tc.desc: CreateSurfaceNodeLayerInfo, input surfaceNode with vcldInfo
 * @tc.type: FUNC
 * @tc.require: issueIBQDHZ
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CreateSurfaceNodeLayerInfoVcldInfo, TestSize.Level1)
{
    RSUniHwcPrevalidateParamUtil::SetIsVcldEnabled(true);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams *>(surfaceNode->GetStagingRenderParams().get());
    ASSERT_NE(stagingSurfaceParams, nullptr);
    RequestLayerInfo info;
    uint32_t zOrder = DEFAULT_Z_ORDER;
    bool ret = RSUniHwcPrevalidateUtil::CreateSurfaceNodeLayerInfo(
        zOrder, surfaceNode, defaultProperty_, info);
    ASSERT_EQ(ret, true);

    auto surfaceNode1 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode1, nullptr);
    surfaceNode1->stagingRenderParams_ = nullptr;
    RequestLayerInfo info1;
    bool ret1 = RSUniHwcPrevalidateUtil::CreateSurfaceNodeLayerInfo(
        zOrder, surfaceNode1, defaultProperty_, info1);
    ASSERT_EQ(ret1, false);
}

/**
 * @tc.name: CreateSurfaceNodeLayerInfo004
 * @tc.desc: CreateSurfaceNodeLayerInfo, isHdiRogEnable=true keeps original dstRect
 * @tc.type: FUNC
 * @tc.require: issues/24560
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CreateSurfaceNodeLayerInfo004, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->dstRect_ = {DEFAULT_POSITION, DEFAULT_POSITION, DEFAULT_WIDTH, DEFAULT_HEIGHT};

    RSScreenProperty screenProperty;
    screenProperty.Set<ScreenPropertyType::IS_ROG_RESOLUTION>(true);
    screenProperty.Set<ScreenPropertyType::IS_HDI_ROG_ENABLE>(true);

    RequestLayerInfo info;
    uint32_t zOrder = DEFAULT_Z_ORDER;
    bool ret = RSUniHwcPrevalidateUtil::CreateSurfaceNodeLayerInfo(
        zOrder, surfaceNode, screenProperty, info);
    ASSERT_EQ(ret, true);
    ASSERT_EQ(info.dstRect.x, DEFAULT_POSITION);
    ASSERT_EQ(info.dstRect.y, DEFAULT_POSITION);
    ASSERT_EQ(info.dstRect.w, DEFAULT_WIDTH);
    ASSERT_EQ(info.dstRect.h, DEFAULT_HEIGHT);
}

/**
 * @tc.name: CreateSurfaceNodeLayerInfo005
 * @tc.desc: CreateSurfaceNodeLayerInfo, isRogResolution=true + isHdiRogEnable=false applies ROG scaling
 * @tc.type: FUNC
 * @tc.require: issues/24560
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CreateSurfaceNodeLayerInfo005, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->dstRect_ = {DEFAULT_POSITION, DEFAULT_POSITION, DEFAULT_WIDTH, DEFAULT_HEIGHT};

    RSScreenProperty screenProperty;
    screenProperty.Set<ScreenPropertyType::IS_ROG_RESOLUTION>(true);
    screenProperty.Set<ScreenPropertyType::IS_HDI_ROG_ENABLE>(false);
    screenProperty.Set<ScreenPropertyType::RENDER_RESOLUTION>(std::pair<uint32_t, uint32_t>{800u, 600u});
    screenProperty.Set<ScreenPropertyType::PHYSICAL_RESOLUTION_REFRESHRATE>(
        std::tuple<uint32_t, uint32_t, uint32_t>{1600u, 1200u, 60u});

    RequestLayerInfo info;
    uint32_t zOrder = DEFAULT_Z_ORDER;
    bool ret = RSUniHwcPrevalidateUtil::CreateSurfaceNodeLayerInfo(
        zOrder, surfaceNode, screenProperty, info);
    ASSERT_EQ(ret, true);
    ASSERT_EQ(info.dstRect.x, DEFAULT_POSITION);
    ASSERT_EQ(info.dstRect.y, DEFAULT_POSITION);
    ASSERT_EQ(info.dstRect.w, DEFAULT_WIDTH * 2);
    ASSERT_EQ(info.dstRect.h, DEFAULT_HEIGHT * 2);
}

/**
 * @tc.name: CreateSurfaceNodeLayerInfo006
 * @tc.desc: CreateSurfaceNodeLayerInfo, isRogResolution=true,isHdiRogEnable=false,IsPointerWindow=true
 * @tc.type: FUNC
 * @tc.require: issues/24560
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CreateSurfaceNodeLayerInfo006, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->dstRect_ = {DEFAULT_POSITION, DEFAULT_POSITION, DEFAULT_WIDTH, DEFAULT_HEIGHT};
    surfaceNode->isOnTheTree_ = true;
    surfaceNode->nodeType_ = RSSurfaceNodeType::CURSOR_NODE;

    RSScreenProperty screenProperty;
    screenProperty.Set<ScreenPropertyType::IS_ROG_RESOLUTION>(true);
    screenProperty.Set<ScreenPropertyType::IS_HDI_ROG_ENABLE>(false);
    screenProperty.Set<ScreenPropertyType::RENDER_RESOLUTION>(std::pair<uint32_t, uint32_t>{800u, 600u});
    screenProperty.Set<ScreenPropertyType::PHYSICAL_RESOLUTION_REFRESHRATE>(
        std::tuple<uint32_t, uint32_t, uint32_t>{1600u, 1200u, 60u});

    RequestLayerInfo info;
    uint32_t zOrder = DEFAULT_Z_ORDER;
    bool ret = RSUniHwcPrevalidateUtil::CreateSurfaceNodeLayerInfo(
        zOrder, surfaceNode, screenProperty, info);
    ASSERT_EQ(ret, true);
    ASSERT_EQ(info.dstRect.x, DEFAULT_POSITION);
    ASSERT_EQ(info.dstRect.y, DEFAULT_POSITION);
    ASSERT_EQ(info.dstRect.w, DEFAULT_WIDTH);
    ASSERT_EQ(info.dstRect.h, DEFAULT_HEIGHT);
}

/**
 * @tc.name: IsYUVBufferFormat001
 * @tc.desc: IsYUVBufferFormat, buffer is nullptr && format is invalid
 * @tc.type: FUNC
 * @tc.require: issueIAZAWR
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, IsYUVBufferFormat001, TestSize.Level1)
{
    bool ret = RSUniHwcPrevalidateParamUtil::IsYUVBufferFormat(nullptr);
    ASSERT_EQ(ret, false);

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto bufferHandle = surfaceNode->surfaceHandler_->buffer_.buffer->GetBufferHandle();
    ASSERT_NE(bufferHandle, nullptr);
    bufferHandle->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_1010102;
    ret = RSUniHwcPrevalidateParamUtil::IsYUVBufferFormat(surfaceNode->surfaceHandler_->buffer_.buffer);
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
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto bufferHandle = surfaceNode->surfaceHandler_->buffer_.buffer->GetBufferHandle();
    ASSERT_NE(bufferHandle, nullptr);
    bufferHandle->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YUV_422_I;
    bool ret = RSUniHwcPrevalidateParamUtil::IsYUVBufferFormat(surfaceNode->surfaceHandler_->buffer_.buffer);
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
    bool ret = RSUniHwcPrevalidateParamUtil::IsNeedDssRotate(GraphicTransformType::GRAPHIC_ROTATE_NONE);
    ASSERT_EQ(ret, false);
    ret = RSUniHwcPrevalidateParamUtil::IsNeedDssRotate(GraphicTransformType::GRAPHIC_ROTATE_90);
    ASSERT_EQ(ret, true);
    ret = RSUniHwcPrevalidateParamUtil::IsNeedDssRotate(GraphicTransformType::GRAPHIC_ROTATE_180);
    ASSERT_EQ(ret, false);
    ret = RSUniHwcPrevalidateParamUtil::IsNeedDssRotate(GraphicTransformType::GRAPHIC_ROTATE_270);
    ASSERT_EQ(ret, true);
    ret = RSUniHwcPrevalidateParamUtil::IsNeedDssRotate(GraphicTransformType::GRAPHIC_FLIP_H);
    ASSERT_EQ(ret, false);
    ret = RSUniHwcPrevalidateParamUtil::IsNeedDssRotate(GraphicTransformType::GRAPHIC_FLIP_V);
    ASSERT_EQ(ret, false);
    ret = RSUniHwcPrevalidateParamUtil::IsNeedDssRotate(GraphicTransformType::GRAPHIC_FLIP_H_ROT90);
    ASSERT_EQ(ret, true);
    ret = RSUniHwcPrevalidateParamUtil::IsNeedDssRotate(GraphicTransformType::GRAPHIC_FLIP_V_ROT90);
    ASSERT_EQ(ret, true);
    ret = RSUniHwcPrevalidateParamUtil::IsNeedDssRotate(GraphicTransformType::GRAPHIC_FLIP_H_ROT180);
    ASSERT_EQ(ret, false);
    ret = RSUniHwcPrevalidateParamUtil::IsNeedDssRotate(GraphicTransformType::GRAPHIC_FLIP_V_ROT180);
    ASSERT_EQ(ret, false);
    ret = RSUniHwcPrevalidateParamUtil::IsNeedDssRotate(GraphicTransformType::GRAPHIC_FLIP_H_ROT270);
    ASSERT_EQ(ret, true);
    ret = RSUniHwcPrevalidateParamUtil::IsNeedDssRotate(GraphicTransformType::GRAPHIC_FLIP_V_ROT270);
    ASSERT_EQ(ret, true);
    ret = RSUniHwcPrevalidateParamUtil::IsNeedDssRotate(GraphicTransformType::GRAPHIC_ROTATE_BUTT);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: CreateScreenNodeLayerInfo001
 * @tc.desc: CreateScreenNodeLayerInfo, node is nullptr
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CreateScreenNodeLayerInfo001, TestSize.Level1)
{
    RequestLayerInfo info;
    bool ret = RSUniHwcPrevalidateUtil::CreateScreenNodeLayerInfo(
        DEFAULT_Z_ORDER, nullptr, defaultProperty_, info);
    ASSERT_EQ(info.fps, DEFAULT_FPS);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: CreateScreenNodeLayerInfo002
 * @tc.desc: CreateScreenNodeLayerInfo, input displayNode
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CreateScreenNodeLayerInfo002, TestSize.Level1)
{
    NodeId id = 0;
    ScreenId screenId = 1;
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    ASSERT_NE(screenNode, nullptr);
    RequestLayerInfo info;
    bool ret = RSUniHwcPrevalidateUtil::CreateScreenNodeLayerInfo(
        DEFAULT_Z_ORDER, screenNode, defaultProperty_, info);
    ASSERT_EQ(info.fps, DEFAULT_FPS);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: CreateScreenNodeLayerInfo003
 * @tc.desc: CreateScreenNodeLayerInfo, input displayNode has drawable
 * @tc.type: FUNC
 * @tc.require: issueICNEE1
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CreateScreenNodeLayerInfo003, TestSize.Level1)
{
    NodeId id = 0;
    ScreenId screenId = 1;
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    ASSERT_NE(screenNode, nullptr);
    if (screenNode->GetRenderDrawable() == nullptr) {
        screenNode->renderDrawable_ = std::make_shared<DrawableV2::RSScreenRenderNodeDrawable>(screenNode);
    }
    RequestLayerInfo info;
    bool ret = RSUniHwcPrevalidateUtil::CreateScreenNodeLayerInfo(
        DEFAULT_Z_ORDER, screenNode, defaultProperty_, info);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: CreateScreenNodeLayerInfo004
 * @tc.desc: CreateScreenNodeLayerInfo with null surfaceHandler
 * @tc.type: FUNC
 * @tc.require: #IBIA3V
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CreateScreenNodeLayerInfo004, TestSize.Level1)
{
    NodeId id = 0;
    ScreenId screenId = 1;
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    ASSERT_NE(screenNode, nullptr);
    if (screenNode->GetRenderDrawable() == nullptr) {
        screenNode->renderDrawable_ = std::make_shared<DrawableV2::RSScreenRenderNodeDrawable>(screenNode);
    }
    auto screenDrawable = static_cast<DrawableV2::RSScreenRenderNodeDrawable*>(screenNode->GetRenderDrawable().get());
    ASSERT_NE(screenDrawable, nullptr);
    
    screenDrawable->surfaceHandler_ = nullptr;
    
    RequestLayerInfo info;
    bool ret = RSUniHwcPrevalidateUtil::CreateScreenNodeLayerInfo(
        DEFAULT_Z_ORDER, screenNode, defaultProperty_, info);
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
    RequestLayerInfo info;
    bool ret = RSUniHwcPrevalidateUtil::CreateRCDLayerInfo(nullptr, defaultProperty_, info);
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
    NodeId id = 1;
    auto node = std::make_shared<RSRcdSurfaceRenderNode>(id, RCDSurfaceType::BOTTOM);
    ASSERT_NE(node, nullptr);
    RequestLayerInfo info;
    bool ret = RSUniHwcPrevalidateUtil::CreateRCDLayerInfo(node, defaultProperty_, info);
    ASSERT_EQ(info.fps, DEFAULT_FPS);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: CreateRCDLayerInfo003
 * @tc.desc: CreateRCDLayerInfo, input RCDSurfaceNode has buffer
 * @tc.type: FUNC
 * @tc.require: issueICNEE1
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CreateRCDLayerInfo003, TestSize.Level1)
{
    auto node = RSTestUtil::CreateRcdNodeWithBuffer();
    ASSERT_NE(node, nullptr);
    RequestLayerInfo info;
    bool ret = RSUniHwcPrevalidateUtil::CreateRCDLayerInfo(node, defaultProperty_, info);
    ASSERT_EQ(info.fps, DEFAULT_FPS);
    ASSERT_EQ(ret, true);
}

/**
 * @tc.name: CheckIfDoArsrPre001
 * @tc.desc: CheckIfDoArsrPre, input normal surfacenode
 * @tc.type: FUNC
 * @tc.require: issueIBA6PF
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CheckIfDoArsrPre001, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto bufferHandle = surfaceNode->surfaceHandler_->buffer_.buffer->GetBufferHandle();
    ASSERT_NE(bufferHandle, nullptr);
    bufferHandle->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YUV_422_I;
    bool ret = RSUniHwcPrevalidateParamUtil::CheckIfDoArsrPre(surfaceNode->surfaceHandler_->buffer_.buffer, "testNode");
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
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    bool ret = RSUniHwcPrevalidateParamUtil::CheckIfDoArsrPre(nullptr, "testNode");
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: CheckIfDoArsrPre003
 * @tc.desc: CheckIfDoArsrPre, non-YUV buffer but nodeName in VIDEO_LAYERS
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CheckIfDoArsrPre003, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto bufferHandle = surfaceNode->surfaceHandler_->buffer_.buffer->GetBufferHandle();
    ASSERT_NE(bufferHandle, nullptr);
    bufferHandle->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888;
    bool ret = RSUniHwcPrevalidateParamUtil::CheckIfDoArsrPre(
        surfaceNode->surfaceHandler_->buffer_.buffer, "xcomponentIdSurface");
    ASSERT_EQ(ret, true);
}

/**
 * @tc.name: CheckIfDoArsrPre004
 * @tc.desc: CheckIfDoArsrPre, non-YUV buffer and nodeName not in VIDEO_LAYERS (else branch, return false)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CheckIfDoArsrPre004, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto bufferHandle = surfaceNode->surfaceHandler_->buffer_.buffer->GetBufferHandle();
    ASSERT_NE(bufferHandle, nullptr);
    bufferHandle->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888;
    bool ret = RSUniHwcPrevalidateParamUtil::CheckIfDoArsrPre(
        surfaceNode->surfaceHandler_->buffer_.buffer, "testNode");
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: CheckIfDoCopybit001
 * @tc.desc: CheckIfDoCopybit, input normal surfacenode
 * @tc.type: FUNC
 * @tc.require: issueIBZZJT
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, ApplyCopybit001, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto bufferHandle = surfaceNode->surfaceHandler_->buffer_.buffer->GetBufferHandle();
    ASSERT_NE(bufferHandle, nullptr);
    bufferHandle->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YUV_422_I;
    RequestLayerInfo info;
    // buffer is nullptr: early return, CopybitTag stays false
    RSUniHwcPrevalidateParamUtil::ApplyCopybit(nullptr,
        GraphicTransformType::GRAPHIC_ROTATE_90, info, surfaceNode);
    ASSERT_EQ(surfaceNode->GetCopybitTag(), false);
    // valid buffer, YUV format + ROTATE_90: IsYUVBufferFormat && IsNeedDssRotate both true, node non-null
    RSUniHwcPrevalidateParamUtil::ApplyCopybit(surfaceNode->surfaceHandler_->buffer_.buffer,
        GraphicTransformType::GRAPHIC_ROTATE_90, info, surfaceNode);
    ASSERT_EQ(surfaceNode->GetCopybitTag(), true);
}

/**
 * @tc.name: CheckIfDoCopybit002
 * @tc.desc: CheckIfDoCopybit, input normal surfacenode
 * @tc.type: FUNC
 * @tc.require: issueIBZZJT
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, ApplyCopybit002, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    RequestLayerInfo info;
    RSUniHwcPrevalidateParamUtil::ApplyCopybit(nullptr, GraphicTransformType::GRAPHIC_ROTATE_90, info, surfaceNode);
    ASSERT_EQ(surfaceNode->GetCopybitTag(), false);
}

/**
 * @tc.name: CheckIfDoCopybit004
 * @tc.desc: CheckIfDoCopybit, input normal surfacenode
 * @tc.type: FUNC
 * @tc.require: issueICNEE1
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, ApplyCopybit004, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto bufferHandle = surfaceNode->surfaceHandler_->buffer_.buffer->GetBufferHandle();
    ASSERT_NE(bufferHandle, nullptr);
    // YUV format but ROTATE_NONE: IsYUVBufferFormat true, IsNeedDssRotate false
    bufferHandle->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YUV_422_I;
    RequestLayerInfo info;
    RSUniHwcPrevalidateParamUtil::ApplyCopybit(surfaceNode->surfaceHandler_->buffer_.buffer,
        GraphicTransformType::GRAPHIC_ROTATE_NONE, info, surfaceNode);
    ASSERT_EQ(surfaceNode->GetCopybitTag(), false);
    ASSERT_EQ(info.perFrameParameters.find("TryToDoCopybit"), info.perFrameParameters.end());
    // RGBA format but ROTATE_90: IsYUVBufferFormat false (short-circuit)
    bufferHandle->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_1010102;
    RSUniHwcPrevalidateParamUtil::ApplyCopybit(surfaceNode->surfaceHandler_->buffer_.buffer,
        GraphicTransformType::GRAPHIC_ROTATE_90, info, surfaceNode);
    ASSERT_EQ(surfaceNode->GetCopybitTag(), false);
    ASSERT_EQ(info.perFrameParameters.find("TryToDoCopybit"), info.perFrameParameters.end());
}

/**
 * @tc.name: CopyCldInfo001
 * @tc.desc: CopyCldInfo
 * @tc.type: FUNC
 * @tc.require: issueIBA6PF
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CopyCldInfo001, TestSize.Level1)
{
    NodeId id = 1;
    auto node = std::make_shared<RSRcdSurfaceRenderNode>(id, RCDSurfaceType::BOTTOM);
    ASSERT_NE(node, nullptr);
    RequestLayerInfo info;
    RSUniHwcPrevalidateUtil::CopyCldInfo(node->GetCldInfo(), info);
}

/**
 * @tc.name: LayerRotate001
 * @tc.desc: LayerRotate
 * @tc.type: FUNC
 * @tc.require: issueIBA6PF
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, LayerRotate001, TestSize.Level1)
{
    RequestLayerInfo info;
    info.dstRect = {50, 50, 100, 200};
    sptr<IConsumerSurface> cSurface = nullptr;
    RSUniHwcPrevalidateUtil::LayerRotate(info, cSurface);
    ASSERT_EQ(info.dstRect.w, 100);
    ASSERT_EQ(info.dstRect.h, 200);
    
    cSurface = IConsumerSurface::Create();
    RSUniHwcPrevalidateUtil::LayerRotate(info, cSurface);
    ASSERT_EQ(info.dstRect.w, 100);
    ASSERT_EQ(info.dstRect.h, 200);
}

/**
 * @tc.name: CollectSurfaceNodeLayerInfo001
 * @tc.desc: CollectSurfaceNodeLayerInfo
 * @tc.type: FUNC
 * @tc.require: issueIBA6PF
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CollectSurfaceNodeLayerInfo001, TestSize.Level1)
{
    std::vector<RequestLayerInfo> prevalidLayers;
    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(0, 0, rsContext);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    screenNode->curMainAndLeashSurfaceNodes_.push_back(surfaceNode);
    uint32_t zOrder = DEFAULT_Z_ORDER;
    RSUniHwcPrevalidateUtil::CollectSurfaceNodeLayerInfo(prevalidLayers, screenNode, zOrder,
        defaultProperty_);
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
HWTEST_F(RSUniHwcPrevalidateUtilTest, ApplyCopybit003, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto bufferHandle = surfaceNode->surfaceHandler_->buffer_.buffer->GetBufferHandle();
    ASSERT_NE(bufferHandle, nullptr);
    bufferHandle->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YUV_422_I;
    RequestLayerInfo info;
    RSUniHwcPrevalidateParamUtil::ApplyCopybit(surfaceNode->surfaceHandler_->buffer_.buffer,
        GraphicTransformType::GRAPHIC_ROTATE_90, info, surfaceNode);
    if (RSSystemParameters::GetIsCopybitSupported()) {
        ASSERT_EQ(surfaceNode->GetCopybitTag(), true);
    } else {
        ASSERT_EQ(surfaceNode->GetCopybitTag(), false);
    }
}

/**
 * @tc.name: CollectSurfaceNodeLayerInfo002
 * @tc.desc: CollectSurfaceNodeLayerInfo
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CollectSurfaceNodeLayerInfo002, TestSize.Level1)
{
    std::vector<RequestLayerInfo> prevalidLayers;
    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(0, 0, rsContext);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto hwcNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(hwcNode, nullptr);

    hwcNode->isOnTheTree_ = true;
    hwcNode->dstRect_ = { DEFAULT_POSITION, DEFAULT_POSITION, DEFAULT_WIDTH, DEFAULT_HEIGHT };
    hwcNode->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    hwcNode->isHardwareEnabledNode_ = true;
    screenNode->curMainAndLeashSurfaceNodes_.emplace_back(surfaceNode);
    screenNode->GetAllHwcNodeAndFilterNode().push_back(hwcNode);
    screenNode->UpdateChildHwcNode();

    uint32_t zOrder = DEFAULT_Z_ORDER;
    RSUniHwcPrevalidateUtil::CollectSurfaceNodeLayerInfo(prevalidLayers, screenNode, zOrder,
        defaultProperty_);
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
    std::vector<RequestLayerInfo> prevalidLayers;
    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(0, 0, rsContext);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto hwcNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(hwcNode, nullptr);

    hwcNode->isOnTheTree_ = true;
    hwcNode->nodeType_ = RSSurfaceNodeType::CURSOR_NODE;
    hwcNode->isHardwareEnabledNode_ = true;
    hwcNode->name_ = "pointer window";
    screenNode->curMainAndLeashSurfaceNodes_.emplace_back(surfaceNode);
    screenNode->childHwcNodes_.push_back(hwcNode);

    uint32_t zOrder = DEFAULT_Z_ORDER;
    RSUniHwcPrevalidateUtil::CollectSurfaceNodeLayerInfo(prevalidLayers, screenNode, zOrder,
        defaultProperty_);
    ASSERT_EQ(prevalidLayers.size(), 1);
}

/**
 * @tc.name: CollectSurfaceNodeLayerInfo004
 * @tc.desc: CheckCollectSurfaceNodeLayerInfo when isOnTheTree_ is true
 * @tc.type: FUNC
 * @tc.require: issueICV4A9
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CollectSurfaceNodeLayerInfo004, TestSize.Level1)
{
    std::vector<RequestLayerInfo> prevalidLayers;
    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(0, 0, rsContext);
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode1, nullptr);

    surfaceNode1->isOnTheTree_ = true;
    surfaceNode1->nodeType_ = RSSurfaceNodeType::CURSOR_NODE;
    surfaceNode1->name_ = "pointer window";

    auto surfaceNode2 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode2, nullptr);
    surfaceNode2->isOnTheTree_ = true;
    auto hwcNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(hwcNode, nullptr);
    hwcNode->isOnTheTree_ = false;
    hwcNode->dstRect_ = { DEFAULT_POSITION, DEFAULT_POSITION, DEFAULT_WIDTH, DEFAULT_HEIGHT };

    screenNode->curMainAndLeashSurfaceNodes_.emplace_back(surfaceNode1);
    screenNode->curMainAndLeashSurfaceNodes_.emplace_back(surfaceNode2);
    screenNode->GetAllHwcNodeAndFilterNode().push_back(hwcNode);
    screenNode->UpdateChildHwcNode();

    uint32_t zOrder = DEFAULT_Z_ORDER;
    RSUniHwcPrevalidateUtil::CollectSurfaceNodeLayerInfo(prevalidLayers, screenNode, zOrder,
        defaultProperty_);
    ASSERT_EQ(prevalidLayers.size(), 1);
}

/**
 * @tc.name: UpdateSurfaceLayerUsage
 * @tc.desc: UpdateSurfaceLayerUsage
 * @tc.type: FUNC
 * @tc.require: issueIAZAWR
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, UpdateSurfaceLayerUsage, TestSize.Level1)
{
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    surfaceNode1->nodeType_ = RSSurfaceNodeType::CURSOR_NODE;
    surfaceNode1->isHardCursor_ = true;
    surfaceNode1->blendType_ = GraphicBlendType::GRAPHIC_BLEND_NONE;
    RequestLayerInfo info;
    ASSERT_EQ(surfaceNode1->GetBlendType(), 0);
    RSUniHwcPrevalidateUtil::UpdateSurfaceLayerUsage(surfaceNode1, info, true);
}

/**
 * @tc.name: SetVcldInfoTest
 * @tc.desc: Test SetVcldInfoTest
 * @tc.type: FUNC
 * @tc.require: issueIAZAWR
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, SetVcldInfoTest, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    node->InitRenderParams();
    ASSERT_NE(node, nullptr);
    RSVcldParam vcldInfo0;
    node->SetVcldInfo(vcldInfo0);
    ASSERT_FALSE(node->GetVcldInfo().enable);
    RSVcldParam vcldInfo1;
    vcldInfo1.enable = true;
    vcldInfo1.radius = 20;
    node->SetVcldInfo(vcldInfo1);
    ASSERT_TRUE(node->GetVcldInfo().enable);
}

/**
 * @tc.name: ResetVcldInfoTest
 * @tc.desc: Test ResetVcldInfoTest
 * @tc.type: FUNC
 * @tc.require: issueIAZAWR
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, ResetVcldInfoTest, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(node, nullptr);
    node->stagingRenderParams_ = nullptr;
    ASSERT_EQ(node->stagingRenderParams_, nullptr);
    node->ResetVcldInfo();
}

/**
 * @tc.name: CollectSurfaceNodeLayerInfo005
 * @tc.desc: Test CollectSurfaceNodeLayerInfo with null screenNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CollectSurfaceNodeLayerInfo005, TestSize.Level2)
{
    std::vector<RequestLayerInfo> prevalidLayers;
    uint32_t zOrder = DEFAULT_Z_ORDER;
    RSUniHwcPrevalidateUtil::CollectSurfaceNodeLayerInfo(prevalidLayers, nullptr, zOrder, defaultProperty_);
    ASSERT_EQ(prevalidLayers.size(), 0);
}

/**
 * @tc.name: CreateSurfaceNodeLayerInfoHmsAppEnabled
 * @tc.desc: Verify HMS app with value "1" in SourceTuningForHmsApp skips ARSR enhancement
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CreateSurfaceNodeLayerInfoHmsAppEnabled, TestSize.Level1)
{
    if (!RSSystemParameters::GetArsrPreEnabled()) {
        return;
    }
    HWCParam::SetSourceTuningForHmsApp("com.hms.excluded.app", "1");

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto bufferHandle = surfaceNode->surfaceHandler_->buffer_.buffer->GetBufferHandle();
    ASSERT_NE(bufferHandle, nullptr);
    bufferHandle->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YUV_422_I;
    surfaceNode->bundleName_ = "com.hms.excluded.app";

    RequestLayerInfo info;
    uint32_t zOrder = DEFAULT_Z_ORDER;
    bool ret = RSUniHwcPrevalidateUtil::CreateSurfaceNodeLayerInfo(
        zOrder, surfaceNode, defaultProperty_, info);
    ASSERT_EQ(ret, true);
    EXPECT_EQ(surfaceNode->GetArsrTag(), false);
    EXPECT_EQ(info.perFrameParameters.find("ArsrDoEnhance"), info.perFrameParameters.end());
}

/**
 * @tc.name: CreateSurfaceNodeLayerInfoHmsAppNotEnabled
 * @tc.desc: Verify HMS app with value "0" in SourceTuningForHmsApp applies normal ARSR enhancement
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CreateSurfaceNodeLayerInfoHmsAppNotEnabled, TestSize.Level1)
{
    if (!RSSystemParameters::GetArsrPreEnabled()) {
        return;
    }
    HWCParam::SetSourceTuningForHmsApp("com.hms.disabled.app", "0");

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto bufferHandle = surfaceNode->surfaceHandler_->buffer_.buffer->GetBufferHandle();
    ASSERT_NE(bufferHandle, nullptr);
    bufferHandle->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YUV_422_I;
    surfaceNode->bundleName_ = "com.hms.disabled.app";

    RequestLayerInfo info;
    uint32_t zOrder = DEFAULT_Z_ORDER;
    bool ret = RSUniHwcPrevalidateUtil::CreateSurfaceNodeLayerInfo(
        zOrder, surfaceNode, defaultProperty_, info);
    ASSERT_EQ(ret, true);
    EXPECT_EQ(surfaceNode->GetArsrTag(), true);
    auto it = info.perFrameParameters.find("ArsrDoEnhance");
    EXPECT_NE(it, info.perFrameParameters.end());
    if (it != info.perFrameParameters.end()) {
        EXPECT_EQ(it->second[0], 1);
    }
}

/**
 * @tc.name: CreateSurfaceNodeLayerInfoHmsAppNotFound
 * @tc.desc: Verify app not in HMS map gets normal ARSR enhancement
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CreateSurfaceNodeLayerInfoHmsAppNotFound, TestSize.Level1)
{
    if (!RSSystemParameters::GetArsrPreEnabled()) {
        return;
    }

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto bufferHandle = surfaceNode->surfaceHandler_->buffer_.buffer->GetBufferHandle();
    ASSERT_NE(bufferHandle, nullptr);
    bufferHandle->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YUV_422_I;
    surfaceNode->bundleName_ = "com.normal.app";

    RequestLayerInfo info;
    uint32_t zOrder = DEFAULT_Z_ORDER;
    bool ret = RSUniHwcPrevalidateUtil::CreateSurfaceNodeLayerInfo(
        zOrder, surfaceNode, defaultProperty_, info);
    ASSERT_EQ(ret, true);
    EXPECT_EQ(surfaceNode->GetArsrTag(), true);
    auto it = info.perFrameParameters.find("ArsrDoEnhance");
    EXPECT_NE(it, info.perFrameParameters.end());
    if (it != info.perFrameParameters.end()) {
        EXPECT_EQ(it->second[0], 1);
    }
}

/**
 * @tc.name: CreateSurfaceNodeLayerInfoHmsAppArsrPreDisabled
 * @tc.desc: Verify when IsArsrPreEnabled() is false HMS app config has no effect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CreateSurfaceNodeLayerInfoHmsAppArsrPreDisabled, TestSize.Level1)
{
    if (RSSystemParameters::GetArsrPreEnabled()) {
        return;
    }
    HWCParam::SetSourceTuningForHmsApp("com.hms.disabled.arsr", "1");

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto bufferHandle = surfaceNode->surfaceHandler_->buffer_.buffer->GetBufferHandle();
    ASSERT_NE(bufferHandle, nullptr);
    bufferHandle->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YUV_422_I;
    surfaceNode->bundleName_ = "com.hms.disabled.arsr";

    RequestLayerInfo info;
    uint32_t zOrder = DEFAULT_Z_ORDER;
    bool ret = RSUniHwcPrevalidateUtil::CreateSurfaceNodeLayerInfo(
        zOrder, surfaceNode, defaultProperty_, info);
    ASSERT_EQ(ret, true);
    EXPECT_EQ(info.perFrameParameters.find("ArsrDoEnhance"), info.perFrameParameters.end());
}

/**
 * @tc.name: CreateSurfaceNodeLayerInfoHmsAppNonYuvBuffer
 * @tc.desc: Verify HMS app config only applies when CheckIfDoArsrPre returns true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CreateSurfaceNodeLayerInfoHmsAppNonYuvBuffer, TestSize.Level1)
{
    if (!RSSystemParameters::GetArsrPreEnabled()) {
        return;
    }
    HWCParam::SetSourceTuningForHmsApp("com.hms.rgba.app", "1");

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto bufferHandle = surfaceNode->surfaceHandler_->buffer_.buffer->GetBufferHandle();
    ASSERT_NE(bufferHandle, nullptr);
    bufferHandle->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888;
    surfaceNode->bundleName_ = "com.hms.rgba.app";

    RequestLayerInfo info;
    uint32_t zOrder = DEFAULT_Z_ORDER;
    bool ret = RSUniHwcPrevalidateUtil::CreateSurfaceNodeLayerInfo(
        zOrder, surfaceNode, defaultProperty_, info);
    ASSERT_EQ(ret, true);
    EXPECT_EQ(info.perFrameParameters.find("ArsrDoEnhance"), info.perFrameParameters.end());
}

/**
 * @tc.name: CollectLayerInfo001
 * @tc.desc: CollectLayerInfo, screenRenderNode is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CollectLayerInfo001, TestSize.Level1)
{
    auto ret = RSUniHwcPrevalidateUtil::CollectLayerInfo(nullptr, DEFAULT_Z_ORDER);
    ASSERT_EQ(ret.size(), 0);
}

/**
 * @tc.name: CollectLayerInfo002
 * @tc.desc: CollectLayerInfo, prevalidLayers is not empty after CollectSurfaceNodeLayerInfo
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CollectLayerInfo002, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(0, 0, rsContext);
    ASSERT_NE(screenNode, nullptr);
    auto hwcNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(hwcNode, nullptr);
    hwcNode->isOnTheTree_ = true;
    hwcNode->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    hwcNode->isHardwareEnabledNode_ = true;
    hwcNode->dstRect_ = { DEFAULT_POSITION, DEFAULT_POSITION, DEFAULT_WIDTH, DEFAULT_HEIGHT };
    screenNode->GetAllHwcNodeAndFilterNode().push_back(RSRenderNode::WeakPtr(hwcNode));
    screenNode->UpdateChildHwcNode();
 
    auto ret = RSUniHwcPrevalidateUtil::CollectLayerInfo(screenNode, DEFAULT_Z_ORDER);
    ASSERT_EQ(ret.size(), 1);
}

/**
 * @tc.name: CollectLayerInfo003
 * @tc.desc: CollectLayerInfo, CreateScreenNodeLayerInfo returns true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CollectLayerInfo003, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(0, 0, rsContext);
    ASSERT_NE(screenNode, nullptr);
    auto hwcNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(hwcNode, nullptr);
    hwcNode->isOnTheTree_ = true;
    hwcNode->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    hwcNode->isHardwareEnabledNode_ = true;
    hwcNode->dstRect_ = { DEFAULT_POSITION, DEFAULT_POSITION, DEFAULT_WIDTH, DEFAULT_HEIGHT };
    screenNode->GetAllHwcNodeAndFilterNode().push_back(RSRenderNode::WeakPtr(hwcNode));
    screenNode->UpdateChildHwcNode();
 
    auto tempNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(tempNode, nullptr);
    screenNode->renderDrawable_ = std::make_shared<DrawableV2::RSScreenRenderNodeDrawable>(screenNode);
    auto screenDrawable = static_cast<DrawableV2::RSScreenRenderNodeDrawable*>(
        screenNode->GetRenderDrawable().get());
    ASSERT_NE(screenDrawable, nullptr);
    screenDrawable->surfaceHandler_ = tempNode->GetRSSurfaceHandler();
 
    auto ret = RSUniHwcPrevalidateUtil::CollectLayerInfo(screenNode, DEFAULT_Z_ORDER);
    ASSERT_EQ(ret.size(), 2);
}

/**
 * @tc.name: AddRcdLayers001
 * @tc.desc: AddRcdLayers, GetRcdEnable is false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, AddRcdLayers001, TestSize.Level1)
{
    auto origParam = OHOS::system::GetParameter("persist.rosen.screenroundcornerrcd.enabled", "1");
    OHOS::system::SetParameter("persist.rosen.screenroundcornerrcd.enabled", "0");
 
    std::vector<RequestLayerInfo> prevalidLayers;
    RSUniHwcPrevalidateUtil::AddRcdLayers(prevalidLayers, 0, defaultProperty_);
    ASSERT_EQ(prevalidLayers.size(), 0);
 
    OHOS::system::SetParameter("persist.rosen.screenroundcornerrcd.enabled", origParam.c_str());
}

/**
 * @tc.name: AddRcdLayers002
 * @tc.desc: AddRcdLayers, bottom CreateRCDLayerInfo returns true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, AddRcdLayers002, TestSize.Level1)
{
    NodeId screenId = 0;
    auto& rcdManager = RSRcdRenderManager::GetInstance();
    auto rcdNode = RSTestUtil::CreateRcdNodeWithBuffer();
    ASSERT_NE(rcdNode, nullptr);
    rcdManager.bottomSurfaceNodeMap_[screenId] = rcdNode;
    rcdManager.topSurfaceNodeMap_.erase(screenId);
 
    std::vector<RequestLayerInfo> prevalidLayers;
    RSUniHwcPrevalidateUtil::AddRcdLayers(prevalidLayers, screenId, defaultProperty_);
    ASSERT_EQ(prevalidLayers.size(), 1);
 
    rcdManager.bottomSurfaceNodeMap_.erase(screenId);
}

/**
 * @tc.name: AddRcdLayers003
 * @tc.desc: AddRcdLayers, bottom CreateRCDLayerInfo returns false (no buffer)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, AddRcdLayers003, TestSize.Level1)
{
    NodeId screenId = 0;
    auto& rcdManager = RSRcdRenderManager::GetInstance();
    auto rcdNode = RSRcdSurfaceRenderNode::Create(999, RCDSurfaceType::BOTTOM);
    ASSERT_NE(rcdNode, nullptr);
    auto csurf = IConsumerSurface::Create("RcdBottomNoBuffer");
    rcdNode->SetConsumer(csurf);
    rcdManager.bottomSurfaceNodeMap_[screenId] = rcdNode;
    rcdManager.topSurfaceNodeMap_.erase(screenId);
 
    std::vector<RequestLayerInfo> prevalidLayers;
    RSUniHwcPrevalidateUtil::AddRcdLayers(prevalidLayers, screenId, defaultProperty_);
    ASSERT_EQ(prevalidLayers.size(), 0);
 
    rcdManager.bottomSurfaceNodeMap_.erase(screenId);
}

/**
 * @tc.name: AddRcdLayers004
 * @tc.desc: AddRcdLayers, top CreateRCDLayerInfo returns true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, AddRcdLayers004, TestSize.Level1)
{
    NodeId screenId = 0;
    auto& rcdManager = RSRcdRenderManager::GetInstance();
    rcdManager.bottomSurfaceNodeMap_.erase(screenId);
    auto rcdNode = RSTestUtil::CreateRcdNodeWithBuffer();
    ASSERT_NE(rcdNode, nullptr);
    rcdManager.topSurfaceNodeMap_[screenId] = rcdNode;
 
    std::vector<RequestLayerInfo> prevalidLayers;
    RSUniHwcPrevalidateUtil::AddRcdLayers(prevalidLayers, screenId, defaultProperty_);
    ASSERT_EQ(prevalidLayers.size(), 1);
 
    rcdManager.topSurfaceNodeMap_.erase(screenId);
}

/**
 * @tc.name: AddRcdLayers005
 * @tc.desc: AddRcdLayers, top CreateRCDLayerInfo returns false (no buffer)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, AddRcdLayers005, TestSize.Level1)
{
    NodeId screenId = 0;
    auto& rcdManager = RSRcdRenderManager::GetInstance();
    rcdManager.bottomSurfaceNodeMap_.erase(screenId);
    auto rcdNode = RSRcdSurfaceRenderNode::Create(998, RCDSurfaceType::TOP);
    ASSERT_NE(rcdNode, nullptr);
    auto csurf = IConsumerSurface::Create("RcdTopNoBuffer");
    rcdNode->SetConsumer(csurf);
    rcdManager.topSurfaceNodeMap_[screenId] = rcdNode;
 
    std::vector<RequestLayerInfo> prevalidLayers;
    RSUniHwcPrevalidateUtil::AddRcdLayers(prevalidLayers, screenId, defaultProperty_);
    ASSERT_EQ(prevalidLayers.size(), 0);
 
    rcdManager.topSurfaceNodeMap_.erase(screenId);
}

/**
 * @tc.name: AddRcdLayers006
 * @tc.desc: AddRcdLayers, both bottom and top CreateRCDLayerInfo return true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, AddRcdLayers006, TestSize.Level1)
{
    NodeId screenId = 0;
    auto& rcdManager = RSRcdRenderManager::GetInstance();
    auto bottomNode = RSTestUtil::CreateRcdNodeWithBuffer();
    ASSERT_NE(bottomNode, nullptr);
    auto topNode = RSTestUtil::CreateRcdNodeWithBuffer();
    ASSERT_NE(topNode, nullptr);
    rcdManager.bottomSurfaceNodeMap_[screenId] = bottomNode;
    rcdManager.topSurfaceNodeMap_[screenId] = topNode;
 
    std::vector<RequestLayerInfo> prevalidLayers;
    RSUniHwcPrevalidateUtil::AddRcdLayers(prevalidLayers, screenId, defaultProperty_);
    ASSERT_EQ(prevalidLayers.size(), 2);
 
    rcdManager.bottomSurfaceNodeMap_.erase(screenId);
    rcdManager.topSurfaceNodeMap_.erase(screenId);
}

/**
 * @tc.name: CollectSurfaceNodeLayerInfo006
 * @tc.desc: CollectSurfaceNodeLayerInfo, CheckHwcNode returns false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, CollectSurfaceNodeLayerInfo006, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(0, 0, rsContext);
    ASSERT_NE(screenNode, nullptr);
    auto hwcNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(hwcNode, nullptr);
    hwcNode->isOnTheTree_ = false;
    hwcNode->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    hwcNode->isHardwareEnabledNode_ = true;
    hwcNode->dstRect_ = { DEFAULT_POSITION, DEFAULT_POSITION, DEFAULT_WIDTH, DEFAULT_HEIGHT };
    screenNode->GetAllHwcNodeAndFilterNode().push_back(RSRenderNode::WeakPtr(hwcNode));
    screenNode->UpdateChildHwcNode();
 
    std::vector<RequestLayerInfo> prevalidLayers;
    uint32_t zOrder = DEFAULT_Z_ORDER;
    RSUniHwcPrevalidateUtil::CollectSurfaceNodeLayerInfo(prevalidLayers, screenNode, zOrder, defaultProperty_);
    ASSERT_EQ(prevalidLayers.size(), 0);
}

/**
 * @tc.name: IsYUVBufferFormat003
 * @tc.desc: IsYUVBufferFormat, format > YCRCB_P010 (third sub-condition true)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, IsYUVBufferFormat003, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto bufferHandle = surfaceNode->surfaceHandler_->buffer_.buffer->GetBufferHandle();
    ASSERT_NE(bufferHandle, nullptr);
    bufferHandle->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RAW10;
    bool ret = RSUniHwcPrevalidateParamUtil::IsYUVBufferFormat(surfaceNode->surfaceHandler_->buffer_.buffer);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: ApplyArsrEnhance001
 * @tc.desc: ApplyArsrEnhance, node is nullptr (!node = true, short-circuit)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, ApplyArsrEnhance001, TestSize.Level1)
{
    RequestLayerInfo info;
    RSUniHwcPrevalidateParamUtil::ApplyArsrEnhance(nullptr, info);
    ASSERT_EQ(info.perFrameParameters.find("ArsrDoEnhance"), info.perFrameParameters.end());
}

/**
 * @tc.name: ApplyArsrEnhance002
 * @tc.desc: ApplyArsrEnhance, node is valid but GetRSSurfaceHandler is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, ApplyArsrEnhance002, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->surfaceHandler_ = nullptr;
    RequestLayerInfo info;
    RSUniHwcPrevalidateParamUtil::ApplyArsrEnhance(surfaceNode, info);
    ASSERT_EQ(info.perFrameParameters.find("ArsrDoEnhance"), info.perFrameParameters.end());
}

/**
 * @tc.name: ApplyArsrEnhance003
 * @tc.desc: ApplyArsrEnhance, CheckIfDoArsrPre returns false (line 85, !CheckIfDoArsrPre=true)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, ApplyArsrEnhance003, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto bufferHandle = surfaceNode->surfaceHandler_->buffer_.buffer->GetBufferHandle();
    ASSERT_NE(bufferHandle, nullptr);
    bufferHandle->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888;
    RequestLayerInfo info;
    RSUniHwcPrevalidateParamUtil::ApplyArsrEnhance(surfaceNode, info);
    ASSERT_EQ(info.perFrameParameters.find("ArsrDoEnhance"), info.perFrameParameters.end());
}

/**
 * @tc.name: ApplyArsrEnhance004
 * @tc.desc: ApplyArsrEnhance, CheckIfDoArsrPre returns true, ArsrDoEnhance is set (line 94)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, ApplyArsrEnhance004, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto bufferHandle = surfaceNode->surfaceHandler_->buffer_.buffer->GetBufferHandle();
    ASSERT_NE(bufferHandle, nullptr);
    bufferHandle->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YUV_422_I;
    RequestLayerInfo info;
    RSUniHwcPrevalidateParamUtil::ApplyArsrEnhance(surfaceNode, info);
    if (!RSSystemParameters::GetArsrPreEnabled()) {
        ASSERT_EQ(info.perFrameParameters.find("ArsrDoEnhance"), info.perFrameParameters.end());
    } else {
        auto it = info.perFrameParameters.find("ArsrDoEnhance");
        ASSERT_NE(it, info.perFrameParameters.end());
    }
}

/**
 * @tc.name: ApplyCopybit005
 * @tc.desc: ApplyCopybit, buffer is null (line 113 branch: !buffer = true)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, ApplyCopybit005, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    RequestLayerInfo info;
    RSUniHwcPrevalidateParamUtil::ApplyCopybit(nullptr,
        GraphicTransformType::GRAPHIC_ROTATE_90, info, surfaceNode);
    ASSERT_EQ(surfaceNode->GetCopybitTag(), false);
    ASSERT_EQ(info.perFrameParameters.find("TryToDoCopybit"), info.perFrameParameters.end());
}

/**
 * @tc.name: ApplyCopybit006
 * @tc.desc: ApplyCopybit, IsYUVBufferFormat true but IsNeedDssRotate false (line 116 branch)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, ApplyCopybit006, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto bufferHandle = surfaceNode->surfaceHandler_->buffer_.buffer->GetBufferHandle();
    ASSERT_NE(bufferHandle, nullptr);
    bufferHandle->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YUV_422_I;
    RequestLayerInfo info;
    RSUniHwcPrevalidateParamUtil::ApplyCopybit(surfaceNode->surfaceHandler_->buffer_.buffer,
        GraphicTransformType::GRAPHIC_ROTATE_NONE, info, surfaceNode);
    ASSERT_EQ(surfaceNode->GetCopybitTag(), false);
    ASSERT_EQ(info.perFrameParameters.find("TryToDoCopybit"), info.perFrameParameters.end());
}

/**
 * @tc.name: ApplyCopybit007
 * @tc.desc: ApplyCopybit, IsYUVBufferFormat false (line 116 short-circuit)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, ApplyCopybit007, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto bufferHandle = surfaceNode->surfaceHandler_->buffer_.buffer->GetBufferHandle();
    ASSERT_NE(bufferHandle, nullptr);
    bufferHandle->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888;
    RequestLayerInfo info;
    RSUniHwcPrevalidateParamUtil::ApplyCopybit(surfaceNode->surfaceHandler_->buffer_.buffer,
        GraphicTransformType::GRAPHIC_ROTATE_90, info, surfaceNode);
    ASSERT_EQ(surfaceNode->GetCopybitTag(), false);
    ASSERT_EQ(info.perFrameParameters.find("TryToDoCopybit"), info.perFrameParameters.end());
}

/**
 * @tc.name: ApplyCopybit008
 * @tc.desc: ApplyCopybit, node is null when IsYUVBufferFormat && IsNeedDssRotate are both true (line 118 branch)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, ApplyCopybit008, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto bufferHandle = surfaceNode->surfaceHandler_->buffer_.buffer->GetBufferHandle();
    ASSERT_NE(bufferHandle, nullptr);
    bufferHandle->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YUV_422_I;
    RequestLayerInfo info;
    // node is nullptr: TryToDoCopybit is set, but SetCopybitTag is skipped
    RSUniHwcPrevalidateParamUtil::ApplyCopybit(surfaceNode->surfaceHandler_->buffer_.buffer,
        GraphicTransformType::GRAPHIC_ROTATE_90, info, nullptr);
    auto it = info.perFrameParameters.find("TryToDoCopybit");
    ASSERT_NE(it, info.perFrameParameters.end());
}

/**
 * @tc.name: ApplyVcldParam001
 * @tc.desc: ApplyVcldParam, isVcldEnabled=true but node is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, ApplyVcldParam001, TestSize.Level1)
{
    RSUniHwcPrevalidateParamUtil::SetIsVcldEnabled(true);
    RequestLayerInfo info;
    RSVcldParam vcldInfo;
    RSUniHwcPrevalidateParamUtil::ApplyVcldParam(vcldInfo, info);
    ASSERT_NE(info.perFrameParameters.find("VcldParam"), info.perFrameParameters.end());
    RSUniHwcPrevalidateParamUtil::SetIsVcldEnabled(false);
}

/**
 * @tc.name: ApplyVcldParam002
 * @tc.desc: ApplyVcldParam, isVcldEnabled=false (short-circuit return)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, ApplyVcldParam002, TestSize.Level1)
{
    RSUniHwcPrevalidateParamUtil::SetIsVcldEnabled(false);
    RequestLayerInfo info;
    RSVcldParam vcldInfo;
    RSUniHwcPrevalidateParamUtil::ApplyVcldParam(vcldInfo, info);
    ASSERT_EQ(info.perFrameParameters.find("VcldParam"), info.perFrameParameters.end());
}

/**
 * @tc.name: ApplyVcldParam003
 * @tc.desc: ApplyVcldParam, isVcldEnabled=true and node is valid (VcldParam set)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, ApplyVcldParam003, TestSize.Level1)
{
    RSUniHwcPrevalidateParamUtil::SetIsVcldEnabled(true);
    RequestLayerInfo info;
    RSVcldParam vcldInfo;
    vcldInfo.enable = true;
    RSUniHwcPrevalidateParamUtil::ApplyVcldParam(vcldInfo, info);
    auto it = info.perFrameParameters.find("VcldParam");
    ASSERT_NE(it, info.perFrameParameters.end());
    RSUniHwcPrevalidateParamUtil::SetIsVcldEnabled(false);
}

/**
 * @tc.name: ApplySourceCropTuning001
 * @tc.desc: ApplySourceCropTuning, GetVideoSurfaceFlag=true and IsYUVBufferFormat=true (if branch)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, ApplySourceCropTuning001, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto bufferHandle = surfaceNode->surfaceHandler_->buffer_.buffer->GetBufferHandle();
    ASSERT_NE(bufferHandle, nullptr);
    bufferHandle->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YUV_422_I;
    RsCommonHook::Instance().SetVideoSurfaceFlag(true);
    RequestLayerInfo info;
    RSUniHwcPrevalidateParamUtil::ApplySourceCropTuning(
        surfaceNode->surfaceHandler_->buffer_.buffer, info);
    auto it = info.perFrameParameters.find("SourceCropTuning");
    ASSERT_NE(it, info.perFrameParameters.end());
    ASSERT_EQ(it->second.size(), 1);
    ASSERT_EQ(it->second[0], 1);
    RsCommonHook::Instance().SetVideoSurfaceFlag(false);
}

/**
 * @tc.name: ApplySourceCropTuning002
 * @tc.desc: ApplySourceCropTuning, GetVideoSurfaceFlag=true and IsYUVBufferFormat=false (else branch)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, ApplySourceCropTuning002, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto bufferHandle = surfaceNode->surfaceHandler_->buffer_.buffer->GetBufferHandle();
    ASSERT_NE(bufferHandle, nullptr);
    bufferHandle->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888;
    RsCommonHook::Instance().SetVideoSurfaceFlag(true);
    RequestLayerInfo info;
    RSUniHwcPrevalidateParamUtil::ApplySourceCropTuning(
        surfaceNode->surfaceHandler_->buffer_.buffer, info);
    auto it = info.perFrameParameters.find("SourceCropTuning");
    ASSERT_NE(it, info.perFrameParameters.end());
    ASSERT_EQ(it->second.size(), 1);
    ASSERT_EQ(it->second[0], 0);
    RsCommonHook::Instance().SetVideoSurfaceFlag(false);
}

/**
 * @tc.name: ApplySourceCropTuning003
 * @tc.desc: ApplySourceCropTuning, GetVideoSurfaceFlag=false (short-circuit, else branch)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, ApplySourceCropTuning003, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto bufferHandle = surfaceNode->surfaceHandler_->buffer_.buffer->GetBufferHandle();
    ASSERT_NE(bufferHandle, nullptr);
    bufferHandle->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YUV_422_I;
    RsCommonHook::Instance().SetVideoSurfaceFlag(false);
    RequestLayerInfo info;
    RSUniHwcPrevalidateParamUtil::ApplySourceCropTuning(
        surfaceNode->surfaceHandler_->buffer_.buffer, info);
    auto it = info.perFrameParameters.find("SourceCropTuning");
    ASSERT_NE(it, info.perFrameParameters.end());
    ASSERT_EQ(it->second.size(), 1);
    ASSERT_EQ(it->second[0], 0);
}

/**
 * @tc.name: ApplyLayerLinearMatrix001
 * @tc.desc: ApplyLayerLinearMatrix, matrix size != MATRIX_SIZE (early return)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, ApplyLayerLinearMatrix001, TestSize.Level1)
{
    std::vector<float> matrix = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f};
    RequestLayerInfo info;
    RSUniHwcPrevalidateParamUtil::ApplyLayerLinearMatrix(matrix, info);
    ASSERT_EQ(info.perFrameParameters.find("LayerLinearMatrix"), info.perFrameParameters.end());
}

/**
 * @tc.name: ApplyLayerLinearMatrix002
 * @tc.desc: ApplyLayerLinearMatrix, matrix size == MATRIX_SIZE and memcpy_s succeeds (if-true branch)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, ApplyLayerLinearMatrix002, TestSize.Level1)
{
    std::vector<float> matrix = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f};
    RequestLayerInfo info;
    RSUniHwcPrevalidateParamUtil::ApplyLayerLinearMatrix(matrix, info);
    auto it = info.perFrameParameters.find("LayerLinearMatrix");
    ASSERT_NE(it, info.perFrameParameters.end());
    ASSERT_EQ(it->second.size(), 9 * sizeof(float));
}

/**
 * @tc.name: ComputeSurfaceSrcRect001
 * @tc.desc: ComputeSurfaceSrcRect, GetCropMetadata true but bufferWidth=0 (divide-by-zero guard)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, ComputeSurfaceSrcRect001, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto buffer = surfaceNode->surfaceHandler_->buffer_.buffer;
    ASSERT_NE(buffer, nullptr);
    buffer->SetCropMetadata({DEFAULT_POSITION, DEFAULT_POSITION, DEFAULT_WIDTH, DEFAULT_HEIGHT});
    auto bufferHandle = buffer->GetBufferHandle();
    ASSERT_NE(bufferHandle, nullptr);
    surfaceNode->srcRect_ = {DEFAULT_POSITION, DEFAULT_POSITION, DEFAULT_WIDTH, DEFAULT_HEIGHT};
    bufferHandle->width = 0;
    RequestLayerInfo info;
    RSUniHwcPrevalidateUtil::ComputeSurfaceSrcRect(info, surfaceNode, buffer);
    ASSERT_EQ(info.srcRect.x, static_cast<uint32_t>(DEFAULT_POSITION));
    ASSERT_EQ(info.srcRect.y, static_cast<uint32_t>(DEFAULT_POSITION));
    ASSERT_EQ(info.srcRect.w, static_cast<uint32_t>(DEFAULT_WIDTH));
    ASSERT_EQ(info.srcRect.h, static_cast<uint32_t>(DEFAULT_HEIGHT));
}

/**
 * @tc.name: ComputeSurfaceSrcRect002
 * @tc.desc: ComputeSurfaceSrcRect, GetCropMetadata true but bufferHeight=0 (divide-by-zero guard)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, ComputeSurfaceSrcRect002, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto buffer = surfaceNode->surfaceHandler_->buffer_.buffer;
    ASSERT_NE(buffer, nullptr);
    buffer->SetCropMetadata({DEFAULT_POSITION, DEFAULT_POSITION, DEFAULT_WIDTH, DEFAULT_HEIGHT});
    auto bufferHandle = buffer->GetBufferHandle();
    ASSERT_NE(bufferHandle, nullptr);
    surfaceNode->srcRect_ = {DEFAULT_POSITION, DEFAULT_POSITION, DEFAULT_WIDTH, DEFAULT_HEIGHT};
    bufferHandle->height = 0;
    RequestLayerInfo info;
    RSUniHwcPrevalidateUtil::ComputeSurfaceSrcRect(info, surfaceNode, buffer);
    ASSERT_EQ(info.srcRect.x, static_cast<uint32_t>(DEFAULT_POSITION));
    ASSERT_EQ(info.srcRect.y, static_cast<uint32_t>(DEFAULT_POSITION));
    ASSERT_EQ(info.srcRect.w, static_cast<uint32_t>(DEFAULT_WIDTH));
    ASSERT_EQ(info.srcRect.h, static_cast<uint32_t>(DEFAULT_HEIGHT));
}

/**
 * @tc.name: ComputeSurfaceSrcRect003
 * @tc.desc: ComputeSurfaceSrcRect, GetCropMetadata true but bufferWidth=0 and bufferHeight=0 (both false)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, ComputeSurfaceSrcRect003, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto buffer = surfaceNode->surfaceHandler_->buffer_.buffer;
    ASSERT_NE(buffer, nullptr);
    buffer->SetCropMetadata({DEFAULT_POSITION, DEFAULT_POSITION, DEFAULT_WIDTH, DEFAULT_HEIGHT});
    auto bufferHandle = buffer->GetBufferHandle();
    ASSERT_NE(bufferHandle, nullptr);
    surfaceNode->srcRect_ = {DEFAULT_POSITION, DEFAULT_POSITION, DEFAULT_WIDTH, DEFAULT_HEIGHT};
    bufferHandle->width = 0;
    bufferHandle->height = 0;
    RequestLayerInfo info;
    RSUniHwcPrevalidateUtil::ComputeSurfaceSrcRect(info, surfaceNode, buffer);
    ASSERT_EQ(info.srcRect.x, static_cast<uint32_t>(DEFAULT_POSITION));
    ASSERT_EQ(info.srcRect.y, static_cast<uint32_t>(DEFAULT_POSITION));
    ASSERT_EQ(info.srcRect.w, static_cast<uint32_t>(DEFAULT_WIDTH));
    ASSERT_EQ(info.srcRect.h, static_cast<uint32_t>(DEFAULT_HEIGHT));
}

/**
 * @tc.name: UpdateScreenUsage001
 * @tc.desc: UpdateScreenUsage, DUAL_SCREEN_ENTER with isUniRenderLayerNull true/false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, UpdateScreenUsage001, TestSize.Level1)
{
    RequestLayerInfo info1;
    RSUniHwcPrevalidateUtil::UpdateScreenUsage(
        DualScreenStatus::DUAL_SCREEN_ENTER, true, info1);
    ASSERT_EQ(info1.layerUsage, USAGE_UNI_LAYER_VAL | USAGE_DUAL_PREVIEW_VAL_VAL | USAGE_UNI_RENDER_LAYER_NULL);

    RequestLayerInfo info2;
    RSUniHwcPrevalidateUtil::UpdateScreenUsage(
        DualScreenStatus::DUAL_SCREEN_ENTER, false, info2);
    ASSERT_EQ(info2.layerUsage, USAGE_UNI_LAYER_VAL | USAGE_DUAL_PREVIEW_VAL_VAL);
}

/**
 * @tc.name: UpdateScreenUsage002
 * @tc.desc: UpdateScreenUsage, DUAL_SCREEN_EXIT with isUniRenderLayerNull true/false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, UpdateScreenUsage002, TestSize.Level1)
{
    RequestLayerInfo info1;
    RSUniHwcPrevalidateUtil::UpdateScreenUsage(
        DualScreenStatus::DUAL_SCREEN_EXIT, true, info1);
    ASSERT_EQ(info1.layerUsage, USAGE_UNI_LAYER_VAL | USAGE_UNI_RENDER_LAYER_NULL);

    RequestLayerInfo info2;
    RSUniHwcPrevalidateUtil::UpdateScreenUsage(
        DualScreenStatus::DUAL_SCREEN_EXIT, false, info2);
    ASSERT_EQ(info2.layerUsage, USAGE_UNI_LAYER_VAL);
}

/*
 * @tc.name: ProcessOfflineStrategy001
 * @tc.desc: Test ProcessOfflineStrategy with empty strategy (for loop not executed)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, ProcessOfflineStrategy001, TestSize.Level2)
{
    std::map<uint64_t, RequestCompositionType> strategy;
    RSUniHwcPrevalidateParamUtil::ProcessOfflineStrategy(strategy);
}

/*
 * @tc.name: ProcessOfflineStrategy002
 * @tc.desc: Test ProcessOfflineStrategy with OFFLINE_DEVICE (if-true, short-circuit)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, ProcessOfflineStrategy002, TestSize.Level2)
{
    std::map<uint64_t, RequestCompositionType> strategy;
    strategy[1] = RequestCompositionType::OFFLINE_DEVICE;
    RSUniHwcPrevalidateParamUtil::ProcessOfflineStrategy(strategy);
}

/*
 * @tc.name: ProcessOfflineStrategy003
 * @tc.desc: Test ProcessOfflineStrategy with OFFLINE_VCLD_OFF (if-true, second condition)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, ProcessOfflineStrategy003, TestSize.Level2)
{
    std::map<uint64_t, RequestCompositionType> strategy;
    strategy[1] = RequestCompositionType::OFFLINE_VCLD_OFF;
    RSUniHwcPrevalidateParamUtil::ProcessOfflineStrategy(strategy);
}

/*
 * @tc.name: ProcessOfflineStrategy004
 * @tc.desc: Test ProcessOfflineStrategy with DEVICE (if-false, not pushed)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest, ProcessOfflineStrategy004, TestSize.Level2)
{
    std::map<uint64_t, RequestCompositionType> strategy;
    strategy[1] = RequestCompositionType::DEVICE;
    RSUniHwcPrevalidateParamUtil::ProcessOfflineStrategy(strategy);
}
}