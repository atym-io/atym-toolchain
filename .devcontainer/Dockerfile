# Copyright (C) 2026 Atym, Inc.  All rights reserved.

FROM ubuntu:24.04 AS atym-base

ARG NODE_MAJOR=20
ARG WASI_SDK_VERSION=29
ARG WASM_TOOLS_VERSION=1.244.0
ARG WAMR_VERSION=2.4.4

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates \
    curl \
    gcc \
    git \
    gpg \
    make \
    build-essential \
    cmake \
    cppcheck \
    clang \
    clang-format \
    clang-tools \
    libclang-rt-dev \
    llvm \
    net-tools \
    sudo \
    jq \
    ccache \
    python3-pip \
    python3-requests \
    ninja-build \
    && if [ "$(dpkg --print-architecture)" = "amd64" ]; then \
        # 32-bit x86 compatibility packages (not available on ARM64)
        apt-get install -y --no-install-recommends \
            g++-multilib \
            libgcc-13-dev \
            lib32gcc-13-dev; \
    fi \
    && curl -fsSL https://deb.nodesource.com/gpgkey/nodesource-repo.gpg.key | gpg --dearmor -o /etc/apt/keyrings/nodesource.gpg \
    && echo "deb [signed-by=/etc/apt/keyrings/nodesource.gpg] https://deb.nodesource.com/node_$NODE_MAJOR.x nodistro main" | tee /etc/apt/sources.list.d/nodesource.list \
    && apt-get update && apt-get install -y --no-install-recommends nodejs \
    && curl -fsSL https://cli.github.com/packages/githubcli-archive-keyring.gpg | gpg --dearmor -o /usr/share/keyrings/githubcli-archive-keyring.gpg \
    && echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/githubcli-archive-keyring.gpg] https://cli.github.com/packages stable main" | tee /etc/apt/sources.list.d/github-cli.list \
    && apt-get update \
    && apt-get install -y --no-install-recommends gh \
    && echo "deb [trusted=yes] http://apt.dist.atym.io stable main" | tee /etc/apt/sources.list.d/myrepo.list \
    && apt-get update \
    && apt-get install -y --no-install-recommends atym \
    && apt-get clean -y \
    && update-alternatives --install /usr/bin/python python /usr/bin/python3 50

# For LLVM build - allow pip to install packages in an externally managed environment 
ENV PIP_BREAK_SYSTEM_PACKAGES=1

# Install wasi-sdk
RUN mkdir /opt/wasi-sdk && \
    curl -sSL https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-$WASI_SDK_VERSION/wasi-sdk-$WASI_SDK_VERSION.0-$(uname -m | sed s/aarch64/arm64/)-linux.tar.gz | \
        tar zxvf - --strip-components=1 -C /opt/wasi-sdk

# Install wasm-tools
RUN mkdir -p /opt/wasm-tools/bin && \
    curl -sSL https://github.com/bytecodealliance/wasm-tools/releases/download/v$WASM_TOOLS_VERSION/wasm-tools-$WASM_TOOLS_VERSION-$(uname -m)-linux.tar.gz | \
        tar zxvf - -C /opt/wasm-tools/bin --strip-components=1

# Install WAMR (iwasm and wamrc)
RUN cd /tmp \
    && curl -sSL https://github.com/bytecodealliance/wasm-micro-runtime/archive/refs/tags/WAMR-$WAMR_VERSION.tar.gz | tar zxvf - \
    && cd wasm-micro-runtime-WAMR-$WAMR_VERSION/product-mini/platforms/linux \
    && mkdir build && cd build \
    && cmake .. \
    && make \
    && mkdir -p /opt/wamr/bin \
    && cp iwasm /opt/wamr/bin/ \
    && cd /tmp/wasm-micro-runtime-WAMR-$WAMR_VERSION/wamr-compiler \
    && ./build_llvm.sh \
    && mkdir build && cd build \
    && cmake .. \
    && make \
    && cp wamrc /opt/wamr/bin/ \
    && rm -rf /tmp/wasm-micro-runtime-WAMR-$WAMR_VERSION

ENV PATH="/opt/wamr/bin:/opt/wasm-tools/bin:${PATH}"

FROM atym-base AS atym-dev

ARG USERNAME=atym-dev
ARG USER_UID=1000
ARG USER_GID=$USER_UID

# Accomodate the user for devcontainer
RUN (userdel -r $USERNAME ; userdel -r `id -nu $USER_UID` ; groupdel `id -ng $USER_GID`) || true \
    && groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME -s /bin/bash \
    && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME

USER $USERNAME

WORKDIR /home/$USERNAME
