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

#include "texgine_path.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#ifndef USE_ROSEN_DRAWING
std::shared_ptr<SkPath> TexginePath::GetPath() const
#else
std::shared_ptr<RSPath> TexginePath::GetPath() const
#endif
{
    return path_;
}

#ifndef USE_ROSEN_DRAWING
void TexginePath::SetPath(const std::shared_ptr<SkPath> path)
#else
void TexginePath::SetPath(const std::shared_ptr<RSPath> path)
#endif
{
    path_ = path;
}

TexginePath &TexginePath::AddOval(const TexgineRect &oval, TexginePathDirection dir)
{
    if (path_ != nullptr && oval.GetRect() != nullptr) {
#ifndef USE_ROSEN_DRAWING
        path_->addOval(*oval.GetRect(), static_cast<SkPathDirection>(dir));
#else
        path_->AddOval(*oval.GetRect(), static_cast<Drawing::PathDirection>(dir));
#endif
    }
    return *this;
}

TexginePath &TexginePath::MoveTo(const TexginePoint &p)
{
#ifndef USE_ROSEN_DRAWING
    if (path_ != nullptr) {
        path_->moveTo(SkPoint { p.fX, p.fY });
    }
#else
    if (path_ != nullptr) {
        path_->MoveTo(p.fX, p.fY);
    }
#endif
    return *this;
}

TexginePath &TexginePath::QuadTo(const TexginePoint &p1, const TexginePoint &p2)
{
#ifndef USE_ROSEN_DRAWING
    SkPoint point1({p1.fX, p1.fY});
    SkPoint point2({p2.fX, p2.fY});
    if (path_ != nullptr) {
        path_->quadTo(point1, point2);
    }
#else
    RSPoint pointL(p1.fX, p1.fY);
    RSPoint pointR(p2.fX, p2.fY);
    if (path_ != nullptr) {
        path_->QuadTo(pointL, pointR);
    }
#endif
    return *this;
}

TexginePath &TexginePath::LineTo(const TexginePoint &p)
{
#ifndef USE_ROSEN_DRAWING
    if (path_ != nullptr) {
        path_->lineTo(SkPoint { p.fX, p.fY });
    }
#else
    if (path_ != nullptr) {
        path_->LineTo(p.fX, p.fY);
    }
#endif
    return *this;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
