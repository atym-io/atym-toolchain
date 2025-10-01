Getting Started with Atym
==========================
This repository contains materials and sample code for building applications with the Atym Platform, including the Atym toolchain for C/C++ via a **[dev container](https://code.visualstudio.com/docs/remote/containers-tutorial)**, which can also be run in VSCode or interactively.

## Getting Started

First, clone this repository with submodules:
```bash
git clone --recursive https://github.com/atym-io/atym-toolchain.git
```
Or if you already cloned without submodules:
```bash
git submodule update --init --recursive
```

**Why submodules?** This repository includes the [Ocre SDK](https://github.com/project-ocre/ocre-sdk) as a submodule, which provides a modular C-based API for building portable embedded applications that compile to WebAssembly. This repository also contains a wide range of generic and board-specific examples that demonstrate real-world use cases. The `--recursive` flag ensures you get both this repository and the SDK headers.

### Development with Visual Studio Code
To develop with the Atym Toolchain using VS Code follow these steps:
1. Install [Docker Desktop](https://www.docker.com/products/docker-desktop/), [Visual Studio Code](https://code.visualstudio.com/), and the [Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers) Extension in VSCode.
2. Start VS Code
3. Run the `Dev Containers: Open Folder in Container...` from the Command Palette (`Cmd + Shift + P` for Mac, or `Ctrl + Shift + P` for Windows).
4. Select the folder containing the cloned repository.

The VS Code window will then reload, but since the container does not exist yet, VS Code will create one and clone the sample repository into an isolated container volume. This will take some time and progress will be displayed in the lower right corner of the VS Code window. This step only needs to be done once.

Opening a terminal will give you console access to the container environment. From there, the samples can be built. 

**Note:**  
If you run the container manually (e.g., with `docker run`), mount your workspace to access your files:
```bash
docker run -it --rm -v $(pwd):/workspaces/atym atym-dev-container:localtest /bin/bash
```
Your files will be available at `/workspaces/atym` inside the container.

## Building the Samples
Each of the sample applications has a CMake file and can be built as follows:
```
cd samples/sample_name
mkdir build && cd build
cmake ..
make
```
**Note:** This step results in the creation of a file named `sample_name.wasm`, which will be used by the Atym CLI to create an Atym container in the next step.

Next, jump up out of the *build* directory and back to the sample root.
```
cd ..
```
Then build the container using the Atym CLI (provided in the Dev Container)
```
atym build -v
```
You should now see a new `.atym` directory in your home directory (`~/.atym`) that contains the container name (as specified in `build.yml`), as well as all of the container contents.

## Multi-Architecture Support

The Atym Dev Container supports both x86_64 (amd64) and ARM64 architectures.  
Docker will automatically select the correct image for your platform.  
If you need to build for a specific architecture, use the `--platform` flag:
```bash
docker buildx build --platform linux/arm64 ...
```

## Troubleshooting

If you encounter errors during the Docker build (such as missing files or architecture mismatches):

- Ensure you have cloned the repository with submodules (`--recursive`).
- Make sure Docker Desktop is running and your user has permission to use Docker.
- If you see architecture errors (e.g., `exec format error`), check that your system matches the image architecture, or use the correct `--platform` flag.
- For missing files (e.g., `resource`), verify that all required directories exist in your workspace and are not excluded by `.dockerignore`.

## What's Next?
Now that you've built your first Atym container, it's time to deploy it to your device! 

Visit our [Quickstart Guide](https://docs.prod.atym.io/quickstart) to learn how to set up the Atym Runtime on your board, register your device with the Atym Hub, and deploy your first containerized application. 
