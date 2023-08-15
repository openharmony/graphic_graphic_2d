/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include<gtest/gtest.h>

//TODO：引入依赖文件
#include "ipc_security/rs_ipc_interface_code_access_verifier_base.h"
#include "platform/common/rs_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen{
class RSInterfaceCodeAccessVerifierBaseTest : public RSInterfaceCodeAccessVerifierBaseTest, testing::Test
{
public:
    RSInterfaceCodeAccessVerifierBaseTest() = default;
    ~RSInterfaceCodeAccessVerifierBaseTest() = default;

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    void InitializeAccessMap() override {};
    static inline std::shared_ptr<RSInterfaceCodeAccessVerifierBaseTest> rsinterfacecodeaccessverifierbase_ = nullptr;

protected:
    bool IsExtraVerificationPassed(CodeUnderlyingType code, const std::string& caller) override {};
};

void RSInterfaceCodeAccessVerifierBaseTest::SetupTestCase() {}
void RSInterfaceCodeAccessVerifierBaseTest::TearDownTestCase() {}
void RSInterfaceCodeAccessVerifierBaseTest::SetUp() {
    rsinterfacecodeaccessverifierbase_ = std::make_shared<RSInterfaceCodeAccessVerifierBaseTest>();
}
void RSInterfaceCodeAccessVerifierBaseTest::TearDown() {}

/**
 * @tc.name: IsInterfaceCodeAccessible001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfaceCodeAccessVerifierBaseTest, IsInterfaceCodeAccessible001, TestSize.Level1)
{
    CodeUnderlyingType code = 1;
    ASSERT_FALSE(rsinterfacecodeaccessverifierbase_->IsInterfaceCodeAccessible(code, "test"));
}

/**
 * @tc.name: IsInterfaceCodeAccessible002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfaceCodeAccessVerifierBaseTest, IsInterfaceCodeAccessible002, TestSize.Level1)
{
    CodeUnderlyingType code = 1;
    ASSERT_FALSE(rsinterfacecodeaccessverifierbase_->IsInterfaceCodeAccessible(code, ""));
}

/**
 * @tc.name: GetCallingFullTokenID001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfaceCodeAccessVerifierBaseTest, GetCallingFullTokenID001, TestSize.Level1)
{
    auto ret = IPCSkeleton::GetCallingFullTokenID();
    EXPECT_EQ(ret, rsinterfacecodeaccessverifierbase_->GetCallingFullTokenID());
}

/**
 * @tc.name: GetCallingFullTokenID002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfaceCodeAccessVerifierBaseTest, GetCallingFullTokenID002, TestSize.Level1)
{
    EXPECT_EQ(0, rsinterfacecodeaccessverifierbase_->GetCallingFullTokenID());
}


/**
 * @tc.name: IsSystemApp001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfaceCodeAccessVerifierBaseTest, IsSystemApp001, TestSize.Level1)
{
    TokenIdType tokenId = GetCallingFullTokenID();
    auto ret = Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(tokenId);
    EXPECT_FALSE(ret, rsinterfacecodeaccessverifierbase_->IsSystemApp());
}

/**
 * @tc.name: IsSystemApp002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfaceCodeAccessVerifierBaseTest, IsSystemApp002, TestSize.Level1)
{
    EXPECT_FALSE(0, rsinterfacecodeaccessverifierbase_->IsSystemApp());
}

} // namespace Rosen
} // namespace OHOS

