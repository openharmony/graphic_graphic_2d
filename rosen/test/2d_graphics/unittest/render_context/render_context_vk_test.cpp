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

#include "render_context.h"

#include "platform/common/rs_system_properties.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
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
    auto renderContext = std::make_shared<RenderContextVK>();
    EXPECT_NE(renderContext, nullptr);

    RsVulkanContext::SetRecyclable(false);
    auto& interface = RsVulkanContext::GetSingleton().GetRsVulkanInterface();
    interface.memHandler_ = nullptr;

    auto res = renderContext->CleanAllShaderCache();
    EXPECT_EQ(res, "");

    interface.memHandler_ = std::make_shared<MemoryHandler>();
    res = renderContext->CleanAllShaderCache();
    EXPECT_NE(res, "");
}

/**
 * @tc.name: GetShaderCacheSize
 * @tc.desc: Verify the GetShaderCacheSize of RenderContextVKTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextVKTest, GetShaderCacheSize, TestSize.Level1)
{
    auto renderContext = std::make_shared<RenderContextVK>();
    EXPECT_NE(renderContext, nullptr);

    RsVulkanContext::SetRecyclable(false);
    auto& interface = RsVulkanContext::GetSingleton().GetRsVulkanInterface();
    interface.memHandler_ = nullptr;
    auto res = renderContext->GetShaderCacheSize();
    EXPECT_EQ(res, "");
}

/**
 * @tc.name: SetUpGpuContextTest001
 * @tc.desc: Verify the SetUpGpuContext and GetPixelFormat of RenderContextVKTest
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextVKTest, SetUpGpuContextTest, Level1)
{
    auto renderContext = std::make_shared<RenderContextVK>();
    EXPECT_NE(renderContext, nullptr);
    renderContext->drGPUContext_ = std::make_shared<Drawing::GPUContext>();
    bool res = renderContext->SetUpGpuContext(nullptr);
    EXPECT_EQ(res, true);
    renderContext->drGPUContext_ = nullptr;
    res = renderContext->SetUpGpuContext(nullptr);
    EXPECT_EQ(res, true);
}
} // namespace OHOS::Rosen