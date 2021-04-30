// Radio
#define RFM69_CS 10

// Analog
#define TEMP_SNSR A2
#define TEMP_POLL_MS 1000
#define TEMP_INTERCEPT 148.5	// 0: 158   1: 148.5   2: 156
#define TEMP_SLOPE 3.103

// Buttons
#define BUTTON_UP digitalPinToInterrupt(A5)
#define BUTTON_DOWN digitalPinToInterrupt(A7)
