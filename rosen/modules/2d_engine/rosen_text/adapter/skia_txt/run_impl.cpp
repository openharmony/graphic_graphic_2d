/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "run_impl.h"
#include "impl/run_impl.h"
#include "convert.h"

namespace OHOS {
namespace Rosen {
namespace AdapterTxt {
RunImpl::RunImpl(std::unique_ptr<SPText::Run> run): run_(std::move(run))
{
}

Drawing::Font RunImpl::GetFont() const
{
    if (!run_) {
        return {};
    }
    return run_->GetFont();
}

size_t RunImpl::GetGlyphCount() const
{
    if (!run_) {
        return 0;
    }
    return run_->GetGlyphCount();
}

std::vector<uint16_t> RunImpl::GetGlyphs() const
{
    if (!run_) {
        return {};
    }
    return run_->GetGlyphs();
}

std::vector<Drawing::Point> RunImpl::GetPositions()
{
    if (!run_) {
        return {};
    }
    return run_->GetPositions();
}

std::vector<Drawing::Point> RunImpl::GetOffsets()
{
    if (!run_) {
        return {};
    }
    return run_->GetOffsets();
}

void RunImpl::Paint(Drawing::Canvas *canvas, double x, double y)
{
    if (!run_) {
        return;
    }
    return run_->Paint(canvas, x, y);
}
} // namespace AdapterTxt
} // namespace Rosen
} // namespace OHOS
