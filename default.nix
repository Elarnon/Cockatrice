{ lib, stdenv, cmake, protobuf, qtbase, qtmultimedia, qttools, qtwebsockets
, wrapQtAppsHook, versionSuffix ? "", withServer ? false }:

let
  pname = "cockatrice";
  version = "2.9.0";

in stdenv.mkDerivation {
  inherit pname;
  version = "${version}${versionSuffix}";

  src = builtins.path {
    path = ./.;
    name = pname;
  };

  buildInputs = [ qtbase qtmultimedia protobuf qttools qtwebsockets ];

  nativeBuildInputs = [ cmake wrapQtAppsHook ];

  cmakeFlags = [ "-DWITH_SERVER=${if withServer then "1" else "0"}" ];

  meta = {
    homepage = "https://github.com/Cockatrice/Cockatrice";
    description =
      "A cross-platform virtual tabletop for multiplayer card games";
    license = lib.licenses.gpl2Plus;
  };
}
