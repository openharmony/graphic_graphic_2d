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

#ifndef FONT_STYLE_SET_H
#define FONT_STYLE_SET_H

#include <memory>

#include "text/typeface.h"

class FontConfig_OHOS;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class FontStyleSetImpl;

class FontStyleSet {
public:
    FontStyleSet();
    FontStyleSet(const std::shared_ptr<FontConfig_OHOS>& fontConfig, int index, bool isFallback = false);
    explicit FontStyleSet(std::shared_ptr<FontStyleSetImpl> fontStyleSetImpl) noexcept;
    virtual ~FontStyleSet() = default;

    /*
     * @brief        Create a typeface for the given index.
     * @param index  The index of the typeface in this fontStyleSet.
     * @return       If successful, return typeface. else, return nullptr.
     */
    Typeface* CreateTypeface(int index);

    /*
     * @brief   Get the count of typeface.
     * @return  The count of typeface in this font style set.
     */
    int Count();

    /*
     * @brief           Register typeface to the fontStyleSet.
     * @param typeface  A shared point to typeface.
     */
    void RegisterTypeface(std::shared_ptr<Typeface> typeface);

private:
    std::shared_ptr<FontStyleSetImpl> fontStyleSetImpl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif