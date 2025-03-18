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
#include "rosen/test/dtk/dtk_test_ext.h"
#include "rosen/modules/2d_graphics/include/text/font.h"
#include "rosen/modules/2d_graphics/include/text/font_mgr.h"
#include "rosen/modules/2d_graphics/include/text/font_style_set.h"
#include "rosen/modules/2d_graphics/include/text/rs_xform.h"
#include "rosen/modules/2d_graphics/include/text/typeface.h"
#include "rosen/modules/2d_graphics/include/utils/point.h"
#include "rosen/modules/2d_graphics/include/utils/rect.h"
#include "rosen/modules/2d_graphics/include/recording/mem_allocator.h"

namespace OHOS {
namespace Rosen {

//演示用例 FontStyleSet_CreateTypeface_3001
DEF_DTK(fontstyleset_createtypeface, TestLevel::L1, 1)
{
    //1、 调用CreateDefaultFontMgr 函数创建FontMgr实例
    //2、调用FontMgr的CreateStyleSet 创建FontStyleSet实例
    //3、通过FontStyleSet 创建字体测试用的typeface
    //4、通过绘制DrawTextBlob 并设置Font的typeface绘制出文字
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(font_mgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(1));

    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);
    std::string text1 = "DDGR ddgr 鸿蒙 !@#%￥^&*;：，。";
    std::string text2 = "-_=+()123`.---~|{}【】,./?、？<>《》";
    std::string text3 = "\xE2\x99\x88\xE2\x99\x8A\xE2\x99\x88\xE2\x99\x8C\xE2\x99\x8D\xE2\x99\x8D";
    std::string texts[] = {text1, text2, text3};
    int line = 200;
    for (auto text :texts) {
        std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromText(text.c_str(), text.size(), font);
        Drawing::Brush brush;
        playbackCanvas_->AttachBrush(brush);
        playbackCanvas_->DrawTextBlob(textBlob.get(), 200, line);
        line += 200;
        playbackCanvas_->DetachBrush();
        Drawing::Pen pen;
        playbackCanvas_->AttachPen(pen);
        playbackCanvas_->DrawTextBlob(textBlob.get(), 200, line);
        line += 200;
        playbackCanvas_->DetachPen();
    }
}

}
}
