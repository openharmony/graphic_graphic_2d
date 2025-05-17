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

namespace OHOS {
namespace Rosen {
class RippleMaskPara : public MaskPara {
public:
    RippleMaskPara()
    {
        type_ = MaskPara::Type::RIPPLE_MASK;
    }
    ~RippleMaskPara() override = default;

    void SetCenter(Vector2f& center)
    {
        center_ = center;
    }

    const Vector2f& GetCenter() const
    {
        return center_;
    }

    void SetRadius(float radius)
    {
        radius_ = radius;
    }

    const float& GetRadius() const
    {
        return radius_;
    }

    void SetWidth(float width)
    {
        width_ = width;
    }

    const float& GetWidth() const
    {
        return width_;
    }

private:
    Vector2f center_;
    float radius_;
    float width_;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_RIPPLE_MASK_PARA_H
