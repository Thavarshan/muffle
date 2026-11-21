#!/usr/bin/env bash
set -euo pipefail

tag="${1:?usage: publish-release.sh TAG VERSION DIST_DIR}"
version="${2:?usage: publish-release.sh TAG VERSION DIST_DIR}"
dist="${3:?usage: publish-release.sh TAG VERSION DIST_DIR}"
notes="docs/release-notes/v${version}.md"

scripts/release.sh validate "$version" "$tag"
test -f "$notes"
test -f "$dist/SHA256SUMS"
(
    cd "$dist"
    sha256sum -c SHA256SUMS
)

published=false
if gh release view "$tag" >/dev/null 2>&1; then
    is_draft="$(gh release view "$tag" --json isDraft --jq .isDraft)"
    if [[ "$is_draft" != true ]]; then
        published=true
    fi
else
    gh release create "$tag" --draft --verify-tag \
        --title "Muffle $tag" --notes-file "$notes"
fi

verification="$(mktemp -d)"
trap 'rm -rf -- "$verification"' EXIT
while IFS= read -r asset; do
    name="$(basename "$asset")"
    if gh release download "$tag" --pattern "$name" \
        --dir "$verification" >/dev/null 2>&1; then
        if ! cmp -s "$asset" "$verification/$name"; then
            echo "Existing release asset differs: $name" >&2
            exit 1
        fi
    else
        if [[ "$published" == true ]]; then
            echo "Published release is missing expected asset: $name" >&2
            exit 1
        fi
        gh release upload "$tag" "$asset"
    fi
done < <(find "$dist" -maxdepth 1 -type f -print | sort)

gh release download "$tag" --pattern 'SHA256SUMS' --dir "$verification"
(
    cd "$verification"
    sha256sum -c SHA256SUMS
)
is_draft="$(gh release view "$tag" --json isDraft --jq .isDraft)"
if [[ "$is_draft" == true ]]; then
    gh release edit "$tag" --draft=false
fi
