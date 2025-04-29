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

#ifndef TEST_FUZZTEST_DRAWING_FUZZER_H
#define TEST_FUZZTEST_DRAWING_FUZZER_H

#include <climits>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <memory>

#include "drawing_bitmap.h"
#include "drawing_brush.h"
#include "drawing_canvas.h"
#include "drawing_color.h"
#include "drawing_font_collection.h"
#include "drawing_path.h"
#include "drawing_pen.h"
#include "drawing_point.h"
#include "drawing_text_declaration.h"
#include "drawing_text_line.h"
#include "drawing_text_lineTypography.h"
#include "drawing_text_typography.h"
#include "drawing_types.h"
#include "get_object.h"

#define FUZZ_PROJECT_NAME "drawing_fuzzer"

namespace OHOS::Rosen::Drawing {
constexpr inline size_t DATA_MIN_SIZE = 8;
constexpr inline size_t DATA_MAX_ENUM_SIZE1 = 3;
constexpr inline size_t DATA_MAX_ENUM_SIZE2 = 4;
constexpr inline size_t DATA_MAX_ENUM_SIZE3 = 5;
constexpr inline size_t DATA_MAX_ENUM_SIZE4 = 8;
constexpr inline size_t DATA_MAX_ENUM_SIZE5 = 11;
constexpr inline size_t DATA_MAX_ENUM_FONTSIZE = 40;
constexpr inline size_t DATA_MAX_LAYOUT_WIDTH = 100;
constexpr inline size_t DATA_MAX_RANDOM = 500;
} // namespace OHOS::Rosen::Drawing

#endif // TEST_FUZZTEST_DRAWING_FUZZER_H