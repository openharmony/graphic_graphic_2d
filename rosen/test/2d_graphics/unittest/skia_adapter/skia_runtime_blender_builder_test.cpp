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
#include "skia_adapter/skia_runtime_blender_builder.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaRuntimeBlenderBuilderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaRuntimeBlenderBuilderTest::SetUpTestCase() {}
void SkiaRuntimeBlenderBuilderTest::TearDownTestCase() {}
void SkiaRuntimeBlenderBuilderTest::SetUp() {}
void SkiaRuntimeBlenderBuilderTest::TearDown() {}

/**
 * @tc.name: MakeBlender001
 * @tc.desc: Test MakeBlender
 * @tc.type: FUNC
 * @tc.require: I91EQ7
 */
HWTEST_F(SkiaRuntimeBlenderBuilderTest, MakeBlender001, TestSize.Level1)
{
    SkiaRuntimeBlenderBuilder skiaRuntimeBlenderBuilder1;
    EXPECT_TRUE(!skiaRuntimeBlenderBuilder1.MakeBlender());
}

/**
 * @tc.name: SetUniform001
 * @tc.desc: Test SetUniform
 * @tc.type: FUNC
 * @tc.require: I91EQ7
 */
HWTEST_F(SkiaRuntimeBlenderBuilderTest, SetUniform001, TestSize.Level1)
{
    SkiaRuntimeBlenderBuilder skiaRuntimeBlenderBuilder1;
    skiaRuntimeBlenderBuilder1.SetUniform("uniform", 1.0f);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS