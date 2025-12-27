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

#include <gtest/gtest.h>

#include "rs_render_composer_context.h"
#include "rs_layer.h"
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RsRenderComposerContextTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsRenderComposerContextTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}
void RsRenderComposerContextTest::TearDownTestCase() {}
void RsRenderComposerContextTest::SetUp() {}
void RsRenderComposerContextTest::TearDown() {}

// Fake RSLayer for testing
class FakeRSLayer : public RSLayer {
public:
    explicit FakeRSLayer(RSLayerId id, bool usingFlag, const std::string& name = "L")
        : id_(id), isNeedComposition_(usingFlag), surfaceName_(name) {}
    ~FakeRSLayer() override = default;

    void SetAlpha(const GraphicLayerAlpha &alpha) override { alpha_ = alpha; }
    const GraphicLayerAlpha &GetAlpha() const override { return alpha_; }
    void SetZorder(int32_t zOrder) override { zOrder_ = zOrder; }
    uint32_t GetZorder() const override { return static_cast<uint32_t>(zOrder_); }
    void SetType(const GraphicLayerType layerType) override { layerType_ = layerType; }
    GraphicLayerType GetType() const override { return layerType_; }
    void SetTransform(GraphicTransformType type) override { transform_ = type; }
    GraphicTransformType GetTransformType() const override { return transform_; }
    void SetCompositionType(GraphicCompositionType type) override { compType_ = type; }
    GraphicCompositionType GetCompositionType() const override { return compType_; }
    void SetVisibleRegions(const std::vector<GraphicIRect>& visibleRegions) override
    { visibleRegions_ = visibleRegions; }
    const std::vector<GraphicIRect>& GetVisibleRegions() const override { return visibleRegions_; }
    void SetDirtyRegions(const std::vector<GraphicIRect>& dirtyRegions) override { dirtyRegions_ = dirtyRegions; }
    const std::vector<GraphicIRect>& GetDirtyRegions() const override { return dirtyRegions_; }
    void SetBlendType(GraphicBlendType type) override { blendType_ = type; }
    GraphicBlendType GetBlendType() const override { return blendType_; }
    void SetCropRect(const GraphicIRect &crop) override { cropRect_ = crop; }
    const GraphicIRect &GetCropRect() const override { return cropRect_; }
    void SetPreMulti(bool preMulti) override { preMulti_ = preMulti; }
    bool IsPreMulti() const override { return preMulti_; }
    void SetLayerSize(const GraphicIRect &layerRect) override { layerSize_ = layerRect; }
    const GraphicIRect &GetLayerSize() const override { return layerSize_; }
    void SetBoundSize(const GraphicIRect &boundRect) override { boundSize_ = boundRect; }
    const GraphicIRect& GetBoundSize() const override { return boundSize_; }
    void SetLayerColor(GraphicLayerColor layerColor) override { layerColor_ = layerColor; }
    const GraphicLayerColor& GetLayerColor() const override { return layerColor_; }
    void SetBackgroundColor(GraphicLayerColor backgroundColor) override { backgroundColor_ = backgroundColor; }
    const GraphicLayerColor& GetBackgroundColor() const override { return backgroundColor_; }
    void SetCornerRadiusInfoForDRM(const std::vector<float>& drmCornerRadiusInfo) override
    { cornerRadiusInfo_ = drmCornerRadiusInfo; }
    const std::vector<float>& GetCornerRadiusInfoForDRM() const override { return cornerRadiusInfo_; }
    void SetColorTransform(const std::vector<float>& matrix) override { colorTransform_ = matrix; }
    const std::vector<float>& GetColorTransform() const override { return colorTransform_; }
    void SetColorDataSpace(GraphicColorDataSpace colorSpace) override { colorSpace_ = colorSpace; }
    GraphicColorDataSpace GetColorDataSpace() const override { return colorSpace_; }
    void SetMetaData(const std::vector<GraphicHDRMetaData>& metaData) override { metaData_ = metaData; }
    const std::vector<GraphicHDRMetaData>& GetMetaData() const override { return metaData_; }
    void SetMetaDataSet(const GraphicHDRMetaDataSet& metaDataSet) override { metaDataSet_ = metaDataSet; }
    const GraphicHDRMetaDataSet& GetMetaDataSet() const override { return metaDataSet_; }
    void SetMatrix(GraphicMatrix matrix) override { matrix_ = matrix; }
    const GraphicMatrix& GetMatrix() const override { return matrix_; }
    void SetGravity(int32_t gravity) override { gravity_ = gravity; }
    int32_t GetGravity() const override { return gravity_; }
    void SetUniRenderFlag(bool isUniRender) override { uniRender_ = isUniRender; }
    bool GetUniRenderFlag() const override { return uniRender_; }
    void SetTunnelHandleChange(bool change) override { tunnelHandleChange_ = change; }
    bool GetTunnelHandleChange() const override { return tunnelHandleChange_; }
    void SetTunnelHandle(const sptr<SurfaceTunnelHandle> &handle) override { tunnelHandle_ = handle; }
    sptr<SurfaceTunnelHandle> GetTunnelHandle() const override { return tunnelHandle_; }
    void SetTunnelLayerId(const uint64_t tunnelLayerId) override { tunnelLayerId_ = tunnelLayerId; }
    uint64_t GetTunnelLayerId() const override { return tunnelLayerId_; }
    void SetTunnelLayerProperty(uint32_t tunnelLayerProperty) override
    { tunnelLayerProperty_ = tunnelLayerProperty; }
    uint32_t GetTunnelLayerProperty() const override { return tunnelLayerProperty_; }
    void SetIsSupportedPresentTimestamp(bool isSupported) override { supportedPresentTimestamp_ = isSupported; }
    bool IsSupportedPresentTimestamp() const override { return supportedPresentTimestamp_; }
    void SetPresentTimestamp(const GraphicPresentTimestamp &timestamp) override { presentTimestamp_ = timestamp; }
    const GraphicPresentTimestamp &GetPresentTimestamp() const override { return presentTimestamp_; }
    void SetSdrNit(float sdrNit) override { sdrNit_ = sdrNit; }
    float GetSdrNit() const override { return sdrNit_; }
    void SetDisplayNit(float displayNit) override { displayNit_ = displayNit; }
    float GetDisplayNit() const override { return displayNit_; }
    void SetBrightnessRatio(float brightnessRatio) override { brightnessRatio_ = brightnessRatio; }
    float GetBrightnessRatio() const override { return brightnessRatio_; }
    void SetLayerLinearMatrix(const std::vector<float>& layerLinearMatrix) override
    { layerLinearMatrix_ = layerLinearMatrix; }
    const std::vector<float>& GetLayerLinearMatrix() const override { return layerLinearMatrix_; }
    void SetLayerSourceTuning(int32_t layerSouce) override { layerSourceTuning_ = layerSouce; }
    int32_t GetLayerSourceTuning() const override { return layerSourceTuning_; }
    void SetWindowsName(std::vector<std::string>& windowsName) override { windowsName_ = windowsName; }
    const std::vector<std::string>& GetWindowsName() const override { return windowsName_; }
    void SetRotationFixed(bool rotationFixed) override { rotationFixed_ = rotationFixed; }
    bool GetRotationFixed() const override { return rotationFixed_; }
    void SetLayerArsr(bool arsrTag) override { layerArsr_ = arsrTag; }
    bool GetLayerArsr() const override { return layerArsr_; }
    void SetLayerCopybit(bool copybitTag) override { layerCopybit_ = copybitTag; }
    bool GetLayerCopybit() const override { return layerCopybit_; }
    void SetNeedBilinearInterpolation(bool need) override { needBilinear_ = need; }
    bool GetNeedBilinearInterpolation() const override { return needBilinear_; }
    void SetIsMaskLayer(bool isMaskLayer) override { isMaskLayer_ = isMaskLayer; }
    bool IsMaskLayer() const override { return isMaskLayer_; }
    void SetIgnoreAlpha(bool ignoreAlpha) override { ignoreAlpha_ = ignoreAlpha; }
    bool GetIgnoreAlpha() const override { return ignoreAlpha_; }
    void SetNodeId(uint64_t nodeId) override { nodeId_ = nodeId; }
    uint64_t GetNodeId() const override { return nodeId_; }
    void SetAncoFlags(const uint32_t ancoFlags) override { ancoFlags_ = ancoFlags; }
    uint32_t GetAncoFlags() const override { return ancoFlags_; }
    bool IsAncoNative() const override { return false; }
    void SetLayerMaskInfo(LayerMask mask) override { layerMask_ = mask; }
    LayerMask GetLayerMaskInfo() const override { return layerMask_; }

    void SetSurface(const sptr<IConsumerSurface>& surface) override { surface_ = surface; }
    sptr<IConsumerSurface> GetSurface() const override { return surface_; }
    void SetBuffer(const sptr<SurfaceBuffer>& sbuffer, const sptr<SyncFence>& acquireFence) override
    {
        buffer_ = sbuffer;
        acquireFence_ = acquireFence;
    }
    void SetBuffer(const sptr<SurfaceBuffer>& sbuffer) override { buffer_ = sbuffer; }
    sptr<SurfaceBuffer> GetBuffer() const override { return buffer_; }
    void SetPreBuffer(const sptr<SurfaceBuffer>& buffer) override { preBuffer_ = buffer; }
    sptr<SurfaceBuffer> GetPreBuffer() const override { return preBuffer_; }
    void SetAcquireFence(const sptr<SyncFence>& acquireFence) override { acquireFence_ = acquireFence; }
    sptr<SyncFence> GetAcquireFence() const override { return acquireFence_; }

    // Provide overrides to avoid undefined reference to base default implementations during linking
    void SetUseDeviceOffline(bool useOffline) override { useDeviceOffline_ = useOffline; }
    bool GetUseDeviceOffline() const override { return useDeviceOffline_; }
    void SetAncoSrcRect(const GraphicIRect& ancoSrcRect) override { ancoSrcRect_ = ancoSrcRect; }
    const GraphicIRect& GetAncoSrcRect() const override { return ancoSrcRect_; }

    void CopyLayerInfo(const std::shared_ptr<RSLayer>& rsLayer) override { (void)rsLayer; }
    void Dump(std::string& result) const override { (void)result; }
    void DumpCurrentFrameLayer() const override {}

private:
    RSLayerId id_ = 0;
    bool isNeedComposition_ = false;
    std::string surfaceName_;

    GraphicLayerAlpha alpha_ {};
    int32_t zOrder_ = 0;
    GraphicLayerType layerType_ = GraphicLayerType::GRAPHIC_LAYER_TYPE_BUTT;
    GraphicTransformType transform_ = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    GraphicCompositionType compType_ = GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT;
    std::vector<GraphicIRect> visibleRegions_;
    std::vector<GraphicIRect> dirtyRegions_;
    GraphicBlendType blendType_ = GraphicBlendType::GRAPHIC_BLEND_NONE;
    GraphicIRect cropRect_ {0, 0, 0, 0};
    bool preMulti_ = false;
    GraphicIRect layerSize_ {0, 0, 0, 0};
    GraphicIRect boundSize_ {0, 0, 0, 0};
    GraphicLayerColor layerColor_ {};
    GraphicLayerColor backgroundColor_ {};
    std::vector<float> cornerRadiusInfo_;
    std::vector<float> colorTransform_;
    GraphicColorDataSpace colorSpace_ = GraphicColorDataSpace::GRAPHIC_COLOR_DATA_SPACE_UNKNOWN;
    std::vector<GraphicHDRMetaData> metaData_;
    GraphicHDRMetaDataSet metaDataSet_ {};
    GraphicMatrix matrix_ {};
    int32_t gravity_ = 0;
    bool uniRender_ = false;
    bool tunnelHandleChange_ = false;
    sptr<SurfaceTunnelHandle> tunnelHandle_ = nullptr;
    uint64_t tunnelLayerId_ = 0;
    uint32_t tunnelLayerProperty_ = 0;
    bool supportedPresentTimestamp_ = false;
    GraphicPresentTimestamp presentTimestamp_ {};
    float sdrNit_ = 0.0f;
    float displayNit_ = 0.0f;
    float brightnessRatio_ = 0.0f;
    std::vector<float> layerLinearMatrix_;
    int32_t layerSourceTuning_ = 0;
    std::vector<std::string> windowsName_;
    bool rotationFixed_ = false;
    bool layerArsr_ = false;
    bool layerCopybit_ = false;
    bool needBilinear_ = false;
    bool isMaskLayer_ = false;
    uint64_t nodeId_ = 0;
    uint32_t ancoFlags_ = 0;
    LayerMask layerMask_ {};
    sptr<IConsumerSurface> surface_ = nullptr;
    uint64_t surfaceUniqueId_ = 0;
    sptr<SurfaceBuffer> buffer_ = nullptr;
    sptr<SurfaceBuffer> preBuffer_ = nullptr;
    sptr<SyncFence> acquireFence_ = nullptr;
    uint32_t cycleBuffersNum_ = 0;
    bool ignoreAlpha_ {false};
    bool useDeviceOffline_ {false};
    GraphicIRect ancoSrcRect_ {-1, -1, -1, -1};
};

/**
 * Function: GetRSLayersVec_Empty_ReturnsEmpty
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerContext
 *                  2. call GetRSLayersVec()
 *                  3. check returned vector is empty
 */
HWTEST_F(RsRenderComposerContextTest, GetRSLayersVec_Empty_ReturnsEmpty, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerContext> ctx = std::make_shared<RSRenderComposerContext>();
    auto vec = ctx->GetRSLayersVec();
    EXPECT_TRUE(vec.empty());
}

/**
 * Function: GetRSLayersVec_FiltersByisNeedComposition
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerContext and add two layers (one used, one unused)
 *                  2. call GetRSLayersVec()
 *                  3. check returned vector only contains the used layer
 */
HWTEST_F(RsRenderComposerContextTest, SetAndGet_ReturnsSameVector, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerContext> ctx = std::make_shared<RSRenderComposerContext>();
    std::vector<std::shared_ptr<RSLayer>> layers;
    std::shared_ptr<RSLayer> l1 = std::make_shared<FakeRSLayer>(1, false, "L1");
    std::shared_ptr<RSLayer> l2 = std::make_shared<FakeRSLayer>(2, true, "L2");
    layers.push_back(l1);
    layers.push_back(l2);
    ctx->SetRSLayersVec(std::move(layers));

    auto vec = ctx->GetRSLayersVec();
    ASSERT_EQ(vec.size(), 2u);
    EXPECT_EQ(vec[0], l1);
    EXPECT_EQ(vec[1], l2);
}

/**
 * Function: GetRSRenderLayers_FoundAndNotFound
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerContext and add layer with id 100
 *                  2. call GetRSRenderLayer() with id 100 and 200
 *                  3. check found 100 and not found 200
 */
HWTEST_F(RsRenderComposerContextTest, ClearAllRSLayers_EmptiesVector, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerContext> ctx = std::make_shared<RSRenderComposerContext>();
    std::vector<std::shared_ptr<RSLayer>> layers;
    layers.push_back(std::shared_ptr<RSLayer>(std::make_shared<FakeRSLayer>(1, true)));
    layers.push_back(std::shared_ptr<RSLayer>(std::make_shared<FakeRSLayer>(2, true)));
    ctx->SetRSLayersVec(std::move(layers));
    ASSERT_EQ(ctx->GetRSLayersVec().size(), 2u);

    ctx->ClearAllRSLayers();
    EXPECT_TRUE(ctx->GetRSLayersVec().empty());
}

/**
 * Function: AddRSRenderLayer_OverrideExisting
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerContext and add layer
 *                  2. re-add layer with same id but different instance
 *                  3. find layer id when first add, and check it's the new instance after override
 */
HWTEST_F(RsRenderComposerContextTest, GetRSLayersVec_CopySemantics, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerContext> ctx = std::make_shared<RSRenderComposerContext>();
    std::vector<std::shared_ptr<RSLayer>> layers;
    layers.push_back(std::shared_ptr<RSLayer>(std::make_shared<FakeRSLayer>(1, true)));
    layers.push_back(std::shared_ptr<RSLayer>(std::make_shared<FakeRSLayer>(2, true)));
    ctx->SetRSLayersVec(std::move(layers));

    auto vecA = ctx->GetRSLayersVec();
    ASSERT_EQ(vecA.size(), 2u);
    // mutate the returned copy
    vecA.pop_back();

    // internal storage should remain unchanged
    auto vecB = ctx->GetRSLayersVec();
    EXPECT_EQ(vecB.size(), 0);
}

/**
 * Function: RemoveRSRenderLayer_Branches
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerContext and add layer
 *                  2. remove layer by existing id and non-existing id
 *                  3. check layer count after add and removes
 */
HWTEST_F(RsRenderComposerContextTest, SetRSLayersVec_Override, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerContext> ctx = std::make_shared<RSRenderComposerContext>();
    std::vector<std::shared_ptr<RSLayer>> layersA;
    layersA.push_back(std::shared_ptr<RSLayer>(std::make_shared<FakeRSLayer>(1, true)));
    ctx->SetRSLayersVec(std::move(layersA));
    ASSERT_EQ(ctx->GetRSLayersVec().size(), 1u);

    std::vector<std::shared_ptr<RSLayer>> layersB;
    layersB.push_back(std::shared_ptr<RSLayer>(std::make_shared<FakeRSLayer>(2, true)));
    layersB.push_back(std::shared_ptr<RSLayer>(std::make_shared<FakeRSLayer>(3, true)));
    ctx->SetRSLayersVec(std::move(layersB));
    auto vec = ctx->GetRSLayersVec();
    EXPECT_EQ(vec.size(), 2u);
}

/**
 * Function: RemoveRSRenderLayer_Branches
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposerContext
 *                  2. add two layers
 *                  3. check GetRSRenderLayerCount() returns accurate count
 */
HWTEST_F(RsRenderComposerContextTest, SetRSLayersVec_WithEmptyVector, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerContext> ctx = std::make_shared<RSRenderComposerContext>();
    std::vector<std::shared_ptr<RSLayer>> empty;
    ctx->SetRSLayersVec(std::move(empty));
    EXPECT_TRUE(ctx->GetRSLayersVec().empty());
}

} // namespace Rosen
} // namespace OHOS
