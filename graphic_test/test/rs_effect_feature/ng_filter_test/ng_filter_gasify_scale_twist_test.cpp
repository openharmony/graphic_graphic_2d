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

#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"
#include "ng_filter_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;
}

class NGFilterGasifyScaleTwistTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

    std::shared_ptr<Media::PixelMap> source =
        DecodePixelMap(BG_PATH, Media::AllocatorType::SHARE_MEM_ALLOC);

    std::shared_ptr<Media::PixelMap> maskRGB =
        DecodePixelMap(MASK_RGB_PATH, Media::AllocatorType::SHARE_MEM_ALLOC);
};

GRAPHIC_TEST(NGFilterGasifyScaleTwistTest, EFFECT_TEST, Set_NG_Filter_Gasify_Scale_Twist_Test)
{
    auto gasifyTwistPtr = CreateFilter(RSNGEffectType::GASIFY_SCALE_TWIST);
    auto gasifyTwistFilter = std::static_pointer_cast<RSNGGasifyScaleTwistFilter>(gasifyTwistPtr);
    gasifyTwistFilter->Setter<GasifyScaleTwistProgressTag>(0.2f);
    gasifyTwistFilter->Setter<GasifyScaleTwistSourceImageTag>(source);
    gasifyTwistFilter->Setter<GasifyScaleTwistScaleTag>(Vector2f{200, 200});
    gasifyTwistFilter->Setter<GasifyScaleTwistMaskTag>(maskRGB);

    int nodeWith = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWith) * i, nodeWith * (i + 1), nodeHeight * (i + 1)});
        backgroundNode->SetForegroundNGFilter(gasifyTwistFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterGasifyScaleTwistTest, EFFECT_TEST, Set_NG_Filter_Gasify_Scale_Twist_Progress_Test)
{
    auto gasifyTwistPtr = CreateFilter(RSNGEffectType::GASIFY_SCALE_TWIST);
    auto gasifyTwistFilter = std::static_pointer_cast<RSNGGasifyScaleTwistFilter>(gasifyTwistPtr);
    gasifyTwistFilter->Setter<GasifyScaleTwistProgressTag>(0.2f);
    gasifyTwistFilter->Setter<GasifyScaleTwistSourceImageTag>(source);
    gasifyTwistFilter->Setter<GasifyScaleTwistScaleTag>(Vector2f{200, 200});
    gasifyTwistFilter->Setter<GasifyScaleTwistMaskTag>(maskRGB);

    int nodeWith = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWith) * i, nodeWith * (i + 1), nodeHeight * (i + 1)});
        gasifyTwistFilter->Setter<GasifyScaleTwistProgressTag>(gasifyProgressVec[i]);
        backgroundNode->SetForegroundNGFilter(gasifyTwistFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterGasifyScaleTwistTest, EFFECT_TEST, Set_NG_Filter_Gasify_Scale_Twist_Source_Test)
{
    auto gasifyTwistPtr = CreateFilter(RSNGEffectType::GASIFY_SCALE_TWIST);
    auto gasifyTwistFilter = std::static_pointer_cast<RSNGGasifyScaleTwistFilter>(gasifyTwistPtr);
    gasifyTwistFilter->Setter<GasifyScaleTwistProgressTag>(0.2f);
    gasifyTwistFilter->Setter<GasifyScaleTwistSourceImageTag>(source);
    gasifyTwistFilter->Setter<GasifyScaleTwistScaleTag>(Vector2f{200, 200});
    gasifyTwistFilter->Setter<GasifyScaleTwistMaskTag>(maskRGB);

    int nodeWith = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWith) * i, nodeWith * (i + 1), nodeHeight * (i + 1)});
        if (i % 2) {
            source = DecodePixelMap(FG_TEST_JPG_PATH, Media::AllocatorType::SHARE_MEM_ALLOC);
            gasifyTwistFilter->Setter<GasifyScaleTwistSourceImageTag>(source);
        }
        backgroundNode->SetForegroundNGFilter(gasifyTwistFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterGasifyScaleTwistTest, EFFECT_TEST, Set_NG_Filter_Gasify_Scale_Twist_Scale_Test)
{
    auto gasifyTwistPtr = CreateFilter(RSNGEffectType::GASIFY_SCALE_TWIST);
    auto gasifyTwistFilter = std::static_pointer_cast<RSNGGasifyScaleTwistFilter>(gasifyTwistPtr);
    gasifyTwistFilter->Setter<GasifyScaleTwistProgressTag>(0.2f);
    gasifyTwistFilter->Setter<GasifyScaleTwistSourceImageTag>(source);
    gasifyTwistFilter->Setter<GasifyScaleTwistScaleTag>(Vector2f{200, 200});
    gasifyTwistFilter->Setter<GasifyScaleTwistMaskTag>(maskRGB);

    int nodeWith = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWith) * i, nodeWith * (i + 1), nodeHeight * (i + 1)});
        gasifyTwistFilter->Setter<GasifyScaleTwistScaleTag>(Vector2f{nodeWith * (i + 1), nodeHeight * (i + 1)});
        backgroundNode->SetForegroundNGFilter(gasifyTwistFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterGasifyScaleTwistTest, EFFECT_TEST, Set_NG_Filter_Gasify_Scale_Twist_Mask_Test)
{
    auto gasifyTwistPtr = CreateFilter(RSNGEffectType::GASIFY_SCALE_TWIST);
    auto gasifyTwistFilter = std::static_pointer_cast<RSNGGasifyScaleTwistFilter>(gasifyTwistPtr);
    gasifyTwistFilter->Setter<GasifyScaleTwistProgressTag>(0.2f);
    gasifyTwistFilter->Setter<GasifyScaleTwistSourceImageTag>(source);
    gasifyTwistFilter->Setter<GasifyScaleTwistScaleTag>(Vector2f{200, 200});
    gasifyTwistFilter->Setter<GasifyScaleTwistMaskTag>(maskRGB);

    int nodeWith = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWith) * i, nodeWith * (i + 1), nodeHeight * (i + 1)});
        if (i % 2) {
            maskRGB = DecodePixelMap(FG_TEST_JPG_PATH, Media::AllocatorType::SHARE_MEM_ALLOC);
            gasifyTwistFilter->Setter<GasifyScaleTwistMaskTag>(maskRGB);
        }
        backgroundNode->SetForegroundNGFilter(gasifyTwistFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

}