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

#ifndef RS_CORE_PIPELINE_RS_COMPOSER_ADAPTER_H
#define RS_CORE_PIPELINE_RS_COMPOSER_ADAPTER_H

#include "hdi_backend.h"

#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "screen_manager/rs_screen_manager.h"

namespace OHOS {
namespace Rosen {
struct ComposeInfo {
    GraphicIRect srcRect;
    GraphicIRect dstRect;
    GraphicIRect visibleRect;
    int32_t zOrder { 0 };
    GraphicLayerAlpha alpha;
    sptr<SurfaceBuffer> buffer;
    sptr<SurfaceBuffer> preBuffer;
    sptr<SyncFence> fence = SyncFence::INVALID_FENCE;
    GraphicBlendType blendType;
    bool needClient;
};

static inline int RotateEnumToInt(ScreenRotation rotation)
{
    static const std::map<ScreenRotation, int> screenRotationEnumToIntMap = {
        {ScreenRotation::ROTATION_0, 0}, {ScreenRotation::ROTATION_90, 90},
        {ScreenRotation::ROTATION_180, 180}, {ScreenRotation::ROTATION_270, 270}};
    auto iter = screenRotationEnumToIntMap.find(rotation);
    return iter != screenRotationEnumToIntMap.end() ? iter->second : 0;
}

static inline int RotateEnumToInt(GraphicTransformType rotation)
{
    static const std::map<GraphicTransformType, int> transformTypeEnumToIntMap = {
        {GraphicTransformType::GRAPHIC_ROTATE_NONE, 0}, {GraphicTransformType::GRAPHIC_ROTATE_90, 90},
        {GraphicTransformType::GRAPHIC_ROTATE_180, 180}, {GraphicTransformType::GRAPHIC_ROTATE_270, 270}};
    auto iter = transformTypeEnumToIntMap.find(rotation);
    return iter != transformTypeEnumToIntMap.end() ? iter->second : 0;
}

static inline GraphicTransformType RotateEnumToInt(int angle,
    GraphicTransformType flip = GraphicTransformType::GRAPHIC_ROTATE_NONE)
{
    static const std::map<int, GraphicTransformType> intToEnumMap = {
        {0, GraphicTransformType::GRAPHIC_ROTATE_NONE}, {90, GraphicTransformType::GRAPHIC_ROTATE_270},
        {180, GraphicTransformType::GRAPHIC_ROTATE_180}, {270, GraphicTransformType::GRAPHIC_ROTATE_90}};
     static const std::map<std::pair<int, GraphicTransformType>, GraphicTransformType> pairToEnumMap = {
        {{0, GraphicTransformType::GRAPHIC_FLIP_H}, GraphicTransformType::GRAPHIC_FLIP_H},
        {{0, GraphicTransformType::GRAPHIC_FLIP_V}, GraphicTransformType::GRAPHIC_FLIP_V},
        {{90, GraphicTransformType::GRAPHIC_FLIP_H}, GraphicTransformType::GRAPHIC_FLIP_H_ROT90},
        {{90, GraphicTransformType::GRAPHIC_FLIP_V}, GraphicTransformType::GRAPHIC_FLIP_V_ROT90},
        {{180, GraphicTransformType::GRAPHIC_FLIP_H}, GraphicTransformType::GRAPHIC_FLIP_V},
        {{180, GraphicTransformType::GRAPHIC_FLIP_V}, GraphicTransformType::GRAPHIC_FLIP_H},
        {{270, GraphicTransformType::GRAPHIC_FLIP_H}, GraphicTransformType::GRAPHIC_FLIP_V_ROT90},
        {{270, GraphicTransformType::GRAPHIC_FLIP_V}, GraphicTransformType::GRAPHIC_FLIP_H_ROT90},
    };

    if (flip != GraphicTransformType::GRAPHIC_FLIP_H && flip != GraphicTransformType::GRAPHIC_FLIP_V) {
        auto iter = intToEnumMap.find(angle);
        return iter != intToEnumMap.end() ? iter->second : GraphicTransformType::GRAPHIC_ROTATE_NONE;
    } else {
        auto iter = pairToEnumMap.find({angle, flip});
        return iter != pairToEnumMap.end() ? iter->second : GraphicTransformType::GRAPHIC_ROTATE_NONE;
    }
}

using FallbackCallback = std::function<void(const sptr<Surface>& surface, const std::vector<LayerInfoPtr>& layers)>;
class RSComposerAdapter {
public:
    RSComposerAdapter() = default;
    ~RSComposerAdapter() noexcept = default;

    // noncopyable
    RSComposerAdapter(const RSComposerAdapter&) = delete;
    void operator=(const RSComposerAdapter&) = delete;

    bool Init(const ScreenInfo& screenInfo, int32_t offsetX, int32_t offsetY, float mirrorAdaptiveCoefficient,
        const FallbackCallback& cb);

    LayerInfoPtr CreateLayer(RSSurfaceRenderNode& node);
    LayerInfoPtr CreateLayer(RSDisplayRenderNode& node);
    void CommitLayers(const std::vector<LayerInfoPtr>& layers);

private:
    // check if the node is out of the screen region.
    bool IsOutOfScreenRegion(const ComposeInfo& info) const;
    LayerInfoPtr CreateBufferLayer(RSSurfaceRenderNode& node) const;
    LayerInfoPtr CreateTunnelLayer(RSSurfaceRenderNode& node) const;
    ComposeInfo BuildComposeInfo(RSSurfaceRenderNode& node, bool isTunnelCheck = false) const;
    ComposeInfo BuildComposeInfo(RSDisplayRenderNode& node) const;
    static void SetComposeInfoToLayer(
        const LayerInfoPtr& layer,
        const ComposeInfo& info,
        const sptr<Surface>& surface,
        RSBaseRenderNode* node);
    void DealWithNodeGravity(const RSSurfaceRenderNode& node, ComposeInfo& info) const;
    void DumpLayersToFile(const std::vector<LayerInfoPtr>& layers);

    void LayerRotate(const LayerInfoPtr& layer, RSBaseRenderNode& node) const;
    void LayerCrop(const LayerInfoPtr& layer) const;
    static void LayerScaleDown(const LayerInfoPtr& layer);
    static void LayerPresentTimestamp(const LayerInfoPtr& layer, const sptr<Surface>& surface);

    void OnPrepareComplete(sptr<Surface>& surface, const PrepareCompleteParam& param, void* data);
    static void GetComposerInfoSrcRect(ComposeInfo &info, const RSSurfaceRenderNode& node);
    bool GetComposerInfoNeedClient(const ComposeInfo &info, RSSurfaceRenderNode& node) const;
    bool CheckStatusBeforeCreateLayer(RSSurfaceRenderNode& node, bool isTunnelCheck = false) const;
    HdiBackend *hdiBackend_ = nullptr;
    std::shared_ptr<HdiOutput> output_;
    ScreenInfo screenInfo_;

    // The offset on dst screen for all layers.
    int32_t offsetX_ = 0;
    int32_t offsetY_ = 0;

    float mirrorAdaptiveCoefficient_ = 1.0f;
    FallbackCallback fallbackCb_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_CORE_PIPELINE_RS_COMPOSER_ADAPTER_H
