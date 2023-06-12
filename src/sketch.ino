//
// Socket controller.
// Detects load and switching it off for the POWER_OFF_DURATION after WORK_DURATION.
//
#define SENSOR_PIN A0
#define RELAY_PIN 0
#define LED_PIN 1

#define CURRENT_THRESHOLD 15

#define POWER_OFF_DURATION 4000

unsigned long HOUR = 1000L * 60 * 60;
unsigned long HALF_HOUR = 1000L * 60 * 30;

unsigned long WORK_DURATION = HOUR + HALF_HOUR;

unsigned long loadDetectionTime = 0;

bool wasLoadPresent = false;

void setup() {
    pinMode(SENSOR_PIN, INPUT);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    bool loadPresent = isLoadPresent();

    if (loadPresent && !wasLoadPresent) {
        loadDetectionTime = millis();
    }
    wasLoadPresent = loadPresent;

    digitalWrite(LED_PIN, loadPresent);

    if (loadPresent && (millis() - loadDetectionTime > WORK_DURATION)) {
        digitalWrite(LED_PIN, LOW);
        resetLoad();
        digitalWrite(LED_PIN, HIGH);

        loadDetectionTime = 0;
        wasLoadPresent = false;
    }

    delay(1000);
}

/**
 * Power off the load, wait POWER_OFF_DURATION and on power again.
 */
void resetLoad() {
    digitalWrite(RELAY_PIN, HIGH);

    delay(POWER_OFF_DURATION);

    digitalWrite(RELAY_PIN, LOW);
}

/**
 * Returns true if current is detected.
 */
bool isLoadPresent() {
    int current = measureCurrent();
    return current > CURRENT_THRESHOLD;
}

/**
 * Return value which depends on current value.
 *
 * Value is not an ampers but some abstract value which is
 * greater if current is greater.
 *
 * @return integer value 0 - 1023
 *    0 - 4 - no load
 */
int measureCurrent() {
    int minv = 1025;
    int maxv = -1;
    for(int i = 0; i < 40; i++) {
      int value = analogRead(SENSOR_PIN);
      maxv = max(maxv, value);
      minv = min(minv, value);
      delayMicroseconds(500);
    }
    return maxv - minv;
}
