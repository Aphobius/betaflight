/*
 * This file is part of Cleanflight and Betaflight.
 *
 * Cleanflight and Betaflight are free software. You can redistribute
 * this software and/or modify this software under the terms of the
 * GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Cleanflight and Betaflight are distributed in the hope that they
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdbool.h>
#include <stdint.h>

#include "platform.h"

#include "fc/init.h"

#include "scheduler/scheduler.h"

#include "drivers/accgyro/accgyro_fake.h"
#include "rx/rx.h"
#include "rx/msp.h"
#include "target/SITL/target.h"
#include <math.h>

void run(void);

int main(void)
{
    init();

    run();

    return 0;
}

void FAST_CODE FAST_CODE_NOINLINE run(void)
{
    long long counter = 0;
    double angle = 0;
    uint16_t rcData[4];

    while (true)
    {
        double xGyro = 0;
        double yGyro = 0;
        double zGyro = 0;

        if (counter > 100000)
        {
            angle += 0.0001;
            xGyro = sin(angle) * 1024;
            yGyro = cos(angle) * 1024;
            zGyro = -sin(angle) * 1024;
        }
        
        fakeGyroSet(fakeGyroDev, xGyro, yGyro, zGyro);

        if (counter % 100 == 0)
        {
            if (counter < 100000)
            {
                rcData[0] = 1500;
                rcData[1] = 1500;
                rcData[3] = 1500;
                rcData[2] = 988;
                rcData[4] = 1000;
            }
            else
            {
                rcData[0] = 1500 + sin(angle) * 500;
                rcData[1] = 1500 + cos(angle) * 500;
                rcData[3] = 1500 - sin(angle) * 500;
                rcData[2] = 988;
                rcData[4] = 1500;
            }

            rxMspFrameReceive(rcData, 5);
        }

        int16_t* motors = &simMotorsPwm;
        printf("M1: %6d M2: %6d M3: %6d M4: %6d\n", motors[0], motors[1], motors[2], motors[3]);
        
        counter++;

        scheduler();
        processLoopback();
        delayMicroseconds_real(50); // max rate 20kHz
    }
}
