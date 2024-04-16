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

#ifndef ROSEN_TEXT_ADAPTER_TXT_RUN_IMPL_H
#define ROSEN_TEXT_ADAPTER_TXT_RUN_IMPL_H

#include "rosen_text/run.h"
#include "txt/run.h"

namespace OHOS {
namespace Rosen {
namespace AdapterTxt {
class RunImpl : public ::OHOS::Rosen::Run {
public:
    explicit RunImpl(std::unique_ptr<SPText::Run> run);

    Drawing::Font GetFont() const override;
    size_t GetGlyphCount() const override;
    std::vector<uint16_t> GetGlyphs() const override;
    std::vector<Drawing::Point> GetPositions() override;
    std::vector<Drawing::Point> GetOffsets() override;
    void Paint(Drawing::Canvas *canvas, double x, double y) override;

private:
    std::unique_ptr<SPText::Run> run_ = nullptr;
    Drawing::Font font_;
};
} // namespace AdapterTxt
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_TEXT_ADAPTER_TXT_RUN_IMPL_H
