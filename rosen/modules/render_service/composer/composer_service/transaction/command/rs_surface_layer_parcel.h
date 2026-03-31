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

#ifndef RENDER_SERVICE_COMPOSER_SERVICE_TRANSACTION_COMMAND_RS_SURFACE_LAYER_PARCEL_H
#define RENDER_SERVICE_COMPOSER_SERVICE_TRANSACTION_COMMAND_RS_SURFACE_LAYER_PARCEL_H

#include "rs_layer_common_def.h"
#include "rs_layer_parcel_templates.h"

namespace OHOS {
namespace Rosen {
class RSRenderComposerContext;

class RSB_EXPORT RSLayerParcelHelper {
public:
    static void DestroyRSLayerCmd(std::shared_ptr<RSRenderComposerContext> context, RSLayerId layerId,
        const std::shared_ptr<RSRenderLayerCmd>& command);
    static void UpdateRSLayerCmd(std::shared_ptr<RSRenderComposerContext> context, RSLayerId layerId,
        const std::shared_ptr<RSRenderLayerCmd>& command);
    static void UpdateRSRCDLayerCmd(std::shared_ptr<RSRenderComposerContext> context, RSLayerId layerId,
        const std::shared_ptr<RSRenderLayerCmd>& command);
};

ADD_COMMAND(RSDestroyRSLayerCmd,
    ARG(RS_LAYER_DESTROY,
        RSLayerParcelHelper::DestroyRSLayerCmd, RSLayerId, std::shared_ptr<RSRenderLayerCmd>))

ADD_COMMAND(RSUpdateRSLayerCmd,
    ARG(RS_LAYER_UPDATE,
        RSLayerParcelHelper::UpdateRSLayerCmd, RSLayerId, std::shared_ptr<RSRenderLayerCmd>))

ADD_COMMAND(RSUpdateRSRCDLayerCmd,
    ARG(RS_RCD_LAYER_UPDATE,
        RSLayerParcelHelper::UpdateRSRCDLayerCmd, RSLayerId, std::shared_ptr<RSRenderLayerCmd>))

} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_COMPOSER_SERVICE_TRANSACTION_COMMAND_RS_SURFACE_LAYER_PARCEL_H
