FROM ubuntu:18.04 as build

RUN apt-get update -qq && \
    apt-get install -y --no-install-recommends \
            build-essential \
            pkg-config \
            ca-certificates \
            curl \
            libgsm1-dev \
            libspeex-dev \
            libspeexdsp-dev \
            libsrtp0-dev \
            libssl-dev \
            libasound2-dev \
            libxml2-dev \
            libyaml-dev \
            && \
    apt-get purge -y --auto-remove && rm -rf /var/lib/apt/lists/*

COPY config_site.h /tmp/

WORKDIR /app

ENV PJSIP_VERSION=2.9
RUN cd ./ && \
    mkdir pjsip && \
    cd pjsip && \
    curl -vsL http://www.pjsip.org/release/${PJSIP_VERSION}/pjproject-${PJSIP_VERSION}.tar.bz2 | \
         tar --strip-components 1 -xj && \
    mv /tmp/config_site.h pjlib/include/pj/ && \
    CFLAGS="-O2 -DNDEBUG" \
    ./configure --enable-shared \
                --disable-resample \
                --prefix=/usr \
                && \
    make all install && \
    /sbin/ldconfig && \
    cd .. && \
    mkdir applib && \
    cd applib && \
    cp -fa /usr/lib/*.so* . && \
    cd .. && \
    ls -lat ./applib && \
    mkdir pjchat

COPY Makefile pjchat.c /app/pjchat/
    
RUN cd /app/pjchat && \
    make release

FROM ubuntu:18.04

RUN apt-get update -qq && \
    apt-get install -y --no-install-recommends \
            libssl1.1 \
            libasound2 \
            ca-certificates \
            libxml2 \
            libyaml-0-2 \
            && \
    apt-get purge -y --auto-remove && rm -rf /var/lib/apt/lists/*

COPY --from=build /app/pjchat/pjchat /bin/pjchat
COPY --from=build /app/applib/*.so* /usr/lib/
