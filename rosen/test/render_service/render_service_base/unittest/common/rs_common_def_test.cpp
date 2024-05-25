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

#include <new>

#include "gtest/gtest.h"

#include "common/rs_common_def.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSCommonDefTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSCommonDefTest::SetUpTestCase() {}
void RSCommonDefTest::TearDownTestCase() {}
void RSCommonDefTest::SetUp() {}
void RSCommonDefTest::TearDown() {}

/**
 * @tc.name: New001
 * @tc.desc: test results of MemObject::operator new
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCommonDefTest, New001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. MemObject::operator new
     */
    MemObject* memObject = new (std::nothrow) MemObject(1);
    delete memObject;
}


/**
 * @tc.name: operatorTest
 * @tc.desc: test results of operator new operator delete
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSCommonDefTest, operatorTest, TestSize.Level1)
{
    auto* memObject = new MemObject(1);
    ASSERT_NE(memObject, nullptr);
    delete memObject;
}
} // namespace OHOS::Rosen