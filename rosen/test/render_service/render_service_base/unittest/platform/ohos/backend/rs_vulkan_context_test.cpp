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
 * @tc.name: RSVulkanContext001
 * @tc.desc: test results of init RSVulkanContext for uniRender
 * @tc.type:FUNC
 * @tc.require: issueIBWFN1
 */
HWTEST_F(RSVulkanContextTest, RSVulkanContext001, TestSize.Level1)
{
    RsVulkanContext::isHybridRender_ = false;
    RsVulkanContext context;
    ASSERT_NE(context.vulkanInterfaceVec_[size_t(VulkanInterfaceType::BASIC_RENDER)], nullptr);
}

/**
 * @tc.name: RSVulkanContext002
 * @tc.desc: test results of init RSVulkanContext for hybrid render
 * @tc.type:FUNC
 * @tc.require: issueIBWFN1
 */
HWTEST_F(RSVulkanContextTest, RSVulkanContext002, TestSize.Level1)
{
    RsVulkanContext::isHybridRender_ = true;
    RsVulkanContext context;
    ASSERT_NE(context.vulkanInterfaceVec_[size_t(VulkanInterfaceType::BASIC_RENDER)], nullptr);
}

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
}

/**
 * @tc.name: CreateInstance002
 * @tc.desc: test results of CreateInstance repeatedly
 * @tc.type:FUNC
 * @tc.require: issueIBWFN1
 */
HWTEST_F(RSVulkanContextTest, CreateInstance002, TestSize.Level1)
{
    RsVulkanInterface rsVulkanInterface;

    rsVulkanInterface.CreateInstance();
    auto instance1 =  rsVulkanInterface.instance_;

    rsVulkanInterface.CreateInstance();
    auto instance2 =  rsVulkanInterface.instance_;

    ASSERT_EQ(instance1, instance2);
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
    rsVulkanInterface.Init(VulkanInterfaceType::BASIC_RENDER, false);
    auto ret = rsVulkanInterface.SelectPhysicalDevice(true);
    EXPECT_TRUE(ret);
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
    EXPECT_FALSE(rsVulkanInterface.CreateSkiaBackendContext(nullptr, true));
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

    VkDevice device = VK_NULL_HANDLE;
    ret = rsVulkanInterface.AcquireProc(nullptr, device);
    EXPECT_TRUE(ret == nullptr);

    rsVulkanInterface.OpenLibraryHandle();
    EXPECT_TRUE(rsVulkanInterface.SetupLoaderProcAddresses());
    ret = rsVulkanInterface.AcquireProc("text", instance);
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

    rsVulkanInterface.Init(VulkanInterfaceType::BASIC_RENDER);
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
    auto ret = rsVulkanInterface.CreateDrawingContext();
    EXPECT_TRUE(ret != nullptr);
}

/**
 * @tc.name: GetRecyclableSingleton001
 * @tc.desc: test GetRecyclableSingleton
 * @tc.type:FUNC
 * @tc.require: issueIC3PRG
 */
HWTEST_F(RSVulkanContextTest, GetRecyclableSingleton001, TestSize.Level2)
{
    RsVulkanContext::SetRecyclable(true);
    RsVulkanContext::GetRecyclableSingleton();
    ASSERT_NE(RsVulkanContext::recyclableSingleton_, nullptr);

    // reset recyclable singleton
    RsVulkanContext::ReleaseRecyclableSingleton();
}

/**
 * @tc.name: GetRecyclableSingleton002
 * @tc.desc: test GetRecyclableSingleton repeatedly
 * @tc.type:FUNC
 * @tc.require: issueIC3PRG
 */
HWTEST_F(RSVulkanContextTest, GetRecyclableSingleton002, TestSize.Level2)
{
    RsVulkanContext::SetRecyclable(true);

    // get recyclable singleton repeatedly
    RsVulkanContext::GetRecyclableSingleton();
    RsVulkanContext::GetRecyclableSingleton();
    ASSERT_NE(RsVulkanContext::recyclableSingleton_, nullptr);
    
    // reset recyclable singleton
    RsVulkanContext::ReleaseRecyclableSingleton();
}

/**
 * @tc.name: ReleaseRecyclableSingleton001
 * @tc.desc: test ReleaseRecyclableSingleton while vulkan context is recyclable
 * @tc.type:FUNC
 * @tc.require: issueIC3PRG
 */
HWTEST_F(RSVulkanContextTest, ReleaseRecyclableSingleton001, TestSize.Level2)
{
    RsVulkanContext::SetRecyclable(true);

    RsVulkanContext::GetRecyclableSingleton();
    RsVulkanContext::ReleaseRecyclableSingleton();
    ASSERT_EQ(RsVulkanContext::recyclableSingleton_, nullptr);
}


/**
 * @tc.name: ReleaseRecyclableSingleton002
 * @tc.desc: test ReleaseRecyclableSingleton while vulkan context isn't recyclable
 * @tc.type:FUNC
 * @tc.require: issueIC3PRG
 */
HWTEST_F(RSVulkanContextTest, ReleaseRecyclableSingleton002, TestSize.Level2)
{
    RsVulkanContext::SetRecyclable(true);
    RsVulkanContext::GetRecyclableSingleton();

    RsVulkanContext::SetRecyclable(false);
    RsVulkanContext::ReleaseRecyclableSingleton();
    ASSERT_NE(RsVulkanContext::recyclableSingleton_, nullptr);

    // restore
    RsVulkanContext::SetRecyclable(true);
    RsVulkanContext::ReleaseRecyclableSingleton();
    RsVulkanContext::SetRecyclable(false);
}

/**
 * @tc.name: SaveNewDrawingContext001
 * @tc.desc: test SaveNewDrawingContext
 * @tc.type:FUNC
 * @tc.require: issueIC3PRG
 */
HWTEST_F(RSVulkanContextTest, SaveNewDrawingContext001, TestSize.Level2)
{
    auto gpuContext = std::make_shared<Drawing::GPUContext>();
    RsVulkanContext::SaveNewDrawingContext(gettid(), gpuContext);
    ASSERT_FALSE(RsVulkanContext::drawingContextMap_.empty());
}

/**
 * @tc.name: CleanUpRecyclableDrawingContext001
 * @tc.desc: test CleanUpRecyclableDrawingContext
 * @tc.type:FUNC
 * @tc.require: issueIC3PRG
 */
HWTEST_F(RSVulkanContextTest, CleanUpRecyclableDrawingContext001, TestSize.Level2)
{
    auto gpuContext = std::make_shared<Drawing::GPUContext>();
    RsVulkanContext::SaveNewDrawingContext(gettid(), gpuContext);

    RsVulkanContext::CleanUpRecyclableDrawingContext(gettid());
    ASSERT_TRUE(RsVulkanContext::drawingContextMap_.empty());
}

/**
 * @tc.name: SetAndGetRecyclable
 * @tc.desc: test set and get recyclable
 * @tc.type:FUNC
 * @tc.require: issue
 */
HWTEST_F(RSVulkanContextTest, SetAndGetRecyclable, TestSize.Level2)
{
    RsVulkanContext::SetRecyclable(false);
    ASSERT_FALSE(RsVulkanContext::IsRecyclable());
}
} // namespace Rosen
} // namespace OHOS