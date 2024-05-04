{ pkgs ? import <nixpkgs> { } }:

with pkgs;

mkShell {
buildInputs = [ gnumake gcc gdb ]; # your dependencies here
}
