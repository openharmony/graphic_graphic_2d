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
#include "foundation/graphic/graphic_2d/rosen/modules/render_service_base/include/memory/rs_dfx_string.h"
#include "third_party/skia/include/gpu/GrDirectContext.h"

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

/*
 * @tc.name: dumpNumericValueTest
 * @tc.desc: dumpNumericValue Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSkiaMemoryTracerTest, dumpNumericValueTest, testing::ext::TestSize.Level1)
{
    bool itemizeType = true;
    SkiaMemoryTracer skiaMemoryTracer("category", itemizeType);
    uint64_t value = 1;
    skiaMemoryTracer.dumpNumericValue("dumpName", "valueName", "units", value);
}

/*
 * @tc.name: LogOutputTest
 * @tc.desc: LogOutput Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSkiaMemoryTracerTest, LogOutputTest, testing::ext::TestSize.Level1)
{
    bool itemizeType = false;
    SkiaMemoryTracer skiaMemoryTracer("category", itemizeType);
    DfxString log;
    skiaMemoryTracer.LogOutput(log);
}
} // namespace OHOS::Rosen