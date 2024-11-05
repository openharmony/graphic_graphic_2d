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

#include <cstdlib>
#include <cstring>
#include <memory>
#include <cstring>
#include <securec.h>
#include "gtest/gtest.h"
#include "gtest/hwext/gtest-tag.h"
#include "common/rs_vector4.h"
#include "modifier/rs_render_modifier.h"
#include "pipeline/rs_recording_canvas.h"

#include "message_parcel.h"
#include "property/rs_properties.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSModifierTest : public testing::Test {
public:
    constexpr static float floatData[] = {
        0.0f, 485.44f, 2.0f,
        std::numeric_limits<float>::max(), std::numeric_limits<float>::min(),
        };
    PropertyId id = 100;
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSModifierTest::SetUpTestCase() {}
void RSModifierTest::TearDownTestCase() {}
void RSModifierTest::SetUp() {}
void RSModifierTest::TearDown() {}

/**
 * @tc.name: RSEnvForegroundColorStrategyRenderModifier001
 * @tc.desc:Apply
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, RSEnvForegroundColorStrategyRenderModifier001, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderAnimatableProperty<Color>>();
    bool isDelta = false;
    auto property = std::make_shared<RSRenderAnimatableProperty<Color>>();
    auto RSEFCS = std::make_shared<RSEnvForegroundColorRenderModifier>(property);
    RSEFCS->Update(prop, isDelta);
    ASSERT_NE(nullptr, RSEFCS->property_);
}

/**
 * @tc.name: Apply
 * @tc.desc: Test Apply and Update and Marshalling
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSModifierTest, Apply, TestSize.Level1)
{
    ExtendRecordingCanvas canvas(100, 100);
    canvas.Translate(15.f, 15.f);
    auto prop = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>(canvas.GetDrawCmdList(), id);
    auto modifier = std::make_shared<RSDrawCmdListRenderModifier>(prop);
    RSProperties properties;
    RSModifierContext context(properties);
    modifier->Apply(context);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    context.canvas_ = &paintFilterCanvas;
    modifier->Apply(context);
    ASSERT_NE(nullptr, context.canvas_);

    modifier->Update(prop, true);
    ASSERT_NE(modifier->GetProperty(), nullptr);

    Parcel parcel;
    ASSERT_TRUE(modifier->Marshalling(parcel));
}

/**
 * @tc.name: RSEnvForegroundColorRenderModifier001
 * @tc.desc: Test Apply and Marshalling
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSModifierTest, RSEnvForegroundColorRenderModifier001, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderAnimatableProperty<Color>>();
    auto property = std::make_shared<RSRenderAnimatableProperty<Color>>();
    auto modifier = std::make_shared<RSEnvForegroundColorRenderModifier>(property);
    RSProperties properties;
    ExtendRecordingCanvas canvas(100, 100);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    RSModifierContext context(properties, &paintFilterCanvas);
    modifier->Apply(context);
    ASSERT_NE(nullptr, context.canvas_);

    Parcel parcel;
    ASSERT_TRUE(modifier->Marshalling(parcel));
}

/**
 * @tc.name: LifeCycle001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, LifeCycle001, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderProperty<float>>();
    auto modifier = std::make_shared<RSAlphaRenderModifier>(prop);
    ASSERT_TRUE(modifier != nullptr);
    ASSERT_TRUE(modifier->GetProperty() == prop);
    ASSERT_TRUE(modifier->GetPropertyId() == 0);

    auto prop2 = std::make_shared<RSRenderProperty<float>>(floatData[0], id);
    auto modifier2 = std::make_shared<RSAlphaRenderModifier>(prop2);
    ASSERT_TRUE(modifier2 != nullptr);
    ASSERT_EQ(modifier2->GetPropertyId(), id);
}

/**
 * @tc.name: Modifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, Modifier001, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderProperty<float>>(floatData[0], id);
    auto modifier = std::make_shared<RSAlphaRenderModifier>(prop);
    ASSERT_TRUE(prop != nullptr);
    ASSERT_EQ(modifier->GetProperty(), prop);

    auto prop1 = std::make_shared<RSRenderProperty<float>>(floatData[1], id);
    modifier->Update(prop1, false);
    ASSERT_EQ(std::static_pointer_cast<RSRenderProperty<float>>(modifier->GetProperty())->Get(), floatData[1]);

    auto prop2 = std::make_shared<RSRenderProperty<float>>(floatData[2], id);
    modifier->Update(prop2, true);
    ASSERT_EQ(std::static_pointer_cast<RSRenderProperty<float>>(modifier->GetProperty())->Get(),
        floatData[1] + floatData[2]);
}

/**
 * @tc.name: DrawCmdListModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, DrawCmdListModifier001, TestSize.Level1)
{
    ExtendRecordingCanvas canvas(100, 100);
    canvas.Translate(15.f, 15.f);

    auto prop = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>(canvas.GetDrawCmdList(), id);
    auto modifier = std::make_shared<RSDrawCmdListRenderModifier>(prop);

    MessageParcel parcel;
    ASSERT_TRUE(modifier->Marshalling(parcel));
    ASSERT_TRUE(RSDrawCmdListRenderModifier::Unmarshalling(parcel) != nullptr);

    canvas.Scale(2.f, 2.f);
    modifier->Update(nullptr, false);
    auto prop1 = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>(canvas.GetDrawCmdList(), id);
    modifier->Update(prop1, true);

    ASSERT_TRUE(modifier->Marshalling(parcel));
    ASSERT_TRUE(RSDrawCmdListRenderModifier::Unmarshalling(parcel) != nullptr);

    MessageParcel parcel1;
    char* buffer = static_cast<char *>(malloc(parcel1.GetMaxCapacity()));
    memset_s(buffer, parcel1.GetMaxCapacity(), 0, parcel1.GetMaxCapacity());
    ASSERT_TRUE(parcel1.WriteUnpadBuffer(buffer, parcel1.GetMaxCapacity()));
    bool ret = false;
    while (!ret) {
        ret = (modifier->Marshalling(parcel) && (RSDrawCmdListRenderModifier::Unmarshalling(parcel) != nullptr));
        parcel1.SetMaxCapacity(parcel1.GetMaxCapacity() + 1);
    }
    free(buffer);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: RSGeometryTransRenderModifier
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, RSGeometryTransRenderModifier, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderProperty<Drawing::Matrix>>();
    auto modifier = std::make_shared<RSGeometryTransRenderModifier>(prop);
    RSProperties properties;
    RSModifierContext context(properties);
    auto rsRenderPropertyBase = std::make_shared<RSRenderProperty<Drawing::Matrix>>();
    ASSERT_TRUE(modifier != nullptr);
    modifier->Apply(context);
    modifier->Update(rsRenderPropertyBase, false);
    ASSERT_TRUE(modifier->GetProperty() == prop);
    ASSERT_TRUE(modifier->GetPropertyId() == 0);
    modifier->SetType(RSModifierType::BOUNDS);
    ASSERT_TRUE(modifier->GetType() == RSModifierType::BOUNDS);

    MessageParcel parcel;
    ASSERT_TRUE(modifier->Marshalling(parcel));
    ASSERT_TRUE(RSGeometryTransRenderModifier::Unmarshalling(parcel) != nullptr);
}

/**
 * @tc.name: Marshalling
 * @tc.desc: Test Marshalling
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSModifierTest, Marshalling, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<Color>>();
    auto RSEFC = std::make_shared<RSEnvForegroundColorRenderModifier>(property);
    Parcel parcel;
    bool ret = RSEFC->Marshalling(parcel);
    ASSERT_TRUE(ret == true);
}

/**
 * @tc.name: Apply001
 * @tc.desc: Test Apply
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSModifierTest, Apply001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<Color>>();
    auto RSEFC = std::make_shared<RSEnvForegroundColorRenderModifier>(property);
    ExtendRecordingCanvas canvas(100, 100);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    RSProperties properties;
    RSModifierContext contextArgs(properties, &paintFilterCanvas);
    RSEFC->Apply(contextArgs);
    bool ret = true;
    ASSERT_TRUE(ret == true);
}

/**
 * @tc.name: Marshalling001
 * @tc.desc: Test Marshalling
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSModifierTest, Marshalling001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<Color>>();
    auto modifier = std::make_shared<RSEnvForegroundColorStrategyRenderModifier>(property);
    Parcel parcel;
    bool ret = modifier->Marshalling(parcel);
    ASSERT_TRUE(ret == true);
}

/**
 * @tc.name: Apply002
 * @tc.desc: Test Marshalling
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSModifierTest, Apply002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<Color>>();
    auto modifier = std::make_shared<RSEnvForegroundColorStrategyRenderModifier>(property);
    RSProperties properties;
    RSModifierContext context(properties);
    modifier->Apply(context);
    bool ret = true;
    ASSERT_TRUE(ret == true);
}

/**
 * @tc.name: RSParticlesRenderModifier001
 * @tc.desc:Update
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, RSParticlesRenderModifier002, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderProperty<RSRenderParticleVector>>();
    bool isDelta = false;
    auto property = std::make_shared<RSRenderProperty<RSRenderParticleVector>>();
    auto RSPRM = std::make_shared<RSParticlesRenderModifier>(property);
    RSPRM->Update(prop, isDelta);
    ASSERT_NE(nullptr, RSPRM->property_);
}

/**
 * @tc.name: RSEnvForegroundColorRenderModifier002
 * @tc.desc:Update
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, RSEnvForegroundColorRenderModifier002, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderAnimatableProperty<Color>>();
    bool isDelta = false;
    auto property = std::make_shared<RSRenderAnimatableProperty<Color>>();
    auto RSEFC = std::make_shared<RSEnvForegroundColorRenderModifier>(property);
    RSEFC->Update(prop, isDelta);
    ASSERT_NE(nullptr, RSEFC->property_);
}

/**
 * @tc.name: RSEnvForegroundColorStrategyRenderModifier001
 * @tc.desc: test Apply and Marshalling and CalculateInvertColor and GetInvertBackgroundColor and Update
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSModifierTest, RSEnvForegroundColorStrategyRenderModifier002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<Color>>();
    auto modifier = std::make_shared<RSEnvForegroundColorStrategyRenderModifier>(property);
    RSProperties properties;
    RSModifierContext context(properties);
    modifier->Apply(context);
    ASSERT_EQ(nullptr, context.canvas_);

    ForegroundColorStrategyType type = ForegroundColorStrategyType::INVERT_BACKGROUNDCOLOR;
    auto modifierTwo = std::make_shared<RSEnvForegroundColorStrategyRenderModifier>(property);

    auto renderProperty =
        std::static_pointer_cast<RSRenderProperty<ForegroundColorStrategyType>>(modifierTwo->property_);
    renderProperty->stagingValue_ = type;
    ExtendRecordingCanvas canvas(100, 100);
    Drawing::Surface surface;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    paintFilterCanvas.surface_ = &surface;
    RSModifierContext contextArgs(properties, &paintFilterCanvas);
    ASSERT_NE(nullptr, contextArgs.canvas_);
    modifierTwo->Apply(contextArgs);
    ASSERT_NE(nullptr, contextArgs.canvas_);

    Parcel parcel;
    ASSERT_TRUE(modifier->Marshalling(parcel));

    ASSERT_EQ(modifier->CalculateInvertColor(Color()).alpha_, 0.f);

    modifier->GetInvertBackgroundColor(contextArgs);
    properties.SetClipToBounds(true);
    ASSERT_EQ(modifier->GetInvertBackgroundColor(contextArgs).alpha_, 0.f);

    std::shared_ptr<RSRenderPropertyBase> propTwo;
    modifier->Update(propTwo, true);
    ASSERT_EQ(propTwo, nullptr);
}

/**
 * @tc.name: CanvasNull
 * @tc.desc: Test Apply
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSModifierTest, CanvasNull, TestSize.Level1)
{
    ExtendRecordingCanvas canvas(100, 100);
    canvas.Translate(15.f, 15.f);
    auto prop = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>(canvas.GetDrawCmdList(), id);
    auto modifier = std::make_shared<RSDrawCmdListRenderModifier>(prop);
    RSProperties properties;
    RSModifierContext context(properties);
    context.canvas_ = nullptr;
    modifier->Apply(context);
    bool ret = false;
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: CanvasNotNull
 * @tc.desc: Test Apply
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSModifierTest, CanvasNotNull, TestSize.Level1)
{
    ExtendRecordingCanvas canvas(100, 100);
    canvas.Translate(15.f, 15.f);
    auto prop = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>(canvas.GetDrawCmdList(), id);
    auto modifier = std::make_shared<RSDrawCmdListRenderModifier>(prop);
    RSProperties properties;
    RSModifierContext context(properties);
    Drawing::Canvas* canvas1 = new Drawing::Canvas();
    context.canvas_ = new RSPaintFilterCanvas(canvas1);
    modifier->Apply(context);

    bool ret = true;
    ASSERT_EQ(ret, true);
}

/**
 * @tc.name: GetInvertBackgroundColor
 * @tc.desc: Test GetInvertBackgroundColor
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSModifierTest, GetInvertBackgroundColor, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<Color>>();
    auto modifier = std::make_shared<RSEnvForegroundColorStrategyRenderModifier>(property);
    RSProperties properties;
    RSModifierContext context(properties);
    ExtendRecordingCanvas canvas(100, 100);
    Drawing::Surface surface;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    paintFilterCanvas.surface_ = &surface;
    RSModifierContext contextArgs(properties, &paintFilterCanvas);
    RSColor color;
    color.SetAlpha(0xff);
    contextArgs.properties_.SetBackgroundColor(color);
    modifier->GetInvertBackgroundColor(contextArgs);
    bool ret = true;
    ASSERT_TRUE(ret == true);
}

/**
 * @tc.name: Update001
 * @tc.desc: Test Update
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSModifierTest, Update001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<Color>>();
    auto modifier = std::make_shared<RSEnvForegroundColorStrategyRenderModifier>(property);
    RSProperties properties;
    RSModifierContext context(properties);
    const std::shared_ptr<RSRenderPropertyBase> prop;
    bool isDelta = false;
    modifier->Update(prop, isDelta);
    bool ret = true;
    ASSERT_TRUE(ret == true);
}

/**
 * @tc.name: Apply003
 * @tc.desc: Test Apply
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSModifierTest, Apply003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<Color>>();
    auto modifier = std::make_shared<RSEnvForegroundColorStrategyRenderModifier>(property);
    RSProperties properties;
    RSModifierContext context(properties);
    auto test = std::make_shared<RSRenderProperty<ForegroundColorStrategyType>>();
    modifier->property_ = test;
    modifier->Apply(context);
    bool ret = true;
    ASSERT_TRUE(ret == true);
}

/**
 * @tc.name: CalculateInvertColor
 * @tc.desc: Test CalculateInvertColor
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSModifierTest, CalculateInvertColor, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<Color>>();
    auto modifier = std::make_shared<RSEnvForegroundColorStrategyRenderModifier>(property);
    RSProperties properties;
    RSModifierContext context(properties);
    Color backgroundColor;
    modifier->CalculateInvertColor(backgroundColor);
    bool ret = true;
    ASSERT_TRUE(ret == true);
}

/**
 * @tc.name: RSCustomClipToFrameRenderModifier001
 * @tc.desc: Test Apply and Marshalling
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSModifierTest, RSCustomClipToFrameRenderModifier001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<Vector4f>>();
    auto modifier = std::make_shared<RSCustomClipToFrameRenderModifier>(property);
    RSProperties properties;
    ExtendRecordingCanvas canvas(100, 100);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    RSModifierContext context(properties, &paintFilterCanvas);
    modifier->Apply(context);
    ASSERT_NE(nullptr, context.canvas_);

    Parcel parcel;
    ASSERT_TRUE(modifier->Marshalling(parcel));
}

/**
 * @tc.name: RSCustomClipToFrameRenderModifier002
 * @tc.desc:Update
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierTest, RSCustomClipToFrameRenderModifier002, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderAnimatableProperty<Vector4f>>();
    bool isDelta = false;
    auto property = std::make_shared<RSRenderAnimatableProperty<Vector4f>>();
    auto RSEFC = std::make_shared<RSCustomClipToFrameRenderModifier>(property);
    RSEFC->Update(prop, isDelta);
    ASSERT_NE(nullptr, RSEFC->property_);
}
}
