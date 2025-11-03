#pragma once
#include <stdint.h>
#include <cstring>
#include <cmath>

class Time {
private:
    uint16_t year;    // 年份（1970~9999）
    uint8_t  month;   // 月份（1~12）
    uint8_t  day;     // 日期（1~31，随月份变化）
    uint8_t  hour;    // 小时（0~23）
    uint8_t  minute;  // 分钟（0~59）
    uint8_t  second;  // 秒（0~59）
    int8_t   timezone; // 时区偏移（-12~+12，相对于UTC）

    // 辅助函数：判断闰年
    bool isLeapYear(uint16_t y) const {
        if (y % 4 != 0) return false;
        if (y % 100 != 0) return true;
        return (y % 400 == 0);
    }

    // 辅助函数：获取某月的天数
    uint8_t getDaysInMonth(uint16_t y, uint8_t m) const {
        const uint8_t days[] = {31,28,31,30,31,30,31,31,30,31,30,31};
        if (m == 2 && isLeapYear(y)) return 29;
        return days[m-1]; // m为1~12
    }

    // 辅助函数：检查时间是否有效（用于初始化和修改）
    bool isValidTime(uint16_t y, uint8_t m, uint8_t d, 
                    uint8_t h, uint8_t min, uint8_t s) const {
        if (y < 1970 || y > 9999) return false;
        if (m < 1 || m > 12) return false;
        if (d < 1 || d > getDaysInMonth(y, m)) return false;
        if (h > 23 || min > 59 || s > 59) return false;
        return true;
    }

public:
    Time(); // 默认构造（1970-01-01 00:00:00 UTC）
    Time(uint16_t y, uint8_t m, uint8_t d, 
         uint8_t h=0, uint8_t min=0, uint8_t s=0, 
         int8_t tz=0); // 带参数构造

    // 基本功能：获取时间分量
    uint16_t getYear() const { return year; }
    uint8_t getMonth() const { return month; }
    uint8_t getDay() const { return day; }
    uint8_t getHour() const { return hour; }
    uint8_t getMinute() const { return minute; }
    uint8_t getSecond() const { return second; }
    int8_t getTimezone() const { return timezone; }

    // 基本功能：设置时间（返回是否设置成功）
    bool setTime(uint16_t y, uint8_t m, uint8_t d, 
                uint8_t h=0, uint8_t min=0, uint8_t s=0);
    bool setTimezone(int8_t tz); // 设置时区（-12~+12）

    // 基本功能：时间增减（支持秒、分、时、天、月、年）
    Time& addSecond(int32_t s);   // 增减秒（可正可负）
    Time& addMinute(int32_t min); // 增减分钟
    Time& addHour(int32_t h);     // 增减小时
    Time& addDay(int32_t d);      // 增减天
    Time& addMonth(int32_t m);    // 增减月（自动处理月份进位）
    Time& addYear(int32_t y);     // 增减年（处理闰年2月）

    // 基本功能：时间比较（返回当前对象与t的关系）
    bool operator==(const Time& t) const;
    bool operator!=(const Time& t) const;
    bool operator<(const Time& t) const;
    bool operator>(const Time& t) const;

    // 扩展功能：Unix时间戳转换（UTC时间戳，秒数）
    uint64_t toUnixTimestamp() const; // 转换为时间戳
    bool fromUnixTimestamp(uint64_t timestamp); // 从时间戳初始化

    // 扩展功能：时区转换（返回指定时区的时间）
    Time toTimezone(int8_t tz) const;

    // 扩展功能：格式化输出（支持自定义格式）
    // 格式符：YYYY MM DD HH mm ss TZ（大写为补零，小写为原生）
    void format(char* buf, const char* fmt = "%YYYY-%MM-%DD %HH:%mm:%ss") const;

    // 扩展功能：获取星期几（1=周一，7=周日）
    uint8_t getWeekday() const;

    // 扩展功能：判断是否为当天（基于当前对象的日期）
    bool isSameDay(const Time& t) const;
};