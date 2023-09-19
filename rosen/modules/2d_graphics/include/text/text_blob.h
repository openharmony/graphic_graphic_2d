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

#ifndef TEXT_BLOB_H
#define TEXT_BLOB_H

#include <memory>

#include "impl_interface/text_blob_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class TextBlob {
public:
    explicit TextBlob(std::shared_ptr<TextBlobImpl> textBlobImpl) noexcept;
    virtual ~TextBlob() = default;

    template<typename T>
    const std::shared_ptr<T> GetImpl() const
    {
        if (textBlobImpl_) {
            return textBlobImpl_->DowncastingTo<T>();
        }
        return nullptr;
    }

private:
    std::shared_ptr<TextBlobImpl> textBlobImpl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif