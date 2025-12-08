/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <cstdint>
#include <gtest/gtest.h>
#include <hilog/log.h>
#include <iservice_registry.h>
#include <memory>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#include "connection/rs_render_to_composer_connection.h"
#include "feature/hyper_graphic_manager/hgm_context.h"
#include "layer_backend/hdi_output.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
#include "pipeline/rs_render_composer_agent.h"
#include "pipeline/rs_render_composer_client.h"
#include "pipeline/rs_render_composer_manager.h"

#include "layer/rs_surface_layer.h"

using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSSurfaceLayerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline uint32_t screenId = 0;
    static inline std::shared_ptr<RSRenderComposerClient> client;
    static inline std::shared_ptr<RSSurfaceLayer> layer;
};

void RSSurfaceLayerTest::SetUpTestCase()
{
    RsVulkanContext::SetRecyclable(false);
    auto output = std::make_shared<HdiOutput>(screenId);
    RSRenderComposerManager::GetInstance().OnScreenConnected(output);
    client = std::make_shared<RSRenderComposerClient>(
        RSRenderComposerManager::GetInstance().rsComposerConnectionMap_[screenId]);
}

void RSSurfaceLayerTest::TearDownTestCase()
{
    RSRenderComposerManager::GetInstance().rsRenderComposerMap_[screenId]->uniRenderEngine_ = nullptr;
}

void RSSurfaceLayerTest::SetUp() {}
void RSSurfaceLayerTest::TearDown() {}

/**
 * @tc.name: CreateLayerTest
 * @tc.desc: Test RSLayerFactory::CreateRSLayer
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceLayerTest, CreateLayerTest, Function | SmallTest | Level2)
{
    auto layer1 = std::make_shared<RSSurfaceLayer>();
    EXPECT_NE(layer1, nullptr);

    auto layer2 = std::make_shared<RSSurfaceLayer>();
    EXPECT_NE(layer2, nullptr);
}

/**
 * @tc.name: LayerPropertiesChangeTest
 * @tc.desc: Test Change RSLayer Properties
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceLayerTest, LayerPropertiesChangeTest, Function | SmallTest | Level2)
{
    layer = std::make_shared<RSSurfaceLayer>()\;
    EXPECT_NE(layer, nullptr);

    GraphicLayerAlpha alpha;
    alpha.enPixelAlpha = false;
    layer->SetAlpha(alpha);
    EXPECT_EQ(layer->GetAlpha().enPixelAlpha, false);

    layer->SetZorder(100);
    EXPECT_EQ(layer->GetZorder(), 100);

    GraphicLayerType type = GRAPHIC_LAYER_TYPE_GRAPHIC;
    layer->SetType(type);
    EXPECT_EQ(layer->GetType(), type);

    GraphicTransformType type2 = GRAPHIC_ROTATE_NONE;
    layer->SetTransform(type2);
    EXPECT_EQ(layer->GetTransformType(), type2);

    GraphicCompositionType type3 = GRAPHIC_COMPOSITION_CLIENT;
    layer->SetCompositionType(type3);
    EXPECT_EQ(layer->GetCompositionType(), type3);

    std::vector<GraphicIRect> visibleRegions;
    layer->SetVisibleRegions(visibleRegions);
    EXPECT_EQ(layer->GetVisibleRegions(), visibleRegions);

    std::vector<GraphicIRect> dirtyRegions;
    layer->SetDirtyRegions(dirtyRegions);
    EXPECT_EQ(layer->GetDirtyRegions(), dirtyRegions);

    GraphicBlendType type4 = GRAPHIC_BLEND_NONE;
    layer->SetBlendType(type4);
    EXPECT_EQ(layer->GetBlendType(), type4);

    GraphicIRect crop;
    layer->SetCropRect(crop);
    EXPECT_EQ(layer->GetCropRect(), crop);

    layer->SetPreMulti(false);
    EXPECT_EQ(layer->IsPreMulti(), false);
}

/**
 * @tc.name: LayerPropertiesChangeTest
 * @tc.desc: Test Change RSLayer Properties
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceLayerTest, LayerPropertiesChangeTest2, Function | SmallTest | Level2)
{
    GraphicIRect layerRect;
    layer->SetLayerSize(layerRect);
    EXPECT_EQ(layer->GetLayerSize(), layerRect);

    GraphicIRect boundRect;
    layer->SetBoundSize(boundRect);
    EXPECT_EQ(layer->GetBoundSize(), boundRect);

    GraphicLayerColor layerColor;
    layerColor.r = 100;
    layer->SetLayerColor(layerColor);
    EXPECT_EQ(layer->GetLayerColor().r, 100);

    GraphicLayerColor backgroundColor;
    backgroundColor.r = 10;
    layer->SetBackgroundColor(backgroundColor);
    EXPECT_EQ(layer->GetBackgroundColor().r, 10);

    std::vector<float> drmCornerRadiusInfo;
    layer->SetCornerRadiusInfoForDRM(drmCornerRadiusInfo);
    EXPECT_EQ(layer->GetCornerRadiusInfoForDRM(), drmCornerRadiusInfo);

    std::vector<float> matrix;
    layer->SetColorTransform(matrix);
    EXPECT_EQ(layer->GetColorTransform(), matrix);

    GraphicColorDataSpace colorSpace = GRAPHIC_COLOR_DATA_SPACE_UNKNOWN;
    layer->SetColorDataSpace(colorSpace);
    EXPECT_EQ(layer->GetColorDataSpace(), colorSpace);

    std::vector<GraphicHDRMetaData> metaData;
    GraphicHDRMetaData a;
    a.key = GRAPHIC_MATAKEY_RED_PRIMARY_Y;
    a.value = 100;
    metaData.push_back(a);
    layer->SetMetaData(metaData);
    EXPECT_EQ(layer->GetMetaData()[0].key, GRAPHIC_MATAKEY_RED_PRIMARY_Y);

    GraphicHDRMetaDataSet metaDataSet;
    metaDataSet.key = GRAPHIC_MATAKEY_BLUE_PRIMARY_X;
    layer->SetMetaDataSet(metaDataSet);
    EXPECT_EQ(layer->GetMetaDataSet().key, GRAPHIC_MATAKEY_BLUE_PRIMARY_X);

    GraphicMatrix matrix2;
    matrix2.scaleX = 2;
    layer->SetMatrix(matrix2);
    EXPECT_EQ(layer->GetMatrix().scaleX, 2);
}

/**
 * @tc.name: LayerPropertiesChangeTest
 * @tc.desc: Test Change RSLayer Properties
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceLayerTest, LayerPropertiesChangeTest3, Function | SmallTest | Level2)
{
    int32_t gravity = 0;
    layer->SetGravity(gravity);
    EXPECT_EQ(layer->GetGravity(), gravity);

    layer->SetUniRenderFlag(false);
    EXPECT_EQ(layer->GetUniRenderFlag(), false);

    layer->SetTunnelHandleChange(false);
    EXPECT_EQ(layer->GetTunnelHandleChange(), false);

    sptr<SurfaceTunnelHandle> handle = sptr<SurfaceTunnelHandle>::MakeSptr();
    layer->SetTunnelHandle(handle);
    EXPECT_EQ(layer->GetTunnelHandle(), handle);

    layer->SetTunnelLayerId(100);
    EXPECT_EQ(layer->GetTunnelLayerId(), 100);

    layer->SetTunnelLayerProperty(1000);
    EXPECT_EQ(layer->GetTunnelLayerProperty(), 1000);

    GraphicPresentTimestamp timestamp;
    timestamp.type = GRAPHIC_DISPLAY_PTS_DELAY;
    layer->SetPresentTimestamp(timestamp);
    EXPECT_EQ(layer->GetPresentTimestamp().type, GRAPHIC_DISPLAY_PTS_DELAY);

    layer->SetIsSupportedPresentTimestamp(false);
    EXPECT_EQ(layer->IsSupportedPresentTimestamp(), false);

    layer->SetSdrNit(200);
    EXPECT_EQ(layer->GetSdrNit(), 200);

    layer->SetDisplayNit(300);
    EXPECT_EQ(layer->GetDisplayNit(), 300);

    layer->SetBrightnessRatio(300);
    EXPECT_EQ(layer->GetBrightnessRatio(), 300);

    std::vector<float> layerLinearMatrix;
    layerLinearMatrix.push_back(100);
    layer->SetLayerLinearMatrix(layerLinearMatrix);
    EXPECT_EQ(layer->GetLayerLinearMatrix()[0], 100);

    layer->SetLayerSourceTuning(100);
    EXPECT_EQ(layer->GetLayerSourceTuning(), 100);
}

/**
 * @tc.name: LayerPropertiesChangeTest
 * @tc.desc: Test Change RSLayer Properties
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceLayerTest, LayerPropertiesChangeTest4, Function | SmallTest | Level2)
{
    std::vector<std::string> windowsName;
    windowsName.push_back("window");
    layer->SetWindowsName(windowsName);
    EXPECT_EQ(layer->GetWindowsName()[0], "window");

    layer->SetRotationFixed(false);
    EXPECT_EQ(layer->GetRotationFixed(), false);

    layer->SetLayerArsr(false);
    EXPECT_EQ(layer->GetLayerArsr(), false);

    layer->SetLayerCopybit(false);
    EXPECT_EQ(layer->GetLayerCopybit(), false);

    layer->SetNeedBilinearInterpolation(true);
    EXPECT_EQ(layer->GetNeedBilinearInterpolation(), true);

    layer->SetIsMaskLayer(false);
    EXPECT_EQ(layer->IsMaskLayer(), false);

    layer->SetNodeId(100);
    EXPECT_EQ(layer->GetNodeId(), 100);

    layer->SetAncoFlags(0);
    EXPECT_EQ(layer->GetAncoFlags(), 0);

    EXPECT_EQ(layer->IsAncoNative(), false);

    LayerMask mask = LAYER_MASK_NORMAL;
    layer->SetLayerMaskInfo(mask);
    EXPECT_EQ(layer->GetLayerMaskInfo(), mask);

    sptr<IConsumerSurface> surface;
    layer->SetSurface(surface);
    EXPECT_EQ(layer->GetSurface(), surface);

    sptr<SurfaceBuffer> sbuffer;
    sptr<SyncFence> acquireFence;
    layer->SetBuffer(sbuffer, acquireFence);
    layer->SetBuffer(sbuffer);
    EXPECT_EQ(layer->GetBuffer(), sbuffer);
    EXPECT_EQ(layer->acquireFence_, acquireFence);

    sptr<SurfaceBuffer> pbuffer;
    layer->SetPreBuffer(pbuffer);
    EXPECT_EQ(layer->GetPreBuffer(), pbuffer);
}

/**
 * @tc.name: LayerPropertiesChangeTest
 * @tc.desc: Test Change RSLayer Properties
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceLayerTest, LayerPropertiesChangeTest5, Function | SmallTest | Level2)
{
    sptr<SyncFence> acquireFence2;
    layer->SetAcquireFence(acquireFence2);
    EXPECT_EQ(layer->GetAcquireFence(), acquireFence2);

    layer->SetCycleBuffersNum(100);
    EXPECT_EQ(layer->GetCycleBuffersNum(), 100);

    layer->SetUseDeviceOffline(false);
    EXPECT_EQ(layer->GetUseDeviceOffline(), false);

    layer->SetIgnoreAlpha(false);
    EXPECT_EQ(layer->GetIgnoreAlpha(), false);

    auto layer1 = std::make_shared<RSSurfaceLayer>();
    layer1->CopyLayerInfo(layer);
    EXPECT_EQ(layer1->zOrder_, layer->zOrder_);

    layer->cSurface_ = nullptr;
    layer->DumpCurrentFrameLayer();
    layer->cSurface_ = IConsumerSurface::Create("test consumer");
    layer->DumpCurrentFrameLayer();
    EXPECT_NE(layer, nullptr);
}

/**
 * @tc.name: LayerPropertiesChangeTest
 * @tc.desc: Test Change RSLayer Properties
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceLayerTest, LayerPropertiesChangeTest6, Function | SmallTest | Level2)
{
    std::string result = "";
    layer->transformType_ = GRAPHIC_ROTATE_NONE;
    layer->compositionType_ = GRAPHIC_COMPOSITION_CLIENT;
    layer->blendType_ = GRAPHIC_BLEND_NONE;
    layer->cSurface_ = IConsumerSurface::Create("test consumer");
    layer->Dump(result);
    EXPECT_NE(result, "");

    result = "";
    layer->transformType_ = static_cast<GraphicTransformType>(13);
    layer->compositionType_ = GRAPHIC_COMPOSITION_CLIENT;
    layer->blendType_ = GRAPHIC_BLEND_NONE;
    layer->cSurface_ = nullptr;
    layer->Dump(result);
    EXPECT_NE(result, "");

    result = "";
    layer->transformType_ = GRAPHIC_ROTATE_NONE;
    layer->compositionType_ = static_cast<GraphicCompositionType>(13);
    layer->blendType_ = GRAPHIC_BLEND_NONE;
    layer->Dump(result);
    EXPECT_NE(result, "");

    result = "";
    layer->transformType_ = static_cast<GraphicTransformType>(13);
    layer->compositionType_ = static_cast<GraphicCompositionType>(13);
    layer->blendType_ = GRAPHIC_BLEND_NONE;
    layer->Dump(result);
    EXPECT_NE(result, "");

    result = "";
    layer->transformType_ = GRAPHIC_ROTATE_NONE;
    layer->compositionType_ = static_cast<GraphicCompositionType>(13);
    layer->blendType_ = static_cast<GraphicBlendType>(20);
    layer->Dump(result);
    EXPECT_NE(result, "");
}

/**
 * @tc.name: LayerPropertiesChangeTest
 * @tc.desc: Test Change RSLayer Properties
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceLayerTest, LayerPropertiesChangeTest7, Function | SmallTest | Level2)
{
    std::string result = "";
    layer->transformType_ = GRAPHIC_ROTATE_NONE;
    layer->compositionType_ = GRAPHIC_COMPOSITION_CLIENT;
    layer->blendType_ = GRAPHIC_BLEND_NONE;
    layer->cSurface_ = IConsumerSurface::Create("test consumer");
    layer->Dump(result);
    EXPECT_NE(result, "");

    result = "";
    layer->transformType_ = static_cast<GraphicTransformType>(13);
    layer->compositionType_ = static_cast<GraphicCompositionType>(13);
    layer->blendType_ = static_cast<GraphicBlendType>(20);
    layer->Dump(result);
    EXPECT_NE(result, "");

    result = "";
    layer->transformType_ = GRAPHIC_ROTATE_NONE;
    layer->compositionType_ = GRAPHIC_COMPOSITION_CLIENT;
    layer->blendType_ = static_cast<GraphicBlendType>(20);
    layer->Dump(result);
    EXPECT_NE(result, "");

    result = "";
    layer->transformType_ = static_cast<GraphicTransformType>(13);
    layer->compositionType_ = GRAPHIC_COMPOSITION_CLIENT;
    layer->blendType_ = static_cast<GraphicBlendType>(20);
    layer->Dump(result);
    EXPECT_NE(result, "");
}
} // namespace Rosen
} // namespace OHOS