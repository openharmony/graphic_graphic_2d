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

#include <refbase.h>
#include "rs_layer_parcel.h"
#include "rs_render_composer_context.h"
#include "rs_render_surface_layer.h"
#include "rs_render_surface_rcd_layer.h"
#include "rs_surface_layer_parcel.h"

#undef LOG_TAG
#define LOG_TAG "RSLayerParcelHelper"
namespace OHOS {
namespace Rosen {
void RSLayerParcelHelper::DestroyRSLayerCmd(std::shared_ptr<RSRenderComposerContext> context, RSLayerId layerId,
    const std::shared_ptr<RSRenderLayerCmd>& command)
{
    if (context == nullptr) {
        ROSEN_LOGE("%{public}s context is nullptr", __func__);
        return;
    }
    auto rsLayer = context->GetRSRenderLayer(layerId);
    if (rsLayer != nullptr) {
        ROSEN_LOGD("%{public}s remove rsLayer", __func__);
        context->RemoveRSRenderLayer(layerId);
    }
    ROSEN_LOGI("%{public}s rslayermap size:%{public}d", __func__,
        static_cast<int32_t>(context->GetRSRenderLayerCount()));
}

void RSLayerParcelHelper::UpdateRSLayerCmd(std::shared_ptr<RSRenderComposerContext> context, RSLayerId layerId,
    const std::shared_ptr<RSRenderLayerCmd>& command)
{
    if (context == nullptr) {
        ROSEN_LOGE("%{public}s context is nullptr", __func__);
        return;
    }
    auto rsLayer = context->GetRSRenderLayer(layerId);
    if (rsLayer == nullptr) {
        ROSEN_LOGI("%{public}s add rsLayer", __func__);
        rsLayer = std::make_shared<RSRenderSurfaceLayer>();
        context->AddRSRenderLayer(layerId, rsLayer);
    }
    rsLayer->UpdateRSLayerCmd(command);
    ROSEN_LOGD("%{public}s rslayermap size:%{public}d", __func__,
        static_cast<int32_t>(context->GetRSRenderLayerCount()));
}

void RSLayerParcelHelper::UpdateRSRCDLayerCmd(std::shared_ptr<RSRenderComposerContext> context, RSLayerId layerId,
    const std::shared_ptr<RSRenderLayerCmd>& command)
{
    if (context == nullptr) {
        ROSEN_LOGE("%{public}s context is nullptr", __func__);
        return;
    }
    auto rsLayer = context->GetRSRenderLayer(layerId);
    if (rsLayer == nullptr) {
        ROSEN_LOGD("%{public}s add rsLayer", __func__);
        rsLayer = std::make_shared<RSRenderSurfaceRCDLayer>();
        context->AddRSRenderLayer(layerId, rsLayer);
    }
    if (!rsLayer->IsScreenRCDLayer()) {
        // Rcdlayer initialized as RSRenderSurfaceLayer must be convert to RSRenderSurfaceRCDLayer vis pixelmap property
        auto rcdLayer = std::make_shared<RSRenderSurfaceRCDLayer>();
        rcdLayer->CopyLayerInfo(rsLayer);
        rcdLayer->UpdateRSLayerCmd(command);
        context->AddRSRenderLayer(layerId, rcdLayer);
        return;
    }
    rsLayer->UpdateRSLayerCmd(command);
    ROSEN_LOGD("%{public}s rslayermap size:%{public}d", __func__,
        static_cast<int32_t>(context->GetRSRenderLayerCount()));
}
} // namespace Rosen
} // namespace OHOS
