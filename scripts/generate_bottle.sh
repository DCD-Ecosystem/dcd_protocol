#!/usr/bin/env bash
set -eo pipefail

VERS=`sw_vers -productVersion | awk '/10\.14\..*/{print $0}'`
if [[ -z "$VERS" ]]; then
   VERS=`sw_vers -productVersion | awk '/10\.15.*/{print $0}'`
   if [[ -z $VERS ]]; then
      echo "Error, unsupported OS X version"
      exit -1
   fi
   MAC_VERSION="catalina"
else
   MAC_VERSION="mojave"
fi

NAME="${PROJECT}-${VERSION}.${MAC_VERSION}.bottle"

mkdir -p ${PROJECT}/${VERSION}/opt/dcd/lib/cmake

PREFIX="${PROJECT}/${VERSION}"
SPREFIX="\/usr\/local"
SUBPREFIX="opt/${PROJECT}"
SSUBPREFIX="opt\/${PROJECT}"

export PREFIX
export SPREFIX
export SUBPREFIX
export SSUBPREFIX

. ./generate_tarball.sh ${NAME}

hash=`openssl dgst -sha256 ${NAME}.tar.gz | awk 'NF>1{print $NF}'`

echo "class Dcd < Formula

   homepage \"${URL}\"
   revision 0
   url \"https://github.com/dcd/dcd/archive/v${VERSION}.tar.gz\"
   version \"${VERSION}\"

   option :universal

   depends_on \"gmp\"
   depends_on \"openssl@1.1\"
   depends_on \"libusb\"
   depends_on \"libpqxx\"
   depends_on :macos => :mojave
   depends_on :arch =>  :intel

   bottle do
      root_url \"https://github.com/dcd/dcd/releases/download/v${VERSION}\"
      sha256 \"${hash}\" => :${MAC_VERSION}
   end
   def install
      raise \"Error, only supporting binary packages at this time\"
   end
end
__END__" &> dcd.rb

rm -r ${PROJECT} || exit 1