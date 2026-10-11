# Threat model

## What this project does and where untrusted input enters

libwmf reads Windows Metafiles (WMF) and draws them through an output device. The metafile is
the untrusted input, and it usually arrives from outside: an uploaded image converted on a
server, an attachment, or a file shown in a thumbnail or an image viewer.

There are two build shapes, and they expose different code:

- libwmflite: src/api.c, src/bbuf.c, src/meta.c, src/player.c and src/recorder.c, with the
  player headers in src/player/. GraphicsMagick links only this part and supplies its own output
  device, so this is where most real exposure is.
- libwmf, the full library: adds src/font.c (FreeType and the XML fontmap through src/xml.c),
  src/stream.c, src/wmf.c, the DIB decoder in src/ipa/ipa/bmp.h, and the output devices in
  src/ipa (gd, svg, eps, fig, plot, x). The converters in src/convert and the gdk-pixbuf loader
  use it.

The fontmap XML files and the font files are installed with the system and are not attacker
controlled.

## Components that matter most and least

- Matters most: the metafile player and everything it reaches from a file: record parameter
  reads (ParU16 in src/player/record.h), the device context and object table across records,
  region and clip arithmetic (src/player/region.h and clip.h), counts and sizes that drive loops
  or allocations, text and font records, and the memory manager in src/api.c.
- Next: the DIB decoder in src/ipa/ipa/bmp.h and the image sizing in the output devices.
- Lower: src/recorder.c writes metafiles and only runs under API_ENABLE_EDITING. It never reads
  untrusted input.
- Out of scope: src/extra/gd is an old bundled copy of gd that is only built when no system gd is
  found, which does not happen on a normal system. GraphicsMagick's own WMF device is
  GraphicsMagick's code, not ours.

## How to exercise it

- /build/asan/fuzz/wmf_fuzzer: libFuzzer target built with AddressSanitizer and
  UndefinedBehaviorSanitizer. It runs the files named on its command line through wmf_scan only,
  so it does not play the file through a device.
- /build/asan/src/convert/wmf2svg, wmf2eps, wmf2fig and wmf2gd, from the same sanitizer build,
  play a whole file through a device: "wmf2svg --wmf-fontdir=/src/fonts file.wmf -o out.svg".
- /build/lite holds libwmflite alone (src/.libs/libwmflite.a), with the same sanitizers, for a
  driver that plays a file the way GraphicsMagick does.
- /src/examples holds sample metafiles. "make check" in /src runs every converter over them.

## How we rate severity

- An out of bounds write, use after free or double free from a metafile: critical if it is in
  libwmflite, high in the full library only.
- An out of bounds read, or a use of a stale pointer that does not write, from a metafile: high
  in libwmflite, medium in the full library only.
- A null dereference, division by zero, assertion or other clean crash: medium in libwmflite,
  low in the full library only.
- Slow drawing, large allocations that fail cleanly, and timeouts: low.
- Problems that need a malicious fontmap or font file installed on the system: low.

## Anything to leave alone

- Anything inside src/extra/gd.
- Bugs in GraphicsMagick's WMF device. Report those to GraphicsMagick.
- Undefined behaviour that is only a signed shift or an integer overflow, with no memory error
  following from it, is low.

## How reports and patches should look

A reproducer metafile and the sanitizer output are the most useful parts. Say which build shape
the bug is reachable from, lite or full. A patch should be the smallest change that fixes the
bug, in the style of the surrounding code, and must not change the public headers in
include/libwmf, because GraphicsMagick and other users build against them.
