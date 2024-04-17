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

#ifndef ROSEN_MODULES_SPTEXT_TEXT_LINE_BASE_H
#define ROSEN_MODULES_SPTEXT_TEXT_LINE_BASE_H

#include <stddef.h>
#include "draw/canvas.h"
#include "rosen_text/typography.h"
#include "run.h"
#include "utils.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
class TextLineBase {
public:
    virtual ~TextLineBase() = default;

    virtual size_t GetGlyphCount() const = 0;

    virtual std::vector<std::unique_ptr<Run>> GetGlyphRuns() const = 0;

    virtual Range<size_t> GetTextRange() const= 0;

    virtual void Paint(Drawing::Canvas *canvas, double x, double y) = 0;
};
} // namespace SPText
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_SPTEXT_TEXT_LINE_BASE_H