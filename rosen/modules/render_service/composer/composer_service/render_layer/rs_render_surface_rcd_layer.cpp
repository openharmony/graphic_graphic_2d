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

#include "rs_render_surface_rcd_layer.h"
#include "rs_render_layer_cmd.h"

namespace OHOS {
namespace Rosen {

using RSRCDLayerCmdHandler = std::function<void(std::shared_ptr<RSRenderSurfaceRCDLayer>,
    const std::shared_ptr<Rosen::RSRenderLayerPropertyBase>&)>;
#define DECLARE_RSLAYER_CMD(CMD_NAME, TYPE, CMD_TYPE)                                                            \
    {                                                                                                            \
        RSLayerCmdType::CMD_TYPE,                                                                                \
        [](std::shared_ptr<RSRenderSurfaceRCDLayer> rsRenderLayer,                                               \
            const std::shared_ptr<RSRenderLayerPropertyBase>& property) -> void {                                \
            std::shared_ptr<RSRenderLayerCmdProperty<TYPE>> prop =                                               \
                std::static_pointer_cast<RSRenderLayerCmdProperty<TYPE>>(property);                              \
            TYPE target = prop->Get();                                                                           \
            rsRenderLayer->Set##CMD_NAME(target);                                                                \
        },                                                                                                       \
    },

static std::unordered_map<RSLayerCmdType, RSRCDLayerCmdHandler> cmdHandlers_ = {
#include "rs_layer_cmd_def.in"
DECLARE_RSLAYER_CMD(PixelMap, std::shared_ptr<Media::PixelMap>, PIXEL_MAP)
};
#undef DECLARE_RSLAYER_CMD

void RSRenderSurfaceRCDLayer::UpdateRSLayerCmd(const std::shared_ptr<RSRenderLayerCmd>& command)
{
    auto type = command->GetRSRenderLayerCmdType();
    auto property = command->GetRSRenderLayerProperty();
    std::shared_ptr<RSRenderSurfaceRCDLayer> rsRenderLayer =
        std::static_pointer_cast<RSRenderSurfaceRCDLayer>(shared_from_this());
    if (rsRenderLayer == nullptr) {
        return;
    }
    auto it = cmdHandlers_.find(type);
    if (it != cmdHandlers_.end()) {
        it->second(rsRenderLayer, property);
    } else {
        ROSEN_LOGD("RSRenderSurfaceRCDLayer::UpdateRSLayerCmd type err");
    }
    ROSEN_LOGD("RSRenderSurfaceRCDLayer::UpdateRSLayerCmd type:%{}d", static_cast<int32_t>(type));
}
} // namespace Rosen
} // namespace OHOS
