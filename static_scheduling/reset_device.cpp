  /**
  ******************************************************************************
  * @file        : reset_device.hpp
  * @brief       : Reset device
  * @author      : Loup Olivia, Amez-Droz Jonathan
  * @date        : 21.10.24
  ******************************************************************************
  * @details
  * The ResetDevice must implement a reset when the user presses the push button.
  ******************************************************************************
  */
// includes come here
#include "reset_device.hpp"

#if defined(TARGET_DISCO_H747I)
#define PUSH_BUTTON BUTTON1
static constexpr uint8_t kPolarityPressed = 1;
#endif


// code comes here
namespace static_scheduling {

static constexpr std::chrono::microseconds kTaskRunTime = 100000us;

#ifdef TARGET_DISCO_H747I
ResetDevice::ResetDevice(Timer& timer) : _resetButton(PUSH_BUTTON), _timer(timer) {
    _resetButton.rise(callback(this, &ResetDevice::onRise));
}
#else
ResetDevice::ResetDevice(Timer& timer) : _timer(timer) {}
#endif

void ResetDevice::onRise() { _pressTime = _timer.elapsed_time(); }

bool ResetDevice::checkReset() {
    std::chrono::microseconds initialTime = _timer.elapsed_time();
    std::chrono::microseconds elapsedTime = std::chrono::microseconds::zero();
    while (elapsedTime < kTaskRunTime) {
        if (_resetButton.read() == kPolarityPressed) {
            return true;
        }
        elapsedTime = _timer.elapsed_time() - initialTime;
    }
    return false;
}

std::chrono::microseconds ResetDevice::getPressTime() { return _pressTime; }

}  // namespace static_scheduling