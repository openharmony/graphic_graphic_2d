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

#ifndef TEXT_2D_GLOBAL_CONFIG_H
#define TEXT_2D_GLOBAL_CONFIG_H

#include <cstdint>

namespace OHOS::Rosen::SrvText {

enum TextHighContrast {
    /* Follow system's high contrast setting for text rendering */
    TEXT_FOLLOW_SYSTEM_HIGH_CONTRAST,
    /* Disable high contrast rendering regardless of system settings */
    TEXT_APP_DISABLE_HIGH_CONTRAST,
    /* Enable high contrast rendering regardless of system settings */
    TEXT_APP_ENABLE_HIGH_CONTRAST,
    /* Invalid value */
    TEXT_HIGH_CONTRAST_BUTT
};

enum TextError {
    TEXT_SUCCESS = 0,
    TEXT_ERR_PARA_INVALID,
    TEXT_ERR_BUTT
};

enum TextNoGlyphShow {
    NO_GLYPH_USE_DEFAULT,
    NO_GLYPH_USE_TOFU,
    NO_GLYPH_BUTT,
};

class TextGlobalConfig final {
public:
    ~TextGlobalConfig() = default;

    static uint32_t SetTextHighContrast(uint32_t textHighContrast);
    static uint32_t SetTextNoGlyphShow(uint32_t textNoGlyphShow);
private:
    TextGlobalConfig() = default;
};

}

#endif