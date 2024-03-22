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

#include <cstddef>
#include "gtest/gtest.h"
#include "skia_adapter/skia_gpu_context.h"
#include "text/font.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaGPUContextTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaGPUContextTest::SetUpTestCase() {}
void SkiaGPUContextTest::TearDownTestCase() {}
void SkiaGPUContextTest::SetUp() {}
void SkiaGPUContextTest::TearDown() {}

#ifdef RS_ENABLE_VK
/**
 * @tc.name: BuildFromVK001
 * @tc.desc: Test BuildFromVK
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaGPUContextTest, BuildFromVK001, TestSize.Level1)
{
    auto gpuContext = std::make_shared<SkiaGPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    GrVkBackendContext grVkBackendContext;
    gpuContext->BuildFromVK(grVkBackendContext);
}

/**
 * @tc.name: storeVkPipelineCacheData001
 * @tc.desc: Test storeVkPipelineCacheData
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaGPUContextTest, storeVkPipelineCacheData001, TestSize.Level1)
{
    auto gpuContext = std::make_shared<SkiaGPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    gpuContext->storeVkPipelineCacheData();
#ifdef NEW_SKIA
    GrMockOptions options;
    gpuContext->SetGrContext(GrDirectContext::MakeMock(&options));
#endif
    gpuContext->storeVkPipelineCacheData();
}
#endif

/**
 * @tc.name: GetResourceCacheUsage001
 * @tc.desc: Test GetResourceCacheUsage
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaGPUContextTest, GetResourceCacheUsage001, TestSize.Level1)
{
    auto gpuContext = std::make_shared<SkiaGPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    gpuContext->GetResourceCacheUsage(nullptr, nullptr);
#ifdef NEW_SKIA
    GrMockOptions options;
    gpuContext->SetGrContext(GrDirectContext::MakeMock(&options));
#endif
    int resourceCount = 1;
    size_t resourceBytes = 1;
    gpuContext->GetResourceCacheUsage(&resourceCount, &resourceBytes);
}

/**
 * @tc.name: FreeGpuResources001
 * @tc.desc: Test FreeGpuResources
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaGPUContextTest, FreeGpuResources001, TestSize.Level1)
{
    auto gpuContext = std::make_shared<SkiaGPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    gpuContext->FreeGpuResources();
}

/**
 * @tc.name: DumpGpuStats001
 * @tc.desc: Test DumpGpuStats
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaGPUContextTest, DumpGpuStats001, TestSize.Level1)
{
    auto gpuContext = std::make_shared<SkiaGPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    std::string out = "11";
    gpuContext->DumpGpuStats(out);
}

/**
 * @tc.name: ReleaseResourcesAndAbandonContext001
 * @tc.desc: Test ReleaseResourcesAndAbandonContext
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaGPUContextTest, ReleaseResourcesAndAbandonContext001, TestSize.Level1)
{
    auto gpuContext = std::make_shared<SkiaGPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    gpuContext->ReleaseResourcesAndAbandonContext();
#ifdef NEW_SKIA
    GrMockOptions options;
    gpuContext->SetGrContext(GrDirectContext::MakeMock(&options));
#endif
    gpuContext->ReleaseResourcesAndAbandonContext();
}

/**
 * @tc.name: PurgeUnlockedResources001
 * @tc.desc: Test PurgeUnlockedResources
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaGPUContextTest, PurgeUnlockedResources001, TestSize.Level1)
{
    auto gpuContext = std::make_shared<SkiaGPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    gpuContext->PurgeUnlockedResources(true);
}

/**
 * @tc.name: PurgeUnlockedResourcesByTag001
 * @tc.desc: Test PurgeUnlockedResourcesByTag
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaGPUContextTest, PurgeUnlockedResourcesByTag001, TestSize.Level1)
{
    auto gpuContext = std::make_shared<SkiaGPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    GPUResourceTag tag;
    gpuContext->PurgeUnlockedResourcesByTag(true, tag);
#ifdef NEW_SKIA
    GrMockOptions options;
    gpuContext->SetGrContext(GrDirectContext::MakeMock(&options));
#endif
    gpuContext->PurgeUnlockedResourcesByTag(true, tag);
}

/**
 * @tc.name: PurgeUnlockedResourcesByPid001
 * @tc.desc: Test PurgeUnlockedResourcesByPid
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaGPUContextTest, PurgeUnlockedResourcesByPid001, TestSize.Level1)
{
    auto gpuContext = std::make_shared<SkiaGPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    GPUResourceTag tag;
    std::set<pid_t> pidset;
    gpuContext->PurgeUnlockedResourcesByPid(true, pidset);
#ifdef NEW_SKIA
    GrMockOptions options;
    gpuContext->SetGrContext(GrDirectContext::MakeMock(&options));
#endif
    gpuContext->PurgeUnlockedResourcesByPid(true, pidset);
}

/**
 * @tc.name: PurgeUnlockAndSafeCacheGpuResources001
 * @tc.desc: Test PurgeUnlockAndSafeCacheGpuResources
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaGPUContextTest, PurgeUnlockAndSafeCacheGpuResources001, TestSize.Level1)
{
    auto gpuContext = std::make_shared<SkiaGPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    gpuContext->PurgeUnlockAndSafeCacheGpuResources();
}

/**
 * @tc.name: Submit001
 * @tc.desc: Test Submit
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaGPUContextTest, Submit001, TestSize.Level1)
{
    auto gpuContext = std::make_shared<SkiaGPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    gpuContext->Submit();
}

/**
 * @tc.name: ReleaseByTag001
 * @tc.desc: Test ReleaseByTag
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaGPUContextTest, ReleaseByTag001, TestSize.Level1)
{
    auto gpuContext = std::make_shared<SkiaGPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    GPUResourceTag tag;
    gpuContext->ReleaseByTag(tag);
#ifdef NEW_SKIA
    GrMockOptions options;
    gpuContext->SetGrContext(GrDirectContext::MakeMock(&options));
#endif
    gpuContext->ReleaseByTag(tag);
}

/**
 * @tc.name: DumpMemoryStatisticsByTag001
 * @tc.desc: Test DumpMemoryStatisticsByTag
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaGPUContextTest, DumpMemoryStatisticsByTag001, TestSize.Level1)
{
    auto gpuContext = std::make_shared<SkiaGPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    GPUResourceTag tag;
    gpuContext->DumpMemoryStatisticsByTag(nullptr, tag);
#ifdef NEW_SKIA
    GrMockOptions options;
    gpuContext->SetGrContext(GrDirectContext::MakeMock(&options));
#endif
    gpuContext->DumpMemoryStatisticsByTag(nullptr, tag);
}

/**
 * @tc.name: DumpMemoryStatistics001
 * @tc.desc: Test DumpMemoryStatistics
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaGPUContextTest, DumpMemoryStatistics001, TestSize.Level1)
{
    auto gpuContext = std::make_shared<SkiaGPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    gpuContext->DumpMemoryStatistics(nullptr);
}

/**
 * @tc.name: SetCurrentGpuResourceTag001
 * @tc.desc: Test SetCurrentGpuResourceTag
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaGPUContextTest, SetCurrentGpuResourceTag001, TestSize.Level1)
{
    auto gpuContext = std::make_shared<SkiaGPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    GPUResourceTag gPUResourceTag;
    gpuContext->SetCurrentGpuResourceTag(gPUResourceTag);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS