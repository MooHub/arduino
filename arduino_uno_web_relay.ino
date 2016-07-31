#include <SPI.h>
#include <Ethernet.h>

// ethernet configuration
byte mac[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
byte ip[] = { 192, 168, 1, 42 }; 

EthernetServer server(80);              // port 80 is default for HTTP

// initial
int LED = 3;          // led is connected to digital pin 3
int LED2 = 4;
int PIR = 2;          // PIR sensor is connected to digital pin 2 not used yet
int LDR = 5;          // LDR sensor is connected to analog in 5
int PWR = 4;
int PIRstate = 0;     // variable for PIR sensor status
int fromweb = 0;
float photocell = 0;  // variable for photocell (LDR) analog value
float power = 0;
int stat_lamp = 0;
int stat_out = 0;

char c = 0;           // received data
char command[2] = "\0";  // command


void setup()
{
  Ethernet.begin(mac, ip);
  server.begin();
  pinMode(LED, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(PIR, INPUT);
  //enable serial data print 
  Serial.begin(9600); 
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop()
{
  EthernetClient client = server.available();
  // detect if current is the first line
  boolean current_line_is_first = true;
  photocell = analogRead(LDR);
  power = analogRead(PWR);
  PIRstate = digitalRead(2);
  Serial.print("Lettura:");
  Serial.println(photocell);
  if ( fromweb < 1 ){
    if (photocell < 400){
  	Serial.print("Accendo la luce  ");
	digitalWrite(LED, HIGH);
        stat_lamp = 1;
    }else{
        if  (photocell > 500){
  	    digitalWrite(LED, LOW);
            stat_lamp = 0;     
       }
    }
  }
  if (client) {
    // an http request ends with a blank line
    boolean current_line_is_blank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // if we've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so we can send a reply
        if (c == '\n' && current_line_is_blank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();

          // auto reload webpage every 5 second
          client.println("<META HTTP-EQUIV=REFRESH CONTENT=5 URL=>");
          
          // webpage title
          client.println("<center><p><h1>Sensore </h1></p><center><hr><br />");

          // read analog pin 1 for the value of photocell
          client.print("<p><h2>Light reading = <font color=indigo>");
          client.println(photocell, 2);
          client.println("</font></h2></p>");          
          // read analog pin 1 for the value of photocell
          client.print("<p><h2>Power reading = <font color=indigo>");
          client.println(power, 2);
          client.println("</font></h2></p>");
          
          
          // read digital pin 13 for the state of PIR sensor
          client.println("<hr>");
          if (stat_lamp == HIGH) { // PIR sensor detected movement
            client.println("<p><h2><font color=red>Luce accesa!</font></h2></p>");
          }
          else { // No movement is detected
            client.println("<p><h2><font color=green>Luce spenta</font></h2></p>"); 
          }  

          // button functions
          client.print("<p><h2>Lamp </h2></p>");
          
          client.println("<form  method=get name=form>");
          client.println("<button name=b value=5 type=submit style=height:80px;width:150px>AUTO Off</button>");
          client.println("<button name=b value=6 type=submit style=height:80px;width:150px>AUTO On</button>");
          client.println("</form><br />");
          // button functions
          client.println("<form  method=get name=form>");
          client.println("<button name=b value=1 type=submit style=height:80px;width:150px>LED On</button>");
          client.println("<button name=b value=2 type=submit style=height:80px;width:150px>LED Off</button>");
          client.println("</form><br />");


          // button functions
          client.println("<hr>");          
          if (stat_out == HIGH) { // PIR sensor detected movement
            client.println("<p><h2><font color=red>Powerline accesa!</font></h2></p>");
          }
          else { // No movement is detected
            client.println("<p><h2><font color=green>Powerline spenta</font></h2></p>"); 
          }  
          client.println("<p><h2>Powerline</font></h2></p>"); 
          client.println("<form  method=get name=form>");
          client.println("<button name=b value=3 type=submit style=height:80px;width:150px>On</button>");
          client.println("<button name=b value=4 type=submit style=height:80px;width:150px>Off</button>");
          client.println("</form><br />");

          
          // webpage footer
          client.println("<hr><center> fine </p></center>");
          
          break;
        }
        if (c == '\n') {
          // we're starting a new line
          current_line_is_first = false;
          current_line_is_blank = true;
        } 
        else if (c != '\r') {
          // we've gotten a character on the current line
          current_line_is_blank = false;
        }
        // get the first http request
        if (current_line_is_first && c == '=') {
          for (int i = 0; i < 1; i++) {
            c = client.read();
            command[i] = c;
          }
          // LED control
          if (!strcmp(command, "1")) {
            digitalWrite(LED, HIGH);
                    stat_lamp = 1;

          }
          else if (!strcmp(command, "2")) {
            digitalWrite(LED, LOW);
                    stat_lamp = 0;

         }
          
          // LED control
          if (!strcmp(command, "3")) {
            digitalWrite(LED2, HIGH);
            stat_out=1;
          }
          else if (!strcmp(command, "4")) {
            digitalWrite(LED2, LOW);
            stat_out=0;
         }
          if (!strcmp(command, "5")) {
              fromweb=1;
          }
          else if (!strcmp(command, "6")) {
              fromweb=0;
         }
          
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    client.stop();
  }else{
  	delay(500);
  	Serial.print("loop  ");
  	
  }
}
