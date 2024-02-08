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

#include <cstddef>
#include "gtest/gtest.h"
#include "skia_adapter/rs_skia_memory_tracer.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaMemoryTracerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaMemoryTracerTest::SetUpTestCase() {}
void SkiaMemoryTracerTest::TearDownTestCase() {}
void SkiaMemoryTracerTest::SetUp() {}
void SkiaMemoryTracerTest::TearDown() {}

/**
 * @tc.name: SkiaMemoryTracer001
 * @tc.desc: Test functions in SkiaMemoryTracer
 * @tc.type: FUNC
 * @tc.require: I91EQ7
 */
HWTEST_F(SkiaMemoryTracerTest, SkiaMemoryTracer001, TestSize.Level1)
{
    std::vector<ResourcePair> resourceMap{{"dumpName1", "valueName1"}, {"dumpName2", "valueName2"}};
    SkiaMemoryTracer skiaMemoryTracer(resourceMap, true);
    uint64_t value1 = 1;
    uint64_t value2 = 1025;
    skiaMemoryTracer.dumpNumericValue("dumpName1", "valueName", "bytes", value1);
    skiaMemoryTracer.dumpNumericValue("dumpName1", "size", "bytes", value1);
    skiaMemoryTracer.dumpNumericValue("dumpName1", "purgeable_size", "MB", value1);
    skiaMemoryTracer.dumpNumericValue("dumpName1", "type", "KB", value1);
    skiaMemoryTracer.dumpNumericValue("dumpName2", "valueName1", "bytes", value2);
    skiaMemoryTracer.dumpNumericValue("dumpName2", "valueName2", "bytes", value2);
    skiaMemoryTracer.dumpNumericValue("dumpName2", "Scratch", "bytes", value2);
    skiaMemoryTracer.dumpNumericValue("dumpName2", "skia/gr_text_blob_cache", "bytes", value2);
    DfxString log;
    skiaMemoryTracer.LogOutput(log);
    skiaMemoryTracer.LogTotals(log);
    EXPECT_TRUE(skiaMemoryTracer.GetGLMemorySize() >= 0);
    EXPECT_TRUE(skiaMemoryTracer.GetGpuMemorySizeInMB() >= 0);
    SkiaMemoryTracer skiaMemoryTracer2("category", false);
    skiaMemoryTracer2.dumpNumericValue("dumpName", "valueName", "bytes", value1);
    skiaMemoryTracer2.dumpNumericValue("dumpName", "category", "bytes", value1);
    skiaMemoryTracer2.LogOutput(log);
    skiaMemoryTracer2.LogTotals(log);
    std::vector<ResourcePair> resourceMap3{{"size", "valueName1"}, {"dumpName2", "valueName2"}};
    SkiaMemoryTracer skiaMemoryTracer3(resourceMap, false);
    skiaMemoryTracer3.dumpNumericValue("dumpName2", "Scratch", "KB", value1);
    skiaMemoryTracer3.LogOutput(log);
    skiaMemoryTracer3.LogTotals(log);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS