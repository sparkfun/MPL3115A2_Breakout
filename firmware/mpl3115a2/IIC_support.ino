// There are two functions in this sketch. You can use them in
// main sketch as any other function. 

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
