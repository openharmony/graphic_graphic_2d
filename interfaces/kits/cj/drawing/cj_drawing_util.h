/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_CJ_DRAWING_UTIL_H
#define OHOS_CJ_DRAWING_UTIL_H

#include "cj_common_ffi.h"
#include "utils/point.h"
#include "utils/scalar.h"

namespace OHOS {
namespace CJDrawing {
using Rosen::Drawing::scalar;

constexpr size_t ARGC_ZERO = 0;
constexpr size_t ARGC_ONE = 1;
constexpr size_t ARGC_TWO = 2;
constexpr size_t ARGC_THREE = 3;
constexpr size_t ARGC_FOUR = 4;
constexpr size_t ARGC_FIVE = 5;
constexpr size_t ARGC_SIX = 6;
constexpr size_t ARGC_SEVEN = 7;
constexpr size_t ARGC_EIGHT = 8;

struct CPoint {
    scalar x;
    scalar y;
};

struct CRect {
    scalar left;
    scalar top;
    scalar right;
    scalar bottom;
};

struct CArrPoint {
    CPoint* head;
    int64_t size;
};

struct CArrFloat {
    scalar* head;
    int64_t size;
};

struct RetDataScalar {
    int32_t code;
    scalar data;
};

struct RetDataArrPoint {
    int32_t code;
    CArrPoint data;
};

enum class CjDrawingErrorCode : int32_t {
    INTERNAL_ERROR = -1,
    OK = 0,
    ERROR_PARAM_VERIFICATION_FAILED = 25900001,
};

inline int32_t ParserErr(CjDrawingErrorCode code)
{
    return static_cast<int32_t>(code);
}

void ConvertToPointsArray(const CArrPoint& arr, Rosen::Drawing::Point* points, int64_t count);
void WriteToCjPointsArray(const Rosen::Drawing::Point* points, const CArrPoint& arr, int64_t count);
}
}
#endif
