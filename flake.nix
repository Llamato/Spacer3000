{
  description = "An OpenGL native space trading game";

  outputs = { self, nixpkgs, ... }:
    let
      systems = [
        "x86_64-linux"
        "aarch64-linux"
        "x86_64-darwin"
        "aarch64-darwin"
        "armv7l-linux"
      ];
      forAllSystems = nixpkgs.lib.genAttrs systems;
      pkgsFor = system: import nixpkgs { inherit system; };
    in {
      packages = forAllSystems (system:
        let pkgs = pkgsFor system; in {
          default = pkgs.gcc14Stdenv.mkDerivation {
            pname = "spacer3000";
            version = "0.1.0";
            src = pkgs.lib.cleanSource ./.;
            nativeBuildInputs = with pkgs; [
              cmake
              keepBuildTree
              pkg-config
            ];
            buildInputs = with pkgs; [
              glfw
              libGL
              libGLU
              mesa
              libglvnd
              xorg.libX11
              libc
            ];
            installPhase = ''
              mkdir -p $out/bin
              cp build/spacer3000 $out/bin/
            '';
            meta = with pkgs.lib; {
              description = "An OpenGL native space trading game";
              homepage = "http://www.github.com/Llamato/Spacer3000";
              mainProgram = "spacer3000";
              platforms = platforms.unix;
            };
          };
        }
      );

      devShells = forAllSystems (system:
        let pkgs = pkgsFor system; in {
          default = pkgs.mkShell {
            inputsFrom = [
              self.packages.${system}.default
            ];
            packages = with pkgs; [
              gdb
              clang-tools
              bear
              bun
            ];
            shellHook = ''bunx nodemon --watch main.c --exec "sh -c 'make && ./build/spacer3000 || true'"'';
          };
        }
      );
    };
}