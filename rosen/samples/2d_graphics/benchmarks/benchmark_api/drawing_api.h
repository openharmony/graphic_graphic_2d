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

#ifndef DRAWING_API_H
#define DRAWING_API_H
#include "benchmark.h"
namespace OHOS {
namespace Rosen {
class DrawingApi : public BenchMark {
public:
    DrawingApi() {}
    ~DrawingApi() {}

    virtual void Start() override {}
    virtual void Stop() override {}
    virtual void Test(Drawing::Canvas* canvas, int width, int height) override {}
    virtual void Output() override {}
};
}
}
#endif