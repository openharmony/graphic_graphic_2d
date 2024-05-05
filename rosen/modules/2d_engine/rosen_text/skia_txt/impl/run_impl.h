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

#ifndef ROSEN_MODULES_SPTEXT_RUN_IMPL_H
#define ROSEN_MODULES_SPTEXT_RUN_IMPL_H

#include "modules/skparagraph/include/RunBase.h"
#include "txt/paint_record.h"
#include "txt/run.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
class RunImpl : public Run {
public:
    RunImpl(std::unique_ptr<skia::textlayout::RunBase> runBase, const std::vector<PaintRecord>& paints);

    virtual ~RunImpl() = default;

    Drawing::Font GetFont() const override;
    size_t GetGlyphCount() const override;
    std::vector<uint16_t> GetGlyphs() const override;
    std::vector<Drawing::Point> GetPositions() override;
    std::vector<Drawing::Point> GetOffsets() override;
    void Paint(Drawing::Canvas* canvas, double x, double y) override;

private:
    std::unique_ptr<skia::textlayout::RunBase> runBase_;
    const std::vector<PaintRecord>& paints_;
};
} // namespace SPText
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_SPTEXT_RUN_IMPL_H
