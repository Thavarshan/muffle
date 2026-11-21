#!/usr/bin/env bash
set -euo pipefail

root="$(git rev-parse --show-toplevel)"
cd "$root"

usage() {
    echo "Usage: scripts/release.sh validate VERSION [TAG]" >&2
    echo "       scripts/release.sh package-smoke BINARY" >&2
}

validate_version() {
    local version="$1"
    if [[ ! "$version" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
        echo "Invalid release version: $version" >&2
        return 1
    fi
    local project_version
    project_version="$(sed -n 's/^project(muffle VERSION \([^ ]*\).*/\1/p' \
        CMakeLists.txt)"
    if [[ "$version" != "$project_version" ]]; then
        echo "Tag version $version differs from CMake $project_version" >&2
        return 1
    fi
}

case "${1:-}" in
    validate)
        if [[ $# -lt 2 || $# -gt 3 ]]; then
            usage
            exit 2
        fi
        version="$2"
        tag="${3:-v$version}"
        validate_version "$version"
        if [[ "$tag" != "v$version" ]]; then
            echo "Expected tag v$version, got $tag" >&2
            exit 1
        fi
        if [[ -n "${GITHUB_SHA:-}" && "${GITHUB_REF_TYPE:-}" == "tag" ]]; then
            tag_sha="$(git rev-list -n 1 "$tag")"
            git fetch --no-tags origin main
            git merge-base --is-ancestor "$tag_sha" origin/main
        fi
        ;;
    package-smoke)
        if [[ $# -ne 2 ]]; then
            usage
            exit 2
        fi
        binary="$2"
        "$binary" --version
        "$binary" -- sh -c 'echo success'
        set +e
        "$binary" -- sh -c 'echo expected-failure; exit 42'
        status=$?
        set -e
        if [[ $status -ne 42 ]]; then
            echo "Expected child status 42, received $status" >&2
            exit 1
        fi
        ;;
    *)
        usage
        exit 2
        ;;
esac
