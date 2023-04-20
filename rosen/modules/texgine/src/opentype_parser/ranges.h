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

#ifndef ROSEN_MODULES_TEXGINE_SRC_OPENTYPE_PARSER_RANGES_H
#define ROSEN_MODULES_TEXGINE_SRC_OPENTYPE_PARSER_RANGES_H

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class Ranges {
public:
    struct Range {
        uint32_t start;
        uint32_t end;
        int32_t gid;
    };
    static constexpr int32_t InvalidGlyphId = -1;

    /*
     * @brief Add a new range
     */
    void AddRange(const struct Range &range);

    /*
     * @brief Return the glyph id of the codepoint in ranges_
     */
    int32_t GetGlyphId(uint32_t codepoint) const;

    /*
     * @brief Print the dump info
     */
    void Dump() const;

private:
    friend void ReportMemoryUsage(const std::string &member, const Ranges &that, bool needThis);

    std::vector<struct Range> ranges_;
    std::map<uint32_t, int32_t> singles_;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_OPENTYPE_PARSER_RANGES_H
