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

#include "common/rs_upload_texture_thread.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS::Rosen {
class RSUploadTextureThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUploadTextureThreadTest::SetUpTestCase() {}
void RSUploadTextureThreadTest::TearDownTestCase() {}
void RSUploadTextureThreadTest::SetUp() {}
void RSUploadTextureThreadTest::TearDown() {}

/**
 * @tc.name: testing
 * @tc.desc: test results of instance
 * @tc.type:FUNC
 * @tc.require:issueI5NMHT
 */
HWTEST_F(RSUploadTextureThreadTest, testing, TestSize.Level1)
{
    RSUploadTextureThread::Instance().PostTask(nullptr);
    RSUploadTextureThread::Instance().PostSyncTask(nullptr);
    std::string name = "name";
    RSUploadTextureThread::Instance().RemoveTask(name);
    EXPECT_EQ(0, RSUploadTextureThread::Instance().GetFrameCount());
}
} // namespace OHOS::Rosen