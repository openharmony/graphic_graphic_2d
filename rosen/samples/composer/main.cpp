/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "hello_composer.h"

#include <iostream>

int32_t main(int32_t argc, const char *argv[])
{
    std::vector<std::string> runArgs;
    runArgs.resize(argc);

    for (int32_t i = 0; i < argc; i++) {
        std::cout << "argv[" << i << "]: " << argv[i] << std::endl;
        runArgs[i] = argv[i];
    }

    OHOS::Rosen::HelloComposer m;
    m.Run(runArgs);

    return 0;
}