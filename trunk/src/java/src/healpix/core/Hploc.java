/*
 * Experimental HEALPix Java code derived from the Gaia-developed Java sources
 * and the Healpix C++ library.
 *
 *  This code is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This code is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this code; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *  For more information about HEALPix, see http://healpix.jpl.nasa.gov
 */

package healpix.core;

/** Healpix-internal class for specifying locations on the sphere. */
class Hploc
  {
  public double z, phi, sth;
  public boolean have_sth;

  /** Default constructor. */
  public Hploc() {}
  public Hploc (Vec3 v)
    {
    double xl = 1./v.length();
    z = v.z*xl;
    phi = Math.atan2(v.y,v.x);
    if (Math.abs(z)>0.99)
      {
      sth = Math.sqrt(v.x*v.x+v.y*v.y)*xl;
      have_sth=true;
      }
    }
  public Hploc (Zphi zphi)
    {
    z = zphi.z;
    phi = zphi.phi;
    have_sth=false;
    }
  public Hploc (Pointing ptg)
    {
    z = Math.cos(ptg.theta);
    phi = ptg.phi;
    if (Math.abs(z)>0.99)
      {
      sth = Math.sin(ptg.theta);
      have_sth=true;
      }
    }

  public Zphi toZphi()
    { return new Zphi(z,phi); }
  public Pointing toPointing()
    {
    double st = have_sth ? sth : Math.sqrt((1.0-z)*(1.0+z));
    return new Pointing(Math.atan2(st,z),phi);
    }
  public Vec3 toVec3()
    {
    double st = have_sth ? sth : Math.sqrt((1.0-z)*(1.0+z));
    return new Vec3(st*Math.cos(phi),st*Math.sin(phi),z);
    }
  }