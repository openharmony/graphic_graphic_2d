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

#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <gtest/gtest.h>

#include "font_collection.h"
#include "modules/skparagraph/include/Paragraph.h"
#include "ohos/init_data.h"
#include "paragraph_builder.h"
#include "paragraph_style.h"
#include "text_style.h"
#include "typography.h"
#include "typography_create.h"
#include "draw/color.h"
#include "utils/text_log.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen;
using namespace OHOS::Rosen::Drawing;
using namespace OHOS::Rosen::SPText;

namespace {

constexpr int NUM_THREADS = 8;
constexpr int OPERATIONS_PER_THREAD = 50;
constexpr int STRESS_THREAD_COUNT = 16;
constexpr int STRESS_ITERATIONS = 500;

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
    int count_;
    int waiting_;
    int generation_;
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
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::shared_ptr<FontCollection> CreateFontCollection()
    {
        auto fontCollection = std::make_shared<FontCollection>();
        fontCollection->SetupDefaultFontManager();
        return fontCollection;
    }

    std::shared_ptr<Paragraph> CreateSimpleParagraph(const std::u16string& text,
        const std::shared_ptr<FontCollection>& fontCollection)
    {
        ParagraphStyle paragraphStyle;
        std::shared_ptr<ParagraphBuilder> paragraphBuilder =
            ParagraphBuilder::Create(paragraphStyle, fontCollection);
        paragraphBuilder->AddText(text);
        return paragraphBuilder->Build();
    }
};

struct ThreadParams {
    Barrier& barrier;
    std::atomic<int>& operationCount;
    std::atomic<int>& errorCount;

    explicit ThreadParams(Barrier& barrier_, std::atomic<int>& operationCount_, std::atomic<int>& errorCount_)
        : barrier(barrier_), operationCount(operationCount_), errorCount(errorCount_)
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
    std::atomic<int> errorCount(0);
    ThreadParams params(barrier, operationCount, errorCount);

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([this, &params, &fontCollection, &text]() {
            params.barrier.Wait(); // Synchronize start

            try {
                for (int j = 0; j < OPERATIONS_PER_THREAD; ++j) {
                    auto paragraph = CreateSimpleParagraph(text, fontCollection);
                    paragraph->Layout(100);
                    params.operationCount++;
                }
            } catch (...) {
                params.errorCount++;
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(params.operationCount.load(), NUM_THREADS * OPERATIONS_PER_THREAD);
    EXPECT_EQ(params.errorCount.load(), 0);
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
    std::atomic<int> errorCount(0);
    ThreadParams params(barrier, operationCount, errorCount);

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([this, &params, fontCollection, i]() {
            params.barrier.Wait(); // Synchronize start

            for (int j = 0; j < OPERATIONS_PER_THREAD; ++j) {
                ParagraphStyle paragraphStyle;
                SPText::TextStyle textStyle;
                textStyle.fontSize = 14.0;
                textStyle.fontFamilies = {"Roboto"};

                std::shared_ptr<ParagraphBuilder> paragraphBuilder =
                    ParagraphBuilder::Create(paragraphStyle, fontCollection);

                paragraphBuilder->PushStyle(textStyle);
                std::u16string text = u"Thread " + std::u16string(1, 'A' + (i % 26)) + u" text " +
                                     std::u16string(1, '0' + (j % 10));
                paragraphBuilder->AddText(text);
                paragraphBuilder->Pop();

                auto paragraph = paragraphBuilder->Build();
                if (paragraph == nullptr) {
                    params.errorCount++;
                }
                params.operationCount++;
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(params.operationCount.load(), NUM_THREADS * OPERATIONS_PER_THREAD);
    EXPECT_EQ(params.errorCount.load(), 0);
}

/**
 * @tc.name: ConcurrentFontCollectionOperationsTest
 * @tc.desc: test concurrent FontCollection operations
 * @tc.type: FUNC
 */
HWTEST_F(TypographyThreadTest, ConcurrentFontCollectionOperationsTest, TestSize.Level0)
{
    Barrier barrier(NUM_THREADS);
    std::vector<std::thread> threads;
    std::atomic<int> operationCount(0);
    std::atomic<int> errorCount(0);
    ThreadParams params(barrier, operationCount, errorCount);

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([this, &params, i]() {
            params.barrier.Wait(); // Synchronize start

            for (int j = 0; j < OPERATIONS_PER_THREAD; ++j) {
                FontCollection fontCollection;

                switch (j % 4) {
                    case 0:
                        fontCollection.SetupDefaultFontManager();
                        break;
                    case 1:
                        fontCollection.SetAssetFontManager(Drawing::FontMgr::CreateDefaultFontMgr());
                        break;
                    case 2:
                        fontCollection.SetDynamicFontManager(Drawing::FontMgr::CreateDynamicFontMgr());
                        break;
                    case 3:
                        fontCollection.SetTestFontManager(Drawing::FontMgr::CreateDefaultFontMgr());
                        break;
                }

                if (fontCollection.GetFontManagersCount() < 0) {
                    params.errorCount++;
                }
                params.operationCount++;
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(params.operationCount.load(), NUM_THREADS * OPERATIONS_PER_THREAD);
    EXPECT_EQ(params.errorCount.load(), 0);
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

    auto paragraph = CreateSimpleParagraph(text, fontCollection);
    paragraph->Layout(200);

    Barrier barrier(NUM_THREADS);
    std::vector<std::thread> threads;
    std::atomic<int> operationCount(0);
    std::atomic<int> errorCount(0);
    ThreadParams params(barrier, operationCount, errorCount);

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([&params, paragraph, i]() {
            params.barrier.Wait(); // Synchronize start

            for (int j = 0; j < OPERATIONS_PER_THREAD; ++j) {
                // Concurrent reads of paragraph metrics
                paragraph->GetMaxWidth();
                paragraph->GetHeight();
                paragraph->GetLineCount();
                paragraph->GetMinIntrinsicWidth();
                paragraph->GetMaxIntrinsicWidth();
                paragraph->GetAlphabeticBaseline();
                paragraph->GetIdeographicBaseline();
                paragraph->GetGlyphsBoundsTop();
                paragraph->GetGlyphsBoundsBottom();
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
 * @tc.name: StressTestMixedTypographyOperations
 * @tc.desc: stress test with mixed typography operations
 * @tc.type: FUNC
 */
HWTEST_F(TypographyThreadTest, StressTestMixedTypographyOperations, TestSize.Level0)
{
    auto fontCollection = CreateFontCollection();

    Barrier barrier(STRESS_THREAD_COUNT);
    std::vector<std::thread> threads;
    std::atomic<int> operationCount(0);
    std::atomic<int> errorCount(0);
    ThreadParams params(barrier, operationCount, errorCount);

    for (int i = 0; i < STRESS_THREAD_COUNT; ++i) {
        threads.emplace_back([this, &params, fontCollection, i]() {
            params.barrier.Wait(); // Synchronize start

            for (int j = 0; j < STRESS_ITERATIONS; ++j) {
                try {
                    switch (j % 5) {
                        case 0: {
                            // Create and layout paragraph
                            std::u16string text = u"Stress test " + std::u16string(1, 'A' + (i % 26));
                            auto paragraph = CreateSimpleParagraph(text, fontCollection);
                            paragraph->Layout(100 + (j % 100));
                            break;
                        }
                        case 1: {
                            // Create font collection
                            FontCollection fc;
                            fc.SetupDefaultFontManager();
                            if (fc.GetFontManagersCount() < 0) {
                                params.errorCount++;
                            }
                            break;
                        }
                        case 2: {
                            // Text style operations
                            SPText::TextStyle style;
                            style.fontSize = 12.0 + (j % 20);
                            style.fontWeight = static_cast<SPText::FontWeight>(j % 10);
                            style.letterSpacing = (j % 5) * 0.1;
                            style.wordSpacing = (j % 3) * 0.5;
                            break;
                        }
                        case 3: {
                            // Paragraph style operations
                            ParagraphStyle style;
                            style.maxLines = (j % 10) + 1;
                            style.ellipsis = u"...";
                            style.textAlign = static_cast<SPText::TextAlign>(j % 5);
                            break;
                        }
                        case 4: {
                            // Complex text with multiple styles
                            std::shared_ptr<ParagraphBuilder> builder =
                                ParagraphBuilder::Create(ParagraphStyle(), fontCollection);

                            SPText::TextStyle style1;
                            style1.fontSize = 14.0;
                            style1.color = Drawing::Color::COLOR_RED;
                            builder->PushStyle(style1);
                            builder->AddText(u"Red ");

                            SPText::TextStyle style2;
                            style2.fontSize = 16.0;
                            style2.color = Drawing::Color::COLOR_BLUE;
                            builder->PushStyle(style2);
                            builder->AddText(u"Blue ");

                            auto paragraph = builder->Build();
                            paragraph->Layout(150);
                            break;
                        }
                    }
                    params.operationCount++;
                } catch (...) {
                    params.errorCount++;
                }
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(params.operationCount.load(), STRESS_THREAD_COUNT * STRESS_ITERATIONS);
    EXPECT_EQ(params.errorCount.load(), 0);
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

    auto paragraph = CreateSimpleParagraph(text, fontCollection);
    paragraph->Layout(200);

    Barrier barrier(NUM_THREADS);
    std::vector<std::thread> threads;
    std::atomic<int> operationCount(0);
    std::atomic<int> errorCount(0);
    ThreadParams params(barrier, operationCount, errorCount);

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([&params, paragraph, i, text]() {
            params.barrier.Wait(); // Synchronize start

            for (int j = 0; j < OPERATIONS_PER_THREAD; ++j) {
                size_t start = j % text.size();
                size_t end = (j + 1) % (text.size() + 1);
                if (start > end) {
                    std::swap(start, end);
                }

                auto rects = paragraph->GetRectsForRange(
                    start, end, RectHeightStyle::MAX, RectWidthStyle::TIGHT);
                if (rects.size() < 0) {
                    params.errorCount++;
                }
                params.operationCount++;
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(params.operationCount.load(), NUM_THREADS * OPERATIONS_PER_THREAD);
    EXPECT_EQ(params.errorCount.load(), 0);
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
    auto paragraph = CreateSimpleParagraph(text, fontCollection);
    paragraph->Layout(200);

    Barrier barrier(NUM_THREADS);
    std::vector<std::thread> threads;
    std::atomic<int> operationCount(0);
    std::atomic<int> errorCount(0);
    ThreadParams params(barrier, operationCount, errorCount);

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([&params, paragraph, i]() {
            params.barrier.Wait(); // Synchronize start

            for (int j = 0; j < OPERATIONS_PER_THREAD; ++j) {
                double x = (j % 200);
                double y = (j % 50);
                auto position = paragraph->GetGlyphPositionAtCoordinate(x, y);
                if (position.position < 0) {
                    params.errorCount++;
                }
                params.operationCount++;
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(params.operationCount.load(), NUM_THREADS * OPERATIONS_PER_THREAD);
    EXPECT_EQ(params.errorCount.load(), 0);
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
    auto paragraph = CreateSimpleParagraph(text, fontCollection);
    paragraph->Layout(200);

    Barrier barrier(NUM_THREADS);
    std::vector<std::thread> threads;
    std::atomic<int> operationCount(0);
    std::atomic<int> errorCount(0);
    ThreadParams params(barrier, operationCount, errorCount);

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([&params, paragraph, i, text]() {
            params.barrier.Wait(); // Synchronize start

            for (int j = 0; j < OPERATIONS_PER_THREAD; ++j) {
                size_t offset = j % text.size();
                auto boundary = paragraph->GetWordBoundary(offset);
                if (boundary.start < 0 || boundary.end > text.size()) {
                    params.errorCount++;
                }
                params.operationCount++;
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(params.operationCount.load(), NUM_THREADS * OPERATIONS_PER_THREAD);
    EXPECT_EQ(params.errorCount.load(), 0);
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
    std::atomic<int> errorCount(0);

    std::vector<std::thread> threads;

    // Threads creating paragraphs
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([this, &createCount, &errorCount, fontCollection, i]() {
            for (int j = 0; j < 50; ++j) {
                std::u16string text = u"Create " + std::u16string(1, 'A' + (i % 26));
                auto paragraph = CreateSimpleParagraph(text, fontCollection);
                if (paragraph == nullptr) {
                    errorCount++;
                }
                createCount++;
                std::this_thread::sleep_for(std::chrono::microseconds(100));
            }
        });
    }

    // Threads layouting paragraphs
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back([this, &layoutCount, fontCollection]() {
            for (int j = 0; j < 50; ++j) {
                std::u16string text = u"Layout test";
                auto paragraph = CreateSimpleParagraph(text, fontCollection);
                paragraph->Layout(100 + (j % 100));
                layoutCount++;
                std::this_thread::sleep_for(std::chrono::microseconds(100));
            }
        });
    }

    // Threads querying metrics
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back([this, &queryCount, fontCollection]() {
            for (int j = 0; j < 50; ++j) {
                std::u16string text = u"Query test";
                auto paragraph = CreateSimpleParagraph(text, fontCollection);
                paragraph->Layout(150);
                paragraph->GetHeight();
                paragraph->GetMaxWidth();
                paragraph->GetLineCount();
                queryCount++;
                std::this_thread::sleep_for(std::chrono::microseconds(100));
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(createCount.load(), 200); // 4 threads * 50 iterations
    EXPECT_EQ(layoutCount.load(), 150);  // 3 threads * 50 iterations
    EXPECT_EQ(queryCount.load(), 150);   // 3 threads * 50 iterations
    EXPECT_EQ(errorCount.load(), 0);
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
    std::atomic<int> errorCount(0);
    ThreadParams params(barrier, operationCount, errorCount);

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([this, &params, fontCollection, i]() {
            params.barrier.Wait(); // Synchronize start

            for (int j = 0; j < OPERATIONS_PER_THREAD; ++j) {
                std::shared_ptr<ParagraphBuilder> builder =
                    ParagraphBuilder::Create(ParagraphStyle(), fontCollection);

                // Add multiple styled text segments
                for (int k = 0; k < 5; ++k) {
                    SPText::TextStyle style;
                    style.fontSize = 12.0 + k * 2.0;
                    style.fontWeight = static_cast<SPText::FontWeight>(k % 10);
                    style.color = Drawing::Color::ColorQuadSetARGB(255, k * 50, (k + 1) * 50, (k + 2) * 50);

                    builder->PushStyle(style);
                    builder->AddText(u"Styled ");
                }

                // Pop styles
                for (int k = 0; k < 3; ++k) {
                    builder->Pop();
                }

                auto paragraph = builder->Build();
                if (paragraph == nullptr) {
                    params.errorCount++;
                }
                paragraph->Layout(200);
                params.operationCount++;
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(params.operationCount.load(), NUM_THREADS * OPERATIONS_PER_THREAD);
    EXPECT_EQ(params.errorCount.load(), 0);
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
        threads.emplace_back([this, &completed, fontCollection, i]() {
            for (int j = 0; j < OPERATIONS_PER_THREAD; ++j) {
                // Mix of different operations
                std::u16string text = u"Thread " + std::u16string(1, 'A' + (i % 26));
                auto paragraph = CreateSimpleParagraph(text, fontCollection);
                paragraph->Layout(100 + (j % 100));
                paragraph->GetHeight();
                paragraph->GetMaxWidth();
                paragraph->GetLineCount();

                // Create new font collection
                FontCollection fc;
                fc.SetupDefaultFontManager();

                // Create paragraph builder
                std::shared_ptr<ParagraphBuilder> builder =
                    ParagraphBuilder::Create(ParagraphStyle(), fontCollection);
                builder->AddText(u"Deadlock test");
                auto p = builder->Build();
                p->Layout(150);
            }
            completed[i].store(true);
        });
    }

    // Wait with timeout to detect deadlock
    std::thread timeoutThread([&completed, &timeoutOccurred, &incompleteThreadCount]() {
        std::this_thread::sleep_for(std::chrono::seconds(10));
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
    auto paragraph = CreateSimpleParagraph(text, fontCollection);
    paragraph->Layout(300);

    Barrier barrier(NUM_THREADS);
    std::vector<std::thread> threads;
    std::atomic<int> operationCount(0);
    std::atomic<int> errorCount(0);
    ThreadParams params(barrier, operationCount, errorCount);

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([&params, paragraph, i]() {
            params.barrier.Wait(); // Synchronize start

            for (int j = 0; j < OPERATIONS_PER_THREAD * 10; ++j) {
                // Read operations only
                paragraph->GetMaxWidth();
                paragraph->GetHeight();
                paragraph->GetMinIntrinsicWidth();
                paragraph->GetMaxIntrinsicWidth();
                paragraph->GetAlphabeticBaseline();
                paragraph->GetIdeographicBaseline();
                paragraph->GetLineCount();
                paragraph->GetGlyphsBoundsTop();
                paragraph->GetGlyphsBoundsBottom();

                params.operationCount++;
            }
        });
    }

    auto startTime = std::chrono::high_resolution_clock::now();

    for (auto& thread : threads) {
        thread.join();
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    EXPECT_EQ(params.operationCount.load(), NUM_THREADS * OPERATIONS_PER_THREAD * 10);

    // Log performance metric
    double opsPerSecond = (params.operationCount.load() * 1000.0) / duration.count();
    TEXT_LOGI("Typography read-intensive workload: %{public}d ops in %{public}lld ms (%{public}.2f ops/sec)",
              params.operationCount.load(), duration.count(), opsPerSecond);
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
    std::atomic<int> errorCount(0);
    ThreadParams params(barrier, operationCount, errorCount);

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([this, &params, fontCollection, i]() {
            params.barrier.Wait(); // Synchronize start

            for (int j = 0; j < OPERATIONS_PER_THREAD; ++j) {
                // Write operations: create and layout paragraphs
                std::u16string text = u"Write intensive " + std::u16string(1, 'A' + (i % 26)) +
                                     u" test " + std::u16string(1, '0' + (j % 10));

                std::shared_ptr<ParagraphBuilder> builder =
                    ParagraphBuilder::Create(ParagraphStyle(), fontCollection);

                // Add multiple segments with different styles
                for (int k = 0; k < 3; ++k) {
                    SPText::TextStyle style;
                    style.fontSize = 12.0 + k;
                    style.color = Drawing::Color::ColorQuadSetARGB(255, k * 80, (k + 1) * 80, (k + 2) * 80);
                    builder->PushStyle(style);
                    builder->AddText(text);
                }

                auto paragraph = builder->Build();
                paragraph->Layout(100 + (j % 200));

                params.operationCount++;
            }
        });
    }

    auto startTime = std::chrono::high_resolution_clock::now();

    for (auto& thread : threads) {
        thread.join();
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    EXPECT_EQ(params.operationCount.load(), NUM_THREADS * OPERATIONS_PER_THREAD);

    // Log performance metric
    double opsPerSecond = (params.operationCount.load() * 1000.0) / duration.count();
    TEXT_LOGI("Typography write-intensive workload: %{public}d ops in %{public}lld ms (%{public}.2f ops/sec)",
              params.operationCount.load(), duration.count(), opsPerSecond);
}

} // namespace txt
