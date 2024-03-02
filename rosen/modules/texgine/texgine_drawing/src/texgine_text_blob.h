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

#ifndef ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_TEXT_BLOB_H
#define ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_TEXT_BLOB_H

#include <memory>

#include "drawing.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexgineTextBlob {
public:
    /*
     * @brief Return the pointer of SkTextBlob to prepare the paint info
     */
    std::shared_ptr<RSTextBlob> GetTextBlob() const;

    /*
     * @brief Sets SkTextBlob to TexgineTextBlob
     */
    void SetTextBlob(const std::shared_ptr<RSTextBlob> textBlob);

    void GetGlyphIDs(std::vector<uint16_t>& glyphIds);

    RSPath GetPathbyGlyphID(const uint16_t& glyphId);

private:
    std::shared_ptr<RSTextBlob> textBlob_ = nullptr;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_TEXT_BLOB_H
