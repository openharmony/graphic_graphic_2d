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

#ifndef ROSEN_SAMPLES_TEXGINE_FEATURE_TEST_FEATURE_TEST_FRAMEWORK_H
#define ROSEN_SAMPLES_TEXGINE_FEATURE_TEST_FEATURE_TEST_FRAMEWORK_H

#include <functional>
#include <list>
#include <memory>
#include <optional>
#include <string>

#include <texgine/typography.h>

#include "texgine_canvas.h"

struct FeatureTestOption {
    // border
    bool needBorder = true;
    uint32_t colorBorder = 0xffff0000; // red

    // margin
    int32_t marginTop = 50;
    int32_t marginLeft = 50;

    // rainbow char
    bool needRainbowChar = false;
};

struct TypographyData {
    std::shared_ptr<Texgine::Typography> typography = nullptr;
    std::string comment = "";
    size_t rainbowStart = 0;
    size_t rainbowEnd = 1e9;
    Texgine::TextRectWidthStyle ws = Texgine::TextRectWidthStyle::TIGHT;
    Texgine::TextRectHeightStyle hs = Texgine::TextRectHeightStyle::TIGHT;
    std::optional<bool> needRainbowChar = std::nullopt;
    bool atNewline = false;

    std::function<void(const struct TypographyData &, Texgine::TexgineCanvas &, double, double)> onPaint = nullptr;
};

class TestFeature {
public:
    explicit TestFeature(const std::string &testName);
    virtual ~TestFeature() = default;

    virtual void Layout() = 0;

    /*
     * @brief Returns the option in the feature test
     */
    const struct FeatureTestOption &GetFeatureTestOption() const
    {
        return option_;
    }

    /*
     * @brief Returns the list of TypographyData user setting in feature test
     */
    const std::list<struct TypographyData> &GetTypographies() const
    {
        return typographies_;
    }

    const std::string &GetTestName() const
    {
        return testName_;
    }

protected:
    std::list<struct TypographyData> typographies_;
    struct FeatureTestOption option_ = {};

private:
    std::string testName_ = {};
};

class FeatureTestCollection {
public:
    ~FeatureTestCollection() = default;
    static FeatureTestCollection &GetInstance();

    /*
     * @brief Returns the list of all feature test
     */
    const std::list<TestFeature*> &GetTests() const
    {
        return tests_;
    }

    /*
     * @brief Add feature test to FeatureTestCollection
     * @param test The pointer of feature test
     */
    void RegisterTest(const TestFeature *test);

private:
    FeatureTestCollection() = default;
    FeatureTestCollection(const FeatureTestCollection &) = delete;
    FeatureTestCollection(FeatureTestCollection &&) = delete;
    FeatureTestCollection &operator=(const FeatureTestCollection &) = delete;
    FeatureTestCollection &operator=(FeatureTestCollection &&) = delete;

    std::list<TestFeature*> tests_;
};

#endif // ROSEN_SAMPLES_TEXGINE_FEATURE_TEST_FEATURE_TEST_FRAMEWORK_H
