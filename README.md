# washing-machine-monitor
This project uses an arduino to monitor a washing machine by it's vibrations. When it's been running and then stops, after a time interval, it'll send you an SMS letting you known it's done. It'll continue to send an SMS every 10 minutes (or whatever you set) until you press a button on the arduino. This project requires creating a [ThinkSpeak](https://thingspeak.com) account and a [Twilio](https://www.twilio.com) account. You can use free accounts for both.

### Hardware
Here's the hardware used in the project:

1. Adafruit Feather HUZZAH ESP8266 - (https://learn.adafruit.com/adafruit-feather-huzzah-esp8266/overview)
2. FeatherWing OLED - (https://learn.adafruit.com/adafruit-oled-featherwing/pinouts?view=all)
3. Mercury Tilt Switch - (http://www.elechouse.com/elechouse/index.php?main_page=product_info&cPath=152_161&products_id=273&zenid=8olb322gkls2cj0qtbbcm2aui7)
4. Button (1) - (https://www.adafruit.com/products/367)
5. 10k resister (1) - (https://www.adafruit.com/products/2784)
6. Jumper wires - (https://www.adafruit.com/products/758)

OPTIONAL
I'm also creating a custom 3d printed housing for this so it looks a little nicer. Once it's all done, I'll post the files on Thingiverse and link to them from here.

### Configuration
After cloning the repository, create a new file called `Configuration.h` in the same folder. Put the following in the file, but fill in the variables with your own settings.

```
// Configuration.h

// Think Speak Settings
#define API_KEY "YOUR_THINKSPEAK_API_KEY"
#define SEND_NUMBER "+15555555555"

// WIFI Settings
#define WIFI_NETWORK "YOUR WIFI NETWORK NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// PIN definitions
#define VIBRATION_PIN 14
#define BUTTON_PIN 12
```
