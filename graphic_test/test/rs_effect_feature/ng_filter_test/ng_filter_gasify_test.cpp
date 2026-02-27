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

class NGFilterGasifyTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

    std::shared_ptr<Media::PixelMap> source =
        DecodePixelMap(BG_PATH, Media::AllocatorType::SHARE_MEM_ALLOC);

    std::shared_ptr<Media::PixelMap> maskShadow =
        DecodePixelMap(MASK_SHADOW_PATH, Media::AllocatorType::SHARE_MEM_ALLOC);
};

GRAPHIC_TEST(NGFilterGasifyTest, EFFECT_TEST, Set_NG_Filter_Gasify_Test)
{
    auto gasifyPtr = CreateFilter(RSNGEffectType::GASIFY);
    auto gasifyFilter = std::static_pointer_cast<RSNGGasifyFilter>(gasifyPtr);
    gasifyFilter->Setter<GasifyProgressTag>(0.5f);
    gasifyFilter->Setter<GasifySourceImageTag>(source);
    gasifyFilter->Setter<GasifyMaskTag>(maskShadow);

    int nodeWith = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWith) * i, nodeWith * (i + 1), nodeHeight * (i + 1)});
        backgroundNode->SetForegroundNGFilter(gasifyFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterGasifyTest, EFFECT_TEST, Set_NG_Filter_Gasify_Progress_Test)
{
    auto gasifyPtr = CreateFilter(RSNGEffectType::GASIFY);
    auto gasifyFilter = std::static_pointer_cast<RSNGGasifyFilter>(gasifyPtr);
    gasifyFilter->Setter<GasifyProgressTag>(0.2f);
    gasifyFilter->Setter<GasifySourceImageTag>(source);
    gasifyFilter->Setter<GasifyMaskTag>(maskShadow);

    int nodeWith = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWith) * i, nodeWith * (i + 1), nodeHeight * (i + 1)});
        gasifyFilter->Setter<GasifyProgressTag>(gasifyProgressVec[i]);
        backgroundNode->SetForegroundNGFilter(gasifyFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterGasifyTest, EFFECT_TEST, Set_NG_Filter_Gasify_Source_Image_Test)
{
    auto gasifyPtr = CreateFilter(RSNGEffectType::GASIFY);
    auto gasifyFilter = std::static_pointer_cast<RSNGGasifyFilter>(gasifyPtr);
    gasifyFilter->Setter<GasifyProgressTag>(0.2f);
    gasifyFilter->Setter<GasifySourceImageTag>(source);
    gasifyFilter->Setter<GasifyMaskTag>(maskShadow);

    int nodeWith = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWith) * i, nodeWith * (i + 1), nodeHeight * (i + 1)});
        backgroundNode->SetForegroundNGFilter(gasifyFilter);
        if (i > 1) {
            source = DecodePixelMap(FG_TEST_JPG_PATH, Media::AllocatorType::SHARE_MEM_ALLOC);
            gasifyFilter->Setter<GasifySourceImageTag>(source);
        }
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterGasifyTest, EFFECT_TEST, Set_NG_Filter_Gasify_Mask_Test)
{
    auto gasifyPtr = CreateFilter(RSNGEffectType::GASIFY);
    auto gasifyFilter = std::static_pointer_cast<RSNGGasifyFilter>(gasifyPtr);
    gasifyFilter->Setter<GasifyProgressTag>(0.2f);
    gasifyFilter->Setter<GasifySourceImageTag>(source);
    gasifyFilter->Setter<GasifyMaskTag>(maskShadow);

    int nodeWith = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWith) * i, nodeWith * (i + 1), nodeHeight * (i + 1)});
        if (i > 1) {
            maskShadow = DecodePixelMap(FG_TEST_JPG_PATH, Media::AllocatorType::SHARE_MEM_ALLOC);
            gasifyFilter->Setter<GasifyMaskTag>(maskShadow);
        }
        backgroundNode->SetForegroundNGFilter(gasifyFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}
}