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
#include <test_header.h>

#include "surface_capture_param.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class SurfaceCaptureParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SurfaceCaptureParamTest::SetUpTestCase() {}
void SurfaceCaptureParamTest::TearDownTestCase() {}
void SurfaceCaptureParamTest::SetUp() {}
void SurfaceCaptureParamTest::TearDown() {}
/*
 * @tc.name: IsUseOptimizedFlushAndSubmitEnabled
 * @tc.desc: Test IsUseOptimizedFlushAndSubmitEnabled
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SurfaceCaptureParamTest, IsUseOptimizedFlushAndSubmitEnabled, Function | SmallTest | Level1)
{
    std::shared_ptr<SurfaceCaptureParam> surfaceCaptureParam = std::make_shared<SurfaceCaptureParam>();
    surfaceCaptureParam->SetUseOptimizedFlushAndSubmitEnabled(true);
    EXPECT_EQ(surfaceCaptureParam->IsUseOptimizedFlushAndSubmitEnabled(), true);
    surfaceCaptureParam->SetUseOptimizedFlushAndSubmitEnabled(false);
    EXPECT_EQ(surfaceCaptureParam->IsUseOptimizedFlushAndSubmitEnabled(), false);
}
} // namespace OHOS::Rosen