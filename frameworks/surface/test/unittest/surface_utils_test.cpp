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
#include <securec.h>
#include <gtest/gtest.h>
#include <display_type.h>
#include <surface.h>
#include <consumer_surface.h>
#include <surface_utils.h>
#include "buffer_consumer_listener.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class SurfaceUtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline sptr<Surface> csurface1 = nullptr;
    static inline sptr<IBufferProducer> producer1 = nullptr;
    static inline sptr<Surface> psurface1 = nullptr;

    static inline sptr<Surface> csurface2 = nullptr;
    static inline sptr<IBufferProducer> producer2 = nullptr;
    static inline sptr<Surface> psurface2 = nullptr;

    static inline SurfaceUtils *utils = nullptr;
};

void SurfaceUtilsTest::SetUpTestCase()
{
    csurface1 = Surface::CreateSurfaceAsConsumer();
    sptr<IBufferConsumerListener> listener1 = new BufferConsumerListener();
    csurface1->RegisterConsumerListener(listener1);
    producer1 = csurface1->GetProducer();
    psurface1 = Surface::CreateSurfaceAsProducer(producer1);

    csurface2 = Surface::CreateSurfaceAsConsumer();
    sptr<IBufferConsumerListener> listener2 = new BufferConsumerListener();
    csurface2->RegisterConsumerListener(listener2);
    producer2 = csurface2->GetProducer();
    psurface2 = Surface::CreateSurfaceAsProducer(producer2);
}

void SurfaceUtilsTest::TearDownTestCase()
{
    csurface1 = nullptr;
    producer1 = nullptr;
    psurface1 = nullptr;

    csurface2 = nullptr;
    producer2 = nullptr;
    psurface2 = nullptr;
    utils = nullptr;
}

/*
* Function: GetInstance
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetInstance
*                  2. check ret
 */
HWTEST_F(SurfaceUtilsTest, GetInstance001, Function | MediumTest | Level2)
{
    utils = SurfaceUtils::GetInstance();
    ASSERT_NE(utils, nullptr);
}

/*
* Function: GetInstance
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call Add
*                  2. check ret
 */
HWTEST_F(SurfaceUtilsTest, Add001, Function | MediumTest | Level2)
{
    GSError ret = utils->Add(psurface1->GetUniqueId(), nullptr);
    ASSERT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: GetInstance
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call Add 2 times
*                  2. check ret
 */
HWTEST_F(SurfaceUtilsTest, Add002, Function | MediumTest | Level2)
{
    GSError ret = utils->Add(psurface1->GetUniqueId(), psurface1);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = utils->Add(psurface1->GetUniqueId(), psurface1);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: GetSurface
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetSurface by abnormal uniqueId
*                  2. check ret
 */
HWTEST_F(SurfaceUtilsTest, GetSurface001, Function | MediumTest | Level2)
{
    sptr<Surface> surface = utils->GetSurface(0);
    ASSERT_EQ(surface, nullptr);
}

/*
* Function: GetSurface
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetSurface
*                  2. check ret
 */
HWTEST_F(SurfaceUtilsTest, GetSurface002, Function | MediumTest | Level2)
{
    sptr<Surface> surface1 = utils->GetSurface(psurface1->GetUniqueId());
    ASSERT_NE(surface1, nullptr);
}

/*
* Function: GetSurface
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetSurface
*                  2. call Add
*                  3. call GetSurface again
*                  4. check ret
 */
HWTEST_F(SurfaceUtilsTest, GetSurface003, Function | MediumTest | Level2)
{
    sptr<Surface> surface2 = utils->GetSurface(psurface2->GetUniqueId());
    ASSERT_EQ(surface2, nullptr);

    GSError ret = utils->Add(psurface2->GetUniqueId(), psurface2);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    surface2 = utils->GetSurface(psurface2->GetUniqueId());
    ASSERT_NE(surface2, nullptr);
}

/*
* Function: Remove
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call Remove
*                  2. check ret
 */
HWTEST_F(SurfaceUtilsTest, Remove001, Function | MediumTest | Level2)
{
    GSError ret = utils->Remove(0);
    ASSERT_EQ(ret, GSERROR_INVALID_OPERATING);
}

/*
* Function: Remove
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call Remove 2 times
*                  2. check ret
 */
HWTEST_F(SurfaceUtilsTest, Remove002, Function | MediumTest | Level2)
{
    GSError ret = utils->Remove(psurface1->GetUniqueId());
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = utils->Remove(psurface1->GetUniqueId());
    ASSERT_EQ(ret, SURFACE_ERROR_INVALID_OPERATING);
}
}