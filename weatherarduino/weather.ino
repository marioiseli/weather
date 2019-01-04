/* 
 * Copyright (c) 2019 Mario Iseli
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *  
 * Needs the following library to compile:
 * https://github.com/Seeed-Studio/Grove_BME280
 */

#include <Seeed_BME280.h>
#include <Wire.h>
#include <LiquidCrystal.h>         

LiquidCrystal D(12, 11, 5, 4, 3, 2);
BME280 B;

void setup()
{
  Serial.begin(9600);
  Serial.print("Booting up\n");
  B.init();
  Serial.print("Barometer initialized\n");
  D.begin(16,2);
  Serial.print("LCD initialized\n");
}

void loop()
{
  D.clear();
  D.setCursor(0,0);
  D.print(B.getTemperature());
  D.print("C (");
  D.print(B.getHumidity());
  D.print("%)");
  D.setCursor(0,1);
  D.print(B.getPressure());
  D.print("Pa");
  Serial.print("Temperature: ");
  Serial.print(B.getTemperature());
  Serial.print("C\n");
  Serial.print("Pressure: ");
  Serial.print(B.getPressure());
  Serial.print("Pa\n");
  Serial.print("Relative humidity: ");
  Serial.print(B.getHumidity());
  Serial.print("%\n");
  delay(1000);
}
