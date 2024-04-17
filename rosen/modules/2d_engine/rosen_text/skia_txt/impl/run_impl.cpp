/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.. All rights reserved.
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
#include "drawing_painter_impl.h"
#include "text/font.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
namespace skt = skia::textlayout;

RunImpl::RunImpl(std::unique_ptr<skt::RunBase> runBase, const std::vector<PaintRecord>& paints)
    : runBase_(std::move(runBase)), paints_(paints)
{}

Drawing::Font RunImpl::GetFont() const
{
    if (!runBase_) {
        return font_;
    }
    return runBase_->font();
}

size_t RunImpl::GetGlyphCount() const
{
    if (!runBase_) {
        return 0;
    }
    return runBase_->size();
}

std::vector<uint16_t> RunImpl::GetGlyphs() const
{
    if (!runBase_) {
        return {};
    }
    return runBase_->getGlyphs();
}

std::vector<Drawing::Point> RunImpl::GetPositions()
{
    if (!runBase_) {
        return {};
    }
    return runBase_->getPositions();
}

std::vector<Drawing::Point> RunImpl::GetOffsets()
{
    if (!runBase_) {
        return {};
    }
    return runBase_->getOffsets();
}

void RunImpl::Paint(Drawing::Canvas* canvas, double x, double y)
{
    if (!runBase_ || !canvas) {
        return;
    }
    RSCanvasParagraphPainter painter(canvas, paints_);
    runBase_->paint(&painter, x, y);
}

} // namespace SPText
} // namespace Rosen
} // namespace OHOS
