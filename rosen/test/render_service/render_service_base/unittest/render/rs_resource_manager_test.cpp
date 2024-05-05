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

class RSResourceManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSResourceManagerTest::SetUpTestCase() {}
void RSResourceManagerTest::TearDownTestCase() {}
void RSResourceManagerTest::SetUp() {}
void RSResourceManagerTest::TearDown() {}

static bool g_uploadFunctionCalled = false;
static void TestHookFunction(bool paraUpload, const std::shared_ptr<Drawing::Image>& image,
    const std::shared_ptr<Media::PixelMap>& pixelMap, uint64_t uniqueId)
{
    g_uploadFunctionCalled = true;
}
/**
 * @tc.name: InstanceTest001
 * @tc.desc: Verify function Instance
 * @tc.type:FUNC
 */
HWTEST_F(RSResourceManagerTest, InstanceTest001, TestSize.Level1)
{
    EXPECT_EQ(RSResourceManager::Instance().hookFunction_, nullptr);
}

/**
 * @tc.name: UploadTextureTest001
 * @tc.desc: Verify function UploadTexture
 * @tc.type:FUNC
 */
HWTEST_F(RSResourceManagerTest, UploadTextureTest001, TestSize.Level1)
{
    auto resourceManager = std::make_shared<RSResourceManager>();
    auto image = std::make_shared<Drawing::Image>();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    resourceManager->UploadTexture(true, image, pixelMap, 1);
    EXPECT_FALSE(g_uploadFunctionCalled);
    resourceManager->SetUploadTextureFunction(TestHookFunction);
    resourceManager->UploadTexture(true, image, pixelMap, 1);
    EXPECT_TRUE(g_uploadFunctionCalled);
    g_uploadFunctionCalled = false;
}

/**
 * @tc.name: SetUploadTextureFunctionTest001
 * @tc.desc: Verify function SetUploadTextureFunction
 * @tc.type:FUNC
 */
HWTEST_F(RSResourceManagerTest, SetUploadTextureFunctionTest001, TestSize.Level1)
{
    auto resourceManager = std::make_shared<RSResourceManager>();
    auto image = std::make_shared<Drawing::Image>();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    resourceManager->SetUploadTextureFunction(nullptr);
    resourceManager->UploadTexture(true, image, pixelMap, 1);
    EXPECT_FALSE(g_uploadFunctionCalled);
    resourceManager->SetUploadTextureFunction(TestHookFunction);
    resourceManager->UploadTexture(true, image, pixelMap, 1);
    EXPECT_TRUE(g_uploadFunctionCalled);
}

} // namespace Rosen
} // namespace OHOS