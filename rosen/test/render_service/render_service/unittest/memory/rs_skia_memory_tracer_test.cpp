/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "src/drawing/engine_adapter/skia_adapter/rs_skia_memory_tracer.h"
#include "memory/rs_dfx_string.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {


class RSSkiaMemoryTracerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSkiaMemoryTracerTest::SetUpTestCase() {}
void RSSkiaMemoryTracerTest::TearDownTestCase() {}
void RSSkiaMemoryTracerTest::SetUp() {}
void RSSkiaMemoryTracerTest::TearDown() {}

/**
 * @tc.name: SkiaMemoryTracer001
 * @tc.desc: Test SkiaMemoryTracer's constructor
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSkiaMemoryTracerTest, SkiaMemoryTracer001, TestSize.Level1)
{
    auto skiaMemoryTracer = std::make_shared<SkiaMemoryTracer>("category", true);
    ASSERT_TRUE(skiaMemoryTracer != nullptr);
    std::vector<ResourcePair> cpuResourceMap = {
        { "skia/sk_resource_cache/bitmap_", "Bitmaps" },
        { "skia/sk_resource_cache/rrect-blur_", "Masks" },
    };
    skiaMemoryTracer = std::make_shared<SkiaMemoryTracer>(cpuResourceMap, true);
    ASSERT_TRUE(skiaMemoryTracer != nullptr);
}

/**
 * @tc.name: SkiaMemoryTracer002
 * @tc.desc: Test SkiaMemoryTracer's constructor
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSkiaMemoryTracerTest, SkiaMemoryTracer002, TestSize.Level1)
{
    auto skiaMemoryTracer = std::make_shared<SkiaMemoryTracer>("category", false);
    ASSERT_TRUE(skiaMemoryTracer != nullptr);
    std::vector<ResourcePair> cpuResourceMap = {
        { "skia/sk_resource_cache/bitmap_", "Bitmaps" },
        { "skia/sk_resource_cache/rrect-blur_", "Masks" },
    };
    skiaMemoryTracer = std::make_shared<SkiaMemoryTracer>(cpuResourceMap, false);
    ASSERT_TRUE(skiaMemoryTracer != nullptr);
}

/**
 * @tc.name: SourceType2String1
 * @tc.desc: Test SkiaMemoryTracer's SourceType2String
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSkiaMemoryTracerTest, SourceType2String1, TestSize.Level1)
{
    auto skiaMemoryTracer = std::make_shared<SkiaMemoryTracer>("category", true);
    ASSERT_TRUE(skiaMemoryTracer != nullptr);
    auto sourceType = RSTagTracker::SOURCETYPE::SOURCE_DRAWRENDERCONTENT;
    std::string expectedStr = "source_drawrendercontent";
    ASSERT_TRUE(skiaMemoryTracer->SourceType2String(sourceType) == expectedStr);
    int sourceCount = 23;
    sourceType = static_cast<RSTagTracker::SOURCETYPE>(sourceCount);
    expectedStr = "";
    ASSERT_TRUE(skiaMemoryTracer->SourceType2String(sourceType) == expectedStr);
}

/**
 * @tc.name: ProcessElement001
 * @tc.desc: Test SkiaMemoryTracer's ProcessElement
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSkiaMemoryTracerTest, ProcessElement1, TestSize.Level1)
{
    auto skiaMemoryTracer = std::make_shared<SkiaMemoryTracer>("category", true);
    ASSERT_TRUE(skiaMemoryTracer != nullptr);
    skiaMemoryTracer->dumpNumericValue("resourceName", "size", "bytes", 1);
    skiaMemoryTracer->dumpStringValue("resourceName", "type", "External Texture");
    skiaMemoryTracer->dumpStringValue("resourceName", "category", "Image");
    skiaMemoryTracer->ProcessElement();
    ASSERT_TRUE(skiaMemoryTracer->GetGLMemorySize() != -1);
}

/**
 * @tc.name: LogTotals001
 * @tc.desc: Test SkiaMemoryTracer's LogTotals
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSkiaMemoryTracerTest, LogTotals001, TestSize.Level1)
{
    auto skiaMemoryTracer = std::make_shared<SkiaMemoryTracer>("category", true);
    ASSERT_TRUE(skiaMemoryTracer != nullptr);
    skiaMemoryTracer->dumpNumericValue("resourceName", "size", "bytes", 1);
    skiaMemoryTracer->dumpStringValue("resourceName", "type", "Texture");
    skiaMemoryTracer->dumpStringValue("resourceName", "category", "Image");
    DfxString log;
    skiaMemoryTracer->LogTotals(log);
    ASSERT_TRUE(skiaMemoryTracer->GetGLMemorySize() != -1);
}

/**
 * @tc.name: LogTotals002
 * @tc.desc: Test SkiaMemoryTracer's LogTotals
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSkiaMemoryTracerTest, LogTotals002, TestSize.Level1)
{
    auto skiaMemoryTracer = std::make_shared<SkiaMemoryTracer>("category", false);
    ASSERT_TRUE(skiaMemoryTracer != nullptr);
    skiaMemoryTracer->dumpNumericValue("resourceName1", "size", "bytes", 1);
    skiaMemoryTracer->dumpNumericValue("resourceName1", "source", "sourcetype", 1);
    skiaMemoryTracer->dumpStringValue("resourceName1", "type", "Texture");
    skiaMemoryTracer->dumpStringValue("resourceName1", "category", "Image");
    skiaMemoryTracer->dumpNumericValue("resourceName2", "size", "bytes", 1);
    skiaMemoryTracer->dumpNumericValue("resourceName2", "source", "sourcetype", 1);
    skiaMemoryTracer->dumpStringValue("resourceName2", "type", "RenderTarget");
    skiaMemoryTracer->dumpStringValue("resourceName2", "category", "Scratch");
    DfxString log;
    skiaMemoryTracer->LogTotals(log);
    ASSERT_TRUE(skiaMemoryTracer->GetGLMemorySize() != -1);
}

/**
 * @tc.name: LogTotals003
 * @tc.desc: Test SkiaMemoryTracer's LogTotals
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSkiaMemoryTracerTest, LogTotals003, TestSize.Level1)
{
    auto skiaMemoryTracer = std::make_shared<SkiaMemoryTracer>("category", false);
    ASSERT_TRUE(skiaMemoryTracer != nullptr);
    skiaMemoryTracer->dumpNumericValue("resourceName1", "size", "bytes", 1);
    skiaMemoryTracer->dumpNumericValue("resourceName1", "source", "sourcetype", 1);
    skiaMemoryTracer->dumpStringValue("resourceName1", "type", "kIndex Buffer");
    skiaMemoryTracer->dumpStringValue("resourceName1", "category", "Other");
    skiaMemoryTracer->dumpNumericValue("resourceName2", "size", "bytes", 1);
    skiaMemoryTracer->dumpNumericValue("resourceName2", "source", "sourcetype", 1);
    skiaMemoryTracer->dumpStringValue("resourceName2", "type", "kVertex Buffer");
    skiaMemoryTracer->dumpStringValue("resourceName2", "category", "Other");
    DfxString log;
    skiaMemoryTracer->LogTotals(log);
    ASSERT_TRUE(skiaMemoryTracer->GetGLMemorySize() != -1);
}

/*
 * @tc.name: dumpNumericValueTest
 * @tc.desc: dumpNumericValue Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSkiaMemoryTracerTest, dumpNumericValueTest, testing::ext::TestSize.Level1)
{
    bool itemizeType = true;
    auto skiaMemoryTracer = std::make_shared<SkiaMemoryTracer>("category", itemizeType);
    ASSERT_TRUE(skiaMemoryTracer != nullptr);
    uint64_t value = 1;
    skiaMemoryTracer->dumpNumericValue("dumpName", "valueName", "units", value);
    EXPECT_TRUE(skiaMemoryTracer->GetGLMemorySize() != -1);
}

/*
 * @tc.name: LogOutputTest1
 * @tc.desc: LogOutput Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSkiaMemoryTracerTest, LogOutputTest1, testing::ext::TestSize.Level1)
{
    bool itemizeType = false;
    auto skiaMemoryTracer = std::make_shared<SkiaMemoryTracer>("category", itemizeType);
    ASSERT_TRUE(skiaMemoryTracer != nullptr);
    DfxString log;
    skiaMemoryTracer->LogOutput(log);
    EXPECT_TRUE(skiaMemoryTracer->GetGLMemorySize() != -1);
}

/*
 * @tc.name: LogOutputTest2
 * @tc.desc: LogOutput Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSkiaMemoryTracerTest, LogOutputTest2, testing::ext::TestSize.Level1)
{
    bool itemizeType = true;
    auto skiaMemoryTracer = std::make_shared<SkiaMemoryTracer>("category", itemizeType);
    ASSERT_TRUE(skiaMemoryTracer != nullptr);
    skiaMemoryTracer->dumpNumericValue("resourceName1", "size", "bytes", 1);
    skiaMemoryTracer->dumpNumericValue("resourceName1", "source", "sourcetype", 1);
    skiaMemoryTracer->dumpStringValue("resourceName1", "type", "Texture");
    skiaMemoryTracer->dumpStringValue("resourceName1", "category", "Image");
    skiaMemoryTracer->dumpNumericValue("resourceName2", "size", "bytes", 1);
    skiaMemoryTracer->dumpNumericValue("resourceName2", "source", "sourcetype", 1);
    skiaMemoryTracer->dumpStringValue("resourceName2", "type", "Texture");
    skiaMemoryTracer->dumpStringValue("resourceName2", "category", "Image");
    DfxString log;
    skiaMemoryTracer->LogOutput(log);
    EXPECT_TRUE(skiaMemoryTracer->GetGLMemorySize() != -1);
}

/*
 * @tc.name: LogOutputTest3
 * @tc.desc: LogOutput Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSkiaMemoryTracerTest, LogOutputTest3, testing::ext::TestSize.Level1)
{
    bool itemizeType = false;
    auto skiaMemoryTracer = std::make_shared<SkiaMemoryTracer>("category", itemizeType);
    ASSERT_TRUE(skiaMemoryTracer != nullptr);
    skiaMemoryTracer->dumpNumericValue("resourceName1", "size", "bytes", 1);
    skiaMemoryTracer->dumpNumericValue("resourceName1", "source", "sourcetype", 1);
    skiaMemoryTracer->dumpStringValue("resourceName1", "type", "Texture");
    skiaMemoryTracer->dumpNumericValue("resourceName2", "size", "bytes", 1);
    skiaMemoryTracer->dumpNumericValue("resourceName2", "source", "sourcetype", 1);
    skiaMemoryTracer->dumpStringValue("resourceName2", "type", "Texture");
    skiaMemoryTracer->dumpNumericValue("resourceName3", "size", "bytes", 1);
    skiaMemoryTracer->dumpNumericValue("resourceName3", "source", "sourcetype", 0);
    skiaMemoryTracer->dumpStringValue("resourceName3", "type", "kUniform Buffer");
    DfxString log;
    skiaMemoryTracer->LogOutput(log);
    EXPECT_TRUE(skiaMemoryTracer->GetGLMemorySize() != -1);
}

/*
 * @tc.name: LogOutputTest4
 * @tc.desc: LogOutput Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSkiaMemoryTracerTest, LogOutputTest4, testing::ext::TestSize.Level1)
{
    bool itemizeType = false;
    auto skiaMemoryTracer = std::make_shared<SkiaMemoryTracer>("category", itemizeType);
    ASSERT_TRUE(skiaMemoryTracer != nullptr);
    skiaMemoryTracer->dumpNumericValue("resourceName1", "size", "bytes", 1);
    skiaMemoryTracer->dumpNumericValue("resourceName1", "source", "sourcetype", 1);
    skiaMemoryTracer->dumpStringValue("resourceName1", "category", "Image");
    skiaMemoryTracer->dumpStringValue("resourceName1", "type", "Texture");
    skiaMemoryTracer->dumpNumericValue("resourceName2", "size", "bytes", 1);
    skiaMemoryTracer->dumpNumericValue("resourceName2", "source", "sourcetype", 1);
    skiaMemoryTracer->dumpStringValue("resourceName2", "category", "Image");
    skiaMemoryTracer->dumpStringValue("resourceName2", "type", "RenderTarget");
    skiaMemoryTracer->dumpNumericValue("resourceName3", "size", "bytes", 1);
    skiaMemoryTracer->dumpStringValue("resourceName3", "type", "kIndex Buffer");
    DfxString log;
    skiaMemoryTracer->LogOutput(log);
    EXPECT_TRUE(skiaMemoryTracer->GetGLMemorySize() != -1);
}
} // namespace OHOS::Rosen