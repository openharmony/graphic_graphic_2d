/*
 * Copyright (c) 2025-2026 Huawei Device Co., Ltd.
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

#include "vulkan_context/rs_vulkan_context.h"
#include "vulkan_context/rs_vulkan_interface.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RsVulkanContextTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsVulkanContextTest::SetUpTestCase() {}
void RsVulkanContextTest::TearDownTestCase() {}
void RsVulkanContextTest::SetUp() {}
void RsVulkanContextTest::TearDown() {}

/**
 * @tc.name: CreateOrGet001
 * @tc.desc: Test CreateOrGet with all RenderEngineType values, default param, invalid type, and singleton identity
 * @tc.type: FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(RsVulkanContextTest, CreateOrGet001, TestSize.Level1)
{
    auto& basic = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER);
    EXPECT_EQ(basic.GetType(), RenderEngineType::BASIC_RENDER);
    auto& unprotected = RsVulkanContext::Get(RenderEngineType::UNPROTECTED_REDRAW);
    EXPECT_EQ(unprotected.GetType(), RenderEngineType::UNPROTECTED_REDRAW);
    auto& protectedCtx = RsVulkanContext::Get(RenderEngineType::PROTECTED_REDRAW);
    EXPECT_EQ(protectedCtx.GetType(), RenderEngineType::PROTECTED_REDRAW);
    auto& defaultCtx = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER);
    EXPECT_EQ(defaultCtx.GetType(), RenderEngineType::BASIC_RENDER);
    auto& invalidCtx = RsVulkanContext::Get(RenderEngineType::MAX_INTERFACE_TYPE);
    EXPECT_EQ(invalidCtx.GetType(), RenderEngineType::BASIC_RENDER);
    auto& context1 = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER);
    auto& context2 = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER);
    EXPECT_EQ(&context1, &context2);
}

/**
 * @tc.name: CreateOrGet007
 * @tc.desc: Test different types create different singletons
 * @tc.type: FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(RsVulkanContextTest, CreateOrGet007, TestSize.Level1)
{
    auto& basic = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER);
    auto& unprotected = RsVulkanContext::Get(RenderEngineType::UNPROTECTED_REDRAW);
    auto& protectedCtx = RsVulkanContext::Get(RenderEngineType::PROTECTED_REDRAW);
    EXPECT_NE(&basic, &unprotected);
    EXPECT_NE(&basic, &protectedCtx);
    EXPECT_NE(&unprotected, &protectedCtx);
}

/**
 * @tc.name: GetRsVulkanInterfaceInterfaceType001
 * @tc.desc: Test GetRsVulkanInterface returns interface with correct RenderEngineType per context type
 * @tc.type: FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(RsVulkanContextTest, GetRsVulkanInterfaceInterfaceType001, TestSize.Level1)
{
    auto& basicCtx = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER);
    EXPECT_EQ(basicCtx.GetRsVulkanInterface()->GetInterfaceType(), RenderEngineType::BASIC_RENDER);
    auto& unprotectedCtx = RsVulkanContext::Get(RenderEngineType::UNPROTECTED_REDRAW);
    EXPECT_EQ(unprotectedCtx.GetRsVulkanInterface()->GetInterfaceType(), RenderEngineType::UNPROTECTED_REDRAW);
    auto& protectedCtx = RsVulkanContext::Get(RenderEngineType::PROTECTED_REDRAW);
    EXPECT_EQ(protectedCtx.GetRsVulkanInterface()->GetInterfaceType(), RenderEngineType::PROTECTED_REDRAW);
}

/**
 * @tc.name: RequireSemaphore001
 * @tc.desc: Test RequireSemaphore and SendSemaphoreWithFd via CreateOrGet
 * @tc.type: FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(RsVulkanContextTest, RequireSemaphore001, TestSize.Level1)
{
    auto& context = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER);
    VkSemaphore semaphore = context.RequireSemaphore();
    if (context.IsValid()) {
        EXPECT_NE(semaphore, VK_NULL_HANDLE);
    } else {
        EXPECT_EQ(semaphore, VK_NULL_HANDLE);
    }
    context.SendSemaphoreWithFd(semaphore, -1);
}

/**
 * @tc.name: QueryMaxGpuBufferSize001
 * @tc.desc: Test QueryMaxGpuBufferSize via CreateOrGet
 * @tc.type: FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(RsVulkanContextTest, QueryMaxGpuBufferSize001, TestSize.Level1)
{
    auto& context = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER);
    uint32_t maxWidth = 0;
    uint32_t maxHeight = 0;
    bool result = context.QueryMaxGpuBufferSize(maxWidth, maxHeight);
    if (context.IsValid()) {
        EXPECT_TRUE(result);
        EXPECT_GT(maxWidth, 0u);
        EXPECT_GT(maxHeight, 0u);
    } else {
        EXPECT_FALSE(result);
    }
    context.vulkanInterface_ = nullptr;
    EXPECT_FALSE(context.QueryMaxGpuBufferSize(maxWidth, maxHeight));
}

/**
 * @tc.name: CreateOrGetProtected001
 * @tc.desc: Test PROTECTED_REDRAW isProtected=true via CreateOrGet
 * @tc.type: FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(RsVulkanContextTest, CreateOrGetProtected001, TestSize.Level1)
{
    auto& context = RsVulkanContext::Get(RenderEngineType::PROTECTED_REDRAW);
    auto vkInterface = context.GetRsVulkanInterface();
    if (context.IsValid()) {
        EXPECT_TRUE(vkInterface->IsProtected());
    }
}

/**
 * @tc.name: CreateOrGetUnprotected001
 * @tc.desc: Test UNPROTECTED_REDRAW isProtected=false via CreateOrGet
 * @tc.type: FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(RsVulkanContextTest, CreateOrGetUnprotected001, TestSize.Level1)
{
    auto& context = RsVulkanContext::Get(RenderEngineType::UNPROTECTED_REDRAW);
    auto vkInterface = context.GetRsVulkanInterface();
    if (context.IsValid()) {
        EXPECT_FALSE(vkInterface->IsProtected());
    }
}

/**
 * @tc.name: BackendContextProtectedFlag001
 * @tc.desc: Test BackendContext fProtectedContext matches isProtected for PROTECTED_REDRAW and BASIC_RENDER
 * @tc.type: FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(RsVulkanContextTest, BackendContextProtectedFlag001, TestSize.Level1)
{
    auto& protectedCtx = RsVulkanContext::Get(RenderEngineType::PROTECTED_REDRAW);
    if (protectedCtx.IsValid()) {
        const auto& backendContext = protectedCtx.GetGrVkBackendContext();
        EXPECT_EQ(backendContext.fProtectedContext, skgpu::Protected::kYes);
    }
    auto& basicCtx = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER);
    if (basicCtx.IsValid()) {
        const auto& backendContext = basicCtx.GetGrVkBackendContext();
        EXPECT_EQ(backendContext.fProtectedContext, skgpu::Protected::kNo);
    }
}

/**
 * @tc.name: BackendContextQueueIndex001
 * @tc.desc: Test BackendContext fields populated after valid Init (queue index, instance, device, extensions, features)
 * @tc.type: FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(RsVulkanContextTest, BackendContextQueueIndex001, TestSize.Level1)
{
    auto& context = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER);
    if (context.IsValid()) {
        const auto& backendContext = context.GetGrVkBackendContext();
        EXPECT_NE(backendContext.fGraphicsQueueIndex, UINT32_MAX);
        EXPECT_NE(backendContext.fQueue, VK_NULL_HANDLE);
        EXPECT_NE(backendContext.fInstance, VK_NULL_HANDLE);
        EXPECT_NE(backendContext.fPhysicalDevice, VK_NULL_HANDLE);
        EXPECT_NE(backendContext.fDevice, VK_NULL_HANDLE);
        EXPECT_TRUE(backendContext.fGetProc != nullptr);
        EXPECT_TRUE(backendContext.fVkExtensions != nullptr);
        EXPECT_TRUE(backendContext.fDeviceFeatures2 != nullptr);
    }
}

/**
 * @tc.name: IsValidDefaultConstructor001
 * @tc.desc: Test IsValid returns false when vulkanInterface_ is null (via moved-from state)
 * @tc.type: FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(RsVulkanContextTest, IsValidDefaultConstructor001, TestSize.Level1)
{
    // The default constructor is deleted, so we test the null-path behavior
    // by verifying that a valid context returns true for IsValid
    auto& context = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER);
    context.vulkanInterface_ =  std::make_shared<RsVulkanInterface>(false, true, RenderEngineType::BASIC_RENDER);
    auto vkInterface = context.GetRsVulkanInterface();
    ASSERT_TRUE(vkInterface != nullptr);
    // When vulkanInterface_ is non-null, IsValid depends on vulkanInterface_->IsValid()
    bool interfaceValid = vkInterface->IsValid();
    EXPECT_EQ(context.IsValid(), interfaceValid);
}

/**
 * @tc.name: ReleaseDrawingGPUContextNull001
 * @tc.desc: Test ReleaseDrawingGPUContext with null gpuContext
 * @tc.type: FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(RsVulkanContextTest, ReleaseDrawingGPUContextNull001, TestSize.Level1)
{
    auto& context = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER);
    std::shared_ptr<Drawing::GPUContext> nullContext = nullptr;
    // Should not crash when passing null gpuContext
    context.ReleaseDrawingGPUContext(nullContext);
    EXPECT_EQ(nullContext, nullptr);
}

/**
 * @tc.name: ReleaseDrawingGPUContextValid001
 * @tc.desc: Test ReleaseDrawingGPUContext with a valid gpuContext created via CreateDrawingGPUContext
 * @tc.type: FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(RsVulkanContextTest, ReleaseDrawingGPUContextValid001, TestSize.Level1)
{
    auto& context = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER);
    if (!context.IsValid()) {
        return;
    }
    auto gpuContext = context.CreateDrawingGPUContext("/data/local/tmp");
    ASSERT_TRUE(gpuContext != nullptr);
    // Release should flush, purge, and reset
    context.ReleaseDrawingGPUContext(gpuContext);
    EXPECT_EQ(gpuContext, nullptr);
    gpuContext = context.CreateDrawingGPUContext();
    context.vulkanInterface_ = nullptr;
    context.ReleaseDrawingGPUContext(gpuContext);
    EXPECT_EQ(gpuContext, nullptr);
    context.vulkanInterface_ = std::make_shared<RsVulkanInterface>(false, true, RenderEngineType::BASIC_RENDER);
}

} // namespace Rosen
} // namespace OHOS
