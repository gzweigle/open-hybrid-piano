# DIY-Grand-Digital-Piano

## Goal

Build a full-custom hybrid digital piano using a complete grand piano action. 

In the spirit of "everything worth doing is worth overdoing", the performance design option will be intentionally overdesigned and include many marginally useful features.

Additionally, a simpler single board architecture is under development.

This is a hobby and so the approach may be driven primarily by curiosity on a subject, and not by efficiently getting to completion.

## Why?

It is fun.

It is a great way to learn so many new things.

## History

I started December 5, 2020.

Please see [video_documentation.md](video_documentation.md) for historical documentation in video form.

## Related Projects:

Work was inspired, in part, by https://github.com/ekumanov/cybrid

See also:

* https://github.com/jkominek/piano-conversion/
* https://github.com/davidedelvento/Mybrid
* https://www.youtube.com/watch?v=nRBsudMpwwc

## Roadmap

Create and build every component. This includes both sampled and modelled piano sounds.

Design and build architectural variants, to address different use cases.

Helping others build and create based on this work (for example, possibly open source options or kits).

## Performance Architecture Block Diagram
The original architecture is designed for maximum performance, without constraining cost.
![alt text](block_diagram_performance.png)

## Single Board Architecture Block Diagram
A new single board architecure is designed for simplicity and lower cost.
Total system = piano action, one board with MIDI output plus 88 small sensor boards.
The result should be more suitable for others to try than the performance architecture.
![alt text](block_diagram_single_board.png)