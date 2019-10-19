with import <nixpkgs> {};
stdenv.mkDerivation rec {
  name = "game01-allegro-env";
  env = buildEnv { name = name; paths = buildInputs; };
  buildInputs = [
    allegro5 
    glfw
    libGL
    json_c
  ];
}
