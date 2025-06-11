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
    float ratesList[] = { -1.0, 0, 0.1, 1.0, 5.0 };
    float degreeList[] = { -1.0, 0.0, 1.0, 5.0, 10.0 };
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            int x = j * 410;
            int y = i * 410;
            auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 400, 400 });
            testNode->SetDynamicLightUpRate(ratesList[0]);
            testNode->SetDynamicLightUpDegree(degreeList[0]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, DynamicLightUp_Test_2)
{
    float ratesList[] = { -1.0, 0, 0.1, 1.0, 5.0 };
    float degreeList[] = { -1.0, 0.0, 1.0, 5.0, 10.0 };
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            int x = j * 410;
            int y = i * 410;
            auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 400, 400 });
            testNode->SetDynamicLightUpRate(ratesList[0]);
            testNode->SetDynamicLightUpDegree(degreeList[1]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, DynamicLightUp_Test_3)
{
    float ratesList[] = { -1.0, 0, 0.1, 1.0, 5.0 };
    float degreeList[] = { -1.0, 0.0, 1.0, 5.0, 10.0 };
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            int x = j * 410;
            int y = i * 410;
            auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 400, 400 });
            testNode->SetDynamicLightUpRate(ratesList[0]);
            testNode->SetDynamicLightUpDegree(degreeList[2]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, DynamicLightUp_Test_4)
{
    float ratesList[] = { -1.0, 0, 0.1, 1.0, 5.0 };
    float degreeList[] = { -1.0, 0.0, 1.0, 5.0, 10.0 };
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            int x = j * 410;
            int y = i * 410;
            auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 400, 400 });
            testNode->SetDynamicLightUpRate(ratesList[0]);
            testNode->SetDynamicLightUpDegree(degreeList[3]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, DynamicLightUp_Test_5)
{
    float ratesList[] = { -1.0, 0, 0.1, 1.0, 5.0 };
    float degreeList[] = { -1.0, 0.0, 1.0, 5.0, 10.0 };
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            int x = j * 410;
            int y = i * 410;
            auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 400, 400 });
            testNode->SetDynamicLightUpRate(ratesList[0]);
            testNode->SetDynamicLightUpDegree(degreeList[4]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, DynamicLightUp_Test_6)
{
    float ratesList[] = { -1.0, 0, 0.1, 1.0, 5.0 };
    float degreeList[] = { -1.0, 0.0, 1.0, 5.0, 10.0 };
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            int x = j * 410;
            int y = i * 410;
            auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 400, 400 });
            testNode->SetDynamicLightUpRate(ratesList[1]);
            testNode->SetDynamicLightUpDegree(degreeList[0]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, DynamicLightUp_Test_7)
{
    float ratesList[] = { -1.0, 0, 0.1, 1.0, 5.0 };
    float degreeList[] = { -1.0, 0.0, 1.0, 5.0, 10.0 };
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            int x = j * 410;
            int y = i * 410;
            auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 400, 400 });
            testNode->SetDynamicLightUpRate(ratesList[1]);
            testNode->SetDynamicLightUpDegree(degreeList[1]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, DynamicLightUp_Test_8)
{
    float ratesList[] = { -1.0, 0, 0.1, 1.0, 5.0 };
    float degreeList[] = { -1.0, 0.0, 1.0, 5.0, 10.0 };
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            int x = j * 410;
            int y = i * 410;
            auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 400, 400 });
            testNode->SetDynamicLightUpRate(ratesList[1]);
            testNode->SetDynamicLightUpDegree(degreeList[2]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, DynamicLightUp_Test_9)
{
    float ratesList[] = { -1.0, 0, 0.1, 1.0, 5.0 };
    float degreeList[] = { -1.0, 0.0, 1.0, 5.0, 10.0 };
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            int x = j * 410;
            int y = i * 410;
            auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 400, 400 });
            testNode->SetDynamicLightUpRate(ratesList[1]);
            testNode->SetDynamicLightUpDegree(degreeList[3]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, DynamicLightUp_Test_10)
{
    float ratesList[] = { -1.0, 0, 0.1, 1.0, 5.0 };
    float degreeList[] = { -1.0, 0.0, 1.0, 5.0, 10.0 };
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            int x = j * 410;
            int y = i * 410;
            auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 400, 400 });
            testNode->SetDynamicLightUpRate(ratesList[1]);
            testNode->SetDynamicLightUpDegree(degreeList[4]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, DynamicLightUp_Test_11)
{
    float ratesList[] = { -1.0, 0, 0.1, 1.0, 5.0 };
    float degreeList[] = { -1.0, 0.0, 1.0, 5.0, 10.0 };
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            int x = j * 410;
            int y = i * 410;
            auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 400, 400 });
            testNode->SetDynamicLightUpRate(ratesList[2]);
            testNode->SetDynamicLightUpDegree(degreeList[0]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, DynamicLightUp_Test_12)
{
    float ratesList[] = { -1.0, 0, 0.1, 1.0, 5.0 };
    float degreeList[] = { -1.0, 0.0, 1.0, 5.0, 10.0 };
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            int x = j * 410;
            int y = i * 410;
            auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 400, 400 });
            testNode->SetDynamicLightUpRate(ratesList[2]);
            testNode->SetDynamicLightUpDegree(degreeList[1]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, DynamicLightUp_Test_13)
{
    float ratesList[] = { -1.0, 0, 0.1, 1.0, 5.0 };
    float degreeList[] = { -1.0, 0.0, 1.0, 5.0, 10.0 };
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            int x = j * 410;
            int y = i * 410;
            auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 400, 400 });
            testNode->SetDynamicLightUpRate(ratesList[2]);
            testNode->SetDynamicLightUpDegree(degreeList[2]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, DynamicLightUp_Test_14)
{
    float ratesList[] = { -1.0, 0, 0.1, 1.0, 5.0 };
    float degreeList[] = { -1.0, 0.0, 1.0, 5.0, 10.0 };
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            int x = j * 410;
            int y = i * 410;
            auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 400, 400 });
            testNode->SetDynamicLightUpRate(ratesList[2]);
            testNode->SetDynamicLightUpDegree(degreeList[3]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, DynamicLightUp_Test_15)
{
    float ratesList[] = { -1.0, 0, 0.1, 1.0, 5.0 };
    float degreeList[] = { -1.0, 0.0, 1.0, 5.0, 10.0 };
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            int x = j * 410;
            int y = i * 410;
            auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 400, 400 });
            testNode->SetDynamicLightUpRate(ratesList[2]);
            testNode->SetDynamicLightUpDegree(degreeList[4]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, DynamicLightUp_Test_16)
{
    float ratesList[] = { -1.0, 0, 0.1, 1.0, 5.0 };
    float degreeList[] = { -1.0, 0.0, 1.0, 5.0, 10.0 };
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            int x = j * 410;
            int y = i * 410;
            auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 400, 400 });
            testNode->SetDynamicLightUpRate(ratesList[3]);
            testNode->SetDynamicLightUpDegree(degreeList[0]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, DynamicLightUp_Test_17)
{
    float ratesList[] = { -1.0, 0, 0.1, 1.0, 5.0 };
    float degreeList[] = { -1.0, 0.0, 1.0, 5.0, 10.0 };
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            int x = j * 410;
            int y = i * 410;
            auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 400, 400 });
            testNode->SetDynamicLightUpRate(ratesList[3]);
            testNode->SetDynamicLightUpDegree(degreeList[1]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, DynamicLightUp_Test_18)
{
    float ratesList[] = { -1.0, 0, 0.1, 1.0, 5.0 };
    float degreeList[] = { -1.0, 0.0, 1.0, 5.0, 10.0 };
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            int x = j * 410;
            int y = i * 410;
            auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 400, 400 });
            testNode->SetDynamicLightUpRate(ratesList[3]);
            testNode->SetDynamicLightUpDegree(degreeList[2]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, DynamicLightUp_Test_19)
{
    float ratesList[] = { -1.0, 0, 0.1, 1.0, 5.0 };
    float degreeList[] = { -1.0, 0.0, 1.0, 5.0, 10.0 };
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            int x = j * 410;
            int y = i * 410;
            auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 400, 400 });
            testNode->SetDynamicLightUpRate(ratesList[3]);
            testNode->SetDynamicLightUpDegree(degreeList[3]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, DynamicLightUp_Test_20)
{
    float ratesList[] = { -1.0, 0, 0.1, 1.0, 5.0 };
    float degreeList[] = { -1.0, 0.0, 1.0, 5.0, 10.0 };
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            int x = j * 410;
            int y = i * 410;
            auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 400, 400 });
            testNode->SetDynamicLightUpRate(ratesList[3]);
            testNode->SetDynamicLightUpDegree(degreeList[4]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, DynamicLightUp_Test_21)
{
    float ratesList[] = { -1.0, 0, 0.1, 1.0, 5.0 };
    float degreeList[] = { -1.0, 0.0, 1.0, 5.0, 10.0 };
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            int x = j * 410;
            int y = i * 410;
            auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 400, 400 });
            testNode->SetDynamicLightUpRate(ratesList[4]);
            testNode->SetDynamicLightUpDegree(degreeList[0]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, DynamicLightUp_Test_22)
{
    float ratesList[] = { -1.0, 0, 0.1, 1.0, 5.0 };
    float degreeList[] = { -1.0, 0.0, 1.0, 5.0, 10.0 };
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            int x = j * 410;
            int y = i * 410;
            auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 400, 400 });
            testNode->SetDynamicLightUpRate(ratesList[4]);
            testNode->SetDynamicLightUpDegree(degreeList[1]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, DynamicLightUp_Test_23)
{
    float ratesList[] = { -1.0, 0, 0.1, 1.0, 5.0 };
    float degreeList[] = { -1.0, 0.0, 1.0, 5.0, 10.0 };
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            int x = j * 410;
            int y = i * 410;
            auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 400, 400 });
            testNode->SetDynamicLightUpRate(ratesList[4]);
            testNode->SetDynamicLightUpDegree(degreeList[2]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, DynamicLightUp_Test_24)
{
    float ratesList[] = { -1.0, 0, 0.1, 1.0, 5.0 };
    float degreeList[] = { -1.0, 0.0, 1.0, 5.0, 10.0 };
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            int x = j * 410;
            int y = i * 410;
            auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 400, 400 });
            testNode->SetDynamicLightUpRate(ratesList[4]);
            testNode->SetDynamicLightUpDegree(degreeList[3]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, DynamicLightUp_Test_25)
{
    float ratesList[] = { -1.0, 0, 0.1, 1.0, 5.0 };
    float degreeList[] = { -1.0, 0.0, 1.0, 5.0, 15.0 };
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            int x = j * 410;
            int y = i * 410;
            auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 400, 400 });
            testNode->SetDynamicLightUpRate(ratesList[4]);
            testNode->SetDynamicLightUpDegree(degreeList[4]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

} // namespace OHOS::Rosen