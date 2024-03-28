#!/bin/bash

# Build and install the Flatpak
flatpak-builder --force-clean --user --install build-dir build-aux/com.github.chollingworth.Lampray.yml

# Run the Flatpak
flatpak run --user com.github.chollingworth.Lampray