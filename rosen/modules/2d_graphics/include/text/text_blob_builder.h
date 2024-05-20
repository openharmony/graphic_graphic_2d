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

#ifndef TEXT_BLOB_BUILDER_H
#define TEXT_BLOB_BUILDER_H

#include <memory>
#include <cstdint>

#include "text/font.h"
#include "text/text_blob.h"
#include "utils/point.h"
#include "utils/rect.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class TextBlobBuilderImpl;

class DRAWING_API TextBlobBuilder {
public:
    TextBlobBuilder();
    TextBlobBuilder(const TextBlobBuilder& other) = delete;
    TextBlobBuilder& operator=(const TextBlobBuilder& other) = delete;
    virtual ~TextBlobBuilder() = default;

    struct RunBuffer {
        uint16_t*  glyphs;
        scalar*    pos;
        char*      utf8text;
        uint32_t*  clusters;
    };

    /**
     * @brief   Make textBlob by textBlobBuilder.
     * @return  A shared point to textBlob.
     */
    std::shared_ptr<TextBlob> Make();

    /**
     * @brief         Alloc run with storage for glyphs and point positions.
     * @param font    Font used for this run
     * @param count   Number of glyphs
     * @param bounds  Optional run bounding box
     * @return        Run with storage for glyphs and point positions.
     */
    const RunBuffer& AllocRunPos(const Font& font, int count,
                                 const Rect* bounds = nullptr);

    /**
     * @brief         Alloc run with storage for glyphs and affine matrix.
     * @param font    Font used for this run
     * @param count   Number of glyphs
     * @return        Run with storage for glyphs and affine matrix.
     */
    const RunBuffer& AllocRunRSXform(const Font& font, int count);

private:
    std::shared_ptr<TextBlobBuilderImpl> textBlobBuilderImpl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif