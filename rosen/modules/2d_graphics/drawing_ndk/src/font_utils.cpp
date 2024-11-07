/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "font_utils.h"

#include "drawing_text_font_descriptor.h"

namespace OHOS::Rosen::Drawing {
bool CopyFontDescriptor(OH_Drawing_FontDescriptor* dst, const FontParser::FontDescriptor& src)
{
    if (dst == nullptr) {
        return false;
    }
    dst->path = strdup(src.path.c_str());
    dst->postScriptName = strdup(src.postScriptName.c_str());
    dst->fullName = strdup(src.fullName.c_str());
    dst->fontFamily = strdup(src.fontFamily.c_str());
    dst->fontSubfamily = strdup(src.fontSubfamily.c_str());
    if (dst->path == NULL || dst->postScriptName == NULL || dst->fullName == NULL || dst->fontFamily == NULL ||
        dst->fontSubfamily == NULL) {
        free(dst->path);
        free(dst->postScriptName);
        free(dst->fullName);
        free(dst->fontFamily);
        free(dst->fontSubfamily);
        dst->path = nullptr;
        dst->postScriptName = nullptr;
        dst->fullName = nullptr;
        dst->fontFamily = nullptr;
        dst->fontSubfamily = nullptr;
        return false;
    }
    dst->weight = src.weight;
    dst->width = src.width;
    dst->italic = src.italic;
    dst->monoSpace = src.monoSpace;
    dst->symbolic = src.symbolic;
    return true;
}
} // namespace OHOS::Rosen::Drawing