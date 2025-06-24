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
#include <parameter.h>
#include <parameters.h>
#include <unistd.h>
#include "param/sys_param.h"
#include "hpae_base/rs_hpae_perf_thread.h"
#include "hpae_base/rs_hpae_base_types.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSHpaePerfThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline std::string hpaeSwitch;
    static inline std::string hpaeAaeSwitch;
};

void RSHpaePerfThreadTest::SetUpTestCase() {
    hpaeSwitch = OHOS::system::GetParameter("debug.graphic.hpae.blur.enabled", "0");
    hpaeAaeSwitch = OHOS::system::GetParameter("rosen.graphic.hpae.blur.aae.enabled", "0");
    OHOS::system::SetParameter("debug.graphic.hpae.blur.enabled", "1");
    OHOS::system::SetParameter("rosen.graphic.hpae.blur.aae.enabled", "1");
}
void RSHpaePerfThreadTest::TearDownTestCase()
{
    OHOS::system::SetParameter("debug.graphic.hpae.blur.enabled", hpaeSwitch);
    OHOS::system::SetParameter("rosen.graphic.hpae.blur.aae.enabled", hpaeAaeSwitch);
}

}
void RSHpaePerfThreadTest::SetUp() {}
void RSHpaePerfThreadTest::TearDown() {}

/**
 * @tc.name: PostTaskTest
 * @tc.desc: Verify function PostTask
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaePerfThreadTest, PostTaskTest, TestSize.Level1)
{
    RSHpaePerfThread hpaePerfThread;
    hpaePerfThread.PostTask([this]() {
        std::cout<<"RSHpaePerfThread::PostTaskTest"<<std::endl;
    });
    EXPECT_NE(hpaePerfThread.handler_, nullptr);
}

/**
 * @tc.name: PostTaskTest01
 * @tc.desc: Verify function PostTask
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaePerfThreadTest, PostTaskTest01, TestSize.Level1)
{
    RSHpaePerfThread hpaePerfThread;
    hpaePerfThread.handler_.reset();
    hpaePerfThread.PostTask([this]() {
        std::cout<<"RSHpaePerfThread::PostTaskTest"<<std::endl;
    });
    EXPECT_EQ(hpaePerfThread.handler_, nullptr);
}

/**
 * @tc.name: PostSyncTaskTest
 * @tc.desc: Verify function PostSyncTask
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaePerfThreadTest, PostSyncTaskTest, TestSize.Level1)
{
    RSHpaePerfThread hpaePerfThread;
    hpaePerfThread.PostSyncTask([this]() {
        std::cout<<"RSHpaePerfThread::PostSyncTaskTest"<<std::endl;
    });
    EXPECT_NE(hpaePerfThread.handler_, nullptr);
}

/**
 * @tc.name: PostSyncTaskTest01
 * @tc.desc: Verify function PostSyncTask
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaePerfThreadTest, PostSyncTaskTest01, TestSize.Level1)
{
    RSHpaePerfThread hpaePerfThread;
    hpaePerfThread.handler_.reset();
    hpaePerfThread.PostSyncTask([this]() {
        std::cout<<"RSHpaePerfThread::PostSyncTaskTest"<<std::endl;
    });
    EXPECT_EQ(hpaePerfThread.handler_, nullptr);
}

} // namespace Rosen
} // namespace OHOS