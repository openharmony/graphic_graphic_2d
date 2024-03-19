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

#include "draw/path.h"
#include "utils/matrix.h"
#include "utils/scalar.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSPath> RSPath::CreateRSPath()
{
    return std::make_shared<RSPath>();
}

std::shared_ptr<RSPath> RSPath::CreateRSPath(const Drawing::Path& path)
{
    auto rsPath = std::make_shared<RSPath>();
    rsPath->SetDrawingPath(path);
    return rsPath;
}

std::shared_ptr<RSPath> RSPath::CreateRSPath(const std::string& path)
{
    Drawing::Path drAnimationPath;
    drAnimationPath.BuildFromSVGString(path);
    return RSPath::CreateRSPath(drAnimationPath);
}

RSPath::RSPath()
{
    drPath_ = new Drawing::Path();
}

RSPath::~RSPath()
{
    if (drPath_) {
        delete drPath_;
    }
}

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

std::shared_ptr<RSPath> RSPath::Reverse()
{
    Drawing::Path path;
    path.ReverseAddPath(*drPath_);
    return CreateRSPath(path);
}

float RSPath::GetDistance() const
{
    return drPath_->GetLength(false);
}

template<>
bool RSPath::GetPosTan(float distance, Vector2f& pos, float& degrees) const
{
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
    return true;
}

template<>
bool RSPath::GetPosTan(float distance, Vector4f& pos, float& degrees) const
{
    Vector2f position;
    bool res = GetPosTan(distance, position, degrees);
    if (!res) {
        ROSEN_LOGD("PathMeasure get failed");
        return false;
    }
    pos[0] = position[0];
    pos[1] = position[1];
    return res;
}
} // namespace Rosen
} // namespace OHOS
