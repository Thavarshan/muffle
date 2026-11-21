# Releasing

Muffle uses Semantic Versioning while pre-1.0; minor versions may change
behavior. The CMake project version is the source of the executable version.
Check the [release page](https://github.com/Thavarshan/muffle/releases) for
the actual published state. A dry run is mandatory before creating the first
tag for a version.

Before a release, maintainers should:

1. Review user-visible changes and update `CHANGELOG.md` and release notes.
2. Change the version in `project(muffle VERSION X.Y.Z ...)` and keep package
   metadata synchronized.
3. Run `bash scripts/check.sh`, CTest, and package validation. Inspect every
   required job on the intended `main` commit.
4. Run a non-publishing matrix dry run:

   ```sh
   gh workflow run release.yml -R Thavarshan/muffle \
     --ref main -f version=X.Y.Z
   gh run list -R Thavarshan/muffle --workflow release.yml --limit 5
   ```

   Open the dispatch run and wait for every platform and package smoke test.
5. Verify the clean `main` commit, ensure `vX.Y.Z` does not exist locally or
   remotely, then create and push an annotated tag:

   ```sh
   git tag -a vX.Y.Z -m "Muffle vX.Y.Z"
   git push origin vX.Y.Z
   ```

6. The tag workflow rebuilds and tests archives/packages, hashes the release
   files, generates provenance attestations, verifies or creates a draft,
   then publishes it only after all assets are present and verified.
7. Verify the release assets and checksums. Update the Homebrew tap through
   its maintainer-triggered workflow, then confirm formula audit, installation,
   functional tests, and uninstall before calling Homebrew available.
8. Community registry recipes are prepared separately. Submit only after
   verifying each registry's current rules and required identity. An
   unreviewed submission is not an available package.

## Retry and recovery

The release workflow refuses a mismatched existing release asset. A failed
tag run can be rerun when it is a transient failure and the tag still points
to the verified `main` commit. A published release is immutable from the
workflow's perspective: it verifies that every expected file matches and
does not overwrite it. Fix a content defect in a new version rather than
moving a tag or replacing published files. If an artifact is compromised,
withdraw the release and affected package metadata, communicate through the
repository advisories and release notes, and publish a corrected version.

Inspect a failed run with:

```sh
gh run view RUN_ID -R Thavarshan/muffle --log-failed
```

Downstream package publication must follow a verified upstream release. The
Homebrew workflow can be dispatched to retry formula updating; it resolves
the latest non-draft release and computes the source checksum itself.

## Current distribution status

The source contains a Homebrew tap workflow, AUR recipe, Nix flake, and
conda-forge recipe. Presence in Git is not publication. Check the upstream
release page, tap repository, and registry indexes before giving users an
installation command. The AUR and conda-forge recipes require real source
hashes and external account/review steps. Nix flake commands require local
Nix validation; official nixpkgs inclusion requires its own reviewed change.
