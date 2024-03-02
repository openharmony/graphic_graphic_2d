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

#include "texgine_string.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
std::string *TexgineString::GetString()
{
    return string_.get();
}

void TexgineString::SetString(const std::shared_ptr<std::string> string)
{
    string_ = string;
}

std::string TexgineString::ToString() const
{
    if (string_ == nullptr) {
        return "";
    }
    return string_->c_str();
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
