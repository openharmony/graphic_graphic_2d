/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef C_INCLUDE_DRAWING_TEXT_CONVERTOR_H
#define C_INCLUDE_DRAWING_TEXT_CONVERTOR_H

#include "drawing_text_declaration.h"
#include "rosen_text/ui/font_collection.h"
#include "rosen_text/ui/typography.h"
#include "rosen_text/ui/typography_create.h"

namespace rosen {
FontCollection* ConvertToOriginalText(OH_Drawing_FontCollection* fontCollection);
OH_Drawing_FontCollection* ConvertToNDKText(FontCollection* fontCollection);

TypographyStyle* ConvertToOriginalText(OH_Drawing_TypographyStyle* style);
OH_Drawing_TypographyStyle* ConvertToNDKText(TypographyStyle* style);

TypographyCreate* ConvertToOriginalText(OH_Drawing_TypographyCreate* handler);
OH_Drawing_TypographyCreate* ConvertToNDKText(TypographyCreate* handler);

TextStyle* ConvertToOriginalText(OH_Drawing_TextStyle* style);

Typography* ConvertToOriginalText(OH_Drawing_Typography* typography);
OH_Drawing_Typography* ConvertToNDKText(Typography* typography);
}

#endif

