//
// Created by Junchen Liu on 4/26/2022.
//

#ifndef TEMPLATEDB_TIMEUTIL_H
#define TEMPLATEDB_TIMEUTIL_H

#include <chrono>
namespace TimeUtil {
    int64_t getCurrentLocalTimeStamp()
    {
        std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
        auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
        return tmp.count();

        // return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }
}


#endif //TEMPLATEDB_TIMEUTIL_H
