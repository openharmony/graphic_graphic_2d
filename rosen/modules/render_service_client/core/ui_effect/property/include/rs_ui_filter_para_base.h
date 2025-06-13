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

#ifndef ROSEN_ENGINE_CORE_RENDER_UI_FILTER_PARA_BASE_H
#define ROSEN_ENGINE_CORE_RENDER_UI_FILTER_PARA_BASE_H

#include "modifier/rs_property.h"
#include "render/rs_render_filter_base.h"

namespace OHOS {
namespace Rosen {

class RSC_EXPORT RSUIFilterParaBase : public RSPropertyBase, public std::enable_shared_from_this<RSUIFilterParaBase> {
public:
    RSUIFilterParaBase(RSUIFilterType type) : type_(type) {}
    virtual ~RSUIFilterParaBase() = default;

    RSUIFilterType GetType() const;

    RSUIFilterType GetMaskType() const;

    virtual bool IsValid() const;

    virtual void GetDescription(std::string& out) const {}

    virtual void Dump(std::string& out) const;

    virtual bool Equals(const std::shared_ptr<RSUIFilterParaBase>& other) = 0;

    virtual void SetProperty(const std::shared_ptr<RSUIFilterParaBase>& other) = 0;

    RSPropertyType GetPropertyType() const override { return RSPropertyType::UI_FILTER; }
    void SetIsCustom(bool isCustom) override {}
    bool GetIsCustom() const override { return false; }
    void SetValue(const std::shared_ptr<RSPropertyBase>& value) override {}
    std::shared_ptr<RSPropertyBase> Clone() const override { return nullptr; }
    std::shared_ptr<RSRenderPropertyBase> GetRenderProperty() override { return nullptr; }

    virtual std::shared_ptr<RSRenderFilterParaBase> CreateRSRenderFilter()
    {
        return nullptr;
    }

    std::map<RSUIFilterType, std::shared_ptr<RSPropertyBase>> GetFilterProperties() const;

    template<typename PropertyType, typename T>
    inline void Setter(RSUIFilterType type, const T& value)
    {
        auto it = properties_.find(type);
        if (it != properties_.end()) {
            auto property = std::static_pointer_cast<PropertyType>(it->second);
            if (property) {
                property->Set(value);
            }
        } else {
            std::shared_ptr<RSPropertyBase> property = std::make_shared<PropertyType>(value);
            properties_[type] = property;
        }
    }

    std::shared_ptr<RSRenderPropertyBase> GetRSRenderProperty(RSUIFilterType type) const;

    inline std::shared_ptr<RSPropertyBase> GetRSProperty(RSUIFilterType type) const
    {
        auto it = properties_.find(type);
        if (it == properties_.end()) {
            return nullptr;
        }
        return it->second;
    }

    void Clear();

    virtual std::vector<std::shared_ptr<RSPropertyBase>> GetLeafProperties();

protected:
    std::weak_ptr<RSNode> node_;
    RSUIFilterType type_;
    RSUIFilterType maskType_ = RSUIFilterType::NONE;
    std::map<RSUIFilterType, std::shared_ptr<RSPropertyBase>> properties_;
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_ENGINE_CORE_RENDER_UI_FILTER_PARA_BASE_H