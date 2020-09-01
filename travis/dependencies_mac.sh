#!/usr/bin/env bash
set -e

brew update >/dev/null
brew install nasm perl qt || true
brew update >/dev/null
