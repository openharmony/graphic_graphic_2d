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

#include <refbase.h>
#include "rs_layer_parcel.h"
#include "rs_render_composer_context.h"
#include "rs_render_surface_layer.h"
#include "rs_render_surface_rcd_layer.h"
#include "rs_surface_layer_parcel.h"

namespace OHOS {
namespace Rosen {
void RSLayerParcelHelper::DestroyRSLayerCmd(std::shared_ptr<RSRenderComposerContext> context, RSLayerId layerId,
    const std::shared_ptr<RSRenderLayerCmd>& command)
{
    if (context == nullptr) {
        ROSEN_LOGE("RSLayerParcelHelper::DestroyRSLayerCmd context is nullptr");
        return;
    }
    auto rsLayer = context->GetRSRenderLayer(layerId);
    if (rsLayer != nullptr) {
        ROSEN_LOGD("RSLayerParcelHelper::DestroyRSLayerCmd remove rsLayer");
        context->RemoveRSRenderLayer(layerId);
    }
    ROSEN_LOGI("RSLayerParcelHelper::DestroyRSLayerCmd rslayermap size:%{public}d",
        static_cast<int32_t>(context->GetRSRenderLayerCount()));
}

void RSLayerParcelHelper::UpdateRSLayerCmd(std::shared_ptr<RSRenderComposerContext> context, RSLayerId layerId,
    const std::shared_ptr<RSRenderLayerCmd>& command)
{
    if (context == nullptr) {
        ROSEN_LOGE("RSLayerParcelHelper::UpdateRSLayerCmd context is nullptr");
        return;
    }
    auto rsLayer = context->GetRSRenderLayer(layerId);
    if (rsLayer == nullptr) {
        ROSEN_LOGI("RSLayerParcelHelper::UpdateRSLayerCmd add rsLayer");
        rsLayer = std::make_shared<RSRenderSurfaceLayer>();
        context->AddRSRenderLayer(layerId, rsLayer);
    }
    rsLayer->UpdateRSLayerCmd(command);
    ROSEN_LOGD("RSLayerParcelHelper::UpdateRSLayerCmd rslayermap size:%{public}d",
        static_cast<int32_t>(context->GetRSRenderLayerCount()));
}

void RSLayerParcelHelper::UpdateRSRCDLayerCmd(std::shared_ptr<RSRenderComposerContext> context, RSLayerId layerId,
    const std::shared_ptr<RSRenderLayerCmd>& command)
{
    if (context == nullptr) {
        ROSEN_LOGE("RSLayerParcelHelper::UpdateRSRCDLayerCmd context is nullptr");
        return;
    }
    auto rsLayer = context->GetRSRenderLayer(layerId);
    if (rsLayer == nullptr) {
        ROSEN_LOGD("RSLayerParcelHelper::UpdateRSRCDLayerCmd add rsLayer");
        rsLayer = std::make_shared<RSRenderSurfaceRCDLayer>();
        context->AddRSRenderLayer(layerId, rsLayer);
    }
    if (!rsLayer->IsScreenRCDLayer()) {
        auto rcdLayer = std::make_shared<RSRenderSurfaceRCDLayer>();
        rcdLayer->CopyLayerInfo(rsLayer);
        context->AddRSRenderLayer(layerId, rcdLayer);
    }
    rsLayer->UpdateRSLayerCmd(command);
    ROSEN_LOGD("RSLayerParcelHelper::UpdateRSRCDLayerCmd rslayermap size:%{public}d",
        static_cast<int32_t>(context->GetRSRenderLayerCount()));
}
} // namespace Rosen
} // namespace OHOS
