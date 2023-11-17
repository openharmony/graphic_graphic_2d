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

#include "texgine/system_font_provider.h"

#include <mutex>

#include "texgine_exception.h"
#include "texgine/utils/exlog.h"
#include "variant_font_style_set.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
std::shared_ptr<SystemFontProvider> SystemFontProvider::GetInstance() noexcept(true)
{
    if (sfp == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (sfp == nullptr) {
            sfp.reset(new SystemFontProvider());
        }
    }
    return sfp;
}

std::shared_ptr<VariantFontStyleSet> SystemFontProvider::MatchFamily(const std::string &familyName) noexcept(true)
{
    auto fontMgr = TexgineFontManager::RefDefault();
    if (fontMgr == nullptr || fontMgr->GetFontMgr() == nullptr) {
        LOGEX_FUNC_LINE(ERROR) << "fontMgr is nullptr!";
        return nullptr;
    }

    auto fontStyleSet = fontMgr->MatchFamily(familyName.c_str());
    if (fontStyleSet == nullptr || fontStyleSet->GetFontStyleSet() == nullptr) {
        LOGEX_FUNC_LINE(ERROR) << "fontStyleSet is nullptr!";
    }

    return std::make_shared<VariantFontStyleSet>(fontStyleSet);
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
