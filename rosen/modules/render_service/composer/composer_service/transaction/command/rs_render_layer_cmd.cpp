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
#include "rs_render_layer_cmd.h"
#include <unordered_map>
#include "hdi_display_type.h"
#include "iconsumer_surface.h"
#include "platform/common/rs_log.h"
#include "rs_layer_marshalling_helper.h"

namespace OHOS {
namespace Rosen {

using RSRenderLayerCmdUnmarshallingFunc = std::shared_ptr<RSRenderLayerCmd> (*)(OHOS::MessageParcel& parcel);
#define DECLARE_RSLAYER_CMD(CMD_NAME, TYPE, CMD_TYPE)                                                         \
    {                                                                                                            \
        RSLayerCmdType::CMD_TYPE,                                                                                \
        [](OHOS::MessageParcel& parcel) -> std::shared_ptr<RSRenderLayerCmd> {                                   \
            std::shared_ptr<RSRenderLayerCmdProperty<TYPE>> prop;                                                \
            prop = std::make_shared<RSRenderLayerCmdProperty<TYPE>>();                                           \
            if (!prop->OnUnmarshalling(parcel, prop)) {                                                          \
                return nullptr;                                                                                  \
            }                                                                                                    \
            auto command = std::make_shared<RSRenderLayer##CMD_NAME##Cmd>(prop);                                 \
            return command;                                                                                      \
        },                                                                                                       \
    },

static std::unordered_map<RSLayerCmdType, RSRenderLayerCmdUnmarshallingFunc> funcRSRLUT = {
#include "rs_layer_cmd_def.in"
DECLARE_RSLAYER_CMD(PixelMap, std::shared_ptr<Media::PixelMap>, PIXEL_MAP)
};
#undef DECLARE_RSLAYER_CMD

std::shared_ptr<RSRenderLayerCmd> RSRenderLayerCmd::Unmarshalling(OHOS::MessageParcel& parcel)
{
    uint16_t type = 0;
    if (!parcel.ReadUint16(type)) {
        return nullptr;
    }
    auto it = funcRSRLUT.find(static_cast<RSLayerCmdType>(type));
    if (it == funcRSRLUT.end()) {
        return nullptr;
    }
    return it->second(parcel);
}

#define DECLARE_RSLAYER_CMD(CMD_NAME, TYPE, CMD_TYPE)                                                          \
bool RSRenderLayer##CMD_NAME##Cmd::Marshalling(OHOS::MessageParcel& parcel)                                       \
{                                                                                                                 \
    auto renderProperty = std::static_pointer_cast<RSRenderLayerCmdProperty<TYPE>>(rsRenderLayerProperty_);       \
    bool flag = parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::CMD_TYPE)) &&                            \
        (renderProperty != nullptr && renderProperty->OnMarshalling(parcel, renderProperty->Get()));              \
    if (!flag) {                                                                                                  \
        ROSEN_LOGE("RSRenderLayerProperty::Marshalling failed");                                                  \
    }                                                                                                             \
    return flag;                                                                                                  \
}                                                                                                                 \
std::shared_ptr<RSRenderLayerPropertyBase> RSRenderLayer##CMD_NAME##Cmd::GetRSRenderLayerProperty() const         \
{                                                                                                                 \
    return rsRenderLayerProperty_;                                                                                \
}
#include "rs_layer_cmd_def.in"
DECLARE_RSLAYER_CMD(PixelMap, std::shared_ptr<Media::PixelMap>, PIXEL_MAP)
#undef DECLARE_RSLAYER_CMD
} // namespace Rosen
} // namespace OHOS
