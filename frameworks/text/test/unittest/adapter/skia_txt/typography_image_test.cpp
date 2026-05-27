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

#include "gtest/gtest.h"

#include <cstdint>
#include <filesystem>

#include "font_collection.h"
#include "image_packer.h"
#include "impl/paragraph_impl.h"
#include "modules/skparagraph/include/TextStyle.h"
#include "ohos/init_data.h"
#include "paragraph.h"
#include "src/ParagraphImpl.h"
#include "typography.h"
#include "typography_create.h"
#include "txt/text_bundle_config_parser.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
const double TEST_FONT_SIZE = 30;
const Drawing::Color TEST_FONT_COLOR = Drawing::Color::ColorQuadSetARGB(255, 200, 255, 221);
const float IMAGE_PADDING = 10;
const float IMAGE_NEGATIVE_VALUE = -10;
const std::string IMAGE_INPUT_PNG_PATH_TEST = "/data/local/tmp/image/";
const std::string OPTION_FORMAT_TEST = "image/png";
const std::int32_t OPTION_QUALITY_TEST = 100;
const std::int32_t OPTION_NUMBER_HINT_TEST = 1;
class OH_Drawing_TypographyImageTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;
protected:
    std::u16string text_ = u"text";
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate_;
    std::shared_ptr<OHOS::Rosen::Typography> typography_;
    OHOS::Rosen::TextStyle textStyle_;
};

void OH_Drawing_TypographyImageTest::SetUp()
{
    SetHwIcuDirectory();

    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    ASSERT_NE(typographyCreate_, nullptr);
    textStyle_.fontSize = TEST_FONT_SIZE;
    textStyle_.color = TEST_FONT_COLOR;
    textStyle_.fontWeight = FontWeight::W900;
    textStyle_.fontWidth = FontWidth::ULTRA_CONDENSED;
    textStyle_.fontStyle = FontStyle::ITALIC;
    typographyCreate_->PushStyle(textStyle_);
}

void OH_Drawing_TypographyImageTest::TearDown()
{
    typographyCreate_.reset();
    typography_.reset();
}

void EnsureDirectoryExists(const std::string& path)
{
    if (!std::filesystem::exists(path)) {
        std::filesystem::create_directories(path);
        std::cerr << "Directory created: " << path << std::endl;
    } else {
        if (!std::filesystem::is_directory(path)) {
            std::cerr << "Path exists and is not a directory: " << path << std::endl;
        } else {
            std::cerr << "Directory already exists: " << path << std::endl;
        }
    }
}

void CreateImg(std::shared_ptr<OHOS::Media::PixelMap>& pixelMap, const std::string& fileName)
{
    OHOS::Media::ImagePacker imagePacker;
    OHOS::Media::PackOption packOption;
    packOption.format = OPTION_FORMAT_TEST;
    packOption.quality = OPTION_QUALITY_TEST;
    packOption.numberHint = OPTION_NUMBER_HINT_TEST;
    EnsureDirectoryExists(IMAGE_INPUT_PNG_PATH_TEST);
    ASSERT_NE(pixelMap, nullptr);
    imagePacker.StartPacking(IMAGE_INPUT_PNG_PATH_TEST + fileName + ".png", packOption);
    imagePacker.AddImage(*pixelMap);
    imagePacker.FinalizePacking();
}

/*
 * @tc.name: TypographyGetTextPathImageByIndexTest001
 * @tc.desc: test for get text path image by index
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyImageTest, TypographyGetTextPathImageByIndexTest001, TestSize.Level0)
{
    typographyCreate_->AppendText(u"0123456789:");
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    double maxWidth = 500;
    typography_->Layout(maxWidth);
    ImageOptions options{typography_->GetMaxWidth() + IMAGE_PADDING, typography_->GetHeight() + IMAGE_PADDING,
        IMAGE_PADDING, IMAGE_PADDING};
    std::shared_ptr<OHOS::Media::PixelMap> pixelMap = typography_->GetTextPathImageByIndex(0, 11, options, false);
    ASSERT_NE(pixelMap, nullptr);
    CreateImg(pixelMap, "text_test_path_");
}

/*
 * @tc.name: TypographyGetTextPathImageByIndexTest002
 * @tc.desc: test for get text path image by index
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyImageTest, TypographyGetTextPathImageByIndexTest002, TestSize.Level0)
{
    typographyCreate_->AppendText(u"0123456789:");
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    double maxWidth = 500;
    typography_->Layout(maxWidth);
    ImageOptions options{typography_->GetMaxWidth() + IMAGE_PADDING, typography_->GetHeight() + IMAGE_PADDING,
        IMAGE_PADDING, IMAGE_PADDING};
    std::shared_ptr<OHOS::Media::PixelMap> pixelMap = typography_->GetTextPathImageByIndex(0, 11, options, true);
    ASSERT_NE(pixelMap, nullptr);
    CreateImg(pixelMap, "text_test_filled_path_");
}

/*
 * @tc.name: TypographyGetTextPathImageByIndexTest003
 * @tc.desc: test for get text path image by index
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyImageTest, TypographyGetTextPathImageByIndexTest003, TestSize.Level0)
{
    typographyCreate_->AppendText(u"0123456789:");
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    double maxWidth = 500;
    typography_->Layout(maxWidth);
    ImageOptions options{typography_->GetMaxWidth() + IMAGE_PADDING, typography_->GetHeight() + IMAGE_PADDING,
        IMAGE_PADDING, IMAGE_PADDING};
    std::shared_ptr<OHOS::Media::PixelMap> pixelMap = typography_->GetTextPathImageByIndex(3, 8, options, false);
    ASSERT_NE(pixelMap, nullptr);
    EXPECT_EQ(pixelMap->GetWidth(), 510);
    EXPECT_EQ(pixelMap->GetHeight(), 45);
}

/*
 * @tc.name: TypographyGetTextPathImageByIndexTest004
 * @tc.desc: test for get text path image by index
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyImageTest, TypographyGetTextPathImageByIndexTest004, TestSize.Level0)
{
    typographyCreate_->AppendText(u"0123456789:");
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    double maxWidth = 500;
    typography_->Layout(maxWidth);
    ImageOptions options{typography_->GetMaxWidth() + IMAGE_PADDING, typography_->GetHeight() + IMAGE_PADDING,
        IMAGE_PADDING, IMAGE_PADDING};
    std::shared_ptr<OHOS::Media::PixelMap> pixelMap = typography_->GetTextPathImageByIndex(3, 8, options, true);
    ASSERT_NE(pixelMap, nullptr);
    EXPECT_EQ(pixelMap->GetWidth(), 510);
    EXPECT_EQ(pixelMap->GetHeight(), 45);
}

/*
 * @tc.name: TypographyGetTextPathImageByIndexTest005
 * @tc.desc: test for get text path image by index
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyImageTest, TypographyGetTextPathImageByIndexTest005, TestSize.Level0)
{
    typographyCreate_->AppendText(u"0123456789:");
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    ImageOptions options{typography_->GetMaxWidth() + IMAGE_PADDING, typography_->GetHeight() + IMAGE_PADDING,
        IMAGE_PADDING, IMAGE_PADDING};
    std::shared_ptr<OHOS::Media::PixelMap> pixelMap = typography_->GetTextPathImageByIndex(3, 8, options, false);
    ASSERT_EQ(pixelMap, nullptr);
}

/*
 * @tc.name: TypographyGetTextPathImageByIndexTest006
 * @tc.desc: test for get text path image by index
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyImageTest, TypographyGetTextPathImageByIndexTest006, TestSize.Level0)
{
    typographyCreate_->AppendText(u"0123456789:");
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    double maxWidth = 500;
    typography_->Layout(maxWidth);
    ImageOptions options{typography_->GetMaxWidth() + IMAGE_PADDING, typography_->GetHeight() + IMAGE_PADDING,
        IMAGE_PADDING, IMAGE_PADDING};
    std::shared_ptr<OHOS::Media::PixelMap> pixelMap = typography_->GetTextPathImageByIndex(-100, -1, options, false);
    ASSERT_EQ(pixelMap, nullptr);
}

/*
 * @tc.name: TypographyGetTextPathImageByIndexTest007
 * @tc.desc: test for get text path image by index
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyImageTest, TypographyGetTextPathImageByIndexTest007, TestSize.Level0)
{
    typographyCreate_->AppendText(u"0123456789:");
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    double maxWidth = 500;
    typography_->Layout(maxWidth);
    ImageOptions options{typography_->GetMaxWidth() + IMAGE_PADDING, typography_->GetHeight() + IMAGE_PADDING,
        IMAGE_PADDING, IMAGE_PADDING};
    std::shared_ptr<OHOS::Media::PixelMap> pixelMap = typography_->GetTextPathImageByIndex(7, 1, options, false);
    ASSERT_EQ(pixelMap, nullptr);
}

/*
 * @tc.name: TypographyGetTextPathImageByIndexTest008
 * @tc.desc: test for get text path image by index
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyImageTest, TypographyGetTextPathImageByIndexTest008, TestSize.Level0)
{
    typographyCreate_->AppendText(u"0123456789:");
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    double maxWidth = 500;
    typography_->Layout(maxWidth);
    ImageOptions options{typography_->GetMaxWidth() + IMAGE_PADDING, typography_->GetHeight() + IMAGE_PADDING,
        IMAGE_PADDING, IMAGE_PADDING};
    std::shared_ptr<OHOS::Media::PixelMap> pixelMap = typography_->GetTextPathImageByIndex(2, 15, options, false);
    ASSERT_NE(pixelMap, nullptr);
}

/*
 * @tc.name: TypographyGetTextPathImageByIndexTest009
 * @tc.desc: test for get text path image by index
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyImageTest, TypographyGetTextPathImageByIndexTest009, TestSize.Level0)
{
    typographyCreate_->AppendText(u"0123456789:");
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    double maxWidth = 500;
    typography_->Layout(maxWidth);
    ImageOptions options{typography_->GetMaxWidth() + IMAGE_PADDING, typography_->GetHeight() + IMAGE_PADDING,
        IMAGE_PADDING, IMAGE_PADDING};
    std::shared_ptr<OHOS::Media::PixelMap> pixelMap = typography_->GetTextPathImageByIndex(0, SIZE_MAX, options, false);
    ASSERT_NE(pixelMap, nullptr);
}

/*
 * @tc.name: TypographyGetTextPathImageByIndexTest010
 * @tc.desc: test for get text path image by index
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyImageTest, TypographyGetTextPathImageByIndexTest010, TestSize.Level0)
{
    typographyCreate_->AppendText(u"0123456789:");
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    double maxWidth = 500;
    typography_->Layout(maxWidth);
    ImageOptions options{IMAGE_NEGATIVE_VALUE, IMAGE_NEGATIVE_VALUE, IMAGE_NEGATIVE_VALUE, IMAGE_NEGATIVE_VALUE};
    std::shared_ptr<OHOS::Media::PixelMap> pixelMap = typography_->GetTextPathImageByIndex(0, SIZE_MAX, options, false);
    ASSERT_EQ(pixelMap, nullptr);
}

/*
 * @tc.name: TypographyGetTextPathsByIndexTest001
 * @tc.desc: test for get text paths by index with full range
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyImageTest, TypographyGetTextPathsByIndexTest001, TestSize.Level0)
{
    typographyCreate_->AppendText(u"0123456789:");
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    double maxWidth = 500;
    typography_->Layout(maxWidth);
    auto result = typography_->GetTextPathsByIndex(0, 11);
    EXPECT_EQ(result.size(), 11);
}

/*
 * @tc.name: TypographyGetTextPathsByIndexTest002
 * @tc.desc: test for get text paths by index with partial range
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyImageTest, TypographyGetTextPathsByIndexTest002, TestSize.Level0)
{
    typographyCreate_->AppendText(u"0123456789:");
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    double maxWidth = 500;
    typography_->Layout(maxWidth);
    auto result = typography_->GetTextPathsByIndex(3, 8);
    EXPECT_EQ(result.size(), 5);
}

/*
 * @tc.name: TypographyGetTextPathsByIndexTest003
 * @tc.desc: test for get text paths by index without layout
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyImageTest, TypographyGetTextPathsByIndexTest003, TestSize.Level0)
{
    typographyCreate_->AppendText(u"0123456789:");
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    auto result = typography_->GetTextPathsByIndex(3, 8);
    EXPECT_TRUE(result.empty());
}

/*
 * @tc.name: TypographyGetTextPathsByIndexTest004
 * @tc.desc: test for get text paths by index with invalid negative indices
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyImageTest, TypographyGetTextPathsByIndexTest004, TestSize.Level0)
{
    typographyCreate_->AppendText(u"0123456789:");
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    double maxWidth = 500;
    typography_->Layout(maxWidth);
    auto result = typography_->GetTextPathsByIndex(-100, -1);
    EXPECT_TRUE(result.empty());
}

/*
 * @tc.name: TypographyGetTextPathsByIndexTest005
 * @tc.desc: test for get text paths by index with inverted range
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyImageTest, TypographyGetTextPathsByIndexTest005, TestSize.Level0)
{
    typographyCreate_->AppendText(u"0123456789:");
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    double maxWidth = 500;
    typography_->Layout(maxWidth);
    auto result = typography_->GetTextPathsByIndex(7, 1);
    EXPECT_TRUE(result.empty());
}

/*
 * @tc.name: TypographyGetTextPathsByIndexTest006
 * @tc.desc: test for get text paths by index with range exceeding text length
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyImageTest, TypographyGetTextPathsByIndexTest006, TestSize.Level0)
{
    typographyCreate_->AppendText(u"0123456789:");
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    double maxWidth = 500;
    typography_->Layout(maxWidth);
    auto result = typography_->GetTextPathsByIndex(2, 15);
    EXPECT_EQ(result.size(), 9);
}

/*
 * @tc.name: TypographyGetTextPathsByIndexTest007
 * @tc.desc: test for get text paths by index with SIZE_MAX range
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyImageTest, TypographyGetTextPathsByIndexTest007, TestSize.Level0)
{
    typographyCreate_->AppendText(u"0123456789:");
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    double maxWidth = 500;
    typography_->Layout(maxWidth);
    auto result = typography_->GetTextPathsByIndex(0, SIZE_MAX);
    EXPECT_EQ(result.size(), 11);
}

/*
 * @tc.name: TypographyGetTextPathsByIndexTest008
 * @tc.desc: test for get text paths by index with default parameters (all glyphs)
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyImageTest, TypographyGetTextPathsByIndexTest008, TestSize.Level0)
{
    typographyCreate_->AppendText(u"0123456789:");
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    double maxWidth = 500;
    typography_->Layout(maxWidth);
    auto result = typography_->GetTextPathsByIndex();
    EXPECT_EQ(result.size(), 11);
}

/*
 * @tc.name: TypographyGetTextPathsByIndexTest009
 * @tc.desc: test for get text paths by index with bitmap font
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyImageTest, TypographyGetTextPathsByIndexTest009, TestSize.Level0)
{
    typographyCreate_->AppendText(u"0123😊456789:");
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    double maxWidth = 500;
    typography_->Layout(maxWidth);
    auto result = typography_->GetTextPathsByIndex();
    EXPECT_EQ(result.size(), 0);
}

/*
 * @tc.name: TypographyGetTextPathsByIndexFakeBoldTest001
 * @tc.desc: test that fake bold enabled preserves path point count (same topology)
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyImageTest, TypographyGetTextPathsByIndexFakeBoldTest001, TestSize.Level0)
{
    std::u16string text = u"ABCDEFGH";
    double maxWidth = 500;

    // Fake bold ON: fontWeight W900 requests bold but wght axis loads weight-400 typeface.
    // Condition: wantedWeight(900) >= 600 and 900 - 400 >= 200 → embolden applied.
    OHOS::Rosen::TextStyle boldStyle;
    boldStyle.fontSize = TEST_FONT_SIZE;
    boldStyle.color = TEST_FONT_COLOR;
    boldStyle.fontWeight = FontWeight::W900;
    boldStyle.fontVariations.SetAxisValue("wght", 400);
    typographyCreate_->PushStyle(boldStyle);
    typographyCreate_->AppendText(text);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    typography_->Layout(maxWidth);
    auto fakeBoldOnPaths = typography_->GetTextPathsByIndex();
    ASSERT_FALSE(fakeBoldOnPaths.empty());

    // Fake bold OFF: fontWeight W400 matches wght axis 400, no delta → embolden skipped.
    // Condition: wantedWeight(400) < 600 → fake bold not triggered.
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    auto normalBuilder = OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    ASSERT_NE(normalBuilder, nullptr);
    OHOS::Rosen::TextStyle normalStyle;
    normalStyle.fontSize = TEST_FONT_SIZE;
    normalStyle.color = TEST_FONT_COLOR;
    normalStyle.fontWeight = FontWeight::W400;
    normalStyle.fontVariations.SetAxisValue("wght", 400);
    normalBuilder->PushStyle(normalStyle);
    normalBuilder->AppendText(text);
    auto normalTypography = normalBuilder->CreateTypography();
    ASSERT_NE(normalTypography, nullptr);
    normalTypography->Layout(maxWidth);
    auto fakeBoldOffPaths = normalTypography->GetTextPathsByIndex();
    ASSERT_FALSE(fakeBoldOffPaths.empty());

    // Embolden thickens outlines but must not change path topology.
    ASSERT_EQ(fakeBoldOnPaths.size(), fakeBoldOffPaths.size());
    for (size_t i = 0; i < fakeBoldOnPaths.size() && i < fakeBoldOffPaths.size(); i++) {
        EXPECT_EQ(fakeBoldOnPaths[i].path.CountPoints(), fakeBoldOffPaths[i].path.CountPoints())
            << "Point count differs at glyph " << i;
        EXPECT_EQ(fakeBoldOnPaths[i].path.CountVerbs(), fakeBoldOffPaths[i].path.CountVerbs())
            << "Verb count differs at glyph " << i;
        for (int j = 0; j < fakeBoldOnPaths[i].path.CountPoints() && j < fakeBoldOffPaths[i].path.CountPoints(); j++) {
            EXPECT_DOUBLE_EQ(fakeBoldOnPaths[i].path.GetPoint(j).GetX(), fakeBoldOffPaths[i].path.GetPoint(j).GetX())
                << "X coordinate differs at glyph " << i << ", point " << j;
            EXPECT_DOUBLE_EQ(fakeBoldOnPaths[i].path.GetPoint(j).GetY(), fakeBoldOffPaths[i].path.GetPoint(j).GetY())
                << "Y coordinate differs at glyph " << i << ", point " << j;
        }
    }
}
} // namespace Rosen
} // namespace OHOS