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

#ifndef ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_DYNAMIC_FILE_FONT_PROVIDER_H
#define ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_DYNAMIC_FILE_FONT_PROVIDER_H

#include <map>

#include "texgine/dynamic_font_provider.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
/*
 * @brief FileFontProvider is a FontProvider that can load fonts dynamically.
 *        FileFontProvider can accept font from file path.
 */
class DynamicFileFontProvider : DynamicFontProvider, virtual public IFontProvider {
public:
    /*
     * @brief Create DynamicFileFontProvider instance.
     */
    static std::shared_ptr<DynamicFileFontProvider> Create() noexcept(true);

    /*
     * @brief Load font from path.
     * @param path        Path to the font file.
     * @param familyName  The name of the font family you want.
     * @return            0 if success else 1 means failed
     */
    int LoadFont(const std::string& familyName, const std::string& path) noexcept(true);

private:
    std::map<std::string, std::shared_ptr<VariantFontStyleSet>> fontStyleSetMap_;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_DYNAMIC_FILE_FONT_PROVIDER_H
