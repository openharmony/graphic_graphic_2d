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

#include "rsrenderlayer_cmd.h"

namespace OHOS {
namespace Rosen {

using ModifierUnmarshallingFunc = RSRenderLayerCmd* (*)(Parcel& parcel);

#define DECLARE_RSLAYER_CMD(CMD_NAME, TYPE, CMD_TYPE, DELTA_OP, THRESHOLD_TYPE) \
    {                                                                                                            \
        RSLayerCmdType::CMD_TYPE,                                                                           \
        [](Parcel& parcel) -> RSRenderLayerCmd* {                                                                \
            std::shared_ptr<RSRenderLayerProperty<TYPE>> prop;                                              \
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop)) {                                             \
                return nullptr;                                                                                  \
            }                                                                                                    \
            auto command = new RSRender##CMD_NAME##Cmd(prop);                                         \
            return command;                                                                                     \
        },                                                                                                       \
    },

static std::unordered_map<RSLayerCmdType, ModifierUnmarshallingFunc> funcLUT = {
#include "rslayer_cmd_def.in"
};
#undef DECLARE_RSLAYER_CMD

RSRenderLayerCmd* RSRenderLayerCmd::Unmarshalling(Parcel& parcel)
{
    int16_t type = 0;
    if (!parcel.ReadInt16(type)) {
        ROSEN_LOGE("RSRenderLayerCmd::Unmarshalling ReadInt16 failed");
        return nullptr;
    }
    auto it = funcLUT.find(static_cast<RSLayerCmdType>(type));
    if (it == funcLUT.end()) {
        ROSEN_LOGE("RSRenderLayerCmd Unmarshalling cannot find func in lut %{public}d", type);
        return nullptr;
    }
    return it->second(parcel);
}

#define DECLARE_RSLAYER_CMD(CMD_NAME, TYPE, CMD_TYPE, DELTA_OP, THRESHOLD_TYPE)    \
    bool RSRender##CMD_NAME##Cmd::Marshalling(Parcel& parcel)                                             \
    {                                                                                                               \
        auto renderProperty = std::static_pointer_cast<RSRenderLayerProperty<TYPE>>(property_);                \
        bool flag = parcel.WriteInt16(static_cast<int16_t>(RSLayerCmdType::CMD_TYPE)) &&                       \
               RSMarshallingHelper::Marshalling(parcel, renderProperty);                                            \
        if (!flag) {                                                                                                \
            ROSEN_LOGE("RSRenderLayerCmd::Marshalling WriteInt16 or Marshalling failed");         \
        }                                                                                                           \
        return flag;                                                                                                \
    }                                                                                                               \
    void RSRender##CMD_NAME##Cmd::Apply(RSLayerContext& context) const                                 \
    {                                                                                                               \
        auto renderProperty = std::static_pointer_cast<RSRenderLayerProperty<TYPE>>(property_);                \
        context.properties_.Set##CMD_NAME(                                                                     \
            DELTA_OP(context.properties_.Get##CMD_NAME(), renderProperty->Get()));                             \
    }                                                                                                               \
    void RSRender##CMD_NAME##Cmd::Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta) \
    {                                                                                                               \
        if (auto property = std::static_pointer_cast<RSRenderLayerProperty<TYPE>>(prop)) {                     \
            auto renderProperty = std::static_pointer_cast<RSRenderLayerProperty<TYPE>>(property_);            \
            renderProperty->Set(isDelta ? (renderProperty->Get() + property->Get()) : property->Get());             \
        }                                                                                                           \
    }

#include "rslayer_cmd_def.in"

#undef DECLARE_RSLAYER_CMD
} // namespace Rosen
} // namespace OHOS
