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
#ifndef DRAWING_DEMO_H
#define DRAWING_DEMO_H
#include <chrono>
#include <iostream>
#include <string>

#include "transaction/rs_transaction.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_director.h"
#include "utils/log.h"
#include "window.h"

#include "test_case/test_base.h"

namespace OHOS {
namespace Rosen {
#define ALL_TAST_CASE "ALL"
#define DEFAULT_DISPLAY_TIME 10
#define SLEEP_TIME 30000
#define MAX_TRY_NUMBER 3
#define INDEX_TEST_TYPE 1
#define INDEX_DRAWING_TYPE 2
#define INDEX_CASE_NAME 3
#define INDEX_FUNCTION_TIME 4
#define INDEX_COUNT 4
#define INDEX_PERFORMANCE_TIME 5

class DrawingDemo {
public:
    DrawingDemo(int argc, char* argv[]);
    ~DrawingDemo();
    int Test(TestDisplayCanvas* canvas);
    enum {
        FUNCTION_CPU = 0,
        FUNCTION_GPU_UPSCREEN,
        PERFORMANCE_CPU,
        PERFORMANCE_GPU_UPSCREEN,
    };

protected:
    int InitWindow();
    int CreateWindow();
    int GetFunctionalParam(std::unordered_map<std::string, std::function<std::shared_ptr<TestBase>()>>& map);
    int TestFunction(int type);
    int GetPerformanceParam(std::shared_ptr<TestBase>& testCase);
    int TestPerformance(TestDisplayCanvas* canvasExt, int type);

    int argc_ = 0;
    std::vector<std::string> argv_;
    std::shared_ptr<RSNode> rootNode_ = nullptr;
    std::shared_ptr<RSCanvasNode> canvasNode_ = nullptr;
    sptr<OHOS::Rosen::Window> window_ = nullptr;
    std::shared_ptr<RSUIDirector> rsUiDirector_ = nullptr;
    Rect rect_;
    std::string testType_;
    std::string drawingType_;
    std::string caseName_ = ALL_TAST_CASE;
    int displayTime_ = DEFAULT_DISPLAY_TIME;
    int testCount_ = DEFAULT_TEST_COUNT;
};
} // namespace Rosen
} // namespace OHOS
#endif // DRAWING_DEMO_H