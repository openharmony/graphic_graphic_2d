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

#include "skia_region.h"

#include "include/core/SkRegion.h"
#include "include/core/SkRect.h"

#include "utils/region.h"
#include "utils/log.h"
#include "skia_path.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

SkiaRegion::SkiaRegion() noexcept : skRegion_(std::make_shared<SkRegion>()) {}

bool SkiaRegion::SetRect(const RectI& rectI)
{
    auto skIRect = SkIRect::MakeLTRB(rectI.GetLeft(), rectI.GetTop(), rectI.GetRight(), rectI.GetBottom());
    return (skRegion_ == nullptr) ? false : skRegion_->setRect(skIRect);
}

bool SkiaRegion::SetPath(const Path& path, const Region& clip)
{
    auto skPathImpl = path.GetImpl<SkiaPath>();
    if (skPathImpl == nullptr) {
        LOGE("SkiaRegion::SetPath, skPathImpl is nullptr");
        return false;
    }
    auto skPath = skPathImpl->GetPath();

    auto skRegionImpl = clip.GetImpl<SkiaRegion>();
    if (skRegionImpl == nullptr) {
        LOGE("SkiaRegion::SetPath, skRegionImpl is nullptr");
        return false;
    }
    auto skRegion = skRegionImpl->GetSkRegion();
    if (skRegion == nullptr) {
        LOGE("SkiaRegion::SetPath, skRegion is nullptr");
        return false;
    }

    return (skRegion_ == nullptr) ? false : skRegion_->setPath(skPath, *skRegion);
}

bool SkiaRegion::IsIntersects(const Region& other) const
{
    auto skRegionImpl = other.GetImpl<SkiaRegion>();
    if (skRegionImpl == nullptr) {
        LOGE("SkiaRegion::SetPath, skRegionImpl is nullptr");
        return false;
    }
    auto skRegion = skRegionImpl->GetSkRegion();
    if (skRegion == nullptr) {
        LOGE("SkiaRegion::SetPath, skRegion is nullptr");
        return false;
    }

    return (skRegion_ == nullptr) ? false : skRegion_->intersects(*skRegion);
}

bool SkiaRegion::Op(const Region& region, RegionOp op)
{
    auto skiaRegion = region.GetImpl<SkiaRegion>();
    if (skiaRegion == nullptr) {
        LOGE("SkiaRegion::SetPath, skiaRegion is nullptr");
        return false;
    }
    auto skRegion = skiaRegion->GetSkRegion();
    if (skRegion == nullptr) {
        LOGE("SkiaRegion::SetPath, skRegion is nullptr");
        return false;
    }
    return (skRegion_ == nullptr) ? false : skRegion_->op(*skRegion, static_cast<SkRegion::Op>(op));
}

std::shared_ptr<SkRegion> SkiaRegion::GetSkRegion() const
{
    return skRegion_;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
