# KiCad General Information

KiCad information and also information about the KiCad libraries located in this directory.

## Libraries

### Versions
Library directories are versioned in case future KiCad changes force library changes.
* kicad_libraries_v0 - Version 0. Works for KiCad 7.

### Use 

Should be possible to load a KiCad project, view the schematic, view the PCB, and send files for fabrication without loading the *stem piano* specific symbol and footprint libraries.

To add *stem piano* symbol and footprint libraries, see the KiCad documentation for symbol and footprint library management.

### Contents

* Project-specific symbols file. Use with schematic editor. [kicad_libraries_v0/stem_piano_symbols.kicad_sym](kicad_libraries_v0/stem_piano_symbols.kicad_sym)
* Directory of project-specific footprints for the printed circuit board (PCB) layout: [kicad_libraries_v0/stem_piano_footprints.pretty/](kicad_libraries_v0/stem_piano_footprints.pretty/).

## Design Rule Checking

Schematic design rule check possible warnings and errors:
*	**Error**: power pin not driven. Why does this happen? Some symbols do not match the power type on pins. For example, using a header to jumper power. Fix by creating specific symbols for these cases. As long as know why the error happens for each case, it is ok.
* **Error**: no simulation model. Why does this happen? Upgrading between KiCad versions. Select ignore SPICE model issue in the Electrical Rules settings.
* **Warning**: symbol has been modified in library. Why does this happen? Upgrading between KiCad versions.

PCB design rule check possible warnings and errors:
*	**Warning**: Silkscreen clipped by board edge. This is cosmetic.
*	**Warning**: Silkscreen overlap. This is cosmetic.
*	**Warning**: Footprint does not match library. Why does this happen? Could be due to KiCad updates. To eliminate warnings, in the PCB Editor may need to select a footprint and then run the update. However, this could reorient the silkscreen text.
*	**Error**: Thermal relief connection to zone incomplete. Why does this happen? Too few ground connections make soldering more difficult because of heat transfer. In some locations tight spacings made it difficult to get extra ground connections. Is not a problem for hand solder but could cause problems for automated soldering. Check with manufacturer.
*	**Error**: Courtyards overlap. Why does this happen? Usually a close spaced header or test point. Is not a problem for hander solder but could cause problems for automated soldering. Check with manufacturer. If it is a problem, could solve be changing to a combined part. For example, instead of three 1x8 headers, use a single 3x8 header.

The most important error to watch for is unconnected items or schematic parity. This happens if the schematic and PCB do not match. In some cases, these errors are because of KiCad updates. Either fix the error or visually verify correctness of schematic vs PCB.