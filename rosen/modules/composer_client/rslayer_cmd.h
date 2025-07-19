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

#ifndef COMPOSER_CLIENT_RS_LAYER_CMD_H
#define COMPOSER_CLIENT_RS_LAYER_CMD_H

#include "rslayer_property.h"

namespace OHOS {
namespace Rosen {

/**
 * @class RSLayerCmd
 * @brief The base class for all modifiers.
 */
class RSC_EXPORT RSLayerCmd : public std::enable_shared_from_this {
public:
    explicit RSLayerCmd(const std::shared_ptr<RSLayerProperty>& property)
        : rsLayerProperty_(property ? property : std::make_shared<RSLayerProperty>())
    {}

    RSLayerCmd(const std::shared_ptr<RSLayerProperty>& property, const RSLayerCmdType type)
        : property_(property ? property : std::make_shared<RSLayerProperty<bool>>())
    {
        rsLayerProperty_->type_ = type;
    }

    virtual ~RSLayerCmd() = default;

    void SetThresholdType() {}

    std::shared_ptr GetRSLayerProperty()
    {
        return rsLayerProperty_;
    }

    virtual RSLayerCmdType GetRsLayerCmdType() const
    {
        return RSLayerCmdType::INVALID;
    }

    std::shared_ptr<RSLayerProperty> GetRsRenderLayerProperty() const
    {
        return rsLayerProperty_->GetRsRenderLayerProperty();
    }

    virtual std::shared_ptr<RSRenderLayerCmd> CreateRenderLayerCmd();

protected:
    std::shared_ptr<RSLayerProperty> rsLayerProperty_;
};

} // namespace Rosen
} // namespace OHOS

#endif // COMPOSER_CLIENT_RS_LAYER_CMD_H