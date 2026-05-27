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

#ifndef TEXT_TYPOGRAPHY_TEST_COMMON_H
#define TEXT_TYPOGRAPHY_TEST_COMMON_H

#include <fstream>
#include <limits>
#include <string>

#include "drawing_bitmap.h"
#include "drawing_brush.h"
#include "drawing_canvas.h"
#include "drawing_color.h"
#include "drawing_error_code.h"
#include "drawing_font.h"
#include "drawing_font_collection.h"
#include "drawing_path.h"
#include "drawing_pen.h"
#include "drawing_point.h"
#include "drawing_rect.h"
#include "drawing_text_declaration.h"
#include "drawing_text_line.h"
#include "drawing_text_run.h"
#include "drawing_text_typography.h"
#include "font_utils.h"
#include "gtest/gtest.h"
#include "impl/paragraph_impl.h"
#include "modules/skparagraph/include/TextStyle.h"
#include "modules/skparagraph/src/ParagraphImpl.h"
#include "rosen_text/typography.h"
#include "rosen_text/typography_create.h"
#include "text_style.h"
#include "txt/text_bundle_config_parser.h"
#include "typography_types.h"

namespace OHOS {
using namespace OHOS::Rosen;
using namespace testing;
using namespace testing::ext;
constexpr double LEFT_POS = 50.0;
constexpr double RIGHT_POS = 150.0;
constexpr double ARC_FONT_SIZE = 30;
constexpr double DEFAULT_FONT_SIZE = 50;
constexpr const char* DEFAULT_TEXT = "text";
constexpr const char* DEFAULT_LONG_TEXT =
    "中文你好世界。 English Hello World.中文你好世界。 English Hello World.中文你好世界。 English Hello "
    "World.中文你好世界。 English Hello World.";
constexpr double MAX_WIDTH = 800.0;
constexpr double SWEEP_DEGREE = 180.0;
constexpr float FLOAT_DATA_EPSILON = 1e-6f;
constexpr const char* VIS_LIST_FILE_NAME = "/system/fonts/visibility_list.json";

class NdkTypographyTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;
    void CreateTypographyHandler();
    void CreateTypography();
    void AddText();
    void Layout();
    void Paint();

protected:
    OH_Drawing_TypographyCreate* fHandler { nullptr };
    OH_Drawing_Typography* fTypography { nullptr };
    OH_Drawing_TypographyStyle* fTypoStyle { nullptr };
    OH_Drawing_Canvas* fCanvas { nullptr };
    OH_Drawing_Bitmap* fBitmap { nullptr };
    int fLayoutWidth { 50 };
};

TypographyStyle* ConvertToOriginalText(OH_Drawing_TypographyStyle* style);
TextStyle* ConvertToOriginalText(OH_Drawing_TextStyle* style);
} // namespace OHOS

#endif // TEXT_TYPOGRAPHY_TEST_COMMON_H
