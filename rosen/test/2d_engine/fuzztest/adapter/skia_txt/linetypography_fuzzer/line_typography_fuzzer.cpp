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

#include "line_typography_fuzzer.h"
#include <cstddef>
#include "get_object.h"
#include "line_typography.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
void OHLineTypographyFuzz1(const uint8_t* data, size_t size)
{
    size_t startIndex = GetObject<size_t>();
    double width = GetObject<double>();
    size_t count = GetObject<size_t>();
    std::unique_ptr<SPText::ParagraphLineFetcher> lineFetcher_;
    AdapterTxt::LineTypography lineTypography(std::move(lineFetcher_));
    lineTypography.GetLineBreak(startIndex, width);
    lineTypography.CreateLine(startIndex, count);
    lineTypography.GetTempTypography();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::OHLineTypographyFuzz1(data, size);
    return 0;
}
