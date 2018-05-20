// Effects for Neopixel LED Ring Set WS 2812 (1/8/12/16/24)

#include <FastLED.h>

// define parameters for FastLED
#define LED_TYPE            WS2812B
#define LED_COLOR_ORDER     GRB
#define LED_PIN             10

// define number of LEDs, first and last per ring
const int NUM_LEDS      =   61;
const int LAST_LED      =   NUM_LEDS - 1;
const int LEDS_RING[]   =   {1, 8, 12, 16, 24};
const int FIRST_RING[]  =   {0,
                             LEDS_RING[0],
                             LEDS_RING[0] + LEDS_RING[1],
                             LEDS_RING[0] + LEDS_RING[1] + LEDS_RING[2],
                             LEDS_RING[0] + LEDS_RING[1] + LEDS_RING[2] + LEDS_RING[3]};
const int LAST_RING[]   =   {0,
                             FIRST_RING[1] + LEDS_RING[1] -1,
                             FIRST_RING[2] + LEDS_RING[2] -1,
                             FIRST_RING[3] + LEDS_RING[3] -1,
                             FIRST_RING[4] + LEDS_RING[4] -1};

CRGB leds[NUM_LEDS];

// parameters for delay, counter and directions
int delay_counter;
int current_delay       =   30;
int rounds_counter;
bool current_direction  =   false;

// parameters for color, brigthness and current/previous LEDs
uint8_t current_color   =   160;
uint8_t current_bright  =   30;
uint8_t current_led;
uint8_t previous_led;
uint8_t current_led_ring[5];
uint8_t previous_led_ring[5];
uint8_t led_bright[NUM_LEDS];

// input and animation control
char input_command;
char current_animation  =   '1';


void setup() {

  Serial.begin(9600);
  Serial.println("Welcome: Effects LED Ring Set WS 2812");
  print_effects_and_commands();

  // initialize LEDs
  // delay(300);
  FastLED.addLeds<LED_TYPE, LED_PIN, LED_COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  
}

void loop() {

  getInput();

  if (delay_counter >= current_delay) {
    leds_in_circles();
    delay_counter = 0;
  }
  else {
    delay_counter++;
  }

  FastLED.show();

}

void getInput() {

  input_command = char(Serial.read());
  
  switch(input_command) {
    case '?':
      print_effects_and_commands();
      break;
      
    case '1':
    case '2':
      if (input_command != current_animation) {
        current_animation = input_command;
        Serial.print("Current animation (? List): ");
        Serial.println(current_animation);
      }
      else {
        Serial.print("Animation not changed (? List): ");
        Serial.println(current_animation);

      }
      break;

    case -1:
    case 0:
    case '\n':
    case '\r':
      // ignore input:
      break;
      
    default:
        Serial.print("Invalid input ignored (? List): ");
        Serial.println(input_command);
  }
}

void leds_in_circles() {

  // LEDs circling in all rings

  if (rounds_counter < 4) {
    led_step(LAST_RING[4 - rounds_counter]);
  } 
  else {
    fade_in_out(0, 0, current_bright + 3, 3);
  }

  for (int i = rounds_counter; i > 0; i--) {
    ring_step(5 - i);
  }
  
  if (current_led > LAST_RING[4 - rounds_counter]) {
    rounds_counter++;
    current_led = 0;
  }
  
}

void led_step (int last) {

  // go to next through all rings

  leds[previous_led] = ColorFromPalette( RainbowColors_p, current_color, 0, LINEARBLEND);    
  leds[current_led] = ColorFromPalette( RainbowColors_p, current_color, current_bright, LINEARBLEND);    
  previous_led = current_led;
  if (current_led <= last) {
    current_led++;
  }
  
}

void ring_step(int ring) {

  // go to next staying in one ring
  
  if (current_led_ring[ring] < FIRST_RING[ring]) {
    current_led_ring[ring] = FIRST_RING[ring];
    previous_led_ring[ring] = LAST_RING[ring];
  }
  leds[previous_led_ring[ring]] = ColorFromPalette( RainbowColors_p, current_color, 0, LINEARBLEND);    
  leds[current_led_ring[ring]] = ColorFromPalette( RainbowColors_p, current_color, current_bright, LINEARBLEND);    
  previous_led_ring[ring] = current_led_ring[ring];
  current_led_ring[ring]++;
  if (current_led_ring[ring] > LAST_RING[ring]) {
    current_led_ring[ring] = FIRST_RING[ring];
  }
  
}

void fade_in_out(int fade_led, int b_min, int b_max, int fade_step) {
  
  // fade brightness of one LED
  // uses current_direction: true = fade up, false = fade down

  if (led_bright[fade_led] < b_min) {
    led_bright[fade_led] = b_min;
  }

  if (led_bright[fade_led] < b_min + fade_step || led_bright[fade_led] > b_max - fade_step) {
    current_direction = !current_direction;
  }
  if (current_direction) {
   led_bright[fade_led]+= fade_step; 
  }
  else {
    led_bright[fade_led]-= fade_step;
  }
  
  leds[fade_led] = ColorFromPalette( RainbowColors_p, current_color, led_bright[fade_led], LINEARBLEND);    
  
}

void print_effects_and_commands() {
  
  Serial.println();
  Serial.println("---------- Animations ----------");
  Serial.println("1: LEDs circling in all rings");
  Serial.println("--------------------------------");
  Serial.print("Current animation (? List): ");
  Serial.println(current_animation);
  
}

