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
}  // namespace

class NGFilterSoundWaveTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }
};

// Test SoundWave with all uncovered properties
GRAPHIC_TEST(NGFilterSoundWaveTest, EFFECT_TEST, Set_NG_Filter_Sound_Wave_ColorA_Test)
{
    auto soundWavePtr = CreateFilter(RSNGEffectType::SOUND_WAVE);
    auto soundWaveFilter = std::static_pointer_cast<RSNGSoundWaveFilter>(soundWavePtr);

    // Set ColorA tag
    soundWaveFilter->Setter<SoundWaveColorATag>(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    const std::vector<Vector4f> colorValues = {
        Vector4f(1.0f, 0.0f, 0.0f, 1.0f),
        Vector4f(0.0f, 1.0f, 0.0f, 1.0f),
        Vector4f(0.0f, 0.0f, 1.0f, 1.0f),
        Vector4f(1.0f, 1.0f, 0.0f, 0.8f)
    };

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWidth) * i, nodeWidth * (i + 1), nodeHeight * (i + 1)});
        soundWaveFilter->Setter<SoundWaveColorATag>(colorValues[i]);
        backgroundNode->SetForegroundNGFilter(soundWaveFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterSoundWaveTest, EFFECT_TEST, Set_NG_Filter_Sound_Wave_ColorB_Test)
{
    auto soundWavePtr = CreateFilter(RSNGEffectType::SOUND_WAVE);
    auto soundWaveFilter = std::static_pointer_cast<RSNGSoundWaveFilter>(soundWavePtr);

    // Set ColorB tag
    soundWaveFilter->Setter<SoundWaveColorBTag>(Vector4f(0.5f, 0.3f, 0.8f, 0.9f));

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    const std::vector<Vector4f> colorValues = {
        Vector4f(0.8f, 0.2f, 0.6f, 1.0f),
        Vector4f(0.3f, 0.7f, 0.4f, 0.9f),
        Vector4f(0.9f, 0.5f, 0.1f, 0.8f),
        Vector4f(0.6f, 0.6f, 0.6f, 1.0f)
    };

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWidth) * i, nodeWidth * (i + 1), nodeHeight * (i + 1)});
        soundWaveFilter->Setter<SoundWaveColorBTag>(colorValues[i]);
        backgroundNode->SetForegroundNGFilter(soundWaveFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterSoundWaveTest, EFFECT_TEST, Set_NG_Filter_Sound_Wave_ColorC_Test)
{
    auto soundWavePtr = CreateFilter(RSNGEffectType::SOUND_WAVE);
    auto soundWaveFilter = std::static_pointer_cast<RSNGSoundWaveFilter>(soundWavePtr);

    // Set ColorC tag
    soundWaveFilter->Setter<SoundWaveColorCTag>(Vector4f(0.7f, 0.7f, 0.7f, 0.6f));

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    const std::vector<Vector4f> colorValues = {
        Vector4f(0.4f, 0.4f, 0.4f, 0.5f),
        Vector4f(0.8f, 0.8f, 0.2f, 0.7f),
        Vector4f(0.2f, 0.8f, 0.8f, 0.9f),
        Vector4f(1.0f, 0.9f, 0.7f, 1.0f)
    };

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWidth) * i, nodeWidth * (i + 1), nodeHeight * (i + 1)});
        soundWaveFilter->Setter<SoundWaveColorCTag>(colorValues[i]);
        backgroundNode->SetForegroundNGFilter(soundWaveFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterSoundWaveTest, EFFECT_TEST, Set_NG_Filter_Sound_Wave_ColorProgress_Test)
{
    auto soundWavePtr = CreateFilter(RSNGEffectType::SOUND_WAVE);
    auto soundWaveFilter = std::static_pointer_cast<RSNGSoundWaveFilter>(soundWavePtr);

    // Set ColorProgress tag
    soundWaveFilter->Setter<SoundWaveColorProgressTag>(0.5f);

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    const std::vector<float> progressValues = {0.0f, 0.25f, 0.5f, 1.0f};

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWidth) * i, nodeWidth * (i + 1), nodeHeight * (i + 1)});
        soundWaveFilter->Setter<SoundWaveColorProgressTag>(progressValues[i]);
        backgroundNode->SetForegroundNGFilter(soundWaveFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterSoundWaveTest, EFFECT_TEST, Set_NG_Filter_Sound_Wave_Intensity_Test)
{
    auto soundWavePtr = CreateFilter(RSNGEffectType::SOUND_WAVE);
    auto soundWaveFilter = std::static_pointer_cast<RSNGSoundWaveFilter>(soundWavePtr);

    // Set Intensity tag
    soundWaveFilter->Setter<SoundWaveIntensityTag>(0.8f);

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    const std::vector<float> intensityValues = {0.1f, 0.4f, 0.7f, 1.0f};

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWidth) * i, nodeWidth * (i + 1), nodeHeight * (i + 1)});
        soundWaveFilter->Setter<SoundWaveIntensityTag>(intensityValues[i]);
        backgroundNode->SetForegroundNGFilter(soundWaveFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterSoundWaveTest, EFFECT_TEST, Set_NG_Filter_Sound_Wave_AlphaA_Test)
{
    auto soundWavePtr = CreateFilter(RSNGEffectType::SOUND_WAVE);
    auto soundWaveFilter = std::static_pointer_cast<RSNGSoundWaveFilter>(soundWavePtr);

    // Set AlphaA tag
    soundWaveFilter->Setter<SoundWaveAlphaATag>(0.7f);

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    const std::vector<float> alphaValues = {0.0f, 0.3f, 0.6f, 1.0f};

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWidth) * i, nodeWidth * (i + 1), nodeHeight * (i + 1)});
        soundWaveFilter->Setter<SoundWaveAlphaATag>(alphaValues[i]);
        backgroundNode->SetForegroundNGFilter(soundWaveFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterSoundWaveTest, EFFECT_TEST, Set_NG_Filter_Sound_Wave_AlphaB_Test)
{
    auto soundWavePtr = CreateFilter(RSNGEffectType::SOUND_WAVE);
    auto soundWaveFilter = std::static_pointer_cast<RSNGSoundWaveFilter>(soundWavePtr);

    // Set AlphaB tag
    soundWaveFilter->Setter<SoundWaveAlphaBTag>(0.5f);

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    const std::vector<float> alphaValues = {0.1f, 0.4f, 0.7f, 0.9f};

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWidth) * i, nodeWidth * (i + 1), nodeHeight * (i + 1)});
        soundWaveFilter->Setter<SoundWaveAlphaBTag>(alphaValues[i]);
        backgroundNode->SetForegroundNGFilter(soundWaveFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterSoundWaveTest, EFFECT_TEST, Set_NG_Filter_Sound_Wave_ProgressA_Test)
{
    auto soundWavePtr = CreateFilter(RSNGEffectType::SOUND_WAVE);
    auto soundWaveFilter = std::static_pointer_cast<RSNGSoundWaveFilter>(soundWavePtr);

    // Set ProgressA tag
    soundWaveFilter->Setter<SoundWaveProgressATag>(0.3f);

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    const std::vector<float> progressValues = {0.0f, 0.2f, 0.5f, 0.8f};

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWidth) * i, nodeWidth * (i + 1), nodeHeight * (i + 1)});
        soundWaveFilter->Setter<SoundWaveProgressATag>(progressValues[i]);
        backgroundNode->SetForegroundNGFilter(soundWaveFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterSoundWaveTest, EFFECT_TEST, Set_NG_Filter_Sound_Wave_ProgressB_Test)
{
    auto soundWavePtr = CreateFilter(RSNGEffectType::SOUND_WAVE);
    auto soundWaveFilter = std::static_pointer_cast<RSNGSoundWaveFilter>(soundWavePtr);

    // Set ProgressB tag
    soundWaveFilter->Setter<SoundWaveProgressBTag>(0.6f);

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    const std::vector<float> progressValues = {0.1f, 0.3f, 0.6f, 0.9f};

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWidth) * i, nodeWidth * (i + 1), nodeHeight * (i + 1)});
        soundWaveFilter->Setter<SoundWaveProgressBTag>(progressValues[i]);
        backgroundNode->SetForegroundNGFilter(soundWaveFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterSoundWaveTest, EFFECT_TEST, Set_NG_Filter_Sound_Wave_TotalAlpha_Test)
{
    auto soundWavePtr = CreateFilter(RSNGEffectType::SOUND_WAVE);
    auto soundWaveFilter = std::static_pointer_cast<RSNGSoundWaveFilter>(soundWavePtr);

    // Set TotalAlpha tag
    soundWaveFilter->Setter<SoundWaveTotalAlphaTag>(0.85f);

    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    const std::vector<float> alphaValues = {0.2f, 0.5f, 0.75f, 1.0f};

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWidth) * i, nodeWidth * (i + 1), nodeHeight * (i + 1)});
        soundWaveFilter->Setter<SoundWaveTotalAlphaTag>(alphaValues[i]);
        backgroundNode->SetForegroundNGFilter(soundWaveFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

GRAPHIC_TEST(NGFilterSoundWaveTest, EFFECT_TEST, Set_NG_Filter_Sound_Wave_Complete_Test)
{
    auto soundWavePtr = CreateFilter(RSNGEffectType::SOUND_WAVE);
    auto soundWaveFilter = std::static_pointer_cast<RSNGSoundWaveFilter>(soundWavePtr);

    // Set all tags together
    soundWaveFilter->Setter<SoundWaveColorATag>(Vector4f(1.0f, 0.5f, 0.0f, 0.9f));
    soundWaveFilter->Setter<SoundWaveColorBTag>(Vector4f(0.0f, 0.8f, 0.4f, 0.8f));
    soundWaveFilter->Setter<SoundWaveColorCTag>(Vector4f(0.3f, 0.3f, 0.9f, 0.7f));
    soundWaveFilter->Setter<SoundWaveColorProgressTag>(0.5f);
    soundWaveFilter->Setter<SoundWaveIntensityTag>(0.7f);
    soundWaveFilter->Setter<SoundWaveAlphaATag>(0.6f);
    soundWaveFilter->Setter<SoundWaveAlphaBTag>(0.4f);
    soundWaveFilter->Setter<SoundWaveProgressATag>(0.3f);
    soundWaveFilter->Setter<SoundWaveProgressBTag>(0.6f);
    soundWaveFilter->Setter<SoundWaveTotalAlphaTag>(0.85f);

    int nodeWidth = 980;
    int nodeHeight = 1600;
    int start = 110;

    auto backgroundNode = SetUpNodeBgImage(BG_PATH,
        {start, start, nodeWidth, nodeHeight});
    backgroundNode->SetForegroundNGFilter(soundWaveFilter);
    GetRootNode()->AddChild(backgroundNode);
    RegisterNode(backgroundNode);
}

} // namespace OHOS::Rosen
