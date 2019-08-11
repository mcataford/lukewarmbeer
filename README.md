# :video_game: LukeWarmBeer :beers:

## Contributing

You can set up your environment using the bootstrap script `. scripts/boostrap`, then any of the invoke commands in `tasks.py`.

|Command|Effect|
|`compile`|Compiles the `ino` sketch in `src/LukeWarmBeer`. Hardcoded to `avr` targets.|
|`upload`|Uploads the compiled sketch to a board\*|
|`devices`|Lists connected devices|

_* The current setup assumes that only one board is connected at a time. It will grab the port of the first connected device it sees through `arduino-cli board list` and use it as target._
