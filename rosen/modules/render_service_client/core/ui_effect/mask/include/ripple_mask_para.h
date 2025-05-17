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
 #ifndef UIEFFECT_RIPPLE_MASK_PARA_H
 #define UIEFFECT_RIPPLE_MASK_PARA_H
 #include "ui_effect/mask/include/mask_para.h"
 #include "common/rs_vector2.h"
 #include "ui_effect/utils.h"

namespace OHOS {
namespace Rosen {
// limits for ripple mask center parameters
constexpr std::pair<float, float> RIPPLE_MASK_CENTER_LIMITS { -10.0f, 10.0f };
// limits for ripple mask radius parameters
constexpr std::pair<float, float> RIPPLE_MASK_RADIUS_LIMITS { 0.f, 10.0f };
// limits for ripple mask width parameters
constexpr std::pair<float, float> RIPPLE_MASK_WIDTH_LIMITS { 0.f, 10.0f };
// limits for ripple mask offset parameters
constexpr std::pair<float, float> RIPPLE_MASK_OFFSET_LIMITS { -1.0f, 1.0f };

class RippleMaskPara : public MaskPara {
public:
    RippleMaskPara()
    {
        type_ = MaskPara::Type::RIPPLE_MASK;
    }
    ~RippleMaskPara() override = default;

    void SetCenter(Vector2f& center)
    {
        center_ = UIEffect::GetLimitedPara(center, RIPPLE_MASK_CENTER_LIMITS);
    }

    const Vector2f& GetCenter() const
    {
        return center_;
    }

    void SetRadius(float radius)
    {
        radius_ = UIEffect::GetLimitedPara(radius, RIPPLE_MASK_RADIUS_LIMITS);
    }

    const float& GetRadius() const
    {
        return radius_;
    }

    void SetWidth(float width)
    {
        width_ = UIEffect::GetLimitedPara(width, RIPPLE_MASK_WIDTH_LIMITS);
    }

    const float& GetWidth() const
    {
        return width_;
    }

    void SetWidthCenterOffset(float widthCenterOffset)
    {
        widthCenterOffset_ = UIEffect::GetLimitedPara(widthCenterOffset, RIPPLE_MASK_OFFSET_LIMITS);
    }

    const float& GetWidthCenterOffset() const
    {
        return widthCenterOffset_;
    }

private:
    Vector2f center_ = { 0.0f, 0.0f };
    float radius_ = 0.0f;
    float width_ = 0.0f;
    float widthCenterOffset_ = 0.0f;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_RIPPLE_MASK_PARA_H
