{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-22.05";
    flake-utils.url = "github:numtide/flake-utils";

    tinycmmc.url = "github:grumbel/tinycmmc";
    tinycmmc.inputs.nixpkgs.follows = "nixpkgs";
    tinycmmc.inputs.flake-utils.follows = "flake-utils";

    logmich.url = "github:logmich/logmich";
    logmich.inputs.nixpkgs.follows = "nixpkgs";
    logmich.inputs.flake-utils.follows = "flake-utils";
    logmich.inputs.tinycmmc.follows = "tinycmmc";
  };

  outputs = { self, nixpkgs, flake-utils, tinycmmc, logmich }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        project_version_from_file = let
          version_file = pkgs.lib.fileContents ./VERSION;
          project_has_version = ((builtins.substring 0 1) version_file) == "v";
          project_version = if !project_has_version
                            then ("${nixpkgs.lib.substring 0 8 self.lastModifiedDate}-${self.shortRev or "dirty"}")
                            else (builtins.substring 1 ((builtins.stringLength version_file) - 2) version_file);
        in
          project_version;

      in rec {
        packages = flake-utils.lib.flattenTree {
          unsebu = pkgs.stdenv.mkDerivation {
            pname = "unsebu";
            version = project_version_from_file;
            src = nixpkgs.lib.cleanSource ./.;
            postPatch = ''
              echo "v${project_version_from_file}" > VERSION
            '';
            nativeBuildInputs = with pkgs; [
              cmake
              pkg-config
            ];
            buildInputs = with pkgs; [
              pkgs.at-spi2-core
              pkgs.bluez
              pkgs.dbus-glib
              pkgs.epoxy
              pkgs.fmt
              pkgs.glib
              pkgs.gobject-introspection
              pkgs.gtest
              pkgs.gtk3
              pkgs.libdatrie
              pkgs.libselinux
              pkgs.libsepol
              pkgs.libthai
              pkgs.udev
              pkgs.libusb1
              pkgs.libxkbcommon
              pkgs.pcre
              pkgs.python3
              pkgs.python3Packages.dbus-python
              pkgs.util-linux
              pkgs.xorg.libX11
              pkgs.xorg.libXdmcp
              pkgs.xorg.libXtst
            ] ++ [
              tinycmmc.defaultPackage.${system}
              logmich.defaultPackage.${system}
            ];
          };
        };
        defaultPackage = packages.unsebu;
      }
    );
}
