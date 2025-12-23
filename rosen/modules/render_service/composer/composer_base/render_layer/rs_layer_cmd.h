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

#include "rs_layer_cmd_type.h"
#include "rs_render_layer_cmd.h"

namespace OHOS {
namespace Rosen {

class RSLayerCmd : public std::enable_shared_from_this<RSLayerCmd> {
public:
    RSLayerCmd() = default;
    virtual ~RSLayerCmd() = default;

    RSLayerId GetLayerId() const
    {
        return rsLayerId_;
    }

    virtual RSLayerCmdType GetRSLayerCmdType() const
    {
        return RSLayerCmdType::INVALID;
    }

    virtual std::shared_ptr<RSRenderLayerCmd> CreateRenderLayerCmd()
    {
        return nullptr;
    }
protected:
    RSLayerId rsLayerId_ = 0;
};

#define DECLARE_RSLAYER_CMD(CMD_NAME, TYPE, CMD_TYPE)                                                \
class RSLayer##CMD_NAME##Cmd : public RSLayerCmd {                                                      \
public:                                                                                                 \
    explicit RSLayer##CMD_NAME##Cmd(const TYPE& value) : value_(value) {}                               \
    virtual ~RSLayer##CMD_NAME##Cmd() = default;                                                        \
    RSLayerCmdType GetRSLayerCmdType() const override                                                   \
    {                                                                                                   \
        return RSLayerCmdType::CMD_TYPE;                                                                \
    }                                                                                                   \
    std::shared_ptr<RSRenderLayerCmd> CreateRenderLayerCmd() override                                   \
    {                                                                                                   \
        auto renderProperty = std::make_shared<RSRenderLayerCmdProperty<TYPE>>(value_);                 \
        return std::make_shared<RSRenderLayer##CMD_NAME##Cmd>(renderProperty);                          \
    }                                                                                                   \
private:                                                                                                \
    TYPE value_;                                                                                        \
};
#include "rs_layer_cmd_def.in"
DECLARE_RSLAYER_CMD(PixelMap, std::shared_ptr<Media::PixelMap>, PIXEL_MAP)
#undef DECLARE_RSLAYER_CMD

} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_COMPOSER_BASE_RENDER_LAYER_RS_LAYER_CMD_H
