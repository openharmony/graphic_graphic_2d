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

#include <gtest/gtest.h>

#include "platform/ohos/backend/rs_vulkan_context.cpp"
#include "platform/ohos/backend/rs_vulkan_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSVulkanContextTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSVulkanContextTest::SetUpTestCase() {}
void RSVulkanContextTest::TearDownTestCase() {}
void RSVulkanContextTest::SetUp() {}
void RSVulkanContextTest::TearDown() {}

/**
 * @tc.name: SetupLoaderProcAddresses001
 * @tc.desc: test results of SetupLoaderProcAddresses
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSVulkanContextTest, SetupLoaderProcAddresses001, TestSize.Level1)
{
    RsVulkanInterface rsVulkanInterface;
    EXPECT_TRUE(rsVulkanInterface.SetupLoaderProcAddresses());
    rsVulkanInterface.OpenLibraryHandle();
    EXPECT_TRUE(rsVulkanInterface.SetupLoaderProcAddresses());
}

/**
 * @tc.name: CreateInstance001
 * @tc.desc: test results of CreateInstance
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSVulkanContextTest, CreateInstance001, TestSize.Level1)
{
    RsVulkanInterface rsVulkanInterface;
    EXPECT_FALSE(rsVulkanInterface.CreateInstance());

    rsVulkanInterface.acquiredMandatoryProcAddresses_ = true;
    rsVulkanInterface.CreateInstance();
    EXPECT_TRUE(rsVulkanInterface.acquiredMandatoryProcAddresses_ == true);
}

/**
 * @tc.name: SelectPhysicalDevice001
 * @tc.desc: test results of SelectPhysicalDevice
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSVulkanContextTest, SelectPhysicalDevice001, TestSize.Level1)
{
    RsVulkanInterface rsVulkanInterface;
    auto ret = rsVulkanInterface.SelectPhysicalDevice(true);
    EXPECT_FALSE(ret);

    rsVulkanInterface.instance_ = (VkInstance)2;
    rsVulkanInterface.SelectPhysicalDevice(true);
    EXPECT_TRUE(rsVulkanInterface.instance_);
}

/**
 * @tc.name: CreateDevice001
 * @tc.desc: test results of CreateDevice
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSVulkanContextTest, CreateDevice001, TestSize.Level1)
{
    RsVulkanInterface rsVulkanInterface;
    rsVulkanInterface.CreateDevice(true);
    EXPECT_FALSE(rsVulkanInterface.physicalDevice_);

    rsVulkanInterface.physicalDevice_ = (VkPhysicalDevice)2;
    rsVulkanInterface.graphicsQueueFamilyIndex_ = UINT16_MAX;
    rsVulkanInterface.CreateDevice(true);
    EXPECT_TRUE(rsVulkanInterface.physicalDevice_);
}
/**
 * @tc.name: CreateSkiaBackendContext001
 * @tc.desc: test results of CreateSkiaBackendContext
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSVulkanContextTest, CreateSkiaBackendContext001, TestSize.Level1)
{
    RsVulkanInterface rsVulkanInterface;
    EXPECT_FALSE(rsVulkanInterface.CreateSkiaBackendContext(nullptr, true, true));

    rsVulkanInterface.instance_ = (VkInstance)2;
    rsVulkanInterface.device_ = (VkDevice)2;
    EXPECT_TRUE(rsVulkanInterface.CreateSkiaBackendContext(&rsVulkanInterface.backendContext_, true, true));
    EXPECT_TRUE(rsVulkanInterface.CreateSkiaBackendContext(&rsVulkanInterface.backendContext_, false, true));
}

/**
 * @tc.name: CloseLibraryHandle001
 * @tc.desc: test results of CloseLibraryHandle
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSVulkanContextTest, CloseLibraryHandle001, TestSize.Level1)
{
    RsVulkanInterface rsVulkanInterface;
    EXPECT_TRUE(rsVulkanInterface.CloseLibraryHandle());

    rsVulkanInterface.OpenLibraryHandle();
    EXPECT_TRUE(rsVulkanInterface.CloseLibraryHandle());
}

/**
 * @tc.name: AcquireProc001
 * @tc.desc: test results of AcquireProc
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSVulkanContextTest, AcquireProc001, TestSize.Level1)
{
    RsVulkanInterface rsVulkanInterface;
    VkInstance instance = VK_NULL_HANDLE;
    auto ret = rsVulkanInterface.AcquireProc(nullptr, instance);
    EXPECT_TRUE(ret == nullptr);

    ret = rsVulkanInterface.AcquireProc("text", instance);
    EXPECT_TRUE(ret != nullptr);

    VkDevice device = VK_NULL_HANDLE;
    ret = rsVulkanInterface.AcquireProc(nullptr, device);
    EXPECT_TRUE(ret == nullptr);

    device = (VkDevice)2;
    ret = rsVulkanInterface.AcquireProc("text", device);
    EXPECT_TRUE(ret == nullptr);
}
/**
 * @tc.name: CreateSkiaGetProc001
 * @tc.desc: test results of CreateSkiaGetProc
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSVulkanContextTest, CreateSkiaGetProc001, TestSize.Level1)
{
    RsVulkanInterface rsVulkanInterface;
    auto ret = rsVulkanInterface.CreateSkiaGetProc();
    EXPECT_TRUE(ret == nullptr);

    rsVulkanInterface.instance_ = (VkInstance)2;
    rsVulkanInterface.device_ = (VkDevice)2;
    ret = rsVulkanInterface.CreateSkiaGetProc();
    EXPECT_TRUE(ret != nullptr);
}

/**
 * @tc.name: CreateDrawingContext001
 * @tc.desc: test results of CreateDrawingContext
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSVulkanContextTest, CreateDrawingContext001, TestSize.Level1)
{
    RsVulkanInterface rsVulkanInterface;
    auto ret = rsVulkanInterface.CreateDrawingContext(true, true);
    EXPECT_TRUE(ret != nullptr);

    ret = rsVulkanInterface.CreateDrawingContext(false, true);
    EXPECT_TRUE(ret != nullptr);
}

/**
 * @tc.name: HookedVkQueueSubmit001
 * @tc.desc: test results of HookedVkQueueSubmit
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSVulkanContextTest, HookedVkQueueSubmit001, TestSize.Level1)
{
    RsVulkanInterface rsVulkanInterface;
    RsVulkanContext::isProtected_ = true;
    VkSubmitInfo pSubmits;
    RsVulkanContext::HookedVkQueueSubmit(VK_NULL_HANDLE, 1, &pSubmits, VK_NULL_HANDLE);
    EXPECT_TRUE(RsVulkanContext::isProtected_ == true);
}

/**
 * @tc.name: CreateDrawingContext002
 * @tc.desc: test results of CreateDrawingContext
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSVulkanContextTest, CreateDrawingContext002, TestSize.Level1)
{
    RsVulkanInterface rsVulkanInterface;
    auto ret = rsVulkanInterface.CreateDrawingContext(true);
    EXPECT_TRUE(ret != nullptr);

    RsVulkanContext::protectedDrawingContext_ = rsProtectedVulkanInterface.CreateDrawingContext(false, true);
    RsVulkanContext::drawingContext_ = rsVulkanInterface.CreateDrawingContext();
    ret = rsVulkanInterface.CreateDrawingContext(false);
    EXPECT_TRUE(ret != nullptr);
}

/**
 * @tc.name: GetDrawingContext001
 * @tc.desc: test results of GetDrawingContext
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSVulkanContextTest, GetDrawingContext001, TestSize.Level1)
{
    RsVulkanInterface rsVulkanInterface;
    auto ret = rsVulkanInterface.GetDrawingContext();
    EXPECT_TRUE(ret != nullptr);

    RsVulkanContext::protectedDrawingContext_ = rsProtectedVulkanInterface.CreateDrawingContext(false, true);
    RsVulkanContext::drawingContext_ = rsVulkanInterface.CreateDrawingContext();
    ret = rsVulkanInterface.GetDrawingContext();
    EXPECT_TRUE(ret != nullptr);
}
/**
 * @tc.name: SetIsProtected001
 * @tc.desc: test results of SetIsProtected
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSVulkanContextTest, SetIsProtected001, TestSize.Level1)
{
    RsVulkanContext rsVulkanContext;
    rsVulkanContext.SetIsProtected(true);
    EXPECT_TRUE(RsVulkanContext::isProtected_ == true);
}

} // namespace Rosen
} // namespace OHOS