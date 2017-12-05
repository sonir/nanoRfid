#include <SPI.h>
#include <MFRC522.h>

//SETUPS
#define LOOP_INTERVAL 100 //

//LED
#define LED_PIN  7
//RFID PINS
#define RST_PIN  9
#define SS_PIN   10

//id available detection
#define available_THRESHOLD 10
int available_counter = 0;

//Create MFRC522 instance
MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

//UID
//UID buffer
int UID_BUF[7];

//CID
//currentTag's CID(Custom ID)
int CID_val;
//preTag's CID
int pre_val;

//Tag's transion status
boolean tag_transition = false;

boolean attack_detection = false;

int flag = 0;

//Counter to delay loop
int count;


void setup() {
  //SERIAL SETUP
  Serial.begin(9600);
  while (!Serial);

  //RFID and SPI SETUP
  //Init SPI bus
  SPI.begin();
  //Init MFRC522
  mfrc522.PCD_Init();

  //PIN SETUP
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  //Increment count for timer
  count ++;
  if (count > LOOP_INTERVAL) {
    count = 0;
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    //tag_transition = transition_finder(pre_val);
    //if Tag is on the Reader
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      available_counter = 0;
      digitalWrite(LED_PIN, HIGH);
      Serial.write(CID_val);
    } else {
      available_counter ++;
      //Serial.println(available_counter);
      if (available_counter > available_THRESHOLD) {      
        Serial.write(0);
        digitalWrite(LED_PIN, LOW);
      }
    }
  }
}


//read tag value and convert to int
void dump_byte_array(byte * buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    UID_BUF[i] = buffer[i];
  }
  CID_val = UID_BUF[1] % 10;
}

//compare preTag's value and currentTag's value
bool transition_finder(int val) {
  if (val !=  CID_val) {
    return true;
  } else {
    return false;
  }
}

//LED blink only one time
void led_blink() {
  digitalWrite(LED_PIN, HIGH);
  delay(50);
  digitalWrite(LED_PIN, LOW);
  delay(50);
}



