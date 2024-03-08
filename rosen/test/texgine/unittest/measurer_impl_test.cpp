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
#include <hb.h>

#include "measurer_impl.h"
#include "param_test_macros.h"
#include "texgine_exception.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen;

struct hb_buffer_t {};
struct hb_unicode_funcs_t {};
struct hb_font_t {};
struct hb_face_t {};

struct MockVars {
    size_t calledHBFontCreate = 0;
    std::unique_ptr<hb_buffer_t> retvalBufferCreate = std::make_unique<hb_buffer_t>();
    std::unique_ptr<hb_unicode_funcs_t> retvalUnicodeFuncsCreate = std::make_unique<hb_unicode_funcs_t>();
    size_t calledUnicodeScript = 0;
    std::vector<hb_script_t> retvalUnicodeScript = {HB_SCRIPT_INVALID};
    std::unique_ptr<hb_font_t> retvalFontCreate = std::make_unique<hb_font_t>();
    std::unique_ptr<hb_face_t> retvalFaceCreate = std::make_unique<hb_face_t>();
    std::vector<hb_glyph_info_t> retvalGetGlyphInfo = {{}};
    std::vector<hb_glyph_position_t> retvalGetGlyphPosition = {{}};
    int retvalGetUnitsPerEm = 1;
    bool retvalHBFeatureFromString = true;
    bool retvalIsWhitespace = false;
    bool retvalTypefaceHas = true;
    size_t retvalGetTableSize = 1;
    size_t retvalGetTableData = 1;

    std::shared_ptr<TextEngine::Typeface> typeface =
        std::make_shared<TextEngine::Typeface>(std::make_shared<TextEngine::TexgineTypeface>());
} g_measurerMockvars;

std::list<TextEngine::MeasuringRun> mRuns = {};

hb_blob_t *hb_blob_create(const char *, unsigned int, hb_memory_mode_t, void *, hb_destroy_func_t)
{
    return hb_blob_get_empty();
}

hb_unicode_funcs_t *hb_unicode_funcs_create(hb_unicode_funcs_t *)
{
    return g_measurerMockvars.retvalUnicodeFuncsCreate.get();
}

hb_script_t hb_unicode_script(hb_unicode_funcs_t *, hb_codepoint_t)
{
    assert(g_measurerMockvars.retvalUnicodeScript.size() > g_measurerMockvars.calledUnicodeScript);
    return g_measurerMockvars.retvalUnicodeScript[g_measurerMockvars.calledUnicodeScript++];
}

hb_bool_t hb_feature_from_string(const char *, int, hb_feature_t *)
{
    return g_measurerMockvars.retvalHBFeatureFromString;
}

hb_buffer_t *hb_buffer_create()
{
    return g_measurerMockvars.retvalBufferCreate.get();
}

void hb_buffer_destroy(hb_buffer_t *)
{
}

void hb_font_destroy(hb_font_t *)
{
}

void hb_face_destroy(hb_face_t *)
{
}

void hb_buffer_add_utf16(hb_buffer_t *, const uint16_t *, int32_t, uint32_t, int32_t)
{
}

void hb_buffer_set_direction(hb_buffer_t *, hb_direction_t)
{
}

void hb_buffer_set_unicode_funcs(hb_buffer_t *, hb_unicode_funcs_t*)
{
}

hb_unicode_funcs_t *hb_icu_get_unicode_funcs()
{
    return nullptr;
}

void hb_buffer_set_script(hb_buffer_t *, hb_script_t)
{
}

void hb_buffer_set_language(hb_buffer_t *, hb_language_t)
{
}

hb_face_t *hb_face_create_for_tables(hb_reference_table_func_t, void *, hb_destroy_func_t)
{
    return g_measurerMockvars.retvalFaceCreate.get();
}

hb_font_t *hb_font_create(hb_face_t *face)
{
    g_measurerMockvars.calledHBFontCreate++;
    return g_measurerMockvars.retvalFontCreate.get();
}

void hb_shape(hb_font_t *, hb_buffer_t *, const hb_feature_t *, unsigned int)
{
}

hb_glyph_info_t *hb_buffer_get_glyph_infos(hb_buffer_t *buffer, unsigned int *length)
{
    *length = g_measurerMockvars.retvalGetGlyphInfo.size();
    return g_measurerMockvars.retvalGetGlyphInfo.data();
}

hb_glyph_position_t *hb_buffer_get_glyph_positions(hb_buffer_t *buffer, unsigned int *length)
{
    return g_measurerMockvars.retvalGetGlyphPosition.data();
}

U_CAPI UBool U_EXPORT2 u_isWhitespace(UChar32 c)
{
    return g_measurerMockvars.retvalIsWhitespace;
}

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#ifndef USE_ROSEN_DRAWING
size_t TexgineTypeface::GetTableSize(uint32_t tag) const
{
    return g_measurerMockvars.retvalGetTableSize;
}

size_t TexgineTypeface::GetTableData(uint32_t tag, size_t offset, size_t length, void *data) const
{
    return g_measurerMockvars.retvalGetTableData;
}

int TexgineTypeface::GetUnitsPerEm() const
{
    return g_measurerMockvars.retvalGetUnitsPerEm;
}
#endif

std::vector<Boundary> WordBreaker::GetBoundary(const std::vector<uint16_t> &u16str, bool)
{
    return {};
}

bool Typeface::Has(uint32_t ch)
{
    return g_measurerMockvars.retvalTypefaceHas;
}

std::shared_ptr<Typeface> FontCollection::GetTypefaceForChar(const uint32_t &ch,
    FontStyles &style, const std::string &script, const std::string &locale) const
{
    return g_measurerMockvars.typeface;
}

void InitMiMockVars(MockVars vars, std::list<struct MeasuringRun> runs)
{
    g_measurerMockvars = std::move(vars);
    mRuns.clear();
    mRuns.insert(mRuns.begin(), runs.begin(), runs.end());
}

class MeasurerImplTest : public testing::Test {
public:
    MeasurerImplTest()
    {
        normalff_.SetFeature("ftag", 1);
    }

    std::vector<uint16_t> text_ = {0};
    FontCollection fontCollection_ = std::vector<std::shared_ptr<VariantFontStyleSet>>{};
    FontFeatures emptyff_;
    FontFeatures normalff_;
    CharGroups charGroups_;
};

/**
 * @tc.name: HbFaceReferenceTableTypeface
 * @tc.desc: Verify the HbFaceReferenceTableTypeface
 * @tc.type:FUNC
 */
HWTEST_F(MeasurerImplTest, HbFaceReferenceTableTypeface, TestSize.Level1)
{
    EXPECT_EQ(HbFaceReferenceTableTypeface({}, {}, nullptr), nullptr);
}

/**
 * @tc.name: Create
 * @tc.desc: Verify the Create
 * @tc.type:FUNC
 */
HWTEST_F(MeasurerImplTest, Create, TestSize.Level1)
{
    EXPECT_NE(MeasurerImpl::Create({}, fontCollection_), nullptr);
}

/**
 * @tc.name: Measure1
 * @tc.desc: Verify the Measure
 * @tc.type:FUNC
 */
HWTEST_F(MeasurerImplTest, Measure1, TestSize.Level1)
{
    size_t ret = 0;
    InitMiMockVars({.retvalBufferCreate = nullptr}, {});
    uint16_t testText = 2;
    text_ = {testText};
    MeasurerImpl mi(text_, fontCollection_);
    int begin = 0;
    int end = 1;
    mi.SetRange(begin, end);
    mi.SetFontFeatures(emptyff_);

    EXPECT_EQ(mi.Measure(charGroups_), 1);
    EXPECT_EQ(g_measurerMockvars.calledHBFontCreate, ret);
}

/**
 * @tc.name: Measure2
 * @tc.desc: Verify the Measure
 * @tc.type:FUNC
 */
HWTEST_F(MeasurerImplTest, Measure2, TestSize.Level1)
{
    size_t ret = 1;
    InitMiMockVars({}, {});
    uint16_t testText = 2;
    text_ = {testText};
    MeasurerImpl mi(text_, fontCollection_);
    int begin = 0;
    int end = 1;
    mi.SetRange(begin, end);
    mi.SetFontFeatures(emptyff_);

    EXPECT_EQ(mi.Measure(charGroups_), 0);
    EXPECT_EQ(g_measurerMockvars.calledHBFontCreate, ret);
    ret++;
    EXPECT_EQ(mi.Measure(charGroups_), 0);
    EXPECT_EQ(g_measurerMockvars.calledHBFontCreate, ret);
}

/**
 * @tc.name: Measure3
 * @tc.desc: Verify the Measure
 * @tc.type:FUNC
 */
HWTEST_F(MeasurerImplTest, Measure3, TestSize.Level1)
{
    size_t ret = 1;
    InitMiMockVars({}, {});
    text_ = {3};
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetRange(0, 1);
    mi.SetFontFeatures(normalff_);

    EXPECT_EQ(mi.Measure(charGroups_), 0);
    EXPECT_EQ(g_measurerMockvars.calledHBFontCreate, ret);
}

/**
 * @tc.name: SeekTypeface1
 * @tc.desc: Verify the SeekTypeface
 * @tc.type:FUNC
 */
HWTEST_F(MeasurerImplTest, SeekTypeface1, TestSize.Level1)
{
    InitMiMockVars({.typeface = nullptr}, {{.start = 0, .end = 1}});
    text_ = {'a'};
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetRange(0, 1);

    std::list<struct MeasuringRun> runs;
    mi.SeekTypeface(mRuns);
    EXPECT_EQ(mRuns.size(), 1);
    EXPECT_EQ(mRuns.front().typeface, nullptr);
}

/**
 * @tc.name: SeekTypeface2
 * @tc.desc: Verify the SeekTypeface
 * @tc.type:FUNC
 */
HWTEST_F(MeasurerImplTest, SeekTypeface2, TestSize.Level1)
{
    InitMiMockVars({}, {{.start = 0, .end = 2}});
    text_ = {'a', 'b'};
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetRange(0, 2);

    mi.SeekTypeface(mRuns);
    EXPECT_EQ(mRuns.size(), 1);
    EXPECT_EQ(mRuns.front().typeface, g_measurerMockvars.typeface);
}

/**
 * @tc.name: SeekTypeface3
 * @tc.desc: Verify the SeekTypeface
 * @tc.type:FUNC
 */
HWTEST_F(MeasurerImplTest, SeekTypeface3, TestSize.Level1)
{
    InitMiMockVars({.retvalTypefaceHas = false}, {{.start = 0, .end = 2}});
    text_ = {'a', 'b'};
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetRange(0, 2);
    mi.SeekTypeface(mRuns);
    EXPECT_EQ(mRuns.size(), 2);
    EXPECT_EQ(mRuns.front().typeface, g_measurerMockvars.typeface);
}

/**
 * @tc.name: SeekTypeface4
 * @tc.desc: Verify the SeekTypeface
 * @tc.type:FUNC
 */
HWTEST_F(MeasurerImplTest, SeekTypeface4, TestSize.Level1)
{
    InitMiMockVars({.retvalTypefaceHas = false}, {{.start = 0, .end = 2}});
    text_ = {'a'};
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetRange(0, 2);
    mi.SeekTypeface(mRuns);
    EXPECT_EQ(mRuns.size(), 1);
    EXPECT_EQ(mRuns.front().typeface, nullptr);
}

/**
 * @tc.name: SeekScript1
 * @tc.desc: Verify the SeekScript
 * @tc.type:FUNC
 */
HWTEST_F(MeasurerImplTest, SeekScript1, TestSize.Level1)
{
    InitMiMockVars({.retvalUnicodeFuncsCreate = nullptr}, {});
    MeasurerImpl mi(text_, fontCollection_);
    ASSERT_EXCEPTION(ExceptionType::API_FAILED, mi.SeekScript(mRuns));
}

/**
 * @tc.name: SeekScript2
 * @tc.desc: Verify the SeekScript
 * @tc.type:FUNC
 */
HWTEST_F(MeasurerImplTest, SeekScript2, TestSize.Level1)
{
    InitMiMockVars({.retvalUnicodeScript = {HB_SCRIPT_HAN}}, {{.start = 0, .end = 1}});
    MeasurerImpl mi(text_, fontCollection_);

    ASSERT_EXCEPTION(ExceptionType::ERROR_STATUS, mi.SeekScript(mRuns));
}

/**
 * @tc.name: SeekScript3
 * @tc.desc: Verify the SeekScript
 * @tc.type:FUNC
 */
HWTEST_F(MeasurerImplTest, SeekScript3, TestSize.Level1)
{
    InitMiMockVars({.retvalUnicodeScript = {HB_SCRIPT_HAN}}, {});
    text_ = {'a'};
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetRange(0, 1);

    EXPECT_NE(mRuns.front().script, HB_SCRIPT_HAN);
    mi.SeekScript(mRuns);
    EXPECT_EQ(mRuns.size(), 1);
    EXPECT_EQ(mRuns.front().script, HB_SCRIPT_HAN);
}

/**
 * @tc.name: SeekScript4
 * @tc.desc: Verify the SeekScript
 * @tc.type:FUNC
 */
HWTEST_F(MeasurerImplTest, SeekScript4, TestSize.Level1)
{
    InitMiMockVars({.retvalUnicodeScript = {HB_SCRIPT_HAN, HB_SCRIPT_INHERITED}}, {});
    text_ = {'a', 'b'};
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetRange(0, 2);

    mi.SeekScript(mRuns);
    EXPECT_EQ(mRuns.size(), 1);
    EXPECT_EQ(mRuns.front().script, HB_SCRIPT_HAN);

    InitMiMockVars({.retvalUnicodeScript = {HB_SCRIPT_HAN, HB_SCRIPT_COMMON}}, {});
    mi.SeekScript(mRuns);
    EXPECT_EQ(mRuns.size(), 1);
    EXPECT_EQ(mRuns.front().script, HB_SCRIPT_HAN);
}

/**
 * @tc.name: Shape1
 * @tc.desc: Verify the Shape
 * @tc.type:FUNC
 */
HWTEST_F(MeasurerImplTest, Shape1, TestSize.Level1)
{
    size_t ret = 0;
    InitMiMockVars({}, {});
    MeasurerImpl mi(text_, fontCollection_);

    std::list<struct MeasuringRun> runs;
    EXPECT_EQ(mi.Shape(charGroups_, runs, {}), 0);
    EXPECT_EQ(g_measurerMockvars.calledHBFontCreate, ret);
}

/**
 * @tc.name: Shape2
 * @tc.desc: Verify the Shape
 * @tc.type:FUNC
 */
HWTEST_F(MeasurerImplTest, Shape2, TestSize.Level1)
{
    InitMiMockVars({}, {});
    MeasurerImpl mi(text_, fontCollection_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 0});
    ASSERT_EXCEPTION(ExceptionType::ERROR_STATUS, mi.Shape(charGroups_, runs, {}));
}

/**
 * @tc.name: Shape3
 * @tc.desc: Verify the Shape
 * @tc.type:FUNC
 */
HWTEST_F(MeasurerImplTest, Shape3, TestSize.Level1)
{
    size_t ret = 0;
    InitMiMockVars({}, {});
    MeasurerImpl mi(text_, fontCollection_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 1, .typeface = nullptr});
    EXPECT_EQ(mi.Shape(charGroups_, runs, {}), 1);
    EXPECT_EQ(g_measurerMockvars.calledHBFontCreate, ret);
}

/**
 * @tc.name: Shape4
 * @tc.desc: Verify the Shape
 * @tc.type:FUNC
 */
HWTEST_F(MeasurerImplTest, Shape4, TestSize.Level1)
{
    size_t ret = 0;
    InitMiMockVars({.retvalBufferCreate = nullptr}, {});
    MeasurerImpl mi(text_, fontCollection_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 1, .typeface = g_measurerMockvars.typeface});
    EXPECT_EQ(mi.Shape(charGroups_, runs, {}), 1);
    EXPECT_EQ(g_measurerMockvars.calledHBFontCreate, ret);
}

/**
 * @tc.name: Shape5
 * @tc.desc: Verify the Shape
 * @tc.type:FUNC
 */
HWTEST_F(MeasurerImplTest, Shape5, TestSize.Level1)
{
    size_t ret = 0;
    InitMiMockVars({.retvalFaceCreate = nullptr}, {});
    MeasurerImpl mi(text_, fontCollection_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 1, .typeface = g_measurerMockvars.typeface});
    EXPECT_EQ(mi.Shape(charGroups_, runs, {}), 1);
    EXPECT_EQ(g_measurerMockvars.calledHBFontCreate, ret);
}

/**
 * @tc.name: Shape6
 * @tc.desc: Verify the Shape
 * @tc.type:FUNC
 */
HWTEST_F(MeasurerImplTest, Shape6, TestSize.Level1)
{
    size_t ret = 1;
    InitMiMockVars({.retvalFontCreate = nullptr}, {});
    MeasurerImpl mi(text_, fontCollection_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 1, .typeface = g_measurerMockvars.typeface});
    EXPECT_EQ(mi.Shape(charGroups_, runs, {}), 1);
    EXPECT_EQ(g_measurerMockvars.calledHBFontCreate, ret);
}

/**
 * @tc.name: Shape8
 * @tc.desc: Verify the Shape
 * @tc.type:FUNC
 */
HWTEST_F(MeasurerImplTest, Shape8, TestSize.Level1)
{
    size_t ret = 1;
    InitMiMockVars({.retvalGetGlyphPosition = {}}, {});
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetFontFeatures(normalff_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 1, .typeface = g_measurerMockvars.typeface});
    EXPECT_EQ(mi.Shape(charGroups_, runs, {}), 1);
    EXPECT_EQ(g_measurerMockvars.calledHBFontCreate, ret);
}

/**
 * @tc.name: Shape9
 * @tc.desc: Verify the Shape
 * @tc.type:FUNC
 */
HWTEST_F(MeasurerImplTest, Shape9, TestSize.Level1)
{
    size_t ret = 1;
    InitMiMockVars({.retvalGetUnitsPerEm = 0}, {});
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetFontFeatures(normalff_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 1, .typeface = g_measurerMockvars.typeface});
    EXPECT_EQ(mi.Shape(charGroups_, runs, {}), 0);
    EXPECT_EQ(g_measurerMockvars.calledHBFontCreate, ret);
}

/**
 * @tc.name: Shape10
 * @tc.desc: Verify the Shape
 * @tc.type:FUNC
 */
HWTEST_F(MeasurerImplTest, Shape10, TestSize.Level1)
{
    InitMiMockVars({.retvalIsWhitespace = true}, {});
    g_measurerMockvars.retvalGetGlyphPosition[0].x_advance = 10;
    text_ = {'a'};
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetFontFeatures(normalff_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 1, .typeface = g_measurerMockvars.typeface});
    EXPECT_EQ(mi.Shape(charGroups_, runs, {}), 0);
    EXPECT_EQ(charGroups_.GetSize(), 1);
    EXPECT_GT(charGroups_.Get(0).invisibleWidth, charGroups_.Get(0).visibleWidth);
}

/**
 * @tc.name: Shape11
 * @tc.desc: Verify the Shape
 * @tc.type:FUNC
 */
HWTEST_F(MeasurerImplTest, Shape11, TestSize.Level1)
{
    InitMiMockVars({}, {});
    text_ = {'a', 'b'};
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetFontFeatures(normalff_);
    mi.SetRTL(true);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 1, .typeface = g_measurerMockvars.typeface});
    runs.push_back({.start = 1, .end = 2, .typeface = g_measurerMockvars.typeface});
    EXPECT_EQ(mi.Shape(charGroups_, runs, {}), 0);
    EXPECT_EQ(charGroups_.GetSize(), 2);
}

/**
 * @tc.name: Shape12
 * @tc.desc: Verify the Shape
 * @tc.type:FUNC
 */
HWTEST_F(MeasurerImplTest, Shape12, TestSize.Level1)
{
    InitMiMockVars({.retvalGetGlyphInfo = {{.cluster = 0}, {.cluster = 1}},
                  .retvalGetGlyphPosition = {{}, {}}}, {});
    text_ = {'a', 'b'};
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetFontFeatures(normalff_);
    mi.SetSpacing(5, 10);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 2, .typeface = g_measurerMockvars.typeface});
    std::vector<Boundary> boundaries;
    boundaries.emplace_back(0, 2);
    EXPECT_EQ(mi.Shape(charGroups_, runs, boundaries), 0);
    EXPECT_EQ(charGroups_.GetSize(), 2);

    // 5: invisibleWidth, spacing
    EXPECT_EQ(charGroups_.Get(0).invisibleWidth, 5);
    EXPECT_EQ(charGroups_.Get(1).invisibleWidth, 5);
}

/**
 * @tc.name: GenerateHBFeatures1
 * @tc.desc: Verify the GenerateHBFeatures
 * @tc.type:FUNC
 */
HWTEST_F(MeasurerImplTest, GenerateHBFeatures1, TestSize.Level1)
{
    InitMiMockVars({}, {});
    MeasurerImpl mi(text_, fontCollection_);
    std::vector<hb_feature_t> features;
    mi.GenerateHBFeatures(features, nullptr);
    EXPECT_EQ(features.size(), 0);
}

/**
 * @tc.name: GenerateHBFeatures2
 * @tc.desc: Verify the GenerateHBFeatures
 * @tc.type:FUNC
 */
HWTEST_F(MeasurerImplTest, GenerateHBFeatures2, TestSize.Level1)
{
    InitMiMockVars({}, {});
    MeasurerImpl mi(text_, fontCollection_);
    std::vector<hb_feature_t> features;
    mi.GenerateHBFeatures(features, &emptyff_);
    EXPECT_EQ(features.size(), 0);
}

/**
 * @tc.name: GenerateHBFeatures3
 * @tc.desc: Verify the GenerateHBFeatures
 * @tc.type:FUNC
 */
HWTEST_F(MeasurerImplTest, GenerateHBFeatures3, TestSize.Level1)
{
    InitMiMockVars({.retvalHBFeatureFromString = false}, {});
    MeasurerImpl mi(text_, fontCollection_);
    std::vector<hb_feature_t> features;
    mi.GenerateHBFeatures(features, &normalff_);
    EXPECT_EQ(features.size(), 0);
    EXPECT_EQ(normalff_.GetFeatures().size(), 1);
}

/**
 * @tc.name: GenerateHBFeatures4
 * @tc.desc: Verify the GenerateHBFeatures
 * @tc.type:FUNC
 */
HWTEST_F(MeasurerImplTest, GenerateHBFeatures4, TestSize.Level1)
{
    InitMiMockVars({}, {});
    MeasurerImpl mi(text_, fontCollection_);
    std::vector<hb_feature_t> features;
    mi.GenerateHBFeatures(features, &normalff_);
    EXPECT_EQ(features.size(), 1);
    EXPECT_EQ(normalff_.GetFeatures().size(), 1);
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
