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
#include "animation/rs_particle_ripple_field.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class ParticleRippleTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

/*
 * @tc.name: ParticleRippleTest_001
 * @tc.desc: Test SetParticleRippleFields with normal values
 * @tc.type: FUNC
 * @tc.require: issueI7N7M2
 */
GRAPHIC_TEST(ParticleRippleTest, CONTENT_DISPLAY_TEST, ParticleRippleTest_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    // Set particle ripple fields
    auto fields = std::make_shared<ParticleRippleFields>();
    auto field = std::make_shared<ParticleRippleField>(Vector2f(200.0f, 200.0f), 100.0f, 50.0f, 200.0f);
    fields->AddRippleField(field);
    testNode->SetParticleRippleFields(fields);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: ParticleRippleTest_002
 * @tc.desc: Test SetParticleRippleFields with boundary values
 * @tc.type: FUNC
 * @tc.require: issueI7N7M2
 */
GRAPHIC_TEST(ParticleRippleTest, CONTENT_DISPLAY_TEST, ParticleRippleTest_002)
{
    std::vector<std::tuple<float, float, float>> paramsList = {
        { 0.0f, 0.0f, 0.0f },         // minimum values
        { 0.001f, 0.001f, 0.001f },   // near minimum
        { 1.0f, 2.0f, 3.0f },         // normal values
        { 10.0f, 20.0f, 30.0f },      // high values
        { 100.0f, 200.0f, 300.0f },   // very high values
        { -1.0f, -2.0f, -3.0f }       // negative values (edge case)
    };

    for (size_t i = 0; i < paramsList.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 190 + 50, 50, 180, 180 });
        testNode->SetBackgroundColor(0xffff0000);

        auto [param1, param2, param3] = paramsList[i];
        auto fields = std::make_shared<ParticleRippleFields>();
        auto field = std::make_shared<ParticleRippleField>(
            Vector2f(90.0f, 90.0f), param1, param2, param3);
        fields->AddRippleField(field);
        testNode->SetParticleRippleFields(fields);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: ParticleRippleTest_003
 * @tc.desc: Test SetParticleRippleFields with alpha (3x4 matrix)
 * @tc.type: FUNC
 * @tc.require: issueI7N7M2
 */
GRAPHIC_TEST(ParticleRippleTest, CONTENT_DISPLAY_TEST, ParticleRippleTest_003)
{
    std::vector<float> amplitudeValues = { 50.0f, 100.0f, 150.0f };
    std::vector<float> alphaList = { 0.3f, 0.6f, 1.0f };

    for (size_t row = 0; row < amplitudeValues.size(); row++) {
        for (size_t col = 0; col < alphaList.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 350 + 50, 300, 300 });
            testNode->SetBackgroundColor(0xffff0000);
            testNode->SetAlpha(alphaList[col]);

            auto fields = std::make_shared<ParticleRippleFields>();
            auto field = std::make_shared<ParticleRippleField>(
                Vector2f(150.0f, 150.0f), amplitudeValues[row], 50.0f, 200.0f);
            fields->AddRippleField(field);
            testNode->SetParticleRippleFields(fields);

            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/*
 * @tc.name: ParticleRippleTest_004
 * @tc.desc: Test SetParticleRippleFields update scenario
 * @tc.type: FUNC
 * @tc.require: issueI7N7M2
 */
GRAPHIC_TEST(ParticleRippleTest, CONTENT_DISPLAY_TEST, ParticleRippleTest_004)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    // Set initial values
    auto fields = std::make_shared<ParticleRippleFields>();
    auto field1 = std::make_shared<ParticleRippleField>(Vector2f(200.0f, 200.0f), 100.0f, 50.0f, 200.0f);
    fields->AddRippleField(field1);
    testNode->SetParticleRippleFields(fields);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Update to different values
    auto fields2 = std::make_shared<ParticleRippleFields>();
    auto field2 = std::make_shared<ParticleRippleField>(Vector2f(200.0f, 200.0f), 200.0f, 100.0f, 400.0f);
    fields2->AddRippleField(field2);
    testNode->SetParticleRippleFields(fields2);

    // Update again
    auto fields3 = std::make_shared<ParticleRippleFields>();
    auto field3 = std::make_shared<ParticleRippleField>(Vector2f(200.0f, 200.0f), 300.0f, 150.0f, 600.0f);
    fields3->AddRippleField(field3);
    testNode->SetParticleRippleFields(fields3);
}

/*
 * @tc.name: ParticleRippleTest_005
 * @tc.desc: Test SetParticleRippleFields with rotation
 * @tc.type: FUNC
 * @tc.require: issueI7N7M2
 */
GRAPHIC_TEST(ParticleRippleTest, CONTENT_DISPLAY_TEST, ParticleRippleTest_005)
{
    std::vector<float> rotationList = { 0, 45, 90 };

    for (size_t i = 0; i < rotationList.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 380 + 50, 50, 300, 300 });
        testNode->SetBackgroundColor(0xffff0000);
        testNode->SetRotation(rotationList[i]);

        auto fields = std::make_shared<ParticleRippleFields>();
        auto field = std::make_shared<ParticleRippleField>(Vector2f(150.0f, 150.0f), 100.0f, 50.0f, 200.0f);
        fields->AddRippleField(field);
        testNode->SetParticleRippleFields(fields);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: ParticleRippleTest_006
 * @tc.desc: Test SetParticleRippleFields with scale (3x3 matrix)
 * @tc.type: FUNC
 * @tc.require: issueI7N7M2
 */
GRAPHIC_TEST(ParticleRippleTest, CONTENT_DISPLAY_TEST, ParticleRippleTest_006)
{
    std::vector<float> wavelengthValues = { 50.0f, 100.0f, 150.0f };
    std::vector<std::pair<float, float>> scaleList = {
        { 0.5f, 0.5f },
        { 1.0f, 1.0f },
        { 1.5f, 1.5f }
    };

    for (size_t row = 0; row < wavelengthValues.size(); row++) {
        for (size_t col = 0; col < scaleList.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 380 + 50, 300, 300 });
            testNode->SetBackgroundColor(0xffff0000);
            testNode->SetScale(scaleList[col].first, scaleList[col].second);

            auto fields = std::make_shared<ParticleRippleFields>();
            auto field = std::make_shared<ParticleRippleField>(
                Vector2f(150.0f, 150.0f), 100.0f, wavelengthValues[row], 200.0f);
            fields->AddRippleField(field);
            testNode->SetParticleRippleFields(fields);

            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/*
 * @tc.name: ParticleRippleTest_007
 * @tc.desc: Test SetParticleRippleFields with zero bounds
 * @tc.type: FUNC
 * @tc.require: issueI7N7M2
 */
GRAPHIC_TEST(ParticleRippleTest, CONTENT_DISPLAY_TEST, ParticleRippleTest_007)
{
    std::vector<Vector4f> zeroBounds = {
        { 50, 50, 0, 100 },
        { 150, 50, 100, 0 },
        { 300, 50, 0, 0 }
    };

    for (size_t i = 0; i < zeroBounds.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ zeroBounds[i].x_, zeroBounds[i].y_,
            zeroBounds[i].z_, zeroBounds[i].w_ });
        testNode->SetBackgroundColor(0xffff0000);

        auto fields = std::make_shared<ParticleRippleFields>();
        float amplitude = 100.0f + i * 50.0f;
        float wavelength = 50.0f + i * 25.0f;
        float waveSpeed = 200.0f + i * 100.0f;
        auto field = std::make_shared<ParticleRippleField>(
            Vector2f(50.0f, 50.0f), amplitude, wavelength, waveSpeed);
        fields->AddRippleField(field);
        testNode->SetParticleRippleFields(fields);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: ParticleRippleTest_008
 * @tc.desc: Test SetParticleRippleFields with different wave speeds (3x4 matrix)
 * @tc.type: FUNC
 * @tc.require: issueI7N7M2
 */
GRAPHIC_TEST(ParticleRippleTest, CONTENT_DISPLAY_TEST, ParticleRippleTest_008)
{
    std::vector<float> waveSpeedValues = { 100.0f, 200.0f, 300.0f };
    std::vector<float> alphaList = { 0.3f, 0.6f, 1.0f };

    for (size_t row = 0; row < waveSpeedValues.size(); row++) {
        for (size_t col = 0; col < alphaList.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 350 + 50, 300, 300 });
            testNode->SetBackgroundColor(0xffff0000);
            testNode->SetAlpha(alphaList[col]);

            auto fields = std::make_shared<ParticleRippleFields>();
            auto field = std::make_shared<ParticleRippleField>(
                Vector2f(150.0f, 150.0f), 100.0f, 50.0f, waveSpeedValues[row]);
            fields->AddRippleField(field);
            testNode->SetParticleRippleFields(fields);

            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/*
 * @tc.name: ParticleRippleTest_009
 * @tc.desc: Test SetParticleRippleFields with pixelmap
 * @tc.type: FUNC
 * @tc.require: issueI7N7M2
 */
GRAPHIC_TEST(ParticleRippleTest, CONTENT_DISPLAY_TEST, ParticleRippleTest_009)
{
    auto pixelMap = DecodePixelMap("/data/local/tmp/appearance_test.jpg",
                                   Media::AllocatorType::SHARE_MEM_ALLOC);

    std::vector<std::tuple<float, float, float>> paramsList = {
        { 50.0f, 50.0f, 150.0f },
        { 100.0f, 50.0f, 200.0f },
        { 150.0f, 50.0f, 300.0f },
        { 200.0f, 50.0f, 400.0f }
    };

    for (size_t i = 0; i < paramsList.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 280 + 50, 50, 250, 250 });
        testNode->SetPixelmap(pixelMap);

        auto [param1, param2, param3] = paramsList[i];
        auto fields = std::make_shared<ParticleRippleFields>();
        auto field = std::make_shared<ParticleRippleField>(
            Vector2f(125.0f, 125.0f), param1, param2, param3);
        fields->AddRippleField(field);
        testNode->SetParticleRippleFields(fields);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: ParticleRippleTest_010
 * @tc.desc: Test SetParticleRippleFields extreme values
 * @tc.type: FUNC
 * @tc.require: issueI7N7M2
 */
GRAPHIC_TEST(ParticleRippleTest, CONTENT_DISPLAY_TEST, ParticleRippleTest_010)
{
    std::vector<std::tuple<float, float, float>> extremeParams = {
        { 0.0f, 0.0f, 0.0f },         // All zeros
        { 0.001f, 0.001f, 0.001f },   // Very small
        { 100.0f, 100.0f, 100.0f },   // Large
        { 1000.0f, 1000.0f, 1000.0f } // Very large
    };

    for (size_t i = 0; i < extremeParams.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 280 + 50, 50, 250, 250 });
        testNode->SetBackgroundColor(0xffff0000);

        auto [param1, param2, param3] = extremeParams[i];
        auto fields = std::make_shared<ParticleRippleFields>();
        auto field = std::make_shared<ParticleRippleField>(
            Vector2f(125.0f, 125.0f), param1, param2, param3);
        fields->AddRippleField(field);
        testNode->SetParticleRippleFields(fields);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

} // namespace OHOS::Rosen
