# getting-started-internal
This repository contains a collection of sample applications and containers for ATYM/OCRE. It includes both board-specific demos and generic examples to help engineers develop, test, and share reusable code.

This is an internal repository intended for engineering use, onboarding, and demo development. For polished, public-facing demos, please refer to the main [`getting-started`](https://github.com/atym-io/getting-started) repository.

## Getting Started

Clone this repository with submodules to get the [ATYM SDK](https://github.com/atym-io/atym-sdk):
```bash
git clone --recursive https://github.com/atym-io/getting-started-internal.git
```
Or if you already cloned without submodules:
```bash
git submodule update --init --recursive
```

## Using the SDK in Samples
Sample applications can include the ATYM SDK headers:
```c
#include "atym-sdk/ocre_api.h"
```

---
## Additional Information
Feel free to explore, use, and contribute new samples as needed. As the needs of the team evolve, so will the contents and organization of this repository.
