# Hack & Tell
Nov 27 2019

- follow up on last time. Thanks Marc
- recap:
https://www.youtube.com/watch?v=-uOwARIPkDc
- Taito's Ice Cold Beer from 1983
- The level 10 switch in the Ice Cold Beer machine at the Zed80 arcade bar in Toronto is broken, so even if you win it looks like you don't.
- yeah so let's make a cheap ripoff (buy Zeke's Peak on steam)

## How it's going to work
- layers of plywood
- telescoping rod
- motors and pulley
- controls and switches
- microcontroller
- game loop / state machine for game logic
- display
- music and sound effects

easy!

## Problems:
- pulleys
  - very difficult to get the system to be low friction
- motor speed control
  - needed to leave the comfort zone of Arduino programming and needed to explore the bowels of the ATMega328
  - ran out of timers :(
- running out of pins
  - using analog input to read several inputs
  - multiplexer (now broken)
- wiring
  - hard to debug problems often come down to bad wiring
  - molex connectors suck
- not knowing how to write C good
