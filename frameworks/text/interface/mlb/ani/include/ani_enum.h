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

#ifndef OHOS_TEXT_ANI_ENUM_H
#define OHOS_TEXT_ANI_ENUM_H

#include <string_view>

namespace OHOS::Text::ANI {
uint32_t getEnumValue(std::string_view enumName, size_t enumIndex);
size_t getEnumIndex(std::string_view enumName, uint32_t enumValue);
} // namespace OHOS::Text::ANI
#endif // OHOS_TEXT_ANI_ENUM_H