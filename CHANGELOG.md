# Changelog

All notable changes to this project will be documented in this file. See [conventional commits](https://www.conventionalcommits.org/) for commit guidelines.

---

## 1.0.0 - 2026-04-29

#### Features

- commands log and status merged in main - (1984b7e) - Rinat
- add log command and fix naming in vls_command - (eca23a0) - Prokhor
- added beautiful error output - (c324bd9) - Rinat
- merge branch dev-status to main - (583ad19) - Rinat
- merge dev-status to main - (0e89349) - Rinat
- beautiful out for status added - (d8c6606) - Rinat
- added status command - (cc009d9) - Rinat
- added iterator realization - (5947fdf) - Rinat
- added new iterator functions - (17e8bc1) - Rinat
- commit lezzz goooo - (ad13d5c) - Prokhor
- time module to reading and writing data - (6b78cce) - Prokhor
- implement commit (stage → objects) - (3410f9c) - Prokhor
- now add command replaces changed files - (7b27633) - Rinat
- added find file from root function - (7de5e05) - Rinat
- added path from root function - (4eb7d8a) - Rinat
- added status command - (87542ed) - Rinat
- added output for add command - (4af3cbe) - Rinat
- added iterator for double link lists - (c627ed5) - Rinat
- added find root function - (dc75b79) - Rinat
- added status command header - (468e63f) - Rinat
- added logo and colored text to the help command - (f354bac) - Rinat
- added green output - (cd23f93) - Rinat
- added help command, added easy descriptions - (be910d5) - Rinat
- added no new line output for logger - (dc5901e) - Rinat
- add stb_ds.h for hash table - (2934a4b) - Prokhor
- merge add-dev to main - (4e5c6ac) - Rinat
- added adding stage in add command - (aa3c68b) - Rinat
- added remove stage_t for stager - (a191074) - Rinat
- added new typer for stager - (3820bc8) - Rinat
- added add stage function, changed stager header - (633a66a) - Rinat
- edited some macro related to const, wrote convenient fs utils - (cc509ec) - Prokhor
- added stager header, hash_from_string function to hasher - (36be1b7) - Rinat
- added add realization - (09749f9) - Rinat
- added stdout logger function - (2680bca) - Rinat
- added convenient error handling and refactor all old code - (a764deb) - Prokhor
- added stage manager header - (eea982f) - Rinat
- dir walker was declarated - (ac26da6) - Prokhor
- file status and commit_t struct added - (d66748d) - Prokhor
- added object_t struct - (35254b9) - Rinat
- added hasher realization - (ff26d4f) - Rinat
- added hash header - (a04adbe) - Rinat
- added add template, added nextline symbol for output - (23975e6) - Rinat
- routing in main.c with X-macros - (858619e) - Prokhor
- added new param to command_data struct - (b0f9f8e) - Rinat
- added function for add command - (3478227) - Rinat
- added blanks for other commands - (999b166) - Rinat
- added init command - (b09c6fc) - Rinat
- added commands args - (04b4942) - Rinat
- added errno handler for writer - (a1a9b6a) - Rinat
- added output writer - (e033044) - Rinat
- added data writer header - (29fe2e0) - Rinat
- added command header - (5a32a19) - Rinat
- added hello file - (96433c6) - Rinat

#### Bug Fixes

- (**CMakeLists**) Modify CMakeLists.txt for CMake 3.21 and C standard - (520a454) - Oriptal
- output branch free fixed - (c0a52ea) - Rinat
- output for status fixed - (94b92f7) - Rinat
- remove unused in stager - (d30d0cc) - Rinat
- now staged files in objects - (bb8cf6c) - Rinat
- enum value is interpreted as a mask - (95b67f3) - Prokhor
- fix join path second time :D - (b6be42b) - Prokhor
- add command now creates new copies in .vls - (16a6a60) - Rinat
- error with paths add command fixed - (710915f) - Rinat
- fix join_path function with alias buffer - (d885bd4) - Prokhor
- error with changing status add fixed - (a5fa080) - Rinat
- output for add fixed - (bce0c8c) - Rinat
- stager add_stage fixed - (0d8d881) - Rinat
- init command now create stage.json - (db4783d) - Rinat
- now ghost looks to the right sise - (fce9da8) - Rinat
- building fixed, little refactor - (424e70e) - Rinat
- vargs in vls_ensure_file in fs header - (f43f236) - Prokhor
- removed goto in stagerr, add fixed - (60fc728) - Rinat
- add_executable in cmakelists was changed - (9350f23) - Prokhor
- fix old macro with types - (6917e0f) - Prokhor
- delete old macro with types - (fc72a73) - Prokhor
- add command fixed - (2cb8e33) - Rinat
- merge commit '431089d' - (6df4cac) - Prokhor
- added mocks - (575d967) - Prokhor
- command_t declaration fixed in .h file - (6f10316) - Prokhor
- edit naming of command data - (f7a729c) - Prokhor
- error output fixed in init - (baaaf5d) - Rinat
- added existed head check - (5bab4bd) - Rinat
- removed extra check - (9aca2dd) - Rinat
- writer name fixed - (b7141ad) - Rinat
- error output for writer fixed - (4132831) - Rinat

#### Documentation

- added cjson to cmake - (3e593f1) - Rinat
- typos in readme fixed - (384becd) - Rinat
- added readme - (431089d) - Rinat

#### Tests

- add commit integration tests - (0a93dd0) - Prokhor
- add stage → objects test - (c470129) - Prokhor
- add some unit tests and 1 general integration test - (f01d52e) - Prokhor

#### Build system

- add time module to build - (6af5fe8) - Prokhor
- edit CMakeLists to support gcc23 - (4d2ad49) - Prokhor
- change CMakeLists file - (b527f20) - Prokhor
- added stager to cmake - (165c979) - Rinat
- C version changed - (aa75626) - Rinat
- added build system - (bffc347) - Rinat

#### Refactoring

- delete useless next pointer from commit_t - (27410b9) - Prokhor
- move output struct to vls_types header - (14a5477) - Prokhor
- drop stage->objects copy, snapshot to commits/<hash>/ - (a099e0b) - Prokhor
- split commit pipeline + fix status mask & stage write-back. - (1113665) - Prokhor
- init, help was changed - (7f8b74a) - Rinat
- gloval variables moved to header - (dad0610) - Rinat
- merged main into dev-add - (7ed46a4) - Rinat
- added define for stage - (50f56d8) - Rinat
- decompose types, paths and commands to separate files - (ce3b595) - Prokhor
- added file with global initialized pathes - (ba690db) - Prokhor
- naming changed - (716cff1) - Rinat
- super little code refactor - (423820b) - Rinat
- merge into main - (d88b250) - Rinat
- little changes - (06a055f) - Rinat
- merge to main - (eeb0095) - Rinat
- init command fixed - (8412c46) - Rinat
- edited signature of command type - (d6e4c1f) - Prokhor

#### Miscellaneous Chores

- (**version**) 1.0.0 - (dddf3e2) - Prokhor
- cog.toml to ignore merge commits - (fbc96b2) - Prokhor
- delete useless stb_ds.h - (9dba265) - Prokhor
- delete test files - (5a184b8) - Prokhor
- added build to .gitignore - (d92e52e) - Rinat
- add some internal files to .gitignote - (11e0774) - Prokhor
- added cache in gitignore - (9193d5d) - Rinat
- added gitignore - (ccd9e5a) - Rinat

#### Style

- delete useless comments - (04ab172) - Prokhor
- removed extra blanks - (afaa620) - Rinat
- edited naming of shadow varibles - (c2b9b43) - Prokhor
- unused variables now deleted - (024dd12) - Rinat
- typo in writer corrected - (fc97723) - Rinat

---

Changelog generated by [cocogitto](https://github.com/cocogitto/cocogitto).
