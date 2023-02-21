/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef SANDBOX_UTILS_H
#define SANDBOX_UTILS_H

#include <cstdio>
#include <unistd.h>

namespace OHOS {
#ifdef _WIN32
__attribute__((dllexport)) pid_t GetRealPid(void);
#else
__attribute__((visibility("default"))) pid_t GetRealPid(void);
#endif
} // namespace OHOS

#endif // SANDBOX_UTILS_H
