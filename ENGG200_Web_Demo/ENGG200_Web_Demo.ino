#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <SoftwareSerial.h>
#include <string.h>
#include "WebServer.h"
#include "bluetooth_uno.h"

/**********************************************************************************************************************
                                    Error Storage
***********************************************************************************************************************/
String feedback = "Connecting...";

#define USE_DHCP_FOR_IP_ADDRESS

/**********************************************************************************************************************
                                    MAC address and IP address.
***********************************************************************************************************************/
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };


#if !defined USE_DHCP_FOR_IP_ADDRESS
// ip represents the fixed IP address to use if DHCP is disabled.
byte ip[] = { 192, 168, 1, 100 };
#endif

// Http header token delimiters
const char *pSpDelimiters = " \r\n";
const char *pStxDelimiter = "\002";    // STX - ASCII start of text character

/**********************************************************************************************************************
                                    Strings stored in flash of the HTML we will be transmitting
***********************************************************************************************************************/
// sample comment
// HTTP Request message
String integer;
const char * const commA[] PROGMEM = {"Waiting", "Failed to connect", "Client disconnect"};
const char * const commB[] PROGMEM = {"Waiting", "Failed to connect", "Client disconnect"};
const char * const commC[] PROGMEM = {"Waiting", "Failed to connect", "Client disconnect"};
const char * const commD[] PROGMEM = {"Waiting", "Failed_to_connect_000000000000000000", "Client_disconnect"};
const char * const commE[] PROGMEM = {"Waiting", "Failed to connect", "Client disconnect"};
const char content_404[] PROGMEM = "HTTP/1.1 404 Not Found\nServer: arduino\nContent-Type: text/html\n\n<html><head><title>Arduino Web Server - Error 404</title></head><body><h1>Error 404: Sorry, that page cannot be found!</h1></body>";
const char * const page_404[] PROGMEM = { content_404 }; // table with 404 page

// HTML Header for pages
const char content_main_header[] PROGMEM = "HTTP/1.0 200 OK\nServer: arduino\nCache-Control: no-store, no-cache, must-revalidate\nPragma: no-cache\nConnection: close\nContent-Type: text/html\n";
const char content_main_top[] PROGMEM = "<html><head><title>Arduino Web Server</title><style type=\"text/css\">body, html, h1, h2, p, div, a, img, header, footer, span { margin: 0; padding: 0; } * { box-sizing: border-box } html { background: #ffffff; } footer, header { background: #ffffff; color: #444444; font-family: Arial, \"Helvetica Neue\", Helvetica, sans-serif; text-align: center; padding: 2em 0; } main { width: 100%; margin: 0 auto; overflow: hidden; } .container { position: relative; text-align: center; font-size: 30px; font-family: Arial, \"Helvetica Neue\", Helvetica, sans-serif; } .col { width: 33.333%; height: 500px; float: left; text-align: center; padding: 1rem; } .col1 { position: relative; background: Red; font-family: Arial, \"Helvetica Neue\", Helvetica, sans-serif; } .col2 { position: relative; background: Green; font-family: Arial, \"Helvetica Neue\", Helvetica, sans-serif; } .col3 { position: relative; background: Blue; font-family: Arial, \"Helvetica Neue\", Helvetica, sans-serif; } footer { clear: both; }</style></head><body><h1>Arduino - Tile Runner Web Server</h1>";
const char content_main_menu[] PROGMEM = "<table width=\"500\"><tr><td align=\"center\"><a href=\"/\">Main</a></td><td align=\"center\"><a href=\"status\">Status</a></td></tr></table>";
const char content_main_footer[] PROGMEM = "</html>";
const char * const contents_main[] PROGMEM = { content_main_header, content_main_top, content_main_menu, content_main_footer }; // table with 404 page
#define CONT_HEADER 0
#define CONT_TOP 1
#define CONT_MENU 2
#define CONT_FOOTER 3

// Order
const char http_uri1[] PROGMEM = "/";
const char content_title1[] PROGMEM = "<h2>Ordering Page</h2>";
const char content_page1[] PROGMEM = "<hr /><div class=\"col col1\"><h2>Red Cans</h2><p>how many red cans would you like to pick up?</p><div class=\"container\"><form><input id=\"1\" type=\"number\"min=\"0\" max=\"8\" step=\"1\" name=\"Red\"></form></div></div><div class=\"col col2\"><h2>Green Cans</h2><p>how many green cans would you like to pick up?</p><div class=\"container\"><form><input id=\"2\" type=\"number\"min=\"0\" max=\"8\" step=\"1\" name=\"Green\"></form></div></div><div class=\"col col3\"><h2>Blue Cans</h2><p>how many blue cans would you like to pick up?</p><div class=\"container\"><form><input id=\"3\" type=\"number\"min=\"0\" max=\"8\" step=\"1\"name=\"Blue\"></form></div></div></form><form action=\"/\" id=\"canForm\"name=\"canForm\"><input style=\"visibility: hidden\" id=\"4\"type=\"number\"min=\"0\" max=\"8\" step=\"1\" name=\"Red\"><input style=\"visibility:hidden\"id=\"5\"type=\"number\"min=\"0\" max=\"8\"step=\"1\"name=\"Green\"><input style=\"visibility: hidden\" id=\"6\" type=\"number\"min=\"0\" max=\"8\" step=\"1\" name=\"Blue\"><div class=\"container\"><input type=\"button\" value=\"Submit\" onclick=\"sanitiseData()\"></div></form></body><script type=\"text/javascript\">document.getElementById('1').value=0;document.getElementById('2').value=0;document.getElementById('3').value=0;function total(){var redCans = document.getElementById('1').value;var greenCans = document.getElementById('2').value;var blueCans = document.getElementById('3').value;var total=parseInt(redCans)+parseInt(greenCans)+parseInt(blueCans);return total;}function greaterThanTen(){if(total()>10){return true;}return false;}function sanitiseData(){var cansValid=true;if(document.getElementById('1').value>8 || document.getElementById('1').value<0){document.getElementById('1').value=0;document.getElementById('4').value=0;cansValid=false;}if(document.getElementById('2').value>8 || document.getElementById('2').value<0){document.getElementById('2').value=0;document.getElementById('5').value=0;cansValid=false;}if(document.getElementById('3').value>8 || document.getElementById('3').value<0){document.getElementById('3').value=0;document.getElementById('6').value=0;cansValid=false;}if(!greaterThanTen() && cansValid==true){console.log(\"test\");document.getElementById('4').value=document.getElementById('1').value;document.getElementById('5').value=document.getElementById('2').value;document.getElementById('6').value=document.getElementById('3').value;document.getElementById('canForm').action = \"/status\";document.getElementById('canForm').submit();}else{alert(\"Invalid number of Cans submitted. Please submit a max of 10 total. No greater than 8 cans per colour.\"); return window.location=\"/\";}}</script>";

// Status
const char http_uri2[] PROGMEM = "/status";
const char content_title2[] PROGMEM = "<meta http-equiv=\"refresh\" content=15><div class=\"container\"> <h1>Your order is being processed</h1><p id='status'></p></div>";
const char content_page2[] PROGMEM = "<hr /><script type=\"text/javascript\"> var timer = 5; var status = document.getElementById('feedback').value;if(status!=\"Success\" && status.substring(0,5)!=\"Error\"){ document.getElementById('status').innerHTML=\"Status=\"+status; } else if(status.substring(0,5) == \"Error\"){document.location='/error';} else{document.location='/success'; } function refresh(timer){ alert(\"refresh\"); return timer; }</script>";

//Error
const char http_uri3[] PROGMEM = "/error";
const char content_title3[] PROGMEM = "<h1>Error occurs</h1><p>Please fix error and order again</p>";
const char content_page3[] PROGMEM = "<hr /><form action=\"/\"> <div style=\"position: absolute; right:25px; bottom:25px;\"> <button type=\"submit\">New Order</button> </div> </form>";

// Success
const char http_uri5[] PROGMEM = "/success";
const char content_title5[] PROGMEM = "<h1>Order Successful</h1><p>Your cans are ready to be picked up</p>";
const char content_page5[] PROGMEM = "<hr /><form action=\"/\"> <div style=\"position: absolute; right:25px; bottom:25px;\"> <button type=\"submit\">New Order</button> </div> </form>";

// declare tables for the pages
const char * const contents_titles[] PROGMEM = { content_title1, content_title2, content_title3, content_title5 }; // titles
const char * const contents_pages [] PROGMEM = { content_page1, content_page2, content_page3, content_page5 }; // real content


// declare table for all URIs
const char * const http_uris[] PROGMEM = { http_uri1, http_uri2, http_uri3, http_uri5 }; // URIs

#define NUM_PAGES  sizeof(contents_pages)  / sizeof(contents_pages[0])
#define NUM_URIS  (NUM_PAGES)  // Pages URIs + favicon URI, etc

/**********************************************************************************************************************
                                                  Shared variable and Setup()
***********************************************************************************************************************/
EthernetServer server(80);

void setup()
{
  Serial.flush();
  Serial.begin(9600); // DEBUG
  Serial.println("Starting Server.");
  Serial.println("Obtaining IP...");
#ifdef USE_DHCP_FOR_IP_ADDRESS
  Ethernet.begin(mac);  // Use DHCP to get an IP address
#else
  Ethernet.begin(mac, ip);
#endif

  delay(2000);
  server.begin();
  Serial.print("Server is at ");
  Serial.println(Ethernet.localIP());
  BluetoothUno.initiateConnToMega();

}

/**********************************************************************************************************************
                                                            Main loop
***********************************************************************************************************************/
char check = 0;

String parsingString(String str) {
  String result = "0"; //0 for unsuccess & 1 for success. FIXME
  result.concat(str[4]);
  result.concat(str[12]);
  result.concat(str[19]);
  return result;
}

String getErrorMessage(String str) {
  String result = "";
  int code = str.substring(1).toInt();
  switch (str[0]) {
    case 'A':
      result = commA[code];
      break;
    case 'B':
      result = commB[code];
      break;
    case 'C':
      result = commC[code];
      break;
    case 'D':
      result = commD[code];
      break;
    case 'E':
      result = commE[code];
      break;
  }
  return result;
}

void loop()
{
  EthernetClient client = server.available();


  if (client)
  {
    // now client is connected to arduino we need to extract the
    // following fields from the HTTP request.
    int    nUriIndex;  // Gives the index into table of recognized URIs or -1 for not found.
    BUFFER requestContent;    // Request content as a null-terminated string.
    MethodType eMethod = readHttpRequest(client, nUriIndex, requestContent);
    String message = "";
    char check = 1;

    Serial.print("Read Request type: ");
    Serial.print(eMethod);
    Serial.print(" Uri index: ");
    Serial.print(nUriIndex);
    Serial.print(" content: ");
    Serial.println(requestContent);


    if (nUriIndex < 0)
    {
      // URI not found
      sendProgMemAsString(client, (char*)pgm_read_word(&(page_404[0])));
    }
    else if (nUriIndex < NUM_PAGES)
    {

      sendPage(client, nUriIndex, requestContent);

      // Normal page request, may depend on content of the request
      if (nUriIndex == 1) {
        
        String integer = parsingString(requestContent);
<<<<<<< HEAD
        //check = BluetoothUno.transmitToMega(integer);
        
=======
        check = 0;
>>>>>>> 3445914c9a5149c5993ad67b4b436ab09f76c3e6
        Serial.println(integer);
        //feedback = getErrorMessage(Bluetooth::feedback());
        if (check == 1) {
          //BluetoothUno.prepareForMega(integer);
          
          //String encrypted = BluetoothUno.encryptData(integer);
<<<<<<< HEAD
          //check = 
          
=======
>>>>>>> 3445914c9a5149c5993ad67b4b436ab09f76c3e6
          BluetoothUno.transmitToMega(integer);
          feedback = "Attemping to send data...";
          //message = "<p>Red can: " + String(integer[1]) + "</p>";
          //message.concat("<p>Green can: " + String(integer[2]) + "</p>");
          //message.concat("<p>Blue can: " + String(integer[3]) + "</p>");
        }
        //else
          //feedback = BluetoothUno.feedback();
          if (millis < 20000){
            feedback = "Error0111";
          }
          else if (millis < 40000)
            feedback = "Error0111";
          else
            feedback = "Success";
      }
    }

    // give the web browser time to receive the data
    delay(1);
    client.stop();
  }
}

/**********************************************************************************************************************
                                               Method for read HTTP Header Request from web client

  The HTTP request format is defined at http://www.w3.org/Protocols/HTTP/1.0/spec.html#Message-Types
  and shows the following structure:
   Full-Request and Full-Response use the generic message format of RFC 822 [7] for transferring entities. Both messages may include optional header fields
   (also known as "headers") and an entity body. The entity body is separated from the headers by a null line (i.e., a line with nothing preceding the CRLF).
       Full-Request   = Request-Line
*                       *( General-Header
                         | Request-Header
                         | Entity-Header )
                        CRLF
                        [ Entity-Body ]

  The Request-Line begins with a method token, followed by the Request-URI and the protocol version, and ending with CRLF. The elements are separated by SP characters.
  No CR or LF are allowed except in the final CRLF sequence.
       Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
  HTTP header fields, which include General-Header, Request-Header, Response-Header, and Entity-Header fields, follow the same generic format.
  Each header field consists of a name followed immediately by a colon (":"), a single space (SP) character, and the field value.
  Field names are case-insensitive. Header fields can be extended over multiple lines by preceding each extra line with at least one SP or HT, though this is not recommended.
       HTTP-header    = field-name ":" [ field-value ] CRLF
***********************************************************************************************************************/
// Read HTTP request, setting Uri Index, the requestContent and returning the method type.
MethodType readHttpRequest(EthernetClient & client, int & nUriIndex, BUFFER & requestContent)
{
  BUFFER readBuffer;    // Just a work buffer into which we can read records
  int nContentLength = 0;
  bool bIsUrlEncoded;

  requestContent[0] = 0;    // Initialize as an empty string
  // Read the first line: Request-Line setting Uri Index and returning the method type.
  MethodType eMethod = readRequestLine(client, readBuffer, nUriIndex, requestContent);
  // Read any following, non-empty headers setting content length.
  readRequestHeaders(client, readBuffer, nContentLength, bIsUrlEncoded);

  if (nContentLength > 0)
  {
    // If there is some content then read it and do an elementary decode.
    readEntityBody(client, nContentLength, requestContent);
    if (bIsUrlEncoded)
    {
      // The '+' encodes for a space, so decode it within the string
      for (char * pChar = requestContent; (pChar = strchr(pChar, '+')) != NULL; )
        *pChar = ' ';    // Found a '+' so replace with a space
    }
  }

  return eMethod;
}

// Read the first line of the HTTP request, setting Uri Index and returning the method type.
// If it is a GET method then we set the requestContent to whatever follows the '?'. For a other
// methods there is no content except it may get set later, after the headers for a POST method.
MethodType readRequestLine(EthernetClient & client, BUFFER & readBuffer, int & nUriIndex, BUFFER & requestContent)
{
  MethodType eMethod;
  // Get first line of request:
  // Request-Line = Method SP Request-URI SP HTTP-Version CRLF
  getNextHttpLine(client, readBuffer);
  // Split it into the 3 tokens
  char * pMethod  = strtok(readBuffer, pSpDelimiters);
  char * pUri     = strtok(NULL, pSpDelimiters);
  char * pVersion = strtok(NULL, pSpDelimiters);
  // URI may optionally comprise the URI of a queryable object a '?' and a query
  // see http://www.ietf.org/rfc/rfc1630.txt
  strtok(pUri, "?");
  char * pQuery   = strtok(NULL, "?");
  if (pQuery != NULL)
  {
    strcpy(requestContent, pQuery);
    // The '+' encodes for a space, so decode it within the string
    for (pQuery = requestContent; (pQuery = strchr(pQuery, '+')) != NULL; )
      *pQuery = ' ';    // Found a '+' so replace with a space

    Serial.print("Get query string: ");
    Serial.println(requestContent);
    Serial.println(pMethod);
  }
  if (strcmp(pMethod, "GET") == 0)
    eMethod = MethodGet;
  else if (strcmp(pMethod, "POST") == 0)
    eMethod = MethodPost;
  else if (strcmp(pMethod, "HEAD") == 0)
    eMethod = MethodHead;
  else
    eMethod = MethodUnknown;

  // See if we recognize the URI and get its index
  nUriIndex = GetUriIndex(pUri);

  return eMethod;
}

// Read each header of the request till we get the terminating CRLF
void readRequestHeaders(EthernetClient & client, BUFFER & readBuffer, int & nContentLength, bool & bIsUrlEncoded)
{
  nContentLength = 0;      // Default is zero in cate there is no content length.
  bIsUrlEncoded  = true;   // Default encoding
  // Read various headers, each terminated by CRLF.
  // The CRLF gets removed and the buffer holds each header as a string.
  // An empty header of zero length terminates the list.
  do
  {
    getNextHttpLine(client, readBuffer);
    //    Serial.println(readBuffer); // DEBUG
    // Process a header. We only need to extract the (optionl) content
    // length for the binary content that follows all these headers.
    // General-Header = Date | Pragma
    // Request-Header = Authorization | From | If-Modified-Since | Referer | User-Agent
    // Entity-Header  = Allow | Content-Encoding | Content-Length | Content-Type
    //                | Expires | Last-Modified | extension-header
    // extension-header = HTTP-header
    //       HTTP-header    = field-name ":" [ field-value ] CRLF
    //       field-name     = token
    //       field-value    = *( field-content | LWS )
    //       field-content  = <the OCTETs making up the field-value
    //                        and consisting of either *TEXT or combinations
    //                        of token, tspecials, and quoted-string>
    char * pFieldName  = strtok(readBuffer, pSpDelimiters);
    char * pFieldValue = strtok(NULL, pSpDelimiters);

    if (strcmp(pFieldName, "Content-Length:") == 0)
    {
      nContentLength = atoi(pFieldValue);
    }
    else if (strcmp(pFieldName, "Content-Type:") == 0)
    {
      if (strcmp(pFieldValue, "application/x-www-form-urlencoded") != 0)
        bIsUrlEncoded = false;
    }
  } while (strlen(readBuffer) > 0);    // empty string terminates
}

// Read the entity body of given length (after all the headers) into the buffer.
void readEntityBody(EthernetClient & client, int nContentLength, BUFFER & content)
{
  int i;
  char c;

  if (nContentLength >= sizeof(content))
    nContentLength = sizeof(content) - 1;  // Should never happen!

  for (i = 0; i < nContentLength; ++i)
  {
    c = client.read();
    //    Serial.print(c); // DEBUG
    content[i] = c;
  }

  content[nContentLength] = 0;  // Null string terminator

  //Serial.print("Content: ");
  //Serial.println(content);
}

// See if we recognize the URI and get its index; or -1 if we don't recognize it.
int GetUriIndex(char * pUri)
{
  int nUriIndex = -1;

  // select the page from the buffer (GET and POST) [start]
  for (int i = 0; i < NUM_URIS; i++)
  {
    if (strcmp_P(pUri, (PGM_P)pgm_read_word(&(http_uris[i]))) == 0)
    {
      nUriIndex = i;
      Serial.print("URI: ");
      Serial.println(pUri);
      break;
    }
  }
  //  Serial.print("URI: ");
  //  Serial.print(pUri);
  //  Serial.print(" Page: ");
  //  Serial.println(nUriIndex);

  return nUriIndex;
}

/**********************************************************************************************************************
  Read the next HTTP header record which is CRLF delimited.  We replace CRLF with string terminating null.
***********************************************************************************************************************/
void getNextHttpLine(EthernetClient & client, BUFFER & readBuffer)
{
  char c;
  int bufindex = 0; // reset buffer

  // reading next header of HTTP request
  if (client.connected() && client.available())
  {
    // read a line terminated by CRLF
    readBuffer[0] = client.read();
    readBuffer[1] = client.read();
    bufindex = 2;
    for (int i = 2; readBuffer[i - 2] != '\r' && readBuffer[i - 1] != '\n'; ++i)
    {
      // read full line and save it in buffer, up to the buffer size
      c = client.read();
      if (bufindex < sizeof(readBuffer))
        readBuffer[bufindex++] = c;
    }
    readBuffer[bufindex - 2] = 0;  // Null string terminator overwrites '\r'
  }
}

/**********************************************************************************************************************
                                                               Send Pages
       Full-Response  = Status-Line
                         ( General-Header
                         | Response-Header
                         | Entity-Header )
                        CRLF
                        [ Entity-Body ]

       Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
       General-Header = Date | Pragma
       Response-Header = Location | Server | WWW-Authenticate
       Entity-Header  = Allow | Content-Encoding | Content-Length | Content-Type
                      | Expires | Last-Modified | extension-header

***********************************************************************************************************************/
void sendPage(EthernetClient & client, int nUriIndex, BUFFER & requestContent)
{
  // send HTML header
  // sendProgMemAsString(client,(char*)pgm_read_word(&(contents_main[CONT_HEADER])));
  sendProgMemAsString(client, (char*)pgm_read_word(&(contents_main[CONT_TOP])));

  // send menu
  sendProgMemAsString(client, (char*)pgm_read_word(&(contents_main[CONT_MENU])));

  // send title
  sendProgMemAsString(client, (char*)pgm_read_word(&(contents_titles[nUriIndex])));

  if (nUriIndex == 1){
    client.println("<input type='hidden' value='" + feedback + "' id='feedback'>");
  }

  // send the body for the requested page
  sendUriContentByIndex(client, nUriIndex, requestContent);

  // Append the data sent in the original HTTP request
  //client.println("");
  // send POST variables
  //Serial.println(requestContent);

  // send footer
  sendProgMemAsString(client, (char*)pgm_read_word(&(contents_main[CONT_FOOTER])));
}

/**********************************************************************************************************************
                                                               Send content split by buffer size
***********************************************************************************************************************/
// If we provide string data then we don't need specify an explicit size and can do a string copy
void sendProgMemAsString(EthernetClient & client, const char *realword)
{
  sendProgMemAsBinary(client, realword, strlen_P(realword));
}

// Non-string data needs to provide an explicit size
void sendProgMemAsBinary(EthernetClient & client, const char* realword, int realLen)
{
  int remaining = realLen;
  const char * offsetPtr = realword;
  int nSize = sizeof(BUFFER);
  BUFFER buffer;

  while (remaining > 0)
  {
    // print content
    if (nSize > remaining)
      nSize = remaining;      // Partial buffer left to send

    memcpy_P(buffer, offsetPtr, nSize);

    if (client.write((const uint8_t *)buffer, nSize) != nSize)
      Serial.println("Failed to send data");

    // more content to print?
    remaining -= nSize;
    offsetPtr += nSize;
  }
}

/**********************************************************************************************************************
                                                               Send real page content
***********************************************************************************************************************/
// This method takes the contents page identified by nUriIndex, divides it up into buffer-sized
// strings, passes it on for STX substitution and finally sending to the client.
void sendUriContentByIndex(EthernetClient client, int nUriIndex, BUFFER & requestContent)
{
  // Locate the page data for the URI and prepare to process in buffer-sized chunks.
  const char * offsetPtr;               // Pointer to offset within URI for data to be copied to buffer and sent.
  char *pNextString;
  int nSubstituteIndex = -1;            // Count of substitutions so far for this URI
  int remaining;                        // Total bytes (of URI) remaining to be sent
  int nSize = sizeof(BUFFER) - 1;       // Effective size of buffer allowing last char as string terminator
  BUFFER buffer;

  if (nUriIndex < NUM_PAGES)
    offsetPtr = (char*)pgm_read_word(&(contents_pages[nUriIndex]));

  buffer[nSize] = 0;  // ensure there is always a string terminator
  remaining = strlen_P(offsetPtr);  // Set total bytes of URI remaining

  while (remaining > 0)
  {
    // print content
    if (nSize > remaining)
    {
      // Set whole buffer to string terminator before copying remainder.
      memset(buffer, 0, STRING_BUFFER_SIZE);
      nSize = remaining;      // Partial buffer left to send
    }
    memcpy_P(buffer, offsetPtr, nSize);
    offsetPtr += nSize;
    // We have a buffer's worth of page to check for substitution markers/delimiters.
    // Scan the buffer for markers, dividing it up into separate strings.
    if (buffer[0] == *pStxDelimiter)    // First char is delimiter
    {
      //sendSubstitute(client, nUriIndex, ++nSubstituteIndex, requestContent);
      --remaining;
    }
    // First string is either terminated by the null at the end of the buffer
    // or by a substitution delimiter.  So simply send it to the client.
    pNextString = strtok(buffer, pStxDelimiter);
    client.print(pNextString);
    remaining -= strlen(pNextString);
    // Scan for strings between delimiters
    for (pNextString = strtok(NULL, pStxDelimiter); pNextString != NULL && remaining > 0; pNextString = strtok(NULL, pStxDelimiter))
    {
      // pNextString is pointing to the next string AFTER a delimiter
      //sendSubstitute(client, nUriIndex, ++nSubstituteIndex, requestContent);
      --remaining;
      client.print(pNextString);
      remaining -= strlen(pNextString);
    }
  }
}

// Call this method in response to finding a substitution character '\002' within some
// URI content to send the appropriate replacement text, depending on the URI index and
// the substitution index within the content.
/*void sendSubstitute(EthernetClient client, int nUriIndex, int nSubstituteIndex, BUFFER & requestContent)
  {
  if (nUriIndex < NUM_PAGES)
  {
    // Page request
    switch (nUriIndex)
    {
      case 1:  // page 2
        client.print("<b>Insert #");
        client.print(nSubstituteIndex);
        client.print("</b>");
        break;
      case 2:  // page 3
        switch (nSubstituteIndex)
        {
          case 0:  // LedOn button send value
            if (strncmp(requestContent, "LedOn=", 6) == 0)
              setLed(strncmp(&requestContent[6], "true", 4) == 0);

            client.print(isLedOn ? "false" : "true");
            break;
          case 1:  // LedOn button legend
            client.print(isLedOn ? "Off" : "On");
            break;
          case 2:  // LedOn partial image name
            client.print(isLedOn ? "on" : "off");
            break;
          default:
            break;
        }
        break;
    }
  }
  }*/
