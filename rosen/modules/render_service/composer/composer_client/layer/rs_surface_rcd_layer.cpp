/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "rs_composer_context.h"
#include "rs_layer_parcel.h"
#include "rs_surface_layer_parcel.h"
#include "surface_type.h"

#undef LOG_TAG
#define LOG_TAG "RSSurfaceRCDLayer"
namespace OHOS {
namespace Rosen {
RSSurfaceRCDLayer::RSSurfaceRCDLayer(RSLayerId rsLayerId, std::shared_ptr<RSComposerContext> rsComposerContext)
    : RSSurfaceLayer(rsLayerId, rsComposerContext)
{
}

std::shared_ptr<RSLayer> RSSurfaceRCDLayer::Create(RSLayerId rsLayerId,
    const std::shared_ptr<RSComposerContext>& context)
{
    if (context == nullptr) {
        RS_LOGE("%{public}s context is nullptr", __func__);
        return nullptr;
    }
    std::shared_ptr<RSLayer> layer = context->GetRSLayer(rsLayerId);
    if (layer != nullptr && layer->IsScreenRCDLayer()) {
        RS_TRACE_NAME_FMT("%s use exist layer, id: %" PRIu64 ", name: %s, isRCD: %d",
            __func__, rsLayerId, layer->GetSurfaceName().c_str(), static_cast<int>(layer->IsScreenRCDLayer()));
        RS_LOGD("%{public}s get cache layer by layer id: %{public}" PRIu64, __func__, rsLayerId);
        layer->SetRSLayerId(rsLayerId);
        return layer;
    }
    layer = std::shared_ptr<RSSurfaceRCDLayer>(new RSSurfaceRCDLayer(rsLayerId, context));
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
    AddRSLayerParcel(GetRSLayerId(), layerParcel);
}

void RSSurfaceRCDLayer::SetPixelMap(const std::shared_ptr<Media::PixelMap>& pixelMap)
{
    if (pixelMap_ == pixelMap) {
        return;
    }
    RS_OPTIONAL_TRACE_FMT("RSSurfaceRCDLayer::SetPixelMap %d", static_cast<int>(pixelMap != nullptr));
    RS_LOGI("%{public}s rcdLayer id: %{public}" PRIu64, __func__, GetRSLayerId());
    pixelMap_ = pixelMap;
    SetRSLayerCmd<RSRenderLayerPixelMapCmd>(pixelMap);
}

std::shared_ptr<Media::PixelMap> RSSurfaceRCDLayer::GetPixelMap() const
{
    return pixelMap_;
}
} // namespace Rosen
} // namespace OHOS
