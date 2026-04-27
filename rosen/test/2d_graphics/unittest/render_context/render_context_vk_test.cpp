/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
 
#include <gtest/gtest.h>
#include <memory>

#include "platform/common/rs_system_properties.h"
#include "render_context.h"
#include "render_context/new_render_context/render_context_vk.h"
using namespace testing::ext;
 
namespace OHOS::Rosen {
class RenderContextVKTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};
 
void RenderContextVKTest::SetUpTestCase() {}
void RenderContextVKTest::TearDownTestCase() {}
void RenderContextVKTest::SetUp() {}
void RenderContextVKTest::TearDown() {}
 
/**
 * @tc.name: CleanAllShaderCache
 * @tc.desc: Verify the CleanAllShaderCache of RenderContextVKTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextVKTest, CleanAllShaderCache, TestSize.Level1)
{
    if (!RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "opengl enable! skip vulkan test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextVK>();
    EXPECT_NE(renderContext, nullptr);
    auto vkContext = RsVulkanContext::GetInstance();
    if (vkContext == nullptr) {
        GTEST_LOG_(INFO) << "vulkan context is null, skip test";
        return;
    }
    vkContext->InitVulkanContextForHybridRender();
    vkContext->vulkanInterfaceType_ = VulkanInterfaceType::BASIC_RENDER;
    vkContext->vulkanInterfaceVec_[size_t(VulkanInterfaceType::BASIC_RENDER)].memHandle_ = nullptr;
    auto result = renderContext->CleanAllShaderCache();
    EXPECT_EQ(result, "");
}
}
 
/**
 * @tc.name: GetShaderCacheSize
 * @tc.desc: Verify the GetShaderCacheSize of RenderContextVKTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextVKTest, GetShaderCacheSize, TestSize.Level1)
{
    if (!RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "opengl enable! skip vulkan test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextVK>();
    EXPECT_NE(renderContext, nullptr);
    auto vkContext = RsVulkanContext::GetInstance();
    if (vkContext == nullptr) {
        GTEST_LOG_(INFO) << "vulkan context is null, skip test";
        return;
    }
    vkContext->InitVulkanContextForHybridRender();
    vkContext->vulkanInterfaceType_ = VulkanInterfaceType::BASIC_RENDER;
    vkContext->vulkanInterfaceVec_[size_t(VulkanInterfaceType::BASIC_RENDER)].memHandle_ = nullptr;
    auto result = renderContext->GetShaderCacheSize();
    EXPECT_EQ(result, "");
}

/**
 * @tc.name: SetUpGpuContextTest001
 * @tc.desc: Verify the SetUpGpuContext and GetPixelFormat of RenderContextVKTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextVKTest, SetUpGpuContextTest, Level1)
{
    if (!RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "opengl enable! skip vulkan test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextVK>();
    EXPECT_NE(renderContext, nullptr);
    renderContext->drGPUContext_ = std::make_shared<Drawing::GPUContext>();
    bool res = renderContext->SetUpGpuContext();
    EXPECT_EQ(res, true);
    renderContext->drGPUContext_ = nullptr;
    res = renderContext->SetUpGpuContext(nullptr);
    EXPECT_EQ(res, true);
}
 
/**
 * @tc.name: AbandonContextTest001
 * @tc.desc: Verify the AbandonContext and GetPixelFormat of RenderContextVKTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextVKTest, AbandonContextTest, Level1)
{
    if (!RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "opengl enable! skip vulkan test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextVK>();
    renderContext->drGPUContext_ = nullptr;
    auto res = renderContext->AbandonContext();
    EXPECT_EQ(res, true);
    renderContext->drGPUContext_ = std::make_shared<Drawing::GPUContext>();
    res = renderContext->AbandonContext();
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: CreateFromWindowTest
 * @tc.desc: Verify the CreateFromWindow
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextVKTest, CreateFromWindowTest, Level1)
{
    if (!RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "opengl enable! skip vulkan test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextVK>();
    EXPECT_NE(renderContext, nullptr);
    const int32_t width = 500;
    const int32_t height = 500;
    NativeWindow *window = nullptr;
    ImageInfo imageInfo(width, height, COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE);
    window = CreateNativeWindowFromSurface(&surf);
    auto surface = renderContext-> CreateFromWindow(nullptr, imageInfo, window);
    EXPECT_EQ(surface_, nullptr);
    surface = renderContext->CreateFromWindow(nullptr, imageInfo, nullptr);
    EXPECT_EQ(surface_, nullptr);
    bool ret = renderContext->FlushSurface(nullptr);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: SetRenderContextTypeTest
 * @tc.desc: Verify the SetRenderContextType
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextVKTest, SetRenderContextTypeTest, Level1)
{
    auto renderContext = std::make_shared<RenderContextVK>();
    EXPECT_NE(renderContext, nullptr);
    renderContext->SetRenderContextType(1);
    EXPECT_EQ(renderContext->isProtected_.load(), true);
    renderContext->SetRenderContextType(2);
    EXPECT_EQ(renderContext->isProtected_.load(), false);
}

/**
 * @tc.name: ChangeProtectedStateTest
 * @tc.desc: Verify the ChangeProtectedState
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextVKTest, ChangeProtectedStateTest, Level1)
{
    auto renderContext = std::make_shared<RenderContextVK>();
    EXPECT_NE(renderContext, nullptr);
    renderContext->isProtected_.store(false);
    renderContext->ChangeProtectedState(true);
    EXPECT_EQ(renderContext->isProtected_.load(), true);
    renderContext->ChangeProtectedState(true);
    EXPECT_EQ(renderContext->isProtected_.load(), true);
}

/**
 * @tc.name: ChangeProtectedStateFromTrueToFalseTest
 * @tc.desc: Verify the ChangeProtectedState from true to false
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextVKTest, ChangeProtectedStateFromTrueToFalseTest, Level1)
{
    auto renderContext = std::make_shared<RenderContextVK>();
    EXPECT_NE(renderContext, nullptr);
    renderContext->isProtected_.store(true);
    renderContext->ChangeProtectedState(false);
    EXPECT_EQ(renderContext->isProtected_.load(), false);
}

/**
 * @tc.name: ChangeProtectedStateNoChangeTest
 * @tc.desc: Verify the ChangeProtectedState when state doesn't change
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextVKTest, ChangeProtectedStateNoChangeTest, Level1)
{
    auto renderContext = std::make_shared<RenderContextVK>();
    EXPECT_NE(renderContext, nullptr);
    renderContext->isProtected_.store(false);
    auto originalContext = renderContext->drGPUContext_;
    renderContext->ChangeProtectedState(false);
    EXPECT_EQ(renderContext->isProtected_.load(), false);
    // drGPUContext_ should not change when isProtected_ doesn't change
    EXPECT_EQ(renderContext->drGPUContext_, originalContext);
}

/**
 * @tc.name: SetRenderContextTypeBasicRenderTest
 * @tc.desc: Verify the SetRenderContextType with BASIC_RENDER type
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextVKTest, SetRenderContextTypeBasicRenderTest, Level1)
{
    auto renderContext = std::make_shared<RenderContextVK>();
    EXPECT_NE(renderContext, nullptr);
    renderContext->isProtected_.store(true); // Start with protected state
    renderContext->SetRenderContextType(0); // BASIC_RENDER
    // BASIC_RENDER should not change isProtected_ state
    EXPECT_EQ(renderContext->isProtected_.load(), true);
}

/**
 * @tc.name: SetRenderContextTypeInvalidValueTest
 * @tc.desc: Verify the SetRenderContextType with invalid value
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextVKTest, SetRenderContextTypeInvalidValueTest, Level1)
{
    auto renderContext = std::make_shared<RenderContextVK>();
    EXPECT_NE(renderContext, nullptr);
    renderContext->isProtected_.store(false);
    // Test with value >= MAX_INTERFACE_TYPE (invalid)
    renderContext->SetRenderContextType(3);
    // Invalid value should not change isProtected_ state
    EXPECT_EQ(renderContext->isProtected_.load(), false);

    renderContext->isProtected_.store(true);
    renderContext->SetRenderContextType(200);
    EXPECT_EQ(renderContext->isProtected_.load(), true);
}

/**
 * @tc.name: SetRenderContextTypeSequenceTest
 * @tc.desc: Verify the SetRenderContextType with sequential calls
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextVKTest, SetRenderContextTypeSequenceTest, Level1)
{
    auto renderContext = std::make_shared<RenderContextVK>();
    EXPECT_NE(renderContext, nullptr);

    // Test sequence: BASIC -> PROTECTED -> UNPROTECTED -> BASIC
    renderContext->SetRenderContextType(0); // BASIC_RENDER
    EXPECT_EQ(renderContext->isProtected_.load(), false);

    renderContext->SetRenderContextType(1); // PROTECTED_REDRAW
    EXPECT_EQ(renderContext->isProtected_.load(), true);

    renderContext->SetRenderContextType(2); // UNPROTECTED_REDRAW
    EXPECT_EQ(renderContext->isProtected_.load(), false);

    renderContext->SetRenderContextType(0); // BASIC_RENDER
    EXPECT_EQ(renderContext->isProtected_.load(), false);
}

/**
 * @tc.name: QueryMaxGpuBufferSize001
 * @tc.desc: Verify QueryMaxGpuBufferSize with valid Vulkan context
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextVKTest, QueryMaxGpuBufferSize001, TestSize.Level1)
{
    if (!RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "opengl enable! skip vulkan test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextVK>();
    EXPECT_NE(renderContext, nullptr);
    auto vkContext = RsVulkanContext::GetInstance();
    if (vkContext == nullptr) {
        GTEST_LOG_(INFO) << "vulkan context is null, skip test";
        return;
    }
    vkContext->InitVulkanContextForHybridRender();
    uint32_t maxWidth = 0;
    uint32_t maxHeight = 0;
    bool result = renderContext->QueryMaxGpuBufferSize(maxWidth, maxHeight);
    EXPECT_TRUE(result);
    EXPECT_GT(maxWidth, 0);
    EXPECT_GT(maxHeight, 0);
}

/**
 * @tc.name: QueryMaxGpuBufferSize002
 * @tc.desc: Verify QueryMaxGpuBufferSize without Vulkan context
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextVKTest, QueryMaxGpuBufferSize002, TestSize.Level1)
{
    if (!RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "opengl enable! skip vulkan test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextVK>();
    EXPECT_NE(renderContext, nullptr);
    uint32_t maxWidth = 0;
    uint32_t maxHeight = 0;
    bool result = renderContext->QueryMaxGpuBufferSize(maxWidth, maxHeight);
}

/**
 * @tc.name: QueryMaxGpuBufferSize003
 * @tc.desc: Verify QueryMaxGpuBufferSize with preset values
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextVKTest, QueryMaxGpuBufferSize003, TestSize.Level1)
{
    if (!RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "opengl enable! skip vulkan test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextVK>();
    EXPECT_NE(renderContext, nullptr);
    auto vkContext = RsVulkanContext::GetInstance();
    if (vkContext == nullptr) {
        GTEST_LOG_(INFO) << "vulkan context is null, skip test";
        return;
    }
    vkContext->InitVulkanContextForHybridRender();
    uint32_t maxWidth = 4096;
    uint32_t maxHeight = 4096;
    bool result = renderContext->QueryMaxGpuBufferSize(maxWidth, maxHeight);
    EXPECT_TRUE(result);
    EXPECT_GE(maxWidth, 4096);
    EXPECT_GE(maxHeight, 4096);
}

/**
 * @tc.name: QueryMaxGpuBufferSize004
 * @tc.desc: Verify QueryMaxGpuBufferSize consistency across multiple calls
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextVKTest, QueryMaxGpuBufferSize004, TestSize.Level1)
{
    if (!RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "opengl enable! skip vulkan test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextVK>();
    EXPECT_NE(renderContext, nullptr);
    auto vkContext = RsVulkanContext::GetInstance();
    if (vkContext == nullptr) {
        GTEST_LOG_(INFO) << "vulkan context is null, skip test";
        return;
    }
    vkContext->InitVulkanContextForHybridRender();
    uint32_t maxWidth1 = 0;
    uint32_t maxHeight1 = 0;
    bool result1 = renderContext->QueryMaxGpuBufferSize(maxWidth1, maxHeight1);

    uint32_t maxWidth2 = 0;
    uint32_t maxHeight2 = 0;
    bool result2 = renderContext->QueryMaxGpuBufferSize(maxWidth2, maxHeight2);

    EXPECT_TRUE(result1);
    EXPECT_TRUE(result2);
    EXPECT_EQ(maxWidth1, maxWidth2);
    EXPECT_EQ(maxHeight1, maxHeight2);
}
} // namespace OHOS::Rosen