/*
 * Test Scope: Cover RSLayerPropertyBase operator overloads on nullptr.
 * Purpose: Ensure operations on nullptr do not crash and return defaults.
 */

#include <gtest/gtest.h>
#include "rs_layer_cmd_property.h"

using namespace OHOS::Rosen;
using namespace testing;
using namespace testing::ext;
class RSLayerCmdPropertyTest : public Test {};

/*
 * Test: Operators_OnNullptr_NoCrash
 * Purpose: Verify operator overloads on nullptr return safe defaults.
 * Steps: Apply +=, -=, *=, +, -, *; compare == and != with nullptr.
 * Expected: Returns null or false; no crash.
 */
HWTEST(RSLayerCmdPropertyTest, Operators_OnNullptr_NoCrash, TestSize.Level1)
{
    std::shared_ptr<RSLayerPropertyBase> a;
    std::shared_ptr<const RSLayerPropertyBase> b;

    auto r1 = a += b;
    auto r2 = a -= b;
    auto r3 = a *= 1.0f;
    auto r4 = std::shared_ptr<const RSLayerPropertyBase>(nullptr) + b;
    auto r5 = std::shared_ptr<const RSLayerPropertyBase>(nullptr) - b;
    auto r6 = std::shared_ptr<const RSLayerPropertyBase>(nullptr) * 1.0f;

    EXPECT_EQ(r1, nullptr);
    EXPECT_EQ(r2, nullptr);
    EXPECT_EQ(r3, nullptr);
    EXPECT_EQ(r4, nullptr);
    EXPECT_EQ(r5, nullptr);
    EXPECT_EQ(r6, nullptr);

    bool eq = (std::shared_ptr<const RSLayerPropertyBase>(nullptr) == b);
    bool ne = (std::shared_ptr<const RSLayerPropertyBase>(nullptr) != b);
    EXPECT_FALSE(eq);
    EXPECT_FALSE(ne);
}
