/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#ifndef FONT_UNICODE_QUERY_H
#define FONT_UNICODE_QUERY_H

#include <string>

#include "typeface.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class FontUnicodeQuery {
public:
    /**
     * @brief            Returns all supported and valid unicode item in given typeface.
     * @param typeface   given typeface.
     * @return           vector of all supported and valid unicode item in given typeface
     */
    static std::vector<uint32_t> GenerateUnicodeItem(const std::shared_ptr<Typeface>& typeface);
};
} // Drawing
} // Rosen
} // OHOS
#endif // FONT_UNICODE_QUERY_H