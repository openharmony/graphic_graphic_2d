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

#include "gtest/gtest.h"

#include "render/rs_resource_manager.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class RSResourceManagerUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSResourceManagerUnitTest::SetUpTestCase() {}
void RSResourceManagerUnitTest::TearDownTestCase() {}
void RSResourceManagerUnitTest::SetUp() {}
void RSResourceManagerUnitTest::TearDown() {}

static bool g_uploadFunctionCalled = false;
static void TestHookFunction(bool paraUpload, const std::shared_ptr<Drawing::Image>& image,
    const std::shared_ptr<Media::PixelMap>& pixelMap, uint64_t uniqueId)
{
    g_uploadFunctionCalled = true;
}
/**
 * @tc.name: TestInstance01
 * @tc.desc: Verify function Instance
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSResourceManagerUnitTest, TestInstance01, TestSize.Level1)
{
    EXPECT_EQ(RSResourceManager::Instance().hookFunction_, nullptr);
}

/**
 * @tc.name: TestUploadTexture01
 * @tc.desc: Verify function UploadTexture
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSResourceManagerUnitTest, TestUploadTexture01, TestSize.Level1)
{
    auto testManager = std::make_shared<RSResourceManager>();
    auto image = std::make_shared<Drawing::Image>();
    auto testPixelMap = std::make_shared<Media::PixelMap>();
    testManager->UploadTexture(true, image, testPixelMap, 1);
    EXPECT_FALSE(g_uploadFunctionCalled);
    testManager->SetUploadTextureFunction(TestHookFunction);
    testManager->UploadTexture(true, image, testPixelMap, 1);
    EXPECT_TRUE(g_uploadFunctionCalled);
    g_uploadFunctionCalled = false;
}

/**
 * @tc.name: TestSetUploadTextureFunction01
 * @tc.desc: Verify function SetUploadTextureFunction
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSResourceManagerUnitTest, TestSetUploadTextureFunction01, TestSize.Level1)
{
    auto testManager = std::make_shared<RSResourceManager>();
    auto image = std::make_shared<Drawing::Image>();
    auto testPixelMap = std::make_shared<Media::PixelMap>();
    testManager->SetUploadTextureFunction(nullptr);
    testManager->UploadTexture(true, image, testPixelMap, 1);
    EXPECT_FALSE(g_uploadFunctionCalled);
    testManager->SetUploadTextureFunction(TestHookFunction);
    testManager->UploadTexture(true, image, testPixelMap, 1);
    EXPECT_TRUE(g_uploadFunctionCalled);
}

} // namespace Rosen
} // namespace OHOS