/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 */
#include "../../dtk_test_ext.h"
#include "text/font.h"
#include "recording/mem_allocator.h"
#include "text/font_mgr.h"
#include "text/font_style_set.h"
#include "text/rs_xform.h"
#include "utils/point.h"
#include "utils/rect.h"
#include "text/typeface.h"

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
