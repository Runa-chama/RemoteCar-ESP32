#include <ESP32Servo.h>
#include <Ps3Controller.h>


#define ON LOW
#define OFF HIGH

#define battery_pin A10

//motor
#define m1 32
#define m2 33
#define m_ch1 10
#define m_ch2 11

//servo
#define servo_pin 13

//front lump
#define led_front_1 16
#define led_front_2 5

//back lump
#define led_back_1 25
#define led_back_2 21

//brake lump
#define led_brake_1 27
#define led_brake_2 23

//turn signal lump (right)
#define turn_lump_r1 14
#define turn_lump_r2 26
//turn signal lump (left)
#define turn_lump_l1 15
#define turn_lump_l2 22

//Adjust the value to match the motor rating
#define MAX_SPEED 140

//led_pins
const int leds[] = {  led_front_1,  led_front_2,
                      led_back_1,   led_back_2,
                      led_brake_1,  led_brake_2,
                      turn_lump_r1, turn_lump_r2,
                      turn_lump_l1, turn_lump_l2};

//Min Voltage of 2cell
const int battery_min_voltage = 2.10;

//Servo
int minUs = 500;
int maxUs = 2400;
Servo servo;

volatile int interruptCounter;
hw_timer_t * timer = NULL;

bool is_remaining_charge_enough = true;
bool is_turn_right = false;
bool is_turn_left = false;

int speed = 0;

//Callback function When a button pushed
void received(){
  int stick_left_y = Ps3.data.analog.stick.ly;
  int stick_right_x = Ps3.data.analog.stick.rx;
  
  if(is_remaining_charge_enough)
  {
    int cache_speed = speed;
    double max_speed = MAX_SPEED;
    speed = stick_left_y >= 0 ? map(stick_left_y,10,128,0,max_speed) : map(-stick_left_y,10,128,0,max_speed);

    if(stick_left_y < 65 && stick_left_y > -65)//Brake lump
    {
      digitalWrite(led_brake_1,ON);
      digitalWrite(led_brake_2,ON);
    }
    else
    {
      digitalWrite(led_brake_1,OFF);
      digitalWrite(led_brake_2,OFF);
    }
      
      
    if(stick_left_y < -10) //go
    {
       ledcWrite(m_ch1,speed);
       ledcWrite(m_ch2,0);
    }
    else if(stick_left_y > 10) //back
    {
      ledcWrite(m_ch1,0);
      ledcWrite(m_ch2,speed);
      digitalWrite(led_back_1,ON);
      digitalWrite(led_back_2,ON);
    }
    else
    {
      ledcWrite(m_ch1,0);
      ledcWrite(m_ch2,0);
      digitalWrite(led_back_1,OFF);
      digitalWrite(led_back_2,OFF);
    }

    if(stick_right_x < -10) //right
    {
      int angle = 90 + map(-stick_right_x,10,180,0,60);
      servo.write(angle);
    }
    else if(stick_right_x > 10) //left
    {
      int angle = 90 - map(stick_right_x,10,180,0,60);
      servo.write(angle);
    }
    else //default
    {
      servo.write(90);
    }
    
    if(Ps3.data.button.l1)//Left turn signal
    {
      is_turn_right = false;
      is_turn_left = true;
    }
    else if(Ps3.data.button.r1)//Right turn signal
    {
      is_turn_right = true;
      is_turn_left = false;
    }
    else
    {
      is_turn_right = false;
      is_turn_left = false;
    }
    if(Ps3.data.button.circle)//Front light
    {
      digitalWrite(led_front_1,!digitalRead(led_front_1));
      digitalWrite(led_front_2,!digitalRead(led_front_2));
    }
  }
}

void break_lump(int i){
  digitalWrite(led_brake_1,i);
  digitalWrite(led_brake_2,i);
}

void reset_turn_lump()
{
    digitalWrite(turn_lump_l1,OFF);
    digitalWrite(turn_lump_l2,OFF);
    digitalWrite(turn_lump_r1,OFF);
    digitalWrite(turn_lump_r2,OFF);
}


void IRAM_ATTR onTimer() {
//turn signal
  if(is_turn_right)
  {
    digitalWrite(turn_lump_r1,!digitalRead(turn_lump_r1));
    digitalWrite(turn_lump_r2,!digitalRead(turn_lump_r2));
    digitalWrite(turn_lump_l1,OFF);
    digitalWrite(turn_lump_l2,OFF);
  }
  else if(is_turn_left)
  {
    digitalWrite(turn_lump_l1,!digitalRead(turn_lump_l1));
    digitalWrite(turn_lump_l2,!digitalRead(turn_lump_l2));
    digitalWrite(turn_lump_r1,OFF);
    digitalWrite(turn_lump_r2,OFF);
  }
  else
  {
    reset_turn_lump();
  }
}

void setup(){
  for(int led : leds)
  {
    pinMode(led,OUTPUT_OPEN_DRAIN);
    digitalWrite(led,OFF);
  }

  pinMode(battery_pin,INPUT);
  
  //pwm setup
  ledcSetup(m_ch1,450,8);
  ledcSetup(m_ch2,450,8);
  ledcAttachPin(m1,m_ch1);//motor ch1
  ledcAttachPin(m2,m_ch2);//motor ch2
  
  //Timer for turn signal
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 250000, true);
  timerAlarmEnable(timer);
  
  servo.setPeriodHertz(50);
  servo.attach(servo_pin,minUs,maxUs);

  Serial.begin(115200);
  Ps3.attach(received); //Set callback function
  Ps3.begin("00:00:00:00:00:00"); //Bluetooth MAC Address
}

void loop(){
  //check voltage of batterys
  float battery_voltage = analogRead(battery_pin);
  battery_voltage /= 4096;
  battery_voltage *= 3.3;
  Serial.println(analogRead(battery_pin));
  Serial.println(battery_voltage);
  
  //Check battery voltage to prevent over-discharge
  if(battery_voltage < battery_min_voltage) 
    is_remaining_charge_enough = false;
  else
    is_remaining_charge_enough = true;
}
