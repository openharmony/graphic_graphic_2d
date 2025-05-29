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

#ifndef ROSEN_TEXT_LINE_TYPOGRAPHY_H
#define ROSEN_TEXT_LINE_TYPOGRAPHY_H

#include "typography.h"

namespace OHOS {
namespace Rosen {
class LineTypography {
public:
    virtual ~LineTypography() = default;
    virtual size_t GetLineBreak(size_t startIndex, double width) const = 0;
    virtual std::unique_ptr<TextLineBase> CreateLine(size_t startIndex, size_t count) = 0;
    virtual std::unique_ptr<Typography> GetTempTypography() = 0;
    virtual void* GetLineFetcher() const = 0;
    virtual size_t GetUnicodeSize() const = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_TEXT_LINE_TYPOGRAPHY_H