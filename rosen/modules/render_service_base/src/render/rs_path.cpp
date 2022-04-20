/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "render/rs_path.h"

#include "include/core/SkPath.h"
#include "include/core/SkMatrix.h"
#include "include/core/SkPathMeasure.h"
#include "include/utils/SkParsePath.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSPath> RSPath::CreateRSPath()
{
    return std::make_shared<RSPath>();
}

std::shared_ptr<RSPath> RSPath::CreateRSPath(const SkPath& skPath)
{
    auto rsPath = std::make_shared<RSPath>();
    rsPath->SetSkiaPath(skPath);
    return rsPath;
}

std::shared_ptr<RSPath> RSPath::CreateRSPath(const std::string& path)
{
    SkPath skAnimationPath;
    SkParsePath::FromSVGString(path.c_str(), &skAnimationPath);
    return RSPath::CreateRSPath(skAnimationPath);
}

RSPath::RSPath()
{
    skPath_ = new SkPath();
}

RSPath::~RSPath()
{
    if (skPath_) {
        delete skPath_;
    }
}

const SkPath& RSPath::GetSkiaPath() const
{
    return *skPath_;
}

void RSPath::SetSkiaPath(const SkPath& skPath)
{
    if (skPath_) {
        delete skPath_;
    }
    skPath_ = new SkPath(skPath);
}

std::shared_ptr<RSPath> RSPath::Reverse()
{
    SkPath path;
    path.reverseAddPath(*skPath_);
    return CreateRSPath(path);
}

float RSPath::GetDistance() const
{
    SkPathMeasure pathMeasure(*skPath_, false);
    return pathMeasure.getLength();
}

bool RSPath::GetPosTan(float distance, Vector2f& pos, float& degrees) const
{
    SkPoint position;
    SkVector tangent;
    SkPathMeasure pathMeasure(*skPath_, false);
    bool ret = pathMeasure.getPosTan(distance, &position, &tangent);
    if (!ret) {
        ROSEN_LOGE("SkPathMeasure get failed");
        return false;
    }
    pos.data_[0] = position.x();
    pos.data_[1] = position.y();
    degrees = SkRadiansToDegrees(std::atan2(tangent.y(), tangent.x()));
    return true;
}
} // namespace Rosen
} // namespace OHOS
