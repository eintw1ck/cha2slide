## What is ChaChaSlide?

A PNG Encoder with symmetric encryption based on blake2b and chacha20.

## Why though?

I wanted to create something fairly light (hence the choice of C) which could be used on Camera firmware for encryption of images on-device for journalism, or photography in high risk environments like warzones.

## Why is it so bad/buggy?

I made it as a proof of concept for a high school project.

## How can I build it?

### \*nix

```
./configure
make
```

### Windows with Mingw

```
configure.bat
mingw32-make
```

## How can I test it?

```
ffmpeg -i my_image.jpg -pix_fmt rgb24 input.rgb
run_tests
cha2slide encode input.rgb out.png
cha2slide encrypt out.png out.enc.png mykey
cha2slide decrypt out.enc.png out.dec.png mykey
```
