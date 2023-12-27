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
#ifndef USE_ROSEN_DRAWING
TexgineRect::TexgineRect(): rect_(std::make_shared<SkRect>())
#else
TexgineRect::TexgineRect(): rect_(std::make_shared<RSRect>())
#endif
{
#ifndef USE_ROSEN_DRAWING
    fBottom_ = &rect_->fBottom;
    fLeft_ = &rect_->fLeft;
    fTop_ = &rect_->fTop;
    fRight_ = &rect_->fRight;
#else
    fBottom_ = &rect_->bottom_;
    fLeft_ = &rect_->left_;
    fTop_ = &rect_->top_;
    fRight_ = &rect_->right_;
#endif
}

TexgineRect TexgineRect::MakeLTRB(float left, float top, float right, float bottom)
{
    auto rect = std::make_shared<TexgineRect>();
#ifndef USE_ROSEN_DRAWING
    rect->SetRect(SkRect::MakeLTRB(left, top, right, bottom));
#else
    rect->SetRect(RSRect { left, top, right, bottom });
#endif
    return *rect;
}

TexgineRect TexgineRect::MakeXYWH(float x, float y, float w, float h)
{
    auto rect = std::make_shared<TexgineRect>();
#ifndef USE_ROSEN_DRAWING
    rect->SetRect(SkRect::MakeXYWH(x, y, w, h));
#else
    rect->SetRect(RSRect { x, y, x + w, y + h });
#endif
    return *rect;
}

TexgineRect TexgineRect::MakeWH(float w, float h)
{
    auto rect = std::make_shared<TexgineRect>();
#ifndef USE_ROSEN_DRAWING
    rect->SetRect(SkRect::MakeWH(w, h));
#else
    rect->SetRect(RSRect { 0, 0, w, h });
#endif
    return *rect;
}

TexgineRect TexgineRect::MakeRRect(float x, float y, float w, float h, const SkVector skRadii[4])
{
    auto rect = std::make_shared<TexgineRect>();
#ifndef USE_ROSEN_DRAWING
    SkRect skRect = SkRect::MakeXYWH(x, y, w, h);
    SkRRect skRRect;
    skRRect.setRectRadii(skRect, skRadii);
    rect->SetRRect(skRRect);
#else
    RSRect rsRect = { x, y, x + w, y + h };
    RSPoint leftTop(skRadii[0].x(), skRadii[0].y()); // skRadii[0] is leftTop corner
    RSPoint rightTop(skRadii[1].x(), skRadii[1].y()); // skRadii[1] is rightTop corner
    RSPoint rightBottom(skRadii[2].x(), skRadii[2].y()); // skRadii[2] is rightBottom corner
    RSPoint leftBottom(skRadii[3].x(), skRadii[3].y()); // skRadii[3] is leftBottom corner
    std::vector<RSPoint> radiusXY = { leftTop, rightTop, rightBottom, leftBottom };
    rect->SetRRect(RSRoundRect { rsRect, radiusXY });
#endif
    return *rect;
}

#ifndef USE_ROSEN_DRAWING
std::shared_ptr<SkRect> TexgineRect::GetRect() const
#else
std::shared_ptr<RSRect> TexgineRect::GetRect() const
#endif
{
    return rect_;
}

#ifndef USE_ROSEN_DRAWING
std::shared_ptr<SkRRect> TexgineRect::GetRRect() const
#else
std::shared_ptr<RSRoundRect> TexgineRect::GetRRect() const
#endif
{
    return rrect_;
}

#ifndef USE_ROSEN_DRAWING
void TexgineRect::SetRect(const SkRect &rect)
#else
void TexgineRect::SetRect(const RSRect &rect)
#endif
{
#ifndef USE_ROSEN_DRAWING
    *rect_ = rect;
    fBottom_ = &rect_->fBottom;
    fLeft_ = &rect_->fLeft;
    fTop_ = &rect_->fTop;
    fRight_ = &rect_->fRight;
#else
    *rect_ = rect;
    fBottom_ = &rect_->bottom_;
    fLeft_ = &rect_->left_;
    fTop_ = &rect_->top_;
    fRight_ = &rect_->right_;
#endif
}

#ifndef USE_ROSEN_DRAWING
void TexgineRect::SetRRect(const SkRRect &rrect)
{
    rrect_ = std::make_shared<SkRRect>(std::move(rrect));
}
#else
void TexgineRect::SetRRect(const RSRoundRect &rrect)
{
    rrect_ = std::make_shared<RSRoundRect>(std::move(rrect));
}
#endif
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
