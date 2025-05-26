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

#include "drawing_text_global.h"
#include "utils/text_log.h"
#include "text_global_config.h"

using namespace OHOS::Rosen::SrvText;

void OH_Drawing_SetTextHighContrast(OH_Drawing_TextHighContrast action)
{
    auto result = TextGlobalConfig::SetTextHighContrast(static_cast<uint32_t>(action));
    if (result != TEXT_SUCCESS) {
        TEXT_LOGE("Failed to set text high contrast");
    }
}


void OH_Drawing_SetTextNoGlyphShow(OH_Drawing_TextNoGlyphShow noGlyphShow)
{
    auto result = TextGlobalConfig::SetTextNoGlyphShow(static_cast<uint32_t>(noGlyphShow));
    if (result != TEXT_SUCCESS) {
        TEXT_LOGE("Failed to set text no glyph show");
    }
}