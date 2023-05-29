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
std::shared_ptr<SkPath> TexginePath::GetPath() const
{
    return path_;
}

void TexginePath::SetPath(const std::shared_ptr<SkPath> path)
{
    path_ = path;
}

TexginePath &TexginePath::AddOval(const TexgineRect &oval, TexginePathDirection dir)
{
    if (path_ != nullptr && oval.GetRect() != nullptr) {
        *path_ = path_->addOval(*oval.GetRect(), static_cast<SkPath::Direction>(dir));
    }

    return *this;
}

TexginePath &TexginePath::MoveTo(const TexginePoint &p)
{
    SkPoint point({p.fX, p.fY});
    if (path_ != nullptr) {
        *path_ = path_->moveTo(point);
    }

    return *this;
}

TexginePath &TexginePath::QuadTo(const TexginePoint &p1, const TexginePoint &p2)
{
    SkPoint point1({p1.fX, p1.fY});
    SkPoint point2({p2.fX, p2.fY});
    if (path_ != nullptr) {
        *path_ = path_->quadTo(point1, point2);
    }

    return *this;
}

TexginePath &TexginePath::LineTo(const TexginePoint &p)
{
    SkPoint point({p.fX, p.fY});
    if (path_ != nullptr) {
        *path_ = path_->lineTo(point);
    }

    return *this;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
