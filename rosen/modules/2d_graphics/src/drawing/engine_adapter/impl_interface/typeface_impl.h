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

#ifndef TYPEFACE_IMPL_H
#define TYPEFACE_IMPL_H

#include <cstdint>
#include <string>

#include "impl_interface/base_impl.h"
#include "text/font_style.h"
#include "utils/data.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class TypefaceImpl : public BaseImpl {
public:
    ~TypefaceImpl() override = default;
    virtual std::string GetFamilyName() const = 0;
    virtual FontStyle GetFontStyle() const = 0;
    virtual size_t GetTableSize(uint32_t tag) const = 0;
    virtual size_t GetTableData(uint32_t tag, size_t offset, size_t length, void* data) const = 0;
    virtual bool GetItalic() const = 0;
    virtual uint32_t GetUniqueID() const = 0;
    virtual int32_t GetUnitsPerEm() const = 0;
    virtual std::shared_ptr<Data> Serialize() const = 0;

protected:
    TypefaceImpl() noexcept = default;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif