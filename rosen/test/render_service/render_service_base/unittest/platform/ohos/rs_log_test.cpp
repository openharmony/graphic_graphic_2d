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

} // namespace Rosen
} // namespace OHOS