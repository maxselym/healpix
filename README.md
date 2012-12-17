HEALPix Ubuntu packaging scripts
================================

PPA
---

<https://launchpad.net/~zonca/+archive/healpix>

How to build
------------

For testing purposes just build with:

    debuild -uc -us

and then:

    sudo dpkg -i package_name.deb

To upload to the repository make sure the `.gnupg` folder contains the same PGP key of your PPA and then:

    debuild -S

package is built and signed, then:

    dput ppa:zonca/healpix <source.changes>

the package is uploaded to Launchpad and built there.
