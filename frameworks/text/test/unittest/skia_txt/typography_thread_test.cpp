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
#include "font_collection.h"
#include "modules/skparagraph/include/Paragraph.h"
#include "ohos/init_data.h"
#include "paragraph_builder.h"
#include "paragraph_style.h"
#include "text_style.h"
#include "typography.h"
#include "typography_create.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen;
using namespace OHOS::Rosen::Drawing;
using namespace OHOS::Rosen::SPText;

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
                auto paragraph = CreateSimpleParagraph(text, fontCollection);
                paragraph->Layout(100);
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
                ParagraphStyle paragraphStyle;
                SPText::TextStyle textStyle;
                textStyle.fontSize = 14.0;
                textStyle.fontFamilies = {"Roboto"};

                std::shared_ptr<ParagraphBuilder> paragraphBuilder =
                    ParagraphBuilder::Create(paragraphStyle, fontCollection);

                paragraphBuilder->PushStyle(textStyle);
                std::u16string text =
                    u"Thread " + std::u16string(1, 'A' + (i % 26)) + u" text " + std::u16string(1, '0' + (j % 10));
                paragraphBuilder->AddText(text);
                paragraphBuilder->Pop();

                auto paragraph = paragraphBuilder->Build();
                EXPECT_NE(paragraph, nullptr);
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
 * @tc.name: ConcurrentFontCollectionOperationsTest
 * @tc.desc: test concurrent FontCollection operations
 * @tc.type: FUNC
 */
HWTEST_F(TypographyThreadTest, ConcurrentFontCollectionOperationsTest, TestSize.Level0)
{
    Barrier barrier(NUM_THREADS);
    std::vector<std::thread> threads;
    std::atomic<int> operationCount(0);
    ThreadParams params(barrier, operationCount);

    const auto workerLambda = [&params]() {
        params.barrier.Wait(); // Synchronize start

        for (int j = 0; j < OPERATIONS_PER_THREAD; ++j) {
            FontCollection fontCollection;

            switch (j % 5) {
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
                case 4:
                    fontCollection.SetGlobalFontManager(Drawing::FontMgr::CreateDefaultFontMgr());
                    break;
                default:
                    break;
            }
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
    ThreadParams params(barrier, operationCount);

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([&params, paragraph]() {
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
    ThreadParams params(barrier, operationCount);

    const auto workerLambda = [&params, paragraph, text]() {
        params.barrier.Wait(); // Synchronize start

        for (int j = 0; j < OPERATIONS_PER_THREAD; ++j) {
            size_t start = j % text.size();
            size_t end = (j + 1) % (text.size() + 1);
            if (start > end) {
                std::swap(start, end);
            }

            auto rects = paragraph->GetRectsForRange(start, end, RectHeightStyle::MAX, RectWidthStyle::TIGHT);
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
    auto paragraph = CreateSimpleParagraph(text, fontCollection);
    paragraph->Layout(200);

    Barrier barrier(NUM_THREADS);
    std::vector<std::thread> threads;
    std::atomic<int> operationCount(0);
    ThreadParams params(barrier, operationCount);

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([&params, paragraph]() {
            params.barrier.Wait(); // Synchronize start

            for (int j = 0; j < OPERATIONS_PER_THREAD; ++j) {
                double x = (j % 200);
                double y = (j % 50);
                paragraph->GetGlyphPositionAtCoordinate(x, y);
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
    auto paragraph = CreateSimpleParagraph(text, fontCollection);
    paragraph->Layout(200);

    Barrier barrier(NUM_THREADS);
    std::vector<std::thread> threads;
    std::atomic<int> operationCount(0);
    ThreadParams params(barrier, operationCount);

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([&params, paragraph, text]() {
            params.barrier.Wait(); // Synchronize start

            for (int j = 0; j < OPERATIONS_PER_THREAD; ++j) {
                size_t offset = j % text.size();
                paragraph->GetWordBoundary(offset);
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

    // Threads creating paragraphs
    for (int i = 0; i < NUM_CREATE_THREADS; ++i) {
        threads.emplace_back([this, &barrier, &createCount, &fontCollection, i]() {
            barrier.Wait(); // Synchronize start
            for (int j = 0; j < 50; ++j) {
                std::u16string text = u"Create " + std::u16string(1, 'A' + (i % 26));
                auto paragraph = CreateSimpleParagraph(text, fontCollection);
                EXPECT_NE(paragraph, nullptr);
                createCount++;
            }
        });
    }

    // Threads layouting paragraphs
    for (int i = 0; i < NUM_LAYOUT_THREADS; ++i) {
        threads.emplace_back([this, &barrier, &layoutCount, fontCollection]() {
            barrier.Wait(); // Synchronize start
            for (int j = 0; j < 50; ++j) {
                std::u16string text = u"Layout test";
                auto paragraph = CreateSimpleParagraph(text, fontCollection);
                paragraph->Layout(100 + (j % 100));
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
                auto paragraph = CreateSimpleParagraph(text, fontCollection);
                paragraph->Layout(150);
                paragraph->GetHeight();
                paragraph->GetMaxWidth();
                paragraph->GetLineCount();
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
        std::shared_ptr<ParagraphBuilder> builder = ParagraphBuilder::Create(ParagraphStyle(), fontCollection);
        for (int j = 0; j < OPERATIONS_PER_THREAD; ++j) {
            // Add multiple styled text segments
            for (int k = 0; k < 5; ++k) {
                SPText::TextStyle style;
                style.fontSize = 12.0 + k * 2.0;
                style.fontWeight = static_cast<SPText::FontWeight>(k % 10);
                style.color = Drawing::Color::ColorQuadSetARGB(255, k * 30, (k + 1) * 30, (k + 2) * 30);

                builder->PushStyle(style);
                builder->AddText(u"Styled ");
            }

            // Pop styles
            for (int k = 0; k < 3; ++k) {
                builder->Pop();
            }

            auto paragraph = builder->Build();
            EXPECT_NE(paragraph, nullptr);
            paragraph->Layout(200);
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
    auto paragraph = CreateSimpleParagraph(text, fontCollection);
    paragraph->Layout(300);

    Barrier barrier(NUM_THREADS);
    std::vector<std::thread> threads;
    std::atomic<int> operationCount(0);
    ThreadParams params(barrier, operationCount);

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([&params, &paragraph]() {
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
        std::shared_ptr<ParagraphBuilder> builder = ParagraphBuilder::Create(ParagraphStyle(), fontCollection);
        for (int j = 0; j < OPERATIONS_PER_THREAD; ++j) {
            // Write operations: create and layout paragraphs
            std::u16string text =
                u"Write intensive " + std::u16string(1, 'A' + (i % 26)) + u" test " + std::u16string(1, '0' + (j % 10));

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
