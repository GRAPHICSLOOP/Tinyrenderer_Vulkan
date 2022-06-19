#include "ObjectIDAllocator.h"
#include <limits>
#include "core/base/macro.h"

std::atomic<tiny::ObjectID> tiny::ObjectIDAllocator::mNextID = 0;

tiny::ObjectID tiny::ObjectIDAllocator::Alloc()
{
    std::atomic<ObjectID> newObjectRet = mNextID.load();
    mNextID++;
    if (mNextID >= std::numeric_limits<std::size_t>::max())
    {
        TINYLOG_ERROR("object id overflow");
    }

    return newObjectRet;
}
