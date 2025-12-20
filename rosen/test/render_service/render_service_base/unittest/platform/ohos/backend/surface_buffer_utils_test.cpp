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

#include <gtest/gtest.h>

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include "platform/ohos/backend/surface_buffer_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class SurfaceBufferUtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SurfaceBufferUtilsTest::SetUpTestCase() {}
void SurfaceBufferUtilsTest::TearDownTestCase() {}
void SurfaceBufferUtilsTest::SetUp() {}
void SurfaceBufferUtilsTest::TearDown() {}

/**
 * @tc.name: CreateCanvasSurfaceBufferTest
 * @tc.desc: test results of CreateCanvasSurfaceBuffer
 * @tc.type:FUNC
 */
HWTEST_F(SurfaceBufferUtilsTest, CreateCanvasSurfaceBufferTest, TestSize.Level1)
{
    auto buffer = SurfaceBufferUtils::CreateCanvasSurfaceBuffer(1, 0, 0);
    ASSERT_EQ(buffer, nullptr);
    buffer = SurfaceBufferUtils::CreateCanvasSurfaceBuffer(1, 1, 0);
    ASSERT_EQ(buffer, nullptr);
    buffer = SurfaceBufferUtils::CreateCanvasSurfaceBuffer(1, 0, 1);
    ASSERT_EQ(buffer, nullptr);
    buffer = SurfaceBufferUtils::CreateCanvasSurfaceBuffer(1, 11000, 11000);
    ASSERT_EQ(buffer, nullptr);
    buffer = SurfaceBufferUtils::CreateCanvasSurfaceBuffer(1, 100, 100);
    ASSERT_NE(buffer, nullptr);
}

/**
 * @tc.name: ConvertSurfaceBufferToBackendTextureTest
 * @tc.desc: test results of ConvertSurfaceBufferToBackendTexture
 * @tc.type:FUNC
 */
HWTEST_F(SurfaceBufferUtilsTest, ConvertSurfaceBufferToBackendTextureTest, TestSize.Level1)
{
    sptr<SurfaceBuffer> buffer = nullptr;
    SurfaceBufferUtils::SetCanvasSurfaceBufferName(buffer, 1, 0, 0);
    auto texture = SurfaceBufferUtils::ConvertSurfaceBufferToBackendTexture(buffer);
    ASSERT_FALSE(texture.IsValid());
    buffer = SurfaceBuffer::Create();
    SurfaceBufferUtils::SetCanvasSurfaceBufferName(buffer, 1, 0, 0);
    SurfaceBufferUtils::SetCanvasSurfaceBufferName(buffer, 1, 100, 100);
    texture = SurfaceBufferUtils::ConvertSurfaceBufferToBackendTexture(buffer);
    ASSERT_FALSE(texture.IsValid());
    buffer = SurfaceBufferUtils::CreateCanvasSurfaceBuffer(1, 100, 100);
    texture = SurfaceBufferUtils::ConvertSurfaceBufferToBackendTexture(buffer);
    ASSERT_TRUE(texture.IsValid());
}
} // namespace OHOS::Rosen
#endif // ROSEN_OHOS && RS_ENABLE_VK