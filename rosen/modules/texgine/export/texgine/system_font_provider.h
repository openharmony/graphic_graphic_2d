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

#ifndef ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_SYSTEM_FONT_PROVIDER_H
#define ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_SYSTEM_FONT_PROVIDER_H

#include "texgine/ifont_provider.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
/*
 * @brief SystemFontProvider can provide fonts registered in system.
 */
class SystemFontProvider : public IFontProvider {
public:
    /*
     * @brief Returns single instance.
     */
    static std::shared_ptr<SystemFontProvider> GetInstance() noexcept(true);

    std::shared_ptr<VariantFontStyleSet> MatchFamily(const std::string& familyName) noexcept(true) override;

private:
    void ReportMemoryUsage(const std::string& member, const bool needThis) const override;

    static inline std::shared_ptr<SystemFontProvider> sfp = nullptr;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_SYSTEM_FONT_PROVIDER_H
