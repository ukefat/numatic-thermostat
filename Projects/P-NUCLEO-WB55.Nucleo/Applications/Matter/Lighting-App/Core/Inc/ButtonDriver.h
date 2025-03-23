#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "stm32wbxx_hal.h"
#include <functional>

#define MAX_BUTTONS 5  // Change as needed

class Button {
public:
    using Callback = std::function<void()>;  // Callback type

    // Constructor: Initializes button with optional callback
    Button(GPIO_TypeDef *port, uint16_t pin, Callback callback = nullptr);

    // Reads button state (polling mode)
    bool isPressed() const;

    // Registers a callback for interrupt mode
    void setCallback(Callback callback);

    // Handles external interrupt
    static void IRQ_Handler(uint16_t GPIO_Pin);

private:
    GPIO_TypeDef *port;
    uint16_t pin;
    Callback callback;

    static Button *buttonList[MAX_BUTTONS];  // Static list of buttons
    static uint8_t buttonCount;
};

#endif // BUTTON_HPP
