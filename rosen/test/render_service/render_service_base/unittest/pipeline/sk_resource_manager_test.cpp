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

#include "pipeline/rs_task_dispatcher.h"
#include "pipeline/sk_resource_manager.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class SKResourceManagerTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override;

private:
    SKResourceManager skResManager_;
};
void SKResourceManagerTest::TearDown()
{
    RSTaskDispatcher::GetInstance().taskDispatchFuncMap_.clear();
}

static inline void TaskDispatchFunc(const RSTaskDispatcher::RSTask& task, bool isSyncTask = false)
{
    ROSEN_LOGI("%{public}s:%{public}d TaskDispatchFunc!", __func__, __LINE__);
    if (task) {
        ROSEN_LOGI("%{public}s:%{public}d call task!", __func__, __LINE__);
        task();
    }
}

/**
 * @tc.name: HoldResourceImg001
 * @tc.desc: test hold resource of image
 * @tc.type: FUNC
 * @tc.require: issueI9IUKU
 */
HWTEST_F(SKResourceManagerTest, HoldResourceImg001, TestSize.Level1)
{
#ifdef ROSEN_OHOS
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        auto img = std::make_shared<Drawing::Image>();
        // case 1: Task is not registered, should return immediately
        skResManager_.HoldResource(img);
        EXPECT_TRUE(skResManager_.images_.empty());

        // case 2: Task is registered, the img push back to the images_
        RSTaskDispatcher::GetInstance().RegisterTaskDispatchFunc(gettid(), TaskDispatchFunc);
        skResManager_.HoldResource(img);
        EXPECT_EQ(skResManager_.images_.size(), 1);

        // case 3: the img already exists
        skResManager_.HoldResource(img);
        EXPECT_EQ(skResManager_.images_.size(), 1);
    }
#endif
}

/**
 * @tc.name: HoldResourceSurface001
 * @tc.desc: test hold resource surfacePtr
 * @tc.type: FUNC
 * @tc.require: issueI9IUKU
 */
HWTEST_F(SKResourceManagerTest, HoldResourceSurface001, TestSize.Level1)
{
#ifdef ROSEN_OHOS
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        auto surfacePtr = std::make_shared<Drawing::Surface>();
        // case 1: Task is not registered, should return immediately
        skResManager_.HoldResource(surfacePtr);
        EXPECT_TRUE(skResManager_.skSurfaces_.empty());

        // case 2: Task is registered, the surfacePtr push back to the skSurfaces_
        RSTaskDispatcher::GetInstance().RegisterTaskDispatchFunc(gettid(), TaskDispatchFunc);
        skResManager_.HoldResource(surfacePtr);
        EXPECT_EQ(skResManager_.skSurfaces_.size(), 1);

        // case 3: the surfacePtr already exists
        skResManager_.HoldResource(surfacePtr);
        EXPECT_EQ(skResManager_.skSurfaces_.size(), 1);
    }
#endif
}

/**
 * @tc.name: ReleaseResource001
 * @tc.desc: test release resource
 * @tc.type: FUNC
 * @tc.require: issueI9IUKU
 */
HWTEST_F(SKResourceManagerTest, ReleaseResource001, TestSize.Level1)
{
#ifdef ROSEN_OHOS
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        RSTaskDispatcher::GetInstance().RegisterTaskDispatchFunc(gettid(), TaskDispatchFunc);
        // case 1: images_ isn't empty
        {
            auto img = std::make_shared<Drawing::Image>();
            skResManager_.HoldResource(img);
            EXPECT_EQ(skResManager_.images_.size(), 1);
        }
        skResManager_.ReleaseResource();
        for (auto& images : skResManager_.images_) {
            EXPECT_TRUE(images.second->IsEmpty());
        }

        // case 2: skSurfaces_ isn't empty
        {
            auto validDrawingSurface = std::make_shared<Drawing::Surface>();
            skResManager_.HoldResource(validDrawingSurface);
            std::shared_ptr<Drawing::Surface> nullDrawingSurface;
            skResManager_.HoldResource(nullDrawingSurface);
            auto newValidDrawingSurface = std::make_shared<Drawing::Surface>();
            skResManager_.HoldResource(newValidDrawingSurface);
            EXPECT_EQ(skResManager_.skSurfaces_.size(), 1);
        }
        skResManager_.ReleaseResource();
        for (auto& skSurface : skResManager_.skSurfaces_) {
            EXPECT_TRUE(skSurface.second.empty());
        }
    }
#endif
}

/**
 * @tc.name: ReleaseResource002
 * @tc.desc: test release resource
 * @tc.type: FUNC
 * @tc.require: issueI9IUKU
 */
HWTEST_F(SKResourceManagerTest, ReleaseResource002, TestSize.Level1)
{
#ifdef ROSEN_OHOS
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        RSTaskDispatcher::GetInstance().RegisterTaskDispatchFunc(gettid(), TaskDispatchFunc);
        skResManager_.ReleaseResource();
        sleep(1); // make sure release clean

        {
            auto imgptr = std::make_shared<Drawing::Image>();
            skResManager_.HoldResource(imgptr);
            EXPECT_EQ(skResManager_.images_.size(), 1);
            for (auto& images : skResManager_.images_) {
                // resource stil hold by imgptr, so HaveReleaseableResourceCheck should reture false
                EXPECT_FALSE(images.second->HaveReleaseableResourceCheck());
            }

            imgptr = nullptr; // imgptr will not hold resource
            for (auto& images : skResManager_.images_) {
                // only skResManager_ hold resource, HaveReleaseableResourceCheck should reture true
                EXPECT_TRUE(images.second->HaveReleaseableResourceCheck());
            }
        }

        {
            auto surfacePtr = std::make_shared<Drawing::Surface>();
            skResManager_.HoldResource(surfacePtr);

            // resource stil hold by surfacePtr, so HaveReleaseableResourceCheck should reture false
            EXPECT_FALSE(skResManager_.HaveReleaseableResourceCheck(skResManager_.skSurfaces_[gettid()]));
            surfacePtr = nullptr;  // surfacePtr will not hold resource

            // only skResManager_ hold resource, HaveReleaseableResourceCheck should reture true
            EXPECT_TRUE(skResManager_.HaveReleaseableResourceCheck(skResManager_.skSurfaces_[gettid()]));
        }

        skResManager_.ReleaseResource();
        sleep(1); // make sure release clean

        EXPECT_FALSE(skResManager_.HaveReleaseableResourceCheck(skResManager_.skSurfaces_[gettid()]));
        for (auto& images : skResManager_.images_) {
            EXPECT_FALSE(images.second->HaveReleaseableResourceCheck());
        }
    }
#endif
}

/**
 * @tc.name: ReleaseResource003
 * @tc.desc: test release resource
 * @tc.type: FUNC
 * @tc.require: issueI9IUKU
 */
HWTEST_F(SKResourceManagerTest, ReleaseResource003, TestSize.Level1)
{
#ifdef ROSEN_OHOS
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        const  uint32_t MAX_CHECK_SIZE = 20;
        RSTaskDispatcher::GetInstance().RegisterTaskDispatchFunc(gettid(), TaskDispatchFunc);
        skResManager_.ReleaseResource();
        sleep(1); // make sure release clean

        {
            std::list<std::shared_ptr<Drawing::Image>> imagesMap;
            for (uint32_t i = 0; i < MAX_CHECK_SIZE; i++) {
                auto imgPtr = std::make_shared<Drawing::Image>();
                imagesMap.push_back(imgPtr);
                skResManager_.HoldResource(imgPtr);
            }
            for (auto& images : skResManager_.images_) {
                // imagesMap hold resource, so there is no resource need to release
                EXPECT_FALSE(images.second->HaveReleaseableResourceCheck());
            }
            skResManager_.HoldResource(std::make_shared<Drawing::Image>());
            for (auto& images : skResManager_.images_) {
                /* imagesMap hold resource. but size is over MAX_CHECK_SIZE
                * so there maybe have resource need to release
                */
                EXPECT_TRUE(images.second->HaveReleaseableResourceCheck());
            }
        }

        {
            std::list<std::shared_ptr<Drawing::Surface>> skSurfacesMap;
            for (uint32_t i = 0; i < MAX_CHECK_SIZE; i++) {
                auto surfacePtr = std::make_shared<Drawing::Surface>();
                skResManager_.HoldResource(surfacePtr);
                skSurfacesMap.push_back(surfacePtr);
            }
            // skSurfacesMap hold resource, so there is no resource need to release
            EXPECT_FALSE(skResManager_.HaveReleaseableResourceCheck(skResManager_.skSurfaces_[gettid()]));
            skResManager_.HoldResource(std::make_shared<Drawing::Surface>());

            /* skSurfacesMap hold resource. but size is over MAX_CHECK_SIZE
            * so there maybe have resource need to release
            */
            EXPECT_TRUE(skResManager_.HaveReleaseableResourceCheck(skResManager_.skSurfaces_[gettid()]));
        }

        skResManager_.ReleaseResource();
        sleep(1); // make sure release clean
        EXPECT_FALSE(skResManager_.HaveReleaseableResourceCheck(skResManager_.skSurfaces_[gettid()]));
        for (auto& images : skResManager_.images_) {
            EXPECT_FALSE(images.second->HaveReleaseableResourceCheck());
        }
    }
#endif
}
} // namespace OHOS::Rosen