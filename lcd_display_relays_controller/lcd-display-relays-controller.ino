#include <Wire.h>
#include <LiquidCrystal.h>
 
// Setting the LCD shields pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

typedef struct {
  uint8_t id;
  char* name;
  char* status;
} menu;

menu menuItems[] {
  {0, "Option1", "on"},
  {1, "Option2", "off"},
  {2, "Option3", "on"},
  {3, "Option4", "on"},
};

 
int menuLen = (sizeof(menuItems)/sizeof(menuItems[0]));
int selectedItem = 0;
String btn;
boolean mainMenu = true;
boolean subMenu = false;
//  
String subMenuSelector = "on";
boolean activeSelectorRL = false;



// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRight  0
#define btnUp     1
#define btnDown   2
#define btnLeft   3
#define btnSelect 4
#define btnNone   5


// Creates 3 custom characters for the menu display
byte menuCursor[8] = {
  B01000, //  *
  B00100, //   *
  B00010, //    *
  B00001, //     *
  B00010, //    *
  B00100, //   *
  B01000, //  *
  B00000  //
};

byte downArrow[8] = {
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b10101, // * * *
  0b01110, //  ***
  0b00100  //   *
};
 
byte upArrow[8] = {
  0b00100, //   *
  0b01110, //  ***
  0b10101, // * * *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100  //   *
};


// releys
const int relay0 = 35;
const int relay1 = 39; 
const int relay2 = 43;
const int relay3 = 47; 



void setup() {
    // Initializes serial communication
  Serial.begin(9600);
  Serial.println("Starting...");
 
  // Initializes and clears the LCD screen
  lcd.begin(16, 2);
  lcd.clear();
  delay(200);

  lcd.setCursor(1, 0); // Set cursor to the bottom line
  lcd.print("Relay Menu");
  lcd.setCursor(1, 1); // Set cursor to the bottom line
  lcd.print("Starting...");
  delay(1000);
  lcd.clear();

 
  // Creates the byte for the 3 custom characters
  lcd.createChar(0, menuCursor);
  lcd.createChar(1, upArrow);
  lcd.createChar(2, downArrow);

//  initialize digital pin as an output
  pinMode(relay0, OUTPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);

  relayDefaultStatus();
  Serial.println("menu is running");
 
  delay(100);
}


void loop() {

  if (mainMenu) {
    if (btn != "") {
      if (btn == "btnSelect") {
          mainMenu = !mainMenu; 
      } else if (btn == "btnUp" && selectedItem > 0) { 
        selectedItem = selectedItem - 1;
      } else if (btn == "btnDown" && selectedItem < menuLen - 1) {
        selectedItem = selectedItem + 1;
      }
    }
    mainMenuDraw();
  } else {

    if (!activeSelectorRL) {
      if (menuItems[selectedItem].status == "on" ) {
        subMenuSelector = "on";
      } else if ( menuItems[selectedItem].status == "off" ) {
        subMenuSelector = "off";
      } 
    }
 

    if (btn != "") {
      if (btn == "btnLeft" && subMenuSelector == "off") {
        subMenuSelector = "on";
        activeSelectorRL = !activeSelectorRL;
      } else if (btn == "btnRight" && subMenuSelector == "on") { 
        subMenuSelector = "off";
        activeSelectorRL = !activeSelectorRL;
      } else if (btn == "btnUp") { 
        mainMenu = !mainMenu;
        activeSelectorRL = !activeSelectorRL;
      } else if (btn == "btnSelect") {
        mainMenu = !mainMenu;
        updateDict();
        releyStatus();
        activeSelectorRL = !activeSelectorRL;

        Serial.print("changing Dictionary status for: ");
        Serial.println(menuItems[selectedItem].name);

      }
    }  
    subMenuDraw();
  }
  
  // delay(200);

  btn = operateMenu();
  delay(200);
  lcd.clear();
 

}


String operateMenu(){
  adc_key_in = analogRead(0);      // read the value from the sensor 
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) {return "";} // We make this the 1st option for speed reasons since it will be the most likely result
 // For V1.1 us this threshold
 if (adc_key_in < 50) {return "btnRight";}  
 if (adc_key_in < 250) {return "btnUp";}
 if (adc_key_in < 450) {return "btnDown";} 
 if (adc_key_in < 650) {return "btnLeft";}
 if (adc_key_in < 850) {return "btnSelect";}
 return "none";  // when all others fail, return this...
}


void mainMenuDraw(){

  setArrow();

  if (selectedItem == menuLen - 1) {
    lcd.setCursor(1, 0); // Set cursor to the upper line
    lcd.print(menuItems[selectedItem].name);

    lcd.setCursor(12, 0); // Set cursor to the upper line
    lcd.print(menuItems[selectedItem].status);
  
    lcd.setCursor(4, 1); // Set cursor to the bottom line
    lcd.print("end");

//    set selector
    lcd.setCursor(0, 0);
    lcd.write(byte(0));

  } else {
    
    lcd.setCursor(1, 0); // Set cursor to the upper line
    lcd.print(menuItems[selectedItem].name);

    lcd.setCursor(12, 0); // Set cursor to the upper line
    lcd.print(menuItems[selectedItem].status);
  
    lcd.setCursor(1, 1); // Set cursor to the bottom line
    lcd.print(menuItems[selectedItem+1].name);

    lcd.setCursor(12, 1); // Set cursor to the upper line
    lcd.print(menuItems[selectedItem+1].status);

    lcd.setCursor(0, 0);
    lcd.write(byte(0)); 
    
 }
       
}


void setArrow() {
  if (selectedItem == 0 ) {
    lcd.setCursor(15, 1);
    lcd.write(byte(2)); 
  } else if (selectedItem == menuLen - 1 ) {
    lcd.setCursor(15, 0);
    lcd.write(byte(1)); 
  } else {
    lcd.setCursor(15, 0);
    lcd.write(byte(1));
    lcd.setCursor(15, 1);
    lcd.write(byte(2));  
  }
  
}



void subMenuDraw(){
  lcd.setCursor(0, 0); // Set cursor to the upper line
  lcd.print(menuItems[selectedItem].name);

  lcd.setCursor(1, 1); // Set cursor to the upper line
  lcd.print("on");

  lcd.setCursor(13, 1); // Set cursor to the bottom line
  lcd.print("off");

  if (subMenuSelector == "on") {
    lcd.setCursor(0, 1);
    lcd.write(byte(0));
  } else {
    lcd.setCursor(12, 1);
    lcd.write(byte(0));
  }

}


void updateDict() {

  char* newStatus;

   if (subMenuSelector == "on") {
    newStatus = "on";
   } else {
    newStatus = "off";
   }


  if (menuItems[selectedItem].status != newStatus) {
    if (menuItems[selectedItem].status == "on") {
      menuItems[selectedItem].status = newStatus;
    } else {
      menuItems[selectedItem].status = newStatus;
    }
  }
     
}


void relayDefaultStatus() {
//  relay 0
  if (menuItems[0].status == "on") {
    digitalWrite(relay0, LOW); 
  } else {      
    digitalWrite(relay0, HIGH);
  }

//  relay 1

  if (menuItems[1].status == "on") {
    digitalWrite(relay1, LOW); 
  } else {      
    digitalWrite(relay1, HIGH);
  }  

//  relay 2

  if (menuItems[2].status == "on") {
    digitalWrite(relay2, LOW); 
  } else {      
    digitalWrite(relay2, HIGH);
  } 

//  relay 3

  if (menuItems[3].status == "on") {
    digitalWrite(relay3, LOW); 
  } else {      
    digitalWrite(relay3, HIGH);
  }   
    
}


void releyStatus(){

  // relay 0
  if (menuItems[selectedItem].id == 0) {
    if (menuItems[selectedItem].status == "on" ) {
      digitalWrite(relay0, LOW); 
    } else {
      digitalWrite(relay0, HIGH);
    }   
  }


  // relay 1
  if (menuItems[selectedItem].id == 1) {
    if (menuItems[selectedItem].status == "on" ) {
      digitalWrite(relay1, LOW); 
    } else {
      digitalWrite(relay1, HIGH);
    }   
  }


  // relay 2
  if (menuItems[selectedItem].id == 2) {
    if (menuItems[selectedItem].status == "on" ) {
      digitalWrite(relay2, LOW); 
    } else {
      digitalWrite(relay2, HIGH);
    }   
  }


   // relay 3
  if (menuItems[selectedItem].id == 3) {
    if (menuItems[selectedItem].status == "on" ) {
      digitalWrite(relay3, LOW); 
    } else {
      digitalWrite(relay3, HIGH);
    }   
  } 

  Serial.print("Changing ralay status: ");
  Serial.print(menuItems[selectedItem].name);
  Serial.print(" - new status: ");
  Serial.println(menuItems[selectedItem].status);

}
