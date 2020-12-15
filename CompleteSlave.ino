// Home security System 
//Prepared by : Hardi Trivedu and Mudra Suthar

#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Servo.h>
#include <Wire.h>
#include<SoftwareSerial.h>
SoftwareSerial client(0,1); //RX, TX

String webpage="";
int i=0,k=0;
String readString;
int x=0;

boolean No_IP=false;
String IP="";
char temp0='0';

String name="<p>Milton Securities</p>";   //22
String dat="<p>Data Received Successfully.....</p>";     //21

// initialize the library with the numbers of the interface pins
LiquidCrystal LCD( 8, 9, 10, 11, 12, 13 ) ;   //RS,EN,D4,D5,D6,D7

Servo servo ;

const int buzzerPin = 14 ;   // pin-14 = pin-A0
const int servoPin = 15 ;   // pin-15 = pin A-1


const byte ROWS = 4, COLS = 4 ;

// Define the Keymap
char keys[ROWS][COLS] = {
   {'1','2','3','A'},
   {'4','5','6','B'},
   {'7','8','9','C'},
   {'*','0','#','D'}
} ;

// Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
byte rowPins[ROWS] = { 16, 17, 2, 3 } ;
// Connect keypad COL0, COL1 and COL2 to these Arduino pins.
byte colPins[COLS] = { 4, 5, 6, 7 } ;

//  Create the Keypad
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS ) ;

char password[16], string[16] ;
int flag_h_setpassword = 1, flag_inputpassword = 0, flag_inputstring = 0, flag_opendoor = 1;
int count = 0, pos = 0,  temp_count=0, count1 = 0 ;

void check4IP(int t1)
{
  int t2=millis();
  while(t2+t1>millis())
  {
    while(client.available()>0)
    {
      if(client.find("WIFI GOT IP"))
      {
        No_IP=true;
      }
    }
  }
}

void get_ip()
{
  IP="";
  char ch=0;
  while(1)
  {
    client.println("AT+CIFSR");
    while(client.available()>0)
    {
      if(client.find("STAIP,"))
      {
        delay(1000);
        Serial.print("IP Address:");
        while(client.available()>0)
        {
          ch=client.read();
          if(ch=='+')
          break;
          IP+=ch;
        }
      }
      if(ch=='+')
      break;
    }
    if(ch=='+')
    break;
    delay(1000);
  }
  Serial.print(IP);
  Serial.print("Port:");
  Serial.println(80);
}

void connect_wifi(String cmd, int t)
{
  int temp=0,i=0;
  while(1)
  {
    Serial.println(cmd);
    client.println(cmd); 
    while(client.available())
    {
      if(client.find("OK"))
      i=8;
    }
    delay(t);
    if(i>5)
    break;
    i++;
  }
  if(i==8)
  Serial.println("OK");
  else
  Serial.println("Error");
}

void wifi_init()
{
      connect_wifi("AT",100);
      connect_wifi("AT+CWMODE=3",100);
      connect_wifi("AT+CWQAP",100);  
      connect_wifi("AT+RST",5000);
      check4IP(5000);
      if(!No_IP)
      {
        Serial.println("Connecting Wifi....");
        connect_wifi("AT+CWJAP=\"Hogwarts\",\"wasp1603\"",7000);         //provide your WiFi username and password here
     // connect_wifi("AT+CWJAP=\"vpn address\",\"wireless network\"",7000);
      }
      else
        {
        }
      Serial.println("Wifi Connected"); 
      get_ip();
      connect_wifi("AT+CIPMUX=1",100);
      connect_wifi("AT+CIPSERVER=1,80",100);
}

void sendwebdata(String webPage)
{
    int ii=0;
     while(1)
     {
      unsigned int l=webPage.length();
      Serial.print("AT+CIPSEND=0,");
      client.print("AT+CIPSEND=0,");
      Serial.println(l+2);
      client.println(l+2);
      delay(100);
      Serial.println(webPage);
      client.println(webPage);
      while(client.available())
      {
        //Serial.print(Serial.read());
        if(client.find("OK"))
        {
          ii=11;
          break;
        }
      }
      if(ii==11)
      break;
      delay(100);
     }
}


void setup() {
   for(int k=8 ; k<14 ; k++) {
      pinMode(k,OUTPUT) ;   //pins 8-14 are enabled as output
   }

   LCD.begin(16, 2) ;   
   pinMode(buzzerPin, OUTPUT) ;
   servo.attach(servoPin) ;
   Wire.begin(4);                // join i2c bus with address #4
   Wire.onReceive(receiveEvent); // register event
   Serial.begin(9600) ;
    client.begin(9600);
   wifi_init();
   LCD.setCursor(0,0) ;
   LCD.print("   WELCOME !!") ;
   LCD.setCursor(0,1) ;
   LCD.print("Set a Password :") ;

   InitializePassword(), InitializeString() ;
   CloseDoor() ;
   
   
}



void loop() {
   
   

   //First checking the door for the breakin
     
      // Now, the Welcome message...
   
   
      char key = kpd.getKey() ;   //storing pressed key value in a char

      if( key != NO_KEY ) {
         
         if( flag_h_setpassword == 1 ) {
            H_SetPassword() ;
            
         }
         if( key == '*' ) {
            if( flag_inputpassword == 1 ) {
               InitializePassword(), H_SetPassword() ;
            }
            else if( flag_inputstring = 1 ) {
               InitializeString(), H_EnterPassword() ;
            }
         }
         else if( key == '#' ) {
            if( flag_inputpassword == 1  &&  count > 0 ) {
               flag_inputpassword = 0 ;
               password[count] = '\0' ;
               
               H_EnterPassword() ;
            } 
            else if( flag_inputstring == 1  &&  count > 0 ) {
               flag_inputstring = 0 ;
               string[count] = '\0' ;
               //Comparing the password and if correct opening the door
               if( Compare_Password_and_String() == 1 ) {
                  LCD.clear() ;
                  LCD.print("***VERIFIED!!***") ;
                  Serial.println("Sleep tight! your home is safe from intruders") ;
                  count1 = count1 + 1;
                  
                  OpenDoor() ;
               }
               else {
                //In case when wrong password is entered, the the door wont open and alert message will be shown
                  LCD.clear() ;
                  LCD.print("Wrong Password !") ;
                  Serial.println("Someone unsuccessfully attempted to open the lock !") ;
                  
                  tone(buzzerPin, 100, 250) ;
                  //delay(100) ;
                  
                  H_EnterPassword() ;
                  temp_count=temp_count +1;
                  //In case wrong password us entered three consecutive times theb the whole system will freeze
                  if (temp_count>=3){
                     LCD.setCursor(0,0) ;
                     LCD.print("INTRUDER ALERT") ;
                     LCD.setCursor(0,1) ;
                      LCD.print("SYSTEM IS LOCKED") ;
                Serial.println("Intruder Alert!! Call Police!!") ;
               
                     tone(buzzerPin, 450, 150) ;
                     //delay(400) ;
                     tone(buzzerPin, 150, 250) ;
                     exit(0);
                    }
                  
               }
            }
         }
        else if( flag_inputpassword == 1  ||  flag_inputstring == 1 ) {
            LCD.print(key) ;
            delay(100) ;
            LCD.setCursor(count,1) ;
            LCD.print('*') ;
            if( flag_inputpassword == 1 )   password[count] = key ;
            else if( flag_inputstring == 1 )   string[count] = key ;
            ++count ;
         }
      }
 k=0;
  Serial.println("Please Refresh your Page");
  while(k<1000)
  {
    k++;
   while(client.available())
   {
    if(client.find("0,CONNECT"))
    {
      Serial.println("Start Printing");
      Send();
      Serial.println("Done Printing");
      delay(1000);
    }
  }
  delay(1);
 }
   
    

}


//User Defined functions used in the program
void InitializePassword() {
   for( int i=0 ; i<16 ; ++i )
      password[i] = 0 ;
}

void InitializeString() {
   for( int i=0 ; i<16 ; ++i )
      string[i] = 1 ;
}

void H_SetPassword() {
   LCD.clear() ;
   LCD.setCursor(0,0) ;
   LCD.print("Set a Password :") ;
   LCD.setCursor(0,1) ;
   flag_h_setpassword = 0 ;
   flag_inputpassword = 1, count = 0 ;
}

void H_EnterPassword() {
   CloseDoor() ;
   LCD.clear() ;
   LCD.setCursor(0,0) ;
   LCD.print("Enter Password :") ;
   LCD.setCursor(0,1) ;
   flag_inputstring = 1, count = 0 ;
}

int Compare_Password_and_String() {
   int i ;
   for( i=0  ;  password[i]!='\0' && string[i]!='\0'  ;  ++i ) {
      if( password[i] != string[i] )
         return 0 ;
   }
   if( password[i] == '\0'  &&  string[i] == '\0' )
      return 1 ;
   else   return 0 ;
}

void OpenDoor() {
   if( flag_opendoor == 1 )
      return;
   for( pos=15 ; pos<=100 ; ++pos ) {
      servo.write(pos) ;
      delay(15) ;
   }
   flag_opendoor = 1 ;
}

void CloseDoor()  {
   if( flag_opendoor == 0 )
      return;
   for( pos=100 ; pos>=15; --pos ) {
      servo.write(pos) ;
      delay(15) ;
   }
   flag_opendoor = 0 ;
}

  

    //flag_opendoor = flag_opendoor + 0;
    //count1 = count1 + 0;

    void receiveEvent(int howMany)
{
  
  int flagt_buzzer = Wire.read(); 
  //delay(10); 
  int smoke_buzzer = Wire.read();
  //delay(10);
  int flag_pirsensor= Wire.read();
  //delay(10);
  
  if(flagt_buzzer==1){
   tone(buzzerPin, 450, 150) ;
   //delay(400) ;
   tone(buzzerPin, 150, 250) ;
    }
    if(smoke_buzzer==1){
   tone(buzzerPin, 450, 150) ;
   //delay(400) ;
   tone(buzzerPin, 150, 250) ;
    }
    if(flag_pirsensor ==1 && flag_opendoor == 1 && count1==0){
      LCD.setCursor(0,0) ;
       LCD.print("INTRUDER ALERT") ;
       tone(buzzerPin, 450, 150) ;
      // delay(400) ;
       tone(buzzerPin, 150, 250) ;
       exit(0);
      }
}
    void Send()
{
      webpage = "<h1>Welcome to MiltonSecurities</h1><body bgcolor=ffffff color=000080>";
      sendwebdata(webpage);
      webpage=name;
      webpage+=dat;
      sendwebdata(webpage);
      delay(1000);
      
    webpage = "<h2>Be Relaxed! Your home is safe with us</h2><color=000080>";
      sendwebdata(webpage);

      webpage = "<h2>INTRUDER ALERT SYSTEM</h2><color=000080 >";
      if(temp_count >=3){
        webpage+="<h3>Sleep tight your home is safe from Intruders</h3><color=ff0000 ";
        }
        else{
        webpage+="<h3>Intruder Alert!! Call Police</h3><color=000080 >";
        }
      sendwebdata(webpage);

      =
     client.println("AT+CIPCLOSE=0"); 
}
