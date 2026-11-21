{
  description = "Muffle: Less noise. More signal.";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs }:
    let
      systems = [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];
      eachSystem = nixpkgs.lib.genAttrs systems;
    in {
      packages = eachSystem (system:
        let pkgs = import nixpkgs { inherit system; };
        in {
          default = pkgs.stdenv.mkDerivation {
            pname = "muffle";
            version = "0.1.1";
            src = self;
            nativeBuildInputs = [ pkgs.cmake ];
            cmakeBuildType = "Release";
            doCheck = true;
            cmakeBuildDir = "build";
            checkPhase = ''
              ctest --test-dir build --output-on-failure
            '';
            meta = {
              description = "Suppress successful command output";
              homepage = "https://github.com/Thavarshan/muffle";
              license = pkgs.lib.licenses.mit;
              platforms = systems;
              mainProgram = "muffle";
            };
          };
        });

      devShells = eachSystem (system:
        let pkgs = import nixpkgs { inherit system; };
        in {
          default = pkgs.mkShell {
            packages = with pkgs; [ cmake ninja clang-tools doxygen python3 ];
          };
        });
    };
}
