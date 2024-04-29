/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#ifndef TEXT_BLOB_IMPL_H
#define TEXT_BLOB_IMPL_H

#include "impl_interface/base_impl.h"
#include "utils/data.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class TextBlobImpl : public BaseImpl {
public:
    ~TextBlobImpl() override = default;

    virtual std::shared_ptr<Data> Serialize(void* ctx) const = 0;
    virtual std::shared_ptr<Rect> Bounds() const = 0;
    virtual uint32_t UniqueID() const = 0;

protected:
    TextBlobImpl() noexcept = default;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif