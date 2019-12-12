
/**
 * Web Server LED Control 
 *  
 * @author Miguel Alvarez 
 *  
 * @date    27-FEB-2015 
 *  
 * @details A sketch that displays an HTML page with three 
 *          buttons to toggle three digital pins. You may
 *          connect an LED to pins 2,3, and 5 with a 1k
 *          resistor. Then visit the IP address (printed in the
 *          serial window) in a web browser. Tested on this
 *          version of the shield:
 *          http://www.seeedstudio.com/depot/W5200-Ethernet-Shield-p-1577.html
 *  
 *          For more detail instructions and example output see:
 *          http://www.seeedstudio.com/wiki/Ethernet_Shield_V2.4
 *  
 * @circuit Stack the W5200 Ethernet Shield from Seeed Studio on 
 *          the Arduino board. Connect Three leds and 1k
 *          resistors to pins 2, 3, and 5.
 */

#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

int pins[60];             //0=NULL 1=Toggle 2=Button -- Defined in settings.txt
int TYPE_BUTTON = 2;      //Does not stay on, reverts to off after delay
int TYPE_TOGGLE = 1;      //Stays on or stays off
int TYPE_NULL = 0;        
String pinNames[60];      

// NOTE: do not attempt to control the pins used by the SPI bus or 0,1, and 4 pins
 
// define the controller's MAC address
byte mac[] = { 
  0xA8, 0x61, 0x0A, 0xAE, 0x17, 0x0E };
byte ip[] = { 192, 168, 86, 45 };     //Manual setup only
byte gateway[] = { 192, 168, 86, 1 }; //Manual setup only
byte subnet[] = { 255, 255, 255, 0 }; //Manual setup only
 
// set the server at port 80 for HTTP
int port = 80;
String ipAddress = "";
EthernetServer server(port);
File myFile;
// Ethernet controller SPI CS pin
#define W5200_CS  10
// SD card SPI CS pin
#define SDCARD_CS 4

void setup() {

  Serial.begin(9600);
  // make sure that the SD card is not selected for the SPI port
  // by setting the CS pin to HIGH
  pinMode(SDCARD_CS,OUTPUT);
  digitalWrite(SDCARD_CS,HIGH);

  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
  myFile = SD.open("settings.txt");
  String buf;
  if (myFile) {
    Serial.println("settings.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      buf = myFile.readStringUntil('\n');
      //Serial.println(buf);
      String key = getValue(buf, ',', 0);
      if(key == "IP") {
        
      } else if(key == "GATEWAY") {
        
      } else if(key == "SUBNET") {
        
      } else if(key == "PIN") {
        Serial.println(buf);
        int pinNum = getValue(buf, ',', 1).toInt();
        String pinName = getValue(buf, ',', 2);
        String type = getValue(buf, ',', 3);
        Serial.print("Type has a length of ");
        Serial.println(type.length());
        if(type == "Toggle") {
          pins[pinNum] = TYPE_TOGGLE;
        }
        else if(type == "Button") {
          pins[pinNum] = TYPE_BUTTON;
        }
        else {
          pins[pinNum] = TYPE_NULL;
          Serial.print("Pin ");
          Serial.print(pinNum);
          Serial.println(" declared but not a valid control type. Defaulting to null. Check capitalization. ");
          Serial.print(type);
        }
        pinNames[pinNum] = pinName;
        Serial.println(pinNames[pinNum]);
        Serial.println(pins[pinNum]);
        Serial.println(type);
      }
    }
    // close the file:
    myFile.close();
    } else {
      // if the file didn't open, print an error:
      Serial.println("error opening settings.txt");
  }
  
  // the pins to be OUTPUTS and be HIGH intially
  for(int i=0; i<sizeof(pins)/sizeof(pins[0]);i++)
  {
    if(pins[i] > 0) {
      pinMode(i,OUTPUT);
      digitalWrite(i,HIGH);
      Serial.println("Pin set to HIGH ");
      Serial.println(i);
    } 
  }

  // loop until we get a connection to the access point
  while(Ethernet.begin(mac)==0){
    Serial.println(F("DHCP configuration failed. Trying again..."));
  }
  //Ethernet.begin(mac, ip, gateway, subnet); //for manual setup
  // now that we have a good ethernet connection we can start the server
  server.begin();
  Serial.print(F("The server can be accessed at: "));
  IPAddress ip = Ethernet.localIP();
  ipAddress += ip[0];
  ipAddress+= ".";
  ipAddress += ip[1];
  ipAddress += ".";
  ipAddress += ip[2];
  ipAddress += ".";
  ipAddress += ip[3];
  Serial.println(ipAddress);
}

void loop() {
  
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    int activeButton = -1;
    int buttonState = -1;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
 
        // look for the string "pin=" in the browser/client request
        if (!client.find("pin=")) { 
          // the string "pin=" is not present in the browser/client HTTP request
          // so display the buttons webpage.
 
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connnection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");
          // Now the HTML page body follows.
          client.print("<html>");
          client.print("<head>");
          client.print("<title>The Final Door - Prop Controller</title>");
          client.print("<style>");
          client.print(".toggletype.toggleOn { background-color: #7dff75; } .toggletype {background-color:#ff7575;} .buttontype{background-color:#c5efff}");
          client.print(".buttontype,.toggletype{font-size: 25px; margin: 10px; padding: 5px 20px; border-radius: 15px;}");
          client.print("body{max-width:500px; background:#1d1d1d; width:100%; margin:auto;}");
          client.print("</style></head>");
          client.print("<body>");
          client.print("<img src='https://static1.squarespace.com/static/56a3e26f9cadb6769969f8d5/t/5aaee4ff2b6a28af3ecbc967/1557187226358/?format=1500w' />");
          client.print("<h1 style='color: white'>The Final Door - Prop Controller</h1>");
 
          // Create the HTML button code e.g. <button id="xx" class="led">Toggle Pin xx</button> where xx is the pin number
  	  // In the <button id="xx" class="led"> tags, the ID attribute is the value sent to the arduino via the "pin" GET parameter
           for(int i=0; i<sizeof(pins)/sizeof(pins[0]);i++)
           {
            if(pins[i] > 0) {
              String buttonHTML = "<button id=\"";
              buttonHTML+=i;
              if(pins[i] == TYPE_TOGGLE) {
                buttonHTML+="\" class=\"toggletype\">";
              } else if(pins[i] == TYPE_BUTTON) {
                buttonHTML+="\" class=\"buttontype\">";
              }
              buttonHTML+=i;
              buttonHTML+= " ";
              buttonHTML+=pinNames[i];
              buttonHTML+="</button><br />";
              client.println(buttonHTML);
            }
           }
 
          // Include the JQuery library. Note: the computer that loads this page will need an internet connection.
          client.print("<script src=\"http://ajax.googleapis.com/ajax/libs/jquery/2.1.3/jquery.min.js\"></script>"); // include the JQuery library hosted by Google
          // Javascript code that receives the button clicks and the correct pin back to the shield
          client.print("<script type=\"text/javascript\">"); // beg javascript code
          client.print("$(document).ready(function(){");
          client.print("$(\".buttontype, .toggletype\").click(function(){");
          client.print("var btn = $(this); var p = btn.attr('id');"); // get id value (i.e. pin13, pin12, or pin11)
 
          // create the JQuery $.get function "$.get(\"http://ipaddress:port/\", {pin:p},function(data){alert(data)});"
          // The .get functions sends and HTTP GET request to the IP address of the shield with the parameter "pin" and value "p", then executes the function alert
          String get = "$.get(\"http://";
          get+=ipAddress;
          get+=":";
          get+=port;
          get+="/\", {pin:p},function(data){var pD = JSON.parse(data); if(pD.state=='BUTTON'){return;}if(pD.state=='ON'){ btn.addClass('toggleOn'); }else{ btn.removeClass('toggleOn'); } });";//alert('Pin ' + pD.pin + ' is ' + pD.state);
          client.print(get);
          client.print("});"); // close the click function
          client.print("});"); // close the read function
          client.print("</script>"); // end the javascript snippet
          client.print("</body>"); // end HTML body
          client.print("</html>"); // end HTML 
          break;
        }else{
          // The string "pin=" is found in the browser/client rquest, this meanst that the user wants to toggle the LEDs
          int pinNumber = (client.read()-48); // get first number i.e. if the pin 13 then the 1st number is 1
          int secondNumber = (client.read()-48);
          if(secondNumber>=0 && secondNumber<=9)
          {
          pinNumber*=10;
          pinNumber +=secondNumber; // get second number, i.e. if the pin number is 13 then the 2nd number is 3, then add to the first number
          }
          
          Serial.print("Toggling Pin ");
          Serial.println(pinNumber);
          digitalWrite(pinNumber, !digitalRead(pinNumber)); // toggle pin      
           // Build pinstate string. The Arduino replies to the browser with this string.
          String responseData = "{\"pin\":\"";
          responseData += pinNumber;
          responseData += "\"";  
          if(pins[pinNumber]==TYPE_TOGGLE) {
            
            if(!digitalRead(pinNumber)) // check if the pin is ON or OFF
            {
            responseData += ",\"state\":\"ON\"}";
            }
            else
            {
             responseData += ",\"state\":\"OFF\"}";
            }
          }
          if(pins[pinNumber]==TYPE_BUTTON) {
            activeButton = pinNumber;
            responseData += ",\"state\":\"BUTTON\"}";
          }          
          
          // Send HTTP response back to browser/client
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connnection: close");
          client.println();
          client.println(responseData);
          Serial.println(responseData);
          break; 
        } 
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
    if(pins[activeButton] > -1) {
      delay(500);
      digitalWrite(activeButton, !digitalRead(activeButton)); // Buttons do not stay on
    }
  }
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

bool DebounceSW(byte SWx)
{
  //read the passed switch twice and make sure both readings match
  //this prevents multiple triggers due to mechanical noise in 
  //the switch
  
  bool PossVal2;
  bool PossVal = !digitalRead(SWx);   //invert the reading due to the use of pullups
  
  while(true)
  {
    delay(50);          //delay by the debounce amount
    PossVal2 = !digitalRead(SWx);     //re-read the switch
    
    if (PossVal == PossVal2)        //if the two reads are the same
    {
      return (PossVal);         //return the read value
    }
    
    //this code will only execute if the two reads did not match
    //Now read the pin again and look for a match.
    //If the button is cycling very fast, it is possible the code
    //will deadlock here.  This is a very slim possibility
    
    PossVal = !digitalRead(SWx);      //re-take the first reading
    //and loop back to the delay
  }
  return (PossVal);   //this line is never executed, but makes the compiler happy.
}
