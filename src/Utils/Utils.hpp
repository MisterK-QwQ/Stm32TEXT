#pragma once

#define _Dog
#define _Log

#include <cstdint>
#include <cstring>
#include <cmath>
#include "Math.hpp"
#include "stm32f1xx_hal.h"
#include "Time.hpp"
#include "TimerUtil.hpp"

class Utils {
public:
 /**
     * @brief 计算两个时间的差值（秒）
     * @param t1 结束时间
     * @param t2 开始时间
     * @return 差值（t1 - t2，单位：秒），t1 < t2则返回负数
     */
    static int64_t timeDiff(const Time& t1, const Time& t2) {
        return (int64_t)t1.toUnixTimestamp() - (int64_t)t2.toUnixTimestamp();
    }

     /**
     * @brief 格式化Unix时间戳为字符串（调用Time类）
     * @param timestamp Unix时间戳（秒）
     * @param buf 输出缓冲区
     * @param fmt 格式字符串（同Time::format）
     * @return 是否成功
     */
    static bool formatTimestamp(uint64_t timestamp, char* buf, const char* fmt = "%YYYY-%MM-%DD %HH:%mm:%ss") {
        if (buf == nullptr) return false;
        Time t;
        if (!t.fromUnixTimestamp(timestamp)) return false;
        t.format(buf, fmt);
        return true;
    }
    
};