/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "hdi_framebuffer_surface.h"
#include "surface_buffer_impl.h"
#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class HdiFramebufferSurfaceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline sptr<HdiFramebufferSurface> hdiFramebufferSurface_ = nullptr;
};

void HdiFramebufferSurfaceTest::SetUpTestCase()
{
    hdiFramebufferSurface_ = HdiFramebufferSurface::CreateFramebufferSurface();
    hdiFramebufferSurface_->OnBufferAvailable();
}

void HdiFramebufferSurfaceTest::TearDownTestCase()
{
    hdiFramebufferSurface_ = nullptr;
}

namespace {
/*
* Function: ReleaseFramebuffer001
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call ReleaseFramebuffer
*                  2. check ret
*/
HWTEST_F(HdiFramebufferSurfaceTest, ReleaseFramebuffer001, Function | MediumTest| Level3)
{
    sptr<SurfaceBuffer> buffer = nullptr;
    sptr<SyncFence> fence = new SyncFence(10);
    ASSERT_EQ(hdiFramebufferSurface_->ReleaseFramebuffer(buffer, fence), 0);
    buffer = new SurfaceBufferImpl();
    ASSERT_NE(hdiFramebufferSurface_->ReleaseFramebuffer(buffer, fence), 0);
}

/*
* Function: GetBufferQueueSize001
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call GetBufferQueueSize
*                  2. check ret
*/
HWTEST_F(HdiFramebufferSurfaceTest, GetBufferQueueSize001, Function | MediumTest| Level3)
{
    ASSERT_EQ(hdiFramebufferSurface_->GetBufferQueueSize(), HdiFramebufferSurface::MAX_BUFFER_SIZE);
}

}
} // namespace Rosen
} // namespace OHOS