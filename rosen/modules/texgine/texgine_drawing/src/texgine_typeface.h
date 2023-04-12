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

#ifndef ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_TYPEFACE_H
#define ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_TYPEFACE_H

#include <memory>

#include <include/core/SkTypeface.h>

#include "texgine_font_style.h"
#include "texgine_stream.h"
#include "texgine_string.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexgineTypeface {
public:
    TexgineTypeface();
    TexgineTypeface(SkTypeface *tf);
    TexgineTypeface(sk_sp<SkTypeface> typeface);
    TexgineTypeface(void *context);
    sk_sp<SkTypeface> GetTypeface();
    size_t GetTableSize(uint32_t tag);
    size_t GetTableData(uint32_t tag, size_t offset, size_t length, void *data);
    int GetUnitsPerEm();
    void GetFamilyName(TexgineString *name);
    std::shared_ptr<TexgineFontStyle> FontStyle();

    static std::shared_ptr<TexgineTypeface> MakeFromStream(std::unique_ptr<TexgineMemoryStream> stream, int index = 0);
    static std::shared_ptr<TexgineTypeface> MakeFromFile(const char path[], int index = 0);

private:
    sk_sp<SkTypeface> typeface_ = nullptr;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_TYPEFACE_H
