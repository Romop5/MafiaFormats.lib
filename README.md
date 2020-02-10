# MafiaFormats.lib
## About 
MafiaFormats is a C++ library, containing many of Mafia:The City of Lost Heaven's file format parsers.
The parsers were mostly implemented by Zaklaus & DavoSK during [OpenMF project](https://github.com/MafiaHub/OpenMF-Archived).

## Documentation for parsers & formats
A vast documentation of file formats is written in *Russian* language and can be found at [Lost Heaven Modding](https://lhm.fandom.com/ru/wiki/%D0%9A%D0%B0%D1%82%D0%B5%D0%B3%D0%BE%D1%80%D0%B8%D1%8F:%D0%A4%D0%BE%D1%80%D0%BC%D0%B0%D1%82%D1%8B_%D1%84%D0%B0%D0%B9%D0%BB%D0%BE%D0%B2).

Additionaly, documentation for formats covering 3D visuals and navigation mesh can also be found at [DjBozkosz's website](http://www.djbozkosz.wz.cz/index.php?id=4) in Czech or English.

## Usage
Include this repository in your *CMake* building system, link against **MFormat::Formats** and
include ```<mformats/formats.hpp>``` in your project.

## License 
See `LICENSE` file.
