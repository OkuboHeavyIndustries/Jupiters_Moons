#include <Arduino.h>
#include <U8g2lib.h>
#include <TinyGPS++.h> 
#include <Time.h>
#include <Ephemeris.h>
#include <Adafruit_NeoPixel.h>

//the line below specific to the OLED being used.  Replace F with 1 or 2 to reduce buffer size
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

Adafruit_NeoPixel pixels(1, PIN_NEOPIXEL);
 
#define Jupiter_width  8
#define Jupiter_height 8
static unsigned char Jupiter_bits[] = {
 0x18,0x38,0x00,0xfe,0x00,0xce,0x00,0x18};

#define Jupiter_small_width  5
#define Jupiter_small_height 5
static unsigned char Jupiter_small_bits[] = {
 0xe0,0xff,0xe0,0xff,0xe0};
 

int gpschar;
static const uint32_t GPSBaud = 9600;

unsigned long previousMillis = 0;  // will store last time LED was updated

const long interval = 10000;  // interval at which to blink (milliseconds)

double lat_val, lng_val, alt_m_val;
uint8_t hr_val, min_val, sec_val, hr_val_jp, sats_val;
bool loc_valid, alt_valid, time_valid, sats_valid, date_valid;
double year_val;
double month_val;
double day_val;
double dMyLAT;
double dMyLON;
double dMyALT;

int xorigin = 64;
int yorigin = 26;

float jazi;
float jalt;
float Sazi;
float Salt;

int cYear, cMonth;

int a;
int b;
long c;
long c2;
  
long d;
float N;
float N2;
  // Calculate Galilaen moon positions
float P;
float MT;
float MJ;
float JJ;
float VT;
float VJ;
float K;
float DT;
float Z1;
float I;
float F;
float F1;
float U1 ; //Orbital angle of Io
float U2; //Orbital angle of Europa
float U3; //Orbital Angle of Ganymede
float U4; //Orbital angle of Callisto
float X1; //x-axis offset of Io
float X2; //x-axis offset of Europa
float X3; //x-axis offset of Ganymede
float X4; //x-axis offset of Callisto
int modU1;  //Modulus Orbital angle of Io
int modU2;  //Modulus Orbital angle of Europa
int modU3;  //Modulus Orbital angle of Ganymede
int modU4;  //Modulus Orbital angle of Callisto

int off = 16;   //offset for labels
int factor = 2; //Scale size of Jupiter to moon orbits.

char time_string[32];

TinyGPSPlus gps;

void setup() {

  #if defined(NEOPIXEL_POWER)
  // If this board has a power control pin, we must set it to output and high
  // in order to enable the NeoPixels. We put this in an #if defined so it can
  // be reused for other boards without compilation errors
  pinMode(NEOPIXEL_POWER, OUTPUT);
  digitalWrite(NEOPIXEL_POWER, HIGH);
  #endif

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(40); // not so bright

  Serial1.begin(GPSBaud);
  Serial.begin(GPSBaud);
 // while (!Serial) {
 //   ; // wait for serial port to connect. Needed for native USB port only
 // }
 
  u8g2.begin();

  u8g2.firstPage();
  do {
 } while ( u8g2.nextPage() );
  u8g2.setFont(u8g2_font_u8glib_4_tr);    // u8g2 font 
  u8g2.setBitmapMode(1);
}

void loop() {

  while (Serial1.available()>0){  
  gpschar = Serial1.read(); //read raw gps data to gpschar
  gps.encode(gpschar);      // extract useful info from raw gps data
  }

  lat_val = gps.location.lat();  /* Get latitude data */
  loc_valid = gps.location.isValid(); /* Check if valid location data is available */
  lng_val = gps.location.lng(); /* Get longtitude data */
  alt_m_val = gps.altitude.meters();  /* Get altitude data in meters */
  alt_valid = gps.altitude.isValid(); /* Check if valid altitude data is available */
  hr_val = gps.time.hour(); /* Get hour */
  min_val = gps.time.minute();  /* Get minutes */
  sec_val = gps.time.second();  /* Get seconds */
  time_valid = gps.time.isValid();  /* Check if valid time data is available */
  year_val = (gps.date.year());
  month_val = (gps.date.month());
  day_val = (gps.date.day());
  sats_valid = gps.satellites.isValid();
  sats_val = gps.satellites.value();
  date_valid = gps.date.isValid();

    if (hr_val < 15) { hr_val_jp = hr_val + 9;  // convert UTC to Japan time zone
     }
     else {hr_val_jp = hr_val -15;
     }

  dMyLAT = gps.location.lat();
  dMyLON = gps.location.lng();
  dMyALT = gps.altitude.meters();

unsigned long currentMillis = millis();

if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

  //decimal day calculation

  float dDay = ((float)day_val + ((float)hr_val/24) + ((float)min_val/1440) + ((float)sec_val/86400));
  
  jSats(year_val, month_val, dDay);
  jupiter();
  }

  if (jalt > 0){
pixels.setPixelColor(0, pixels.Color((jalt * 2.8), (jalt * 2.8), (jalt * 0.7)));
pixels.show();
} else {
           pixels.clear();
           pixels.show(); 
}

u8g2.firstPage();
u8g2.drawFrame(0, 0, 128,64);  //setup fixed screen info and borders
u8g2.drawLine(0, 9, 128,9);
u8g2.setFont(u8g2_font_u8glib_4_tr);    // u8g2 font 
u8g2.drawStr(2, 7, " JUPITER'S MOONS ");
u8g2.drawLine(0,45, 128,45);
u8g2.drawStr(2, 52, " Jupiter Azi: ");
u8g2.drawStr(78, 52, " El: ");
if (!loc_valid) { 
u8g2.setCursor(48, 52);
u8g2.print("****");   
u8g2.setCursor(95, 52);
u8g2.print("****"); 
}       
 else {     
u8g2.setCursor(48, 52);
u8g2.print(jazi);   
u8g2.setCursor(95, 52);
u8g2.print(jalt); 
 }
    
u8g2.drawLine(0, 54, 128,54);
u8g2.drawStr(2, 61, "OKUBO HEAVY INDUSTRIES");

if (!time_valid)
        {
          u8g2.drawStr(74, 7, "Time : ********");
         
        }
        else
        {
          char time_string[32];
          sprintf(time_string, "Time : %02d:%02d:%02d \n", hr_val_jp, min_val, sec_val);
          u8g2.setCursor(74, 7);
          u8g2.print(time_string);    
        }  
if ((year_val < 2022) || (!time_valid)){
  u8g2.drawStr(7, 30, "Acquiring Satellite Downlink");
  Serial.println("Acquiring Satellite Downlink");
  u8g2.setCursor(80, 30);  
  }
else{
//u8g2.setFont(u8g2_font_tinytim_tr);    // u8g2 font 
u8g2.setFont(u8g2_font_tinyunicode_tr);   // u8g2 font 

//Scale display if Callisto x-axis offset is large
if ((X4 > 20)||(X4 < -20)){
  factor = 2;
  u8g2.drawXBM(xorigin-2,yorigin-2, 5, 5, Jupiter_small_bits);
} else{
  factor = 3;
  u8g2.drawXBM(xorigin-4,yorigin-4, 8, 8, Jupiter_bits);
}

u8g2.drawCircle(xorigin,yorigin,factor+1);//Jupiter
//y-axis offset isn't calculated
u8g2.drawDisc(xorigin-(X1*factor),yorigin,1); //Draw Io

if ((X1 > -1) && (X1 < 1) && (modU1 > 175) && (modU1 < 185)){
  // Don't label Io if eclipsed by Jupiter
  }else{
    if ((modU1 > 90) && (modU1 < 270)){  //Check if Io is on far side of orbit (travelling left on the screen) and place label above
      off = -8;
    } else{
      off = 16; //otherwise put label below
    }
u8g2.drawStr(xorigin-(X1*factor),yorigin+off, "I"); //Label Io
}
u8g2.drawDisc(xorigin-(X2*factor),yorigin,1); //Draw Europa
if ((X2 > -1) && (X2 < 1) && (modU2 > 175) && (modU2 < 185)){
  //
  }else{
    if ((modU2 > 90) && (modU2 < 270)){
      off = -8;
    } else{
      off = 16;
    }
u8g2.drawStr(xorigin-(X2*factor),yorigin+off, "E"); //Label Europa
}
u8g2.drawDisc(xorigin-(X3*factor),yorigin,1); // Draw Ganymede
if ((X3 > -1) && (X3 < 1) && (modU3 > 175) && (modU3 < 185)){
  //
  }else{
    if ((modU3 > 90) && (modU3 < 270)){
      off = -8;
    } else{
      off = 16;
    }
u8g2.drawStr(xorigin-(X3*factor),yorigin+off, "G"); //Label Ganymede
  }
u8g2.drawDisc(xorigin-(X4*factor),yorigin,1); //Draw Callisto
if ((X4 > -1) && (X4 < 1) && (modU4 > 175) && (modU4 < 185)){
  //
  }else{
    if ((modU4 > 90) && (modU4 < 270)){
      off = -8;
    } else{
      off = 16;
    }
u8g2.drawStr(xorigin-(X4*factor),yorigin+off, "C"); // label Callisto
  }
}
u8g2.nextPage();
//Serial.println(Sazi);
//Serial.println(Salt);
//Serial.println(jazi);
//Serial.println(jalt);

}

void printDateAndTime(int day_val, int month_val, int year_val, int hr_val, int min_val, int sec_val )
{
  Serial.print(day_val);
  Serial.print("/");
  Serial.print(month_val);
  Serial.print("/");
  Serial.print(year_val);
  Serial.print(" ");
  Serial.print(hr_val);
  Serial.print(":");
  Serial.print(min_val);
  Serial.print(":");
  Serial.print(sec_val);
}

void equatorialCoordinatesToString(EquatorialCoordinates coord, char raCoord[14] , char decCoord[14])
{
  int raHour,raMinute;
  float raSecond;
  Ephemeris::floatingHoursToHoursMinutesSeconds(coord.ra, &raHour, &raMinute, &raSecond);
    
  sprintf(raCoord," %02dh%02dm%02ds.%02d",raHour,raMinute,(int)raSecond,(int)round(((float)(raSecond-(int)raSecond)*pow(10,2))));
    
  int decDegree,decMinute;
  float decSecond;
  Ephemeris::floatingDegreesToDegreesMinutesSeconds(coord.dec, &decDegree, &decMinute, &decSecond);
    
  if(decDegree<0)
  {
    sprintf(decCoord,"%02dd%02d'%02d\".%02d",(int)decDegree,decMinute,(int)decSecond,(int)round(((float)(decSecond-(int)decSecond)*pow(10,2))));
  }
  else
  {
    sprintf(decCoord," %02dd%02d'%02d\".%02d",(int)decDegree,decMinute,(int)decSecond,(int)round(((float)(decSecond-(int)decSecond)*pow(10,2))));
  }
}

void printEquatorialCoordinates(EquatorialCoordinates coord)
{
  if( isnan(coord.ra) ||  isnan(coord.dec))
  {
    // Do not work for Earth of course...
    Serial.println("R.A: -");
    Serial.println("Dec: -");
        
    return;
  }
    
  char raCoord[14];
  char decCoord[14];
  equatorialCoordinatesToString(coord,raCoord,decCoord);

  Serial.print("R.A: ");
  Serial.println(raCoord);

  Serial.print("Dec: ");
  Serial.println(decCoord);

  return;
}

void printHorizontalCoordinates(HorizontalCoordinates coord)
{
  if( isnan(coord.azi) ||  isnan(coord.alt))
  {
    // Do not work for Earth of course...
    Serial.println("Azi: -");
    Serial.println("Alt: -");
        
    return;
  }

  
  jazi = (coord.azi);
  jalt = (coord.alt);


//if (solarSystemObjectName == Sun){
//  Sazi = (coord.azi);
//  Salt = (coord.alt);
//}
  Serial.print("Azi: ");
  Serial.print(coord.azi,2);
  Serial.println("˚");

  Serial.print("Alt: ");
  Serial.print(coord.alt,2);
  Serial.println("˚");
}

void printSolarSystemObjects(int day_val, int month_val, int year_val, int hr_val, int min_val, int sec_val)
{
  Serial.println("_____________________________________");
  printPlanet("Sun",      Sun, day_val, month_val, year_val, hr_val, min_val, sec_val);
  Serial.println("_____________________________________");
  printPlanet("Jupiter",      Jupiter, day_val, month_val, year_val, hr_val, min_val, sec_val);
  Serial.println("_____________________________________");
  
  

}

void printPlanet(char *solarSystemObjectName, SolarSystemObjectIndex index, int day_val, int month_val, int year_val, int hr_val, int min_val, int sec_val )
{
  SolarSystemObject solarSystemObject = Ephemeris::solarSystemObjectAtDateAndTime(index, day_val, month_val, year_val, hr_val, min_val, sec_val);

  if( index == Earth )
  {
    Serial.println(solarSystemObjectName);
    Serial.println("Look under your feet... ;)");

    return;
  }
  
  Serial.println(solarSystemObjectName);
  printEquatorialCoordinates(solarSystemObject.equaCoordinates);
  printHorizontalCoordinates(solarSystemObject.horiCoordinates);
  //Serial.println(coord.azi,2);
  //Serial.println(coord.alt,2);
  if( isnan(solarSystemObject.diameter) )
  {
    // Do not work for Earth of course...
    Serial.println("Dist: -");
    Serial.println("Diam: -");
  }
  else
  {
    Serial.print("Dist: ");
    if( index != EarthsMoon )
    {
      Serial.print(solarSystemObject.distance,3);
      Serial.println(" AU");
    }
    else
    {
      Serial.print(solarSystemObject.distance/6.68459e-9);
      Serial.println(" Km");
    }
    
    if( solarSystemObject.diameter <= 1 )
    {
      Serial.print("Diam: ");
      Serial.print(solarSystemObject.diameter*60,2);
      Serial.println("\"");
    }
    else
    {
      Serial.print("Diam: ");
      Serial.print(solarSystemObject.diameter,2);
      Serial.println("'");
    }
  }
}

void jupiter(){
// Set location on earth for horizontal coordinates transformations
  Ephemeris::setLocationOnEarth(dMyLAT,  // Lat: 48°50'11"
                                dMyLON); // Lon: -2°20'14"

  // East is negative and West is positive
  Ephemeris::flipLongitude(false);
    
  // Set altitude to improve rise and set precision
  Ephemeris::setAltitude(dMyALT);
                                
  // Compute and print solar system objects
  Serial.print("Data of Solar system objects (");
  printDateAndTime(day_val,month_val,year_val,hr_val,min_val,sec_val);
  Serial.println(")");
  printSolarSystemObjects(day_val,month_val,year_val,hr_val,min_val,sec_val);

}

void jSats(int tYear, int tMonth, float tDay) {
  
  if (tMonth < 3) {
    cYear = tYear - 1;
    cMonth = tMonth + 12;
  } else {
    cYear = tYear;
    cMonth = tMonth;
  }
  // Calculate the Julian Date offset from Epoch
  a = cYear / 100;
  b = 2 - a + (int)(a / 4);
  c = 365.25 * cYear;
  c2 = 365.25 * (cYear+4716);
  
  d = 30.6001 * (cMonth + 1);
  N = b + c + d + tDay - 694025.5;
  N2 = c2 + d + tDay - 1524.5;


  // Calculate Galilaen moon positions
  P = PI / 180;
  MT = (358.476 + 0.9856003 * N) * P;
  MJ = (225.328 + 0.0830853 * N) * P;
  JJ = 221.647 + 0.9025179 * N;
  VT = 1.92 * sin(MT) + 0.02 * sin(2 * MT);
  VJ = 5.55 * sin(MJ) + 0.17 * sin(2 * MJ);
  K = (JJ + VT - VJ) * P;
  DT = sqrt(28.07 - 10.406 * cos(K));
  Z1 = sin(K) / DT;
  I = atan(Z1 / sqrt(1 - Z1 * Z1));
  I = I / P;
  F = (N - DT / 173);
  F1 = I - VJ;
  U1 = 84.5506 + 203.405863 * F + F1; //Orbital angle of Io
  U2 = 41.5015 + 101.2916323 * F + F1; //Orbital angle of Europa
  U3 = 109.9770 + 50.2345169 * F + F1; //Orbital Angle of Ganymede
  U4 = 176.3586 + 21.4879802 * F + F1; //Orbital angle of Callisto
  X1 = 5.906 * sin(U1 * P + PI); //x-axis offset of Io
  X2 = 9.397 * sin(U2 * P + PI); //x-axis offset of Europa
  X3 = 14.989 * sin(U3 * P + PI); //x-axis offset of Ganymede
  X4 = 26.364 * sin(U4 * P + PI); //x-axis offset of Callisto
  modU1 = (int(U1)%360);  //Modulus Orbital angle of Io
  modU2 = (int(U2)%360);  //Modulus Orbital angle of Europa
  modU3 = (int(U3)%360);  //Modulus Orbital angle of Ganymede
  modU4 = (int(U4)%360);  //Modulus Orbital angle of Callisto

  // Print out results to serial monitor
  Serial.println();
  //Serial.println(N);
  //Serial.println(modU1);
  //Serial.println(modU2);
  //Serial.println(modU3);
 // Serial.println(modU4);
  Serial.println();
  Serial.print(F("The Galilean moons of Jupiter offset and orbital angle on "));
  Serial.print(tMonth);
  Serial.print("/");
  Serial.print(tDay,6);
  Serial.print("/");
  Serial.println(tYear);
  Serial.println();
  Serial.print(F("      Io: "));
  Serial.print(X1,6);
  Serial.print("  ");
  Serial.println(modU1);
 
  Serial.print(F("  Europa: "));
  Serial.print(X2,6);
  Serial.print("  ");
  Serial.println(modU2);

  Serial.print(F("Ganymede: "));
  Serial.print(X3,6);
  Serial.print("  ");
  Serial.println(modU3);

  Serial.print(F("Callisto: "));
  Serial.print(X4,6);
  Serial.print("  ");
  Serial.println(modU4);
  Serial.println();


Serial.println("Ground station location");
Serial.print("Latitude:  ");
Serial.println(dMyLAT);
Serial.print("Longitude:  ");
Serial.println(dMyLON);
Serial.print("Altitude:  ");
Serial.println(dMyALT);
Serial.print("Satellite Lock:  ");
Serial.println(sats_val);
Serial.println("  ");

}
