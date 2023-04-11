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

#ifndef ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_DYNAMIC_FONT_PROVIDER_H
#define ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_DYNAMIC_FONT_PROVIDER_H

#include <map>

#include "texgine/ifont_provider.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
/*
 * @brief DynamicFontProvider is a FontProvider that can load fonts dynamically.
 *        DynamicFontProvider can accept font typed memory buffer.
 */
class DynamicFontProvider : virtual public IFontProvider {
public:
    /*
     * @brief Create DynamicFontProvider instance.
     * @return DynamicFontProvider
     */
    static std::shared_ptr<DynamicFontProvider> Create() noexcept(true);

    /*
     * @brief Load font from data, if familyName is empty, it will use the name that comes
     *        with the font file.
     * @param familyName  The name of the font family you want,
     *                    empty String by default means use the name that
     *                    comes with the font file.
     * @param data        The font data pointer.
     * @param datalen     The font data length.
     * @return            0 if success
     *                    1 if the arguments are invalid
     *                    2 if the call to the internal API fails
     */
    int LoadFont(const std::string& familyName, const void *data, size_t datalen) noexcept(true);

    std::shared_ptr<VariantFontStyleSet> MatchFamily(const std::string& familyName) noexcept(true) override;

private:
    void ReportMemoryUsage(const std::string& member, const bool needThis) const override;

    std::map<std::string, std::shared_ptr<VariantFontStyleSet>> fontStyleSetMap_;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_DYNAMIC_FONT_PROVIDER_H
