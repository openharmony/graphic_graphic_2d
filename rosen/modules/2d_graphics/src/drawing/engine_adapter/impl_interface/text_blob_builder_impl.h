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

#ifndef TEXT_BLOB_BUILDER_IMPL_H
#define TEXT_BLOB_BUILDER_IMPL_H

#include "impl_interface/base_impl.h"
#include "text/font.h"
#include "text/text_blob.h"
#include "text/text_blob_builder.h"
#include "utils/rect.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class TextBlobBuilderImpl : public BaseImpl {
public:
    TextBlobBuilderImpl(const TextBlobBuilderImpl& other) = delete;
    TextBlobBuilderImpl& operator=(const TextBlobBuilderImpl& other) = delete;
    ~TextBlobBuilderImpl() override = default;

    virtual std::shared_ptr<TextBlob> Make() = 0;

    virtual const TextBlobBuilder::RunBuffer& AllocRunPos(const Font& font, int count,
        const Rect* bounds) = 0;

    virtual const TextBlobBuilder::RunBuffer& AllocRunRSXform(const Font& font, int count) = 0;

protected:
    TextBlobBuilderImpl() noexcept = default;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif