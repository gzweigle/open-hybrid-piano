# Hardware Releases

All released hardware is located in this directory.

See [../kicad_general.md](../kicad_general.md) for more information on KiCad files.

## Manufacturing

### Gerber Files

To manufacture a circuit board, send the files in a *_gerber/ folder to a PCB manufacturer. Board fabrication and test is always with the *_gerber/ folder files.

If the circuit board directory does not include a *_gerber/ folder, this means the boards have not been fully fabricated and tested. In this case, use the KiCad project. Once tested, add a *_gerber/ folder and files.

### KiCad Project Files

Boards for Gerber files were fabricated and tested.

Each directory of KiCad project files end with a "KiCad files" _v* version number. The KiCad libraries directories in this repository also end with the same "KiCad Files" _v* version number. Use the KiCad libraries directory with the same "KiCad files" _v* version number as the KiCad project files directories.

The "KiCad files" _v* version number system is a different numbering system than the KiCad program version and is different than the circuit board version. A unique "KiCad files" _v* version number for the KiCad directories allows flexible updates with anything related to KiCad changes.

### Components

Each bill of materials file ends with a number. This number allows different component variants for the same board. Piano building instructions will specify the bill of materials variant.

For any discrepancy between schematic and bill of materials, use the bill of materials. This includes part values as well as whether a through-hole component is placed on front or back of a circuit board.

## Numbering Systems Summary

* Circuit board version. Each circuit board version is in its own unique subdirectory.
* Bill of materials. The number on each bill of materials is for a board variant. Instructions for building each piano will specify the correct bill of materials variant.
* KiCad files _v* version number. Use this number to match KiCad files with the symbol and footprint library files in this repository.
* KiCad program version. If a circuit board directory includes multiple KiCad program versions, the circuit boards may have a silkscreen label indicating the KiCad program version used for fabrication.