/**

  Nagios Monitor for the Spark Core
  Copyright (C) 2014 Nicolas Cortot
  https://github.com/ncortot/spark-nagios-monitor

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "application.h"
#include "ht1632c.h"

#define MATRIX_DATA_PIN         D3 // blue
#define MATRIX_CLK_PIN          D4 // white
#define MATRIX_CS_PIN           D5 // yellow
#define MATRIX_WR_PIN           D6 // green

#define BRIGHTNESS              0xa

ht1632c matrix = ht1632c(MATRIX_DATA_PIN, MATRIX_WR_PIN, MATRIX_CLK_PIN,
                         MATRIX_CS_PIN, GEOM_32x16, 2);

char buffer[512];
int offset = 0;


void setup()
{
    // Serial over USB used for debugging
    Serial.begin(115200);
    Serial.println("Starting...");

    // Serial line used for control
    Serial1.begin(115200);

    // Set up the LED matrix
    matrix.begin();
    matrix.setBrightness(BRIGHTNESS);
    matrix.rect(0, 0, 63, 15, ORANGE);
    matrix.sendframe();
}


void loop()
{
    // Tune brightness to ambient luminosity
    int32_t brightness = analogRead(A7);
    matrix.setBrightness(map(brightness, 0, 0xfff, 0, 0xf));

    // Read the bitmap on the serial line
    while (Serial1.available()) {
        char c = (char) Serial1.read();
        if (c == 0xff) {
            if (offset > 0) {
                display_frame(buffer);
            }
            offset = 0;
        } else {
            if (offset < 512) {
                buffer[offset++] = c;
            }
        }
    }
}


void display_frame(const char *bits)
{
    matrix.clear();

    for (int y = 0; y < 16; ++y) {
        for (int x = 0; x < 64; ++x) {
            char b = bits[(y * 32) + (x / 2)];
            switch ((x & 0x1 ? b : b >> 4) & 0xd) {
              case 0x5:
                matrix.setPixel(x, y, GREEN);
                break;
              case 0x9:
                matrix.setPixel(x, y, RED);
                break;
              case 0xd:
                matrix.setPixel(x, y, ORANGE);
                break;
            }
        }
    }

    matrix.sendframe();
}
