// Load Wi-Fi library
#include <ESP8266WiFi.h>

// Network credentials
const char* ssid = "SSID";
const char* password = "PASSWORD";

// Define WiFi Access Point
boolean result = WiFi.softAP("SSID", "");

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Assign output variables to GPIO pins (Motor Connections)
const int IN1 = 14; //L298N IN1 GPIO14(D5)
const int IN2 = 12; //L298N IN2 GPIO12(D6)
const int IN3 = 13; //L298N IN3 GPIO13(D7)
const int IN4 = 15; //L298N IN4 GPIO15(D8)

void setup() {
  Serial.begin(115200);

  // Initialize the output variables as outputs (all motor control pins are outputs)
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Set outputs to LOW
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.softAP(ssid, password);

  // Print the IP address
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);  

  // Start the server
  server.begin();
  Serial.println("Server started");
}

void loop() 
{
  WiFiClient client = server.available();   // Listen for incoming clients
  
  if(client)                                // If a new client connects
  {
    String currentLine = "";                // Make a String to hold incoming data from the client
    while (client.connected())              // Loop while the client's connected
    {
      if(client.available())                // If there's bytes to read from the client
      {
        char c = client.read();             // Read a byte, then
        Serial.write(c);                    // Print it out the serial monitor
        header += c;
        if (c == '\n')                      // If the byte is a newline character        
        {
          if (currentLine.length() == 0)
          {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows that's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // Turns the GPIOs on or off
            if(header.indexOf("/Stop") >=0)
            {
              // turn on motor A
              digitalWrite(IN1, LOW);
              digitalWrite(IN2, LOW);
              // turn on motor B
              digitalWrite(IN3, LOW);
              digitalWrite(IN4, LOW);
            } 
            else if(header.indexOf("/Forward") >=0)
            {
              // turn on motor A
              digitalWrite(IN1, LOW);
              digitalWrite(IN2, HIGH);
              // turn on motor B
              digitalWrite(IN3, LOW);
              digitalWrite(IN4, HIGH);
            }
            else if(header.indexOf("/Reverse") >=0)
            {
              // turn on motor A
              digitalWrite(IN1, HIGH);
              digitalWrite(IN2, LOW);
              // turn on motor B
              digitalWrite(IN3, HIGH);
              digitalWrite(IN4, LOW);
            }
            else if(header.indexOf("/Left") >=0)
            {
              // turn on motor A
              digitalWrite(IN1, HIGH);
              digitalWrite(IN2, LOW);
              // turn on motor B
              digitalWrite(IN3, LOW);
              digitalWrite(IN4, HIGH);
            }  
            else if(header.indexOf("/Right") >=0)
            {
              // turn on motor A
              digitalWrite(IN1, LOW);
              digitalWrite(IN2, HIGH);
              // turn on motor B
              digitalWrite(IN3, HIGH);
              digitalWrite(IN4, LOW);
            }      
        
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\"></head>");            
            client.println("<body><center>");
            client.println("<i><h2>Rover Remote Control</h2></i>");
            client.println("<a href=\"/Forward\"><button style=\"background-color: C0C0C0; height: 80px; width: 80px\"><b>Forward </b></button></a><br>");
            client.println("<a href=\"/Left\"><button style=\"background-color: C0C0C0; height: 80px; width: 80px\"><b>Left </b></button></a>");
            client.println("<a href=\"/Stop\"><button style=\"background-color: FF0000; height: 80px; width: 80px\"><b>Stop </b></button></a>");
            client.println("<a href=\"/Right\"><button style=\"background-color: C0C0C0; height: 80px; width: 80px\"><b>Right </b></button></a><br>");
            client.println("<a href=\"/Reverse\"><button style=\"background-color: C0C0C0; height: 80px; width: 80px\"><b>Reverse </b></button></a><br>");
            client.println("</body></center></html>");
  
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;            
          }
          else
          {
            // If you got a newline, then clear currentLine
            currentLine = "";
          }
        }
        else if (c != '\r')   // If you got anything else but a carriage return character
        {
          currentLine += c;   // Add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
