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
std::shared_ptr<RSPath> TexginePath::GetPath() const
{
    return path_;
}

void TexginePath::SetPath(const std::shared_ptr<RSPath> path)
{
    path_ = path;
}

TexginePath &TexginePath::AddOval(const TexgineRect &oval, TexginePathDirection dir)
{
    if (path_ != nullptr && oval.GetRect() != nullptr) {
        path_->AddOval(*oval.GetRect(), static_cast<Drawing::PathDirection>(dir));
    }
    return *this;
}

TexginePath &TexginePath::MoveTo(const TexginePoint &p)
{
    if (path_ != nullptr) {
        path_->MoveTo(p.fX, p.fY);
    }
    return *this;
}

TexginePath &TexginePath::QuadTo(const TexginePoint &p1, const TexginePoint &p2)
{
    RSPoint pointL(p1.fX, p1.fY);
    RSPoint pointR(p2.fX, p2.fY);
    if (path_ != nullptr) {
        path_->QuadTo(pointL, pointR);
    }
    return *this;
}

TexginePath &TexginePath::LineTo(const TexginePoint &p)
{
    if (path_ != nullptr) {
        path_->LineTo(p.fX, p.fY);
    }
    return *this;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
