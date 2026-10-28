# Dino-Ball-Rush_Using_Arduino

A simple arcade game for Arduino and a 16x2 I2C LCD. Move the dino, collect the balls, and dodge the cacti. You have 3 lives.


![image alt](https://github.com/digbijoy1908/Dino-Ball-Rush_Using_Arduino/blob/16657b1ba65599bf9514dc808bdced179e427027/dino-ball-rush-banner.png)


## How to play

- **LEFT / RIGHT** buttons move the dino sideways.
- **JUMP** button swaps the dino between the top and bottom row.
- Collect balls to score points.
- Hitting a cactus costs one life.
- After 3 lives the **GAME OVER** screen stays until you press **JUMP**, which starts a new game.

## Hardware

| Part | Quantity |
|------|----------|
| Arduino Uno / Nano (or compatible) | 1 |
| 16x2 LCD with I2C backpack (address `0x27`) | 1 |
| Push buttons | 3 |
| Jumper wires | several |

## Circuit Diagram


## Wiring

| Component | Arduino pin |
|-----------|-------------|
| LCD SDA | A4 |
| LCD SCL | A5 |
| LCD VCC | 5V |
| LCD GND | GND |
| JUMP button | D2 to GND |
| LEFT button | D3 to GND |
| RIGHT button | D4 to GND |

The buttons use the internal pull-up resistors, so each button connects between its pin and GND. No external resistors are needed.

## Software setup

1. Install the [Arduino IDE](https://www.arduino.cc/en/software).
2. Install the **LiquidCrystal I2C** library from *Sketch > Include Library > Manage Libraries*.
3. Open `dino_ball_rush/dino_ball_rush.ino`.
4. Select your board and port, then click **Upload**.

If the screen stays blank, change the LCD address in the sketch from `0x27` to `0x3F`, or adjust the contrast screw on the back of the LCD.

## Settings

These values are at the top of the sketch:

| Setting | Meaning |
|---------|---------|
| `OBS_SPEED` | Cactus speed in ms per step (lower is faster) |
| `COIN_TIMEOUT` | How long a ball stays on screen |
| `MAX_LIVES` | Number of lives |

