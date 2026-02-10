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
    testNode->SetParticleRippleFields(1.0f, 2.0f, 3.0f);

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
        testNode->SetParticleRippleFields(param1, param2, param3);

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
    std::vector<float> param1Values = { 0.5f, 1.0f, 1.5f };
    std::vector<float> alphaList = { 0.3f, 0.6f, 1.0f };

    for (size_t row = 0; row < param1Values.size(); row++) {
        for (size_t col = 0; col < alphaList.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 350 + 50, 300, 300 });
            testNode->SetBackgroundColor(0xffff0000);
            testNode->SetAlpha(alphaList[col]);

            testNode->SetParticleRippleFields(param1Values[row], 2.0f, 3.0f);

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
    testNode->SetParticleRippleFields(1.0f, 2.0f, 3.0f);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Update to different values
    testNode->SetParticleRippleFields(2.0f, 4.0f, 6.0f);

    // Update again
    testNode->SetParticleRippleFields(3.0f, 6.0f, 9.0f);
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

        testNode->SetParticleRippleFields(1.0f, 2.0f, 3.0f);

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
    std::vector<float> param2Values = { 1.0f, 2.0f, 3.0f };
    std::vector<std::pair<float, float>> scaleList = {
        { 0.5f, 0.5f },
        { 1.0f, 1.0f },
        { 1.5f, 1.5f }
    };

    for (size_t row = 0; row < param2Values.size(); row++) {
        for (size_t col = 0; col < scaleList.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 380 + 50, 300, 300 });
            testNode->SetBackgroundColor(0xffff0000);
            testNode->SetScale(scaleList[col].first, scaleList[col].second);

            testNode->SetParticleRippleFields(1.0f, param2Values[row], 3.0f);

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

        testNode->SetParticleRippleFields(1.0f + i, 2.0f + i, 3.0f + i);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: ParticleRippleTest_008
 * @tc.desc: Test SetParticleRippleFields with different param3 values (3x4 matrix)
 * @tc.type: FUNC
 * @tc.require: issueI7N7M2
 */
GRAPHIC_TEST(ParticleRippleTest, CONTENT_DISPLAY_TEST, ParticleRippleTest_008)
{
    std::vector<float> param3Values = { 1.0f, 2.0f, 3.0f };
    std::vector<float> alphaList = { 0.3f, 0.6f, 1.0f };

    for (size_t row = 0; row < param3Values.size(); row++) {
        for (size_t col = 0; col < alphaList.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 350 + 50, 300, 300 });
            testNode->SetBackgroundColor(0xffff0000);
            testNode->SetAlpha(alphaList[col]);

            testNode->SetParticleRippleFields(1.0f, 2.0f, param3Values[row]);

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
        { 0.5f, 1.0f, 1.5f },
        { 1.0f, 2.0f, 3.0f },
        { 1.5f, 3.0f, 4.5f },
        { 2.0f, 4.0f, 6.0f }
    };

    for (size_t i = 0; i < paramsList.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 280 + 50, 50, 250, 250 });
        testNode->SetPixelMap(pixelMap);

        auto [param1, param2, param3] = paramsList[i];
        testNode->SetParticleRippleFields(param1, param2, param3);

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
        testNode->SetParticleRippleFields(param1, param2, param3);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

} // namespace OHOS::Rosen
