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
#include "src/core/SkReadBuffer.h"
#include "src/core/SkWriteBuffer.h"

#include "utils/data.h"
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
    return skRegion_->setRect(skIRect);
}

bool SkiaRegion::SetPath(const Path& path, const Region& clip)
{
    auto skPath = path.GetImpl<SkiaPath>()->GetPath();
    auto skRegion = clip.GetImpl<SkiaRegion>()->GetSkRegion();
    if (skRegion == nullptr) {
        LOGD("SkiaRegion::SetPath, skRegion is nullptr");
        return false;
    }

    return skRegion_->setPath(skPath, *skRegion);
}

bool SkiaRegion::GetBoundaryPath(Path* path) const
{
    if (!path) {
        return skRegion_->getBoundaryPath(nullptr);
    }
    auto skiaPath = path->GetImpl<SkiaPath>();
    if (!skiaPath) {
        LOGD("SkiaRegion::GetBoundaryPath, skiaPath is nullptr");
        return skRegion_->getBoundaryPath(nullptr);
    }
    SkPath skPath = skiaPath->GetPath();
    bool res = skRegion_->getBoundaryPath(&skPath);
    skiaPath->SetPath(skPath);
    return res;
}

bool SkiaRegion::IsIntersects(const Region& other) const
{
    auto skRegion = other.GetImpl<SkiaRegion>()->GetSkRegion();
    if (skRegion == nullptr) {
        LOGD("SkiaRegion::IsIntersects, skRegion is nullptr");
        return false;
    }

    return skRegion_->intersects(*skRegion);
}

bool SkiaRegion::IsEmpty() const
{
    return skRegion_->isEmpty();
}

bool SkiaRegion::IsRect() const
{
    return skRegion_->isRect();
}

bool SkiaRegion::Op(const Region& region, RegionOp op)
{
    auto skRegion = region.GetImpl<SkiaRegion>()->GetSkRegion();
    if (skRegion == nullptr) {
        LOGD("SkiaRegion::Op, skRegion is nullptr");
        return false;
    }
    return skRegion_->op(*skRegion, static_cast<SkRegion::Op>(op));
}

void SkiaRegion::Clone(const Region& other)
{
    auto skRegion = other.GetImpl<SkiaRegion>()->GetSkRegion();
    if (skRegion == nullptr) {
        LOGD("SkiaRegion::Clone, skRegion is nullptr");
        return;
    }
    *skRegion_ = *skRegion;
}

std::shared_ptr<Data> SkiaRegion::Serialize() const
{
    SkBinaryWriteBuffer writer;
    writer.writeRegion(*skRegion_);
    size_t length = writer.bytesWritten();
    std::shared_ptr<Data> data = std::make_shared<Data>();
    data->BuildUninitialized(length);
    writer.writeToMemory(data->WritableData());
    return data;
}

bool SkiaRegion::Deserialize(std::shared_ptr<Data> data)
{
    if (data == nullptr) {
        LOGD("SkiaRegion::Deserialize, data is invalid!");
        return false;
    }
    SkReadBuffer reader(data->GetData(), data->GetSize());
    reader.readRegion(skRegion_.get());
    return true;
}

std::shared_ptr<SkRegion> SkiaRegion::GetSkRegion() const
{
    return skRegion_;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
