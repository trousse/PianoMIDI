#include <Arduino.h>
#include <ArduinoJson.h>
#include <Wire.h>

// slave address
#define SLAVE_ADDRESS 0x12

String str = "";
bool first_row = true;

//counter of row (before data processing)
int row_count = 0; //

// this variable will tell the program to start a new game
bool start = false;
bool finish = false;

// declare analog pins
static int analogPin[6] = {0, 1, 2, 3, 4, 5};

// number of Pin (for the leds)
static int numberOfPin = 6;
// number of led rows
static int Led_rows = 2;

// output data that corresponds to pushed or not pushed (respectively 1 or 0)
int *digitalData = NULL;

// arrays where the comming data from the raspberry will be saved
int *array_temp_prev = NULL;
// LED range 1 (first row)
int *array_led1 = NULL;
// LED range 2 (row below the first row)
int *array_led2 = NULL;
//current data that the user need to do
int *array_temp_next = NULL;
//data recived from the raspberry
int* data = NULL;
//result of the user command with the array_temp_next
//it is the array that will be sent to the raspberry
bool *comparision_array = NULL;
//array for start message
char* tab = NULL;

int* test = (int*)malloc(6 * sizeof(int));
bool startMessagePassed = false;
bool stopMessage = false;

//NO LONGER USED
// variables for calibrating the LDR
int sensorValue = 0;  // the sensor value
int sensorMin = 1023; // minimum sensor value
int sensorMax = 0;    // maximum sensor value
//seuil = rapport d'erreur de 10%
static int seuil = 0.1;
int* calib_data_max = NULL;
int* calib_data_min = NULL;

//***********************************************************************************

//function that copies array2 values to array1
void copy_array(int* array1, int* array2) {
  for (int i = 0; i < numberOfPin; i++) {
    array1[i] = array2[i];
  }
}

//function that initialize the led's array to 0
void initialize_leds_arrays() {
  for (int i = 0; i < numberOfPin; i++) {
    array_temp_prev[i] = 0;
    array_led1[i] = 0;
    array_led2[i] = 0;
    array_temp_next[i] = 0;
  }

}

//NO LONGER USED
// function to calibrate the LDR sensor (copy-paste from internet)
void calibration_sensor(int sensorPin) {
  Serial.println("sensors calibration :");
  int i = 10;
  calib_data_max = (int*)malloc(numberOfPin * sizeof(int));
  calib_data_min = (int*)malloc(numberOfPin * sizeof(int));
  calib_data_min[sensorPin] = 0;
  calib_data_max[sensorPin] = 1023;

  // calibrate during the first five seconds
  while (millis() < 5000) {
    Serial.println(i);
    sensorValue = analogRead(sensorPin);
    //get all the sensorMax value of each photoresistor

    // record the maximum sensor value
    if (sensorValue > sensorMax) {
      sensorMax = sensorValue;
    }

    // record the minimum sensor value
    if (sensorValue < sensorMin) {
      sensorMin = sensorValue;
    }

    // record the maximum sensor value for each sensor
    if (sensorValue > calib_data_max[sensorPin]) {
      calib_data_max[sensorPin] = sensorValue;

    }
    // record the minimum sensor value for each sensor
    if (sensorValue < calib_data_min[sensorPin]) {
      calib_data_min[sensorPin] = sensorValue;
    }
    i--;
  }
  Serial.println("min : " + String(sensorMin));
  Serial.println("max : " + String(sensorMax));

}

//function that send data to the raspberr4y
void sendData(String data) {
  Wire.write(data.c_str());
}

// function that compares the user command with array_temp_next array and fill the result "true" or "false in the comparision_array
void comparision() {
  for (int i = 0; i < numberOfPin; i++) {
    if (digitalData[i] == array_temp_next[i])
      comparision_array[i] = true;
    else
      comparision_array[i] = false;
  }
}


//get the user's data
void get_user_data_button() {
  // begin to play if start and !finish
  // if (start == true && finish != true) {
  // get the data of each sensors
  for (int i = 0; i < numberOfPin; i++) {
    //get the analog value
    int val = analogRead(analogPin[i]);
    // if the ldr is pushed (so 1 => correct)
    if (val == 1023) {
      digitalData[i] = 1; //pushed
      Serial.println("sensor " + String(i) + " pushed");
    }
    else {
      digitalData[i] = 0; //not pushed
      Serial.println("sensor " + String(i) + " not pushed");
    }

    str += String(digitalData[i]);
    if (i != numberOfPin - 1) str += " ";
  }
  Serial.println("user command : " + str);
  str = "";
}

//NO LONGER USED
//function that get data from the photoresistor
void get_user_data_ldr() {
  // begin to play if start and !finish
  // if (start == true && finish != true) {
  // get the data of each sensors
  Serial.println("sensorMin " + sensorMin);
  Serial.println("sensorMin " + sensorMax);
  for (int i = 0; i < numberOfPin; i++) {
    //get the analog value
    int val = analogRead(analogPin[i]);
    //map it between sensorMin and sensorMax in function of the scale of the sensor
    Serial.println("calib_data_min " + calib_data_min[i]);
    Serial.println("calib_data_max " + calib_data_max[i]);
    val = map(val, calib_data_min[i], calib_data_max[i], sensorMin, sensorMax);
    // if the ldr is pushed (so 1 => correct)
    if (val >= 0 && val < sensorMax) {
      digitalData[i] = 1; //pushed
      Serial.println("sensor " + String(i) + " pushed");
    }
    else {
      digitalData[i] = 0; //not pushed
      Serial.println("sensor " + String(i) + " not pushed");
    }

    str += String(digitalData[i]);
    if (i != numberOfPin - 1) str += " ";
  }
  Serial.println("user command : " + str);
  str = "";
}

//switch the data over each led's array and change the array_temp_prev
void switch_led_arrays() {
  copy_array(array_temp_next, array_led2);
  copy_array(array_led2, array_led1);
  copy_array(array_led1, array_temp_prev);
  Serial.println("LEDs switched !");
}

//function that applies led's arrays
void apply_led_arrays() {
  // apply array_led1
  //row led1 : 2 => 8
  for (int i = 0; i < numberOfPin; i++) {
    digitalWrite(i + 2, array_led1[i]);
  }
  // apply array_led2
  for (int i = 0; i < numberOfPin; i++) {
    digitalWrite(i + 2 + numberOfPin, array_led2[i]);
  }
  //Serial.println("Arrays applied to LED1 and LED2 !");
  delay(1000);//>>>>>>>>>change the position of this
}

//function that create and fill the json
void create_send_json() {
  StaticJsonBuffer<20> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  //fill it with the result of the comparision
  json["A0"] = digitalData[0];
  json["A1"] = digitalData[1];
  json["A2"] = digitalData[2];
  json["A3"] = digitalData[3];
  json["A4"] = digitalData[4];
  json["A5"] = digitalData[5];
  Serial.println("JSON created !");
  //Serial.println(json);
  //Wire.print(json);
}

//begin to treat the data after received 'start"
// the data received are like "0 1 1 1 0 1"
//we will use the "data" array
void receiveData() {
  //if this is the start of the game (is executed one time ONLY)
  if (!startMessagePassed) {
    Serial.println("boucle startMessagePassed");
    int u = 0;
    while (Wire.available()) {
      tab[u] = (char)Wire.read();
      Serial.println(tab[u]);
      u++;
    }
      if(isMessageStart(tab)){
        startMessagePassed = true;
        Serial.println("Start Message => OK");
      }
      else {
        Serial.println("Please begin with a Start Message First");
      }
    
  }
  //if the start message has been executed
  else if (startMessagePassed) {
    Serial.println("other boucle");
    int v = 0;
    //get the line
    while (Wire.available()) {
      //fill the data array
      data[v] = (char)Wire.read();
      v++;
    }
    //check if there is a stop message
    if (isMessageStop(data)) {
      //reinitialize the led and apply the values
      initialize_leds_arrays();
      apply_led_arrays();
      //reinitialize startMessagePassed
      startMessagePassed = false;
      //reinit the counter of rows
      row_count = 0;
    }
    else {
      test_displayData(data);
      copy_array(array_temp_prev, data);
      //switch the led's array (only the data structure)
      switch_led_arrays();
      //apply the led's array
      apply_led_arrays();
      if (row_count >= Led_rows) {
        Serial.println("row > 2");
        //get the data from the user
        get_user_data_button();
        //compare the array_temp_next with the user command
        comparision();
        test_comparision();
        //create json file
        create_send_json();
        row_count++;
      }
      else row_count++;
    }
  }
}
//***********************************************************

void setup() {
  // start the serial port with a baud rate of 9600
  Serial.begin(9600);

  // slave address
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);

  // dynamic declarations of all arrays
  digitalData = (int *)malloc(numberOfPin * sizeof(int));
  array_temp_prev = (int *)malloc(numberOfPin * sizeof(int));
  array_led1 = (int *)malloc(numberOfPin * sizeof(int));
  array_led2 = (int *)malloc(numberOfPin * sizeof(int));
  array_temp_next = (int *)malloc(numberOfPin * sizeof(int));
  comparision_array = (bool *)malloc(numberOfPin * sizeof(bool));
  data = (int *)malloc(numberOfPin * sizeof(int));
  tab = (char*)malloc(5 * sizeof(char));
  /*
    // calibrate all the sensors (just need one sensor) for example A0
    for (int i = 0; i < numberOfPin; i++) {
      calibration_sensor(analogPin[0]);
    }
    test_calib_data();
  */
  initialize_leds_arrays();

  //define the digital pin as output
  for (int i = 2; i <= 13; i++) {
    pinMode(i, OUTPUT);
  }
}

//****************************************************************

int* create_fake_data() {
  int rand_int;
  int* tab = (int*)malloc(sizeof(int));
  for (int i = 0; i < numberOfPin; i++) {
    rand_int = random(0, 2);
    tab[i] = rand_int;
    Serial.print(tab[i]);
  }
  Serial.println("fake data created !");
  return tab;
}

//****************************************************************
void fake_data() {
  int* data = create_fake_data();

  test_displayData(data); // OK
  row_count++;
  // process the data
  copy_array(array_temp_prev, data);
  //test_display_array_temp_prev(); // OK
  //test_display_array_led1(); // OK
  //test_display_array_led2(); // OK
  //test_display_array_temp_next(); // OK
  switch_led_arrays(); // OK
  apply_led_arrays(); //OK mais peut etre à modifié (en fonction des pins)
  if (row_count > Led_rows) {
    get_user_data_button();// OK
    comparision(); // OK
    test_comparision();// OK
    create_send_json();
  }

}

//***********************************************************************

//TESTS Bellow

void test_displayData(int* data) {
  Serial.print("Fake Data Received : ");
  for (int i = 0; i < numberOfPin; i++) {
    Serial.print(data[i]);
  } Serial.print("\n");
}

void test_display_array_temp_prev() {
  Serial.print("array_temp_prev : ");
  for (int i = 0; i < numberOfPin; i++) {
    Serial.print(array_temp_prev[i]);
  } Serial.print("\n");
}

void test_display_array_led1() {
  Serial.print("array_led1 : ");
  for (int i = 0; i < numberOfPin; i++) {
    Serial.print(array_led1[i]);
  } Serial.print("\n");

}

void test_display_array_led2() {
  Serial.print("array_led2 : ");
  for (int i = 0; i < numberOfPin; i++) {
    Serial.print(array_led2[i]);
  } Serial.print("\n");

}

void test_display_array_temp_next() {
  Serial.print("array_temp_next : ");
  for (int i = 0; i < numberOfPin; i++) {
    Serial.print(array_temp_next[i]);
  } Serial.print("\n");
}

void test_calib_data() {
  Serial.print("");
  for (int i = 0; i < numberOfPin; i++) {
    Serial.print(calib_data_max[i]);
  } Serial.print("\n");
}

void test_sensor() {
  int val = analogRead(0);
  if (val == 1023) {
    digitalData[0] = 1; //pushed
    Serial.println("sensor " + String(0) + " pushed");
  }
  else {
    digitalData[0] = 0; //not pushed
    Serial.println("sensor " + String(0) + " not pushed");
  }

}

void test_comparision() {
  Serial.println("Comparision");
  for (int i = 0; i < numberOfPin; i++) {
    Serial.print(String(comparision_array[i]) + " ");
  }
  Serial.print("\n");
}


//*******************************************************************
void loop() {

}

bool isMessageStart(char tab []) {
  String str = "start";
  for (int i = 0; i < str.length(); i++) {
    if (str[i] != tab[i]) return false;
  }
  return true;
}

bool isMessageStop(int tab []) {
  String str = "stop";
  for (int i = 0; i < str.length(); i++) {
    if (str[i] != (char)tab[i]) return false;
  }
  return true;
}

bool isData(char tab[]) {
  if (strlen(tab) == 6) return true;
  return false;
}

/*
  String data1 = "0 0 1 0 1 0";
  while(1){
  Wire.write(data1.c_str());
  delay(1000);
  }
*/


//fake_data();
//test_sensor();


