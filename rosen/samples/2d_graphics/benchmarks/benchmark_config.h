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
#ifndef BENCHMARK_CONFIG_H
#define BENCHMARK_CONFIG_H

#include <iostream>

#include "benchmark.h"

namespace OHOS {
namespace Rosen {
enum class BenchMarkName {
    SINGLETHREAD = 0,
    MULTITHREAD,
    API,
    DCL,
    LASTNAME,
};

class BenchmarkConfig {
public:
    BenchmarkConfig() {}
    ~BenchmarkConfig() {}
    static void SetBenchMarkType(const std::string &type);
    static enum BenchMarkName benchMarkType_;
};
}
}
#endif