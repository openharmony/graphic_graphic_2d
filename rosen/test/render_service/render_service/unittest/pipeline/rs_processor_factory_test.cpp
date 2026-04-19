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

#include "gtest/gtest.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/rs_processor_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSProcessorFactoryTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSProcessorFactoryTest::SetUpTestCase() {}
void RSProcessorFactoryTest::TearDownTestCase() {}
void RSProcessorFactoryTest::SetUp() {}
void RSProcessorFactoryTest::TearDown() {}

/**
 * @tc.name: CreateAndDestroy001
 * @tc.desc: Check if RSProcessorFactory constructor and destructor is available
 * @tc.type: FUNC
 * @tc.require: issueI6PXHH
 */
HWTEST_F(RSProcessorFactoryTest, CreateAndDestroy001, TestSize.Level1)
{
    // The using of RSProcessorFactory constructor is not suggested, but allowed.
    // The using of RSProcessorFactory destructor is not suggested, but allowed.
    // Use its static function by :: first.
    RSProcessorFactory f;
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto p = f.CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    EXPECT_FALSE(nullptr == p);
}

/**
 * @tc.name: CreateProcessor001
 * @tc.desc: Create shared point of RSSoftwareProcessor
 * @tc.type: FUNC
 * @tc.require: issueI6PXHH
 */
HWTEST_F(RSProcessorFactoryTest, CreateProcessor001, TestSize.Level1)
{
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto p = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    EXPECT_FALSE(nullptr == p);
}

/**
 * @tc.name: CreateProcessor002
 * @tc.desc: Create shared point of RSHardwareProcessor
 * @tc.type: FUNC
 * @tc.require: issueI6PXHH
 */
HWTEST_F(RSProcessorFactoryTest, CreateProcessor002, TestSize.Level1)
{
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto p = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    EXPECT_FALSE(nullptr == p);
}

/**
 * @tc.name: CreateProcessor003
 * @tc.desc: Create shared point of UniRenderProcessor.
 * @tc.type: FUNC
 * @tc.require: issueI6PXHH
 */
HWTEST_F(RSProcessorFactoryTest, CreateProcessor003, TestSize.Level1)
{
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto p = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE, 0);
    EXPECT_TRUE(nullptr != p);
}

/**
 * @tc.name: CreateProcessor_SOFTWARE
 * @tc.desc: Create shared pointer of RSVirtualScreenProcessor via SOFTWARE_COMPOSITE
 * @tc.type: FUNC
 * @tc.require: issueIXXXXXX
 */
HWTEST_F(RSProcessorFactoryTest, CreateProcessor_SOFTWARE, TestSize.Level1)
{
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto p = RSProcessorFactory::CreateProcessor(CompositeType::SOFTWARE_COMPOSITE, 0);
    EXPECT_NE(p, nullptr);
}

/**
 * @tc.name: CreateProcessor_UNI_RENDER
 * @tc.desc: Create shared pointer for UNI_RENDER_COMPOSITE under GPU guard, else expect nullptr
 * @tc.type: FUNC
 * @tc.require: issueIXXXXXX
 */
HWTEST_F(RSProcessorFactoryTest, CreateProcessor_UNI_RENDER, TestSize.Level1)
{
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
#ifdef RS_ENABLE_GPU
    auto p = RSProcessorFactory::CreateProcessor(CompositeType::UNI_RENDER_COMPOSITE, 0);
    EXPECT_NE(p, nullptr);
#else
    auto p = RSProcessorFactory::CreateProcessor(CompositeType::UNI_RENDER_COMPOSITE, 0);
    EXPECT_EQ(p, nullptr);
#endif
}

/**
 * @tc.name: CreateProcessor_UNI_RENDER_VIRTUAL
 * @tc.desc: Create shared pointer for mirror/expand composite under GPU guard, else expect nullptr
 * @tc.type: FUNC
 * @tc.require: issueIXXXXXX
 */
HWTEST_F(RSProcessorFactoryTest, CreateProcessor_UNI_RENDER_VIRTUAL, TestSize.Level1)
{
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
#ifdef RS_ENABLE_GPU
    auto pm = RSProcessorFactory::CreateProcessor(CompositeType::UNI_RENDER_MIRROR_COMPOSITE, 0);
    auto pe = RSProcessorFactory::CreateProcessor(CompositeType::UNI_RENDER_EXPAND_COMPOSITE, 0);
    EXPECT_NE(pm, nullptr);
    EXPECT_NE(pe, nullptr);
#else
    auto pm = RSProcessorFactory::CreateProcessor(CompositeType::UNI_RENDER_MIRROR_COMPOSITE, 0);
    auto pe = RSProcessorFactory::CreateProcessor(CompositeType::UNI_RENDER_EXPAND_COMPOSITE, 0);
    EXPECT_EQ(pm, nullptr);
    EXPECT_EQ(pe, nullptr);
#endif
}

/**
 * @tc.name: CreateProcessor_DefaultBranch_ReturnsNull
 * @tc.desc: Passing an invalid CompositeType should hit default and return nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSProcessorFactoryTest, CreateProcessor_DefaultBranch_ReturnsNull, TestSize.Level1)
{
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto invalidType = static_cast<CompositeType>(99999);
    auto p = RSProcessorFactory::CreateProcessor(invalidType, 0);
    EXPECT_EQ(p, nullptr);
}
} // namespace OHOS::Rosen
