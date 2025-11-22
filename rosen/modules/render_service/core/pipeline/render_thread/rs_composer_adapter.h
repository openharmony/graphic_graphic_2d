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

#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "rs_base_render_util.h"
#include "screen_manager/rs_screen_manager.h"

namespace OHOS {
namespace Rosen {

class RSRenderComposerClient;
using FallbackCallback = std::function<void(const sptr<Surface>& surface, const std::vector<RSLayerPtr>& layers)>;
class RSComposerAdapter {
public:
    RSComposerAdapter() = default;
    ~RSComposerAdapter() noexcept = default;

    // noncopyable
    RSComposerAdapter(const RSComposerAdapter&) = delete;
    void operator=(const RSComposerAdapter&) = delete;

    bool Init(const ScreenInfo& screenInfo, int32_t offsetX, int32_t offsetY, float mirrorAdaptiveCoefficient,
        const FallbackCallback& cb);
    bool Init(const RSScreenRenderNode& node, const ScreenInfo& screenInfo, const ScreenInfo& mirroredScreenInfo,
        float mirrorAdaptiveCoefficient, const FallbackCallback& cb);

    RSLayerPtr CreateLayer(RSSurfaceRenderNode& node) const;
    RSLayerPtr CreateLayer(RSScreenRenderNode& node) const;
    void CommitLayers(const std::vector<RSLayerPtr>& layers);
    /* only used for mock tests */
    void SetHdiBackendDevice(HdiDevice* device);
    void SetMirroredScreenInfo(const ScreenInfo& mirroredScreenInfo);
    void SetColorFilterMode(ColorFilterMode colorFilterMode);

private:
    // check if the node is out of the screen region.
    bool IsOutOfScreenRegion(const ComposeInfo& info) const;
    RSLayerPtr CreateBufferLayer(RSSurfaceRenderNode& node) const;
    RSLayerPtr CreateTunnelLayer(RSSurfaceRenderNode& node) const;
    ComposeInfo BuildComposeInfo(RSSurfaceRenderNode& node, bool isTunnelCheck = false) const;
    ComposeInfo BuildComposeInfo(RSScreenRenderNode& node) const;
    void SetComposeInfoToLayer(
        const RSLayerPtr& layer,
        const ComposeInfo& info,
        const sptr<IConsumerSurface>& surface,
        RSBaseRenderNode* node) const;
    void SetMetaDataInfoToLayer(const RSLayerPtr& layer, const ComposeInfo& info,
                                const sptr<IConsumerSurface>& surface) const;
    void DealWithNodeGravity(const RSSurfaceRenderNode& node, ComposeInfo& info) const;
    void DumpLayersToFile(const std::vector<RSLayerPtr>& layers);

    void LayerRotate(const RSLayerPtr& layer, RSBaseRenderNode& node) const;
    void LayerCrop(const RSLayerPtr& layer) const;
    static void LayerScaleDown(const RSLayerPtr& layer);
    static void LayerPresentTimestamp(const RSLayerPtr& layer, const sptr<IConsumerSurface>& surface);

    void OnPrepareComplete(sptr<Surface>& surface, const PrepareCompleteParam& param, void* data);
    static void GetComposerInfoSrcRect(ComposeInfo& info, const RSSurfaceRenderNode& node);
    bool GetComposerInfoNeedClient(const ComposeInfo& info, RSSurfaceRenderNode& node) const;
    bool CheckStatusBeforeCreateLayer(RSSurfaceRenderNode& node, bool isTunnelCheck = false) const;
    HdiBackend *hdiBackend_ = nullptr;
    std::shared_ptr<HdiOutput> output_;
    ScreenInfo screenInfo_;
    ScreenInfo mirroredScreenInfo_;
    ColorFilterMode colorFilterMode_ = ColorFilterMode::COLOR_FILTER_END;

    // The offset on dst screen for all layers.
    int32_t offsetX_ = 0;
    int32_t offsetY_ = 0;

    float mirrorAdaptiveCoefficient_ = 1.0f;
    FallbackCallback fallbackCb_;
    std::shared_ptr<RSRenderComposerClient> composerClient_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_CORE_PIPELINE_RS_COMPOSER_ADAPTER_H
