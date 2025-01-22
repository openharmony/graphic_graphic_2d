#ifndef DDGR_TEST_DTK_DTK_TEST_EXT_H
#define DDGR_TEST_DTK_DTK_TEST_EXT_H

#include "dtk_test_register.h"

namespace OHOS {
namespace Rosen {
template<typename T>
class EmptyMixin {};

#define DEF_DTK_CLASS(classUniqueId, test_case_name, test_level, test_case_index, mixin)                    \
class Dtk_##classUniqueId : public OHOS::Rosen::TestBase, public mixin<Dtk_##classUniqueId> {     \
public:                                                                                                 \
    Dtk_##classUniqueId() : TestBase() {}                                                            \
    virtual ~Dtk_##classUniqueId() = default;                                                        \
protected:                                                                                              \
    friend class mixin<Dtk_##classUniqueId>;                                                         \
    void OnRecording() override;                                                                        \
};                                                                                                      \
bool DTK_UNIQUE_ID_##classUniqueId =                                                                 \
    OHOS::Rosen::DtkTestRegister::Instance()->regist(#test_case_name,                                \
    []()->std::shared_ptr<OHOS::Rosen::TestBase> {return std::make_shared<Dtk_##classUniqueId>();},  \
    test_level, test_case_index);                                                                       \
void Dtk_##classUniqueId::OnRecording()

#define DEF_DTK_CLASS_UNIQUE_ID(file_line, test_case_name, test_level, test_case_index, mixin) \
DEF_DTK_CLASS(test_case_name##_##file_line, test_case_name, test_level, test_case_index, mixin)

#define DEF_DTK_CLASS_UNIQUE_ID_2(file_line, test_case_name, test_level, test_case_index, mixin) \
DEF_DTK_CLASS_UNIQUE_ID(file_line, test_case_name, test_level, test_case_index, mixin)

#define DEF_DTK(test_case_name, test_level, test_case_index) \
DEF_DTK_CLASS_UNIQUE_ID_2(__LINE__, test_case_name, test_level, test_case_index, EmptyMixin)

#define DEF_DTK_MIXIN(test_case_name, test_level, test_case_index, mixin) \
DEF_DTK_CLASS_UNIQUE_ID_2(__LINE__, test_case_name, test_level, test_case_index, mixin)
} // namespace Rosen
} // namespace OHOS
#endif // DDGR_TEST_DTK_DTK_TEST_EXT_H