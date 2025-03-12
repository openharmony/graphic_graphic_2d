/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"

#include "utils/performanceCaculate.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class PerformanceCaculateTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void PerformanceCaculateTest::SetUpTestCase() {}
void PerformanceCaculateTest::TearDownTestCase() {}
void PerformanceCaculateTest::SetUp() {}
void PerformanceCaculateTest::TearDown() {}

/**
 * @tc.name: GetUpTime001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:IAKGJ7
 * @tc.author:
 */
HWTEST_F(PerformanceCaculateTest, GetUpTime001, TestSize.Level1)
{
    std::unique_ptr<PerformanceCaculate> caculate = std::make_unique<PerformanceCaculate>();
    bool addCount = true;
    long long time = caculate->GetUpTime(addCount);
    ASSERT_TRUE(time != 0);
}

/**
 * @tc.name: SetCaculateSwitch002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:IAKGJ7
 * @tc.author:
 */
HWTEST_F(PerformanceCaculateTest, SetCaculateSwitch002, TestSize.Level1)
{
    std::unique_ptr<PerformanceCaculate> caculate = std::make_unique<PerformanceCaculate>();
    bool start = true;
    caculate->SetCaculateSwitch(start);
    ASSERT_TRUE(caculate->drawingTestFlag_ == PerformanceCaculate::TestFlag::TSET_ALL);
}

/**
 * @tc.name: GetDrawingTestJsEnabled003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:IAKGJ7
 * @tc.author:
 */
HWTEST_F(PerformanceCaculateTest, GetDrawingTestJsEnabled003, TestSize.Level1)
{
    std::unique_ptr<PerformanceCaculate> caculate = std::make_unique<PerformanceCaculate>();
    caculate->performanceCaculateSwitch_ = true;
    caculate->drawingTestFlag_ = PerformanceCaculate::TestFlag::TEST_JS;
    bool testflag = caculate->GetDrawingTestJsEnabled();
    ASSERT_TRUE(testflag);
}

/**
 * @tc.name: GetDrawingTestNapiEnabled004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:IAKGJ7
 * @tc.author:
 */
HWTEST_F(PerformanceCaculateTest, GetDrawingTestNapiEnabled004, TestSize.Level1)
{
    std::unique_ptr<PerformanceCaculate> caculate = std::make_unique<PerformanceCaculate>();
    caculate->performanceCaculateSwitch_ = true;
    caculate->drawingTestFlag_ = PerformanceCaculate::TestFlag::TEST_NAPI;
    bool testflag = caculate->GetDrawingTestNapiEnabled();
    ASSERT_TRUE(testflag);
}

/**
 * @tc.name: GetDrawingTestSkiaEnabled005
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:IAKGJ7
 * @tc.author:
 */
HWTEST_F(PerformanceCaculateTest, GetDrawingTestSkiaEnabled005, TestSize.Level1)
{
    std::unique_ptr<PerformanceCaculate> caculate = std::make_unique<PerformanceCaculate>();
    caculate->performanceCaculateSwitch_ = true;
    caculate->drawingTestFlag_ = PerformanceCaculate::TestFlag::TEST_SKIA;
    bool testflag = caculate->GetDrawingTestSkiaEnabled();
    ASSERT_TRUE(testflag);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS