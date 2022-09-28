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

#include "hdi_layer.h"

#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class HdiLayerSysTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline std::shared_ptr<HdiLayer> hdiLayer_;
};

void HdiLayerSysTest::SetUpTestCase()
{
    hdiLayer_ = HdiLayer::CreateHdiLayer(0);
    LayerInfoPtr layerInfo;
    hdiLayer_->Init(layerInfo);
    hdiLayer_->UpdateLayerInfo(layerInfo);
}

void HdiLayerSysTest::TearDownTestCase() {}

namespace {
/*
* Function: TestHdiLayer001
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call SetLayerStatus()
*                  2. call GetLayerStatus()
*                  3. check ret
*/
HWTEST_F(HdiLayerSysTest, TestHdiLayer001, Function | MediumTest| Level3)
{
    bool isUsing = true;
    HdiLayerSysTest::hdiLayer_->SetLayerStatus(isUsing);
    ASSERT_EQ(HdiLayerSysTest::hdiLayer_->GetLayerStatus(), true);
}
} // namespace
} // namespace Rosen
} // namespace OHOS