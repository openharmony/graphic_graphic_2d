#include <gtest/gtest.h>
#include "rs_layer_parcel_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSLayerParcelFactoryTest : public Test {};

/**
 * Function: Register_And_Get_Function_LUT
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. register a dummy unmarshalling func
 *                  2. get it back and compare pointer
 */
HWTEST_F(RSLayerParcelFactoryTest, Register_And_Get_Function_LUT, TestSize.Level1)
{
    uint16_t key = 0x1234;
    UnmarshallingFunc func = [](OHOS::MessageParcel&) { return static_cast<RSLayerParcel*>(nullptr); };
    RSLayerParcelFactory::Instance().Register(key, func);
    auto got = RSLayerParcelFactory::Instance().GetUnmarshallingFunc(key);
    ASSERT_NE(got, nullptr);
}

/**
 * Function: Get_Unregistered_Key_Returns_Null
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. query unknown key
 *                  2. expect nullptr
 */
HWTEST_F(RSLayerParcelFactoryTest, Get_Unregistered_Key_Returns_Null, TestSize.Level1)
{
    auto got = RSLayerParcelFactory::Instance().GetUnmarshallingFunc(0xFFFF);
    EXPECT_EQ(got, nullptr);
}
} // namespace OHOS::Rosen
