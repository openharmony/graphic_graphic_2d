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

#ifndef RENDER_SERVICE_COMPOSER_BASE_RENDER_LAYER_RS_LAYER_CMD_H
#define RENDER_SERVICE_COMPOSER_BASE_RENDER_LAYER_RS_LAYER_CMD_H

#include "rs_layer_cmd_property.h"
#include "rs_layer_cmd_type.h"
#include "rs_render_layer_cmd.h"

namespace OHOS {
namespace Rosen {

class RSLayerCmd : public std::enable_shared_from_this<RSLayerCmd> {
public:
    RSLayerCmd() = default;
    virtual ~RSLayerCmd() = default;

    explicit RSLayerCmd(const std::shared_ptr<RSLayerPropertyBase>& property)
    {
        rsLayerProperty_ = property ? property : std::make_shared<RSLayerCmdProperty<bool>>();
    }

    explicit RSLayerCmd(const std::shared_ptr<RSLayerPropertyBase>& property, const RSLayerCmdType type)
    {
        rsLayerProperty_ = property ? property : std::make_shared<RSLayerCmdProperty<bool>>();
        rsLayerCmdType_ = type;
    }

    RSLayerId GetLayerId() const
    {
        return rsLayerId_;
    }

    std::shared_ptr<RSLayerPropertyBase> GetRSLayerProperty() const
    {
        return rsLayerProperty_;
    }

    virtual RSLayerCmdType GetRSLayerCmdType() const
    {
        return rsLayerCmdType_;
    }

    std::shared_ptr<RSRenderLayerPropertyBase> GetRSRenderLayerProperty() const
    {
        return rsLayerProperty_->GetRSRenderLayerProperty();
    }

    virtual std::shared_ptr<RSRenderLayerCmd> CreateRenderLayerCmd()
    {
        return nullptr;
    }
protected:
    RSLayerId rsLayerId_ = 0;
    std::shared_ptr<RSLayerPropertyBase> rsLayerProperty_;
    RSLayerCmdType rsLayerCmdType_ = RSLayerCmdType::INVALID;

    friend class RSLayerPropertyBase;
    friend class RSRenderLayerPropertyBase;
};

#define DECLARE_RSLAYER_CMD(CMD_NAME, TYPE, CMD_TYPE, DELTA_OP, THRESHOLD_TYPE)                         \
class RSLayer##CMD_NAME##Cmd : public RSLayerCmd {                                                      \
    public:                                                                                             \
       RSLayer##CMD_NAME##Cmd(const std::shared_ptr<RSLayerPropertyBase>& property)                     \
            : RSLayerCmd(property)                                                                      \
        {}                                                                                              \
        virtual ~RSLayer##CMD_NAME##Cmd() = default;                                                    \
        RSLayerCmdType GetRSLayerCmdType() const override                                               \
        {                                                                                               \
            return rsLayerCmdType_;                                                                     \
        }                                                                                               \
        RSLayerCmdType rsLayerCmdType_ = RSLayerCmdType::CMD_TYPE;                                      \
        std::shared_ptr<RSRenderLayerCmd> CreateRenderLayerCmd() override                               \
        {                                                                                               \
            auto renderProperty = GetRSRenderLayerProperty();                                           \
            auto renderLayerCmd = std::make_shared<RSRenderLayer##CMD_NAME##Cmd>(renderProperty);       \
            return renderLayerCmd;                                                                      \
        }                                                                                               \
    };
#include "rs_layer_cmd_def.in"
DECLARE_RSLAYER_CMD(PixelMap, std::shared_ptr<Media::PixelMap>, PIXEL_MAP, Overwrite, ZERO)
#undef DECLARE_RSLAYER_CMD

} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_COMPOSER_BASE_RENDER_LAYER_RS_LAYER_CMD_H
