#include "Status.h"
#include <typeinfo>

bool const operator==(const Status &a, const Status &b)
{
    return typeid(a).name() == typeid(b).name();
}
