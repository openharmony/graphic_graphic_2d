/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "platform/common/rs_log.h"
#include <hilog/log.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSLogTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSLogTest::SetUpTestCase() {}
void RSLogTest::TearDownTestCase() {}
void RSLogTest::SetUp() {}
void RSLogTest::TearDown() {}

/**
 * @tc.name: RSLogOutput001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSLogTest, RSLogOutput001, TestSize.Level1)
{
    RSLog rsLog;
    RSLog::Tag tag = RSLog::Tag::RS_CLIENT;
    RSLogOutput(tag, RSLog::Level::LEVEL_DEBUG, "format");
    RSLogOutput(tag, RSLog::Level::LEVEL_WARN, "format");
    RSLogOutput(tag, RSLog::Level::LEVEL_ERROR, "format");
    RSLogOutput(tag, RSLog::Level::LEVEL_INFO, "format");
    RSLogOutput(tag, RSLog::Level::LEVEL_FATAL, "format");
    EXPECT_NE(tag, -1);
}

/**
 * @tc.name: RSUserLogTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSLogTest, RSUserLogTest, TestSize.Level1)
{
    uint64_t screenId = 100;
    RS_USER_LOGI(screenId, "USER_TEST LOGI with screenId=%{public}" PRIu64, screenId);
    RS_USER_LOGD(screenId, "USER_TEST LOGD with screenId=%{public}" PRIu64, screenId);
    RS_USER_LOGW(screenId, "USER_TEST LOGW with screenId=%{public}" PRIu64, screenId);
    RS_USER_LOGE(screenId, "USER_TEST LOGE with screenId=%{public}" PRIu64, screenId);
    RS_USER_LOGF(screenId, "USER_TEST LOGF with screenId=%{public}" PRIu64, screenId);
    // Test USER conditional log macros true
    RS_USER_LOGI_IF(true, screenId, "USER_TEST LOGI_IF true with screenId=%{public}" PRIu64, screenId);
    RS_USER_LOGD_IF(true, screenId, "USER_TEST LOGD_IF true with screenId=%{public}" PRIu64, screenId);
    RS_USER_LOGW_IF(true, screenId, "USER_TEST LOGW_IF true with screenId=%{public}" PRIu64, screenId);
    RS_USER_LOGE_IF(true, screenId, "USER_TEST LOGE_IF true with screenId=%{public}" PRIu64, screenId);
    RS_USER_LOGF_IF(true, screenId, "USER_TEST LOGF_IF true with screenId=%{public}" PRIu64, screenId);
    // Test USER conditional log macros false
    RS_USER_LOGI_IF(false, screenId, "USER_TEST LOGI_IF false with screenId=%{public}" PRIu64, screenId);
    RS_USER_LOGD_IF(false, screenId, "USER_TEST LOGD_IF false with screenId=%{public}" PRIu64, screenId);
    RS_USER_LOGW_IF(false, screenId, "USER_TEST LOGW_IF false with screenId=%{public}" PRIu64, screenId);
    RS_USER_LOGE_IF(false, screenId, "USER_TEST LOGE_IF false with screenId=%{public}" PRIu64, screenId);
    RS_USER_LOGF_IF(false, screenId, "USER_TEST LOGF_IF false with screenId=%{public}" PRIu64, screenId);
    // Test USER rate-limit log macros
    for (int i = 0; i < 10; i++) {
        RS_USER_LOGE_LIMIT(TestUserTraceAndLog, 0, screenId,
                            "USER_TEST LOGE_LIMIT with screenId=%{public}" PRIu64, screenId);
        RS_USER_LOGI_LIMIT(TestUserTraceAndLog, 0, screenId,
                            "USER_TEST LOGI_LIMIT with screenId=%{public}" PRIu64, screenId);
    }
    
    EXPECT_NE(screenId, 0);
}
} // namespace Rosen
} // namespace OHOS