#include <iostream>
#include <thread>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <hil.h>
#include "qube_servo3_driver.hpp"
#include <atomic>
#include <csignal>
#include <fstream>

namespace fs = std::filesystem;

const double M_PI = 3.14159265358979323846;

std::atomic<bool> running(true);

void signalHandler(int)
{
    running = false;
}

namespace qube_servo3_driver
{
    QubeServo3Driver::QubeServo3Driver()
    {
        declareParameters();

        state_ = QubeState{};
        prev_state_ = QubeState{};
        commanded_effort_ = 0.0;

        initializeHardware();
    }

    QubeServo3Driver::~QubeServo3Driver()
    {
        shutdownHardware();
    }

    void QubeServo3Driver::declareParameters()
    {
        board_type_ = "qube_servo3_usb";
        board_identifier_ = "0";
        update_rate_ = 1000.0;
        encoder_resolution_ = 2048.0;
        pendulum_encoder_resolution_ = 2048.0;
        max_effort_ = 10.0;
        effort_to_voltage_ = 1.0;
        current_sensor_gain_ = 1;
    }

    bool QubeServo3Driver::initializeHardware()
    {
        t_error result;

        encoder_channels_[0] = 0;
        encoder_channels_[1] = 1;
        encoder_vel_channels_[0] = 14000;
        encoder_vel_channels_[1] = 14001;
        digital_out_channels_[0] = 0;

        dac_channels_[0] = 0;
        adc_channels_[0] = 0;
        pwm_channels_[0] = 0;

        result = hil_open(board_type_.c_str(), board_identifier_.c_str(), &card_);
        if (result != 0)
        {
            handleQuanserError("hil_open", result);
            return false;
        }

        // Initialize DAC to 0 before enabling amplifier
        writeVoltage(0.0);

        t_boolean enable_value[1] = {true};
        result = hil_write_digital(card_, digital_out_channels_, 1, enable_value);
        if (result != 0)
        {
            handleQuanserError("hil_write_digital (enable amplifier)", result);
        }
        else
        {
        }

        // std::this_thread::sleep_for(std::chrono::milliseconds(10000)); // Short delay to ensure hardware is ready

        motor_encoder_offset_ = 0;
        pendulum_encoder_offset_ = 0;

        hardware_initialized_ = true;
        readEncoders();

        motor_encoder_offset_ = state_.encoder_counts[0];
        pendulum_encoder_offset_ = state_.encoder_counts[1];

        return true;
    }

    void QubeServo3Driver::shutdownHardware()
    {
        if (!hardware_initialized_)
            return;

        writeVoltage(0.0);

        if (card_)
        {
            hil_close(card_);
            card_ = nullptr;
        }

        hardware_initialized_ = false;
    }

    bool QubeServo3Driver::readEncoders()
    {
        if (!hardware_initialized_)
            return false;

        t_int32 counts[2];
        t_error result = hil_read_encoder(card_, encoder_channels_, 2, counts);
        if (result != 0)
        {
            handleQuanserError("hil_read_encoder", result);
            return false;
        }

        counts[0] -= motor_encoder_offset_;
        counts[1] -= pendulum_encoder_offset_;

        state_.encoder_counts[0] = counts[0];
        state_.encoder_counts[1] = counts[1];
        state_.motor_position = (static_cast<double>(counts[0]) / encoder_resolution_) * 2.0 * M_PI;

        // Wrap counts to [-resolution/2, resolution/2] before converting to radians
        double wrapped_counts = std::remainder(static_cast<double>(counts[1]), pendulum_encoder_resolution_);
        state_.pendulum_position = (wrapped_counts / pendulum_encoder_resolution_) * 2.0 * M_PI;

        return true;
    }

    bool QubeServo3Driver::readCurrent()
    {
        if (!hardware_initialized_)
            return false;

        t_error result = hil_read_analog(card_, adc_channels_, 1, adc_values_);
        if (result != 0)
        {
            handleQuanserError("hil_read_analog", result);
            return false;
        }

        state_.motor_current = adc_values_[0] / current_sensor_gain_;

        return true;
    }

    void QubeServo3Driver::writeVoltage(double voltage)
    {
        if (!hardware_initialized_)
            return;

        voltage = std::clamp(voltage, -max_effort_, max_effort_);

        t_double voltages[1] = {voltage};

        t_error result = hil_write_analog(card_, dac_channels_, ARRAY_LENGTH(dac_channels_), voltages);
        if (result != 0)
        {
            handleQuanserError("hil_write_analog", result);
            return;
        }

        state_.motor_voltage = voltage;
    }

    void QubeServo3Driver::calculateVelocities()
    {
        if (!hardware_initialized_)
            return;

        t_double counts[2];
        t_error result = hil_read_other(card_, encoder_vel_channels_, 2, counts);
        if (result != 0)
        {
            handleQuanserError("hil_read_other", result);
            return;
        }

        state_.motor_velocity = (static_cast<double>(counts[0]) / encoder_resolution_) * 2.0 * M_PI;
        state_.pendulum_velocity = (static_cast<double>(counts[1]) / pendulum_encoder_resolution_) * 2.0 * M_PI;

        prev_state_ = state_;
    }

    double QubeServo3Driver::unwrapAngle(double current, double previous)
    {
        double delta = current - previous;
        if (delta > M_PI)
        {
            delta -= 2.0 * M_PI;
        }
        else if (delta < -M_PI)
        {
            delta += 2.0 * M_PI;
        }
        return delta;
    }

    void QubeServo3Driver::checkHardwareStatus()
    {
        if (!hardware_initialized_)
        {
            hardware_status_ = -1;
            return;
        }
        hardware_status_ = (card_ != nullptr) ? 0 : -2;
    }
    void QubeServo3Driver::controlLoop()
    {
        const double dt = 0.001; // 1 kHz loop (same as ROS version)
        std::ofstream file("C:/Users/lucky/Documents/Qube_Servo_3_Controller/data.csv");

        if (!file.is_open())
        {
            std::cerr << "Failed to open file" << std::endl;
            return;
        }
        file << "time,theta1,theta2,omega1,omega2,current,voltage\n";

        while (hardware_initialized_ && running)
        {
            auto start = std::chrono::steady_clock::now();
            static auto first_start = start;
            if (!readEncoders())
                continue;

            readCurrent();
            calculateVelocities();

            writeVoltage(0);
            auto end = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsed = end - start;

            // enforce 1kHz timing
            if (elapsed.count() < dt)
            {
                std::this_thread::sleep_for(
                    std::chrono::duration<double>(dt - elapsed.count()));
            }
            std::chrono::duration<double> elapsed_total = end - first_start;
            file << elapsed_total.count() << "," << state_.motor_position << "," << state_.pendulum_position << ","
                 << state_.motor_velocity << "," << state_.pendulum_velocity << "," << state_.motor_current << ","
                 << state_.motor_voltage << "\n";
        }
        file.close();
        std::cout << "Control loop exited, shutting down hardware." << std::endl;
    }
    void QubeServo3Driver::handleQuanserError(const char *function, int result)
    {
        result = std::abs(result);
        std::cout << "Quanser error in " << function << ": ";
        switch (result)
        {
        case 0:
            std::cerr << function << " succeeded." << std::endl;
            break;
        case QERR_HIL_WRITE_ANALOG_NOT_SUPPORTED:
            std::cerr << function << " failed: HIL write analog not supported." << std::endl;
            break;
        case QERR_INVALID_CARD_HANDLE:
            std::cerr << function << " failed: Invalid card handle." << std::endl;
            break;
        case QERR_MISSING_ANALOG_OUTPUTS:
            std::cerr << function << " failed: Missing analog outputs." << std::endl;
            break;
        case QERR_MISSING_ANALOG_OUTPUT_BUFFER:
            std::cerr << function << " failed: Missing analog output buffer." << std::endl;
            break;
        case QERR_TOO_MANY_ANALOG_OUTPUT_CHANNELS:
            std::cerr << function << " failed: Too many analog output channels." << std::endl;
            break;
        case QERR_INVALID_ANALOG_OUTPUT_CHANNEL:
            std::cerr << function << " failed: Invalid analog output channel." << std::endl;
            break;
        case QERR_DRIVER_INCOMPATIBLE_WITH_BOARD_DLL:
            std::cerr << function << " failed: Driver incompatible with board DLL." << std::endl;
            break;
        case QERR_INTERNAL_BUFFER_TOO_SMALL:
            std::cerr << function << " failed: Internal buffer too small." << std::endl;
            break;
        case QERR_OUT_OF_REQUIRED_SYSTEM_RESOURCES:
            std::cerr << function << " failed: Out of required system resources." << std::endl;
            break;
        default:
            std::cerr << function << " failed with error code: " << result << std::endl;
        }
    }
}

int main()
{
    std::signal(SIGINT, signalHandler);

    qube_servo3_driver::QubeServo3Driver driver;
    driver.controlLoop();
    return 0;
}