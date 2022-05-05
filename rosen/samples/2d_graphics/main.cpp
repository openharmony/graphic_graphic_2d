/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "drawing_engine_sample.h"

#include <iostream>
#include <string>

#include "benchmark_config.h"
#include "drawing_multithread.h"
#include "drawing_singlethread.h"
#include "drawing_api.h"

#include "drawing_singlethread.h"

using namespace OHOS;
using namespace OHOS::Rosen;

int32_t main(int32_t argc, char *argv[])
{
    DrawingEngineSample m;

    std::cout << "benchmark name is " << argv[1] << std::endl;

    BenchmarkConfig::SetBenchMarkType(std::string(argv[1]));

    BenchMark* benchMark = nullptr;

    switch (BenchmarkConfig::benchMarkType_) {
        case BenchMarkName::SINGLETHREAD:
            std::cout << "new DrawingSinglethread()" << std::endl;
            benchMark = new DrawingSinglethread();
            break;
        case BenchMarkName::MULTITHREAD:
            std::cout << "new DrawingMultithread()" << std::endl;
            benchMark = new DrawingMultithread();
            break;
        case BenchMarkName::API:
            std::cout << "new DrawingApi()" << std::endl;
            benchMark = new DrawingApi();
            break;
        default:
            break;
    }

    std::cout << "start to SetBenchMark" << std::endl;
    m.SetBenchMark(benchMark);
    m.Run();
    return 0;
}