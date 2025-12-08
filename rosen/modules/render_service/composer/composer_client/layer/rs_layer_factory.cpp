/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "rs_layer_factory.h"
#include "rs_surface_layer.h"
#include "rs_surface_rcd_layer.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSLayer> RSLayerFactory::CreateRSLayer(const std::shared_ptr<RSRenderComposerClient>& client,
    RSLayerId rsLayerId, bool isSreenRCDLayer)
{
    if (client == nullptr) {
        RS_LOGE("RSLayerFactory::CreateRSLayer client is nullptr");
        return nullptr;
    }
    auto context = client->GetRSLayerContext();
    if (context == nullptr) {
        RS_LOGE("RSLayerFactory::CreateRSLayer context is nullptr");
        return nullptr;
    }
    std::shared_ptr<RSLayer> layer = context->GetRSLayer(rsLayerId);
    if (layer != nullptr) {
        RS_TRACE_NAME_FMT("RSLayerFactory::CreateRSLayer: use exist layer, id: %" PRIu64 ", name: %s",
            rsLayerId, layer->GetSurfaceName().c_str());
        RS_LOGD("RSLayerFactory::CreateRSLayer get cache layer by layer id: %{public}" PRIu64, rsLayerId);
        layer->SetRSLayerId(rsLayerId);
        return layer;
    }
    layer = isSreenRCDLayer ? std::make_shared<RSSurfaceRCDLayer>(rsLayerId, context):
        std::make_shared<RSSurfaceLayer>(rsLayerId, context);
    context->AddRSLayer(layer);
    return layer;
}
} // namespace Rosen
} // namespace OHOS