#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

const int button_pins[] = { A1, A2, A3, A4 };
const int buzz_pin = A0;
const int rst_pin = 9;
const int sda_pin = 10;
const String tag = "113 89 174 38";
const String pin = "1234";
MFRC522 mfrc522(sda_pin, rst_pin);
LiquidCrystal lcd(8, 7, 2, 3, 4, 5);

/********************************************************************************/

void setup() {
    for (int btn : button_pins) {
        pinMode(btn, INPUT_PULLUP);
    }

    Serial.begin(9600);
    lcd_setup();
    spi_setup();

    pinMode(buzz_pin, OUTPUT);
    lcd.print("Welcome!");
}

void loop() {
    if (detect_tag()) {
        tone(buzz_pin, 640, 250);
        check_pin(lcd_write());
    }
}

/*******************************************************************************
*   LCD
*/
void lcd_setup() {
    lcd.begin(16, 2);
    lcd.clear();
}

String lcd_write() {
    String str = "";
    lcd.clear();
    lcd.print("Enter PIN: ");

    while (str.equals("")) {
        str = press_buttons();
    }
    lcd.clear();
    return str;
}

/*******************************************************************************
*   SPI and RFID
*/
void spi_setup() {
    SPI.begin();
    mfrc522.PCD_Init();
}

boolean detect_tag() {
    String rfid_tag = "";
    if (mfrc522.PICC_IsNewCardPresent()) {
        if (mfrc522.PICC_ReadCardSerial()) {
            for (byte i = 0; i < mfrc522.uid.size; i++) {
                rfid_tag.concat(mfrc522.uid.uidByte[i]);
                rfid_tag.concat(" ");
            }
        }
    }

    rfid_tag.remove(rfid_tag.length() - 1);
    if (rfid_tag.equals(tag)) {
        Serial.println("Enter PIN");
        return true;
    }
    return false;
}

/********************************************************************************/
boolean check_pin(String data) {
    if (data.equals(pin)) {
        lcd.clear();
        lcd.print("Access Granted");
        delay(2500);
        lcd.clear();

        tone(buzz_pin, 540, 1000);
        lcd.print("Welcome!");
        return true;
    }

    lcd.print("Access Denied");
    delay(2500);
    lcd.clear();
    lcd.print("Welcome!");
    return false;
}


String press_buttons() {
    String password = "";
    while (true) {
        float btn1 = analogRead(button_pins[0]) * 5.0 / 1023;
        float btn2 = analogRead(button_pins[1]) * 5.0 / 1023;
        float btn3 = analogRead(button_pins[2]) * 5.0 / 1023;
        float btn4 = analogRead(button_pins[3]) * 5.0 / 1023;

        if (btn1 < 3) password.concat("1");
        if (btn2 < 3) password.concat("2");
        if (btn3 < 3) password.concat("3");
        if (btn4 < 3) password.concat("4");
        Serial.println(password); delay(500);
        lcd.setCursor(0, 1);
        lcd.print(password);
        if (password.length() == 4) {
            delay(1000);
            return password;
        }
    }
}





