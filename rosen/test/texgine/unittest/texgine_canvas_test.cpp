/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "texgine_canvas.h"

using namespace testing;
using namespace testing::ext;

struct MockVars {
    std::shared_ptr<SkCanvas> skCanvas_ = std::make_shared<SkCanvas>();
    std::shared_ptr<OHOS::Rosen::TextEngine::TexginePaint> texginePaint_ =
        std::make_shared<OHOS::Rosen::TextEngine::TexginePaint>();
    std::shared_ptr<OHOS::Rosen::TextEngine::TexgineRect> texgineRect_ =
        std::make_shared<OHOS::Rosen::TextEngine::TexgineRect>();
    std::shared_ptr<OHOS::Rosen::TextEngine::TexgineTextBlob> blob_ =
        std::make_shared<OHOS::Rosen::TextEngine::TexgineTextBlob>();
} g_tcMockvars;

void InitTcMockVars(struct MockVars &&vars)
{
    g_tcMockvars = std::move(vars);
}

void SkCanvas::drawLine(SkScalar x0, SkScalar y0, SkScalar x1, SkScalar y1, const SkPaint &paint)
{
}
void SkCanvas::drawRect(const SkRect &r, const SkPaint &paint)
{
}

void SkCanvas::drawTextBlob(const SkTextBlob *blob, SkScalar x, SkScalar y, const SkPaint &paint)
{
}

int SkCanvas::save()
{
    return 0;
}

void SkCanvas::translate(SkScalar dx, SkScalar dy)
{
}

void SkCanvas::restore()
{
}

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexgineCanvasTest : public testing::Test {
};

/**
 * @tc.name:DrawLine
 * @tc.desc: Verify the DrawLine
 * @tc.type:FUNC
 */
HWTEST_F(TexgineCanvasTest, DrawLine, TestSize.Level1)
{
    std::shared_ptr<TexgineCanvas> tc = std::make_shared<TexgineCanvas>();
    EXPECT_NO_THROW({
        InitTcMockVars({});
        tc->DrawLine(0.0, 0.0, 0.0, 0.0, *g_tcMockvars.texginePaint_);
        tc->SetCanvas(g_tcMockvars.skCanvas_.get());
        tc->DrawLine(0.0, 0.0, 0.0, 0.0, *g_tcMockvars.texginePaint_);
    });
}

/**
 * @tc.name:DrawLine
 * @tc.desc: Verify the DrawLine001
 * @tc.type:FUNC
 */
HWTEST_F(TexgineCanvasTest, DrawLine001, TestSize.Level1)
{
    std::shared_ptr<TexgineCanvas> tc = std::make_shared<TexgineCanvas>();
    EXPECT_NO_THROW({
        InitTcMockVars({});
        tc->SetCanvas(g_tcMockvars.skCanvas_.get());
        tc->DrawLine(0.0, 0.0, 0.0, 0.0, *g_tcMockvars.texginePaint_);
        g_tcMockvars.skCanvas_ = nullptr;
        tc->SetCanvas(g_tcMockvars.skCanvas_.get());
        tc->DrawLine(0.0, 0.0, 0.0, 0.0, *g_tcMockvars.texginePaint_);
    });
}

/**
 * @tc.name:DrawRect
 * @tc.desc: Verify the DrawRect
 * @tc.type:FUNC
 */
HWTEST_F(TexgineCanvasTest, DrawRect, TestSize.Level1)
{
    std::shared_ptr<TexgineCanvas> tc = std::make_shared<TexgineCanvas>();
    EXPECT_NO_THROW({
        InitTcMockVars({});
        tc->DrawRect(*g_tcMockvars.texgineRect_, *g_tcMockvars.texginePaint_);
        tc->SetCanvas(g_tcMockvars.skCanvas_.get());
        tc->DrawRect(*g_tcMockvars.texgineRect_, *g_tcMockvars.texginePaint_);
        g_tcMockvars.texgineRect_->SetRect(SkRect::MakeEmpty());
        tc->DrawRect(*g_tcMockvars.texgineRect_, *g_tcMockvars.texginePaint_);
    });
}

/**
 * @tc.name:DrawRect
 * @tc.desc: Verify the DrawRect001
 * @tc.type:FUNC
 */
HWTEST_F(TexgineCanvasTest, DrawRect001, TestSize.Level1)
{
    std::shared_ptr<TexgineCanvas> tc = std::make_shared<TexgineCanvas>();
    EXPECT_NO_THROW({
        InitTcMockVars({});
        tc->SetCanvas(g_tcMockvars.skCanvas_.get());
        tc->DrawRect(*g_tcMockvars.texgineRect_, *g_tcMockvars.texginePaint_);
        g_tcMockvars.skCanvas_ = nullptr;
        tc->SetCanvas(g_tcMockvars.skCanvas_.get());
        tc->DrawRect(*g_tcMockvars.texgineRect_, *g_tcMockvars.texginePaint_);
        g_tcMockvars.texgineRect_->SetRect(SkRect::MakeEmpty());
        tc->DrawRect(*g_tcMockvars.texgineRect_, *g_tcMockvars.texginePaint_);
    });
}

/**
 * @tc.name:DrawTextBlob
 * @tc.desc: Verify the DrawTextBlob
 * @tc.type:FUNC
 */
HWTEST_F(TexgineCanvasTest, DrawTextBlob, TestSize.Level1)
{
    std::shared_ptr<TexgineCanvas> tc = std::make_shared<TexgineCanvas>();
    EXPECT_NO_THROW({
        InitTcMockVars({});
        tc->DrawTextBlob(g_tcMockvars.blob_, 0.0, 0.0, *g_tcMockvars.texginePaint_);
        tc->SetCanvas(g_tcMockvars.skCanvas_.get());
        std::shared_ptr<TexgineTextBlob> blob = nullptr;
        tc->DrawTextBlob(blob, 0.0, 0.0, *g_tcMockvars.texginePaint_);
        tc->DrawTextBlob(g_tcMockvars.blob_, 0.0, 0.0, *g_tcMockvars.texginePaint_);
    });
}

/**
 * @tc.name:DrawTextBlob
 * @tc.desc: Verify the DrawTextBlob001
 * @tc.type:FUNC
 */
HWTEST_F(TexgineCanvasTest, DrawTextBlob001, TestSize.Level1)
{
    std::shared_ptr<TexgineCanvas> tc = std::make_shared<TexgineCanvas>();
    EXPECT_NO_THROW({
        InitTcMockVars({});
        tc->SetCanvas(g_tcMockvars.skCanvas_.get());
        tc->DrawTextBlob(g_tcMockvars.blob_, 0.0, 0.0, *g_tcMockvars.texginePaint_);
        g_tcMockvars.skCanvas_ = nullptr;
        tc->SetCanvas(g_tcMockvars.skCanvas_.get());
        g_tcMockvars.blob_ = nullptr;
        tc->DrawTextBlob(g_tcMockvars.blob_, 0.0, 0.0, *g_tcMockvars.texginePaint_);
    });
}

/**
 * @tc.name:Clear
 * @tc.desc: Verify the Clear
 * @tc.type:FUNC
 */
HWTEST_F(TexgineCanvasTest, Clear, TestSize.Level1)
{
    std::shared_ptr<TexgineCanvas> tc = std::make_shared<TexgineCanvas>();
    EXPECT_NO_THROW({
        InitTcMockVars({});
        tc->Clear(0);
        tc->SetCanvas(g_tcMockvars.skCanvas_.get());
        tc->Clear(0);
    });
}

/**
 * @tc.name:Clear
 * @tc.desc: Verify the Clear001
 * @tc.type:FUNC
 */
HWTEST_F(TexgineCanvasTest, Clear001, TestSize.Level1)
{
    std::shared_ptr<TexgineCanvas> tc = std::make_shared<TexgineCanvas>();
    EXPECT_NO_THROW({
        InitTcMockVars({});
        tc->SetCanvas(g_tcMockvars.skCanvas_.get());
        tc->Clear(0);
        g_tcMockvars.skCanvas_ = nullptr;
        tc->SetCanvas(g_tcMockvars.skCanvas_.get());
        tc->Clear(0);
    });
}

/**
 * @tc.name:Save
 * @tc.desc: Verify the Save
 * @tc.type:FUNC
 */
HWTEST_F(TexgineCanvasTest, Save, TestSize.Level1)
{
    std::shared_ptr<TexgineCanvas> tc = std::make_shared<TexgineCanvas>();
    EXPECT_NO_THROW({
        InitTcMockVars({});
        EXPECT_EQ(tc->Save(), 1);
        tc->SetCanvas(g_tcMockvars.skCanvas_.get());
        EXPECT_EQ(tc->Save(), 0);
    });
}

/**
 * @tc.name:Save
 * @tc.desc: Verify the Save001
 * @tc.type:FUNC
 */
HWTEST_F(TexgineCanvasTest, Save001, TestSize.Level1)
{
    std::shared_ptr<TexgineCanvas> tc = std::make_shared<TexgineCanvas>();
    EXPECT_NO_THROW({
        InitTcMockVars({});
        tc->SetCanvas(g_tcMockvars.skCanvas_.get());
        EXPECT_EQ(tc->Save(), 1);
        g_tcMockvars.skCanvas_ = nullptr;
        tc->SetCanvas(g_tcMockvars.skCanvas_.get());
        EXPECT_EQ(tc->Save(), 0);
    });
}

/**
 * @tc.name:Translate
 * @tc.desc: Verify the Translate
 * @tc.type:FUNC
 */
HWTEST_F(TexgineCanvasTest, Translate, TestSize.Level1)
{
    std::shared_ptr<TexgineCanvas> tc = std::make_shared<TexgineCanvas>();
    EXPECT_NO_THROW({
        InitTcMockVars({});
        tc->Translate(0.0, 0.0);
        tc->SetCanvas(g_tcMockvars.skCanvas_.get());
        tc->Translate(0.0, 0.0);
    });
}

/**
 * @tc.name:Translate
 * @tc.desc: Verify the Translate001
 * @tc.type:FUNC
 */
HWTEST_F(TexgineCanvasTest, Translate001, TestSize.Level1)
{
    std::shared_ptr<TexgineCanvas> tc = std::make_shared<TexgineCanvas>();
    EXPECT_NO_THROW({
        InitTcMockVars({});
        tc->SetCanvas(g_tcMockvars.skCanvas_.get());
        tc->Translate(0.0, 0.0);
        g_tcMockvars.skCanvas_ = nullptr;
        tc->SetCanvas(g_tcMockvars.skCanvas_.get());
        tc->Translate(0.0, 0.0);
    });
}

/**
 * @tc.name:Restore
 * @tc.desc: Verify the Restore
 * @tc.type:FUNC
 */
HWTEST_F(TexgineCanvasTest, Restore, TestSize.Level1)
{
    std::shared_ptr<TexgineCanvas> tc = std::make_shared<TexgineCanvas>();
    EXPECT_NO_THROW({
        InitTcMockVars({});
        tc->Restore();
        tc->SetCanvas(g_tcMockvars.skCanvas_.get());
        tc->Restore();
    });
}

/**
 * @tc.name:Restore
 * @tc.desc: Verify the Restore001
 * @tc.type:FUNC
 */
HWTEST_F(TexgineCanvasTest, Restore001, TestSize.Level1)
{
    std::shared_ptr<TexgineCanvas> tc = std::make_shared<TexgineCanvas>();
    EXPECT_NO_THROW({
        InitTcMockVars({});
        tc->SetCanvas(g_tcMockvars.skCanvas_.get());
        tc->Restore();
        g_tcMockvars.skCanvas_ = nullptr;
        tc->SetCanvas(g_tcMockvars.skCanvas_.get());
        tc->Restore();
    });
}

/**
 * @tc.name:GetCanvas
 * @tc.desc: Verify the GetCanvas
 * @tc.type:FUNC
 */
HWTEST_F(TexgineCanvasTest, GetCanvas, TestSize.Level1)
{
    std::shared_ptr<TexgineCanvas> tc = std::make_shared<TexgineCanvas>();
    EXPECT_NO_THROW({
        InitTcMockVars({});
        tc->SetCanvas(g_tcMockvars.skCanvas_.get());
        EXPECT_EQ(tc->GetCanvas(), g_tcMockvars.skCanvas_.get());
    });
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
