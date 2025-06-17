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

#include <cstdio>
#include <iostream>

#include "common/safuzz_log.h"
#include "pipeline/argument_parser.h"

namespace OHOS {
namespace Rosen {
int RSIpcDosSimulator(int argc, char *argv[])
{
    fflush(stdout);

    std::cout << std::endl << "command list:" << std::endl;
    for (int i = 1; i < argc; ++i) {
        std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
    }
    std::cout << std::endl;

    ArgumentParser argumentParser;
    if (!argumentParser.Init(argc, argv)) {
        SAFUZZ_LOGE("RSIpcDosSimulator init failed");
        return -1;
    }
    if (!argumentParser.Process()) {
        SAFUZZ_LOGE("RSIpcDosSimulator process failed");
        return -1;
    }

    SAFUZZ_LOGI("RSIpcDosSimulator end successfully");
    return 0;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    return OHOS::Rosen::RSIpcDosSimulator(*argc, *argv);
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* /* data */, size_t /* size */)
{
    quick_exit(0);
    return 0;
}
