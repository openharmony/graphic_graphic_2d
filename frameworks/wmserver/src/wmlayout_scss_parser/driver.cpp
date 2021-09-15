#include <iostream>

#include "driver.h"

namespace OHOS {
Driver::Driver() :
    parser(scanner, *this), scanner(*this)
{
}

int Driver::parse(std::istream &infile)
{
    scanner.switch_streams(infile, std::cout);
    return parser.parse();
}
} // namespace OHOS
