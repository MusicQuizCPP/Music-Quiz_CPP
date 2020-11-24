#!/usr/bin/env bash
set -e

brew update >/dev/null
brew install qt boost || true
brew update >/dev/null