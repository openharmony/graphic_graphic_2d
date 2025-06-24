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

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_BACKGROUND_RS_BACKGROUND_IMAGE_MODIFIER_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_BACKGROUND_RS_BACKGROUND_IMAGE_MODIFIER_H

#include "modifier_ng/rs_modifier_ng.h"

namespace OHOS::Rosen::ModifierNG {
class RSC_EXPORT RSBackgroundImageModifier : public RSModifier {
public:
    RSBackgroundImageModifier() = default;
    ~RSBackgroundImageModifier() override = default;

    static inline constexpr auto Type = RSModifierType::BACKGROUND_IMAGE;
    RSModifierType GetType() const override
    {
        return Type;
    };

    void SetBgImage(const std::shared_ptr<RSImage>& image);
    void SetBgImageInnerRect(const Vector4f& innerRect);
    void SetBgImageWidth(float width);
    void SetBgImageHeight(float height);
    void SetBgImagePositionX(float positionX);
    void SetBgImagePositionY(float positionY);
    void SetBgImageDstRect(const Vector4f& dstRect);

    std::shared_ptr<RSImage> GetBgImage() const;
    Vector4f GetBgImageInnerRect() const;
    float GetBgImageWidth() const;
    float GetBgImageHeight() const;
    float GetBgImagePositionX() const;
    float GetBgImagePositionY() const;
    Vector4f GetBgImageDstRect() const;
};
} // namespace OHOS::Rosen::ModifierNG
#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_BACKGROUND_RS_BACKGROUND_IMAGE_MODIFIER_H
