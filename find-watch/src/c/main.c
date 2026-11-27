// Find Watch — Pebble Time Steel (basalt) — part of Find Both.
// Phone -> watch. This app is hidden (hiddenApp in package.json), so it
// does not appear in the watch launcher. Launch it from the Pebble app
// on your phone and the watch starts buzzing and flashing at once.
//
// Time Steel has no speaker, so this is vibrate + backlight flash only.
// Runs for 5 minutes, then stops by itself. Any button stops it early;
// pressing a button again starts it buzzing again.
//
// Adapted from Find Watch 2 by Christian Ratzenhofer (MIT),
// github.com/merlin1991/pebble-findwatch2, simplified: no config page,
// fixed 5-minute run, vibrate + flash always on.
#include <pebble.h>

#define RUN_MINUTES 5

static Window *s_window;
static TextLayer *s_timer_layer;
static TextLayer *s_hint_layer;
static int s_minutes = 0;
static int s_seconds = 0;
static bool s_running = false;

static void prv_draw(void) {
  static char buf[8];
  snprintf(buf, sizeof(buf), "%d:%02d", s_minutes, s_seconds);
  text_layer_set_text(s_timer_layer, buf);
  if (s_running && (s_seconds % 2 == 0)) {
    window_set_background_color(s_window, GColorWhite);
    text_layer_set_background_color(s_timer_layer, GColorWhite);
    text_layer_set_text_color(s_timer_layer, GColorBlack);
    text_layer_set_background_color(s_hint_layer, GColorWhite);
    text_layer_set_text_color(s_hint_layer, GColorBlack);
    light_enable(true);
  } else if (s_running) {
    window_set_background_color(s_window, GColorBlack);
    text_layer_set_background_color(s_timer_layer, GColorBlack);
    text_layer_set_text_color(s_timer_layer, GColorWhite);
    text_layer_set_background_color(s_hint_layer, GColorBlack);
    text_layer_set_text_color(s_hint_layer, GColorWhite);
    light_enable(false);
  } else {
    window_set_background_color(s_window, GColorWhite);
    text_layer_set_background_color(s_timer_layer, GColorWhite);
    text_layer_set_text_color(s_timer_layer, GColorBlack);
    text_layer_set_background_color(s_hint_layer, GColorWhite);
    text_layer_set_text_color(s_hint_layer, GColorBlack);
    light_enable(false);
  }
}

static void prv_stop(void) {
  s_running = false;
  s_minutes = 0;
  s_seconds = 0;
  text_layer_set_text(s_hint_layer, "Found! Press any\nbutton to buzz again");
  prv_draw();
}

static void prv_start(void) {
  s_running = true;
  s_minutes = 0;
  s_seconds = 0;
  text_layer_set_text(s_hint_layer, "Buzzing — press any\nbutton when found");
  prv_draw();
}

static void prv_tick(struct tm *tick_time, TimeUnits units_changed) {
  if (!s_running) {
    return;
  }
  s_seconds++;
  if (s_seconds > 59) {
    s_minutes++;
    s_seconds = 0;
  }
  vibes_short_pulse();
#if defined(PBL_SPEAKER)
  speaker_play_tone(800, 250, 90, SpeakerWaveformSquare);
#endif
  prv_draw();
  if (s_minutes >= RUN_MINUTES) {
    prv_stop();
    text_layer_set_text(s_hint_layer, "Stopped after 5 min.\nPress any button\nto buzz again");
  }
}

static void prv_click(ClickRecognizerRef recognizer, void *context) {
  if (s_running) {
    prv_stop();
  } else {
    prv_start();
  }
}

static void prv_click_config(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, prv_click);
  window_single_click_subscribe(BUTTON_ID_UP, prv_click);
  window_single_click_subscribe(BUTTON_ID_DOWN, prv_click);
  window_single_click_subscribe(BUTTON_ID_BACK, prv_click);
}

static void prv_window_load(Window *window) {
  Layer *root = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root);

  s_timer_layer = text_layer_create(GRect(0, 44, bounds.size.w, 60));
  text_layer_set_text_alignment(s_timer_layer, GTextAlignmentCenter);
  text_layer_set_font(s_timer_layer, fonts_get_system_font(FONT_KEY_LECO_38_BOLD_NUMBERS));
  layer_add_child(root, text_layer_get_layer(s_timer_layer));

  s_hint_layer = text_layer_create(GRect(8, 112, bounds.size.w - 16, 50));
  text_layer_set_text_alignment(s_hint_layer, GTextAlignmentCenter);
  text_layer_set_font(s_hint_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_overflow_mode(s_hint_layer, GTextOverflowModeWordWrap);
  layer_add_child(root, text_layer_get_layer(s_hint_layer));

  // Starts buzzing the moment it is launched from the phone.
  prv_start();
}

static void prv_window_unload(Window *window) {
  text_layer_destroy(s_timer_layer);
  text_layer_destroy(s_hint_layer);
}

static void prv_init(void) {
  s_window = window_create();
  window_set_click_config_provider(s_window, prv_click_config);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });
  window_stack_push(s_window, true);
  tick_timer_service_subscribe(SECOND_UNIT, prv_tick);
}

static void prv_deinit(void) {
  tick_timer_service_unsubscribe();
  window_destroy(s_window);
}

int main(void) {
  prv_init();
  app_event_loop();
  prv_deinit();
}
