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

#ifndef TEST_CONVERT_FUZZER_H
#define TEST_CONVERT_FUZZER_H

#include <climits>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "font_collection.h"
#include "paragraph_builder.h"
#include "paragraph_style.h"
#include "run_impl.h"
#include "typography.h"

#include "utils/string_util.h"

#define FUZZ_PROJECT_NAME "run_fuzzer"
constexpr inline size_t DATA_MAX_ENUM_SIZE1 = 3;
constexpr inline size_t DATA_MAX_ENUM_SIZE2 = 4;
constexpr inline size_t DATA_MAX_ENUM_SIZE3 = 7;

#endif // TEST_CONVERT_FUZZER_H