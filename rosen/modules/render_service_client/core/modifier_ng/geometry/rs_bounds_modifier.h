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

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_GEOMETRY_RS_BOUNDS_MODIFIER_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_GEOMETRY_RS_BOUNDS_MODIFIER_H

#include "modifier_ng/rs_modifier_ng.h"

namespace OHOS::Rosen::ModifierNG {
class RSC_EXPORT RSBoundsModifier : public RSModifier {
public:
    RSBoundsModifier() = default;
    ~RSBoundsModifier() = default;

    static inline constexpr auto Type = RSModifierType::BOUNDS;
    RSModifierType GetType() const override
    {
        return Type;
    }

    void MarkNodeDirty() override
    {
        if (auto node = node_.lock()) {
            node->MarkDirty(NodeDirtyType::GEOMETRY, true);
        }
    }

    void ApplyGeometry(const std::shared_ptr<RSObjAbsGeometry>& geometry);

    void SetBounds(Vector4f bounds);
    void SetBoundsSize(Vector2f size);
    void SetBoundsWidth(float width);
    void SetBoundsHeight(float height);
    void SetBoundsPosition(Vector2f position);
    void SetBoundsPositionX(float positionX);
    void SetBoundsPositionY(float positionY);

    Vector4f GetBounds() const;
    Vector2f GetBoundsSize() const;
    float GetBoundsWidth() const;
    float GetBoundsHeight() const;
    Vector2f GetBoundsPosition() const;
    float GetBoundsPositionX() const;
    float GetBoundsPositionY() const;
};
} // namespace OHOS::Rosen::ModifierNG
#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_GEOMETRY_RS_BOUNDS_MODIFIER_H
