/*
  MPL3115A2 Altitude Sensor Example
  SparkFun Electronics, A.Weiss, 7/17/2012
  Beerware License - whatever
  
  Hardware Connections:
  -VCC = 3.3V
  -SDA = A4, SCL = A5
  -INT pins can be left unconnected for this demo
  
  Usage:
  -Serial terminal at 9600bps
  -Prints altitude in meters, temperature in degrees C, with 1/16 
   resolution.
  -software enabled interrupt on new data, ~1Hz with full resolution
  -the IIC_support file contains the IIC read and write functions
  -IIC_support.ino is just another .ion sketch that holds functions
   that do not need to be modified.
  
*/

#include <Wire.h> // for IIC communication

const int SENSORADDRESS = 0x60; // address specific to the MPL3115A1, 
                                // value found in datasheet
void setup()
{
  Wire.begin();        // join i2c bus
  Serial.begin(9600);  // start serial for output
  
  // This is a basic II2 communication check. If the sensor doesn't
  // return decicmal number 196 (see 0x0C register in datasheet), 
  // "IIC bad" is printed, otherwise nothing happens. 
  if(IIC_Read(0x0C) == 196); //checks who_am_i bit for basic I2C handshake test
  else Serial.println("i2c bad");
  
  // Enable and configure the sensor. 
  sensor_config();
}

void loop()
{
  sensor_read_data();
  // your code here
}

boolean check_new()
{
  // This function check to see if there is new data. 
  // You can call this function and it will return TRUE if there is
  // new data and FALSE if there is no new data.
  
  // If INT_SOURCE (0x12) register's DRDY flag is enabled, return
  if(IIC_Read(0x12) == 0x80) // check INT_SOURCE register on
                             // new data ready (SRC_DRDY)
  {
      return true;
  }
  else return false;
}

void sensor_config()
{
  // To configure the sensor, find the register hex value and 
  // enter it into the first field of IIC_Write (see below). Then
  // fingure out the hex value of the data you want to send.
  // 
  // For example:
  // For CTRL_REG1, the address is 0x26 and the data is 0xB9
  // 0xB9 corresponds to binary 1011 1001. Each place holder 
  // represents a data field in CTRL_REG1. 
  
  // CTRL_REG1 (0x26): enable sensor, oversampling, altimeter mode 
  IIC_Write(0x26, 0xB9);
  
  // CTRL_REG4 (0x29): Data ready interrupt enbabled
  IIC_Write(0x29, 0x80); 
  
  // PT_DATA_CFG (0x13): enable both pressure and temp event flags 
  IIC_Write(0x13, 0x07);
  
  // This configuration option calibrates the sensor according to 
  // the sea level pressure for the measurement location
  
  // BAR_IN_MSB (0x14):
  IIC_Write(0x14, 0xC6);
  
  // BAR_IN_LSB (0x15):
  IIC_Write(0x15, 0x5B);
}

void sensor_read_data()
{
  // This function reads the altitude and temperature registers, then 
  // concatenates the data together, and prints in values of 
  // meters for altitude and degrees C for temperature. 
  
  // variables for the calculations
  int m_altitude, m_temp, c_altitude; 
  // these must be floats since there is a fractional calculation
  float l_altitude, l_temp;  
  float altitude, temperature;
  
  // read registers 0x01 through 0x05
  m_altitude = IIC_Read(0x01);
  c_altitude = IIC_Read(0x02);
  // the least significant bytes l_altitude and l_temp are 4-bit,
  // fractional values, so you must cast the calulation in (float),
  // shift the value over 4 spots to the right and divide by 16 (since 
  // there are 16 values in 4-bits). 
  l_altitude = (float)(IIC_Read(0x03)>>4)/16.0;
  m_temp = IIC_Read(0x04); //temp, degrees
  l_temp = (float)(IIC_Read(0x05)>>4)/16.0; //temp, fraction of a degree
  
  // here is where we calculate the altitude and temperature
  altitude = (float)((m_altitude << 8)|c_altitude) + l_altitude;
  temperature = (float)(m_temp + l_temp);
  
  // wait here for new data
  while(check_new() == false);
  
  // once there is new data, it is printed
  Serial.print(altitude); // in meters
  Serial.print(",");
  Serial.println(temperature); // in degrees C
}

// These are the two I2C functions in this sketch.
byte IIC_Read(byte regAddr)
{
  // This function reads one byte over IIC
  Wire.beginTransmission(SENSORADDRESS);
  Wire.write(regAddr);  // Address of CTRL_REG1
  Wire.endTransmission(false); // Send data to I2C dev with option
                               //  for a repeated start. THIS IS
                               //  NECESSARY and not supported before
                               //  Arduino V1.0.1!!!!!!!!!
  Wire.requestFrom(SENSORADDRESS, 1); // Request the data...
  return Wire.read();
}

void IIC_Write(byte regAddr, byte value)
{
  // This function writes one byto over IIC
  Wire.beginTransmission(SENSORADDRESS);
  Wire.write(regAddr);
  Wire.write(value);
  Wire.endTransmission(true);
}
