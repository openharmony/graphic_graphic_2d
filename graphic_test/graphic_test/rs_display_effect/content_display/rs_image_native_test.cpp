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
 
#include "ui/rs_effect_node.h"
 
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RsImageNativeTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;
    Drawing::SamplingOptions sampling;
    Drawing::Rect frameRect;
    std::shared_ptr<Media::PixelMap> bgpixelmap =
            DecodePixelMap("/data/local/tmp/Images/3200x2000.jpg", Media::AllocatorType::SHARE_MEM_ALLOC);
    std::shared_ptr<Media::PixelMap> smpixelmap =
            DecodePixelMap("/data/local/tmp/fg_test.jpg", Media::AllocatorType::SHARE_MEM_ALLOC);
public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSurfaceBounds({ 0, 0, screenWidth, screenHeight });
    }
};

GRAPHIC_TEST(RsImageNativeTest, CONTENT_DISPLAY_TEST, RS_IMAGE_SCALE_TEST)
{
    if (smpixelmap == nullptr) {
        return;
    }

    float scale = 0.5;
    int32_t nodeWith = 800;
    int32_t nodeHeight = 500;
    int32_t gap = 100;
    int32_t raw = 4;

    for (int32_t i = 0; i < raw; i++) {
        auto rosenImage = std::make_shared<Rosen::RSImage>();
        rosenImage->SetImageFit(static_cast<int>(ImageFit::SCALE_DOWN));
        rosenImage->SetScale(scale);
        scale += 0.5;
        auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetClipToBounds(true);
        canvasNode->SetBounds({gap, gap + (nodeHeight + gap) * i, nodeWith, nodeHeight});
        canvasNode->SetFrame({gap, gap + (nodeHeight + gap) * i, nodeWith, nodeHeight});
        canvasNode->SetBackgroundColor(0xFFFFFFFF);
        GetRootNode()->AddChild(canvasNode);

        auto drawing = canvasNode->BeginRecording(gap + nodeWith, (gap + nodeHeight) * (i + 1));
        drawing->DrawPixelMapWithParm(smpixelmap, imageInfo, sampling);
        canvasNode->FinishRecording();
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        RegisterNode(canvasNode);
    }
}

GRAPHIC_TEST(RsImageNativeTest, CONTENT_DISPLAY_TEST, RS_IMAGE_RADIUS_TEST)
{
    if (smpixelmap == nullptr) {
        return;
    }

    float radiusCorner = 1.f;
    int32_t nodeWith = 800;
    int32_t nodeHeight = 500;
    int32_t gap = 100;
    int32_t raw = 4;

    for (int32_t i = 0; i < raw; i++) {
        auto rosenImage = std::make_shared<Rosen::RSImage>();
        std::vector<Drawing::Point> radius = { Drawing::Point {radiusCorner, radiusCorner},
            Drawing::Point {radiusCorner, radiusCorner}, Drawing::Point {radiusCorner, radiusCorner},
            Drawing::Point {radiusCorner, radiusCorner}};
        rosenImage->SetRadius(radius);
        radiusCorner += 20.f;
        auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetClipToBounds(true);
        canvasNode->SetBounds({gap, gap + (nodeHeight + gap) * i, nodeWith, nodeHeight});
        canvasNode->SetFrame({gap, gap + (nodeHeight + gap) * i, nodeWith, nodeHeight});
        canvasNode->SetBackgroundColor(0xFFFFFFFF);
        GetRootNode()->AddChild(canvasNode);

        auto drawing = canvasNode->BeginRecording(gap + nodeWith, (gap + nodeHeight) * (i + 1));
        drawing->DrawPixelMapWithParm(smpixelmap, imageInfo, sampling);
        canvasNode->FinishRecording();
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        RegisterNode(canvasNode);
    }
}

GRAPHIC_TEST(RsImageNativeTest, CONTENT_DISPLAY_TEST, RS_IMAGE_NATIVE_IMAGEFIT_TEST)
{
    if (bgpixelmap == nullptr) {
        return;
    }

    int32_t fit = 0;
    int32_t nodeWith = 300;
    int32_t nodeHeight = 300;
    int32_t gap = 100;
    int32_t column = 3;
    int32_t raw = 6;

    for (int32_t i = 0; i < raw; i++) {
        for (int32_t j = 0; j < column; j++) {
            auto rosenImage = std::make_shared<Rosen::RSImage>();
            rosenImage->SetImageFit(fit++);
            auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

            auto canvasNode = RSCanvasNode::Create();
            canvasNode->SetClipToBounds(true);
            canvasNode->SetBounds({gap + (nodeWith + gap) * j, gap + (nodeHeight + gap) * i, nodeWith, nodeHeight});
            canvasNode->SetFrame({gap + (nodeWith + gap) * j, gap + (nodeHeight + gap) * i, nodeWith, nodeHeight});
            canvasNode->SetBackgroundColor(0xFFFFFFFF);
            GetRootNode()->AddChild(canvasNode);

            auto drawing = canvasNode->BeginRecording((gap + nodeWith) * (j + 1), (gap + nodeHeight) * (i + 1));
            drawing->DrawPixelMapWithParm(bgpixelmap, imageInfo, sampling);
            canvasNode->FinishRecording();
            RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
            RegisterNode(canvasNode);
        }
    }
}

GRAPHIC_TEST(RsImageNativeTest, CONTENT_DISPLAY_TEST, RS_IMAGE_REPEAT_TEST)
{
    if (smpixelmap == nullptr) {
        return;
    }

    int32_t repeat = 0;
    int32_t nodeWith = 800;
    int32_t nodeHeight = 500;
    int32_t gap = 100;
    int32_t raw = 4;

    for (int32_t i = 0; i < raw; i++) {
        auto rosenImage = std::make_shared<Rosen::RSImage>();
        rosenImage->SetImageFit(static_cast<int>(ImageFit::TOP_LEFT));
        rosenImage->SetImageRepeat(repeat++);
        auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetClipToBounds(true);
        canvasNode->SetBounds({gap, gap + (nodeHeight + gap) * i, nodeWith, nodeHeight});
        canvasNode->SetFrame({gap, gap + (nodeHeight + gap) * i, nodeWith, nodeHeight});
        canvasNode->SetBackgroundColor(0xFFFFFFFF);
        GetRootNode()->AddChild(canvasNode);

        auto drawing = canvasNode->BeginRecording(gap + nodeWith, (gap + nodeHeight) * (i + 1));
        drawing->DrawPixelMapWithParm(smpixelmap, imageInfo, sampling);
        canvasNode->FinishRecording();
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        RegisterNode(canvasNode);
    }
}

GRAPHIC_TEST(RsImageNativeTest, CONTENT_DISPLAY_TEST, RS_IMAGE_ORIENTATION_TEST)
{
    if (bgpixelmap == nullptr) {
        return;
    }

    int32_t orientationFit = 0;
    int32_t nodeWith = 800;
    int32_t nodeHeight = 500;
    int32_t gap = 100;
    int32_t raw = 3;

    for (int32_t i = 0; i < raw; i++) {
        auto rosenImage = std::make_shared<Rosen::RSImage>();
        rosenImage->SetOrientationFit(orientationFit++);
        auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetClipToBounds(true);
        canvasNode->SetBounds({gap, gap + (nodeHeight + gap) * i, nodeWith, nodeHeight});
        canvasNode->SetFrame({gap, gap + (nodeHeight + gap) * i, nodeWith, nodeHeight});
        canvasNode->SetBackgroundColor(0xFFFFFFFF);
        GetRootNode()->AddChild(canvasNode);

        auto drawing = canvasNode->BeginRecording(gap + nodeWith, (gap + nodeHeight) * (i + 1));
        drawing->DrawPixelMapWithParm(bgpixelmap, imageInfo, sampling);
        canvasNode->FinishRecording();
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        RegisterNode(canvasNode);
    }
}

GRAPHIC_TEST(RsImageNativeTest, CONTENT_DISPLAY_TEST, RS_IMAGE_ROTATE_DEGREE_TEST)
{
    if (bgpixelmap == nullptr) {
        return;
    }

    int32_t rotateDegree = -90;
    int32_t nodeWith = 800;
    int32_t nodeHeight = 500;
    int32_t gap = 100;
    int32_t raw = 4;

    for (int32_t i = 0; i < raw; i++) {
        auto rosenImage = std::make_shared<Rosen::RSImage>();
        rosenImage->SetImageRotateDegree(rotateDegree);
        rotateDegree += 90;
        auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetClipToBounds(true);
        canvasNode->SetBounds({gap, gap + (nodeHeight + gap) * i, nodeWith, nodeHeight});
        canvasNode->SetFrame({gap, gap + (nodeHeight + gap) * i, nodeWith, nodeHeight});
        canvasNode->SetBackgroundColor(0xFFFFFFFF);
        GetRootNode()->AddChild(canvasNode);

        auto drawing = canvasNode->BeginRecording(gap + nodeWith, (gap + nodeHeight) * (i + 1));
        drawing->DrawPixelMapWithParm(bgpixelmap, imageInfo, sampling);
        canvasNode->FinishRecording();
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        RegisterNode(canvasNode);
    }
}

GRAPHIC_TEST(RsImageNativeTest, CONTENT_DISPLAY_TEST, RS_IMAGE_ROTATE_DEGREE_AND_ORIENTATION_TEST)
{
    if (smpixelmap == nullptr) {
        return;
    }

    int32_t rotateDegree = -90;
    int32_t orientationFit = 0;
    int32_t nodeWith = 300;
    int32_t nodeHeight = 300;
    int32_t gap = 100;
    int32_t column = 3;
    int32_t raw = 4;

    for (int32_t i = 0; i < raw; i++) {
        orientationFit = 0;
        for (int32_t j = 0; j < column; j++) {
            auto rosenImage = std::make_shared<Rosen::RSImage>();
            rosenImage->SetOrientationFit(orientationFit++);
            rosenImage->SetImageRotateDegree(rotateDegree);
            auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

            auto canvasNode = RSCanvasNode::Create();
            canvasNode->SetClipToBounds(true);
            canvasNode->SetBounds({gap + (nodeWith + gap) * j, gap + (nodeHeight + gap) * i, nodeWith, nodeHeight});
            canvasNode->SetFrame({gap + (nodeWith + gap) * j, gap + (nodeHeight + gap) * i, nodeWith, nodeHeight});
            canvasNode->SetBackgroundColor(0xFFFFFFFF);
            GetRootNode()->AddChild(canvasNode);

            auto drawing = canvasNode->BeginRecording((gap + nodeWith) * (j + 1), (gap + nodeHeight) * (i + 1));
            drawing->DrawPixelMapWithParm(smpixelmap, imageInfo, sampling);
            canvasNode->FinishRecording();
            RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
            RegisterNode(canvasNode);
        }
        rotateDegree += 90;
    }
}

GRAPHIC_TEST(RsImageNativeTest, CONTENT_DISPLAY_TEST, RS_IMAGE_FIT_MATRIX_TEST)
{
    if (smpixelmap == nullptr) {
        return;
    }

    std::array<Drawing::Matrix, 15> matrixs;

    Drawing::Matrix mat1;
    matrixs[0] = mat1;

    Drawing::Matrix mat2;
    Drawing::scalar testTranslateX1 = 150;
    mat2.Translate(testTranslateX1, 0);
    matrixs[1] = mat2;

    Drawing::Matrix mat3;
    Drawing::scalar testTranslateY1 = 150;
    mat3.Translate(0, testTranslateY1);
    matrixs[2] = mat3;

    Drawing::Matrix mat4;
    Drawing::scalar testTranslateX2 = -150;
    mat4.Translate(testTranslateX2, 0);
    matrixs[3] = mat4;

    Drawing::Matrix mat5;
    Drawing::scalar testTranslateY2 = -150;
    mat5.Translate(0, testTranslateY2);
    matrixs[4] = mat5;

    Drawing::Matrix mat6;
    mat6.Translate(testTranslateX2, testTranslateY2);
    matrixs[5] = mat6;

    Drawing::Matrix mat7;
    Drawing::scalar testRotateNum1 = 45;
    mat7.Rotate(testRotateNum1, 0, 0);
    matrixs[6] = mat7;

    Drawing::Matrix mat8;
    Drawing::scalar testRotateNum2 = -45;
    mat8.Rotate(testRotateNum2, 0, 0);
    matrixs[7] = mat8;

    Drawing::Matrix mat9;
    Drawing::scalar testRotateNum3 = 90;
    mat9.Rotate(testRotateNum3, testTranslateX1, testTranslateY1);
    matrixs[8] = mat9;

    Drawing::Matrix mat10;
    Drawing::scalar testRotateNum4 = 180;
    mat10.Rotate(testRotateNum4, testTranslateX1, testTranslateY1);
    matrixs[9] = mat10;

    Drawing::Matrix mat11;
    Drawing::scalar testRotateNum5 = -90;
    mat11.Rotate(testRotateNum5, testTranslateX1, testTranslateY1);
    matrixs[10] = mat11;

    Drawing::Matrix mat12;
    Drawing::scalar scaleX = 10;
    mat12.SetScale(scaleX, 1);
    matrixs[11] = mat12;

    Drawing::Matrix mat13;
    Drawing::scalar scaleY = 10;
    mat13.SetScale(1, scaleY);
    matrixs[12] = mat13;

    Drawing::Matrix mat14;
    mat14.SetScale(scaleX, scaleY);
    matrixs[13] = mat14;

    Drawing::Matrix mat15;
    Drawing::Point p1 = {0, 0};
    Drawing::Point p2 = {100, 0};
    Drawing::Point p3 = {0, 100};
    Drawing::Point p4 = {100, 100};
    Drawing::Point src[] = {p1, p2, p3, p4};
    Drawing::Point p5 = {0, 0};
    Drawing::Point p6 = {100, 0};
    Drawing::Point p7 = {0, 100};
    Drawing::Point p8 = {75, 100};
    Drawing::Point dst[] = {p5, p6, p7, p8};
    uint32_t cnt = 4;
    mat15.SetPolyToPoly(src, dst, cnt);
    matrixs[14] = mat15;

    int32_t nodeWith = 300;
    int32_t nodeHeight = 300;
    int32_t gap = 100;
    int32_t column = 3;
    int32_t raw = 5;

    for (int32_t i = 0; i < raw; i++) {
        for (int32_t j = 0; j < column; j++) {
            auto rosenImage = std::make_shared<Rosen::RSImage>();
            rosenImage->SetImageFit(static_cast<int>(ImageFit::MATRIX));
            rosenImage->SetFitMatrix(matrixs[i * column + j]);
            auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

            auto canvasNode = RSCanvasNode::Create();
            canvasNode->SetClipToBounds(true);
            canvasNode->SetBounds({gap + (nodeWith + gap) * j, gap + (nodeHeight + gap) * i, nodeWith, nodeHeight});
            canvasNode->SetFrame({gap + (nodeWith + gap) * j, gap + (nodeHeight + gap) * i, nodeWith, nodeHeight});
            canvasNode->SetBackgroundColor(0xFFFFFFFF);
            GetRootNode()->AddChild(canvasNode);

            auto drawing = canvasNode->BeginRecording((gap + nodeWith) * (j + 1), (gap + nodeHeight) * (i + 1));
            drawing->DrawPixelMapWithParm(smpixelmap, imageInfo, sampling);
            canvasNode->FinishRecording();
            RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
            RegisterNode(canvasNode);
        }
    }
}
} // namespace OHOS::Rosen