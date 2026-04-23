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

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <gtest/gtest.h>
#include <mutex>
#include <thread>
#include <vector>

#include "draw/color.h"
#include "rosen_text/font_collection.h"
#include "rosen_text/typography.h"
#include "rosen_text/typography_create.h"
#include "rosen_text/typography_style.h"
#include "rosen_text/text_style.h"
#include "ohos/init_data.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen;
using namespace OHOS::Rosen::Drawing;

namespace {

constexpr int NUM_THREADS = 8;
constexpr int NUM_CREATE_THREADS = 4;
constexpr int NUM_LAYOUT_THREADS = 3;
constexpr int NUM_QUERY_THREADS = 3;
constexpr int OPERATIONS_PER_THREAD = 50;
constexpr int THREAD_SLEEP_TIME_100 = 100;
constexpr int THREAD_SLEEP_TIME_10 = 10;

// Barrier for synchronizing thread start
class Barrier {
public:
    explicit Barrier(int count) : count_(count), waiting_(0), generation_(0) {}

    void Wait()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        int gen = generation_;
        ++waiting_;
        if (waiting_ == count_) {
            ++generation_;
            waiting_ = 0;
            cv_.notify_all();
        } else {
            cv_.wait(lock, [this, gen] { return gen != generation_; });
        }
    }

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    int count_ { 0 };
    int waiting_ { 0 };
    int generation_ { 0 };
};

} // namespace

namespace txt {
class TypographyThreadTest : public testing::Test {
protected:
    void SetUp() override
    {
        SetHwIcuDirectory();
    }

    void TearDown() override
    {
        // Allow some time for all threads to finish
        std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_SLEEP_TIME_100));
    }

    std::shared_ptr<OHOS::Rosen::FontCollection> CreateFontCollection()
    {
        return OHOS::Rosen::FontCollection::Create();
    }

    std::unique_ptr<OHOS::Rosen::Typography> CreateSimpleTypography(const std::u16string& text,
        const std::shared_ptr<OHOS::Rosen::FontCollection>& fontCollection)
    {
        OHOS::Rosen::TypographyStyle typographyStyle;
        std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
            OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
        typographyCreate->AppendText(text);
        return typographyCreate->CreateTypography();
    }
};

struct ThreadParams {
    Barrier& barrier;
    std::atomic<int>& operationCount;

    explicit ThreadParams(Barrier& barrier, std::atomic<int>& operationCount)
        : barrier(barrier), operationCount(operationCount)
    {}
};

/**
 * @tc.name: ConcurrentParagraphLayoutTest
 * @tc.desc: test concurrent Paragraph Layout operations
 * @tc.type: FUNC
 */
HWTEST_F(TypographyThreadTest, ConcurrentParagraphLayoutTest, TestSize.Level0)
{
    auto fontCollection = CreateFontCollection();
    std::u16string text = u"Hello World";

    Barrier barrier(NUM_THREADS);
    std::vector<std::thread> threads;
    std::atomic<int> operationCount(0);
    ThreadParams params(barrier, operationCount);

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([this, &params, &fontCollection, &text]() {
            params.barrier.Wait(); // Synchronize start

            for (int j = 0; j < OPERATIONS_PER_THREAD; ++j) {
                auto typography = CreateSimpleTypography(text, fontCollection);
                typography->Layout(100);
                params.operationCount++;
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(params.operationCount.load(), NUM_THREADS * OPERATIONS_PER_THREAD);
}

/**
 * @tc.name: ConcurrentParagraphBuilderTest
 * @tc.desc: test concurrent ParagraphBuilder operations
 * @tc.type: FUNC
 */
HWTEST_F(TypographyThreadTest, ConcurrentParagraphBuilderTest, TestSize.Level0)
{
    auto fontCollection = CreateFontCollection();

    Barrier barrier(NUM_THREADS);
    std::vector<std::thread> threads;
    std::atomic<int> operationCount(0);
    ThreadParams params(barrier, operationCount);

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([&params, fontCollection, i]() {
            params.barrier.Wait(); // Synchronize start

            for (int j = 0; j < OPERATIONS_PER_THREAD; ++j) {
                TypographyStyle typographyStyle;
                TextStyle textStyle;
                textStyle.fontSize = 14.0;
                textStyle.fontFamilies = {"Roboto"};

                std::unique_ptr<TypographyCreate> typographyCreate =
                    TypographyCreate::Create(typographyStyle, fontCollection);

                typographyCreate->PushStyle(textStyle);
                std::u16string text =
                    u"Thread " + std::u16string(1, 'A' + (i % 26)) + u" text " + std::u16string(1, '0' + (j % 10));
                typographyCreate->AppendText(text);
                typographyCreate->PopStyle();

                auto typography = typographyCreate->CreateTypography();
                EXPECT_NE(typography, nullptr);
                params.operationCount++;
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(params.operationCount.load(), NUM_THREADS * OPERATIONS_PER_THREAD);
}

/**
 * @tc.name: ConcurrentParagraphMetricsTest
 * @tc.desc: test concurrent Paragraph metrics queries
 * @tc.type: FUNC
 */
HWTEST_F(TypographyThreadTest, ConcurrentParagraphMetricsTest, TestSize.Level0)
{
    auto fontCollection = CreateFontCollection();
    std::u16string text = u"Test text for metrics";

    auto typography = CreateSimpleTypography(text, fontCollection);
    typography->Layout(200);

    Barrier barrier(NUM_THREADS);
    std::vector<std::thread> threads;
    std::atomic<int> operationCount(0);
    ThreadParams params(barrier, operationCount);

    // Get raw pointer since unique_ptr cannot be copied
    Typography* typographyPtr = typography.get();
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([&params, typographyPtr]() {
            params.barrier.Wait(); // Synchronize start

            for (int j = 0; j < OPERATIONS_PER_THREAD; ++j) {
                // Concurrent reads of typography metrics
                typographyPtr->GetMaxWidth();
                typographyPtr->GetHeight();
                typographyPtr->GetLineCount();
                typographyPtr->GetMinIntrinsicWidth();
                typographyPtr->GetMaxIntrinsicWidth();
                typographyPtr->GetAlphabeticBaseline();
                typographyPtr->GetIdeographicBaseline();
                typographyPtr->GetGlyphsBoundsTop();
                typographyPtr->GetGlyphsBoundsBottom();
                params.operationCount++;
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(params.operationCount.load(), NUM_THREADS * OPERATIONS_PER_THREAD);
}

/**
 * @tc.name: ConcurrentParagraphGetRectsTest
 * @tc.desc: test concurrent GetRectsForRange operations
 * @tc.type: FUNC
 */
HWTEST_F(TypographyThreadTest, ConcurrentParagraphGetRectsTest, TestSize.Level0)
{
    auto fontCollection = CreateFontCollection();
    std::u16string text = u"Test text for getting rects";

    auto typography = CreateSimpleTypography(text, fontCollection);
    typography->Layout(200);

    Barrier barrier(NUM_THREADS);
    std::vector<std::thread> threads;
    std::atomic<int> operationCount(0);
    ThreadParams params(barrier, operationCount);

    // Get raw pointer since unique_ptr cannot be copied
    Typography* typographyPtr = typography.get();
    const auto workerLambda = [&params, typographyPtr, text]() {
        params.barrier.Wait(); // Synchronize start

        for (int j = 0; j < OPERATIONS_PER_THREAD; ++j) {
            size_t start = j % text.size();
            size_t end = (j + 1) % (text.size() + 1);
            if (start > end) {
                std::swap(start, end);
            }

            auto rects = typographyPtr->GetTextRectsByBoundary(start, end,
                TextRectHeightStyle::COVER_TOP_AND_BOTTOM, TextRectWidthStyle::TIGHT);
            params.operationCount++;
        }
    };

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back(workerLambda);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(params.operationCount.load(), NUM_THREADS * OPERATIONS_PER_THREAD);
}

/**
 * @tc.name: ConcurrentParagraphGetGlyphPositionTest
 * @tc.desc: test concurrent GetGlyphPositionAtCoordinate operations
 * @tc.type: FUNC
 */
HWTEST_F(TypographyThreadTest, ConcurrentParagraphGetGlyphPositionTest, TestSize.Level0)
{
    auto fontCollection = CreateFontCollection();
    std::u16string text = u"Test text for glyph position";
    auto typography = CreateSimpleTypography(text, fontCollection);
    typography->Layout(200);

    Barrier barrier(NUM_THREADS);
    std::vector<std::thread> threads;
    std::atomic<int> operationCount(0);
    ThreadParams params(barrier, operationCount);

    // Get raw pointer since unique_ptr cannot be copied
    Typography* typographyPtr = typography.get();
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([&params, typographyPtr]() {
            params.barrier.Wait(); // Synchronize start

            for (int j = 0; j < OPERATIONS_PER_THREAD; ++j) {
                double x = (j % 200);
                double y = (j % 50);
                typographyPtr->GetGlyphIndexByCoordinate(x, y);
                params.operationCount++;
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(params.operationCount.load(), NUM_THREADS * OPERATIONS_PER_THREAD);
}

/**
 * @tc.name: ConcurrentParagraphGetWordBoundaryTest
 * @tc.desc: test concurrent GetWordBoundary operations
 * @tc.type: FUNC
 */
HWTEST_F(TypographyThreadTest, ConcurrentParagraphGetWordBoundaryTest, TestSize.Level0)
{
    auto fontCollection = CreateFontCollection();
    std::u16string text = u"Test text for word boundary";
    auto typography = CreateSimpleTypography(text, fontCollection);
    typography->Layout(200);

    Barrier barrier(NUM_THREADS);
    std::vector<std::thread> threads;
    std::atomic<int> operationCount(0);
    ThreadParams params(barrier, operationCount);

    // Get raw pointer since unique_ptr cannot be copied
    Typography* typographyPtr = typography.get();
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([&params, typographyPtr, text]() {
            params.barrier.Wait(); // Synchronize start

            for (int j = 0; j < OPERATIONS_PER_THREAD; ++j) {
                size_t offset = j % text.size();
                typographyPtr->GetWordBoundaryByIndex(offset);
                params.operationCount++;
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(params.operationCount.load(), NUM_THREADS * OPERATIONS_PER_THREAD);
}

/**
 * @tc.name: RaceConditionParagraphLifecycleTest
 * @tc.desc: test for race conditions during Paragraph lifecycle
 * @tc.type: FUNC
 */
HWTEST_F(TypographyThreadTest, RaceConditionParagraphLifecycleTest, TestSize.Level0)
{
    auto fontCollection = CreateFontCollection();
    std::atomic<int> createCount(0);
    std::atomic<int> layoutCount(0);
    std::atomic<int> queryCount(0);

    Barrier barrier(NUM_CREATE_THREADS + NUM_LAYOUT_THREADS + NUM_QUERY_THREADS);
    std::vector<std::thread> threads;

    // Threads creating typography
    for (int i = 0; i < NUM_CREATE_THREADS; ++i) {
        threads.emplace_back([this, &barrier, &createCount, &fontCollection, i]() {
            barrier.Wait(); // Synchronize start
            for (int j = 0; j < 50; ++j) {
                std::u16string text = u"Create " + std::u16string(1, 'A' + (i % 26));
                auto typography = CreateSimpleTypography(text, fontCollection);
                EXPECT_NE(typography, nullptr);
                createCount++;
            }
        });
    }

    // Threads layouting typography
    for (int i = 0; i < NUM_LAYOUT_THREADS; ++i) {
        threads.emplace_back([this, &barrier, &layoutCount, fontCollection]() {
            barrier.Wait(); // Synchronize start
            for (int j = 0; j < 50; ++j) {
                std::u16string text = u"Layout test";
                auto typography = CreateSimpleTypography(text, fontCollection);
                typography->Layout(100 + (j % 100));
                layoutCount++;
            }
        });
    }

    // Threads querying metrics
    for (int i = 0; i < NUM_QUERY_THREADS; ++i) {
        threads.emplace_back([this, &barrier, &queryCount, &fontCollection]() {
            barrier.Wait(); // Synchronize start
            for (int j = 0; j < 50; ++j) {
                std::u16string text = u"Query test";
                auto typography = CreateSimpleTypography(text, fontCollection);
                typography->Layout(150);
                typography->GetHeight();
                typography->GetMaxWidth();
                typography->GetLineCount();
                queryCount++;
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(createCount.load(), NUM_CREATE_THREADS * 50); // 4 threads * 50 iterations
    EXPECT_EQ(layoutCount.load(), NUM_LAYOUT_THREADS * 50); // 3 threads * 50 iterations
    EXPECT_EQ(queryCount.load(), NUM_QUERY_THREADS * 50);   // 3 threads * 50 iterations
}

/**
 * @tc.name: ConcurrentParagraphWithMultipleStylesTest
 * @tc.desc: test concurrent paragraph building with multiple styles
 * @tc.type: FUNC
 */
HWTEST_F(TypographyThreadTest, ConcurrentParagraphWithMultipleStylesTest, TestSize.Level0)
{
    auto fontCollection = CreateFontCollection();

    Barrier barrier(NUM_THREADS);
    std::vector<std::thread> threads;
    std::atomic<int> operationCount(0);
    ThreadParams params(barrier, operationCount);

    const auto workerLambda = [&params, &fontCollection]() {
        params.barrier.Wait(); // Synchronize start
        std::unique_ptr<TypographyCreate> builder = TypographyCreate::Create(TypographyStyle(), fontCollection);
        for (int j = 0; j < OPERATIONS_PER_THREAD; ++j) {
            // Add multiple styled text segments
            for (int k = 0; k < 5; ++k) {
                TextStyle style;
                style.fontSize = 12.0 + k * 2.0;
                style.fontWeight = static_cast<FontWeight>(k % 10);
                style.color = Drawing::Color::ColorQuadSetARGB(255, k * 30, (k + 1) * 30, (k + 2) * 30);

                builder->PushStyle(style);
                builder->AppendText(u"Styled ");
            }

            // Pop styles
            for (int k = 0; k < 3; ++k) {
                builder->PopStyle();
            }

            auto typography = builder->CreateTypography();
            EXPECT_NE(typography, nullptr);
            typography->Layout(200);
            params.operationCount++;
        }
    };

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back(workerLambda);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(params.operationCount.load(), NUM_THREADS * OPERATIONS_PER_THREAD);
}

/**
 * @tc.name: DeadlockPreventionTypographyTest
 * @tc.desc: test that no deadlocks occur with mixed typography operations
 * @tc.type: FUNC
 */
HWTEST_F(TypographyThreadTest, DeadlockPreventionTypographyTest, TestSize.Level0)
{
    auto fontCollection = CreateFontCollection();

    std::atomic<bool> completed[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; ++i) {
        completed[i].store(false);
    }
    std::atomic<bool> timeoutOccurred(false);
    std::atomic<int> incompleteThreadCount(0);

    std::vector<std::thread> threads;
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([this, &completed, &fontCollection, i]() {
            for (int j = 0; j < OPERATIONS_PER_THREAD; ++j) {
                // Mix of different operations
                std::u16string text = u"Thread " + std::u16string(1, 'A' + (i % 26));
                auto typography = CreateSimpleTypography(text, fontCollection);
                typography->Layout(100 + (j % 100));
                typography->GetHeight();
                typography->GetMaxWidth();
                typography->GetLineCount();

                // Create new font collection
                auto fc = OHOS::Rosen::FontCollection::Create();

                // Create typography builder
                std::unique_ptr<OHOS::Rosen::TypographyCreate> builder =
                    OHOS::Rosen::TypographyCreate::Create(OHOS::Rosen::TypographyStyle(), fontCollection);
                builder->AppendText(u"Deadlock test");
                auto t = builder->CreateTypography();
                t->Layout(150);
            }
            completed[i].store(true);
        });
    }

    // Wait with timeout to detect deadlock
    std::thread timeoutThread([&completed, &timeoutOccurred, &incompleteThreadCount]() {
        std::this_thread::sleep_for(std::chrono::seconds(THREAD_SLEEP_TIME_10));
        for (int i = 0; i < NUM_THREADS; ++i) {
            if (!completed[i].load()) {
                // Timeout - potential deadlock, record but don't use EXPECT in thread
                incompleteThreadCount++;
            }
        }
        timeoutOccurred.store(true);
    });

    for (auto& thread : threads) {
        thread.join();
    }

    timeoutThread.join();

    // All threads should have completed
    for (int i = 0; i < NUM_THREADS; ++i) {
        EXPECT_TRUE(completed[i].load());
    }

    // If timeout occurred, fail the test
    EXPECT_EQ(incompleteThreadCount.load(), 0) << "Some threads did not complete - possible deadlock";
}

/**
 * @tc.name: ConcurrentReadIntensiveTypographyWorkloadTest
 * @tc.desc: test read-heavy concurrent workload for typography
 * @tc.type: FUNC
 */
HWTEST_F(TypographyThreadTest, ConcurrentReadIntensiveTypographyWorkloadTest, TestSize.Level0)
{
    auto fontCollection = CreateFontCollection();
    std::u16string text = u"Read intensive test text for measuring performance under load";
    auto typography = CreateSimpleTypography(text, fontCollection);
    typography->Layout(300);

    Barrier barrier(NUM_THREADS);
    std::vector<std::thread> threads;
    std::atomic<int> operationCount(0);
    ThreadParams params(barrier, operationCount);

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([&params, &typography]() {
            params.barrier.Wait(); // Synchronize start

            for (int j = 0; j < OPERATIONS_PER_THREAD * 10; ++j) {
                // Read operations only
                typography->GetMaxWidth();
                typography->GetHeight();
                typography->GetMinIntrinsicWidth();
                typography->GetMaxIntrinsicWidth();
                typography->GetAlphabeticBaseline();
                typography->GetIdeographicBaseline();
                typography->GetLineCount();
                typography->GetGlyphsBoundsTop();
                typography->GetGlyphsBoundsBottom();

                params.operationCount++;
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }
    EXPECT_EQ(params.operationCount.load(), NUM_THREADS * OPERATIONS_PER_THREAD * 10);
}

/**
 * @tc.name: ConcurrentWriteIntensiveTypographyWorkloadTest
 * @tc.desc: test write-heavy concurrent workload for typography
 * @tc.type: FUNC
 */
HWTEST_F(TypographyThreadTest, ConcurrentWriteIntensiveTypographyWorkloadTest, TestSize.Level0)
{
    auto fontCollection = CreateFontCollection();
    Barrier barrier(NUM_THREADS);
    std::vector<std::thread> threads;
    std::atomic<int> operationCount(0);
    ThreadParams params(barrier, operationCount);

    auto workerLambda = [&params, &fontCollection](int i) {
        params.barrier.Wait(); // Synchronize start
        std::unique_ptr<TypographyCreate> builder = TypographyCreate::Create(TypographyStyle(), fontCollection);
        for (int j = 0; j < OPERATIONS_PER_THREAD; ++j) {
            // Write operations: create and layout typography
            std::u16string text =
                u"Write intensive " + std::u16string(1, 'A' + (i % 26)) + u" test " + std::u16string(1, '0' + (j % 10));

            // Add multiple segments with different styles
            for (int k = 0; k < 3; ++k) {
                TextStyle style;
                style.fontSize = 12.0 + k;
                style.color = Drawing::Color::ColorQuadSetARGB(255, k * 80, (k + 1) * 80, (k + 2) * 80);
                builder->PushStyle(style);
                builder->AppendText(text);
            }

            auto typography = builder->CreateTypography();
            typography->Layout(100 + (j % 200));

            params.operationCount++;
        }
    };

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back(workerLambda, i);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(params.operationCount.load(), NUM_THREADS * OPERATIONS_PER_THREAD);
}

} // namespace txt
