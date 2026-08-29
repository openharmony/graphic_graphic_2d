/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "parameters_defination.h"
#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"

#include "ui_effect/effect/include/colorful_brightness_blender.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class ColorfulBrightnessBlenderTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

GRAPHIC_TEST(ColorfulBrightnessBlenderTest, CONTENT_DISPLAY_TEST, Foreground_SetColorfulBrightnessBlender_Test_1)
{
    float rateList[] = { -0.05, 0.0, 1.0, 20.0 };
    float saturationList[] = { 0.0, 5.0, 10.0, 20.0 };
    float fractionVal = 0.5;
    Vector3f RGB[] = { { 2.3, 4.5, 2 }, { 0.5, 2, 0.5 } };
    for (int i = 0; i < FOUR_; i++) {
        for (int j = 0; j < FOUR_; j++) {
            for (int k = 0; k < TWO_; k++) {
                auto blenderPara = std::make_shared<ColorfulBrightnessBlender>();
                blenderPara->SetFraction(fractionVal);
                blenderPara->SetLinearRate(rateList[i]);
                blenderPara->SetDegree(rateList[(i + 3) % 4]);
                blenderPara->SetCubicRate(saturationList[j]);
                blenderPara->SetQuadRate(saturationList[(j + 1) % 4]);
                blenderPara->SetSaturation(saturationList[j]);
                blenderPara->SetPositiveCoeff(RGB[k]);
                blenderPara->SetNegativeCoeff(RGB[(k + 1) % 2]);
                blenderPara->SetDarkenWeight(0.5f);
                blenderPara->SetVibrancyStrength(0.5f);
                blenderPara->SetLumaDiff(0.3f);
                int x = i * THREE_HUNDRED_TEN_;
                int y = (k + j * 2) * THREE_HUNDRED_TEN_;
                auto testFaNode =
                    SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, THREE_HUNDRED_, THREE_HUNDRED_ });
                auto testNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
                testNode->SetBounds({ 0, 0, THREE_HUNDRED_, THREE_HUNDRED_ });
                testNode->SetForegroundColor(0xff7d112c);
                testNode->SetBlender(blenderPara.get());
                GetRootNode()->AddChild(testFaNode);
                testFaNode->AddChild(testNode);
                RegisterNode(testFaNode);
                RegisterNode(testNode);
            }
        }
    }
}

GRAPHIC_TEST(ColorfulBrightnessBlenderTest, CONTENT_DISPLAY_TEST, Foreground_SetColorfulBrightnessBlender_Test_2)
{
    float rateList[] = { -0.05, 0.0, 1.0, 20.0 };
    float saturationList[] = { 0.0, 5.0, 10.0, 20.0 };
    float fractionVal = 1.0;
    Vector3f RGB[] = { { 2.3, 4.5, 2 }, { 0.5, 2, 0.5 } };
    for (int i = 0; i < FOUR_; i++) {
        for (int j = 0; j < FOUR_; j++) {
            for (int k = 0; k < TWO_; k++) {
                auto blenderPara = std::make_shared<ColorfulBrightnessBlender>();
                blenderPara->SetFraction(fractionVal);
                blenderPara->SetLinearRate(rateList[i]);
                blenderPara->SetDegree(rateList[(i + 3) % 4]);
                blenderPara->SetCubicRate(saturationList[j]);
                blenderPara->SetQuadRate(saturationList[(j + 1) % 4]);
                blenderPara->SetSaturation(saturationList[j]);
                blenderPara->SetPositiveCoeff(RGB[k]);
                blenderPara->SetNegativeCoeff(RGB[(k + 1) % 2]);
                blenderPara->SetDarkenWeight(0.5f);
                blenderPara->SetVibrancyStrength(0.5f);
                blenderPara->SetLumaDiff(0.3f);
                int x = i * THREE_HUNDRED_TEN_;
                int y = (k + j * 2) * THREE_HUNDRED_TEN_;
                auto testFaNode =
                    SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, THREE_HUNDRED_, THREE_HUNDRED_ });
                auto testNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
                testNode->SetBounds({ 0, 0, THREE_HUNDRED_, THREE_HUNDRED_ });
                testNode->SetForegroundColor(0xff7d112c);
                testNode->SetBlender(blenderPara.get());
                GetRootNode()->AddChild(testFaNode);
                testFaNode->AddChild(testNode);
                RegisterNode(testFaNode);
                RegisterNode(testNode);
            }
        }
    }
}

GRAPHIC_TEST(ColorfulBrightnessBlenderTest, CONTENT_DISPLAY_TEST, Foreground_SetColorfulBrightnessBlender_Test_3)
{
    float rateList[] = { -0.05, 0.0, 1.0, 20.0 };
    float saturationList[] = { 0.0, 5.0, 10.0, 20.0 };
    float fractionVal = 0.0;
    Vector3f RGB[] = { { 2.3, 4.5, 2 }, { 0.5, 2, 0.5 } };
    for (int i = 0; i < FOUR_; i++) {
        for (int j = 0; j < FOUR_; j++) {
            for (int k = 0; k < TWO_; k++) {
                auto blenderPara = std::make_shared<ColorfulBrightnessBlender>();
                blenderPara->SetFraction(fractionVal);
                blenderPara->SetLinearRate(rateList[i]);
                blenderPara->SetDegree(rateList[(i + 3) % 4]);
                blenderPara->SetCubicRate(saturationList[j]);
                blenderPara->SetQuadRate(saturationList[(j + 1) % 4]);
                blenderPara->SetSaturation(saturationList[j]);
                blenderPara->SetPositiveCoeff(RGB[k]);
                blenderPara->SetNegativeCoeff(RGB[(k + 1) % 2]);
                blenderPara->SetDarkenWeight(0.5f);
                blenderPara->SetVibrancyStrength(0.5f);
                blenderPara->SetLumaDiff(0.3f);
                int x = i * THREE_HUNDRED_TEN_;
                int y = (k + j * 2) * THREE_HUNDRED_TEN_;
                auto testFaNode =
                    SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, THREE_HUNDRED_, THREE_HUNDRED_ });
                auto testNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
                testNode->SetBounds({ 0, 0, THREE_HUNDRED_, THREE_HUNDRED_ });
                testNode->SetForegroundColor(0xff7d112c);
                testNode->SetBlender(blenderPara.get());
                GetRootNode()->AddChild(testFaNode);
                testFaNode->AddChild(testNode);
                RegisterNode(testFaNode);
                RegisterNode(testNode);
            }
        }
    }
}

GRAPHIC_TEST(ColorfulBrightnessBlenderTest, CONTENT_DISPLAY_TEST,
    Foreground_ColorfulBrightnessBlender_DarkenWeight_Test)
{
    float darkenWeightList[] = { 0.0f, 0.5f, 1.0f, -1.0f, 2.0f };
    for (int i = 0; i < FIVE_; i++) {
        auto blenderPara = std::make_shared<ColorfulBrightnessBlender>();
        blenderPara->SetFraction(0.5f);
        blenderPara->SetDarkenWeight(darkenWeightList[i]);
        blenderPara->SetVibrancyStrength(0.5f);
        blenderPara->SetLumaDiff(0.3f);
        blenderPara->SetLinearRate(1.0f);
        blenderPara->SetSaturation(5.0f);
        int x = ONE_HUNDRED_;
        int y = i * THREE_HUNDRED_TEN_;
        auto testFaNode =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, THREE_HUNDRED_, THREE_HUNDRED_ });
        auto testNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        testNode->SetBounds({ 0, 0, THREE_HUNDRED_, THREE_HUNDRED_ });
        testNode->SetForegroundColor(0xff7d112c);
        testNode->SetBlender(blenderPara.get());
        GetRootNode()->AddChild(testFaNode);
        testFaNode->AddChild(testNode);
        RegisterNode(testFaNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ColorfulBrightnessBlenderTest, CONTENT_DISPLAY_TEST,
    Foreground_ColorfulBrightnessBlender_VibrancyLumaDiff_Test)
{
    float boundaryList[] = { 0.0f, 0.5f, 1.0f, -1.0f, 2.0f };
    for (int i = 0; i < FIVE_; i++) {
        auto blenderPara1 = std::make_shared<ColorfulBrightnessBlender>();
        blenderPara1->SetFraction(0.5f);
        blenderPara1->SetDarkenWeight(0.5f);
        blenderPara1->SetVibrancyStrength(boundaryList[i]);
        blenderPara1->SetLumaDiff(0.3f);
        blenderPara1->SetLinearRate(1.0f);
        blenderPara1->SetSaturation(5.0f);
        int x1 = ONE_HUNDRED_;
        int y = i * THREE_HUNDRED_TEN_;
        auto testFaNode1 =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x1, y, THREE_HUNDRED_, THREE_HUNDRED_ });
        auto testNode1 = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        testNode1->SetBounds({ 0, 0, THREE_HUNDRED_, THREE_HUNDRED_ });
        testNode1->SetForegroundColor(0xff7d112c);
        testNode1->SetBlender(blenderPara1.get());
        GetRootNode()->AddChild(testFaNode1);
        testFaNode1->AddChild(testNode1);
        RegisterNode(testFaNode1);
        RegisterNode(testNode1);

        auto blenderPara2 = std::make_shared<ColorfulBrightnessBlender>();
        blenderPara2->SetFraction(0.5f);
        blenderPara2->SetDarkenWeight(0.5f);
        blenderPara2->SetVibrancyStrength(0.5f);
        blenderPara2->SetLumaDiff(boundaryList[i]);
        blenderPara2->SetLinearRate(1.0f);
        blenderPara2->SetSaturation(5.0f);
        int x2 = FOUR_HUNDRED_TEN_;
        auto testFaNode2 =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x2, y, THREE_HUNDRED_, THREE_HUNDRED_ });
        auto testNode2 = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        testNode2->SetBounds({ 0, 0, THREE_HUNDRED_, THREE_HUNDRED_ });
        testNode2->SetForegroundColor(0xff7d112c);
        testNode2->SetBlender(blenderPara2.get());
        GetRootNode()->AddChild(testFaNode2);
        testFaNode2->AddChild(testNode2);
        RegisterNode(testFaNode2);
        RegisterNode(testNode2);
    }
}

GRAPHIC_TEST(ColorfulBrightnessBlenderTest, CONTENT_DISPLAY_TEST,
    Foreground_ColorfulBrightnessBlender_AbnormalParams_Test)
{
    float abnormalRateList[] = { -20.0f, -25.0f, 25.0f, 30.0f };
    float abnormalSatList[] = { -5.0f, 0.0f, 20.0f, 25.0f };
    for (int i = 0; i < FOUR_; i++) {
        auto blenderPara = std::make_shared<ColorfulBrightnessBlender>();
        blenderPara->SetFraction(0.5f);
        blenderPara->SetDarkenWeight(0.5f);
        blenderPara->SetVibrancyStrength(0.5f);
        blenderPara->SetLumaDiff(0.3f);
        blenderPara->SetLinearRate(abnormalRateList[i]);
        blenderPara->SetDegree(abnormalRateList[(i + 2) % 4]);
        blenderPara->SetCubicRate(abnormalRateList[(i + 1) % 4]);
        blenderPara->SetQuadRate(abnormalRateList[(i + 3) % 4]);
        blenderPara->SetSaturation(abnormalSatList[i]);
        blenderPara->SetPositiveCoeff({ abnormalRateList[i], 1.0f, abnormalRateList[(i + 1) % 4] });
        blenderPara->SetNegativeCoeff({ 1.0f, abnormalRateList[(i + 2) % 4], 0.5f });
        int x = ONE_HUNDRED_;
        int y = i * THREE_HUNDRED_TEN_;
        auto testFaNode =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, THREE_HUNDRED_, THREE_HUNDRED_ });
        auto testNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        testNode->SetBounds({ 0, 0, THREE_HUNDRED_, THREE_HUNDRED_ });
        testNode->SetForegroundColor(0xff7d112c);
        testNode->SetBlender(blenderPara.get());
        GetRootNode()->AddChild(testFaNode);
        testFaNode->AddChild(testNode);
        RegisterNode(testFaNode);
        RegisterNode(testNode);
    }
}

} // namespace OHOS::Rosen
