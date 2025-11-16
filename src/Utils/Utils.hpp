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
#define MIN2(a, b) ((a) < (b) ? (a) : (b))
#define MIN3(a, b, c) MIN2(MIN2(a, b), c)
#define MIN4(a, b, c, d) MIN2(MIN3(a, b, c), d)

#define MIN(...) ({ \
    __typeof__((__VA_ARGS__)) _arr[] = {__VA_ARGS__}; \
    __typeof__((__VA_ARGS__)) _min = _arr[0]; \
    for (size_t i = 1; i < sizeof(_arr)/sizeof(_arr[0]); i++) { \
        if (_arr[i] < _min) _min = _arr[i]; \
    } \
    _min; \
})

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
    
    static inline void HAL_Delay_us(uint32_t us) {
        uint32_t ticks = us * (SystemCoreClock / 1000000);
        uint32_t start = SysTick->VAL;
        while ((SysTick->VAL - start + (1 << 24)) % (1 << 24) < ticks) {
            // 空循环等待，无中断依赖
        }
    }
};