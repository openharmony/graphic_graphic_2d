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

#include "cj_drawing_util.h"

namespace OHOS {
namespace CJDrawing {
void ConvertToPointsArray(const CArrPoint& arr, Rosen::Drawing::Point* points, int64_t count)
{
    for (int64_t index = 0; index < count; index++) {
        points[index].SetX(arr.head[index].x);
        points[index].SetY(arr.head[index].y);
    }
}

void WriteToCjPointsArray(const Rosen::Drawing::Point* points, const CArrPoint& arr, int64_t count)
{
    for (int64_t index = 0; index < count; index++) {
        arr.head[index].x = points[index].GetX();
        arr.head[index].y = points[index].GetY();
    }
}
}
}
