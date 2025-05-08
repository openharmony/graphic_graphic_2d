/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <fstream>
#include <gtest/gtest.h>

#include "drawing_error_code.h"
#include "drawing_memory_stream.h"

#include "utils/memory_stream.h"

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingMemoryStreamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeDrawingMemoryStreamTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}
void NativeDrawingMemoryStreamTest::TearDownTestCase() {}
void NativeDrawingMemoryStreamTest::SetUp() {}
void NativeDrawingMemoryStreamTest::TearDown() {}

/**
 * @tc.name: NativeDrawingCastToMemoryStreamTest001
 * @tc.desc: test for OH_Drawing_MemoryStreamCreate.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(NativeDrawingMemoryStreamTest, NativeDrawingMemoryStreamCreateTest001, TestSize.Level1)
{
    char data[10] = { 0 };
    OH_Drawing_MemoryStream* stream = OH_Drawing_MemoryStreamCreate(data, 10, false);
    ASSERT_TRUE(stream != nullptr);
    OH_Drawing_MemoryStreamCreate(nullptr, 0, false);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_MemoryStreamCreate(stream, 0, false);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
}

/**
 * @tc.name: NativeDrawingMemoryStreamDestroyTest001
 * @tc.desc: test for OH_Drawing_MemoryStreamCreate.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(NativeDrawingMemoryStreamTest, NativeDrawingMemoryStreamDestroyTest001, TestSize.Level1)
{
    char data[10] = { 0 };
    OH_Drawing_MemoryStream* stream = OH_Drawing_MemoryStreamCreate(data, 10, false);
    ASSERT_TRUE(stream != nullptr);
    OH_Drawing_MemoryStreamDestroy(stream);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
