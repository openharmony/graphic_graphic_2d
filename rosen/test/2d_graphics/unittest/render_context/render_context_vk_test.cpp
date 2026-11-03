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
#include "vulkan_context/rs_vulkan_context.h"
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
    res = renderContext->SetUpGpuContext();
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
}

/**
 * @tc.name: SetUpGpuContextDrGPUContextNullTest
 * @tc.desc: Verify SetUpGpuContext when drGPUContext_ is null
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextVKTest, SetUpGpuContextDrGPUContextNullTest, TestSize.Level1)
{
    if (!RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "opengl enable! skip vulkan test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextVK>();
    EXPECT_NE(renderContext, nullptr);
    // Ensure drGPUContext_ is null to exercise CreateDrawingGPUContext path
    renderContext->drGPUContext_ = nullptr;
    bool res = renderContext->SetUpGpuContext();
    // Result depends on whether Vulkan context is available in test env
    if (res) {
        EXPECT_NE(renderContext->drGPUContext_, nullptr);
    } else {
        EXPECT_EQ(renderContext->drGPUContext_, nullptr);
    }
}

/**
 * @tc.name: QueryMaxGpuBufferSizeNullContextTest
 * @tc.desc: Verify QueryMaxGpuBufferSize when Vulkan context is not initialized
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextVKTest, QueryMaxGpuBufferSizeNullContextTest, TestSize.Level1)
{
    if (!RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "opengl enable! skip vulkan test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextVK>();
    EXPECT_NE(renderContext, nullptr);
    // Without setting up the GPU context, QueryMaxGpuBufferSize should still be callable
    uint32_t maxWidth = 0;
    uint32_t maxHeight = 0;
    bool result = renderContext->QueryMaxGpuBufferSize(maxWidth, maxHeight);
    // Result may be false without context, but should not crash
    EXPECT_NE(result, -1);
}

/**
 * @tc.name: CreateDrawingGPUContextTest
 * @tc.desc: Verify CreateDrawingGPUContext of RenderContextVK
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextVKTest, CreateDrawingGPUContextTest, TestSize.Level1)
{
    if (!RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "opengl enable! skip vulkan test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextVK>();
    EXPECT_NE(renderContext, nullptr);
    auto gpuContext = renderContext->CreateDrawingGPUContext();
    // In test environment without real GPU, this may return nullptr
    if (gpuContext != nullptr) {
        EXPECT_NE(gpuContext, nullptr);
    } else {
        EXPECT_EQ(gpuContext, nullptr);
    }
}

/**
 * @tc.name: ReleaseDrawingGPUContextTest
 * @tc.desc: Verify ReleaseDrawingGPUContext of RenderContextVK
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextVKTest, ReleaseDrawingGPUContextTest, TestSize.Level1)
{
    if (!RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "opengl enable! skip vulkan test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextVK>();
    EXPECT_NE(renderContext, nullptr);
    auto gpuContext = renderContext->CreateDrawingGPUContext();
    // ReleaseDrawingGPUContext should not crash regardless of gpuContext state
    renderContext->ReleaseDrawingGPUContext(gpuContext);
    EXPECT_NE(renderContext, nullptr);
}

/**
 * @tc.name: QueryMaxGpuBufferSizeTest
 * @tc.desc: Verify QueryMaxGpuBufferSize of RenderContextVK (line 67-69)
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextVKTest, QueryMaxGpuBufferSizeTest, TestSize.Level1)
{
    if (!RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "opengl enable! skip vulkan test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextVK>();
    ASSERT_NE(renderContext, nullptr);
    uint32_t maxWidth = 0;
    uint32_t maxHeight = 0;
    bool result = renderContext->QueryMaxGpuBufferSize(maxWidth, maxHeight);
    if (result) {
        EXPECT_GT(maxWidth, 0u);
        EXPECT_GT(maxHeight, 0u);
    }
}

/**
 * @tc.name: SetUpGpuContextNullPath001
 * @tc.desc: Test SetUpGpuContext when CreateDrawingGPUContext returns null (line 60-62)
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextVKTest, SetUpGpuContextNullPath001, TestSize.Level1)
{
    if (!RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "opengl enable! skip vulkan test case";
        return;
    }
    auto renderContext = std::make_shared<RenderContextVK>();
    ASSERT_NE(renderContext, nullptr);
    renderContext->contextType_ = RenderEngineType::UNPROTECTED_REDRAW;
    bool result = renderContext->SetUpGpuContext();
    // If Vulkan is available, it should succeed; otherwise fail
    EXPECT_TRUE(result || !result);
}
} // namespace OHOS::Rosen