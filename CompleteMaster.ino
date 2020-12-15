//IoT Final project 
//Prepared by: Mudra Suthar(Au1741022) and Hardi Trivedi (AU1741092)
//Guided by: Prof.Anurag Lakhlani 

#include <Wire.h>
#include<SoftwareSerial.h>
SoftwareSerial client(2,3); //RX, TX

String webpage="";
int i=0,k=0;
String readString;
int x=0;

boolean No_IP=false;
String IP="";
//char temp0='0';

String name="<p>Wasp Securities</p>";   //22
String dat="<p>Data Received Successfully.....</p>";     //21

int temp_pin = A0;//declaring temperature sensor pin
int tmp_sensorvalue;    //value from the sensor
double temp;

int smoke_pin = A1;//declaring smokesensor pin
int smoke_sensorvalue;    //value from the sensor


int pir_sensorpin = 13;
int pir_sensorvalue = 0;

int temp1,temp2,temp3;


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


void setup()
{
  Serial.begin(9600);
  Wire.begin(); // join i2c bus (address optional for master)
   client.begin(9600);
   wifi_init();
  pinMode(pir_sensorpin, INPUT);
}



void loop()
{
  tmp_sensorvalue = analogRead(A0);
  smoke_sensorvalue = analogRead(A1);
  pir_sensorvalue = digitalRead(pir_sensorpin);
  temp = calculateTemp(tmp_sensorvalue);
  if(temp >= 50){
    Serial.println("There is Fire!! Make sure you are okay");
    temp1 = 1;
    }
    else{
      Serial.println("Your home is safe and cool just like you;)");
      temp2 = 0;
      }
      byte flagt_buzzer = temp1;
      double smoke = map (smoke_sensorvalue,300,750,0,100);
      if(smoke >= 40){
    Serial.println("We are sensing something weird! Harmful smoke is detected");
    temp2 = 1;
    }
    else{
      Serial.println("Be calm! Everything is fine");
      temp2 = 0;
      }
      byte smoke_buzzer = temp2;
      if (pir_sensorvalue == HIGH){
        temp3 = 1;
        }
        else{
          temp3 = 0;
          }
          byte flag_pirsensor = temp3;
        
  Wire.beginTransmission(4); // transmit to device #4
  Wire.write(flagt_buzzer); // sends one byte  
  Wire.write(smoke_buzzer);
  Wire.write(flag_pirsensor);
  Wire.endTransmission();    // stop transmitting

 
  delay(2000);
  
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
int calculateTemp(int x){
  float voltage = x * 5.0;
 voltage /= 1024.0; 
 float temperatureC = (voltage - 0.5) * 100 ; 
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

      webpage = "<h2>FIRE ALARAM SYSTEM</h2><color=000080 >";
      if(flagt_buzzer == 1){
        webpage+="<h3>There is a fire!!Make sure you are Okay</h3><color=ff0000 ";
        }
        if(flagt_buzzer == 0){
        webpage+="<h3>Your home is safe and cool just like you</h3><color=000080 >";
        }
      sendwebdata(webpage);

       webpage = "<h2>SMOKE DETECTION SYSTEM</h2><color=000080 >";
      if(flagt_buzzer == 1){
        webpage+="<h3>We are sensing something weird!!Harmful smoke detected</h3><color=ff0000 ";
        }
        if(flagt_buzzer == 0){
        webpage+="<h3>Be calm! Everything is fine</h3><color=000080 >";
        }
      sendwebdata(webpage);
     client.println("AT+CIPCLOSE=0"); 
}
