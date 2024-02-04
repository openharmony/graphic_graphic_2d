/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#ifndef BenchMark_H
#define BenchMark_H

#include <iostream>
#include <vector>
#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/core/SkBitmap.h"
#include "include/core/SkImage.h"
#include "recording/draw_cmd_list.h"
#include "benchmark_result.h"

namespace OHOS {
namespace Rosen {
class BenchMark {
public:
    BenchMark() {}
    virtual ~BenchMark() {}
    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual void Test(Drawing::Canvas* canvas, int width, int height) = 0;
    virtual void Output() = 0;
};
}
}
#endif