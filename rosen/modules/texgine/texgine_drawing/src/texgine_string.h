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

#ifndef USE_ROSEN_DRAWING
#include <include/core/SkString.h>
#else
#include <string>
#endif

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexgineString {
public:
    /*
     * @brief Returns the pointer of SkString
     */
#ifndef USE_ROSEN_DRAWING
    SkString *GetString() const;
#else
    std::string *GetString();
#endif

    /*
     * @brief Sets SkString to TexgineString
     * @param string SkString
     */
#ifndef USE_ROSEN_DRAWING
    void SetString(const std::shared_ptr<SkString> string);
#else
    void SetString(const std::shared_ptr<std::string> string);
#endif

    /*
     * @brief Covert SkString to String
     */
    std::string ToString() const;

private:
#ifndef USE_ROSEN_DRAWING
    std::shared_ptr<SkString> string_ = std::make_shared<SkString>();
#else
    std::shared_ptr<std::string> string_ = std::make_shared<std::string>();
#endif
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_STRING_H
