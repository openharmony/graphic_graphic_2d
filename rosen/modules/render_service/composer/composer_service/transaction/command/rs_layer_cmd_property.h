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

#ifndef RENDER_SERVICE_COMPOSER_SERVICE_TRANSACTION_COMMAND_RS_LAYER_CMD_PROPERTY_H
#define RENDER_SERVICE_COMPOSER_SERVICE_TRANSACTION_COMMAND_RS_LAYER_CMD_PROPERTY_H

#include <type_traits>

#include "rs_layer_cmd_type.h"
#include "rs_layer_common_def.h"
#include "rs_render_layer_cmd_property.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSLayerPropertyBase {
public:
    RSLayerPropertyBase() = default;
    virtual ~RSLayerPropertyBase() = default;

protected:
    virtual void SetValue(const std::shared_ptr<RSLayerPropertyBase>& value) = 0;
    virtual std::shared_ptr<RSLayerPropertyBase> Clone() const = 0;
    virtual std::shared_ptr<RSRenderLayerPropertyBase> GetRSRenderLayerProperty() = 0;

    friend class RSLayerCmd;
};
template<typename T>
class RSLayerCmdProperty : public RSLayerPropertyBase {
public:
    RSLayerCmdProperty() = default;
    explicit RSLayerCmdProperty(const T& value)
    {
        stagingValue_ = value;
    }

    ~RSLayerCmdProperty() override = default;

    using ValueType = T;

    virtual void Set(const T& value)
    {
        stagingValue_ = value;
    }

    virtual T Get() const
    {
        return stagingValue_;
    }
protected:
    void SetValue(const std::shared_ptr<RSLayerPropertyBase>& value) override
    {
        auto property = std::static_pointer_cast<RSLayerCmdProperty<T>>(value);
        if (property != nullptr) {
            stagingValue_ = property->stagingValue_;
        }
    }

    std::shared_ptr<RSLayerPropertyBase> Clone() const override
    {
        return std::make_shared<RSLayerCmdProperty<T>>(stagingValue_);
    }

    bool IsValid(const T& value)
    {
        return true;
    }

    std::shared_ptr<RSRenderLayerPropertyBase> GetRSRenderLayerProperty() override
    {
        return std::make_shared<RSRenderLayerCmdProperty<T>>(stagingValue_);
    }

    T stagingValue_ {};
    friend class RSLayerCmd;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_COMPOSER_SERVICE_TRANSACTION_COMMAND_RS_LAYER_CMD_PROPERTY_H
