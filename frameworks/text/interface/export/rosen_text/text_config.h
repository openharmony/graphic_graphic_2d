/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef ROSEN_TEXT_EXPORT_ROSEN_TEXT_TEXT_CONFIG_H
#define ROSEN_TEXT_EXPORT_ROSEN_TEXT_TEXT_CONFIG_H

namespace OHOS {
namespace Rosen {

/**
 * @brief TextConfig provides global configuration for text layout
 *
 * This class allows configuring text layout behaviors such as locale-aware text breaking,
 * which enables locale-specific line breaking rules.
 */
class TextConfig {
public:
    /**
     * @brief Set whether locale-aware text breaking is enabled for text layout
     * @param enabled true to enable locale-aware text breaking, false to disable
     *
     * When enabled, text layout will consider locale-specific rules for line breaking,
     * such as allowing breaks before certain opening punctuation marks in Chinese text
     * and handling email addresses more intelligently.
     */
    static void SetLocaleTextBreakEnabled(bool enabled);

    /**
     * @brief Check if locale-aware text breaking is currently enabled
     * @return true if locale-aware text breaking is enabled, false otherwise
     */
    static bool IsLocaleTextBreakEnabled();
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_TEXT_EXPORT_ROSEN_TEXT_TEXT_CONFIG_H
