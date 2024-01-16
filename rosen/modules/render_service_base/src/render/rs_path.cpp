/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef USE_ROSEN_DRAWING
#include "include/core/SkPath.h"
#include "include/core/SkMatrix.h"
#include "include/core/SkPathMeasure.h"
#include "include/utils/SkParsePath.h"
#else
#include "draw/path.h"
#include "utils/matrix.h"
#include "utils/scalar.h"
#endif

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSPath> RSPath::CreateRSPath()
{
    return std::make_shared<RSPath>();
}

#ifndef USE_ROSEN_DRAWING
std::shared_ptr<RSPath> RSPath::CreateRSPath(const SkPath& skPath)
{
    auto rsPath = std::make_shared<RSPath>();
    rsPath->SetSkiaPath(skPath);
    return rsPath;
}
#else
std::shared_ptr<RSPath> RSPath::CreateRSPath(const Drawing::Path& path)
{
    auto rsPath = std::make_shared<RSPath>();
    rsPath->SetDrawingPath(path);
    return rsPath;
}
#endif

std::shared_ptr<RSPath> RSPath::CreateRSPath(const std::string& path)
{
#ifndef USE_ROSEN_DRAWING
    SkPath skAnimationPath;
    SkParsePath::FromSVGString(path.c_str(), &skAnimationPath);
    return RSPath::CreateRSPath(skAnimationPath);
#else
    Drawing::Path drAnimationPath;
    drAnimationPath.BuildFromSVGString(path);
    return RSPath::CreateRSPath(drAnimationPath);
#endif
}

RSPath::RSPath()
{
#ifndef USE_ROSEN_DRAWING
    skPath_ = new SkPath();
#else
    drPath_ = new Drawing::Path();
#endif
}

RSPath::~RSPath()
{
#ifndef USE_ROSEN_DRAWING
    if (skPath_) {
        delete skPath_;
    }
#else
    if (drPath_) {
        delete drPath_;
    }
#endif
}

#ifndef USE_ROSEN_DRAWING
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
#else
const Drawing::Path& RSPath::GetDrawingPath() const
{
    return *drPath_;
}

void RSPath::SetDrawingPath(const Drawing::Path& path)
{
    if (drPath_) {
        delete drPath_;
    }
    drPath_ = new Drawing::Path(path);
}
#endif

std::shared_ptr<RSPath> RSPath::Reverse()
{
#ifndef USE_ROSEN_DRAWING
    SkPath path;
    path.reverseAddPath(*skPath_);
#else
    Drawing::Path path;
    path.ReverseAddPath(*drPath_);
#endif
    return CreateRSPath(path);
}

float RSPath::GetDistance() const
{
#ifndef USE_ROSEN_DRAWING
    SkPathMeasure pathMeasure(*skPath_, false);
    return pathMeasure.getLength();
#else
    return drPath_->GetLength(false);
#endif
}

template<>
bool RSPath::GetPosTan(float distance, Vector2f& pos, float& degrees) const
{
#ifndef USE_ROSEN_DRAWING
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
#else
    Drawing::Point position;
    Drawing::Point tangent;
    bool ret = drPath_->GetPositionAndTangent(distance, position, tangent, false);
    if (!ret) {
        ROSEN_LOGE("PathMeasure get failed");
        return false;
    }
    pos.data_[0] = position.GetX();
    pos.data_[1] = position.GetY();
    degrees = Drawing::ConvertRadiansToDegrees(std::atan2(tangent.GetY(), tangent.GetX()));
#endif
    return true;
}

template<>
bool RSPath::GetPosTan(float distance, Vector4f& pos, float& degrees) const
{
    Vector2f position;
    bool res = GetPosTan(distance, position, degrees);
    if (!res) {
#ifndef USE_ROSEN_DRAWING
        ROSEN_LOGD("SkPathMeasure get failed");
#else
        ROSEN_LOGD("PathMeasure get failed");
#endif
        return false;
    }
    pos[0] = position[0];
    pos[1] = position[1];
    return res;
}
} // namespace Rosen
} // namespace OHOS
