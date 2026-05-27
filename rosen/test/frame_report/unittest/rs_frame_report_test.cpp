/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#include <string>
#ifdef RS_ENABLE_VK
#include <dlfcn.h>
#include <gmock/gmock.h>
#endif

#include "rs_frame_report.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RsFrameReportTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsFrameReportTest::SetUpTestCase() {}
void RsFrameReportTest::TearDownTestCase() {}
void RsFrameReportTest::SetUp() {}
void RsFrameReportTest::TearDown() {}
#ifdef RS_ENABLE_VK
class MockDlopen {
public:
    MOCK_METHOD(void*, dlopen, (const char* filename, int flag));
};

class MockDlsym {
public:
    MOCK_METHOD(void*, dlsym, (void* restrict, const char* funcname));
};

void* mockvkGetDeviceProcAddr(void*, const char*)
{
    return nullptr;
}
void* (*mockvkGetDeviceProcAddrFunc)(void*, const char*) = mockvkGetDeviceProcAddr;
 
void* mockvkEnumeratePhysicalDevices(void*, const char*)
{
    return nullptr;
}
void* mockvkGetInstanceProcAddr(void* instance, const char* funcname)
{
    if (strcmp(funcname, "vkEnumeratePhysicalDevices") == 0) {
        return reinterpret_cast<void*>(mockvkEnumeratePhysicalDevices);
    }
    return nullptr;
}
void* (*mockvkGetInstanceProcAddrFunc)(void*, const char*) = mockvkGetInstanceProcAddr;
 
VkResult mockvkCreateInstance(const VkInstanceCreateInfo*, const VkAllocationCallbacks*, VkInstance*)
{
    return VK_TIMEOUT;
}
VkResult (*mockvkCreateInstanceFunc)(const VkInstanceCreateInfo*, const VkAllocationCallbacks*, VkInstance*) =
    mockvkCreateInstance;
#endif

/**
 * @tc.name: NoBranchTestCase001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, NoBranchTestCase, TestSize.Level1)
{
    ASSERT_FALSE(RsFrameReport::IsInitSchedCompleted());
    RsFrameReport::InitDeadline();
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());
    
    RsFrameReport::SetFrameParam(0, 0, 0, 0);
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());

    RsFrameReport::SendCommandsStart();
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());

    RsFrameReport::RenderStart(1, 1);
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());

    RsFrameReport::RenderEnd();
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());

    RsFrameReport::DirectRenderEnd();
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());

    RsFrameReport::UniRenderStart();
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());

    RsFrameReport::UniRenderEnd();
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());

    RsFrameReport::CheckUnblockMainThreadPoint();
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());

    RsFrameReport::CheckPostAndWaitPoint();
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());

    RsFrameReport::CheckBeginFlushPoint();
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());

    RsFrameReport::ReportComposerInfo(1, 1);
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());

    RsFrameReport::ReportDDGRTaskInfo();
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());
}

/**
 * @tc.name: ReportScbSceneInfo001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, ReportScbSceneInfo001, TestSize.Level1)
{
    std::string description = "test";
    bool eventStatus = true;
    RsFrameReport::ReportScbSceneInfo(description, eventStatus);
 
    eventStatus = false;
    RsFrameReport::ReportScbSceneInfo(description, eventStatus);
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());
}

/**
 * @tc.name: ReportFrameDeadline001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, ReportFrameDeadline001, TestSize.Level1)
{
    int deadline = 8333;
    uint32_t currentRate = 120;
    RsFrameReport::ReportFrameDeadline(deadline, currentRate);
 
    deadline = -8333;
    RsFrameReport::ReportFrameDeadline(deadline, currentRate);
 
    deadline = 0;
    RsFrameReport::ReportFrameDeadline(deadline, currentRate);
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());
}

/**
 * @tc.name: ReportBufferCount001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, ReportBufferCount001, TestSize.Level1)
{
    RsFrameReport::ReportBufferCount(1);
    RsFrameReport::ReportBufferCount(1);
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());
}

/**
 * @tc.name: ReportUnmarshalData001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, ReportUnmarshalData001, TestSize.Level1)
{
    int unmarshalTid = 123456;
    size_t dataSize = 20 * 1024;
    RsFrameReport::ReportUnmarshalData(unmarshalTid, dataSize);
 
    unmarshalTid = 0;
    RsFrameReport::ReportUnmarshalData(unmarshalTid, dataSize);
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());
}

#ifdef RS_ENABLE_VK
/**
 * @tc.name: ReportWindowInfo001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, ReportWindowInfo001, TestSize.Level1)
{
    RsFrameReport::isInit.store(false);
    RsFrameReport::mSetFrontWindowStatusHUAWEI = nullptr;
    RsFrameReport::ReportWindowInfo(false, "com.test.app");
    EXPECT_TRUE(RsFrameReport::isInit.load());
}

/**
 * @tc.name: VkHandleDeleter001
 * @tc.desc: test Nullptr Do Not Call_dlclose
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, VkHandleDeleter001, TestSize.Level1)
{
    RsFrameReport::VkHandleDeleter deleter;
    deleter(nullptr);
    EXPECT_TRUE(RsFrameReport::isInit.load());
}

/**
 * @tc.name: GetSetFrontWindowStatus
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, GetSetFrontWindowStatus, TestSize.Level1)
{
    RsFrameReport::vkGetDeviceProcAddr = nullptr;
    bool result = RsFrameReport::GetSetFrontWindowStatusHUAWEI();
    EXPECT_FALSE(result);
    RsFrameReport::isInit.store(false);
    RsFrameReport::InitializeVulkanExtensions();
}
 
/**
 * @tc.name: CreateVulkanDevice001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, CreateVulkanDevice001, TestSize.Level1)
{
    auto mockvkCreateDevice =
        [](VkPhysicalDevice, const VkDeviceCreateInfo*, const VkAllocationCallbacks*, VkDevice*) -> VkResult {
        return VK_TIMEOUT;
    };
    RsFrameReport::vkCreateDevice = mockvkCreateDevice;
    EXPECT_FALSE(RsFrameReport::CreateVulkanDevice());
 
    auto mockvkGetPhysicalDeviceQueueFamilyProperties =
        [](VkPhysicalDevice, uint32_t* pPropertyCount, VkQueueFamilyProperties*) -> void {
        *pPropertyCount = 0;
    };
    RsFrameReport::vkGetPhysicalDeviceQueueFamilyProperties = mockvkGetPhysicalDeviceQueueFamilyProperties;
    EXPECT_FALSE(RsFrameReport::CreateVulkanDevice());
 
    auto mockvkEnumeratePhysicalDevices = [](VkInstance, uint32_t* pDeviceCount, VkPhysicalDevice*) -> VkResult {
        *pDeviceCount = 0;
        return VK_SUCCESS;
    };
    RsFrameReport::vkEnumeratePhysicalDevices = mockvkEnumeratePhysicalDevices;
    EXPECT_FALSE(RsFrameReport::CreateVulkanDevice());
    RsFrameReport::isInit.store(false);
    RsFrameReport::InitializeVulkanExtensions();
}
 
/**
 * @tc.name: CreateVulkanDevice002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, CreateVulkanDevice002, TestSize.Level1)
{
    RsFrameReport::vkCreateDevice = nullptr;
    EXPECT_FALSE(RsFrameReport::CreateVulkanDevice());
    RsFrameReport::vkEnumeratePhysicalDevices = nullptr;
    EXPECT_FALSE(RsFrameReport::CreateVulkanDevice());
    RsFrameReport::isInit.store(false);
    RsFrameReport::InitializeVulkanExtensions();
}
 
/**
 * @tc.name: FindQueueFamilyIndex001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, FindQueueFamilyIndex001, TestSize.Level1)
{
    auto mockvkGetPhysicalDeviceQueueFamilyProperties =
        [](VkPhysicalDevice, uint32_t* pPropertyCount, VkQueueFamilyProperties* queueFamilies) -> void {
        *pPropertyCount = 1;
        if (queueFamilies) {
            for (uint32_t i = 0; i < *pPropertyCount; i++) {
                queueFamilies[i].queueFlags = false;
            }
        }
    };
    RsFrameReport::vkGetPhysicalDeviceQueueFamilyProperties = mockvkGetPhysicalDeviceQueueFamilyProperties;
    EXPECT_FALSE(RsFrameReport::CreateVulkanDevice());
    RsFrameReport::isInit.store(false);
    RsFrameReport::InitializeVulkanExtensions();
}
 
/**
 * @tc.name: FindQueueFamilyIndex002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, FindQueueFamilyIndex002, TestSize.Level1)
{
    RsFrameReport::vkGetPhysicalDeviceQueueFamilyProperties = nullptr;
    EXPECT_FALSE(RsFrameReport::CreateVulkanDevice());
    RsFrameReport::isInit.store(false);
    RsFrameReport::InitializeVulkanExtensions();
}
 
/**
 * @tc.name: CreateVulkanInstance001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, CreateVulkanInstance001, TestSize.Level1)
{
    RsFrameReport::vkCreateInstance = nullptr;
    bool result = RsFrameReport::CreateVulkanInstance();
    EXPECT_FALSE(result);
    RsFrameReport::isInit.store(false);
    RsFrameReport::InitializeVulkanExtensions();
}
 
/**
 * @tc.name: GetVulkanFunctionPointersByInstance001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, GetVulkanFunctionPointersByInstance001, TestSize.Level1)
{
    RsFrameReport::vkGetInstanceProcAddr = nullptr;
    EXPECT_FALSE(RsFrameReport::GetVulkanFunctionPointersByInstance());
    RsFrameReport::vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(0x123);
    RsFrameReport::instance_ = nullptr;
    EXPECT_FALSE(RsFrameReport::GetVulkanFunctionPointersByInstance());
    RsFrameReport::isInit.store(false);
    RsFrameReport::InitializeVulkanExtensions();
}
 
/**
 * @tc.name: InitializeVulkanExtensions001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, InitializeVulkanExtensions001, TestSize.Level1)
{
    RsFrameReport::isInit.store(true);
    RsFrameReport::mSetFrontWindowStatusHUAWEI = nullptr;
    EXPECT_TRUE(RsFrameReport::InitializeVulkanExtensions());
    RsFrameReport::isInit.store(true);
    RsFrameReport::mSetFrontWindowStatusHUAWEI = reinterpret_cast<PFN_vkSetFrontWindowStatusHUAWEI>(0x1234);
    EXPECT_TRUE(RsFrameReport::InitializeVulkanExtensions());
}
 
/**
 * @tc.name: InitializeVulkanExtensions002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, InitializeVulkanExtensions002, TestSize.Level1)
{
    RsFrameReport::isInit.store(false);
    auto originDlsym = RsFrameReport::dlsymFunc;
    MockDlsym mockDlsym;
    RsFrameReport::dlsymFunc = [&mockDlsym, &originDlsym](void* restrict, const char* funcname) {
        if (strcmp(funcname, "vkGetDeviceProcAddr") == 0) {
            return mockDlsym.dlsym(restrict, funcname);
        }
        return originDlsym(restrict, funcname);
    };
    EXPECT_CALL(mockDlsym, dlsym(testing::_, testing::_)).WillOnce(testing::Invoke([](void*, const char*) {
        return reinterpret_cast<void*>(mockvkGetDeviceProcAddrFunc);
    }));
    EXPECT_FALSE(RsFrameReport::InitializeVulkanExtensions());
    RsFrameReport::dlsymFunc = originDlsym;
}
 
/**
 * @tc.name: InitializeVulkanExtensions003
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, InitializeVulkanExtensions003, TestSize.Level1)
{
    RsFrameReport::isInit.store(false);
    auto originDlsym = RsFrameReport::dlsymFunc;
    MockDlsym mockDlsym;
    RsFrameReport::dlsymFunc = [&mockDlsym, &originDlsym](void* restrict, const char* funcname) {
        if (strcmp(funcname, "vkCreateInstance") == 0) {
            return mockDlsym.dlsym(restrict, funcname);
        }
        return originDlsym(restrict, funcname);
    };
    EXPECT_CALL(mockDlsym, dlsym(testing::_, testing::_)).WillOnce(testing::Invoke([](void*, const char*) {
        return reinterpret_cast<void*>(mockvkCreateInstanceFunc);
    }));
    EXPECT_FALSE(RsFrameReport::InitializeVulkanExtensions());
    RsFrameReport::dlsymFunc = originDlsym;
}
 
/**
 * @tc.name: InitializeVulkanExtensions004
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, InitializeVulkanExtensions004, TestSize.Level1)
{
    RsFrameReport::isInit.store(false);
    auto originDlsym = RsFrameReport::dlsymFunc;
    MockDlsym mockDlsym;
    RsFrameReport::dlsymFunc = [&mockDlsym, &originDlsym](void* restrict, const char* funcname) {
        if (strcmp(funcname, "vkGetInstanceProcAddr") == 0) {
            return mockDlsym.dlsym(restrict, funcname);
        }
        return originDlsym(restrict, funcname);
    };
    EXPECT_CALL(mockDlsym, dlsym(testing::_, testing::_)).WillOnce(testing::Invoke([](void*, const char*) {
        return reinterpret_cast<void*>(mockvkGetInstanceProcAddrFunc);
    }));
    EXPECT_FALSE(RsFrameReport::InitializeVulkanExtensions());
    RsFrameReport::dlsymFunc = originDlsym;
}
 
/**
 * @tc.name: LoadVulkanLibrary001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, LoadVulkanLibrary001, TestSize.Level1)
{
    auto originDlopen = RsFrameReport::dlopenFunc;
    MockDlopen mockDlopen;
    RsFrameReport::dlopenFunc = [&mockDlopen](const char* filename, int flag) {
        return mockDlopen.dlopen(filename, flag);
    };
    EXPECT_CALL(mockDlopen, dlopen(testing::_, testing::_)).WillOnce(testing::Return(nullptr));
    RsFrameReport::isInit.store(false);
    RsFrameReport::ReportWindowInfo(false, "com.test.app");
    EXPECT_FALSE(RsFrameReport::isInit.load());
    RsFrameReport::dlopenFunc = originDlopen;
}
#endif
} // namespace Rosen
} // namespace OHOS
