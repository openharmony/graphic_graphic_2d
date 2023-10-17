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

#include "gtest/gtest.h"

#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "GLES3/gl32.h"

#include "draw/color.h"
#include "image/gpu_context.h"
#include "utils/log.h"

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
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
