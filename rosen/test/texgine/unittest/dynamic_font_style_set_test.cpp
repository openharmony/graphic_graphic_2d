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

#include <dynamic_font_style_set.h>

#include "param_test_macros.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace TextEngine {
struct Mockvars {
    std::shared_ptr<TexgineTypeface> typeface = std::make_shared<TexgineTypeface>();
    std::shared_ptr<TexgineFontStyle> fontStyle = std::make_shared<TexgineFontStyle>();
    std::shared_ptr<TextEngine::DynamicFontStyleSet> normalSet = nullptr;
} g_dfssMockVars;


auto Init(struct Mockvars vars)
{
    return [vars = std::move(vars)] {
        g_dfssMockVars = std::move(vars);
        auto typeface = std::make_unique<Typeface>(g_dfssMockVars.typeface);
        g_dfssMockVars.normalSet = std::make_shared<DynamicFontStyleSet>(std::move(typeface));
    };
}


Typeface::Typeface(std::shared_ptr<TexgineTypeface> tf)
{
    typeface_ = g_dfssMockVars.typeface;
}

#define PARAMCLASS DynamicFontStyleSet
class DynamicFontStyleSetTest : public testing::Test {
public:
};

#define PARAMFUNC Count
HWTEST_F(DynamicFontStyleSetTest, Count, TestSize.Level1)
{
    DEFINE_TESTINFO0();

    DynamicFontStyleSet nullset(nullptr);
    RUN_TESTINFO0(nullset, {.checkFunc = CreateChecker(0)});

    DynamicFontStyleSet normset(std::make_unique<Typeface>(g_dfssMockVars.typeface));
    RUN_TESTINFO0(normset, {.checkFunc = CreateChecker(1)});
}
#undef PARAMFUNC

#define PARAMFUNC GetStyle
auto CreateStyleChecker(std::shared_ptr<TexgineFontStyle> s)
{
    return [s] (int, std::shared_ptr<TexgineFontStyle> style, std::shared_ptr<TexgineString>, DynamicFontStyleSet &) {
        return *s->GetFontStyle() == *style->GetFontStyle();
    };
}

/**
 * @tc.name: GetStyle
 * @tc.desc: Verify the GetStyle
 * @tc.type:FUNC
 * @tc.require: issueI6V6JU
 */
HWTEST_F(DynamicFontStyleSetTest, GetStyle, TestSize.Level1)
{
    DEFINE_VOID_TESTINFO3(int, std::shared_ptr<TexgineFontStyle>, std::shared_ptr<TexgineString>);

    TexgineFontStyle::Slant slant {};
    // (0, 0, slant): (wegiht, width, slant)
    auto s1 = std::make_shared<TexgineFontStyle>(0, 0, slant);
    auto s2 = std::make_shared<TexgineFontStyle>(100, 100, slant);
    DynamicFontStyleSet nullset(nullptr);

#define CORE_TEST(obj, mock, a1, expect)               \
    {                                                  \
        std::shared_ptr<TexgineFontStyle> style = s1;  \
        RUN_VOID_TESTINFO3(obj, {                      \
            .init = Init({.fontStyle = mock}),        \
            .arg1 = a1, .arg2 = style,                 \
            .checkFunc = CreateStyleChecker(expect)}); \
    }

    RUN_VOID_TESTINFO3(nullset, {.arg2 = nullptr, .exception = ExceptionType::INVALID_ARGUMENT});

    // -1 is the parameter of GetStyle, index
    CORE_TEST(nullset, s2, -1, s1);
    CORE_TEST(nullset, s2, 0, s1);
    CORE_TEST(nullset, s2, 1, s1);
    CORE_TEST(*g_dfssMockVars.normalSet, s2, -1, s1);
    CORE_TEST(*g_dfssMockVars.normalSet, s2, 1, s1);
#undef CORE_TEST

    auto style = s1;
    auto initfunc = Init({.typeface = nullptr, .fontStyle = s2});
    auto checkfunc = CreateStyleChecker(s1);
    RUN_VOID_TESTINFO3(*g_dfssMockVars.normalSet, {.init = initfunc, .arg2 = style, .checkFunc = checkfunc});
}
#undef PARAMFUNC

#define PARAMFUNC CreateTypeface
/**
 * @tc.name: CreateTypeface
 * @tc.desc: Verify the CreateTypeface
 * @tc.type:FUNC
 * @tc.require: issueI6V6JU
 */
HWTEST_F(DynamicFontStyleSetTest, CreateTypeface, TestSize.Level1)
{
    DEFINE_TESTINFO1(int);
    Init({})();
    DynamicFontStyleSet nullset(nullptr);
    DynamicFontStyleSet normset(std::make_unique<Typeface>(g_dfssMockVars.typeface));
    // arg1 is the parameter of CreateTypeface, index
    RUN_TESTINFO1(nullset, {.arg1 = -1, .checkFunc = CreateChecker<std::shared_ptr<TexgineTypeface>>(nullptr)});
    RUN_TESTINFO1(nullset, {.arg1 = 0, .checkFunc = CreateChecker<std::shared_ptr<TexgineTypeface>>(nullptr)});
    RUN_TESTINFO1(nullset, {.arg1 = 1, .checkFunc = CreateChecker<std::shared_ptr<TexgineTypeface>>(nullptr)});
    RUN_TESTINFO1(normset, {.arg1 = -1, .checkFunc = CreateChecker<std::shared_ptr<TexgineTypeface>>(nullptr)});
    RUN_TESTINFO1(normset, {.arg1 = 0,
        .checkFunc = CreateChecker<std::shared_ptr<TexgineTypeface>>(g_dfssMockVars.typeface)});
    RUN_TESTINFO1(normset, {.arg1 = 1, .checkFunc = CreateChecker<std::shared_ptr<TexgineTypeface>>(nullptr)});
}
#undef PARAMFUNC

#define PARAMFUNC MatchStyle
/**
 * @tc.name: MatchStyle
 * @tc.desc: Verify the MatchStyle
 * @tc.type:FUNC
 * @tc.require: issueI6V6JU
 */
HWTEST_F(DynamicFontStyleSetTest, MatchStyle, TestSize.Level1)
{
    DEFINE_TESTINFO1(std::shared_ptr<TexgineFontStyle>);
    Init({})();
    DynamicFontStyleSet nullset(nullptr);
    DynamicFontStyleSet normset(std::make_unique<Typeface>(g_dfssMockVars.typeface));
    RUN_TESTINFO1(nullset, {.checkFunc = CreateChecker<std::shared_ptr<TexgineTypeface>>(nullptr)});
    RUN_TESTINFO1(normset, {.checkFunc = CreateChecker<std::shared_ptr<TexgineTypeface>>(g_dfssMockVars.typeface)});
}
#undef PARAMFUNC
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
