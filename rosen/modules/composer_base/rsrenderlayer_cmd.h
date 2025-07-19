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

#ifndef COMPOSER_CLIENT_RS_RENDER_LAYER_CMD_H
#define COMPOSER_CLIENT_RS_RENDER_LAYER_CMD_H

#include <memory>

#include "parcel.h"
#include "rslayer_cmd_type.h"

namespace OHOS {
namespace Rosen {

class RSLayerContext {
public:
    RSLayerContext() = default;
    ~RSLayerContext() = default;
};

class RSC_EXPORT RSRenderLayerCmd {
public:
    RSRenderLayerCmd(const RSRenderLayerCmd&) = delete;
    RSRenderLayerCmd(const RSRenderLayerCmd&&) = delete;
    RSRenderLayerCmd& operator=(const RSRenderLayerCmd&) = delete;
    RSRenderLayerCmd& operator=(const RSRenderLayerCmd&&) = delete;
    virtual ~RSRenderLayerCmd() = default;

    virtual PropertyId GetPropertyId() = 0;
    virtual std::shared_ptr<RSRenderLayerProperty> GetProperty() const = 0;

    virtual RSLayerCmdType GetRsRenderLayerCmdType() const
    {
        return RSLayerCmdType::INVALID;
    }

    virtual void Apply(RSLayerContext& context) const = 0;
    virtual void Update(const std::shared_ptr<RSRenderLayerProperty>& prop, bool isDelta) = 0;
    virtual bool Marshalling(Parcel& parcel) = 0;
    [[nodiscard]] static RSRenderLayerCmd* Unmarshalling(Parcel& parcel);

protected:
    std::shared_ptr<RSRenderLayerProperty> property_;
};

#define DECLARE_RSLAYER_CMD(CMD_NAME, TYPE, CMD_TYPE, DELTA_OP, THRESHOLD_TYPE) \
    class RSB_EXPORT RSRender##CMD_NAME##Cmd : public RSRenderLayerCmd {              \
    public:                                                                                                      \
        RSRender##CMD_NAME##Cmd(const std::shared_ptr<RSRenderLayerProperty>& property)                 \
            : RSRenderLayerCmd(property)                                                        \
        {                                                                                                        \
            property->SetModifierType(RSLayerCmdType::CMD_TYPE);                                            \
        }                                                                                                        \
        virtual ~RSRender##CMD_NAME##Cmd() = default;                                                  \
        void Apply(RSLayerContext& context) const override;                                                   \
        void Update(const std::shared_ptr<RSRenderLayerProperty>& prop, bool isDelta) override;                   \
        bool Marshalling(Parcel& parcel) override;                                                               \
        RSLayerCmdType GetRsRenderLayerCmdType() override                                                                        \
        {                                                                                                        \
            return (RSLayerCmdType::CMD_TYPE);                                                              \
        }                                                                                                        \                                                                                                     \
    };

#include "rslayer_cmd_def.in"

#undef DECLARE_RSLAYER_CMD
} // namespace Rosen
} // namespace OHOS

#endif // COMPOSER_CLIENT_RS_RENDER_LAYER_CMD_H