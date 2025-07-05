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

#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"
#include "ui_filter_test_data_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class UIFilterTest : public RSGraphicTest {
public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
        UIFilterTestDataManager::Initialize();
    }

private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;
    const int filterParaTypeCount = static_cast<int>(FilterPara::ParaType::CONTENT_LIGHT);
};

GRAPHIC_TEST(UIFilterTest, EFFECT_TEST, Set_UI_Filter_Null_Test)
{
    int columnCount = 1;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    int x = 0;
    int y = 0;
    auto foregroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    foregroundTestNode->SetUIForegroundFilter(nullptr);
    GetRootNode()->AddChild(foregroundTestNode);
    RegisterNode(foregroundTestNode);
    x = 0;
    y = sizeY;
    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    backgroundTestNode->SetUIBackgroundFilter(nullptr);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
    x = 0;
    y = 2 * sizeY;
    auto compositingTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    compositingTestNode->SetUICompositingFilter(nullptr);
    GetRootNode()->AddChild(compositingTestNode);
    RegisterNode(compositingTestNode);
}

GRAPHIC_TEST(UIFilterTest, EFFECT_TEST, Set_UI_Filter_Blur_Test)
{
    int columnCount = 3;
    int rowCount = static_cast<int>(TestDataGroupType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = (0 % columnCount) * sizeX;
        int y = i * sizeY;
        auto foregroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        auto filterObj = UIFilterTestDataManager::GetFilter(FilterPara::ParaType::BLUR,
            static_cast<TestDataGroupType>(i));
        foregroundTestNode->SetUIForegroundFilter(filterObj);
        GetRootNode()->AddChild(foregroundTestNode);
        RegisterNode(foregroundTestNode);
        x = (1 % columnCount) * sizeX;
        y = i * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetUIBackgroundFilter(filterObj);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        x = (2 % columnCount) * sizeX;
        y = i * sizeY;
        auto compositingTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        compositingTestNode->SetUICompositingFilter(filterObj);
        GetRootNode()->AddChild(compositingTestNode);
        RegisterNode(compositingTestNode);
    }
}

GRAPHIC_TEST(UIFilterTest, EFFECT_TEST, Set_UI_Filter_Pixel_Stretch_Test)
{
    int columnCount = 3;
    int rowCount = static_cast<int>(TestDataGroupType::COUNT);
    int scaleFactor = 3;
    int sizeX = screenWidth / columnCount;
    int sizeY = screenHeight / rowCount;
    int imgSizeX = sizeX / scaleFactor;
    int imgSizeY = sizeY / scaleFactor;
    int spaceSizeX = (sizeX - imgSizeX) / 2;
    int spaceSizeY = (sizeY - imgSizeY) / 2;
    for (int i = 0; i < rowCount; i++) {
        int x = spaceSizeX;
        int y = i * sizeY + spaceSizeY;
        auto foregroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, imgSizeX, imgSizeY });
        auto filterObj = UIFilterTestDataManager::GetFilter(FilterPara::ParaType::PIXEL_STRETCH,
            static_cast<TestDataGroupType>(i));
        foregroundTestNode->SetUIForegroundFilter(filterObj);
        GetRootNode()->AddChild(foregroundTestNode);
        RegisterNode(foregroundTestNode);
        x = (1 % columnCount) * sizeX + spaceSizeX;
        y = i * sizeY + spaceSizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, imgSizeX, imgSizeY });
        backgroundTestNode->SetUIBackgroundFilter(filterObj);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        x = (2 % columnCount) * sizeX + spaceSizeX;
        y = i * sizeY + spaceSizeY;
        auto compositingTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, imgSizeX, imgSizeY });
        compositingTestNode->SetUICompositingFilter(filterObj);
        GetRootNode()->AddChild(compositingTestNode);
        RegisterNode(compositingTestNode);
    }
}

GRAPHIC_TEST(UIFilterTest, EFFECT_TEST, Set_UI_Filter_Water_Ripple_Test)
{
    int columnCount = 3;
    int rowCount = static_cast<int>(TestDataGroupType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = (0 % columnCount) * sizeX;
        int y = i * sizeY;
        auto foregroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        auto filterObj = UIFilterTestDataManager::GetFilter(FilterPara::ParaType::WATER_RIPPLE,
            static_cast<TestDataGroupType>(i));
        foregroundTestNode->SetUIForegroundFilter(filterObj);
        GetRootNode()->AddChild(foregroundTestNode);
        RegisterNode(foregroundTestNode);
        x = (1 % columnCount) * sizeX;
        y = i * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetUIBackgroundFilter(filterObj);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        x = (2 % columnCount) * sizeX;
        y = i * sizeY;
        auto compositingTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        compositingTestNode->SetUICompositingFilter(filterObj);
        GetRootNode()->AddChild(compositingTestNode);
        RegisterNode(compositingTestNode);
    }
}

GRAPHIC_TEST(UIFilterTest, EFFECT_TEST, Set_UI_Filter_Fly_Out_Test)
{
    int columnCount = 3;
    int rowCount = static_cast<int>(TestDataGroupType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = (0 % columnCount) * sizeX;
        int y = i * sizeY;
        auto foregroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        auto filterObj = UIFilterTestDataManager::GetFilter(FilterPara::ParaType::FLY_OUT,
            static_cast<TestDataGroupType>(i));
        foregroundTestNode->SetUIForegroundFilter(filterObj);
        GetRootNode()->AddChild(foregroundTestNode);
        RegisterNode(foregroundTestNode);
        x = (1 % columnCount) * sizeX;
        y = i * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetUIBackgroundFilter(filterObj);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        x = (2 % columnCount) * sizeX;
        y = i * sizeY;
        auto compositingTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        compositingTestNode->SetUICompositingFilter(filterObj);
        GetRootNode()->AddChild(compositingTestNode);
        RegisterNode(compositingTestNode);
    }
}

GRAPHIC_TEST(UIFilterTest, EFFECT_TEST, Set_UI_Filter_Distort_Test)
{
    int columnCount = 3;
    int rowCount = static_cast<int>(TestDataGroupType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = (0 % columnCount) * sizeX;
        int y = i * sizeY;
        auto foregroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        auto filterObj = UIFilterTestDataManager::GetFilter(FilterPara::ParaType::DISTORT,
            static_cast<TestDataGroupType>(i));
        foregroundTestNode->SetUIForegroundFilter(filterObj);
        GetRootNode()->AddChild(foregroundTestNode);
        RegisterNode(foregroundTestNode);
        x = (1 % columnCount) * sizeX;
        y = i * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetUIBackgroundFilter(filterObj);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        x = (2 % columnCount) * sizeX;
        y = i * sizeY;
        auto compositingTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        compositingTestNode->SetUICompositingFilter(filterObj);
        GetRootNode()->AddChild(compositingTestNode);
        RegisterNode(compositingTestNode);
    }
}

GRAPHIC_TEST(UIFilterTest, EFFECT_TEST, Set_UI_Filter_Radius_Gradient_Blur_Test)
{
    int columnCount = 3;
    int rowCount = static_cast<int>(TestDataGroupType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = (0 % columnCount) * sizeX;
        int y = i * sizeY;
        auto foregroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        auto filterObj = UIFilterTestDataManager::GetFilter(FilterPara::ParaType::RADIUS_GRADIENT_BLUR,
            static_cast<TestDataGroupType>(i));
        foregroundTestNode->SetUIForegroundFilter(filterObj);
        GetRootNode()->AddChild(foregroundTestNode);
        RegisterNode(foregroundTestNode);
        x = (1 % columnCount) * sizeX;
        y = i * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetUIBackgroundFilter(filterObj);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        x = (2 % columnCount) * sizeX;
        y = i * sizeY;
        auto compositingTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        compositingTestNode->SetUICompositingFilter(filterObj);
        GetRootNode()->AddChild(compositingTestNode);
        RegisterNode(compositingTestNode);
    }
}

GRAPHIC_TEST(UIFilterTest, EFFECT_TEST, Set_UI_Filter_Displacement_Distort_Test)
{
    int columnCount = 3;
    int rowCount = static_cast<int>(TestDataGroupType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = (0 % columnCount) * sizeX;
        int y = i * sizeY;
        auto foregroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        auto filterObj = UIFilterTestDataManager::GetFilter(FilterPara::ParaType::DISPLACEMENT_DISTORT,
            static_cast<TestDataGroupType>(i));
        foregroundTestNode->SetUIForegroundFilter(filterObj);
        GetRootNode()->AddChild(foregroundTestNode);
        RegisterNode(foregroundTestNode);
        x = (1 % columnCount) * sizeX;
        y = i * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetUIBackgroundFilter(filterObj);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        x = (2 % columnCount) * sizeX;
        y = i * sizeY;
        auto compositingTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        compositingTestNode->SetUICompositingFilter(filterObj);
        GetRootNode()->AddChild(compositingTestNode);
        RegisterNode(compositingTestNode);
    }
}

GRAPHIC_TEST(UIFilterTest, EFFECT_TEST, Set_UI_Filter_Color_Gradient_Test)
{
    int columnCount = 3;
    int rowCount = static_cast<int>(TestDataGroupType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = (0 % columnCount) * sizeX;
        int y = i * sizeY;
        auto foregroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        auto filterObj = UIFilterTestDataManager::GetFilter(FilterPara::ParaType::COLOR_GRADIENT,
            static_cast<TestDataGroupType>(i));
        foregroundTestNode->SetUIForegroundFilter(filterObj);
        GetRootNode()->AddChild(foregroundTestNode);
        RegisterNode(foregroundTestNode);
        x = (1 % columnCount) * sizeX;
        y = i * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetUIBackgroundFilter(filterObj);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        x = (2 % columnCount) * sizeX;
        y = i * sizeY;
        auto compositingTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        compositingTestNode->SetUICompositingFilter(filterObj);
        GetRootNode()->AddChild(compositingTestNode);
        RegisterNode(compositingTestNode);
    }
}

GRAPHIC_TEST(UIFilterTest, EFFECT_TEST, Set_UI_Filter_Sound_Wave_Test)
{
    int columnCount = 3;
    int rowCount = static_cast<int>(TestDataGroupType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = (0 % columnCount) * sizeX;
        int y = i * sizeY;
        auto foregroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        auto filterObj = UIFilterTestDataManager::GetFilter(FilterPara::ParaType::SOUND_WAVE,
            static_cast<TestDataGroupType>(i));
        foregroundTestNode->SetUIForegroundFilter(filterObj);
        GetRootNode()->AddChild(foregroundTestNode);
        RegisterNode(foregroundTestNode);
        x = (1 % columnCount) * sizeX;
        y = i * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetUIBackgroundFilter(filterObj);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        x = (2 % columnCount) * sizeX;
        y = i * sizeY;
        auto compositingTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        compositingTestNode->SetUICompositingFilter(filterObj);
        GetRootNode()->AddChild(compositingTestNode);
        RegisterNode(compositingTestNode);
    }
}

GRAPHIC_TEST(UIFilterTest, EFFECT_TEST, Set_UI_Filter_Edge_Light_Test)
{
    int columnCount = 3;
    int rowCount = static_cast<int>(TestDataGroupType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = (0 % columnCount) * sizeX;
        int y = i * sizeY;
        auto foregroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        auto filterObj = UIFilterTestDataManager::GetFilter(FilterPara::ParaType::EDGE_LIGHT,
            static_cast<TestDataGroupType>(i));
        foregroundTestNode->SetUIForegroundFilter(filterObj);
        GetRootNode()->AddChild(foregroundTestNode);
        RegisterNode(foregroundTestNode);
        x = (1 % columnCount) * sizeX;
        y = i * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetUIBackgroundFilter(filterObj);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        x = (2 % columnCount) * sizeX;
        y = i * sizeY;
        auto compositingTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        compositingTestNode->SetUICompositingFilter(filterObj);
        GetRootNode()->AddChild(compositingTestNode);
        RegisterNode(compositingTestNode);
    }
}

GRAPHIC_TEST(UIFilterTest, EFFECT_TEST, Set_UI_Filter_Bezier_Warp_Test)
{
    int columnCount = 3;
    int rowCount = static_cast<int>(TestDataGroupType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = (0 % columnCount) * sizeX;
        int y = i * sizeY;
        auto foregroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        auto filterObj = UIFilterTestDataManager::GetFilter(FilterPara::ParaType::BEZIER_WARP,
            static_cast<TestDataGroupType>(i));
        foregroundTestNode->SetUIForegroundFilter(filterObj);
        GetRootNode()->AddChild(foregroundTestNode);
        RegisterNode(foregroundTestNode);
        x = (1 % columnCount) * sizeX;
        y = i * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetUIBackgroundFilter(filterObj);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        x = (2 % columnCount) * sizeX;
        y = i * sizeY;
        auto compositingTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        compositingTestNode->SetUICompositingFilter(filterObj);
        GetRootNode()->AddChild(compositingTestNode);
        RegisterNode(compositingTestNode);
    }
}

GRAPHIC_TEST(UIFilterTest, EFFECT_TEST, Set_UI_Filter_Dispersion_Test)
{
    int columnCount = 3;
    int rowCount = static_cast<int>(TestDataGroupType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = (0 % columnCount) * sizeX;
        int y = i * sizeY;
        auto foregroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        auto filterObj = UIFilterTestDataManager::GetFilter(FilterPara::ParaType::DISPERSION,
            static_cast<TestDataGroupType>(i));
        foregroundTestNode->SetUIForegroundFilter(filterObj);
        GetRootNode()->AddChild(foregroundTestNode);
        RegisterNode(foregroundTestNode);
        x = (1 % columnCount) * sizeX;
        y = i * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetUIBackgroundFilter(filterObj);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        x = (2 % columnCount) * sizeX;
        y = i * sizeY;
        auto compositingTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        compositingTestNode->SetUICompositingFilter(filterObj);
        GetRootNode()->AddChild(compositingTestNode);
        RegisterNode(compositingTestNode);
    }
}

GRAPHIC_TEST(UIFilterTest, EFFECT_TEST, Set_UI_Filter_Hdr_Brightness_Ratio_Test)
{
    int columnCount = 3;
    int rowCount = static_cast<int>(TestDataGroupType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = (0 % columnCount) * sizeX;
        int y = i * sizeY;
        auto foregroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        auto filterObj = UIFilterTestDataManager::GetFilter(FilterPara::ParaType::HDR_BRIGHTNESS_RATIO,
            static_cast<TestDataGroupType>(i));
        foregroundTestNode->SetUIForegroundFilter(filterObj);
        GetRootNode()->AddChild(foregroundTestNode);
        RegisterNode(foregroundTestNode);
        x = (1 % columnCount) * sizeX;
        y = i * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetUIBackgroundFilter(filterObj);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        x = (2 % columnCount) * sizeX;
        y = i * sizeY;
        auto compositingTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        compositingTestNode->SetUICompositingFilter(filterObj);
        GetRootNode()->AddChild(compositingTestNode);
        RegisterNode(compositingTestNode);
    }
}

GRAPHIC_TEST(UIFilterTest, EFFECT_TEST, Set_UI_Filter_Content_Light_Test)
{
    int columnCount = 3;
    int rowCount = static_cast<int>(TestDataGroupType::COUNT);
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = (0 % columnCount) * sizeX;
        int y = i * sizeY;
        auto foregroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        auto filterObj = UIFilterTestDataManager::GetFilter(FilterPara::ParaType::CONTENT_LIGHT,
            static_cast<TestDataGroupType>(i));
        foregroundTestNode->SetUIForegroundFilter(filterObj);
        GetRootNode()->AddChild(foregroundTestNode);
        RegisterNode(foregroundTestNode);
        x = (1 % columnCount) * sizeX;
        y = i * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetUIBackgroundFilter(filterObj);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        x = (2 % columnCount) * sizeX;
        y = i * sizeY;
        auto compositingTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        compositingTestNode->SetUICompositingFilter(filterObj);
        GetRootNode()->AddChild(compositingTestNode);
        RegisterNode(compositingTestNode);
    }
}

GRAPHIC_TEST(UIFilterTest, EFFECT_TEST, Set_UI_Filter_Combined_All_Test1)
{
    int columnCount = 1;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    vector<FilterPara::ParaType> testParaVec;
    for (int i = 0; i < filterParaTypeCount; i++) {
        testParaVec.push_back(static_cast<FilterPara::ParaType>(i + 1));
    }
    int x = 0;
    int y = 0;
    auto combinedFilter = UIFilterTestDataManager::GetCombinedFilter(testParaVec, TestDataGroupType::VALID_DATA1);
    auto foregroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    foregroundTestNode->SetUIForegroundFilter(combinedFilter);
    GetRootNode()->AddChild(foregroundTestNode);
    RegisterNode(foregroundTestNode);
    x = 0;
    y = sizeY;
    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    backgroundTestNode->SetUIBackgroundFilter(combinedFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
    x = 0;
    y = 2 * sizeY;
    auto compositingTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    compositingTestNode->SetUICompositingFilter(combinedFilter);
    GetRootNode()->AddChild(compositingTestNode);
    RegisterNode(compositingTestNode);
}

GRAPHIC_TEST(UIFilterTest, EFFECT_TEST, Set_UI_Filter_Combined_All_Test2)
{
    int columnCount = 1;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    vector<FilterPara::ParaType> testParaVec;
    for (int i = 0; i < filterParaTypeCount; i++) {
        testParaVec.push_back(static_cast<FilterPara::ParaType>(i + 1));
    }
    int x = 0;
    int y = 0;
    auto combinedFilter = UIFilterTestDataManager::GetCombinedFilter(testParaVec, TestDataGroupType::VALID_DATA2);
    auto foregroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    foregroundTestNode->SetUIForegroundFilter(combinedFilter);
    GetRootNode()->AddChild(foregroundTestNode);
    RegisterNode(foregroundTestNode);
    x = 0;
    y = sizeY;
    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    backgroundTestNode->SetUIBackgroundFilter(combinedFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
    x = 0;
    y = 2 * sizeY;
    auto compositingTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    compositingTestNode->SetUICompositingFilter(combinedFilter);
    GetRootNode()->AddChild(compositingTestNode);
    RegisterNode(compositingTestNode);
}

GRAPHIC_TEST(UIFilterTest, EFFECT_TEST, Set_UI_Filter_Combined_All_Test3)
{
    int columnCount = 1;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    vector<FilterPara::ParaType> testParaVec;
    for (int i = 0; i < filterParaTypeCount; i++) {
        testParaVec.push_back(static_cast<FilterPara::ParaType>(i + 1));
    }
    int x = 0;
    int y = 0;
    auto combinedFilter = UIFilterTestDataManager::GetCombinedFilter(testParaVec, TestDataGroupType::VALID_DATA3);
    auto foregroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    foregroundTestNode->SetUIForegroundFilter(combinedFilter);
    GetRootNode()->AddChild(foregroundTestNode);
    RegisterNode(foregroundTestNode);
    x = 0;
    y = sizeY;
    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    backgroundTestNode->SetUIBackgroundFilter(combinedFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
    x = 0;
    y = 2 * sizeY;
    auto compositingTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    compositingTestNode->SetUICompositingFilter(combinedFilter);
    GetRootNode()->AddChild(compositingTestNode);
    RegisterNode(compositingTestNode);
}

GRAPHIC_TEST(UIFilterTest, EFFECT_TEST, Set_UI_Filter_Combined_All_Test4)
{
    int columnCount = 1;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    vector<FilterPara::ParaType> testParaVec;
    for (int i = 0; i < filterParaTypeCount; i++) {
        testParaVec.push_back(static_cast<FilterPara::ParaType>(i + 1));
    }
    int x = 0;
    int y = 0;
    auto combinedFilter = UIFilterTestDataManager::GetCombinedFilter(testParaVec, TestDataGroupType::VALID_DATA4);
    auto foregroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    foregroundTestNode->SetUIForegroundFilter(combinedFilter);
    GetRootNode()->AddChild(foregroundTestNode);
    RegisterNode(foregroundTestNode);
    x = 0;
    y = sizeY;
    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    backgroundTestNode->SetUIBackgroundFilter(combinedFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
    x = 0;
    y = 2 * sizeY;
    auto compositingTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
    compositingTestNode->SetUICompositingFilter(combinedFilter);
    GetRootNode()->AddChild(compositingTestNode);
    RegisterNode(compositingTestNode);
}

GRAPHIC_TEST(UIFilterTest, EFFECT_TEST, Set_UI_Filter_Combined_Blur_Two_Test)
{
    int columnCount = 3;
    int rowCount = filterParaTypeCount;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = 0 * sizeX;
        int y = i * sizeY;
        vector<FilterPara::ParaType> testParaVec = {
            FilterPara::ParaType::BLUR,
            static_cast<FilterPara::ParaType>(i + 1)
        };
        auto combinedFilter = UIFilterTestDataManager::GetCombinedFilter(testParaVec, TestDataGroupType::VALID_DATA2);
        auto foregroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        foregroundTestNode->SetUIForegroundFilter(combinedFilter);
        GetRootNode()->AddChild(foregroundTestNode);
        RegisterNode(foregroundTestNode);
        x = sizeX;
        y = i * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetUIBackgroundFilter(combinedFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        x = 2 * sizeX;
        y = i * sizeY;
        auto compositingTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        compositingTestNode->SetUICompositingFilter(combinedFilter);
        GetRootNode()->AddChild(compositingTestNode);
        RegisterNode(compositingTestNode);
    }
}

GRAPHIC_TEST(UIFilterTest, EFFECT_TEST, Set_UI_Filter_Combined_Pixel_Stretch_Two_Test)
{
    int columnCount = 3;
    int rowCount = filterParaTypeCount;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = 0 * sizeX;
        int y = i * sizeY;
        vector<FilterPara::ParaType> testParaVec = {
            FilterPara::ParaType::PIXEL_STRETCH,
            static_cast<FilterPara::ParaType>(i + 1)
        };
        auto combinedFilter = UIFilterTestDataManager::GetCombinedFilter(testParaVec, TestDataGroupType::VALID_DATA2);
        auto foregroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        foregroundTestNode->SetUIForegroundFilter(combinedFilter);
        GetRootNode()->AddChild(foregroundTestNode);
        RegisterNode(foregroundTestNode);
        x = sizeX;
        y = i * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetUIBackgroundFilter(combinedFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        x = 2 * sizeX;
        y = i * sizeY;
        auto compositingTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        compositingTestNode->SetUICompositingFilter(combinedFilter);
        GetRootNode()->AddChild(compositingTestNode);
        RegisterNode(compositingTestNode);
    }
}

GRAPHIC_TEST(UIFilterTest, EFFECT_TEST, Set_UI_Filter_Combined_Water_Ripple_Two_Test)
{
    int columnCount = 3;
    int rowCount = filterParaTypeCount;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = 0 * sizeX;
        int y = i * sizeY;
        vector<FilterPara::ParaType> testParaVec = {
            FilterPara::ParaType::WATER_RIPPLE,
            static_cast<FilterPara::ParaType>(i + 1)
        };
        auto combinedFilter = UIFilterTestDataManager::GetCombinedFilter(testParaVec, TestDataGroupType::VALID_DATA2);
        auto foregroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        foregroundTestNode->SetUIForegroundFilter(combinedFilter);
        GetRootNode()->AddChild(foregroundTestNode);
        RegisterNode(foregroundTestNode);
        x = sizeX;
        y = i * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetUIBackgroundFilter(combinedFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        x = 2 * sizeX;
        y = i * sizeY;
        auto compositingTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        compositingTestNode->SetUICompositingFilter(combinedFilter);
        GetRootNode()->AddChild(compositingTestNode);
        RegisterNode(compositingTestNode);
    }
}

GRAPHIC_TEST(UIFilterTest, EFFECT_TEST, Set_UI_Filter_Combined_Fly_Out_Two_Test)
{
    int columnCount = 3;
    int rowCount = filterParaTypeCount;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = 0 * sizeX;
        int y = i * sizeY;
        vector<FilterPara::ParaType> testParaVec = {
            FilterPara::ParaType::FLY_OUT,
            static_cast<FilterPara::ParaType>(i + 1)
        };
        auto combinedFilter = UIFilterTestDataManager::GetCombinedFilter(testParaVec, TestDataGroupType::VALID_DATA2);
        auto foregroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        foregroundTestNode->SetUIForegroundFilter(combinedFilter);
        GetRootNode()->AddChild(foregroundTestNode);
        RegisterNode(foregroundTestNode);
        x = sizeX;
        y = i * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetUIBackgroundFilter(combinedFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        x = 2 * sizeX;
        y = i * sizeY;
        auto compositingTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        compositingTestNode->SetUICompositingFilter(combinedFilter);
        GetRootNode()->AddChild(compositingTestNode);
        RegisterNode(compositingTestNode);
    }
}

GRAPHIC_TEST(UIFilterTest, EFFECT_TEST, Set_UI_Filter_Combined_Distort_Two_Test)
{
    int columnCount = 3;
    int rowCount = filterParaTypeCount;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = 0 * sizeX;
        int y = i * sizeY;
        vector<FilterPara::ParaType> testParaVec = {
            FilterPara::ParaType::DISTORT,
            static_cast<FilterPara::ParaType>(i + 1)
        };
        auto combinedFilter = UIFilterTestDataManager::GetCombinedFilter(testParaVec, TestDataGroupType::VALID_DATA2);
        auto foregroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        foregroundTestNode->SetUIForegroundFilter(combinedFilter);
        GetRootNode()->AddChild(foregroundTestNode);
        RegisterNode(foregroundTestNode);
        x = sizeX;
        y = i * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetUIBackgroundFilter(combinedFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        x = 2 * sizeX;
        y = i * sizeY;
        auto compositingTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        compositingTestNode->SetUICompositingFilter(combinedFilter);
        GetRootNode()->AddChild(compositingTestNode);
        RegisterNode(compositingTestNode);
    }
}

GRAPHIC_TEST(UIFilterTest, EFFECT_TEST, Set_UI_Filter_Combined_Radius_Gradient_Blur_Two_Test)
{
    int columnCount = 3;
    int rowCount = filterParaTypeCount;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = 0 * sizeX;
        int y = i * sizeY;
        vector<FilterPara::ParaType> testParaVec = {
            FilterPara::ParaType::RADIUS_GRADIENT_BLUR,
            static_cast<FilterPara::ParaType>(i + 1)
        };
        auto combinedFilter = UIFilterTestDataManager::GetCombinedFilter(testParaVec, TestDataGroupType::VALID_DATA2);
        auto foregroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        foregroundTestNode->SetUIForegroundFilter(combinedFilter);
        GetRootNode()->AddChild(foregroundTestNode);
        RegisterNode(foregroundTestNode);
        x = sizeX;
        y = i * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetUIBackgroundFilter(combinedFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        x = 2 * sizeX;
        y = i * sizeY;
        auto compositingTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        compositingTestNode->SetUICompositingFilter(combinedFilter);
        GetRootNode()->AddChild(compositingTestNode);
        RegisterNode(compositingTestNode);
    }
}

GRAPHIC_TEST(UIFilterTest, EFFECT_TEST, Set_UI_Filter_Combined_Displacement_Distort_Two_Test)
{
    int columnCount = 3;
    int rowCount = filterParaTypeCount;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = 0 * sizeX;
        int y = i * sizeY;
        vector<FilterPara::ParaType> testParaVec = {
            FilterPara::ParaType::DISPLACEMENT_DISTORT,
            static_cast<FilterPara::ParaType>(i + 1)
        };
        auto combinedFilter = UIFilterTestDataManager::GetCombinedFilter(testParaVec, TestDataGroupType::VALID_DATA2);
        auto foregroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        foregroundTestNode->SetUIForegroundFilter(combinedFilter);
        GetRootNode()->AddChild(foregroundTestNode);
        RegisterNode(foregroundTestNode);
        x = sizeX;
        y = i * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetUIBackgroundFilter(combinedFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        x = 2 * sizeX;
        y = i * sizeY;
        auto compositingTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        compositingTestNode->SetUICompositingFilter(combinedFilter);
        GetRootNode()->AddChild(compositingTestNode);
        RegisterNode(compositingTestNode);
    }
}

GRAPHIC_TEST(UIFilterTest, EFFECT_TEST, Set_UI_Filter_Combined_Color_Gradient_Two_Test)
{
    int columnCount = 3;
    int rowCount = filterParaTypeCount;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = 0 * sizeX;
        int y = i * sizeY;
        vector<FilterPara::ParaType> testParaVec = {
            FilterPara::ParaType::COLOR_GRADIENT,
            static_cast<FilterPara::ParaType>(i + 1)
        };
        auto combinedFilter = UIFilterTestDataManager::GetCombinedFilter(testParaVec, TestDataGroupType::VALID_DATA2);
        auto foregroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        foregroundTestNode->SetUIForegroundFilter(combinedFilter);
        GetRootNode()->AddChild(foregroundTestNode);
        RegisterNode(foregroundTestNode);
        x = sizeX;
        y = i * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetUIBackgroundFilter(combinedFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        x = 2 * sizeX;
        y = i * sizeY;
        auto compositingTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        compositingTestNode->SetUICompositingFilter(combinedFilter);
        GetRootNode()->AddChild(compositingTestNode);
        RegisterNode(compositingTestNode);
    }
}

GRAPHIC_TEST(UIFilterTest, EFFECT_TEST, Set_UI_Filter_Combined_Sound_Wave_Two_Test)
{
    int columnCount = 3;
    int rowCount = filterParaTypeCount;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = 0 * sizeX;
        int y = i * sizeY;
        vector<FilterPara::ParaType> testParaVec = {
            FilterPara::ParaType::SOUND_WAVE,
            static_cast<FilterPara::ParaType>(i + 1)
        };
        auto combinedFilter = UIFilterTestDataManager::GetCombinedFilter(testParaVec, TestDataGroupType::VALID_DATA2);
        auto foregroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        foregroundTestNode->SetUIForegroundFilter(combinedFilter);
        GetRootNode()->AddChild(foregroundTestNode);
        RegisterNode(foregroundTestNode);
        x = sizeX;
        y = i * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetUIBackgroundFilter(combinedFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        x = 2 * sizeX;
        y = i * sizeY;
        auto compositingTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        compositingTestNode->SetUICompositingFilter(combinedFilter);
        GetRootNode()->AddChild(compositingTestNode);
        RegisterNode(compositingTestNode);
    }
}

GRAPHIC_TEST(UIFilterTest, EFFECT_TEST, Set_UI_Filter_Combined_Edge_Light_Two_Test)
{
    int columnCount = 3;
    int rowCount = filterParaTypeCount;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = 0 * sizeX;
        int y = i * sizeY;
        vector<FilterPara::ParaType> testParaVec = {
            FilterPara::ParaType::EDGE_LIGHT,
            static_cast<FilterPara::ParaType>(i + 1)
        };
        auto combinedFilter = UIFilterTestDataManager::GetCombinedFilter(testParaVec, TestDataGroupType::VALID_DATA2);
        auto foregroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        foregroundTestNode->SetUIForegroundFilter(combinedFilter);
        GetRootNode()->AddChild(foregroundTestNode);
        RegisterNode(foregroundTestNode);
        x = sizeX;
        y = i * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetUIBackgroundFilter(combinedFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        x = 2 * sizeX;
        y = i * sizeY;
        auto compositingTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        compositingTestNode->SetUICompositingFilter(combinedFilter);
        GetRootNode()->AddChild(compositingTestNode);
        RegisterNode(compositingTestNode);
    }
}

GRAPHIC_TEST(UIFilterTest, EFFECT_TEST, Set_UI_Filter_Combined_Bezier_Warp_Two_Test)
{
    int columnCount = 3;
    int rowCount = filterParaTypeCount;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = 0 * sizeX;
        int y = i * sizeY;
        vector<FilterPara::ParaType> testParaVec = {
            FilterPara::ParaType::BEZIER_WARP,
            static_cast<FilterPara::ParaType>(i + 1)
        };
        auto combinedFilter = UIFilterTestDataManager::GetCombinedFilter(testParaVec, TestDataGroupType::VALID_DATA2);
        auto foregroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        foregroundTestNode->SetUIForegroundFilter(combinedFilter);
        GetRootNode()->AddChild(foregroundTestNode);
        RegisterNode(foregroundTestNode);
        x = sizeX;
        y = i * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetUIBackgroundFilter(combinedFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        x = 2 * sizeX;
        y = i * sizeY;
        auto compositingTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        compositingTestNode->SetUICompositingFilter(combinedFilter);
        GetRootNode()->AddChild(compositingTestNode);
        RegisterNode(compositingTestNode);
    }
}

GRAPHIC_TEST(UIFilterTest, EFFECT_TEST, Set_UI_Filter_Combined_Dispersion_Two_Test)
{
    int columnCount = 3;
    int rowCount = filterParaTypeCount;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = 0 * sizeX;
        int y = i * sizeY;
        vector<FilterPara::ParaType> testParaVec = {
            FilterPara::ParaType::DISPERSION,
            static_cast<FilterPara::ParaType>(i + 1)
        };
        auto combinedFilter = UIFilterTestDataManager::GetCombinedFilter(testParaVec, TestDataGroupType::VALID_DATA2);
        auto foregroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        foregroundTestNode->SetUIForegroundFilter(combinedFilter);
        GetRootNode()->AddChild(foregroundTestNode);
        RegisterNode(foregroundTestNode);
        x = sizeX;
        y = i * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetUIBackgroundFilter(combinedFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        x = 2 * sizeX;
        y = i * sizeY;
        auto compositingTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        compositingTestNode->SetUICompositingFilter(combinedFilter);
        GetRootNode()->AddChild(compositingTestNode);
        RegisterNode(compositingTestNode);
    }
}

GRAPHIC_TEST(UIFilterTest, EFFECT_TEST, Set_UI_Filter_Combined_Hdr_Two_Test)
{
    int columnCount = 3;
    int rowCount = filterParaTypeCount;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = 0 * sizeX;
        int y = i * sizeY;
        vector<FilterPara::ParaType> testParaVec = {
            FilterPara::ParaType::HDR_BRIGHTNESS_RATIO,
            static_cast<FilterPara::ParaType>(i + 1)
        };
        auto combinedFilter = UIFilterTestDataManager::GetCombinedFilter(testParaVec, TestDataGroupType::VALID_DATA2);
        auto foregroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        foregroundTestNode->SetUIForegroundFilter(combinedFilter);
        GetRootNode()->AddChild(foregroundTestNode);
        RegisterNode(foregroundTestNode);
        x = sizeX;
        y = i * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetUIBackgroundFilter(combinedFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        x = 2 * sizeX;
        y = i * sizeY;
        auto compositingTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        compositingTestNode->SetUICompositingFilter(combinedFilter);
        GetRootNode()->AddChild(compositingTestNode);
        RegisterNode(compositingTestNode);
    }
}

GRAPHIC_TEST(UIFilterTest, EFFECT_TEST, Set_UI_Filter_Combined_Content_Light_Two_Test)
{
    int columnCount = 3;
    int rowCount = filterParaTypeCount;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = 0 * sizeX;
        int y = i * sizeY;
        vector<FilterPara::ParaType> testParaVec = {
            FilterPara::ParaType::CONTENT_LIGHT,
            static_cast<FilterPara::ParaType>(i + 1)
        };
        auto combinedFilter = UIFilterTestDataManager::GetCombinedFilter(testParaVec, TestDataGroupType::VALID_DATA2);
        auto foregroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        foregroundTestNode->SetUIForegroundFilter(combinedFilter);
        GetRootNode()->AddChild(foregroundTestNode);
        RegisterNode(foregroundTestNode);
        x = sizeX;
        y = i * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetUIBackgroundFilter(combinedFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        x = 2 * sizeX;
        y = i * sizeY;
        auto compositingTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        compositingTestNode->SetUICompositingFilter(combinedFilter);
        GetRootNode()->AddChild(compositingTestNode);
        RegisterNode(compositingTestNode);
    }
}

} // namespace OHOS::Rosen
