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

struct hb_buffer_t {};
struct hb_unicode_funcs_t {};
struct hb_font_t {};
struct hb_face_t {};

struct MockVars {
    size_t calledHBFontCreate_ = 0;
    std::unique_ptr<hb_buffer_t> retvalBufferCreate_ = std::make_unique<hb_buffer_t>();
    std::unique_ptr<hb_unicode_funcs_t> retvalUnicodeFuncsCreate_ = std::make_unique<hb_unicode_funcs_t>();
    size_t calledUnicodeScript_ = 0;
    std::vector<hb_script_t> retvalUnicodeScript_ = {HB_SCRIPT_INVALID};
    std::unique_ptr<hb_font_t> retvalFontCreate_ = std::make_unique<hb_font_t>();
    std::unique_ptr<hb_face_t> retvalFaceCreate_ = std::make_unique<hb_face_t>();
    std::vector<hb_glyph_info_t> retvalGetGlyphInfo_ = {{}};
    std::vector<hb_glyph_position_t> retvalGetGlyphPosition_ = {{}};
    int retvalGetUnitsPerEm_ = 1;
    bool retvalHBFeatureFromString_ = true;
    bool retvalIsWhitespace = false;
    bool retvalTypefaceHas_ = true;
    size_t retvalGetTableSize_ = 1;
    size_t retvalGetTableData_ = 1;

    std::shared_ptr<Texgine::Typeface> typeface_ =
        std::make_shared<Texgine::Typeface>(std::make_shared<Texgine::TexgineTypeface>());
} measurerMockvars;

std::list<Texgine::MeasuringRun> mRuns = {};

hb_blob_t *hb_blob_create(const char *, unsigned int, hb_memory_mode_t, void *, hb_destroy_func_t)
{
    return hb_blob_get_empty();
}

hb_unicode_funcs_t *hb_unicode_funcs_create(hb_unicode_funcs_t *)
{
    return measurerMockvars.retvalUnicodeFuncsCreate_.get();
}

hb_script_t hb_unicode_script(hb_unicode_funcs_t *, hb_codepoint_t)
{
    assert(measurerMockvars.retvalUnicodeScript_.size() > measurerMockvars.calledUnicodeScript_);
    return measurerMockvars.retvalUnicodeScript_[measurerMockvars.calledUnicodeScript_++];
}

hb_bool_t hb_feature_from_string(const char *, int, hb_feature_t *)
{
    return measurerMockvars.retvalHBFeatureFromString_;
}

hb_buffer_t *hb_buffer_create()
{
    return measurerMockvars.retvalBufferCreate_.get();
}

void hb_buffer_destroy(hb_buffer_t *)
{
}

void hb_font_destroy (hb_font_t *)
{
}

void hb_face_destroy (hb_face_t *)
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
    return measurerMockvars.retvalFaceCreate_.get();
}

hb_font_t *hb_font_create(hb_face_t *face)
{
    measurerMockvars.calledHBFontCreate_++;
    return measurerMockvars.retvalFontCreate_.get();
}

void hb_shape(hb_font_t *, hb_buffer_t *, const hb_feature_t *, unsigned int)
{
}

hb_glyph_info_t *hb_buffer_get_glyph_infos(hb_buffer_t *buffer, unsigned int *length)
{
    *length = measurerMockvars.retvalGetGlyphInfo_.size();
    return measurerMockvars.retvalGetGlyphInfo_.data();
}

hb_glyph_position_t *hb_buffer_get_glyph_positions(hb_buffer_t *buffer, unsigned int *length)
{
    return measurerMockvars.retvalGetGlyphPosition_.data();
}

U_CAPI UBool U_EXPORT2 u_isWhitespace(UChar32 c)
{
    return measurerMockvars.retvalIsWhitespace;
}

namespace OHOS {
namespace Rosen {
namespace TextEngine {
size_t TexgineTypeface::GetTableSize(uint32_t tag)
{
    return measurerMockvars.retvalGetTableSize_;
}

size_t TexgineTypeface::GetTableData(uint32_t tag, size_t offset, size_t length, void *data)
{
    return measurerMockvars.retvalGetTableData_;
}

int TexgineTypeface::GetUnitsPerEm()
{
    return measurerMockvars.retvalGetUnitsPerEm_;
}

std::vector<Boundary> WordBreaker::GetBoundary(const std::vector<uint16_t> &u16str, bool)
{
    return {};
}

bool Typeface::Has(uint32_t ch)
{
    return measurerMockvars.retvalTypefaceHas_;
}

std::shared_ptr<Typeface> FontCollection::GetTypefaceForChar(const uint32_t &ch,
    const FontStyles &style, const std::string &script, const std::string &locale) const
{
    return measurerMockvars.typeface_;
}

void InitMiMockVars(MockVars vars, std::list<struct MeasuringRun> runs)
{
    measurerMockvars = std::move(vars);
    mRuns.clear();
    mRuns.insert(mRuns.begin(), runs.begin(), runs.end());
}

class MeasurerImplTest : public testing::Test {
public:
    MeasurerImplTest()
    {
        normalff_.SetFeature("ftag", 1);
    }

    std::vector<uint16_t> text_ = {};
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

    InitMiMockVars({.retvalGetTableSize_ = 0}, {});
    EXPECT_EQ(HbFaceReferenceTableTypeface({}, {}, this), nullptr);

    InitMiMockVars({.retvalGetTableSize_ = static_cast<size_t>(1000)}, {});
    EXPECT_EQ(HbFaceReferenceTableTypeface({}, {}, this), nullptr);

    InitMiMockVars({.retvalGetTableSize_ = 2, .retvalGetTableData_ = 1}, {});
    EXPECT_EQ(HbFaceReferenceTableTypeface({}, {}, this), nullptr);

    InitMiMockVars({}, {});
    EXPECT_NE(HbFaceReferenceTableTypeface({}, {}, this), nullptr);
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
    InitMiMockVars({.retvalBufferCreate_ = nullptr}, {});
    uint16_t testText = 2;
    text_ = {testText};
    MeasurerImpl mi(text_, fontCollection_);
    int begin = 0;
    int end = 1;
    mi.SetRange(begine, end);
    mi.SetFontFeatures(emptyff_);

    EXPECT_EQ(mi.Measure(charGroups_), 1);
    EXPECT_EQ(measurerMockvars.calledHBFontCreate_, 0);
}

/**
 * @tc.name: Measure2
 * @tc.desc: Verify the Measure
 * @tc.type:FUNC
 */
HWTEST_F(MeasurerImplTest, Measure2, TestSize.Level1)
{
    InitMiMockVars({}, {});
    uint16_t testText = 2;
    text_ = {testText};
    MeasurerImpl mi(text_, fontCollection_);
    int begin = 0;
    int end = 1;
    mi.SetRange(begine, end);
    mi.SetFontFeatures(emptyff_);

    EXPECT_EQ(mi.Measure(charGroups_), 0);
    EXPECT_EQ(measurerMockvars.calledHBFontCreate_, 1);
    EXPECT_EQ(mi.Measure(charGroups_), 0);
    EXPECT_EQ(measurerMockvars.calledHBFontCreate_, 1);
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
