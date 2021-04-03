# DenseTrussSearchKT
This is the code implemented for a paper *Cohesive Subgraph Search Using Keywords in Large Networks*. Originally written by others but modified by me.

## Compilation Note
This program needs [SNAP library](https://snap.stanford.edu/) to be installed on your computer as a **LIBRARY**. The compiler tries to find libSnap (compiler flag: -lSnap) in your computer. If not, `make` will exit. Also, this program assumes that Snap library is copied to the following paths:

- `/usr/local/include/snap-core`

- `/usr/local/include/glib-core`

- `/usr/local/include/snap-adv`

- `/usr/local/include/snap-exp`

If this is **NOT** where SNAP Library is located on your computer, you are free to modify the `Makefile`.