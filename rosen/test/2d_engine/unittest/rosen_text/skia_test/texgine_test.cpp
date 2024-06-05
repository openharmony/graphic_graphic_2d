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

#include "gtest/gtest.h"
#include "texgine/utils/exlog.h"
#include "texgine/utils/logger.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class OHTexgineTxtTest : public testing::Test {};

/*
 * @tc.name: OHTexgineTxtTest001
 * @tc.desc: test for log
 * @tc.type: FUNC
 */
HWTEST_F(OHTexgineTxtTest, OHTexgineTxtTest001, TestSize.Level1)
{
    // 1 is used for test
    Logger logger1("123", "func1", 1, Logger::LOG_LEVEL::WARN, nullptr);
    // 2 is used for test
    Logger logger2("123", "func2", 2, Logger::LOG_LEVEL::FATAL, nullptr);
    Logger logger3(logger1);
    Logger logger4(std::move(logger3));
    Logger::SetToNoReturn(logger1, Logger::LOG_PHASE::BEGIN);
    Logger::SetToContinue(logger2, Logger::LOG_PHASE::BEGIN);
    Logger::OutputByStderr(logger4, Logger::LOG_PHASE::BEGIN);
    Logger::OutputByStderr(logger1, Logger::LOG_PHASE::END);
    Logger::OutputByStderr(logger2, Logger::LOG_PHASE::END);
    Logger::OutputByFileLog(logger1, Logger::LOG_PHASE::BEGIN);
    Logger::OutputByFileLog(logger1, Logger::LOG_PHASE::END);
    Logger::AppendFunc(logger1, Logger::LOG_PHASE::BEGIN);
    Logger::AppendFileLine(logger1, Logger::LOG_PHASE::BEGIN);
    Logger::AppendFileFuncLine(logger1, Logger::LOG_PHASE::BEGIN);
    Logger::AppendPidTid(logger1, Logger::LOG_PHASE::BEGIN);
    // 1 is used for test
    Logger::SetScopeParam(1, 1);
    Logger::EnterScope();
    Logger::ExitScope();
    std::string str = logger1.GetFile();
    EXPECT_EQ(str, "123");
    va_list valist = logger1.GetVariousArgument();
    valist = logger2.GetVariousArgument();
    // 1 is used for test
    logger1.Align(1);
    logger2.Align(1);
    NoLogger noLogger;
    ScopedLogger scopedLogger1(std::move(noLogger));
    // "123" is used for test
    ScopedLogger scopedLogger2(std::move(noLogger), "123");
    ScopedLogger scopedLogger3(std::move(logger3));
    // "123" is used for test
    ScopedLogger scopedLogger4(std::move(logger3), "123");
    scopedLogger1.Finish();
    scopedLogger2.Finish();
    scopedLogger3.Finish();
    scopedLogger4.Finish();
    ExTime(logger1, Logger::LOG_PHASE::BEGIN);
    ExTime(logger1, Logger::LOG_PHASE::END);
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
