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
#include "property/rs_property_trace.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSPropertyTraceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPropertyTraceTest::SetUpTestCase() {}
void RSPropertyTraceTest::TearDownTestCase() {}
void RSPropertyTraceTest::SetUp() {}
void RSPropertyTraceTest::TearDown() {}

/**
 * @tc.name: PropertiesDisplayByTrace001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyTraceTest, PropertiesDisplayByTrace001, TestSize.Level1)
{
    RSProperties properties;
    const NodeId id = 1;
    RSPropertyTrace::GetInstance().PropertiesDisplayByTrace(id, properties);
}

/**
 * @tc.name: RefreshNodeTraceInfo001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyTraceTest, RefreshNodeTraceInfo001, TestSize.Level1)
{
    RSPropertyTrace::GetInstance().RefreshNodeTraceInfo();
}
} // namespace Rosen
} // namespace OHOS