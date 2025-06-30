/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <parameter.h>
#include <parameters.h>
#include "gtest/gtest.h"

#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "GLES3/gl32.h"

#include "draw/color.h"
#include "image/gpu_context.h"
#include "utils/log.h"

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
constexpr int32_t EGL_CONTEXT_CLIENT_VERSION_NUM = 2;

class ShaderPersistentCache : public GPUContextOptions::PersistentCache {
public:
    ShaderPersistentCache() = default;
    ~ShaderPersistentCache() override = default;

    std::shared_ptr<Data> Load(const Data& key) override { return nullptr; };
    void Store(const Data& key, const Data& data) override {};
};

class GpuContextTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static void InitEGL();
    static void DestroyEGL();

private:
    static EGLDisplay eglDisplay_;
    static EGLContext eglContext_;
};

EGLDisplay GpuContextTest::eglDisplay_ = EGL_NO_DISPLAY;
EGLContext GpuContextTest::eglContext_ = EGL_NO_CONTEXT;

void GpuContextTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
    InitEGL();
}

void GpuContextTest::TearDownTestCase()
{
    DestroyEGL();
}

void GpuContextTest::SetUp() {}
void GpuContextTest::TearDown() {}

void GpuContextTest::InitEGL()
{
    LOGI("Creating EGLContext!!!");
    eglDisplay_ = eglGetDisplay(static_cast<EGLNativeDisplayType>(EGL_DEFAULT_DISPLAY));
    if (eglDisplay_ == EGL_NO_DISPLAY) {
        LOGW("Failed to create EGLDisplay gl errno : %{public}x", eglGetError());
        return;
    }

    EGLint major, minor;
    if (eglInitialize(eglDisplay_, &major, &minor) == EGL_FALSE) {
        LOGE("Failed to initialize EGLDisplay");
        return;
    }

    if (eglBindAPI(EGL_OPENGL_ES_API) == EGL_FALSE) {
        LOGE("Failed to bind OpenGL ES API");
        return;
    }

    unsigned int ret;
    EGLConfig config;
    EGLint count;
    EGLint configAttribs[] = { EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8, EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT, EGL_NONE };

    ret = eglChooseConfig(eglDisplay_, configAttribs, &config, 1, &count);
    if (!(ret && static_cast<unsigned int>(count) >= 1)) {
        LOGE("Failed to eglChooseConfig");
        return;
    }

    static const EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, EGL_CONTEXT_CLIENT_VERSION_NUM, EGL_NONE };

    eglContext_ = eglCreateContext(eglDisplay_, config, EGL_NO_CONTEXT, contextAttribs);
    if (eglContext_ == EGL_NO_CONTEXT) {
        LOGE("Failed to create egl context %{public}x", eglGetError());
        return;
    }
    if (!eglMakeCurrent(eglDisplay_, EGL_NO_SURFACE, EGL_NO_SURFACE, eglContext_)) {
        LOGE("Failed to make current on surface, error is %{public}x", eglGetError());
        return;
    }

    LOGI("Create EGL context successfully, version %{public}d.%{public}d", major, minor);
}

void GpuContextTest::DestroyEGL()
{
    if (eglDisplay_ == EGL_NO_DISPLAY) {
        return;
    }

    eglDestroyContext(eglDisplay_, eglContext_);
    eglMakeCurrent(eglDisplay_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglTerminate(eglDisplay_);
    eglReleaseThread();

    eglDisplay_ = EGL_NO_DISPLAY;
    eglContext_ = EGL_NO_CONTEXT;
}

/**
 * @tc.name: GPUContextCreateTest001
 * @tc.desc: Test for creating GPUContext.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, GPUContextCreateTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
}

/**
 * @tc.name: GPUContextCreateTest001
 * @tc.desc: Test for creating a GL GPUContext for a backend context.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, BuildFromGLTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    GPUContextOptions options;
    EXPECT_TRUE(gpuContext->BuildFromGL(options));

    gpuContext->Flush();
    std::chrono::milliseconds msNotUsed;
    gpuContext->PerformDeferredCleanup(msNotUsed);
    int32_t maxResource = 100;
    size_t maxResourceBytes = 1000;
    gpuContext->GetResourceCacheLimits(&maxResource, &maxResourceBytes);
    gpuContext->SetResourceCacheLimits(maxResource, maxResourceBytes);
}

/**
 * @tc.name: GPUContextCreateTest002
 * @tc.desc: Test for creating a GL GPUContext for a backend context.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, BuildFromGLTest002, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    GPUContextOptions options;
    auto persistentCache = std::make_shared<ShaderPersistentCache>();
    options.SetPersistentCache(persistentCache.get());
    EXPECT_TRUE(gpuContext->BuildFromGL(options));
}

#ifdef RS_ENABLE_VK
/**
 * @tc.name: GPUContextCreateTest003
 * @tc.desc: Test for creating a VK GPUContext for a backend context.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, BuildFromVKTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
#ifdef USE_M133_SKIA
    skgpu::VulkanBackendContext grVkBackendContext;
#else
    GrVkBackendContext grVkBackendContext;
#endif
    ASSERT_FALSE(gpuContext->BuildFromVK(grVkBackendContext));
}

/**
 * @tc.name: GPUContextCreateTest004
 * @tc.desc: Test for creating a VK GPUContext for a backend context.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, BuildFromVKTest002, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
#ifdef USE_M133_SKIA
    skgpu::VulkanBackendContext grVkBackendContext;
#else
    GrVkBackendContext grVkBackendContext;
#endif
    GPUContextOptions options;
    options.SetAllowPathMaskCaching(true);
    ASSERT_FALSE(gpuContext->BuildFromVK(grVkBackendContext, options));
}

/**
 * @tc.name: GPUContextStoreVkPipelineCacheDataTest001
 * @tc.desc: Test for storing VK pipeline cache data.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, GPUContextStoreVkPipelineCacheDataTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    gpuContext->StoreVkPipelineCacheData();
}
#endif

/**
 * @tc.name: FlushTest001
 * @tc.desc: Test for flushing to underlying 3D API specific objects.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, FlushTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    gpuContext->Flush();
}

/**
 * @tc.name: PerformDeferredCleanupTest001
 * @tc.desc: Test for Purging GPU resources that haven't been used in the past 'msNotUsed' milliseconds.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, PerformDeferredCleanupTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    std::chrono::milliseconds msNotUsed;
    gpuContext->PerformDeferredCleanup(msNotUsed);
}

/**
 * @tc.name: GetResourceCacheLimitsTest001
 * @tc.desc: Test for geting the current GPU resource cache limits.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, GetResourceCacheLimitsTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    int32_t maxResource = 0;
    size_t maxResourceBytes = 0;
    gpuContext->GetResourceCacheLimits(&maxResource, &maxResourceBytes);
}

/**
 * @tc.name: GetResourceCacheLimitsTest002
 * @tc.desc: Test for geting the current GPU resource cache limits.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, GetResourceCacheLimitsTest002, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    int32_t maxResource = 10;
    size_t maxResourceBytes = 1000;
    gpuContext->GetResourceCacheLimits(&maxResource, &maxResourceBytes);
}

/**
 * @tc.name: SetResourceCacheLimitsTest001
 * @tc.desc: Test for set specify the GPU resource cache limits.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, SetResourceCacheLimitsTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    int32_t maxResource = 0;
    size_t maxResourceBytes = 0;
    gpuContext->SetResourceCacheLimits(maxResource, maxResourceBytes);
}

/**
 * @tc.name: SetResourceCacheLimitsTest002
 * @tc.desc: Test for set specify the GPU resource cache limits.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, SetResourceCacheLimitsTest002, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    int32_t maxResource = 100;
    size_t maxResourceBytes = 1000;
    gpuContext->SetResourceCacheLimits(maxResource, maxResourceBytes);
}

/**
 * @tc.name: ReleaseResourcesAndAbandonContextTest001
 * @tc.desc: Test for Purging GPU resources that haven't been used in the past 'msNotUsed' milliseconds.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, ReleaseResourcesAndAbandonContextTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    gpuContext->ReleaseResourcesAndAbandonContext();
}

/**
 * @tc.name: PurgeUnlockedResourcesByTagTest001
 * @tc.desc: Test for Purging GPU resources that haven't been used in the past 'msNotUsed' milliseconds.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, PurgeUnlockedResourcesByTagTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    GPUResourceTag tag(0, 0, 0, 0, "PurgeUnlockedResourcesByTagTest001");
    gpuContext->PurgeUnlockedResourcesByTag(true, tag);
}

/**
 * @tc.name: ReleaseByTagTest001
 * @tc.desc: Test for Purging GPU resources that haven't been used in the past 'msNotUsed' milliseconds.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, ReleaseByTagTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    GPUResourceTag tag(0, 0, 0, 0, "ReleaseByTagTest001");
    gpuContext->ReleaseByTag(tag);
}

/**
 * @tc.name: FlushAndSubmitTest001
 * @tc.desc: Test for flushing and submitting.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, FlushAndSubmitTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    gpuContext->FlushAndSubmit();
}

/**
 * @tc.name: GetResourceCacheUsageTest001
 * @tc.desc: Test for getting resource cache usage.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, GetResourceCacheUsageTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    int resourceCount;
    size_t resourceBytes;
    gpuContext->GetResourceCacheUsage(&resourceCount, &resourceBytes);
}

/**
 * @tc.name: DumpGpuStatsTest001
 * @tc.desc: Test for dumping gpu stats.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, DumpGpuStatsTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    std::string out;
    gpuContext->DumpGpuStats(out);
}

/**
 * @tc.name: PurgeUnlockedResourcesTest001
 * @tc.desc: Test for purging unlocked resources.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, PurgeUnlockedResourcesTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    gpuContext->PurgeUnlockedResources(true);
    gpuContext->PurgeUnlockedResources(false);
}

/**
 * @tc.name: PurgeUnlockedResourcesByPidTest001
 * @tc.desc: Test for purging unlocked resources by pid.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, PurgeUnlockedResourcesByPidTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    std::set<pid_t> exitedPidSet = { 0, 1, 100 };
    gpuContext->PurgeUnlockedResourcesByPid(true, exitedPidSet);
    gpuContext->PurgeUnlockedResourcesByPid(false, exitedPidSet);
}

/**
 * @tc.name: PurgeUnlockAndSafeCacheGpuResourcesTest001
 * @tc.desc: Test for purging unlocked and safe resources.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, PurgeUnlockAndSafeCacheGpuResourcesTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    gpuContext->PurgeUnlockAndSafeCacheGpuResources();
}

/**
 * @tc.name: PurgeCacheBetweenFramesTest001
 * @tc.desc: Test for purging cache betweem frames.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, PurgeCacheBetweenFramesTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    std::set<pid_t> exitedPidSet = { 0, 1, 100 };
    std::set<pid_t> protectedPidSet = { 0, 1 };
    gpuContext->PurgeCacheBetweenFrames(true, exitedPidSet, protectedPidSet);
    gpuContext->PurgeCacheBetweenFrames(false, exitedPidSet, protectedPidSet);
}

/**
 * @tc.name: DumpMemoryStatisticsByTagTest001
 * @tc.desc: Test for dumping memory statistics by tag.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, DumpMemoryStatisticsByTagTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    TraceMemoryDump traceMemoryDump("category", true);
    GPUResourceTag tag(0, 0, 0, 0, "tag");
    gpuContext->DumpMemoryStatisticsByTag(&traceMemoryDump, tag);
    gpuContext->DumpMemoryStatisticsByTag(nullptr, tag);
}

/**
 * @tc.name: DumpMemoryStatisticsTest001
 * @tc.desc: Test for dumping memory statistics.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, DumpMemoryStatisticsTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    TraceMemoryDump traceMemoryDump("category", true);
    gpuContext->DumpMemoryStatistics(&traceMemoryDump);
}

/**
 * @tc.name: GetUpdatedMemoryMapTest001
 * @tc.desc: Test for getting updated memory map.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, GetUpdatedMemoryMapTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    std::unordered_map<pid_t, size_t> out;
    gpuContext->GetUpdatedMemoryMap(out);
}

/**
 * @tc.name: InitGpuMemoryLimitTest001
 * @tc.desc: Test for initiating gpu memory limit.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, InitGpuMemoryLimitTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    uint64_t size = 1024 * 2;
    gpuContext->InitGpuMemoryLimit(nullptr, size);
}

/**
 * @tc.name: ResetContextTest001
 * @tc.desc: Test for resetting context.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, ResetContextTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    gpuContext->ResetContext();
}

/**
 * @tc.name: VmaDefragmentTest001
 * @tc.desc: Test for vma defragment.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, VmaDefragmentTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    gpuContext->VmaDefragment();
}

/**
 * @tc.name: BeginFrameTest001
 * @tc.desc: Test for beginning frame.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, BeginFrameTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    gpuContext->BeginFrame();
}

/**
 * @tc.name: EndFrameTest001
 * @tc.desc: Test for ending frame.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, EndFrameTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    gpuContext->EndFrame();
}

/**
 * @tc.name: SetGpuCacheSuppressWindowSwitchTest001
 * @tc.desc: Test for setting gpu cache suppress window switch.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, SetGpuCacheSuppressWindowSwitchTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    gpuContext->SetGpuCacheSuppressWindowSwitch(true);
    gpuContext->SetGpuCacheSuppressWindowSwitch(false);
}

/**
 * @tc.name: SetGpuMemoryAsyncReclaimerSwitchTest001
 * @tc.desc: Test for setting gpu memory async reclaimer switch.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, SetGpuMemoryAsyncReclaimerSwitchTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    gpuContext->SetGpuMemoryAsyncReclaimerSwitch(true, nullptr);
    gpuContext->SetGpuMemoryAsyncReclaimerSwitch(false, nullptr);
}

/**
 * @tc.name: FlushGpuMemoryInWaitQueueTest001
 * @tc.desc: Test for flushing gpu memory in wait queue.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, FlushGpuMemoryInWaitQueueTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    gpuContext->FlushGpuMemoryInWaitQueue();
}

/**
 * @tc.name: SuppressGpuCacheBelowCertainRatioTest001
 * @tc.desc: Test for suppressing gpu cache below certain ratio.
 * @tc.type: FUNC
 * @tc.require: I774GD
 */
HWTEST_F(GpuContextTest, SuppressGpuCacheBelowCertainRatioTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    gpuContext->SuppressGpuCacheBelowCertainRatio(nullptr);
}

/**
 * @tc.name: GetHpsEffectSupportTest001
 * @tc.desc: Test for get HPS effect support status.
 * @tc.type: FUNC
 * @tc.require: I774GD
*/
HWTEST_F(GpuContextTest, GetHpsEffectSupportTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    std::vector<const char*> extensionProperties;
    gpuContext->GetHpsEffectSupport(extensionProperties);
}

/**
 * @tc.name: RegisterVulkanErrorCallbackTest001
 * @tc.desc: Test for register vulkan error callback.
 * @tc.type: FUNC
 * @tc.require: IBOLWU
 */
HWTEST_F(GpuContextTest, RegisterVulkanErrorCallbackTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    gpuContext->RegisterVulkanErrorCallback(nullptr);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
