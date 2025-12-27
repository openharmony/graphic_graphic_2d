/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef OHOS_ROSEN_TEXT_FONT_TOOL_SET_H
#define OHOS_ROSEN_TEXT_FONT_TOOL_SET_H

#include <string>
#include <vector>

namespace OHOS::Rosen {

class FontToolSet final {
public:
    static FontToolSet& GetInstance();
    ~FontToolSet() = default;
    
    // This function only works in the OHOS environment.
    std::vector<std::string> GetFontFullName(int fd);
private:
    FontToolSet() = default;
    FontToolSet(const FontToolSet&) = delete;
    FontToolSet& operator=(const FontToolSet&) = delete;
    FontToolSet(FontToolSet&&) = delete;
    FontToolSet& operator=(FontToolSet&&) = delete;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_TEXT_FONT_TOOL_SET_H