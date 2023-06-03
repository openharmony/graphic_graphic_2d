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

#include "hdi_backend.h"

#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class HdiBackendTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline HdiBackend* hdiBackend_ = nullptr;
};

void HdiBackendTest::SetUpTestCase()
{
    hdiBackend_ = HdiBackend::GetInstance();
}

void HdiBackendTest::TearDownTestCase() {}

namespace {


/*
* Function: RegScreenHotplug001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call RegScreenHotplug(nullptr, nullptr)
*                  2. check ret
*/
HWTEST_F(HdiBackendTest, RegScreenHotplug001, Function | MediumTest| Level3)
{
    ASSERT_EQ(HdiBackendTest::hdiBackend_->RegScreenHotplug(nullptr, nullptr), ROSEN_ERROR_INVALID_ARGUMENTS);
}

/*
* Function: RegScreenHotplug002
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call RegScreenHotplug(func, nullptr)
*                  2. check ret
*/
HWTEST_F(HdiBackendTest, RegScreenHotplug002, Function | MediumTest| Level3)
{
    auto func = [](OutputPtr &, bool, void*) -> void {};
    ASSERT_EQ(HdiBackendTest::hdiBackend_->RegScreenHotplug(func, nullptr), ROSEN_ERROR_OK);
}

/*
* Function: RegPrepareComplete001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call RegPrepareComplete(nullptr, nullptr)
*                  2. check ret
*/
HWTEST_F(HdiBackendTest, RegPrepareComplete001, Function | MediumTest| Level3)
{
    ASSERT_EQ(HdiBackendTest::hdiBackend_->RegPrepareComplete(nullptr, nullptr), ROSEN_ERROR_INVALID_ARGUMENTS);
}

/*
* Function: RegPrepareComplete002
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call RegPrepareComplete(func, nullptr)
*                  2. check ret
*/
HWTEST_F(HdiBackendTest, RegPrepareComplete002, Function | MediumTest| Level3)
{
    auto func = [](sptr<Surface> &, const struct PrepareCompleteParam &param, void* data) -> void {};
    ASSERT_EQ(HdiBackendTest::hdiBackend_->RegPrepareComplete(func, nullptr), ROSEN_ERROR_OK);
}

} // namespace
} // namespace Rosen
} // namespace OHOS