# Security Policy

## Supported versions

Only the latest release is maintained. Fixes go to `master`. If you are
running something older, the answer to a security report is to
upgrade.

## What is in scope

Memory safety problems reachable from a metafile: a crash, or a read or
write outside a buffer, when a WMF file is read and played. This covers
libwmflite (the metafile player, which GraphicsMagick links) and the
rest of libwmf (fonts, bitmaps and the output devices).

## What is out of scope

1. src/extra/gd, an old bundled copy of gd that is only built when no
   system gd is found.
2. Slow drawing or high memory use on a file built to be pathological.
3. Bugs in the programs that embed libwmf, including GraphicsMagick's
   own WMF code. Report those to the program.

## How to report

Use GitHub's private reporting: the Security tab of this repository,
then "Report a vulnerability". That keeps the report private until
there is a fix.

If that does not work for you, mail caolanm@gmail.com.

A report is most useful with the metafile that triggers it, the version
or commit you tested, and the sanitizer output if you have it.

## What happens next

libwmf has one maintainer, and there is no fixed timetable.

Most findings are fixed in a public commit once they are understood. A
finding serious enough to need handling differently is handled case by
case.

Please allow 90 days before disclosing publicly.

Reporters are credited unless they ask not to be.
