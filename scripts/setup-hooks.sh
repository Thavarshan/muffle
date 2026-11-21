#!/usr/bin/env bash
set -euo pipefail

git config --local core.hooksPath .githooks
printf '%s\n' 'Muffle Git hooks enabled for this clone.'
