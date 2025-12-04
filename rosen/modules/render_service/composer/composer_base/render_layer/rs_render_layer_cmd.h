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

#ifndef RENDER_SERVICE_COMPOSER_BASE_RENDER_LAYER_RS_RENDER_LAYER_CMD_H
#define RENDER_SERVICE_COMPOSER_BASE_RENDER_LAYER_RS_RENDER_LAYER_CMD_H

#include <memory>
#include "rs_layer_cmd_type.h"
#include "rs_render_layer_cmd_property.h"

namespace OHOS {
namespace Rosen {
class RSLayerContext;
class RSB_EXPORT RSRenderLayerCmd {
public:
    RSRenderLayerCmd() = default;
    RSRenderLayerCmd(const RSRenderLayerCmd&) = delete;
    RSRenderLayerCmd(const RSRenderLayerCmd&&) = delete;
    RSRenderLayerCmd& operator=(const RSRenderLayerCmd&) = delete;
    RSRenderLayerCmd& operator=(const RSRenderLayerCmd&&) = delete;
    virtual ~RSRenderLayerCmd() = default;

    explicit RSRenderLayerCmd(const std::shared_ptr<RSRenderLayerPropertyBase>& property)
    {
        rsRenderLayerProperty_ = property ? property : std::make_shared<RSRenderLayerCmdProperty<bool>>();
    }

    virtual std::shared_ptr<RSRenderLayerPropertyBase> GetRSRenderLayerProperty() const = 0;
    virtual RSLayerCmdType GetRSRenderLayerCmdType() const
    {
        return RSLayerCmdType::INVALID;
    }
    virtual bool Marshalling(OHOS::MessageParcel& parcel) = 0;
    [[nodiscard]] static std::shared_ptr<RSRenderLayerCmd> Unmarshalling(OHOS::MessageParcel& parcel);

protected:
    std::shared_ptr<RSRenderLayerPropertyBase> rsRenderLayerProperty_;
};

#define DECLARE_RSLAYER_CMD(CMD_NAME, TYPE, CMD_TYPE, DELTA_OP, THRESHOLD_TYPE)                          \
class RSB_EXPORT RSRenderLayer##CMD_NAME##Cmd : public RSRenderLayerCmd {                                \
public:                                                                                                  \
    RSRenderLayer##CMD_NAME##Cmd(const std::shared_ptr<RSRenderLayerPropertyBase>& property)             \
    : RSRenderLayerCmd(property)                                                                         \
    {                                                                                                    \
        property->SetCmdType(RSLayerCmdType::CMD_TYPE);                                                  \
    }                                                                                                    \
    virtual ~RSRenderLayer##CMD_NAME##Cmd() = default;                                                   \
    bool Marshalling(OHOS::MessageParcel& parcel) override;                                              \
    RSLayerCmdType GetRSRenderLayerCmdType() const override                                              \
    {                                                                                                    \
        return (RSLayerCmdType::CMD_TYPE);                                                               \
    }                                                                                                    \
    std::shared_ptr<RSRenderLayerPropertyBase> GetRSRenderLayerProperty() const override;                \
};
#include "rs_layer_cmd_def.in"
DECLARE_RSLAYER_CMD(PixelMap, std::shared_ptr<Media::PixelMap>, PIXEL_MAP, Overwrite, ZERO)
#undef DECLARE_RSLAYER_CMD

} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_COMPOSER_BASE_RENDER_LAYER_RS_RENDER_LAYER_CMD_H
