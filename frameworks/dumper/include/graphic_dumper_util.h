/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FRAMEWORKS_DUMPER_INCLUDE_GRAPHIC_DUMPER_UTIL_H
#define FRAMEWORKS_DUMPER_INCLUDE_GRAPHIC_DUMPER_UTIL_H

#include <string>
#include <vector>

namespace OHOS {
constexpr int32_t CFGTERMSIZE = 2;

std::vector<std::string> Split(std::string src, const std::string &splitStr);
} // namespace OHOS

#endif // FRAMEWORKS_DUMPER_INCLUDE_GRAPHIC_DUMPER_UTIL_H
