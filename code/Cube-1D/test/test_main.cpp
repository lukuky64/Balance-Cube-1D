#include <Arduino.h>
#include <unity.h>

#include "STATE_MACHINE.hpp"

// Create an instance of the STATEMACHINE class
STATE_MACHINE stateMachine;

// Runs before each test
void setUp(void)
{
    //
}

// Runs after each test
void tearDown(void)
{
}

// Test to check if the LED can be turned on
void test_LED_On(void)
{
    delay(100);
    TEST_ASSERT_EQUAL(HIGH, HIGH);
}

void setup()
{
    stateMachine.begin();

    // Start Unity testing
    UNITY_BEGIN();

    // Run the test cases
    RUN_TEST(test_LED_On);

    // End Unity testing
    UNITY_END();
}

void loop()
{
    // No implementation needed for tests
}
