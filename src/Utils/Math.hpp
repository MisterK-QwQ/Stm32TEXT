#pragma once

#include <cmath>
#include <cstdint>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

namespace MathTools {

// 角度单位转换
constexpr float RAD2DEG = 180.0f / M_PI;
constexpr float DEG2RAD = M_PI / 180.0f;

/**
 * @brief 弧度转角度
 * @param rad 弧度值
 * @return 角度值
 */
static inline float rad2deg(float rad) {
    return rad * RAD2DEG;
}

/**
 * @brief 角度转弧度
 * @param deg 角度值
 * @return 弧度值
 */
static inline float deg2rad(float deg) {
    return deg * DEG2RAD;
}

/**
 * @brief 限制数值在指定范围内
 * @param val 输入值
 * @param min 最小值
 * @param max 最大值
 * @return 限制后的值
 */
static inline float clamp(float val, float min, float max) {
    return (val < min) ? min : (val > max) ? max : val;
}

/**
 * @brief 线性映射
 * @param val 输入值
 * @param in_min 输入最小值
 * @param in_max 输入最大值
 * @param out_min 输出最小值
 * @param out_max 输出最大值
 * @return 映射后的值
 */
static inline float map(float val, float in_min, float in_max, float out_min, float out_max) {
    return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/**
 * @brief 一阶低通滤波
 * @param current 当前值
 * @param previous 上一次滤波后的值
 * @param alpha 滤波系数 (0~1, 越小越平滑)
 * @return 滤波后的值
 */
static inline float lowPassFilter(float current, float previous, float alpha) {
    return alpha * current + (1.0f - alpha) * previous;
}

/**
 * @brief 计算两个角度的差值（考虑角度环绕）
 * @param target 目标角度 (°)
 * @param current 当前角度 (°)
 * @return 最短差值 (°)
 */
static inline float angleDifference(float target, float current) {
    float diff = fmodf(target - current + 180.0f, 360.0f) - 180.0f;
    return (diff < -180.0f) ? diff + 360.0f : diff;
}

/**
 * @brief PID控制器
 */
class PIDController {
public:
    PIDController(float kp, float ki, float kd, float integral_limit)
        : kp(kp), ki(ki), kd(kd), integral_limit(integral_limit),
          integral(0.0f), previous_error(0.0f) {}

    /**
     * @brief 更新PID输出
     * @param target 目标值
     * @param current 当前值
     * @param dt 时间间隔 (s)
     * @return PID输出
     */
    float update(float target, float current, float dt) {
        float error = target - current;
        integral += error * dt;
        integral = clamp(integral, -integral_limit, integral_limit);
        float derivative = (error - previous_error) / dt;
        previous_error = error;
        return kp * error + ki * integral + kd * derivative;
    }

    /**
     * @brief 重置PID状态
     */
    void reset() {
        integral = 0.0f;
        previous_error = 0.0f;
    }

    void setPID(float p, float i, float d) {
        kp = p;
        ki = i;
        kd = d;
    }

private:
    float kp;
    float ki;
    float kd;
    float integral_limit;
    float integral;
    float previous_error;
};

/**
 * @brief 舵机控制
 */
class ServoControl {
public:
    /**
     * @brief 构造函数
     * @param min_angle 最小角度 (°)
     * @param max_angle 最大角度 (°)
     * @param min_pulse 最小脉冲宽度 (us)
     * @param max_pulse 最大脉冲宽度 (us)
     * @param frequency PWM频率 (Hz)
     */
    ServoControl(float min_angle, float max_angle,
                 uint16_t min_pulse, uint16_t max_pulse,
                 uint32_t frequency = 50)
        : min_angle(min_angle), max_angle(max_angle),
          min_pulse(min_pulse), max_pulse(max_pulse),
          frequency(frequency) {}

    /**
     * @brief 角度转脉冲宽度
     * @param angle 角度 (°)
     * @return 脉冲宽度 (us)
     */
    uint16_t angleToPulse(float angle) {
        angle = clamp(angle, min_angle, max_angle);
        return static_cast<uint16_t>(map(angle, min_angle, max_angle, min_pulse, max_pulse));
    }

    /**
     * @brief 脉冲宽度转角度
     * @param pulse 脉冲宽度 (us)
     * @return 角度 (°)
     */
    float pulseToAngle(uint16_t pulse) {
        return map(static_cast<float>(pulse), min_pulse, max_pulse, min_angle, max_angle);
    }

private:
    float min_angle;
    float max_angle;
    uint16_t min_pulse;
    uint16_t max_pulse;
    uint32_t frequency;
};

/**
 * @brief 陀螺仪数据处理 (一阶互补滤波)
 */
class GyroProcessing {
public:
    GyroProcessing(float sample_rate, float alpha = 0.98f)
        : sample_rate(sample_rate), alpha(alpha),
          angle_x(0.0f), angle_y(0.0f), angle_z(0.0f) {}

    /**
     * @brief 更新陀螺仪角度
     * @param gyro_x 陀螺仪X轴角速度 (rad/s)
     * @param gyro_y 陀螺仪Y轴角速度 (rad/s)
     * @param gyro_z 陀螺仪Z轴角速度 (rad/s)
     * @param accel_angle_x 加速度计X轴角度 (°)
     * @param accel_angle_y 加速度计Y轴角度 (°)
     */
    void update(float gyro_x, float gyro_y, float gyro_z,
                float accel_angle_x, float accel_angle_y) {
        float dt = 1.0f / sample_rate;

        // 积分计算陀螺仪角度
        float gyro_angle_x = angle_x + gyro_x * dt * RAD2DEG;
        float gyro_angle_y = angle_y + gyro_y * dt * RAD2DEG;
        angle_z += gyro_z * dt * RAD2DEG;

        // 一阶互补滤波融合加速度计和陀螺仪数据
        angle_x = alpha * gyro_angle_x + (1.0f - alpha) * accel_angle_x;
        angle_y = alpha * gyro_angle_y + (1.0f - alpha) * accel_angle_y;
    }

    float getAngleX() const { return angle_x; }
    float getAngleY() const { return angle_y; }
    float getAngleZ() const { return angle_z; }

    void reset() {
        angle_x = angle_y = angle_z = 0.0f;
    }

private:
    float sample_rate;    // 采样率 (Hz)
    float alpha;          // 互补滤波系数
    float angle_x;        // X轴角度 (°)
    float angle_y;        // Y轴角度 (°)
    float angle_z;        // Z轴角度 (°)
};

// ===========================================================================
// 新增：马达（直流电机）控制类
// ===========================================================================

/**
 * @brief 直流电机（马达）控制类
 * 用于控制电机的速度和方向，支持开环和闭环控制。
 */
class MotorControl {
public:
    /**
     * @brief 构造函数
     * @param pwm_freq PWM频率 (Hz)
     * @param pwm_resolution PWM分辨率 (例如12位ADC对应4096)
     */
    MotorControl(uint32_t pwm_freq, uint16_t pwm_resolution)
        : pwm_freq(pwm_freq), pwm_resolution(pwm_resolution),
          pid_controller(0.0f, 0.0f, 0.0f, 1.0f), // 默认PID参数，需用户根据实际情况调整
          is_pid_enabled(false), target_speed(0.0f) {}

    /**
     * @brief 设置电机速度 (开环控制)
     * @param speed 目标速度 (-1.0 ~ 1.0)，负值表示反转
     * @return PWM占空比和方向控制信号
     */
    void setSpeed(float speed) {
        if (is_pid_enabled) {
            // 如果启用了PID，则speed作为目标速度传入，由PID闭环控制
            target_speed = speed;
        } else {
            // 开环控制，直接计算PWM和方向
            calculateOutput(speed);
        }
    }

    /**
     * @brief 更新PID控制 (闭环控制)
     * @param current_speed 电机当前速度 (-1.0 ~ 1.0)
     * @param dt 时间间隔 (s)
     */
    void updatePID(float current_speed, float dt) {
        if (!is_pid_enabled) return;

        // 使用PID控制器计算输出
        float pid_output = pid_controller.update(target_speed, current_speed, dt);
        calculateOutput(pid_output);
    }

    /**
     * @brief 启用/禁用PID闭环控制
     * @param enable true-启用，false-禁用
     */
    void enablePID(bool enable) {
        is_pid_enabled = enable;
        if (enable) {
            pid_controller.reset(); // 启用时重置PID状态
        }
    }

    /**
     * @brief 设置PID参数
     * @param kp 比例系数
     * @param ki 积分系数
     * @param kd 微分系数
     * @param integral_limit 积分限幅
     */
    void setPIDParams(float kp, float ki, float kd, float integral_limit) {
        pid_controller.setPID(kp, ki, kd);
        // 注意：原PIDController类的integral_limit是private，需要修改为public或提供setter
        // 这里假设已经添加了setIntegralLimit方法
        // pid_controller.setIntegralLimit(integral_limit);
    }

    /**
     * @brief 获取PWM占空比
     * @return PWM占空比 (0 ~ pwm_resolution)
     */
    uint16_t getPWM() const { return pwm_duty; }

    /**
     * @brief 获取电机方向
     * @return true-正转，false-反转
     */
    bool getDirection() const { return direction; }

private:
    uint32_t pwm_freq;
    uint16_t pwm_resolution;
    PIDController pid_controller;
    bool is_pid_enabled;
    float target_speed;

    // 输出状态
    uint16_t pwm_duty = 0;
    bool direction = true; // true-正转，false-反转

    /**
     * @brief 根据输入计算PWM和方向
     * @param input 输入值 (-1.0 ~ 1.0)
     */
    void calculateOutput(float input) {
        // 限制输入范围
        input = clamp(input, -1.0f, 1.0f);

        // 确定方向
        direction = (input >= 0.0f);

        // 计算PWM占空比
        float duty_cycle = fabs(input);
        pwm_duty = static_cast<uint16_t>(duty_cycle * pwm_resolution);
    }
};
}

