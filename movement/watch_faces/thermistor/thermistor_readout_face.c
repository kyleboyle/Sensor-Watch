#include <stdlib.h>
#include <string.h>
#include "thermistor_readout_face.h"
#include "thermistor_driver.h"
#include "watch.h"

static void _thermistor_readout_face_update_display(bool in_fahrenheit) {
    thermistor_driver_enable();
    float temperature_c = thermistor_driver_get_temperature();
    char buf[14];
    if (in_fahrenheit) {
        sprintf(buf, "%4.1f#F", temperature_c * 1.8 + 32.0);
    } else {
        sprintf(buf, "%4.1f#C", temperature_c);
    }
    watch_display_string(buf, 4);
    thermistor_driver_disable();
}

void thermistor_readout_face_setup(movement_settings_t *settings, uint8_t watch_face_index, void ** context_ptr) {
    (void) settings;
    (void) watch_face_index;
    (void) context_ptr;
}

void thermistor_readout_face_activate(movement_settings_t *settings, void *context) {
    (void) settings;
    (void) context;
    watch_display_string("TE", 0);
}

bool thermistor_readout_face_loop(movement_event_t event, movement_settings_t *settings, void *context) {
    (void) context;
    watch_date_time date_time = watch_rtc_get_date_time();
    switch (event.event_type) {
        case EVENT_MODE_BUTTON_UP:
            movement_move_to_next_face();
            break;
        case EVENT_LIGHT_BUTTON_DOWN:
            movement_illuminate_led();
            break;
        case EVENT_ALARM_BUTTON_UP:
            settings->bit.use_imperial_units = !settings->bit.use_imperial_units;
            _thermistor_readout_face_update_display(settings->bit.use_imperial_units);
            break;
        case EVENT_ACTIVATE:
            // force a measurement to be taken immediately.
            date_time.unit.second = 0;
            // fall through
        case EVENT_TICK:
            if (date_time.unit.second % 5 == 4) {
                // Not 100% on this, but I like the idea of using the signal indicator to indicate that we're sensing data.
                // In this case we turn the indicator on a second before the reading is taken, and clear it when we're done.
                // In reality the measurement takes a fraction of a second, but this is just to show something is happening.
                watch_set_indicator(WATCH_INDICATOR_SIGNAL);
            } else if (date_time.unit.second % 5 == 0) {
                _thermistor_readout_face_update_display(settings->bit.use_imperial_units);
                watch_clear_indicator(WATCH_INDICATOR_SIGNAL);
            }
            break;
        case EVENT_LOW_ENERGY_UPDATE:
            watch_display_string("TE  SLEEP ", 0);
            break;
        default:
            break;
    }

    return true;
}

void thermistor_readout_face_resign(movement_settings_t *settings, void *context) {
    (void) settings;
    (void) context;
}
