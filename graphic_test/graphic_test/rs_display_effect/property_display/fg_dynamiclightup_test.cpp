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

#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class ForegroundTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, DynamicLightUp_Test_1)
{
    float ratesList[] = { 0, 0.1, 1.0, 5.0 };
    float degreeList[] = { -1.0, 0.0, 1.0 };
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            int x = j * 410;
            int y = i * 410;
            auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 400, 400 });
            testNode->SetDynamicLightUpRate(ratesList[i]);
            testNode->SetDynamicLightUpDegree(degreeList[j]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

} // namespace OHOS::Rosen