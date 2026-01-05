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

#include "rs_surface_rcd_layer.h"
#include <memory>
#include "rs_layer_parcel.h"
#include "rs_composer_context.h"
#include "rs_surface_layer_parcel.h"
#include "surface_type.h"

namespace OHOS {
namespace Rosen {
RSSurfaceRCDLayer::RSSurfaceRCDLayer(RSLayerId rsLayerId, std::shared_ptr<RSComposerContext> rsComposerContext) :
    RSSurfaceLayer(rsLayerId, rsComposerContext)
{
}

std::shared_ptr<RSLayer> RSSurfaceRCDLayer::Create(const std::shared_ptr<RSComposerContext>& context,
    RSLayerId rsLayerId)
{
    if (context == nullptr) {
        RS_LOGE("RSSurfaceRCDLayer::Create context is nullptr");
        return nullptr;
    }
    std::shared_ptr<RSLayer> layer = context->GetRSLayer(rsLayerId);
    if (layer != nullptr && layer->IsScreenRCDLayer()) {
        RS_TRACE_NAME_FMT("RSSurfaceRCDLayer::Create use exist layer, id: %" PRIu64 ", name: %s, isRCD: %d",
            rsLayerId, layer->GetSurfaceName().c_str(), static_cast<int>(layer->IsScreenRCDLayer()));
        RS_LOGD("RSSurfaceRCDLayer::Create get cache layer by layer id: %{public}" PRIu64, rsLayerId);
        layer->SetRSLayerId(rsLayerId);
        return layer;
    }
    layer = std::make_shared<RSSurfaceRCDLayer>(rsLayerId, context);
    context->AddRSLayer(layer);
    return layer;
}

template<typename RSRenderLayerCmdName, typename T>
void RSSurfaceRCDLayer::SetRSLayerCmd(const T& value)
{
    auto renderProperty = std::make_shared<RSRenderLayerCmdProperty<T>>(value);
    auto renderLayerCmd = std::make_shared<RSRenderLayerCmdName>(renderProperty);
    std::shared_ptr<RSLayerParcel> layerParcel =
        std::make_shared<RSUpdateRSRCDLayerCmd>(GetRSLayerId(), renderLayerCmd);
    AddRSLayerParcel(layerParcel, GetRSLayerId());
}

void RSSurfaceRCDLayer::SetPixelMap(const std::shared_ptr<Media::PixelMap>& pixelMap)
{
    if (pixelMap_ != nullptr) {
        pixelMap_ = pixelMap;
        SetRSLayerCmd<RSRenderLayerPixelMapCmd>(pixelMap);
    }
}

std::shared_ptr<Media::PixelMap> RSSurfaceRCDLayer::GetPixelMap() const
{
    return pixelMap_;
}
} // namespace Rosen
} // namespace OHOS
