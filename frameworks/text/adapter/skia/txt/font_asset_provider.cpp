/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.. All rights reserved.
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

#include "txt/font_asset_provider.h"

#include <algorithm>
#include <string>

namespace txt {
std::string FontAssetProvider::CanonicalFamilyName(std::string familyName)
{
    std::string result(familyName.length(), 0);
    std::transform(familyName.begin(), familyName.end(), result.begin(),
        [](char c) { return (c & 0x80) ? c : ::tolower(c); });
    return result;
}
} // namespace txt