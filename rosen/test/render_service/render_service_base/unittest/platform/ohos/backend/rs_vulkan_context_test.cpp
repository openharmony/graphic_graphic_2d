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

static std::vector<const char*> gMandatoryDeviceExtensions = {
    VK_KHR_EXTERNAL_SEMAPHORE_FD_EXTENSION_NAME,
    VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME,
    VK_KHR_MAINTENANCE1_EXTENSION_NAME,
    VK_KHR_MAINTENANCE2_EXTENSION_NAME,
    VK_KHR_MAINTENANCE3_EXTENSION_NAME,
    VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
    VK_KHR_BIND_MEMORY_2_EXTENSION_NAME,
    VK_OHOS_NATIVE_BUFFER_EXTENSION_NAME,
    VK_OHOS_EXTERNAL_MEMORY_EXTENSION_NAME,
};

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
 * @tc.name: SelectPhysicalDevice001
 * @tc.desc: test results of SelectPhysicalDevice
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSVulkanContextTest, SelectPhysicalDevice001, TestSize.Level1)
{
    RsVulkanInterface rsVulkanInterface;
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
 * @tc.desc: test results of CreateDrawingContext while vulkanInterfaceType_ is BASIC_RENDER
 * @tc.type:FUNC
 * @tc.require: issueICDVVY
 */
HWTEST_F(RSVulkanContextTest, CreateDrawingContext001, TestSize.Level2)
{
    RsVulkanContext::GetSingleton().InitVulkanContextForUniRender("");
    RsVulkanContext::drawingContextMap_.clear();
    ASSERT_NE(RsVulkanContext::GetSingleton().CreateDrawingContext(), nullptr);
}

/**
 * @tc.name: CreateDrawingContext002
 * @tc.desc: test results of CreateDrawingContext while drawingContextMap_ contain nullptr
 * @tc.type:FUNC
 * @tc.require: issueICDVVY
 */
HWTEST_F(RSVulkanContextTest, CreateDrawingContext002, TestSize.Level2)
{
    RsVulkanContext::GetSingleton().InitVulkanContextForUniRender("");
    DrawingContextProperty property;
    RsVulkanContext::drawingContextMap_[gettid()] = property;
    ASSERT_NE(RsVulkanContext::GetSingleton().CreateDrawingContext(), nullptr);
}

/**
 * @tc.name: CreateDrawingContext003
 * @tc.desc: test results of CreateDrawingContext
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSVulkanContextTest, CreateDrawingContext003, TestSize.Level2)
{
    RsVulkanInterface rsVulkanInterface;
    auto ret = rsVulkanInterface.CreateDrawingContext();
    EXPECT_TRUE(ret != nullptr);
}

/**
 * @tc.name: GetRecyclableSingleton001
 * @tc.desc: test GetRecyclableSingleton
 * @tc.type:FUNC
 * @tc.require: issueICB7BS
 */
HWTEST_F(RSVulkanContextTest, GetRecyclableSingleton001, TestSize.Level2)
{
    RsVulkanContext::SetRecyclable(true);
    (void)RsVulkanContext::ReleaseRecyclableSingleton();
    ASSERT_NE(&RsVulkanContext::GetRecyclableSingleton(), nullptr);
    // reset recyclable singleton
    RsVulkanContext::ReleaseRecyclableSingleton();
    RsVulkanContext::SetRecyclable(false);
}

/**
 * @tc.name: GetRecyclableSingletonPtr001
 * @tc.desc: test GetRecyclableSingletonPtr when called for the first time
 * @tc.type:FUNC
 * @tc.require: issueICB7BS
 */
HWTEST_F(RSVulkanContextTest, GetRecyclableSingletonPtr001, TestSize.Level2)
{
    RsVulkanContext::SetRecyclable(true);
    RsVulkanContext::ReleaseRecyclableSingleton();
    (void)RsVulkanContext::GetRecyclableSingleton();
    ASSERT_NE(RsVulkanContext::GetRecyclableSingletonPtr(), nullptr);

    // reset recyclable singleton
    RsVulkanContext::ReleaseRecyclableSingleton();
    RsVulkanContext::SetRecyclable(false);
}

/**
 * @tc.name: GetRecyclableSingletonPtr002
 * @tc.desc: test GetRecyclableSingletonPtr when singleton already init
 * @tc.type:FUNC
 * @tc.require: issueICB7BS
 */
HWTEST_F(RSVulkanContextTest, GetRecyclableSingletonPtr002, TestSize.Level2)
{
    RsVulkanContext::SetRecyclable(true);
    RsVulkanContext::ReleaseRecyclableSingleton();
    // GetRecyclableSingletonPtr return the same singleton
    (void)RsVulkanContext::GetRecyclableSingleton();
    ASSERT_NE(RsVulkanContext::GetRecyclableSingletonPtr(), nullptr);

    // reset recyclable singleton
    RsVulkanContext::ReleaseRecyclableSingleton();
    RsVulkanContext::SetRecyclable(false);
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
    ASSERT_FALSE(RsVulkanContext::drawingContextMap_.empty());

    // restore
    RsVulkanContext::SetRecyclable(false);
}


/**
 * @tc.name: ReleaseRecyclableSingleton002
 * @tc.desc: test ReleaseRecyclableSingleton while vulkan context isn't recyclable
 * @tc.type:FUNC
 * @tc.require: issueIC3PRG
 */
HWTEST_F(RSVulkanContextTest, ReleaseRecyclableSingleton002, TestSize.Level2)
{
    RsVulkanContext::SetRecyclable(false);
    RsVulkanContext::GetSingleton();
    RsVulkanContext::ReleaseRecyclableSingleton();
    ASSERT_FALSE(RsVulkanContext::drawingContextMap_.empty());
}

/**
 * @tc.name: ReleaseRecyclableSingleton003
 * @tc.desc: test ReleaseRecyclableSingleton while drawingContextMap/protectedDrawingContextMap contain nullptr
 * @tc.type:FUNC
 * @tc.require: issuesIC7U3T
 */
HWTEST_F(RSVulkanContextTest, ReleaseRecyclableSingleton003, TestSize.Level2)
{
    RsVulkanContext::SetRecyclable(true);
    RsVulkanContext::GetRecyclableSingleton();

    DrawingContextProperty property;
    RsVulkanContext::drawingContextMap_[gettid()] = property;

    RsVulkanContext::ReleaseRecyclableSingleton();
    ASSERT_TRUE(RsVulkanContext::drawingContextMap_.empty());

    // restore
    RsVulkanContext::SetRecyclable(false);
}

/**
 * @tc.name: ReleaseRecyclableSingleton004
 * @tc.desc: test ReleaseRecyclableSingleton while drawingContextMap/protectedDrawingContextMap isn't empty
 * @tc.type:FUNC
 * @tc.require: issuesIC7U3T
 */
HWTEST_F(RSVulkanContextTest, ReleaseRecyclableSingleton004, TestSize.Level2)
{
    RsVulkanContext::SetRecyclable(true);
    RsVulkanContext::GetRecyclableSingleton();

    auto drawingContext = std::make_shared<Drawing::GPUContext>();
    auto protectDrawingContext = std::make_shared<Drawing::GPUContext>();
    DrawingContextProperty property;
    property.unprotectContext = drawingContext;
    property.protectContext = protectDrawingContext;

    RsVulkanContext::drawingContextMap_[gettid()] = property;

    RsVulkanContext::ReleaseRecyclableSingleton();
    ASSERT_FALSE(RsVulkanContext::drawingContextMap_.empty());

    // restore
    RsVulkanContext::SetRecyclable(false);
}

/**
 * @tc.name: ReleaseRecyclableSingleton005
 * @tc.desc: test ReleaseRecyclableSingleton while drawingContextMap/protectedDrawingContextMap is empty
 * @tc.type:FUNC
 * @tc.require: issuesIC7U3T
 */
HWTEST_F(RSVulkanContextTest, ReleaseRecyclableSingleton005, TestSize.Level2)
{
    RsVulkanContext::SetRecyclable(true);
    RsVulkanContext::GetRecyclableSingleton();

    RsVulkanContext::drawingContextMap_.clear();

    RsVulkanContext::ReleaseRecyclableSingleton();
    ASSERT_TRUE(RsVulkanContext::drawingContextMap_.empty());

    // restore
    RsVulkanContext::SetRecyclable(false);
}

/**
 * @tc.name: GetRecyclableDrawingContext001
 * @tc.desc: test GetRecyclableDrawingContext for protected context
 * @tc.type:FUNC
 * @tc.require: issueICB7BS
 */
HWTEST_F(RSVulkanContextTest, GetRecyclableDrawingContext001, TestSize.Level2)
{
    RsVulkanContext::isProtected_ = true;
    auto context = RsVulkanContext::GetSingleton().GetRecyclableDrawingContext();
    ASSERT_NE(context, nullptr);

    // restore
    RsVulkanContext::isProtected_ = false;
}

/**
 * @tc.name: GetRecyclableDrawingContext002
 * @tc.desc: test GetRecyclableDrawingContext for unprotected context
 * @tc.type:FUNC
 * @tc.require: issueICB7BS
 */
HWTEST_F(RSVulkanContextTest, GetRecyclableDrawingContext002, TestSize.Level2)
{
    RsVulkanContext::isProtected_ = false;
    auto context = RsVulkanContext::GetSingleton().GetRecyclableDrawingContext();

    ASSERT_NE(context, nullptr);
}

/**
 * @tc.name: GetRecyclableDrawingContext003
 * @tc.desc: test GetRecyclableDrawingContext while drawingContextMap_ is empty
 * @tc.type:FUNC
 * @tc.require: issueICB7BS
 */
HWTEST_F(RSVulkanContextTest, GetRecyclableDrawingContext003, TestSize.Level2)
{
    RsVulkanContext::isProtected_ = false;
    auto drawingContext = RsVulkanContext::GetSingleton().CreateDrawingContext();
    ASSERT_NE(drawingContext, nullptr);
    RsVulkanContext::ReleaseDrawingContextMap();

    auto recyclableContext = RsVulkanContext::GetSingleton().GetRecyclableDrawingContext();
    ASSERT_NE(recyclableContext, nullptr);
}

/**
 * @tc.name: ReleaseRecyclableDrawingContext002
 * @tc.desc: test ReleaseRecyclableDrawingContext while drawingContextMap is empty
 * @tc.type:FUNC
 * @tc.require: issueICB7BS
 */
HWTEST_F(RSVulkanContextTest, ReleaseRecyclableDrawingContext002, TestSize.Level2)
{
    RsVulkanContext::isProtected_ = false;
    RsVulkanContext::GetSingleton();
    RsVulkanContext::drawingContextMap_.clear();
    RsVulkanContext::ReleaseRecyclableDrawingContext();
    ASSERT_TRUE(RsVulkanContext::drawingContextMap_.empty());
}

/**
 * @tc.name: ReleaseRecyclableDrawingContext003
 * @tc.desc: test ReleaseRecyclableDrawingContext while drawingContextMap isn't empty
 * @tc.type:FUNC
 * @tc.require: issueICB7BS
 */
HWTEST_F(RSVulkanContextTest, ReleaseRecyclableDrawingContext003, TestSize.Level2)
{
    RsVulkanContext::isProtected_ = false;
    auto drawingContext = std::make_shared<Drawing::GPUContext>();
    DrawingContextProperty property;
    property.unprotectContext = drawingContext;
    property.unprotectRecyclable = true;
    RsVulkanContext::drawingContextMap_[gettid()] = property;

    RsVulkanContext::ReleaseRecyclableDrawingContext();
    ASSERT_TRUE(RsVulkanContext::drawingContextMap_.empty());
}

/**
 * @tc.name: ReleaseRecyclableDrawingContext004
 * @tc.desc: test ReleaseRecyclableDrawingContext while drawingContextMap isn't recyclable
 * @tc.type:FUNC
 * @tc.require: issueICB7BS
 */
HWTEST_F(RSVulkanContextTest, ReleaseRecyclableDrawingContext004, TestSize.Level2)
{
    RsVulkanContext::isProtected_ = false;
    auto drawingContext = std::make_shared<Drawing::GPUContext>();
    DrawingContextProperty property;
    property.unprotectContext = drawingContext;
    RsVulkanContext::drawingContextMap_[gettid()] = property;

    RsVulkanContext::ReleaseRecyclableDrawingContext();
    ASSERT_FALSE(RsVulkanContext::drawingContextMap_.empty());
}

/**
 * @tc.name: SaveNewDrawingContext001
 * @tc.desc: test SaveNewDrawingContext for protected context
 * @tc.type:FUNC
 * @tc.require: issueICB7BS
 */
HWTEST_F(RSVulkanContextTest, SaveNewDrawingContext001, TestSize.Level2)
{
    RsVulkanContext::isProtected_ = true;
    auto gpuContext = std::make_shared<Drawing::GPUContext>();
    RsVulkanContext::SaveNewDrawingContext(gettid(), gpuContext);
    ASSERT_FALSE(RsVulkanContext::drawingContextMap_.empty());

    // restore
    RsVulkanContext::isProtected_ = false;
}

/**
 * @tc.name: SaveNewDrawingContext002
 * @tc.desc: test SaveNewDrawingContext for unprotected context
 * @tc.type:FUNC
 * @tc.require: issueICB7BS
 */
HWTEST_F(RSVulkanContextTest, SaveNewDrawingContext002, TestSize.Level2)
{
    RsVulkanContext::isProtected_ = false;
    auto gpuContext = std::make_shared<Drawing::GPUContext>();
    RsVulkanContext::SaveNewDrawingContext(gettid(), gpuContext);
    ASSERT_FALSE(RsVulkanContext::drawingContextMap_.empty());
}

/**
 * @tc.name: ReleaseDrawingContextForThread001
 * @tc.desc: test ReleaseDrawingContextForThread
 * @tc.type:FUNC
 * @tc.require: issueIC3PRG
 */
HWTEST_F(RSVulkanContextTest, ReleaseDrawingContextForThread001, TestSize.Level2)
{
    auto gpuContext = std::make_shared<Drawing::GPUContext>();
    RsVulkanContext::SaveNewDrawingContext(gettid(), gpuContext);

    RsVulkanContext::ReleaseDrawingContextForThread(gettid());
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

/**
 * @tc.name: IsRecyclableSingletonValid
 * @tc.desc: test IsRecyclableSingletonValid
 * @tc.type:FUNC
 * @tc.require: issueICD3VN
 */
HWTEST_F(RSVulkanContextTest, IsRecyclableSingletonValid, TestSize.Level2)
{
    RsVulkanContext::ReleaseRecyclableDrawingContext();
    ASSERT_FALSE(RsVulkanContext::IsRecyclableSingletonValid());
}

/**
 * @tc.name: RSVulkanContextDestruction
 * @tc.desc: Test RSVulkanContext destruction
 * @tc.type:FUNC
 * @tc.require:issuesIC7U3T
*/
HWTEST_F(RSVulkanContextTest, RSVulkanContextDestruction, TestSize.Level2)
{
    // create recyclable vulkan context
    RsVulkanContext::SetRecyclable(true);
    (void)RsVulkanContext::GetRecyclableSingleton();
    ASSERT_NE(RsVulkanContext::GetRecyclableSingletonPtr(), nullptr);

    RsVulkanContext::ReleaseRecyclableSingleton();
    ASSERT_FALSE(RsVulkanContext::drawingContextMap_.empty());

    // restore
    RsVulkanContext::SetRecyclable(false);
}

/**
 * @tc.name: ConfigureFeatures_Test_00
 * @tc.desc: Test ConfigureFeatures
 * @tc.type:FUNC
 * @tc.require:issuesIC7U3T
*/
HWTEST_F(RSVulkanContextTest, ConfigureFeatures_Test_00, TestSize.Level2)
{
    RsVulkanInterface testInterface;
    testInterface.ConfigureFeatures(true);
    ASSERT_TRUE(testInterface.ycbcrFeature_.pNext == testInterface.protectedMemoryFeatures_);
}

/**
 * @tc.name: ConfigureFeatures_Test_01
 * @tc.desc: Test ConfigureFeatures
 * @tc.type:FUNC
 * @tc.require:issuesIC7U3T
*/
HWTEST_F(RSVulkanContextTest, ConfigureFeatures_Test_01, TestSize.Level2)
{
    RsVulkanInterface testInterface;
    testInterface.ConfigureFeatures(false);
    ASSERT_TRUE(testInterface.ycbcrFeature_.pNext == nullptr);
}

/**
 * @tc.name: ConfigureExtensions_Test_00
 * @tc.desc: Test ConfigureExtensions
 * @tc.type:FUNC
 * @tc.require:issuesIC7U3T
*/
HWTEST_F(RSVulkanContextTest, ConfigureExtensions_Test_00, TestSize.Level2)
{
    RsVulkanInterface testInterface;
    // mock missing mandatory ext
    testInterface.vkEnumerateDeviceExtensionProperties =
        [](VkPhysicalDevice device, const char*, uint32_t* count, VkExtensionProperties* props) {
        *count = 0;
        return VK_SUCCESS;
    };
    testInterface.ConfigureExtensions();
    ASSERT_EQ(testInterface.deviceExtensions_.size(), gMandatoryDeviceExtensions.size());
    for (const auto& ext:testInterface.deviceExtensions_) {
        ASSERT_NE(std::find(gMandatoryDeviceExtensions.begin(), gMandatoryDeviceExtensions.end(), ext),
                    gMandatoryDeviceExtensions.end());
    }
}

/**
 * @tc.name: ConfigureExtensions_Test_01
 * @tc.desc: Test ConfigureExtensions
 * @tc.type:FUNC
 * @tc.require:issuesIC7U3T
*/
HWTEST_F(RSVulkanContextTest, ConfigureExtensions_Test_01, TestSize.Level2)
{
    RsVulkanInterface testInterface;
    // mock missing mandatory ext
    testInterface.vkEnumerateDeviceExtensionProperties =
        [](VkPhysicalDevice device, const char*, uint32_t* count, VkExtensionProperties* props) {
        *count = 0;
        return VK_ERROR_UNKNOWN;
    };
    testInterface.ConfigureExtensions();
    ASSERT_EQ(testInterface.deviceExtensions_.size(), gMandatoryDeviceExtensions.size());
    for (const auto& ext:testInterface.deviceExtensions_) {
        ASSERT_NE(std::find(gMandatoryDeviceExtensions.begin(), gMandatoryDeviceExtensions.end(), ext),
                    gMandatoryDeviceExtensions.end());
    }
}

/**
 * @tc.name: ConfigureExtensions_Test_02
 * @tc.desc: Test ConfigureExtensions
 * @tc.type:FUNC
 * @tc.require:issuesIC7U3T
*/
HWTEST_F(RSVulkanContextTest, ConfigureExtensions_Test_02, TestSize.Level2)
{
    RsVulkanInterface testInterface;
    // mock missing mandatory ext
    testInterface.vkEnumerateDeviceExtensionProperties =
        [](VkPhysicalDevice device, const char*, uint32_t* count, VkExtensionProperties* props) {
        if (props != nullptr) {
            return VK_ERROR_UNKNOWN;
        } else {
            *count = 0;
            return VK_SUCCESS;
        }
    };
    testInterface.ConfigureExtensions();
    ASSERT_EQ(testInterface.deviceExtensions_.size(), gMandatoryDeviceExtensions.size());
    for (const auto& ext:testInterface.deviceExtensions_) {
        ASSERT_NE(std::find(gMandatoryDeviceExtensions.begin(), gMandatoryDeviceExtensions.end(), ext),
                    gMandatoryDeviceExtensions.end());
    }
}

/**
 * @tc.name: GetDrawingContext001
 * @tc.desc: test results of GetDrawingContext while drawingContextMap_ is empty
 * @tc.type:FUNC
 * @tc.require: issueICDVVY
 */
HWTEST_F(RSVulkanContextTest, GetDrawingContext001, TestSize.Level2)
{
    RsVulkanContext::GetSingleton().InitVulkanContextForUniRender("");

    RsVulkanContext::ReleaseDrawingContextMap();
    ASSERT_NE(RsVulkanContext::GetSingleton().GetDrawingContext(), nullptr);

    // restore
    RsVulkanContext::ReleaseDrawingContextMap();
}

/**
 * @tc.name: GetDrawingContext002
 * @tc.desc: test results of GetDrawingContext while protectedDrawingContextMap_ is empty
 * @tc.type:FUNC
 * @tc.require: issueICDVVY
 */
HWTEST_F(RSVulkanContextTest, GetDrawingContext002, TestSize.Level2)
{
    RsVulkanContext::GetSingleton().InitVulkanContextForUniRender("");
    RsVulkanContext::GetSingleton().SetIsProtected(true);

    RsVulkanContext::ReleaseDrawingContextMap();
    ASSERT_NE(RsVulkanContext::GetSingleton().GetDrawingContext(), nullptr);

    // restore
    RsVulkanContext::ReleaseDrawingContextMap();
}

/**
 * @tc.name: GetDrawingContext003
 * @tc.desc: test results of GetDrawingContext while protectedDrawingContext is nullptr
 * @tc.type:FUNC
 * @tc.require: issueICDVVY
 */
HWTEST_F(RSVulkanContextTest, GetDrawingContext003, TestSize.Level2)
{
    RsVulkanContext::GetSingleton().InitVulkanContextForUniRender("");
    RsVulkanContext::GetSingleton().SetIsProtected(true);
    
    RsVulkanContext::SaveNewDrawingContext(gettid(), nullptr);
    ASSERT_NE(RsVulkanContext::GetSingleton().GetDrawingContext(), nullptr);

    // restore
    RsVulkanContext::ReleaseDrawingContextMap();
}

/**
 * @tc.name: GetDrawingContext004
 * @tc.desc: test results of GetDrawingContext while protectedDrawingContext isn't nullptr
 * @tc.type:FUNC
 * @tc.require: issueICDVVY
 */
HWTEST_F(RSVulkanContextTest, GetDrawingContext004, TestSize.Level2)
{
    RsVulkanContext::GetSingleton().InitVulkanContextForUniRender("");
    RsVulkanContext::GetSingleton().SetIsProtected(true);
    
    auto protectedDrawingContext = std::make_shared<Drawing::GPUContext>();
    RsVulkanContext::SaveNewDrawingContext(gettid(), protectedDrawingContext);
    ASSERT_NE(RsVulkanContext::GetSingleton().GetDrawingContext(), nullptr);

    // restore
    RsVulkanContext::ReleaseDrawingContextMap();
}

/**
 * @tc.name: CreateDrawingContext004
 * @tc.desc: test results of CreateDrawingContext while protectedDrawingContextMap_ is empty
 * @tc.type:FUNC
 * @tc.require: issueICDVVY
 */
HWTEST_F(RSVulkanContextTest, CreateDrawingContext004, TestSize.Level2)
{
    RsVulkanContext::GetSingleton().InitVulkanContextForUniRender("");
    RsVulkanContext::GetSingleton().SetIsProtected(true);

    RsVulkanContext::ReleaseDrawingContextMap();
    ASSERT_NE(RsVulkanContext::GetSingleton().CreateDrawingContext(), nullptr);

    // restore
    RsVulkanContext::ReleaseDrawingContextMap();
}

/**
 * @tc.name: CreateDrawingContext005
 * @tc.desc: test results of CreateDrawingContext while protectedDrawingContext is nullptr
 * @tc.type:FUNC
 * @tc.require: issueICDVVY
 */
HWTEST_F(RSVulkanContextTest, CreateDrawingContext005, TestSize.Level2)
{
    RsVulkanContext::GetSingleton().InitVulkanContextForUniRender("");
    RsVulkanContext::GetSingleton().SetIsProtected(true);
    
    RsVulkanContext::SaveNewDrawingContext(gettid(), nullptr);
    ASSERT_NE(RsVulkanContext::GetSingleton().CreateDrawingContext(), nullptr);

    // restore
    RsVulkanContext::ReleaseDrawingContextMap();
}

/**
 * @tc.name: CreateDrawingContext006
 * @tc.desc: test results of CreateDrawingContext while protectedDrawingContext isn't nullptr
 * @tc.type:FUNC
 * @tc.require: issueICDVVY
 */
HWTEST_F(RSVulkanContextTest, CreateDrawingContext006, TestSize.Level2)
{
    RsVulkanContext::GetSingleton().InitVulkanContextForUniRender("");
    RsVulkanContext::GetSingleton().SetIsProtected(true);
    
    auto protectedDrawingContext = std::make_shared<Drawing::GPUContext>();
    RsVulkanContext::SaveNewDrawingContext(gettid(), protectedDrawingContext);
    ASSERT_NE(RsVulkanContext::GetSingleton().CreateDrawingContext(), nullptr);

    // restore
    RsVulkanContext::ReleaseDrawingContextMap();
}

/**
 * @tc.name: CreateDrawingContext007
 * @tc.desc: test results of CreateDrawingContext while protectedDrawingContext isn't nullptr
 * @tc.type:FUNC
 * @tc.require: issueICDVVY
 */
HWTEST_F(RSVulkanContextTest, CreateDrawingContext007, TestSize.Level2)
{
    RsVulkanContext::GetSingleton().InitVulkanContextForUniRender("");
    RsVulkanContext::GetSingleton().SetIsProtected(true);
    auto& singletonPtr = RsVulkanContext::GetRecyclableSingletonPtr("");
    singletonPtr.reset();
    RsVulkanContext::GetRecyclableSingleton();
    ASSERT_NE(RsVulkanContext::GetRecyclableSingletonPtr(), nullptr);
}

/**
 * @tc.name: RequireSemaphoreTest
 * @tc.desc: test results of RequireSemaphore
 * @tc.type:FUNC
 * @tc.require: issueICDVVY
 */
HWTEST_F(RSVulkanContextTest, RequireSemaphoreTest, TestSize.Level2)
{
    auto interface = std::make_shared<RsVulkanInterface>();
    interface->Init(VulkanInterfaceType::BASIC_RENDER);
    if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN) {
        RsVulkanInterface::callbackSemaphoreInfofdDupCnt_ = 144000;
        VkSemaphore res = interface->RequireSemaphore();
        EXPECT_NE(res, VK_NULL_HANDLE);

        RsVulkanInterface::callbackSemaphoreInfofdDupCnt_ = 144001;
        res = interface->RequireSemaphore();
        EXPECT_NE(res, VK_NULL_HANDLE);
    } else {
        RsVulkanInterface::callbackSemaphoreInfofdDupCnt_ = 144000;
        VkSemaphore res = interface->RequireSemaphore();
        EXPECT_EQ(res, VK_NULL_HANDLE);

        RsVulkanInterface::callbackSemaphoreInfofdDupCnt_ = 144001;
        res = interface->RequireSemaphore();
        EXPECT_EQ(res, VK_NULL_HANDLE);
    }
}
} // namespace Rosen
} // namespace OHOS