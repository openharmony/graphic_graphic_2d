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

#ifndef USE_ROSEN_DRAWING
#include <include/core/SkTypeface.h>
#include <include/core/SkFontMgr.h>
#else
#include "drawing.h"
#endif

#include "texgine_font_style.h"
#include "texgine_stream.h"
#include "texgine_string.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexgineTypeface {
public:
    TexgineTypeface();
    explicit TexgineTypeface(void *context);
#ifndef USE_ROSEN_DRAWING
    sk_sp<SkTypeface> GetTypeface() const;
    explicit TexgineTypeface(sk_sp<SkTypeface> typeface);
#else
    std::shared_ptr<RSTypeface> GetTypeface() const;
    explicit TexgineTypeface(std::shared_ptr<RSTypeface>  typeface);
#endif

    /*
     * @brief Return the table contents that accroding table tag
     * @param tag The table tag
     */
    size_t GetTableSize(uint32_t tag) const;

    /*
     * @brief Copy the contents of a table into data
     * @param tag The table tag whose contents are to be copied
     * @param offset The offset (in bytes) of the table content, from which the copy should start
     * @param length The number of bytes of table data to be copied, starting from offset
     * @param data The table data copied to this address
     * @return the number of bytes actually copied into data
     */
    size_t GetTableData(uint32_t tag, size_t offset, size_t length, void *data) const;

    /*
     * @brief Returns the unit/em value of this font, and returns zero if there are errors
     */
    int GetUnitsPerEm() const;

    /*
     * @brief Return the family name of this typeface
     */
    void GetFamilyName(TexgineString *name) const;

    /*
     * @brief Return the typeface`s font style
     */
    std::shared_ptr<TexgineFontStyle> GetFontStyle() const;

    size_t FontStyleDetection();
    /*
     * @brief Create a typeface accroding to the stream
     */
    static std::shared_ptr<TexgineTypeface> MakeFromStream(std::unique_ptr<TexgineMemoryStream> stream, int index = 0);

    /*
     * @brief Create a typeface with the file
     * @path The typeface file path
     * @index The ttc index, default is 0
     */
    static std::shared_ptr<TexgineTypeface> MakeFromFile(const std::string &path, int index = 0);

    bool DetectRawInformation() const;

    void InputOriginalStyle(bool primitivism);

private:
    bool rawInformation_ = false;
#ifndef USE_ROSEN_DRAWING
    sk_sp<SkTypeface> typeface_ = nullptr;
#else
    std::shared_ptr<RSTypeface> typeface_ = nullptr;
#endif
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_TYPEFACE_H
