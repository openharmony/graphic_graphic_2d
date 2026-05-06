/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include <gtest/gtest.h>
#include "rs_trace.h"
#include "platform/common/rs_log.h"
 
using namespace testing;
using namespace testing::ext;
 
namespace OHOS {
namespace Rosen {
class RSTraceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};
 
void RSTraceTest::SetUpTestCase() {}
void RSTraceTest::TearDownTestCase() {}
void RSTraceTest::SetUp() {}
void RSTraceTest::TearDown() {}
 
/**
 * @tc.name: RSUserLogTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTraceTest, RSUserTraceTest, TestSize.Level1)
{
    uint64_t screenId = 100;
    // Test USER trace macros - basic version
    RS_USER_TRACE_BEGIN(screenId, "USER_TEST Test");
    RS_USER_TRACE_END(screenId);
    RS_USER_TRACE_NAME(screenId, "USER_TEST_TRACE_NAM");
    RS_USER_TRACE_NAME_FMT(screenId, "USER_TEST screenId=%" PRIu64, screenId);
    RS_USER_TRACE_INT(screenId, "USER_TEST_TestInt", 123);
    RS_USER_ASYNC_TRACE_BEGIN(screenId, "USER_TEST_AsyncTest", 1);
    RS_USER_ASYNC_TRACE_END(screenId, "USER_TEST_AsyncTest", 1);
    RS_USER_TRACE_FUNC(screenId);
    // Test USER trace macros - DEBUG level
    RS_USER_TRACE_BEGIN_DEBUG(screenId, "USER_TEST_DEBUG Test", "");
    RS_USER_TRACE_END_DEBUG(screenId);
    RS_USER_TRACE_NAME_DEBUG(screenId, "USER_TEST_NAME_DEBUG Test", "");
    RS_USER_TRACE_NAME_FMT_DEBUG(screenId, "", "USER_TEST_DEBUG screenId=%" PRIu64, screenId);
    RS_USER_ASYNC_TRACE_BEGIN_DEBUG(screenId, "USER_TEST_DEBUG AsyncTest", 2, "", "");
    RS_USER_ASYNC_TRACE_END_DEBUG(screenId, "USER_TEST_DEBUG AsyncTest", 2);
    RS_USER_TRACE_INT_DEBUG(screenId, "USER_TEST_DEBUG TestInt", 456);
    RS_USER_TRACE_FUNC_DEBUG(screenId);
    // Test USER trace macros - INFO level
    RS_USER_TRACE_BEGIN_INFO(screenId, "USER_TEST_INFO Test", "");
    RS_USER_TRACE_END_INFO(screenId);
    RS_USER_TRACE_NAME_INFO(screenId, "USER_TEST_NAME_INFO Test", "");
    RS_USER_TRACE_NAME_FMT_INFO(screenId, "", "USER_TEST_INFO screenId=%" PRIu64, screenId);
    RS_USER_ASYNC_TRACE_BEGIN_INFO(screenId, "USER_TEST_INFO AsyncTest", 3, "", "");
    RS_USER_ASYNC_TRACE_END_INFO(screenId, "USER_TEST_INFO AsyncTest", 3);
    RS_USER_TRACE_INT_INFO(screenId, "USER_TEST_INFO TestInt", 789);
    RS_USER_TRACE_FUNC_INFO(screenId);
    // Test USER trace macros - CRITICAL level
    RS_USER_TRACE_BEGIN_CRITICAL(screenId, "USER_TEST_CRITICAL Test", "");
    RS_USER_TRACE_END_CRITICAL(screenId);
    RS_USER_TRACE_NAME_CRITICAL(screenId, "USER_TEST_NAME_CRITICAL Test", "");
    RS_USER_TRACE_NAME_FMT_CRITICAL(screenId, "", "USER_TEST_CRITICAL screenId=%" PRIu64, screenId);
    RS_USER_ASYNC_TRACE_BEGIN_CRITICAL(screenId, "USER_TEST_CRITICAL AsyncTest", 4, "", "");
    RS_USER_ASYNC_TRACE_END_CRITICAL(screenId, "USER_TEST_CRITICAL AsyncTest", 4);
    RS_USER_TRACE_INT_CRITICAL(screenId, "USER_TEST_CRITICAL TestInt", 101);
    RS_USER_TRACE_FUNC_CRITICAL(screenId);
    // Test USER trace macros - COMMERCIAL level
    RS_USER_TRACE_BEGIN_COMMERCIAL(screenId, "USER_TEST_COMMERCIAL Test", "");
    RS_USER_TRACE_END_COMMERCIAL(screenId);
    RS_USER_TRACE_NAME_COMMERCIAL(screenId, "USER_TEST_NAME_COMMERCIAL Test", "");
    RS_USER_TRACE_NAME_FMT_COMMERCIAL(screenId, "", "USER_TEST_COMMERCIAL screenId=%" PRIu64, screenId);
    RS_USER_ASYNC_TRACE_BEGIN_COMMERCIAL(screenId, "USER_TEST_COMMERCIAL AsyncTest", 5, "", "");
    RS_USER_ASYNC_TRACE_END_COMMERCIAL(screenId, "USER_TEST_COMMERCIAL AsyncTest", 5);
    RS_USER_TRACE_INT_COMMERCIAL(screenId, "USER_TEST_COMMERCIAL TestInt", 202);
    RS_USER_TRACE_FUNC_COMMERCIAL(screenId);
    RS_USER_TRACE_BEGIN(screenId, "USER_TEST Test");
    RS_USER_TRACE_END(screenId);
    EXPECT_NE(screenId, 0);
}
} // namespace Rosen
} // namespace OHOS