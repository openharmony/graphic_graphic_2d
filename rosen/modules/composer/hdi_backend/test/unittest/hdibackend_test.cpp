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
/**
 * @tc.name: RegScreenHotplug001
 * @tc.desc: Verify the RegScreenHotplug of hdibackend
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiBackendTest, RegScreenHotplug001, Function | MediumTest| Level3)
{
    ASSERT_EQ(HdiBackendTest::hdiBackend_->RegScreenHotplug(nullptr, nullptr), ROSEN_ERROR_INVALID_ARGUMENTS);
}

/**
 * @tc.name: RegPrepareComplete001
 * @tc.desc: Verify the RegPrepareComplete of hdibackend
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */

HWTEST_F(HdiBackendTest, RegPrepareComplete001, Function | MediumTest| Level3)
{
    auto func = [](std::shared_ptr<RSSurface> &surface, const struct PrepareCompleteParam &param, void* data) -> void {};
    ASSERT_EQ(HdiBackendTest::hdiBackend_->RegPrepareComplete(func, nullptr), ROSEN_ERROR_OK);
}

/**
 * @tc.name: RegPrepareComplete002
 * @tc.desc: Verify the RegPrepareComplete of hdibackend
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */

HWTEST_F(HdiBackendTest, RegPrepareComplete002, Function | MediumTest| Level3)
{
    ASSERT_EQ(HdiBackendTest::hdiBackend_->RegPrepareComplete(nullptr, nullptr), ROSEN_ERROR_INVALID_ARGUMENTS);
}
} // namespace
} // namespace Rosen
} // namespace OHOS