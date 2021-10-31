#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_SSD1306.h>
#include <Button2.h>

#define ADC_PIN 34
#define VREF 1100
#define BUTTON_1 35
#define BUTTON_2 0

Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);

#define PIN 32   // On Trinket or Gemma, suggest changing this to 1
#define PIN_2 33 // On Trinket or Gemma, suggest changing this to 1

#define NUMPIXELS 60 // Popular NeoPixel ring size

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels_2(NUMPIXELS, PIN_2, NEO_GRB + NEO_KHZ800);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

hw_timer_t *timers[4] = {NULL, NULL, NULL, NULL};

uint16_t current_animation = 0;
bool shoot_requested = false;

#define DELAYVAL 100 // Time (in milliseconds) to pause between pixels

void IRAM_ATTR button1_handler(Button2 &btn)
{
  uint32_t click_type = btn.getClickType();

  if (click_type == SINGLE_CLICK || click_type == LONG_CLICK)
  {
    Serial.println("Click");
    current_animation = current_animation < 2 ? current_animation + 1 : 0;
  }
}

void IRAM_ATTR button2_handler(Button2 &btn)
{
  uint32_t click_type = btn.getClickType();

  if (click_type == SINGLE_CLICK || click_type == LONG_CLICK)
  {
    Serial.println("Click");
    shoot_requested = true;
  }
  if (click_type == DOUBLE_CLICK)
  {
  }
}

void button_init()
{
  btn1.setClickHandler(button1_handler);
  btn1.setLongClickHandler(button1_handler);
  btn1.setDoubleClickHandler(button1_handler);
  btn1.setTripleClickHandler(button1_handler);
  btn2.setClickHandler(button2_handler);
  btn2.setLongClickHandler(button2_handler);
  btn2.setDoubleClickHandler(button2_handler);
  btn2.setTripleClickHandler(button2_handler);
}

void IRAM_ATTR button_loop()
{
  btn1.loop();
  btn2.loop();
}

void IRAM_ATTR call_buttons(void)
{
  button_loop();
}

void update_screen(void)
{
}

void setup()
{
  Serial.begin(115200);
  //display setup
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 1 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);
  display.setTextSize(3);
  // put your setup code here, to run once:
  pixels.begin();
  pixels_2.begin();

  timers[3] = timerBegin(3, 80, true);
  timerAttachInterrupt(timers[3], &call_buttons, false);
  timerAlarmWrite(timers[3], 50 * 1000, true);
  timerAlarmEnable(timers[3]);

  button_init();
}

void update_voltage(bool print)
{
  display.clearDisplay();
  display.setCursor(35, 0);
  display.setTextSize(2);
  uint16_t v = analogRead(ADC_PIN);
  float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (VREF / 1000.0);
  display.printf("%.2fv", battery_voltage);
  if (print)
  {
    display.display();
  }
}

void show_mode(bool print)
{
  display.setCursor(35, 30);
  display.setTextSize(2);
  display.printf("=>%d<=", current_animation);
  if (print)
  {
    display.display();
  }
}

void shoot()
{
  update_voltage(false);
  show_mode(true);

  // put your main code here, to run repeatedly:
  pixels.clear();   // Set all pixel colors to 'off'
  pixels_2.clear(); // Set all pixel colors to 'off'

  // The first NeoPixel in a strand is #0, second is 1, all the way up
  // to the count of pixels minus one.
  for (int i = 0; i < 20; i++)
  { // For each pixel...
    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    // pixels.setPixelColor(i, pixels.Color(30, 30, 30));
    pixels_2.setPixelColor(i, pixels_2.Color(200, 0, 0));

    // pixels.show(); // Send the updated pixel colors to the hardware.
  }
  pixels_2.show(); // Send the updated pixel colors to the hardware.

  delay(50);
  pixels_2.clear(); // Set all pixel colors to 'off'
  pixels_2.show();  // Send the updated pixel colors to the hardware.
  for (int u = 0; u < NUMPIXELS; u++)
  {
    pixels.clear(); // Set all pixel colors to 'off'

    for (int i = 0; i < NUMPIXELS; i++)
    { // For each pixel...
      // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
      // Here we're using a moderately bright green color:
      if (u >= i && u <= i + 3)
      {
        pixels.setPixelColor(i, pixels.Color(map(u - i, 0, 4, 250, 0),
                                             map(u - i, 0, 4, 250, 0),
                                             map(u - i, 0, 4, 250, 0)));
      }
    }

    pixels.show(); // Send the updated pixel colors to the hardware.
    delay(1);      // Pause before next pass through loop
  }
}

void rainbow(int wait)
{
  for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256)
  {
    for (int i = 0; i < pixels.numPixels(); i++)
    {
      int pixelHue = firstPixelHue + (i * 65536L / pixels.numPixels());
      pixels.setPixelColor(i, pixels.gamma32(pixels.ColorHSV(pixelHue)));
      pixels_2.setPixelColor(i, pixels_2.gamma32(pixels_2.ColorHSV(pixelHue)));
    }
    pixels.show();
    pixels_2.show();
    delay(wait);
    update_voltage(false);
    show_mode(true);
    if (current_animation != 1)
      break;
  }
}
void animation_2(void)
{
  pixels.setBrightness(50);
  pixels_2.setBrightness(50);
  rainbow(10);
}

void animation_1(void)
{
  pixels_2.clear(); // Set all pixel colors to 'off'
  pixels_2.show();
  pixels.clear(); // Set all pixel colors to 'off'
  pixels.show();
    pixels.setBrightness(100);
    pixels_2.setBrightness(100);
    shoot();
  for (int i = 0; i < 20; i++)
  {
    delay(100);
    update_voltage(false);
    show_mode(true);
    if (current_animation != 0)
      break;
  }
}

void animation_3(void)
{
  for (int u = 0; u < NUMPIXELS; u++)
  {

    for (int i = 0; i < NUMPIXELS; i++)
    { // For each pixel...
      // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
      // Here we're using a moderately bright green color:
      pixels.setPixelColor(i, pixels.Color(map((u + i) % NUMPIXELS, 0, NUMPIXELS, 250, 20),
                                           map((u + i) % NUMPIXELS, 0, NUMPIXELS, 5, 0),
                                           map((u + i) % NUMPIXELS, 0, NUMPIXELS, 5, 0)));
      pixels_2.setPixelColor(i, pixels_2.Color(map((u + i) % NUMPIXELS, 0, NUMPIXELS, 250, 20),
                                           map((u + i) % NUMPIXELS, 0, NUMPIXELS, 5, 0),
                                           map((u + i) % NUMPIXELS, 0, NUMPIXELS, 5, 0)));

    }
    pixels.show();
    pixels_2.show();
        update_voltage(false);
    show_mode(true);
    if (current_animation != 2)
      break;
    delay(100);      // Pause before next pass through loop
  
  }
}

void loop()
{
  // pixels_2.clear(); // Set all pixel colors to 'off'
  // pixels_2.show();
  // pixels.clear(); // Set all pixel colors to 'off'
  // pixels.show();
  update_voltage(false);
  show_mode(true);
  // 	display.clearDisplay();
  // 	display.setCursor(0, 0);
  //   	uint16_t v = analogRead(ADC_PIN);
  // float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (VREF / 1000.0);
  // 	display.printf("%.2fv", battery_voltage);
  // 	display.display();

  if (current_animation == 0)
  {
    animation_1();
  }
  else if (current_animation == 1)
  {
    animation_2();
  }
  else if (current_animation == 2)
  {
    animation_3();
  }
  else
  {
      pixels_2.clear(); // Set all pixel colors to 'off'
  pixels_2.show();
  pixels.clear(); // Set all pixel colors to 'off'
  pixels.show();
  }
}
