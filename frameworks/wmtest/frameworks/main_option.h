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

#ifndef FRAMEWORKS_WMTEST_FRAMEWORKS_MAIN_OPTION_H
#define FRAMEWORKS_WMTEST_FRAMEWORKS_MAIN_OPTION_H

#include <option_parser.h>

class MainOption : public OptionParser {
public:
    MainOption();
    int32_t Parse(int32_t argc, const char **argv);

    // attr
    std::string domain = "";
    int32_t testcase = -1;
    int32_t displayID = 0;
};

#endif // FRAMEWORKS_WMTEST_FRAMEWORKS_MAIN_OPTION_H
