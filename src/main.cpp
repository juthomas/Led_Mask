#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_SSD1306.h>

#define PIN        32 // On Trinket or Gemma, suggest changing this to 1
#define PIN_2        33 // On Trinket or Gemma, suggest changing this to 1

#define NUMPIXELS 60 // Popular NeoPixel ring size

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels_2(NUMPIXELS, PIN_2, NEO_GRB + NEO_KHZ800);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


#define DELAYVAL 100 // Time (in milliseconds) to pause between pixels

void setup() {
  Serial.begin(115200);
	//display setup
	if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
	{
		Serial.println(F("SSD1306 1 allocation failed"));
		for (;;)
			; // Don't proceed, loop forever
	}
	display.setTextSize(1);	   // Normal 1:1 pixel scale
	display.setTextColor(WHITE); // Draw white text
	display.setCursor(0, 0);	   // Start at top-left corner
	display.cp437(true);
	display.setTextSize(3);
  // put your setup code here, to run once:
  pixels.begin();
  pixels_2.begin();
}

void loop() {
		display.clearDisplay();
		display.setCursor(0, 0);
		display.printf("Hello");
		display.display();

  // put your main code here, to run repeatedly:
    pixels.clear(); // Set all pixel colors to 'off'
    pixels_2.clear(); // Set all pixel colors to 'off'

  // The first NeoPixel in a strand is #0, second is 1, all the way up
  // to the count of pixels minus one.
  for(int i=0; i<NUMPIXELS; i++) { // For each pixel...

    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    pixels.setPixelColor(i, pixels.Color(30, 30, 30));
    pixels_2.setPixelColor(i, pixels_2.Color(0, 30, 30));

    pixels.show();   // Send the updated pixel colors to the hardware.
    pixels_2.show();   // Send the updated pixel colors to the hardware.

    delay(DELAYVAL); // Pause before next pass through loop
  }
}