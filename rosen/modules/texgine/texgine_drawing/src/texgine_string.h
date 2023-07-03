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

#ifndef ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_STRING_H
#define ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_STRING_H

#include <memory>

#include <include/core/SkString.h>

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexgineString {
public:
    /*
     * @brief Returns the pointer of SkString
     */
    SkString *GetString() const;

    /*
     * @brief Sets SkString to TexgineString
     * @param string SkString
     */
    void SetString(const std::shared_ptr<SkString> string);

    /*
     * @brief Sets string to TexgineString
     */
    void SetString(const std::string &s);

    /*
     * @brief Covert SkString to String
     */
    std::string ToString() const;

private:
    std::shared_ptr<SkString> string_ = std::make_shared<SkString>();
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_STRING_H
