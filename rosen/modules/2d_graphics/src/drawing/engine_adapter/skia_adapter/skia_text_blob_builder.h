/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef SKIA_TEXT_BLOB_BUILDER_H
#define SKIA_TEXT_BLOB_BUILDER_H

#include <memory>

#include "include/core/SkTextBlob.h"

#include "impl_interface/text_blob_builder_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaTextBlobBuilder : public TextBlobBuilderImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;

    SkiaTextBlobBuilder() = default;
    SkiaTextBlobBuilder(const SkiaTextBlobBuilder& other) = delete;
    SkiaTextBlobBuilder& operator=(const SkiaTextBlobBuilder& other) = delete;
    ~SkiaTextBlobBuilder() override = default;

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    std::shared_ptr<TextBlob> Make() override;

    const TextBlobBuilder::RunBuffer& AllocRunPos(const Font& font, int count, const Rect* bounds) override;

    const TextBlobBuilder::RunBuffer& AllocRunRSXform(const Font& font, int count) override;

private:
    SkTextBlobBuilder skTextBlobBuilder_;
    TextBlobBuilder::RunBuffer drawingRunBuffer_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif