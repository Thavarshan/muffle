# Package maintenance

This page separates package definitions in the source tree from packages that
users can actually install. Check release and registry pages before claiming
availability.

## Homebrew tap

The maintained tap is `Thavarshan/homebrew-muffle`. Its formula builds the
immutable upstream release source with CMake. A scheduled or manually
triggered tap workflow resolves the latest non-draft upstream release,
calculates the source archive SHA-256, updates the formula, audits and styles
it, installs and tests it, then pushes the tested update. Confirm the workflow
and tap commit before announcing a new formula version.

The formula test checks version output, failure output, and exit status. Local
maintenance commands are:

```sh
brew audit --strict --formula Formula/muffle.rb
brew style Formula/muffle.rb
brew install --build-from-source Thavarshan/muffle/muffle
brew test Thavarshan/muffle/muffle
brew uninstall muffle
```

## Debian and RPM

CPack builds `.deb` and `.rpm` artifacts from the same tested CMake install
tree as the release binaries. The release matrix installs and exercises the
Debian package on Ubuntu, and tests RPM installation and removal in Fedora.
These are downloadable packages, not APT or DNF repositories. Do not provide
repository-based install instructions until signed metadata and a maintained
index are separately implemented.

## AUR

`packaging/aur/PKGBUILD` is a prepared source recipe. Before submission, replace
the source checksum with the real immutable release archive hash, generate
`.SRCINFO` using `makepkg --printsrcinfo`, run `namcap`, and build/test in a
clean Arch environment. Publish only with an authorized AUR account and SSH
key; external account access and AUR acceptance cannot be inferred from this
checkout.

## Nix

`flake.nix` exposes a source-built package and development shell. Validate with
`nix flake check` and `nix build`, then execute the resulting binary on each
claimed system. Pin and commit `flake.lock` before describing a release as
reproducibly pinned. Official nixpkgs inclusion follows the current nixpkgs
contribution and review process and is separate from this flake.

## conda-forge

`packaging/conda-forge/meta.yaml` is a draft recipe. Set its source hash from
the verified tagged archive and validate it with the current conda-forge
recipe tooling. Submit through staged-recipes only when the recipe and
maintainer metadata are complete. A submitted recipe is not installable until
it is reviewed, merged, built, and indexed.

## Update recovery

Never update a downstream recipe from a draft or failed upstream release.
Retain the previous package version until the new one passes its own package
checks. If a release must be withdrawn, stop automated downstream updates,
remove or mark affected recipes according to registry policy, and publish a
corrected version with new immutable identifiers. Do not replace published
checksums or move tags silently.
