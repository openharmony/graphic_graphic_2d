/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PROP_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PROP_H

#include "modifier/rs_animatable_arithmetic.h"
#include "common/rs_common_def.h"
#include "pipeline/rs_base_render_node.h"

namespace OHOS {
namespace Rosen {
class RSRenderPropertyBase {
public:
    RSRenderPropertyBase(const PropertyId& id) : id_(id) {}

    PropertyId GetId() const
    {
        return id_;
    }

    void Attach(std::weak_ptr<RSBaseRenderNode> node)
    {
        node_ = node;
    }

protected:
    void OnChange() const
    {
        if (auto node = node_.lock()) {
            node->SetDirty();
        }
    }
    PropertyId id_;
    std::weak_ptr<RSBaseRenderNode> node_;
};

template<typename T>
class RSRenderProperty : public RSRenderPropertyBase {
public:
    RSRenderProperty() : RSRenderPropertyBase(0) {}
    RSRenderProperty(const T& value, const PropertyId& id) : RSRenderPropertyBase(id), stagingValue_(value) {}
    virtual ~RSRenderProperty() = default;

    void Set(const T& value)
    {
        if (stagingValue_ != value) {
            stagingValue_ = value;
            OnChange();
        }
    }

    T Get() const
    {
        return stagingValue_;
    }

private:
    T stagingValue_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PROP_H
