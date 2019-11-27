# :video_game: LukeWarmBeer :beers:

## Contributing and Forking

You can set up your environment using the bootstrap script `. scripts/boostrap`, then any of the invoke commands in `tasks.py`.

| Command   | Effect                                                                       |
|-----------|------------------------------------------------------------------------------|
| `compile` | Compiles the `ino` sketch in `src/LukeWarmBeer`. Hardcoded to `avr` targets. |
| `upload`  | Uploads the compiled sketch to a board\*                                     |
| `devices` | Lists connected devices                                                      |

The current setup assumes that only one board is connected at a time. It will grab the port of the first connected device it sees through `arduino-cli board list` and use it as target.


## :hammer: Build
[Build Description](build/build.md)


## :electric_plug: Circuit

Circuit was drawn using easyeda.com
https://easyeda.com/editor#id=|5c82f9019097461287bc8b2208835527

![Circuit Diagram](circuit/circuit.png)


## :pencil2: Next steps

- [x] design thing
- [x] build thing
- [x] design circuit
- [x] build circuit
- [x] find a good ball
- [x] design code
- [ ] write code
- [ ] replace multiplexer chip
- [ ] music and sound effects
- [ ] art and polish


## Bill of Materials

| Description                   | Part Number                       | Quantity | Approx Cost Per |
|-------------------------------|-----------------------------------|----------|-----------------|
| Microcontroller               | Arduino Uno                       |          |                 |
| Quad AND                      | 74ACT11008N                       | 2        | 3.82 CAD        |
| 16 channel multiplexer        | 74HC4067                          |          |                 |
| 7 segment display driver      | MAX7219                           | 2        |                 |
| Stepper motor driver          | A4988 breakout (Pololu)           | 2        | 5.95 USD        |
| Timing belt and shaft coupler | https://tinyurl.com/y4ls89b8      |          |                 |
| Telescoping rod               | e.g. https://tinyurl.com/y29gk65v |          |                 |
| 5/8 inch Ball bearing         |                                   |          |                 |
| ...                           |                                   |          |                 |
