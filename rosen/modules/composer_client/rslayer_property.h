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

#ifndef COMPOSER_CLIENT_RS_LAYER_PROPERTY_H
#define COMPOSER_CLIENT_RS_LAYER_PROPERTY_H

#include <type_traits>
#include <unistd.h>

#include "rslayer_cmd_type.h"

namespace OHOS {
namespace Rosen {
/**
 * @class RSLayerProperty
 * @brief The class for RSLayerProperty.
 */
template <typename T>
class RSC_EXPORT RSLayerProperty : public std::enable_shared_from_this<RSLayerProperty> {
public:
    RSLayerProperty() = default;

    explicit RSLayerProperty(const T& value)
    {
        stagingValue_ = value;
    }

    ~RSLayerProperty() = default;

    using ValueType = T;

    virtual void Set(const T& value)
    {
        if (ROSEN_EQ(value, stagingValue_) || !IsValid(value)) {
            return;
        }
        stagingValue_ = value;
    }

    virtual T Get() const
    {
        return stagingValue_;
    }

    std::shared_ptr<RSRenderLayerProperty> GetRsRenderLayerProperty()
    {
        return std::make_shared<RSRenderLayerProperty<T>>(stagingValue_);
    }

protected:
    bool IsValid(const T& value)
    {
        return true;
    }

    RSLayerCmdType type_ { RSLayerCmdType::INVALID };

    T stagingValue_ {};
};

#define DECLARE_RSLAYER_PROPERTY(T, TYPE_ENUM) \
    template<>                             \
    inline const RSLayerPropertyType RSLayerProperty<T>::type_ = RSLayerPropertyType::TYPE_ENUM;    \
    template class RSLayerProperty<T>

#include "modifier/rs_property_def.in"

#undef DECLARE_RSLAYER_PROPERTY
} // namespace Rosen
} // namespace OHOS

#endif // COMPOSER_CLIENT_RS_LAYER_PROPERTY_H