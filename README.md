# Multitextor
Cross Platform console mode library and text editor.

This project is attempt of recreating my old text editor.

## Tested on:
 - Windows 7 - Microsoft Visual Studio Community 2017
 - Windows 10 - Microsoft Visual Studio Community 2019 / 2017
 - Linux Ubuntu 20.04 - gcc version 9.3.0

Need compiler with C++ 0x17 full support.

Minimal requirenment: gcc 8.0 or MSVS 2017

[![Build Status](https://travis-ci.org/vikonix/multitextor.svg?branch=main)][travis]
[![BSD-2 license](https://img.shields.io/github/license/vikonix/multitextor)][license]

[travis]: https://travis-ci.org/vikonix/multitextor
[license]: https://github.com/vikonix/multitextor/blob/main/LICENSE

## Need to install packages in Linux:
 - sudo apt-get install -y libncurses5-dev
 - sudo apt-get install -y libgpm-dev
 - sudo apt-get install -y gpm
 
## What is ready now:
 - Third party library (easyloggingpp, utfcpp, termdb).
 - Console input/output library.
 - Console windows library.
 
## In progress:
 - Editor components.