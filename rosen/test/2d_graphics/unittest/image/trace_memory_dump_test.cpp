/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "image/trace_memory_dump.h"

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class TraceMemoryDumpTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void TraceMemoryDumpTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}
void TraceMemoryDumpTest::TearDownTestCase() {}
void TraceMemoryDumpTest::SetUp() {}
void TraceMemoryDumpTest::TearDown() {}

/**
 * @tc.name: DumpNumericValueTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(TraceMemoryDumpTest, DumpNumericValueTest001, TestSize.Level1)
{
    std::unique_ptr<TraceMemoryDump> traceMemoryDump = std::make_unique<TraceMemoryDump>("category", true);
    ASSERT_TRUE(traceMemoryDump != nullptr);
    traceMemoryDump->DumpNumericValue("dumpName", "valueName", "units", 0);
}

/**
 * @tc.name: DumpStringValueTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(TraceMemoryDumpTest, DumpStringValueTest001, TestSize.Level1)
{
    std::unique_ptr<TraceMemoryDump> traceMemoryDump = std::make_unique<TraceMemoryDump>("category", true);
    ASSERT_TRUE(traceMemoryDump != nullptr);
    traceMemoryDump->DumpStringValue("dumpName", "valueName", "value");
}

/**
 * @tc.name: LogOutputTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(TraceMemoryDumpTest, LogOutputTest001, TestSize.Level1)
{
    std::unique_ptr<TraceMemoryDump> traceMemoryDump = std::make_unique<TraceMemoryDump>("category", true);
    ASSERT_TRUE(traceMemoryDump != nullptr);
    DfxString log;
    traceMemoryDump->LogOutput(log);
}

/**
 * @tc.name: LogTotalsTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(TraceMemoryDumpTest, LogTotalsTest001, TestSize.Level1)
{
    std::unique_ptr<TraceMemoryDump> traceMemoryDump = std::make_unique<TraceMemoryDump>("category", true);
    ASSERT_TRUE(traceMemoryDump != nullptr);
    DfxString log;
    traceMemoryDump->LogTotals(log);
}

/**
 * @tc.name: GetGpuMemorySizeInMBTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(TraceMemoryDumpTest, GetGpuMemorySizeInMBTest001, TestSize.Level1)
{
    std::unique_ptr<TraceMemoryDump> traceMemoryDump = std::make_unique<TraceMemoryDump>("category", true);
    ASSERT_TRUE(traceMemoryDump != nullptr);
    float mem = traceMemoryDump->GetGpuMemorySizeInMB();
    EXPECT_GE(mem, 0.0);
}

/**
 * @tc.name: GetGLMemorySizeTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(TraceMemoryDumpTest, GetGLMemorySizeTest001, TestSize.Level1)
{
    std::unique_ptr<TraceMemoryDump> traceMemoryDump = std::make_unique<TraceMemoryDump>("category", true);
    ASSERT_TRUE(traceMemoryDump != nullptr);
    float mem = traceMemoryDump->GetGLMemorySize();
    EXPECT_GE(mem, 0.0);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS