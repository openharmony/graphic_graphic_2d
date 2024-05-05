/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "texgine_rect.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
TexgineRect::TexgineRect(): rect_(std::make_shared<RSRect>())
{
    fBottom_ = &rect_->bottom_;
    fLeft_ = &rect_->left_;
    fTop_ = &rect_->top_;
    fRight_ = &rect_->right_;
}

TexgineRect TexgineRect::MakeLTRB(float left, float top, float right, float bottom)
{
    auto rect = std::make_shared<TexgineRect>();
    rect->SetRect(RSRect { left, top, right, bottom });
    return *rect;
}

TexgineRect TexgineRect::MakeXYWH(float x, float y, float w, float h)
{
    auto rect = std::make_shared<TexgineRect>();
    rect->SetRect(RSRect { x, y, x + w, y + h });
    return *rect;
}

TexgineRect TexgineRect::MakeWH(float w, float h)
{
    auto rect = std::make_shared<TexgineRect>();
    rect->SetRect(RSRect { 0, 0, w, h });
    return *rect;
}

TexgineRect TexgineRect::MakeRRect(float x, float y, float w, float h, const SkVector skRadii[4])
{
    auto rect = std::make_shared<TexgineRect>();
    RSRect rsRect = { x, y, x + w, y + h };
    RSPoint leftTop(skRadii[0].x(), skRadii[0].y()); // skRadii[0] is leftTop corner
    RSPoint rightTop(skRadii[1].x(), skRadii[1].y()); // skRadii[1] is rightTop corner
    RSPoint rightBottom(skRadii[2].x(), skRadii[2].y()); // skRadii[2] is rightBottom corner
    RSPoint leftBottom(skRadii[3].x(), skRadii[3].y()); // skRadii[3] is leftBottom corner
    std::vector<RSPoint> radiusXY = { leftTop, rightTop, rightBottom, leftBottom };
    rect->SetRRect(RSRoundRect { rsRect, radiusXY });
    return *rect;
}

std::shared_ptr<RSRect> TexgineRect::GetRect() const
{
    return rect_;
}

std::shared_ptr<RSRoundRect> TexgineRect::GetRRect() const
{
    return rrect_;
}

void TexgineRect::SetRect(const RSRect &rect)
{
    *rect_ = rect;
    fBottom_ = &rect_->bottom_;
    fLeft_ = &rect_->left_;
    fTop_ = &rect_->top_;
    fRight_ = &rect_->right_;
}

void TexgineRect::SetRRect(const RSRoundRect &rrect)
{
    rrect_ = std::make_shared<RSRoundRect>(std::move(rrect));
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
