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

To be able to compile the DEC112 pjchat, make sure that you've installed or downloaded the following:
* libxml-2.0, yaml-0.1, libpjproject

Additionally, pjchat requires a YAML configuration file (`./config/config.yml`) that includes attributes required to connect to DEC112 services. An example is given below.

_config.yml_

```c  
--- # pjchat config
domain: "root.dects.dec112.eu"
user: "21223aeeb829ed8e611ffa2c8e95eb16"
passwd: "52b524e67250a7183a720bd5"
proxy: "sip:root.dects.dec112.eu;transport=tcp"
device: "39fa95fe-f0cc-a2b4-7c8c-91d876496342"
lon: "16.363449"
lat: "48.210033"
rad: "10"
ref: "http://root.dects.dec112.eu/api/v2/"
did: "did:ion:GTmRFQ2PhHIEgZzeUtA_u4j7cWiKDNaC3sBOwrldxqXoy5"
rid: "39fa95fe-f0cc-a2b4-7c8c-91d876496342"
api: "api-key"
eval: "(1) This is the (Echo Bot Service) of (DECTS (TEST))."
debug: "3"

```
As an option, messages can be stored in a text file (`./config/msg.txt`) which will be sent sequentially by pjchat. Each line requires at least 2 characters and lines are separated by CRLF. A `*` at the line end marks the message whose response should be validated - refer to `eval` in the configuration file. See an example below.

_msg.txt_
```
Message one
Message two (response to be evaluated) *
```

## Compiling and running pjchat

1. Have a look at [Clone or download the repository](https://help.github.com/en/articles/cloning-a-repository)
2. `cd src/`
3. `make` and `cp pjchat ../bin` (or `make install`)
4. `cd ../bin` and `./pjchat -r 'sip:555@root.dects.dec112.eu;transport=tcp' -a -n 5 -i 1 -f ../config/msg`<br/>

Usage:
```
pjchat -r <sip-uri> [-u <service-urn>] [-f <yaml-cfg>] [-t <msg file>] [-n <number> -i <intervall>] [-a] [-s] [-x]

-r sip-uri (request line and from header)
-u service urn (request line)
-n number of message requests
-i intervall time in seconds between message requests
-a generate automatic messages (considering number/interval)
-s use TLS
-t read messages from text file
-x include DEC112 specific test header
```

## Docker

__Guide to build a pjchat docker image.__

An image is simply created using the Dockerfile example [Dockerfile](https://github.com/dec112/pjchat/blob/master/docker/Dockerfile) with the following commands.

```
cd pjchat/src
cp ../docker/Dockerfile .
docker build --tag pjchat:1.0 .
```
