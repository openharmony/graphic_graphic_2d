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

#include "rs_render_surface_solid_filled_color_layer.h"
#include "rs_render_layer_cmd.h"

#undef LOG_TAG
#define LOG_TAG "RSRenderSurfaceSolidFilledColorLayer"
namespace OHOS {
namespace Rosen {

using RSSolidFilledColorLayerCmdHandler = std::function<void(std::shared_ptr<RSRenderSurfaceSolidFilledColorLayer>,
    const std::shared_ptr<Rosen::RSRenderLayerPropertyBase>&)>;
#define DECLARE_RSLAYER_CMD(CMD_NAME, TYPE, CMD_TYPE)                                                            \
    {                                                                                                            \
        RSLayerCmdType::CMD_TYPE,                                                                                \
        [](std::shared_ptr<RSRenderSurfaceSolidFilledColorLayer> rsRenderLayer,                                  \
            const std::shared_ptr<RSRenderLayerPropertyBase>& property) -> void {                                \
            std::shared_ptr<RSRenderLayerCmdProperty<TYPE>> prop =                                               \
                std::static_pointer_cast<RSRenderLayerCmdProperty<TYPE>>(property);                              \
            TYPE target = prop->Get();                                                                           \
            rsRenderLayer->Set##CMD_NAME(target);                                                                \
        },                                                                                                       \
    },

static std::unordered_map<RSLayerCmdType, RSSolidFilledColorLayerCmdHandler> cmdHandlers_ = {
#include "rs_layer_cmd_def.in"
};
#undef DECLARE_RSLAYER_CMD

void RSRenderSurfaceSolidFilledColorLayer::UpdateRSLayerCmd(const std::shared_ptr<RSRenderLayerCmd>& command)
{
    auto type = command->GetRSRenderLayerCmdType();
    auto property = command->GetRSRenderLayerProperty();
    std::shared_ptr<RSRenderSurfaceSolidFilledColorLayer> rsRenderLayer =
        std::static_pointer_cast<RSRenderSurfaceSolidFilledColorLayer>(shared_from_this());
    if (rsRenderLayer == nullptr) {
        return;
    }
    auto it = cmdHandlers_.find(type);
    if (it != cmdHandlers_.end()) {
        it->second(rsRenderLayer, property);
    } else {
        ROSEN_LOGD("%{public}s type err:%{public}d", __func__, static_cast<int32_t>(type));
    }
}

void RSRenderSurfaceSolidFilledColorLayer::CopyLayerInfo(const std::shared_ptr<RSLayer>& rsLayer)
{
    if (rsLayer == nullptr) {
        return;
    }
    RSRenderSurfaceLayer::CopyLayerInfo(rsLayer);
}
} // namespace Rosen
} // namespace OHOS