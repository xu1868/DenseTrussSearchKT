# DenseTrussSearchKT
This is the code implemented for a paper *Cohesive Subgraph Search Using Keywords in Large Networks*. Originally written by others but modified by me.

## Compilation Note
This program needs [SNAP library](https://snap.stanford.edu/) to be installed on your computer as a **LIBRARY**. The compiler tries to find libSnap (compiler flag: -lSnap) in your computer. If not, `make` will exit. Also, this program assumes that Snap library is copied to the following paths:

- `/usr/local/include/snap-core`

- `/usr/local/include/glib-core`

- `/usr/local/include/snap-adv`

- `/usr/local/include/snap-exp`

If this is **NOT** where SNAP Library is located on your computer, you are free to modify the `Makefile`.

## Possible Improvements
Several parameters in this piece of code are chosen **RANDOMLY**. You are invited to check what parameters work the best:

- `maxOutputHeapSize` in `BanksAlgorithm.hpp`, which is the minimum size for BANKS algorithm to stop searching for new trees. Bigger size means better results (maybe?) but lower speed.

- Deleting policy of early-stop batch based deletion in `MinDenseTruss.cpp`. Currently, my implementation starts the deletion size with 1 and increases it by 1 when the last deletion was successful. Otherwise it is reset to 1. There may be better ways to handle this.

- `earlyStopThreshold` in `MinDenseTruss.hpp`, which is the number of unsuccessful deletion it requires for the algorithm to stop deleting.

- `steinerTreeThreshold` in `MinDenseTruss.hpp`, which is the threshold of the size of vertices added to the steiner tree on once in the local exploration algorithm.

------------------------------------

If any bugs exist, report to me or you can fix them by yourself. There are extensive comments so it should be fairly easy. Enjoy!