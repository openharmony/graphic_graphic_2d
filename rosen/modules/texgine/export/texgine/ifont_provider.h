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

#ifndef ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_IFONT_PROVIDER_H
#define ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_IFONT_PROVIDER_H

#include <memory>
#include <string>

#include "texgine/utils/memory_object.h"
#include "texgine_font_manager.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class VariantFontStyleSet;

/*
 * @brief IFontProvider is abstraction for a source of fonts. Subclasses can be backed by
 *        a memory font, or a font directory, or a font file, or network fonts manager,
 *        or something else.
 */
class IFontProvider : public MemoryObject {
public:
    virtual ~IFontProvider() = default;

    /*
     * @brief Returns FontStyleSet when familyName can correspond to a FontStyleSet.
     *        This may return nullptr if there is no FontStyleSet correspond to familyName.
     */
    virtual std::shared_ptr<VariantFontStyleSet> MatchFamily(const std::string& familyName) noexcept(true) = 0;

protected:
    IFontProvider() = default;

private:
    friend void ReportMemoryUsage(const std::string& member, const IFontProvider& that, const bool needThis);
    void ReportMemoryUsage(const std::string& member, const bool needThis) const override;

    IFontProvider(const IFontProvider&) = delete;
    IFontProvider(IFontProvider&&) = delete;
    IFontProvider& operator=(const IFontProvider&) = delete;
    IFontProvider& operator=(const IFontProvider&&) = delete;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_IFONT_PROVIDER_H
