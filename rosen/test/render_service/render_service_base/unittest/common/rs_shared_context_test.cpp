/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"

#include "render_context/render_context.h"

#include "common/rs_shared_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSSharedContextTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSharedContextTest::SetUpTestCase() {}
void RSSharedContextTest::TearDownTestCase() {}
void RSSharedContextTest::SetUp() {}
void RSSharedContextTest::TearDown() {}

/**
 * @tc.name: MakeSharedGLContext001
 * @tc.desc: test results of MakeSharedGLContext
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSharedContextTest, MakeSharedGLContext001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. MakeSharedGLContext
     */
#ifdef ACE_ENABLE_GPU
    RenderContext* renderContext = RenderContextFactory::GetInstance().CreateEngine();
    if (renderContext) {
        renderContext->InitializeEglContext();
        EGLContext eglContext = renderContext->GetEGLContext();
        RSSharedContext::MakeSharedGLContext(eglContext);
    }
    else {
        RSSharedContext::MakeSharedGLContext(EGL_NO_CONTEXT);
    }
#else
    RSSharedContext::MakeSharedGLContext(EGL_NO_CONTEXT);
#endif
}

/**
 * @tc.name: MakeCurrent001
 * @tc.desc: test results of MakeCurrent
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSharedContextTest, MakeCurrent001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. MakeCurrent
     */
#ifdef ACE_ENABLE_GPU
    RenderContext* renderContext = RenderContextFactory::GetInstance().CreateEngine();
    if (renderContext) {
        renderContext->InitializeEglContext();
        EGLContext eglContext = renderContext->GetEGLContext();
        std::shared_ptr<RSSharedContext> sharedGlContext = RSSharedContext::MakeSharedGLContext(eglContext);
        ASSERT_NE(sharedGlContext, nullptr);
        sharedGlContext->MakeCurrent();
    }
#endif
}

/**
 * @tc.name: MakeGrContext001
 * @tc.desc: test results of MakeGrContext
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSharedContextTest, MakeGrContext001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. MakeGrContext
     */
#ifdef ACE_ENABLE_GPU
    RenderContext* renderContext = RenderContextFactory::GetInstance().CreateEngine();
    if (renderContext) {
        renderContext->InitializeEglContext();
        EGLContext eglContext = renderContext->GetEGLContext();
        std::shared_ptr<RSSharedContext> sharedGlContext = RSSharedContext::MakeSharedGLContext(eglContext);
        ASSERT_NE(sharedGlContext, nullptr);
#ifndef USE_ROSEN_DRAWING
        sharedGlContext->MakeGrContext();
#else
        sharedGlContext->MakeDrContext();
#endif
    }
#endif
}
} // namespace OHOS::Rosen