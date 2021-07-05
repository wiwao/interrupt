#include <WioLTEforArduino.h>
#include <stdio.h>

#define APN               "soracom.io"
#define USERNAME          "sora"
#define PASSWORD          "sora"
#define WEBHOOK_EVENTNAME "ENTER_YOUR_WEBHOOK_EVENTNAME_HERE"
#define WEBHOOK_KEY       "ENTER_YOUR_WEBHOOK_KEY_HERE"
#define WEBHOOK_URL       "https://maker.ifttt.com/trigger/"WEBHOOK_EVENTNAME"/with/key/"WEBHOOK_KEY
#define INTERVAL          (60000)

// You can use WIOLTE_(D20|A4|A6) with `Wio.PowerSupplyGrove(true);`
#define BUTTON_PIN  (WIOLTE_D38)

WioLTE Wio;
volatile bool State = false;

void post_to_ifttt()
{
  char data[100];
  int status;
  State = false;

  SerialUSB.println("### Power supply ON.");
  Wio.PowerSupplyLTE(true);
  delay(500);

  SerialUSB.println("### Turn on or reset.");
  if (!Wio.TurnOnOrReset()) {
    SerialUSB.println("### ERROR! ###");
    return;
  }

  SerialUSB.println("### Connecting to \""APN"\".");
  if (!Wio.Activate(APN, USERNAME, PASSWORD)) {
    SerialUSB.println("### ERROR! ###");
    return;
  }

  SerialUSB.println("### Post.");
  sprintf(data, "{\"value1\":\"uptime %lu\"}", millis() / 1000);
  SerialUSB.print("Post:");
  SerialUSB.println(data);
  if (!Wio.HttpPost(WEBHOOK_URL, data, &status)) {
    SerialUSB.println("### ERROR! ###");
    delay(INTERVAL);
  }

  SerialUSB.print("Status:");
  SerialUSB.println(status);
  SerialUSB.println("### Power supply OFF.");

  Wio.Deactivate();  // Deactivate a PDP context. Added at v1.1.9
  Wio.TurnOff(); // Shutdown the LTE module. Added at v1.1.6
  Wio.PowerSupplyLTE(false); // Turn the power supply to LTE module off

  attachInterrupt(BUTTON_PIN, change_state, RISING);
}
void change_state()
{
  detachInterrupt(BUTTON_PIN);
//  SerialUSB.println("Interrupt happen");
  State = true;
}

void setup()
{
  delay(200);

  SerialUSB.println("");
  SerialUSB.println("--- START ---");
  SerialUSB.println("### I/O Initialize.");
  Wio.Init();
  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(BUTTON_PIN, change_state, RISING);
}

void loop()
{
  if (State) {
    post_to_ifttt();
  }
  else {
    delay(1000);
  }
}
