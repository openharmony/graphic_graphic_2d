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
#include "animation/rs_particle_velocity_field.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class ParticleVelocityTest : public RSGraphicTest {
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
 * @tc.name: ParticleVelocityTest_001
 * @tc.desc: Test SetParticleVelocityFields with normal values
 * @tc.type: FUNC
 * @tc.require: issueI7N7M3
 */
GRAPHIC_TEST(ParticleVelocityTest, CONTENT_DISPLAY_TEST, ParticleVelocityTest_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    // Set particle velocity fields
    auto fields = std::make_shared<ParticleVelocityFields>();
    auto field = std::make_shared<ParticleVelocityField>(Vector2f(1.0f, 2.0f));
    fields->AddVelocityField(field);
    testNode->SetParticleVelocityFields(fields);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: ParticleVelocityTest_002
 * @tc.desc: Test SetParticleVelocityFields with boundary values
 * @tc.type: FUNC
 * @tc.require: issueI7N7M3
 */
GRAPHIC_TEST(ParticleVelocityTest, CONTENT_DISPLAY_TEST, ParticleVelocityTest_002)
{
    std::vector<std::tuple<float, float, float>> paramsList = {
        { 0.0f, 0.0f, 0.0f },         // minimum values
        { 0.001f, 0.001f, 0.001f },   // near minimum
        { 5.0f, 10.0f, 15.0f },       // normal values
        { 50.0f, 100.0f, 150.0f },    // high values
        { -5.0f, -10.0f, -15.0f }     // negative values (edge case)
    };

    for (size_t i = 0; i < paramsList.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 190 + 50, 50, 180, 180 });
        testNode->SetBackgroundColor(0xffff0000);

        auto [param1, param2, param3] = paramsList[i];
        auto fields = std::make_shared<ParticleVelocityFields>();
        auto field = std::make_shared<ParticleVelocityField>(Vector2f(param1, param2));
        fields->AddVelocityField(field);
        testNode->SetParticleVelocityFields(fields);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: ParticleVelocityTest_003
 * @tc.desc: Test SetParticleVelocityFields with alpha (3x4 matrix)
 * @tc.type: FUNC
 * @tc.require: issueI7N7M3
 */
GRAPHIC_TEST(ParticleVelocityTest, CONTENT_DISPLAY_TEST, ParticleVelocityTest_003)
{
    std::vector<float> velocityValues = { 2.0f, 5.0f, 10.0f };
    std::vector<float> alphaList = { 0.3f, 0.6f, 1.0f };

    for (size_t row = 0; row < velocityValues.size(); row++) {
        for (size_t col = 0; col < alphaList.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 350 + 50, 300, 300 });
            testNode->SetBackgroundColor(0xffff0000);
            testNode->SetAlpha(alphaList[col]);

            auto fields = std::make_shared<ParticleVelocityFields>();
            auto field = std::make_shared<ParticleVelocityField>(
                Vector2f(velocityValues[row], velocityValues[row] * 2));
            fields->AddVelocityField(field);
            testNode->SetParticleVelocityFields(fields);

            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/*
 * @tc.name: ParticleVelocityTest_004
 * @tc.desc: Test SetParticleVelocityFields update scenario
 * @tc.type: FUNC
 * @tc.require: issueI7N7M3
 */
GRAPHIC_TEST(ParticleVelocityTest, CONTENT_DISPLAY_TEST, ParticleVelocityTest_004)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    // Set initial values
    auto fields1 = std::make_shared<ParticleVelocityFields>();
    auto field1 = std::make_shared<ParticleVelocityField>(Vector2f(1.0f, 2.0f));
    fields1->AddVelocityField(field1);
    testNode->SetParticleVelocityFields(fields1);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Update to different values
    auto fields2 = std::make_shared<ParticleVelocityFields>();
    auto field2 = std::make_shared<ParticleVelocityField>(Vector2f(5.0f, 10.0f));
    fields2->AddVelocityField(field2);
    testNode->SetParticleVelocityFields(fields2);

    // Update again
    auto fields3 = std::make_shared<ParticleVelocityFields>();
    auto field3 = std::make_shared<ParticleVelocityField>(Vector2f(10.0f, 20.0f));
    fields3->AddVelocityField(field3);
    testNode->SetParticleVelocityFields(fields3);
}

/*
 * @tc.name: ParticleVelocityTest_005
 * @tc.desc: Test SetParticleVelocityFields with rotation
 * @tc.type: FUNC
 * @tc.require: issueI7N7M3
 */
GRAPHIC_TEST(ParticleVelocityTest, CONTENT_DISPLAY_TEST, ParticleVelocityTest_005)
{
    std::vector<float> rotationList = { 0, 45, 90, 135 };

    for (size_t i = 0; i < rotationList.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 280 + 50, 50, 250, 250 });
        testNode->SetBackgroundColor(0xffff0000);
        testNode->SetRotation(rotationList[i]);

        auto fields = std::make_shared<ParticleVelocityFields>();
        float vx = 1.0f + i;
        float vy = 2.0f + i;
        auto field = std::make_shared<ParticleVelocityField>(Vector2f(vx, vy));
        fields->AddVelocityField(field);
        testNode->SetParticleVelocityFields(fields);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: ParticleVelocityTest_006
 * @tc.desc: Test SetParticleVelocityFields with scale (3x3 matrix)
 * @tc.type: FUNC
 * @tc.require: issueI7N7M3
 */
GRAPHIC_TEST(ParticleVelocityTest, CONTENT_DISPLAY_TEST, ParticleVelocityTest_006)
{
    std::vector<float> velocityScale = { 2.0f, 5.0f, 10.0f };
    std::vector<std::pair<float, float>> scaleList = {
        { 0.5f, 0.5f },
        { 1.0f, 1.0f },
        { 1.5f, 1.5f }
    };

    for (size_t row = 0; row < velocityScale.size(); row++) {
        for (size_t col = 0; col < scaleList.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 380 + 50, 300, 300 });
            testNode->SetBackgroundColor(0xffff0000);
            testNode->SetScale(scaleList[col].first, scaleList[col].second);

            auto fields = std::make_shared<ParticleVelocityFields>();
            auto field = std::make_shared<ParticleVelocityField>(
                Vector2f(velocityScale[row], velocityScale[row] * 2));
            fields->AddVelocityField(field);
            testNode->SetParticleVelocityFields(fields);

            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/*
 * @tc.name: ParticleVelocityTest_007
 * @tc.desc: Test SetParticleVelocityFields with zero bounds
 * @tc.type: FUNC
 * @tc.require: issueI7N7M3
 */
GRAPHIC_TEST(ParticleVelocityTest, CONTENT_DISPLAY_TEST, ParticleVelocityTest_007)
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

        auto fields = std::make_shared<ParticleVelocityFields>();
        float vx = 1.0f * (i + 1);
        float vy = 2.0f * (i + 1);
        auto field = std::make_shared<ParticleVelocityField>(Vector2f(vx, vy));
        fields->AddVelocityField(field);
        testNode->SetParticleVelocityFields(fields);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: ParticleVelocityTest_008
 * @tc.desc: Test SetParticleVelocityFields with translation
 * @tc.type: FUNC
 * @tc.require: issueI7N7M3
 */
GRAPHIC_TEST(ParticleVelocityTest, CONTENT_DISPLAY_TEST, ParticleVelocityTest_008)
{
    std::vector<std::pair<float, float>> translateList = {
        { 0, 0 },
        { 50, 0 },
        { 0, 50 },
        { 50, 50 }
    };

    for (size_t i = 0; i < translateList.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 280 + 50, 50, 250, 250 });
        testNode->SetBackgroundColor(0xffff0000);
        testNode->SetTranslate(Vector2f(translateList[i].first, translateList[i].second));

        auto fields = std::make_shared<ParticleVelocityFields>();
        float vx = 1.0f + i;
        float vy = 2.0f + i;
        auto field = std::make_shared<ParticleVelocityField>(Vector2f(vx, vy));
        fields->AddVelocityField(field);
        testNode->SetParticleVelocityFields(fields);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: ParticleVelocityTest_009
 * @tc.desc: Test SetParticleVelocityFields extreme values
 * @tc.type: FUNC
 * @tc.require: issueI7N7M3
 */
GRAPHIC_TEST(ParticleVelocityTest, CONTENT_DISPLAY_TEST, ParticleVelocityTest_009)
{
    std::vector<std::tuple<float, float, float>> extremeParams = {
        { 0.0f, 0.0f, 0.0f },           // All zeros
        { 0.001f, 0.001f, 0.001f },     // Very small
        { 100.0f, 200.0f, 300.0f },     // Large
        { 1000.0f, 2000.0f, 3000.0f }   // Very large
    };

    for (size_t i = 0; i < extremeParams.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 280 + 50, 50, 250, 250 });
        testNode->SetBackgroundColor(0xffff0000);

        auto [param1, param2, param3] = extremeParams[i];
        auto fields = std::make_shared<ParticleVelocityFields>();
        auto field = std::make_shared<ParticleVelocityField>(Vector2f(param1, param2));
        fields->AddVelocityField(field);
        testNode->SetParticleVelocityFields(fields);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: ParticleVelocityTest_010
 * @tc.desc: Test SetParticleVelocityFields with pixelmap
 * @tc.type: FUNC
 * @tc.require: issueI7N7M3
 */
GRAPHIC_TEST(ParticleVelocityTest, CONTENT_DISPLAY_TEST, ParticleVelocityTest_010)
{
    auto pixelMap = DecodePixelMap("/data/local/tmp/appearance_test.jpg",
                                   Media::AllocatorType::SHARE_MEM_ALLOC);

    std::vector<std::tuple<float, float, float>> paramsList = {
        { 1.0f, 2.0f, 3.0f },
        { 3.0f, 6.0f, 9.0f },
        { 5.0f, 10.0f, 15.0f },
        { 7.0f, 14.0f, 21.0f }
    };

    for (size_t i = 0; i < paramsList.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 280 + 50, 50, 250, 250 });
        testNode->SetPixelmap(pixelMap);

        auto [param1, param2, param3] = paramsList[i];
        auto fields = std::make_shared<ParticleVelocityFields>();
        auto field = std::make_shared<ParticleVelocityField>(Vector2f(param1, param2));
        fields->AddVelocityField(field);
        testNode->SetParticleVelocityFields(fields);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

} // namespace OHOS::Rosen
