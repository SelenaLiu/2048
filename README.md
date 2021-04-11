# 2048
ECE243 Computer Organization project made in the winter semester of 2021. Developed in C for ARM processor by Selena Liu and Navidur Rahman.

![alt_text](https://github.com/SelenaLiu/2048/blob/main/screenshots/2048Start.jpg)

## Instructions
This game environment is a 4x4 grid, and you begin with 2 tiles valued at either 2 or 4. By pressing the arrow keys, the player can shift all the tiles to one of the four walls. When two tiles with the same value are adjacent to each other and are moved in the direction of that adjacency, the tiles combine and double their value. The goal of the game is to create the 2048 tile. You lose when you can no longer combine any more tiles and when there are no more spaces to spawn a new tile.

## Usage
This C program can be run on an ARMv7 FPGA board. The screenshots and demos are shown via CPUlator, a program that mimics the functionality of an FPGA.
