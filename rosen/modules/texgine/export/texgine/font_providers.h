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

#ifndef ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_FONT_PROVIDERS_H
#define ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_FONT_PROVIDERS_H

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "texgine/ifont_provider.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class FontCollection;

/*
 * @brief FontProviders providers an interface for developers to determine the order in
 *        which fonts are used.
 *        Texgine is provided with the internal function of generate font collection by
 *        using font families.
 *        The order in which fonts are used in generating depends on the order in which
 *        the collection is appended with FontProvider.
 */
class FontProviders {
public:
    FontProviders() = default;
    /*
     * @brief Allocates an empty FontProviders.
     */
    static std::shared_ptr<FontProviders> Create() noexcept(true);

    /*
     * @brief Allocates a FontProviders that have only one FontProvider which is SystemFontProvider.
     *        The first FontProvider in the allocated FontProviders is SystemFontProvider,
     *        add additional FontProvider can be appended if necessary.
     */
    static std::shared_ptr<FontProviders> SystemFontOnly() noexcept(true);

    /*
     * @brief Append FontProvider into FontProviders.
     * @param provider  FontProvider that's going to be appended to the FontProviders
     *                  FontProvider cannot be appended repeatedly
     */
    void AppendFontProvider(const std::shared_ptr<IFontProvider>& provider) noexcept(true);

    /*
     * @brief Disable find and use fallback font.
     */
    void DisableFallback();

    /*
     * @brief Generate font collection based on parameter font families and FontProvider that
     *        has been appended.
     * @param families  A family vector for selecting fonts from FontProviders
     * @return          FontCollection from selected fonts
     */
    std::shared_ptr<FontCollection> GenerateFontCollection(
        const std::vector<std::string>& families) const noexcept(true);

    void Clear()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        fontStyleSetCache_.clear();
    }

private:
    friend void ReportMemoryUsage(const std::string& member, const FontProviders& that, const bool needThis);

    FontProviders(const FontProviders&) = delete;
    FontProviders(FontProviders&&) = delete;
    FontProviders& operator=(const FontProviders&) = delete;
    FontProviders& operator=(FontProviders&&) = delete;

    bool enableFallback_ = true;
    std::vector<std::shared_ptr<IFontProvider>> providers_ = {};

    mutable std::map<std::string, std::shared_ptr<VariantFontStyleSet>> fontStyleSetCache_ = {};
    mutable std::mutex mutex_;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_FONT_PROVIDERS_H
