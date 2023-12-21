{
  inputs = { nixpkgs.url = "github:NixOs/nixpkgs/release-23.11"; };
  outputs = { self, nixpkgs, }:
    let
      linuxSystems = [ "x86_64-linux" "aarch64-linux" ];
      darwinSystems = [ "x86_64-darwin" "aarch64-darwin" ];
      allSystems = linuxSystems ++ darwinSystems;

      forEachSystem = systems: f:
        nixpkgs.lib.genAttrs systems
        (system: f system nixpkgs.legacyPackages.${system});

      forLinuxSystems = forEachSystem linuxSystems;
      forDarwinSystems = forEachSystem darwinSystems;
      forAllSystems = forEachSystem allSystems;
    in {
      devShells = forAllSystems (system: pkgs:
        with pkgs; {
          default = mkShell {
            buildInputs = [
              protobuf
              zlib
              qt6.full
              # For format.sh
              clang-tools
              bash
            ];

            nativeBuildInputs = [ cmake ninja ];
          };
        });

      packages = forAllSystems (system: pkgs: rec {
        default = cockatrice;
        cockatrice = pkgs.qt6Packages.callPackage ./default.nix {
          withServer = true;
          versionSuffix = "+${
              builtins.substring 0 8
              (self.lastModifiedDate or self.lastModified or "19700101")
            }_${self.shortRev or "dirty"}";
        };
      });

      apps = forAllSystems (system: pkgs: {
        cockatrice = {
          type = "app";
          program = "${self.packages.${system}.cockatrice}/bin/cockatrice";
        };
        servatrice = {
          type = "app";
          program = "${self.packages.${system}.cockatrice}/bin/servatrice";
        };
        oracle = {
          type = "app";
          program = "${self.packages.${system}.cockatrice}/bin/oracle";
        };
      });
    };
}
