/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "text_global_config.h"

#include <atomic>

#include "modules/skparagraph/include/TextGlobalConfig.h"
#include "rosen_text/text_config.h"
#include "text/text_blob.h"
#include "utils/text_log.h"

namespace OHOS::Rosen::SrvText {

using namespace OHOS::Rosen::Drawing;

uint32_t TextGlobalConfig::SetTextHighContrast(uint32_t textHighContrast)
{
    if (textHighContrast >= TEXT_HIGH_CONTRAST_BUTT) {
        return TEXT_ERR_PARA_INVALID;
    }
    auto& instance = ProcessTextConstrast::Instance();
    instance.SetTextContrast(static_cast<TextContrast>(textHighContrast));
    TEXT_LOGI("Succeed in setting text contrast %{public}u", textHighContrast);
    return TEXT_SUCCESS;
}

uint32_t TextGlobalConfig::SetTextUndefinedGlyphDisplay(uint32_t textUndefinedGlyphDisplay)
{
    if (textUndefinedGlyphDisplay >= UNDEFINED_GLYPH_BUTT) {
        return TEXT_ERR_PARA_INVALID;
    }
    skia::textlayout::TextGlobalConfig::SetUndefinedGlyphDisplay(textUndefinedGlyphDisplay);
    return TEXT_SUCCESS;
}

} // namespace OHOS::Rosen::SrvText

// Exported API for external modules
namespace OHOS {
namespace Rosen {

namespace {
    static std::atomic<bool> g_localeTextBreakEnabled{false};
}

void TextConfig::SetLocaleTextBreakEnabled(bool enabled)
{
    g_localeTextBreakEnabled.store(enabled, std::memory_order_relaxed);
    TEXT_LOGI("Locale text break set to: %{public}s", enabled ? "enabled" : "disabled");
}

bool TextConfig::IsLocaleTextBreakEnabled()
{
    return g_localeTextBreakEnabled.load(std::memory_order_relaxed);
}

} // namespace Rosen
} // namespace OHOS