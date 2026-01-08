/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RS_PARAMETER_PARSE_H
#define RS_PARAMETER_PARSE_H

#include <string>
#include <unordered_set>
#include "rs_graphic_test_ext.h"

namespace OHOS {
namespace Rosen {
class RSParameterParse {
private:
    RSParameterParse() {}

public:
    static RSParameterParse& Instance();
    static void Parse(int argc, char **argv);
    void SetSkipCapture(bool isSkip);

    std::string imageSavePath = "/data/local/graphic_test/";
    int testCaseWaitTime = 1000; //ms
    int surfaceCaptureWaitTime = 1000; //ms
    int manualTestWaitTime = 1500; //ms
    std::unordered_set<RSGraphicTestType> filterTestTypes = {};
    RSGraphicTestMode runTestMode = RSGraphicTestMode::ALL;
    int32_t vsyncRate = 1;
    bool skipCapture_ = false;
};

} // namespace Rosen
} // namespace OHOS

#endif // RS_PARAMETER_PARSE_H
