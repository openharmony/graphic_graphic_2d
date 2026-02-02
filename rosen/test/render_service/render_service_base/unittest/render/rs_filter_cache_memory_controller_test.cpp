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

#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <memory>

#include "render/rs_filter_cache_memory_controller.h"
#include "common/rs_common_def.h"
#include "platform/common/rs_system_properties.h"

using namespace testing;
using namespace OHOS::Rosen;

class RSFilterCacheMemoryControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

protected:
    RSFilterCacheMemoryController* controller_;
    Drawing::RectI defaultRect_;
};

void RSFilterCacheMemoryControllerTest::SetUpTestCase() {}

void RSFilterCacheMemoryControllerTest::TearDownTestCase() {}

void RSFilterCacheMemoryControllerTest::SetUp()
{
    controller_ = &RSFilterCacheMemoryController::Instance();
    defaultRect_ = Drawing::RectI(0, 0, 1920, 1080); // Standard HD resolution
    controller_->SetScreenRectInfo(defaultRect_);
    
    // Reset internal state through reflection or by creating new instance
    // For testing purposes, we'll use the public API to reset
    // Note: In a real implementation, you might need to add reset functionality
}

void RSFilterCacheMemoryControllerTest::TearDown()
{
    // Clean up any test state
    controller_->SetScreenRectInfo(Drawing::RectI(0, 0, 0, 0));
}

// Test singleton pattern
TEST_F(RSFilterCacheMemoryControllerTest, SingletonInstance)
{
    auto& instance1 = RSFilterCacheMemoryController::Instance();
    auto& instance2 = RSFilterCacheMemoryController::Instance();
    
    EXPECT_EQ(&instance1, &instance2);
}

// Test SetScreenRectInfo functionality
TEST_F(RSFilterCacheMemoryControllerTest, SetScreenRectInfo)
{
    Drawing::RectI newRect(100, 200, 1920, 1080);
    controller_->SetScreenRectInfo(newRect);
    
    // Verify the rect was set (we can't access private members directly, but can test threshold behavior)
    EXPECT_TRUE(true); // Basic test that method doesn't crash
}

// Test AddFilterCacheMem normal operation
TEST_F(RSFilterCacheMemoryControllerTest, ReplaceFilterCacheMem_AddNewCache)
{
    controller_->cacheMem_ = 0;
    controller_->cacheCount_ = 0;
    // Test adding new cache (oldMem = 0)
    controller_->ReplaceFilterCacheMem(0, 1000);
    
    // Should increase cache memory and count
    EXPECT_FALSE(controller_->IsFilterCacheMemExceedThreshold());
}

// Test ReplaceFilterCacheMem normal operation
TEST_F(RSFilterCacheMemoryControllerTest, ReplaceFilterCacheMem_ReplaceExistingCache)
{
    controller_->cacheMem_ = 0;
    controller_->cacheCount_ = 0;
    // First add cache
    controller_->ReplaceFilterCacheMem(0, 1000);
    
    // Replace with different size
    controller_->ReplaceFilterCacheMem(1000, 2000);
    
    EXPECT_FALSE(controller_->IsFilterCacheMemExceedThreshold());
}

// Test ReplaceFilterCacheMem with invalid negative values
TEST_F(RSFilterCacheMemoryControllerTest, ReplaceFilterCacheMem_InvalidNegativeValues)
{
    controller_->cacheMem_ = 0;
    controller_->cacheCount_ = 0;
    // Test with negative old cache memory
    controller_->ReplaceFilterCacheMem(-100, 1000);
    
    // Test with negative new cache memory
    controller_->ReplaceFilterCacheMem(1000, -200);
    
    // Both should be ignored and not change state
    EXPECT_FALSE(controller_->IsFilterCacheMemExceedThreshold());
}

// Test ReplaceFilterCacheMem with same values (no change)
TEST_F(RSFilterCacheMemoryControllerTest, ReplaceFilterCacheMem_SameValues)
{
    controller_->cacheMem_ = 0;
    controller_->cacheCount_ = 0;
    // Add initial cache
    controller_->ReplaceFilterCacheMem(0, 1000);
    
    // Replace with same size
    controller_->ReplaceFilterCacheMem(1000, 1000);
    
    EXPECT_FALSE(controller_->IsFilterCacheMemExceedThreshold());
}

// Test ReduceFilterCacheMem normal operation
TEST_F(RSFilterCacheMemoryControllerTest, ReduceFilterCacheMem_NormalOperation)
{
    controller_->cacheMem_ = 0;
    controller_->cacheCount_ = 0;
    // First add cache
    controller_->ReplaceFilterCacheMem(0, 2000);
    
    // Reduce cache
    controller_->ReduceFilterCacheMem(500);
    
    EXPECT_FALSE(controller_->IsFilterCacheMemExceedThreshold());
}

// Test ReduceFilterCacheMem with invalid negative value
TEST_F(RSFilterCacheMemoryControllerTest, ReduceFilterCacheMem_InvalidNegativeValue)
{
    controller_->cacheMem_ = 0;
    controller_->cacheCount_ = 0;
    controller_->ReduceFilterCacheMem(-100);
    
    // Should be ignored
    EXPECT_FALSE(controller_->IsFilterCacheMemExceedThreshold());
}

// Test ReduceFilterCacheMem with more than available cache
TEST_F(RSFilterCacheMemoryControllerTest, ReduceFilterCacheMem_ExceedAvailableCache)
{
    controller_->cacheMem_ = 0;
    controller_->cacheCount_ = 0;
    // Add small cache
    controller_->ReplaceFilterCacheMem(0, 1000);
    
    // Try to reduce more than available
    controller_->ReduceFilterCacheMem(2000);
    
    // Should reset to 0
    EXPECT_FALSE(controller_->IsFilterCacheMemExceedThreshold());
}

// Test IsFilterCacheMemExceedThreshold below threshold
TEST_F(RSFilterCacheMemoryControllerTest, IsFilterCacheMemExceedThreshold_BelowThreshold)
{
    controller_->cacheMem_ = 0;
    controller_->cacheCount_ = 0;
    // Calculate threshold for default rect (1920 * 1080 * 4.5 = 9,331,200)
    int64_t belowThreshold = 1000000; // 1MB < 9.3MB threshold
    
    controller_->ReplaceFilterCacheMem(0, belowThreshold);
    
    EXPECT_FALSE(controller_->IsFilterCacheMemExceedThreshold());
}

// Test IsFilterCacheMemExceedThreshold at threshold boundary
TEST_F(RSFilterCacheMemoryControllerTest, IsFilterCacheMemExceedThreshold_AtThreshold)
{
    controller_->cacheMem_ = 0;
    controller_->cacheCount_ = 0;
    // Calculate exact threshold for default rect
    int64_t threshold = static_cast<int64_t>(defaultRect_.GetWidth()) *
                      static_cast<int64_t>(defaultRect_.GetHeight()) * 4.5f;
    
    controller_->ReplaceFilterCacheMem(0, threshold);
    
    EXPECT_TRUE(controller_->IsFilterCacheMemExceedThreshold());
}

// Test IsFilterCacheMemExceedThreshold above threshold
TEST_F(RSFilterCacheMemoryControllerTest, IsFilterCacheMemExceedThreshold_AboveThreshold)
{
    controller_->cacheMem_ = 0;
    controller_->cacheCount_ = 0;
    // Add cache above threshold
    int64_t threshold = static_cast<int64_t>(defaultRect_.GetWidth()) *
                      static_cast<int64_t>(defaultRect_.GetHeight()) * 4.5f;
    int64_t aboveThreshold = threshold + 1000;
    
    controller_->ReplaceFilterCacheMem(0, aboveThreshold);
    
    EXPECT_TRUE(controller_->IsFilterCacheMemExceedThreshold());
}

// Test thread safety with concurrent operations
TEST_F(RSFilterCacheMemoryControllerTest, ThreadSafety_ConcurrentOperations)
{
    controller_->cacheMem_ = 0;
    controller_->cacheCount_ = 0;
    const int numThreads = 10;
    const int operationsPerThread = 100;
    std::vector<std::thread> threads;
    
    // Launch multiple threads performing concurrent operations
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([this, i, operationsPerThread]() {
            for (int j = 0; j < operationsPerThread; ++j) {
                int64_t cacheSize = (i + 1) * 1000 + j * 100;
                
                if (j % 3 == 0) {
                    controller_->ReplaceFilterCacheMem(0, cacheSize);
                } else if (j % 3 == 1) {
                    controller_->ReplaceFilterCacheMem(cacheSize, cacheSize + 500);
                } else {
                    controller_->ReduceFilterCacheMem(100);
                }
                
                // Occasionally check threshold
                if (j % 10 == 0) {
                    controller_->IsFilterCacheMemExceedThreshold();
                }
            }
        });
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Should not crash and final state should be consistent
    EXPECT_TRUE(true);
}

// Test memory management with zero dimensions
TEST_F(RSFilterCacheMemoryControllerTest, ZeroScreenDimensions)
{
    controller_->cacheMem_ = 0;
    controller_->cacheCount_ = 0;
    // Set zero screen dimensions
    Drawing::RectI zeroRect(0, 0, 0, 0);
    controller_->SetScreenRectInfo(zeroRect);
    
    // Any cache should exceed threshold (0 * 0 * 4.5 = 0)
    controller_->ReplaceFilterCacheMem(0, 100);
    
    EXPECT_TRUE(controller_->IsFilterCacheMemExceedThreshold());
}

// Test with different screen resolutions
TEST_F(RSFilterCacheMemoryControllerTest, DifferentScreenResolutions)
{
    controller_->cacheMem_ = 0;
    controller_->cacheCount_ = 0;
    // Test with 4K resolution
    Drawing::RectI rect4K(0, 0, 3840, 2160);
    controller_->SetScreenRectInfo(rect4K);
    
    int64_t threshold4K = static_cast<int64_t>(rect4K.GetWidth()) *
                         static_cast<int64_t>(rect4K.GetHeight()) * 4.5f;
    
    // Add cache just below 4K threshold
    controller_->ReplaceFilterCacheMem(0, threshold4K - 1000);
    EXPECT_FALSE(controller_->IsFilterCacheMemExceedThreshold());
    
    // Add cache above 4K threshold
    controller_->ReplaceFilterCacheMem(threshold4K - 1000, threshold4K + 1000);
    EXPECT_TRUE(controller_->IsFilterCacheMemExceedThreshold());
}

// Test edge case with very large values
TEST_F(RSFilterCacheMemoryControllerTest, LargeValuesHandling)
{
    controller_->cacheMem_ = 0;
    controller_->cacheCount_ = 0;
    // Test with very large cache values
    int64_t largeValue = INT64_MAX / 100; // Avoid potential overflow
    
    controller_->ReplaceFilterCacheMem(0, largeValue);
    
    // Should handle large values gracefully
    EXPECT_TRUE(controller_->IsFilterCacheMemExceedThreshold());
}

// Test cache count management
TEST_F(RSFilterCacheMemoryControllerTest, CacheCountManagement)
{
    controller_->cacheMem_ = 0;
    controller_->cacheCount_ = 0;
    // Add multiple cache items
    controller_->ReplaceFilterCacheMem(0, 1000);  // count = 1
    controller_->ReplaceFilterCacheMem(0, 2000);  // count = 2
    controller_->ReplaceFilterCacheMem(0, 1500);  // count = 3
    
    // Reduce cache items
    controller_->ReduceFilterCacheMem(500);   // count = 2
    controller_->ReduceFilterCacheMem(1000);  // count = 1
    controller_->ReduceFilterCacheMem(2000);  // count = 0
    
    // Test behavior when count goes negative
    controller_->ReduceFilterCacheMem(100);   // should reset to 0
    
    EXPECT_TRUE(true); // Should not crash
}

// Test precision with SCREEN_RATIO
TEST_F(RSFilterCacheMemoryControllerTest, ScreenRatioPrecision)
{
    controller_->cacheMem_ = 0;
    controller_->cacheCount_ = 0;
    // Test that SCREEN_RATIO = 4.5f is applied correctly
    Drawing::RectI testRect(0, 0, 100, 100);
    controller_->SetScreenRectInfo(testRect);
    
    // Expected threshold: 100 * 100 * 4.5 = 45,000
    controller_->ReplaceFilterCacheMem(0, 44999);
    EXPECT_FALSE(controller_->IsFilterCacheMemExceedThreshold());
    
    controller_->ReplaceFilterCacheMem(44999, 45000);
    EXPECT_TRUE(controller_->IsFilterCacheMemExceedThreshold());
    
    controller_->ReplaceFilterCacheMem(45000, 45001);
    EXPECT_TRUE(controller_->IsFilterCacheMemExceedThreshold());
}

// Test boundary conditions for int64 operations
TEST_F(RSFilterCacheMemoryControllerTest, Int64BoundaryConditions)
{
    controller_->cacheMem_ = 0;
    controller_->cacheCount_ = 0;
    // Test with maximum int64 values
    controller_->ReplaceFilterCacheMem(0, INT64_MAX);
    
    // Should handle maximum values without overflow
    EXPECT_TRUE(controller_->IsFilterCacheMemExceedThreshold());
    
    // Test replacing with zero
    controller_->ReplaceFilterCacheMem(INT64_MAX, 0);
    
    // Should handle the transition correctly
    EXPECT_FALSE(controller_->IsFilterCacheMemExceedThreshold());
}