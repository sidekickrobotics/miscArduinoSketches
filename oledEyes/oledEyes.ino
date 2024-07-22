//Testing Eyes program - Flux garage
//HAD TO REMOVE FAST led ASLO


//TODO:

//Version 5: add time intergave
//Version 6: add blink

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/* --- CONSTANTS will not change --------------------- */
const int inputPin = 6;      //MOOD D6 pin 12        //Pin 4 goes to D2 on Node MCU - for button switch
const int buttonInterval_ms = 1000; //Allow button to be pressed once per second
const int PRINTSERIAL = 0;
const int potPin = A2;

const int hb_LED_Pin = 2; //output 

const int laugh_Pin = 9;
const int confused_Pin = 8;
const int flicker_Pin = 7;

const int mode_Pin = 10;
const int plusplus_Pin = 11;
const int minusminus_Pin = 12;



/* --- Calibrations are to allow for flexibility ----- */
long k_patternMinTime_ms = 2000; //Make a pattern minimum time 1 second (Change to longer)
long previousMillis;
unsigned long k_readInputsTime_ms = 500; //how often the analog and switch inputs are read


//Instead of random change ever 10 seconds, change on Button press
int val = HIGH;
int key = 0; //this is key to detect the power flicker 
int prevPowerLevel = 0;
unsigned long keyTimer_ms = 0;
int potValue = 0;
unsigned long prevT_readInputs_ms = 0;    // time when button press last checked





/* --- Timing Variables */
unsigned long currentT_ms = 0;        //value of millis in each iteration of loop()
unsigned long loopT_ms = 0;           //to keep track of loop time
unsigned long prevT_Button_ms = 0;    // time when button press last checked
unsigned long prevT_TempCheck_ms = 0; //time when Temp was last checked
unsigned long prevT_DisplayUpdate_ms = 0;
unsigned long prevT_LEDBlinkUpdate_ms = 0;


unsigned long k_LEDBlinkUpdate_ms = 3000;

bool mode_button_pressed = 0;
bool plusplus_btn_pressed = 0;
bool minusminus_btn_pressed = 0;
bool laugh_btn_pressed = 0;
bool confused_btn_pressed = 0;
bool flicker_btn_pressed = 0;
int MODE = 0;


//Eye Variables
int x_new = 64; //start in midde - poitns will ne updated
int y_new = 32; 
int blink_rate_temp = 1;
bool curious_mode = true;

unsigned long prevT_newEyePos_ms = 0; //reset counts
unsigned long  k_newEyePos_interval_ms = 500;//set as base - moves will not be less than thsi
unsigned long  variable_interval_ms = k_newEyePos_interval_ms;  // initialize

unsigned long prevT_Blink_ms = 0; //reset counts
unsigned long  k_Blink_interval_ms = 1000;//set as base - moves will not be less than thsi
unsigned long  vari_blink_interval_ms = k_Blink_interval_ms;  // initialize


// Initialize the client library
//WiFiClient client;

void setup() {
  Serial.begin(9600);
   while (!Serial) {
    ; //wait for serial port - 
  }
  
  Serial.println("ProMini_SuperUsefulStarter_eyes_v3.ino is starting - 13JUL2024");
  Serial.println(F("Testing eyes"));
  
  delay(1000); // 2 second delay for recovery

//old display code using lchgfx
  //Initialize display
  //display.begin();
  //display.setFixedFont(ssd1306xled_font6x8);
  //display.fill( 0x00 );
  //display.printFixed(0,  0, "PMSU Eyes V3", STYLE_NORMAL);
  //pTODO: Put more splash scren here

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.display();
  delay(2000);
  display.clearDisplay();
  display.drawPixel(10,10, SSD1306_WHITE);
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("PMSU Eyes V3"));
  display.display();


  //Serial.println("after display");


  //Setup for Switch input
  pinMode(hb_LED_Pin, OUTPUT); //Heartbeat LED
  pinMode(inputPin, INPUT_PULLUP); //MOOD

  pinMode(laugh_Pin, INPUT_PULLUP);
  pinMode(confused_Pin, INPUT_PULLUP);
  pinMode(flicker_Pin, INPUT_PULLUP);
 
  pinMode(mode_Pin, INPUT_PULLUP);
  pinMode(plusplus_Pin, INPUT_PULLUP);
  pinMode(minusminus_Pin, INPUT_PULLUP);

  //Serial.println("after pin sets");
  previousMillis = millis(); //initalize timestamp variable for timing

  delay(1000); //for spash screen to stay
  display.clearDisplay(); // Clear the display buffer
  //Serial.println("Setup complete");
}//end setup


  
void loop()
{

 // Serial.println("In loop");

  blink_rate_temp = random(1,6);

  
  readButton(); //Instead of random change, change on Button press
  textDemo();
  readInputs(); //these are joystick and button inputs
  UpdateLED();
  idleEyes(x_new, y_new);
  gblink(x_new, y_new, blink_rate_temp);
  
 // Serial.print("after_functioncalls");

 
  
}//End main loop



//TODO: Change this to toggle instead of blink. 
void UpdateLED(){
  //to blink LED to make sure that the program is still running
  if (millis() - prevT_LEDBlinkUpdate_ms >= k_LEDBlinkUpdate_ms){
    //prevT_LEDBlinkUpdate_ms = millis(); //update last time
    prevT_LEDBlinkUpdate_ms = prevT_LEDBlinkUpdate_ms + k_LEDBlinkUpdate_ms;

    if(digitalRead(hb_LED_Pin)){ //if the pin is high LED is ON
      digitalWrite(hb_LED_Pin, LOW); //turn it off
    }else{
      digitalWrite(hb_LED_Pin, HIGH); //otherwise it is off, turn it on
    }
  
  }//end check
  
}//end Update LED Function


 

void readButton() {

      // only read button after interval has elapsed
      // this avoids multiple flashes if the button bounces
      // every time the button is pressed it changes buttonLed_State causing the Led to go on or off

 if (millis() - prevT_Button_ms >= k_patternMinTime_ms){
    
    val = digitalRead(inputPin);
    
    if (val == LOW){
      
      //prevT_Button_ms += buttonInterval_ms; //Reset the millis since the pattern was reset
      prevT_Button_ms = millis(); //Reset the millis since the pattern was reset
      Serial.print("Pattern Changed: ");
  
      } //End if value is LOW
    
    } //End time duration check

}//End readButton()



static void textDemo(){

   //Serial.print("in the print function");
    
  if (millis() - prevT_DisplayUpdate_ms >= 1000){
    
     //display.clearDisplay(); // Clear the display buffer  
      display.fillRect(0, 0, 128, 15, BLACK); //just clear the top part of the screen
      display.setTextSize(1);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(0,1);             // Start at top-left corner
      display.println(F("T:"));
      display.setCursor(12,1);      
      //display.println(45.0, DEC);
      display.println(variable_interval_ms, DEC);
      display.setCursor(70,1);   
      display.println(blink_rate_temp, DEC);
      display.setCursor(120,1);
      display.println(F("2"));
      display.setCursor(0,9);
      //display.println(F("ABCDEFGHIJKLMNOPQRSTU"));
      display.println(F("---------------------"));
      display.display();

      prevT_DisplayUpdate_ms = millis(); 
    }//end time duration check
}



void readInputs() {
  
  if (millis() - prevT_readInputs_ms >= k_readInputsTime_ms ){
    prevT_readInputs_ms+=k_readInputsTime_ms;
    potValue = analogRead(potPin); 

    if ((digitalRead(mode_Pin) == LOW) && (mode_button_pressed == 0)){
       Serial.print("MODE pressed: "); 
       
       mode_button_pressed = 1; //set to pressed 
       //TODO: Do something with mode button
       MODE = MODE + 1;
       Serial.println(MODE);
    } //end if MODE
    
    if ((digitalRead(laugh_Pin) == LOW) && (laugh_btn_pressed == 0)){
       Serial.println("Laugh pressed: "); 
       
       laugh_btn_pressed = 1; //set to pressed 
       //TODO: Do something with button press

    } //end laugh
    
    if ((digitalRead(confused_Pin) == LOW) && (confused_btn_pressed == 0)){
       Serial.println("Confused pressed: "); 
       
       confused_btn_pressed = 1; //set to pressed 
       //TODO: Do something with button press

    } //end confused
   
    if ((digitalRead(flicker_Pin) == LOW) && (flicker_btn_pressed == 0)){
       Serial.println("Flicker pressed: "); 
       
       flicker_btn_pressed = 1; //set to pressed 
       //TODO: Do something with button press
       testeye();

    } //end flicker

    if ((digitalRead(plusplus_Pin) == LOW) && (plusplus_btn_pressed == 0)){
       Serial.println("++ pressed: "); 
       
       plusplus_btn_pressed = 1; //set to pressed 
       //TODO: Do something with button press

    } //end plusplus

    if ((digitalRead(minusminus_Pin) == LOW) && (minusminus_btn_pressed == 0)){
       Serial.println("-- pressed: "); 
       
       minusminus_btn_pressed = 1; //set to pressed 
       //TODO: Do something with button press

    } //end flicker
    
  //  Serial.print(" Pot:");
  // Serial.println(potValue);
/*    ldrValue = analogRead(LDRPin);


      // Read all analog inputs and map them to one Byte value
  data.j1PotX = map(analogRead(A1), 0, 1023, 0, 255); // Convert the analog read value from 0 to 1023 into a BYTE value from 0 to 255
  data.j1PotY = map(analogRead(A0), 0, 1023, 0, 255);
  data.pot1 = map(analogRead(potPin), 0, 1023, 0, 255);
  // Read all digital inputs
  data.j1Button = digitalRead(jB1);
  

  Serial.print(" Pot:");
  Serial.print(potValue);
  Serial.print(", ldr:"); //use this format to make the serial plotter work better
  Serial.print(ldrValue);
  Serial.print(", X:");
  Serial.print(data.j1PotX);
  //Serial.print(" <X Y> "); //dont do it like this for serial monitor.
  Serial.print(", Y:");
  Serial.print(data.j1PotY);
  Serial.print(", B:");
  Serial.print(data.j1Button);
  Serial.print(", Bright:");
  Serial.print(brightness);
  Serial.println(" ");
  */

  }

  resetInputs();
  
}//end read Inputs


void resetInputs(){


  laugh_btn_pressed = 0;
  confused_btn_pressed = 0;
  flicker_btn_pressed = 0;

  mode_button_pressed = 0;
  plusplus_btn_pressed = 0; 
  minusminus_btn_pressed = 0;
 

} //end reset inputs

void testeye(){
//void drawRoundRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t radius, uint16_t color);
//void fillRoundRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t radius, uint16_t color);
  
  int x0 = 20;
  int y0 = 20;
  int w = 20;
  int h = 30;
  int radius = 5;
  
  display.fillRoundRect(x0, y0, w,  h, radius, WHITE);
  display.display();
  delay(500);
}

void drawEyes(int xcenter, int ycenter, uint16_t color){
  //Based on center point, draw two eyes
  //rounded rectangle
  
  int eye_spread = 6;
  int eye_width = 20;
  int eye_height = 30;
  int radius = 7;
  int left_eye_pos_x = xcenter - eye_width - (eye_spread/2); 
  int left_eye_pos_y = ycenter - (eye_height/2); 
  int right_eye_pos_x = xcenter + eye_spread/2; 
  int right_eye_pos_y = ycenter - (eye_height/2); 

  //draw a single center pixed
  if(0){
    display.drawPixel(xcenter, ycenter, WHITE);
    display.drawPixel(10, 10, WHITE);
    display.drawPixel(118, 10, WHITE);

  }//end if to draw center points

  //draw the left eye
  display.fillRoundRect(left_eye_pos_x, left_eye_pos_y, eye_width, eye_height, radius, color);
  
  //draw the right eye
  display.fillRoundRect(right_eye_pos_x, right_eye_pos_y, eye_width, eye_height, radius, color);
  display.display();

  
}//end draw eyes

void drawEyesCurious(int xcenter, int ycenter, uint16_t color){
  //Based on center point, draw two eyes
  //rounded rectangle
  
  int eye_spread = 6;
  int eye_width = 20;
  int eye_height = 30;
  int radius = 7;
  int left_eye_pos_x = xcenter - eye_width - (eye_spread/2); 
  int left_eye_pos_y = ycenter - (eye_height/2); 
  int right_eye_pos_x = xcenter + eye_spread/2; 
  int right_eye_pos_y = ycenter - (eye_height/2); 

  //draw a single center pixed
  if(0){
    display.drawPixel(xcenter, ycenter, WHITE);
    display.drawPixel(10, 10, WHITE);
    display.drawPixel(118, 10, WHITE);

  }//end if to draw center points

  if (curious_mode){ //if curious mode
    if(xcenter > 64){ //on right side of display, make the right eye bigger
      
      display.fillRoundRect(left_eye_pos_x, left_eye_pos_y, eye_width, eye_height, radius, color); //draw the left eye
      display.fillRoundRect(right_eye_pos_x-2, right_eye_pos_y-2, eye_width+2, eye_height+4, radius, color); //draw the right eye
      display.display();      
      
    }else if(xcenter <=63){ // its is on left side
      
      display.fillRoundRect(left_eye_pos_x, left_eye_pos_y-2, eye_width+2, eye_height+4, radius, color);
      display.fillRoundRect(right_eye_pos_x, right_eye_pos_y, eye_width, eye_height, radius, color);
      display.display(); 
         
    }else{ //no modification
      
      display.fillRoundRect(left_eye_pos_x, left_eye_pos_y, eye_width, eye_height, radius, color);
      display.fillRoundRect(right_eye_pos_x, right_eye_pos_y, eye_width, eye_height, radius, color);
      display.display();       
    }
  } else{ //NORMAL MODE end if curiuous - normal mode
      
      display.fillRoundRect(left_eye_pos_x, left_eye_pos_y, eye_width, eye_height, radius, color); //draw the left eye
      display.fillRoundRect(right_eye_pos_x, right_eye_pos_y, eye_width, eye_height, radius, color); //draw the right eye
      display.display();
  }// end if currious or not



  
}//end draw eyes


void drawTweenEyesNL(int px1, int py1, int px2, int py2, int num_inter_pts){

  int i=0; //counting variable - be sure to initialize to zero 

  int DEBUG = 1;

  int xarray[num_inter_pts]; //create array to store the points - first calc, then use, don't calc on fly
  int yarray[num_inter_pts]; 

  int x_dist_to_cover = px2-px1; //diff in x points, TODO: Need to do abs? 
  int y_dist_to_cover = py2-py1;
  int x_to_add = 0; //temporary accumulation variable - initialize to zero
  int y_to_add = 0;
  
  //for troubleshooting, show line that is to be followed
  if (0){
    display.drawLine(px1, py1, px2, py2, WHITE);
    display.display();
    delay(500); //show for only a short period of time
    
  }

  for (i=0; i<num_inter_pts ; i++){
    x_to_add = x_to_add +int(x_dist_to_cover/3); //TODO: Why 3 - this seems to work
    y_to_add = y_to_add +int(y_dist_to_cover/3); //TODO: Why 3 - this seems to work
    xarray[i] = px1 + x_to_add;//start from first point and add increasing amount at decreasing rate
    yarray[i] = py1 + y_to_add;

    //update distance to cover
    x_dist_to_cover = px2-xarray[i]; //reduce teh distance to cover by the points just calculated
    y_dist_to_cover = py2-yarray[i];

    if (DEBUG){
      Serial.print("Distance to Cover [");
      Serial.print(i);
      Serial.print("] = ");
      Serial.print(x_dist_to_cover);
      Serial.print(",");
      Serial.println(y_dist_to_cover);     
    }//end print for DEBUG   
  }//end for to calcuate the x and y array points

  //now that points have been calculated, interate to move the eyes
  for (i=0; i<num_inter_pts ; i++){

    //display.clearDisplay(); //TODO investigate just drawing over the last set of eyes
    
    //void fillRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color);
    //only fill the lower half of the screen
    display.fillRect(0, 17, 128, 64-17, BLACK);
    //drawEyes(xarray[i], yarray[i], WHITE); 
    drawEyesCurious(xarray[i], yarray[i], WHITE); 
    display.display();
    if (0){
      Serial.print("Point[");
      Serial.print(i);
      Serial.print("] = ");
      Serial.print(xarray[i]);
      Serial.print(",");
      Serial.println(yarray[i]);
    }//end if   
    
  }//end for to draw eyes along path

  
}//end function drawTweenEyesNL - Non Linear

void idleEyes(int x_pos, int y_pos){
  ///Idel eyes cinds a random po

  //need two oriinal points

  if (millis() - prevT_newEyePos_ms >= variable_interval_ms){
      prevT_newEyePos_ms = millis(); //reset counts
      //calcualte new interval
      variable_interval_ms = k_newEyePos_interval_ms + 50* random(0,10);
      //do something
      //pick ne wpoints
      x_new = random(24, 104); //pcik new poitn but don't go too close to edge
      y_new = random(36, 48);
       
      drawTweenEyesNL(x_pos, y_pos, x_new, y_new, 6);
     }//end if time elsapsed

  //set the origianl points to the new poitns - this is done by cakkubg tgus function with the new x y postition
  
}//end idle eyes



void gblink(int xpos, int ypos, int num_blink_steps){ //need to know the position

  if (millis()- prevT_Blink_ms >= vari_blink_interval_ms){

       prevT_Blink_ms = millis();
       vari_blink_interval_ms = k_Blink_interval_ms + 500* random(0,10);

       int j=0;
             
       //make these 4 global variables
       int eye_spread = 6;
       int eye_width = 20;
       int eye_height = 30;
       int radius = 7;
       int bss = eye_height/num_blink_steps;//blink_step_size
    
       //nned to calcualte from all the variables
       int left_blink_pos_x = xpos - eye_width - (eye_spread/2);
       int left_blink_pos_y = ypos - (eye_height/2); 
       int right_blink_pos_x = xpos + eye_spread/2; 
       int right_blink_pos_y = ypos - (eye_height/2);

       if (curious_mode){
            //there are three different curious modes blink - normal, right bigger, left bigger
            if(xpos>64){ //eyes are on the right side, make right eye bigger
               for(j = 0; j <= num_blink_steps;j++){
                  //erase previous eyes
                  //display.fillRect(left_blink_pos_x-10, left_blink_pos_y-2, 4*eye_width,4+eye_height, BLACK);
                  display.fillRect(0, 17, 128, 64-17, BLACK);
                  //create new eye
                  display.fillRoundRect(left_blink_pos_x, left_blink_pos_y+int(j*bss/2), eye_width, eye_height-(j*bss), radius, WHITE);
                  display.fillRoundRect(right_blink_pos_x-2, right_blink_pos_y+int(j*bss/2)-2, eye_width+2, eye_height+4-(j*bss), radius, WHITE);
                  display.display();
               }//end close blink
               
               for(j = num_blink_steps ; j >=0;j--){
                  //no need to erase since eye is growing
                  //create growing new eye
                  display.fillRoundRect(left_blink_pos_x, left_blink_pos_y+int(j*bss/2), eye_width, eye_height-(j*bss), radius, WHITE);
                  display.fillRoundRect(right_blink_pos_x-2, right_blink_pos_y+int(j*bss/2)-2, eye_width+2, eye_height+4-(j*bss), radius, WHITE);
                  display.display();
               }//end for
                    
            }else if(xpos<=63){//end wyws on right

               for(j = 0; j <= num_blink_steps;j++){
                  //erase previous eyes
                  //display.fillRect(left_blink_pos_x-10, left_blink_pos_y-2, 4*eye_width,4+eye_height, BLACK);
                  display.fillRect(0, 17, 128, 64-17, BLACK);
                  //create new eye
                  display.fillRoundRect(left_blink_pos_x, left_blink_pos_y+int(j*bss/2)-2, eye_width+2, eye_height+4-(j*bss), radius, WHITE);
                  display.fillRoundRect(right_blink_pos_x, right_blink_pos_y+int(j*bss/2), eye_width, eye_height-(j*bss), radius, WHITE);
                  display.display();
               }//end close blink
               
               for(j = num_blink_steps ; j >=0;j--){
                  //no need to erase since eye is growing
                  //create growing new eye
                  display.fillRoundRect(left_blink_pos_x, left_blink_pos_y+int(j*bss/2)-2, eye_width+2, eye_height+4-(j*bss), radius, WHITE);
                  display.fillRoundRect(right_blink_pos_x, right_blink_pos_y+int(j*bss/2), eye_width, eye_height-(j*bss), radius, WHITE);
                  display.display();
               }//end for
  
              
            }else{ //end on left
                for(j = 0; j <= num_blink_steps;j++){
                  //erase previous eyes
                  //display.fillRect(left_blink_pos_x-10, left_blink_pos_y-2, 4*eye_width,4+eye_height, BLACK);
                  display.fillRect(0, 17, 128, 64-17, BLACK);
                  //create new eye
                  display.fillRoundRect(left_blink_pos_x, left_blink_pos_y+int(j*bss/2), eye_width, eye_height-(j*bss), radius, WHITE);
                  display.fillRoundRect(right_blink_pos_x, right_blink_pos_y+int(j*bss/2), eye_width, eye_height-(j*bss), radius, WHITE);
                  display.display();
               }//end close blink
               
               for(j = num_blink_steps ; j >=0;j--){
                  //no need to erase since eye is growing
                  //create growing new eye
                  display.fillRoundRect(left_blink_pos_x, left_blink_pos_y+int(j*bss/2), eye_width, eye_height-(j*bss), radius, WHITE);
                  display.fillRoundRect(right_blink_pos_x, right_blink_pos_y+int(j*bss/2), eye_width, eye_height-(j*bss), radius, WHITE);
                  display.display();
               }//end for
  
            }//end else
        
       }else{
          //Not surious mode then use default
          for(j = 0; j <= num_blink_steps;j++){
            //erase previous eyes
            //display.fillRect(left_blink_pos_x-10, left_blink_pos_y-2, 4*eye_width,4+eye_height, BLACK);
            display.fillRect(0, 17, 128, 64-17, BLACK);
            //create new eye
            display.fillRoundRect(left_blink_pos_x, left_blink_pos_y+int(j*bss/2), eye_width, eye_height-(j*bss), radius, WHITE);
            display.fillRoundRect(right_blink_pos_x, right_blink_pos_y+int(j*bss/2), eye_width, eye_height-(j*bss), radius, WHITE);
            display.display();
         }//end close blink
         
         for(j = num_blink_steps ; j >=0;j--){
            //no need to erase since eye is growing
            //create growing new eye
            display.fillRoundRect(left_blink_pos_x, left_blink_pos_y+int(j*bss/2), eye_width, eye_height-(j*bss), radius, WHITE);
            display.fillRoundRect(right_blink_pos_x, right_blink_pos_y+int(j*bss/2), eye_width, eye_height-(j*bss), radius, WHITE);
            display.display();
         }//end for
       }//end not curious mode
       

    
  }//end blink time chekck

  
}//end generic blink



/* === THE END ==================================== */
  
