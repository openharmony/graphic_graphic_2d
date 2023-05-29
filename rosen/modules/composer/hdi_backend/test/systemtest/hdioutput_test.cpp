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

#include "hdi_output.h"
#include "hdilayer_context_systest.h"
#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::MockSys;

namespace OHOS {
namespace Rosen {
class HdiOutputSysTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static inline std::shared_ptr<HdiOutput> hdiOutput_;
};

void HdiOutputSysTest::SetUpTestCase()
{
    uint32_t screenId = 0;
    hdiOutput_ = HdiOutput::CreateHdiOutput(screenId);
    std::vector<LayerInfoPtr> layerInfos;
    int32_t width = 50;
    int32_t height = 50;
    GraphicIRect srcRect = {0, 0, width, height};
    GraphicIRect dstRect = {0, 0, width, height};
    uint32_t zOrder = 0;
    std::shared_ptr<HdiLayerContext> hdiLayerTemp = std::make_unique<HdiLayerContext>(dstRect, srcRect, zOrder);
    hdiLayerTemp->DrawBufferColor();
    hdiLayerTemp->FillHdiLayer();
    layerInfos.emplace_back(hdiLayerTemp->GetHdiLayer());
    zOrder = 1;
    hdiLayerTemp = std::make_unique<HdiLayerContext>(dstRect, srcRect, zOrder);
    hdiLayerTemp->DrawBufferColor();
    hdiLayerTemp->FillHdiLayer();
    layerInfos.emplace_back(hdiLayerTemp->GetHdiLayer());
    HdiOutputSysTest::hdiOutput_->SetLayerInfo(layerInfos);
}

void HdiOutputSysTest::TearDownTestCase() {}

namespace {
/*
* Function: TestHdiOutput001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call HdiOutputFunc
*                  2. check ret
*/
HWTEST_F(HdiOutputSysTest, TestHdiOutput001, Function | MediumTest| Level3)
{
    ASSERT_EQ(HdiOutputSysTest::hdiOutput_->GetScreenId(), 0u);
    ASSERT_EQ(HdiOutputSysTest::hdiOutput_->Init(), ROSEN_ERROR_OK);
    GraphicIRect iRect = {
        .x = 0,
        .y = 0,
        .w = 800,
        .h = 600,
    };
    std::vector<GraphicIRect> inDamages;
    inDamages.emplace_back(iRect);
    HdiOutputSysTest::hdiOutput_->SetOutputDamages(inDamages);
    const std::vector<GraphicIRect>& outDamages = HdiOutputSysTest::hdiOutput_->GetOutputDamages();
    ASSERT_EQ(outDamages.size(), 1);
    ASSERT_EQ(outDamages[0].x, iRect.x);
    ASSERT_EQ(outDamages[0].y, iRect.y);
    ASSERT_EQ(outDamages[0].w, iRect.w);
    ASSERT_EQ(outDamages[0].h, iRect.h);

    std::string dumpStr = "";
    HdiOutputSysTest::hdiOutput_->Dump(dumpStr);
    std::size_t found = dumpStr.find("FrameBufferSurface");
    ASSERT_NE(found, -1);

    std::string dumpFpsStr = "";
    HdiOutputSysTest::hdiOutput_->DumpFps(dumpFpsStr, "composer");
    HdiOutputSysTest::hdiOutput_->DumpFps(dumpFpsStr, "noname");
    found = dumpFpsStr.find("screen");
    ASSERT_NE(found, -1);

    std::string clearFpsStr = "";
    HdiOutputSysTest::hdiOutput_->ClearFpsDump(clearFpsStr, "composer");
    found = clearFpsStr.find("screen");
    ASSERT_NE(found, -1);

    ASSERT_NE(HdiOutputSysTest::hdiOutput_->GetFrameBufferSurface(), nullptr);
    ASSERT_EQ(HdiOutputSysTest::hdiOutput_->GetFramebuffer(), nullptr);
}
} // namespace
} // namespace Rosen
} // namespace OHOS