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

#ifndef FONT_FILETYPE_H
#define FONT_FILETYPE_H

#include <string>

#include "typeface.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

class DRAWING_API FontFileType {
public:
    enum class FontFileFormat {
        UNKNOWN,
        TTF,
        OTF,
        TTC,
        OTC
    };

    /**
     * @brief            Returns the file type of target typeface.
     * @param typeface   given typeface.
     * @return           file type of aimed typeface
     */
    static FontFileFormat GetFontFileType(const std::shared_ptr<Typeface>& typeface);

    /**
     * @brief            Returns the file type and face numbers of aimed file path.
     * @param path       file path.
     * @param fileCount  number of faces contained in object.
     * @return           file type of aimed typeface.
     */
    static FontFileFormat GetFontFileType(const std::string& path, int& fileCount);

    /**
     * @brief            Returns the file type and face numbers of aimed file data.
     * @param data       file data.
     * @param fileCount  number of faces contained in object.
     * @return           file type of aimed typeface.
     */
    static FontFileFormat GetFontFileType(const std::vector<uint8_t>& data, int& fileCount);
};
} // Drawing
} // Rosen
} // OHOS
#endif // FONT_FILETYPE_H