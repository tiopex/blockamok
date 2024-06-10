<h1 align="center">Blockamok</h1>

<p align="center"><b>A CPU-only, from-scratch 3D game written in C, where you dodge blocks</b></p>

<p align="center">You accidentally fly your spaceship into a wormhole. To your shock, you find yourself in an alternate dimension filled with blocks. The throttle on your spaceship is broken and keeps increasing the speed. How far can you make it?</p>

<p align="center"><img alt="Gameplay" src="gameplay.gif"/></p>

# Background

I made this game to play with 3D graphics. Specifically, I wanted to challenge myself to create a 3D game without any pre-made 3D engine or utilities. The 3D cubes are all made by strategically drawing polygons to create the illusion of perspective.

# Compiling for MiyooCFW

```sh
docker pull miyoocfw/toolchain-static-musl:latest
docker run --volume ./:/src/ -it miyoocfw/toolchain-static-musl:latest
cd /src
make
```

# Running

copy `blockamok` and `Mono.ttf` to the device

# License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
