{
  description = "Tiger compiler flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-24.11";
    flake-utils = {
      url = "github:numtide/flake-utils";
      inputs.systems.follows = "systems";
    };
    systems.url = "github:nix-systems/x86_64-linux";
    buildenv = {
      url = "git+https://gitlab.lre.epita.fr/tiger/buildenv?ref=master&dir=nix";
      inputs = {
        nixpkgs.follows = "nixpkgs";
      };
    };
  };
  outputs =
    {
      self,
      nixpkgs,
      buildenv,
      flake-utils,
      ...
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import nixpkgs { inherit system; };
        pkgs_buildenv = buildenv.outputs.packages.${system};
      in
      {
        packages = {
          default = self.packages.${system}.tc;
          tc = pkgs.stdenv.mkDerivation {
            pname = "tc";
            version = "1.90";

            src = self;

            enableParallelBuilding = true;
            hardeningDisable = [ "all" ];

            preConfigure = ''
              patchShebangs --build ./build-aux/bin/
              patchShebangs --build ./dev/
              ./bootstrap
            '';

            nativeBuildInputs =
              with pkgs;
              [
                bison
                libxslt
                autoconf
                automake
                gnumake
                libtool
                perl
                swig4
                clang_multi
                libllvm
                multiStdenv.cc
              ]
              ++ (with pkgs_buildenv; [
                reflex
              ])
              ;

            buildInputs =
              with pkgs;
              [
                doxygen
                gdb
                graphviz
                valgrind
                libxml2
                boost
                (with pkgs.python312Packages; [
                  pyyaml
                  colorama
                  tqdm
                  lxml
                ])
              ]
              ++ (with pkgs_buildenv; [
                havm
                nolimips
              ]);
          };
        };
        formatter = pkgs.nixfmt-rfc-style;
      }
    );
}
