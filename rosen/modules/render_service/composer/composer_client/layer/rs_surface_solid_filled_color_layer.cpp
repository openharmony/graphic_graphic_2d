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

#include "rs_surface_solid_filled_color_layer.h"

#include "rs_composer_context.h"
#include "rs_surface_layer_parcel.h"
#include "surface_type.h"

#undef LOG_TAG
#define LOG_TAG "RSSurfaceSolidFilledColorLayer"
namespace OHOS {
namespace Rosen {
RSSurfaceSolidFilledColorLayer::RSSurfaceSolidFilledColorLayer(RSLayerId rsLayerId,
    std::shared_ptr<RSComposerContext> rsComposerContext)
    : RSSurfaceLayer(rsLayerId, rsComposerContext)
{
}

std::shared_ptr<RSLayer> RSSurfaceSolidFilledColorLayer::Create(RSLayerId rsLayerId,
    const std::shared_ptr<RSComposerContext>& context)
{
    if (context == nullptr) {
        RS_LOGE("%{public}s context is nullptr", __func__);
        return nullptr;
    }
    std::shared_ptr<RSLayer> layer = context->GetRSLayer(rsLayerId);
    if (layer != nullptr && layer->IsSolidFilledColorLayer()) {
        RS_TRACE_NAME_FMT("%s use exist layer, id: %" PRIu64 ", name: %s, isGradientColor: %d",
            __func__, rsLayerId, layer->GetSurfaceName().c_str(), static_cast<int>(layer->IsSolidFilledColorLayer()));
        RS_LOGD("%{public}s get cache layer by layer id: %{public}" PRIu64, __func__, rsLayerId);
        layer->SetRSLayerId(rsLayerId);
        return layer;
    }
    layer = std::shared_ptr<RSSurfaceSolidFilledColorLayer>(new RSSurfaceSolidFilledColorLayer(rsLayerId, context));
    context->AddRSLayer(layer);
    return layer;
}
} // namespace Rosen
} // namespace OHOS