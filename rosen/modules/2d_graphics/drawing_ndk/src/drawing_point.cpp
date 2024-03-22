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

#include "drawing_point.h"

#include "utils/point.h"
#include "utils/point3.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static Point* CastToPoint(OH_Drawing_Point* cPoint)
{
    return reinterpret_cast<Point*>(cPoint);
}

OH_Drawing_Point* OH_Drawing_PointCreate(float x, float y)
{
    return (OH_Drawing_Point*)new Point(x, y);
}

void OH_Drawing_PointDestroy(OH_Drawing_Point* cPoint)
{
    delete CastToPoint(cPoint);
}
