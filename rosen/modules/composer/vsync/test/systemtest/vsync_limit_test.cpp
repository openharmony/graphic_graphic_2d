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
#include <gtest/gtest.h>
#include <unistd.h>
#include <vector>
#include "native_vsync.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
class VSyncLimitTest : public testing::Test {
};

/*
* Function: OH_NativeVSync_Create
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: NativeVSyncLimitTest
 */
HWTEST_F(VSyncLimitTest, NativeVSyncLimitTest, Function | MediumTest | Level2)
{
    std::vector<OH_NativeVSync *> nativeVsyncArray;
    char name[] = "vsyncLimitTest";
    for (int i = 0; i < 257; i++) {
        OH_NativeVSync *native_vsync = OH_NativeVSync_Create(name, sizeof(name));
        ASSERT_NE(native_vsync, nullptr);
        nativeVsyncArray.push_back(native_vsync);
    }
    OH_NativeVSync *nativeVsync = OH_NativeVSync_Create(name, sizeof(name));
    ASSERT_EQ(nativeVsync, nullptr);
    for (int i = 0; i < nativeVsyncArray.size(); i++) {
        OH_NativeVSync_Destroy(nativeVsyncArray[i]);
    }
}
}
}