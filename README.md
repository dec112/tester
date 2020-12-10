# DEC112/DEC112 2.0 pjchat test client

__Guide to build the pjchat test client from sources.__

For more about the DEC112 Project visit: [dec112.at](https://dec112.at)

```
Main Author: Wolfgang Kampichler

Support: <info@dec112.at>

Licence: GPLv3
```

## Overview

This is a step by step tutorial about how to build and install the DEC112 pjchat test client using the sources downloaded from the repository. 

## Prerequisites

To be able to compile the DEC112 PRF qngin, make sure that you've installed or dowdowloaded the following:
* libxml-2.0, yaml-0.1, libpjproject

Additionally, pjchat requires a YAML configuration file (`./config/config.yml`) that lists all DEC112 Border services to which qngin subscribes HEALTH information. An example is given below.

```c  
tbd ...
```
## Compiling and running the PRF qngin service

1. Have a look at [Clone or download the repository](https://help.github.com/en/articles/cloning-a-repository)
2. `cd src/`
3. `make` and `cp qngin ../bin`
4. `cd ../bin` and `./qngin -v -c ../config/config.yml -d ../../data/prf.sqlite`<br/>(usage: `qngin -v -c <config> -d <database>`)
5. `-v` sets qngin to verbose mode (optional)
6. Note: log4crc may require changes (refer to the example below):



## Docker

__Guide to build a PRF qngin service docker image.__

An image is simply created using the Dockerfile example [Dockerfile](https://github.com/dec112/pjchat/blob/master/docker/Dockerfile) with the following commands.

```
cd qngin/src
cp ../docker/Dockerfile .
docker build --tag qngin:1.0 .
```

