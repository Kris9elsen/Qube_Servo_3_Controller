#ifndef QUBE_SERVO3_DRIVER_HPP
#define QUBE_SERVO3_DRIVER_HPP

#include <iostream>
#include <thread>
#include <chrono>

#include <hil.h>

namespace qube_servo3_driver
{

    struct QubeState
    {
        double motor_position;      // rad
        double motor_velocity;      // rad/s
        double pendulum_position;   // rad
        double pendulum_velocity;   // rad/s
        double motor_current;       // A
        double motor_voltage;       // V
        uint32_t encoder_counts[2]; // raw counts
    };

    class QubeServo3Driver
    {
    public:
        QubeServo3Driver();
        ~QubeServo3Driver();
        void controlLoop();

    private:


        // Quanser hardware communication - HIL API
        bool initializeHardware();
        void shutdownHardware();
        bool readEncoders();
        bool readCurrent();
        void writeVoltage(double voltage);

        // Utility functions
        void calculateVelocities();
        double unwrapAngle(double current, double previous);
        void checkHardwareStatus();

        // Parameters
        std::string board_type_;
        std::string board_identifier_;
        double update_rate_;
        double encoder_resolution_;          // motor encoder counts/rev
        double pendulum_encoder_resolution_; // pendulum encoder counts/rev
        double max_effort_;
        double effort_to_voltage_;
        double filter_alpha_;
        double current_sensor_gain_;
        bool enable_diagnostics_;

        // State variables
        QubeState state_;
        QubeState prev_state_;
        double commanded_effort_;

        // Quanser hardware handles - HIL API
        t_card card_;

        // Channel configuration
        t_uint32 encoder_channels_[2];
        t_uint32 encoder_vel_channels_[2];
        t_uint32 dac_channels_[1];
        t_uint32 adc_channels_[1];
        t_uint32 pwm_channels_[1];
        t_uint32 encoder_counts_[2];
        t_double adc_values_[1];
        t_uint32 digital_out_channels_[1];


        // Encoder offset
        t_int32 motor_encoder_offset_;
        t_int32 pendulum_encoder_offset_;

        // Hardware status
        bool hardware_initialized_;
        int hardware_status_;

        // Declare parameters
        void declareParameters();

        // Error handling
        void handleQuanserError(const char *function, int result);
    };

} // namespace qube_servo3_driver

#endif // QUBE_SERVO3_DRIVER_HPP