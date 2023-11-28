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

#ifndef FONT_STYLE_SET_IMPL_H
#define FONT_STYLE_SET_IMPL_H

#include <string>

#include "impl_interface/base_impl.h"
#include "text/font_style.h"
#include "text/typeface.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class FontStyleSetImpl : public BaseImpl {
public:
    ~FontStyleSetImpl() override = default;

    virtual Typeface* CreateTypeface(int index) = 0;

    virtual void GetStyle(int32_t index, FontStyle* fontStyle, std::string* styleName) = 0;

    virtual Typeface* MatchStyle(const FontStyle& pattern) = 0;

    virtual int Count() = 0;

protected:
    FontStyleSetImpl() noexcept = default;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif