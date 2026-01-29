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

#include "gtest/gtest.h"

#include "feature/single_frame_composer/rs_single_frame_composer.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSSingleFrameComposerTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override;
    void TearDown() override;

private:
    RSSingleFrameComposer rSSingleFrameComposer_;
    std::shared_ptr<RSRenderProperty<Drawing::DrawCmdListPtr>> property_;
};
void RSSingleFrameComposerTest::SetUp() {}
void RSSingleFrameComposerTest::TearDown()
{
    RSSingleFrameComposer::ipcThreadIdMap_.clear();
}

/**
 * @tc.name: SetSingleFrameFlag001
 * @tc.desc: test set single frame flag
 * @tc.type: FUNC
 * @tc.require: issueI9IUKU
 */
HWTEST_F(RSSingleFrameComposerTest, SetSingleFrameFlag001, TestSize.Level1)
{
    std::thread::id ipcThreadId = 1024;
    RSSingleFrameComposer::SetSingleFrameFlag(ipcThreadId);
    EXPECT_EQ(RSSingleFrameComposer::ipcThreadIdMap_[ipcThreadId], 1);
    RSSingleFrameComposer::SetSingleFrameFlag(ipcThreadId);
    EXPECT_EQ(RSSingleFrameComposer::ipcThreadIdMap_[ipcThreadId], 2);
}

/**
 * @tc.name: IsShouldSingleFrameComposer001
 * @tc.desc: test is should single frame composer
 * @tc.type: FUNC
 * @tc.require: issueI9IUKU
 */
HWTEST_F(RSSingleFrameComposerTest, IsShouldSingleFrameComposer001, TestSize.Level1)
{
    auto ret = RSSingleFrameComposer::IsShouldSingleFrameComposer();
    EXPECT_TRUE(!ret);
    RSSingleFrameComposer::SetSingleFrameFlag(std::this_thread::get_id());
    ret = RSSingleFrameComposer::IsShouldSingleFrameComposer();
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: AddOrRemoveAppPidToMap001
 * @tc.desc: test add or remove app pid to map
 * @tc.type: FUNC
 * @tc.require: issueI9IUKU
 */
HWTEST_F(RSSingleFrameComposerTest, AddOrRemoveAppPidToMap001, TestSize.Level1)
{
    auto pid = getpid();
    RSSingleFrameComposer::AddOrRemoveAppPidToMap(true, pid);
    EXPECT_EQ(RSSingleFrameComposer::appPidMap_[pid], 1);
    RSSingleFrameComposer::AddOrRemoveAppPidToMap(false, pid);
    EXPECT_TRUE(RSSingleFrameComposer::appPidMap_.empty());
}

/**
 * @tc.name: IsShouldProcessByIpcThread001
 * @tc.desc: test is should process by ipc thread
 * @tc.type: FUNC
 * @tc.require: issueI9IUKU
 */
HWTEST_F(RSSingleFrameComposerTest, IsShouldProcessByIpcThread001, TestSize.Level1)
{
    auto pid = getpid();
    auto ret = RSSingleFrameComposer::IsShouldProcessByIpcThread(pid);
    EXPECT_TRUE(!ret);
    RSSingleFrameComposer::AddOrRemoveAppPidToMap(true, pid);
    ret = RSSingleFrameComposer::IsShouldProcessByIpcThread(pid);
    EXPECT_TRUE(ret);
}

} // namespace OHOS::Rosen