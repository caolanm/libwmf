# OSS Scanner build for libwmf (https://github.com/caolanm/libwmf).
#
# Three builds of the same checkout:
#
#   /build/asan   the full library, clang, AddressSanitizer and UndefinedBehaviorSanitizer, -O1 -g. Holds
#                 the converters in src/convert (wmf2svg, wmf2eps, wmf2fig, wmf2gd), which play a whole
#                 metafile through an output device, and the libFuzzer target fuzz/wmf_fuzzer, which runs
#                 the files named on its command line.
#   /build/lite   libwmflite only (--disable-heavy), with the same sanitizers. This is the shape that
#                 GraphicsMagick links: api, bbuf, meta, player and recorder, with no output devices.
#   /src          a plain -O2 -g build with frame pointers, installed to /usr/local, with make check run.
FROM ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive

# libgd-dev makes configure use the system gd, as distributions do, so the bundled copy in src/extra/gd
# is not built.
RUN apt-get update && apt-get install -y --no-install-recommends \
      ca-certificates git python3 make autoconf automake libtool pkg-config \
      clang libclang-rt-18-dev llvm gcc gdb valgrind \
      libfreetype-dev libxml2-dev zlib1g-dev libpng-dev libjpeg-dev libgd-dev libgdk-pixbuf-2.0-dev \
    && rm -rf /var/lib/apt/lists/*

# scanner contract: the checkout lives inside the image, at /src
COPY . /src
WORKDIR /src

RUN autoreconf -fi

ENV SANITIZE="-O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined -fno-sanitize-recover=undefined"

# The build is not set up for a separate build directory, so each sanitizer build gets its own copy of
# the source.
RUN mkdir -p /build/asan /build/lite \
    && tar -C /src -cf - . | tar -C /build/asan -xf - \
    && tar -C /src -cf - . | tar -C /build/lite -xf -

RUN cd /build/asan \
    && ./configure --disable-shared --enable-static --without-expat --with-libxml2 \
         CC=clang CFLAGS="$SANITIZE -fsanitize=fuzzer-no-link" LDFLAGS="-fsanitize=address,undefined" \
    && make -j"$(nproc)" \
    && clang $SANITIZE -fsanitize=fuzzer $(pkg-config --cflags freetype2) -Iinclude -I. \
         fuzz/wmf_fuzzer.c src/.libs/libwmf.a src/.libs/libwmflite.a \
         $(sed -n 's/^WMF_DEP_LIBS = *//p' src/Makefile) -o fuzz/wmf_fuzzer

RUN cd /build/lite \
    && ./configure --disable-heavy --disable-shared --enable-static \
         CC=clang CFLAGS="$SANITIZE" LDFLAGS="-fsanitize=address,undefined" \
    && make -j"$(nproc)"

# Plain build, as distributions build it.
RUN CFLAGS="-O2 -g -fno-omit-frame-pointer" ./configure \
    && make -j"$(nproc)" && make install && ldconfig

RUN make -j"$(nproc)" check || echo "WARNING: make check reported failures"
