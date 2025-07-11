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

#include "gtest/gtest.h"
#include "parcel.h"
#include "rs_profiler.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSProfilerBaseTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

/*
* @tc.name: IsPlaybackParcel
* @tc.desc: Test IsPlaybackParcel
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(RSProfilerBaseTest, IsPlaybackParcel, Level1)
{
    RSProfiler::SetMode(Mode::READ_EMUL);
    Parcel parcel;
    EXPECT_FALSE(RSProfiler::IsPlaybackParcel(parcel));
}

} // namespace OHOS::Rosen