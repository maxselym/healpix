/*
 *  This file is part of Healpix_cxx.
 *
 *  Healpix_cxx is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Healpix_cxx is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Healpix_cxx; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *  For more information about HEALPix, see http://healpix.jpl.nasa.gov
 */

/*
 *  Healpix_cxx is being developed at the Max-Planck-Institut fuer Astrophysik
 *  and financially supported by the Deutsches Zentrum fuer Luft- und Raumfahrt
 *  (DLR).
 */

/*
 *  Copyright (C) 2003-2011 Max-Planck-Society
 *  Author: Martin Reinecke
 */

#include "healpix_base2.h"
#include "geom_utils.h"
#include "lsconstants.h"

using namespace std;

int Healpix_Base2::nside2order (int64 nside)
  {
  planck_assert (nside>0, "invalid value for Nside");
  if ((nside)&(nside-1)) return -1;
  return ilog2(nside);
  }
int64 Healpix_Base2::npix2nside (int64 npix)
  {
  int64 res=isqrt(npix/12);
  planck_assert (npix==res*res*12, "npix2nside: invalid argument");
  return res;
  }

int64 Healpix_Base2::ring_above (double z) const
  {
  double az=abs(z);
  if (az<=twothird) // equatorial region
    return int64(nside_*(2-1.5*z));
  int64 iring = int64(nside_*sqrt(3*(1-az)));
  return (z>0) ? iring : 4*nside_-iring-1;
  }

int64 Healpix_Base2::spread_bits (int v) const
  {
  return (int64(utab[ v     &0xff]))
       | (int64(utab[(v>> 8)&0xff])<<16)
       | (int64(utab[(v>>16)&0xff])<<32)
       | (int64(utab[(v>>24)&0xff])<<48);
  }

int Healpix_Base2::compress_bits (int64 v) const
  {
  int32 raw = ((v&0x555500000000ull)>>16)
             | ((v&0x5555000000000000ull)>>31)
             | (v&0x5555)
             | ((v&0x55550000)>>15);
  return ctab[raw&0xff]
      | (ctab[(raw>>8)&0xff]<<4)
      | (ctab[(raw>>16)&0xff]<<16)
      | (ctab[(raw>>24)&0xff]<<20);
  }

void Healpix_Base2::nest2xyf (int64 pix, int &ix, int &iy, int &face_num)
  const
  {
  face_num = int(pix>>(2*order_));
  pix &= (npface_-1);
  ix = compress_bits(pix);
  iy = compress_bits(pix>>1);
  }

int64 Healpix_Base2::xyf2nest (int ix, int iy, int face_num) const
  {
  return (int64(face_num)<<(2*order_)) +
    spread_bits(ix) + (spread_bits(iy)<<1);
  }

void Healpix_Base2::ring2xyf (int64 pix, int &ix, int &iy, int &face_num)
  const
  {
  int64 iring, iphi, kshift, nr;

  int64 nl2 = 2*nside_;

  if (pix<ncap_) // North Polar cap
    {
    iring = (1+isqrt(1+2*pix))>>1; //counted from North pole
    iphi  = (pix+1) - 2*iring*(iring-1);
    kshift = 0;
    nr = iring;
    face_num=(iphi-1)/nr;
    }
  else if (pix<(npix_-ncap_)) // Equatorial region
    {
    int64 ip = pix - ncap_;
    int64 tmp = (order_>=0) ? ip>>(order_+2) : ip/(4*nside_);
    iring = tmp+nside_;
    iphi = ip-tmp*4*nside_ + 1;
    kshift = (iring+nside_)&1;
    nr = nside_;
    uint64 ire = iring-nside_+1,
           irm = nl2+2-ire;
    int64 ifm = iphi - ire/2 + nside_ -1,
          ifp = iphi - irm/2 + nside_ -1;
    if (order_>=0)
      { ifm >>= order_; ifp >>= order_; }
    else
      { ifm /= nside_; ifp /= nside_; }
    if (ifp == ifm) // faces 4 to 7
      face_num = (ifp&3)+4;
    else if (ifp<ifm) // (half-)faces 0 to 3
      face_num = ifp;
    else // (half-)faces 8 to 11
      face_num = ifm + 8;
    }
  else // South Polar cap
    {
    int64 ip = npix_ - pix;
    iring = (1+isqrt(2*ip-1))>>1; //counted from South pole
    iphi  = 4*iring + 1 - (ip - 2*iring*(iring-1));
    kshift = 0;
    nr = iring;
    iring = 2*nl2-iring;
    face_num = 8 + (iphi-1)/nr;
    }

  int64 irt = iring - (jrll[face_num]*nside_) + 1;
  int64 ipt = 2*iphi- jpll[face_num]*nr - kshift -1;
  if (ipt>=nl2) ipt-=8*nside_;

  ix =  (ipt-irt) >>1;
  iy = (-ipt-irt) >>1;
  }

int64 Healpix_Base2::xyf2ring (int ix, int iy, int face_num) const
  {
  int64 nl4 = 4*nside_;
  int64 jr = (jrll[face_num]*nside_) - ix - iy  - 1;

  int64 nr, kshift, n_before;
  if (jr<nside_)
    {
    nr = jr;
    n_before = 2*nr*(nr-1);
    kshift = 0;
    }
  else if (jr > 3*nside_)
    {
    nr = nl4-jr;
    n_before = npix_ - 2*(nr+1)*nr;
    kshift = 0;
    }
  else
    {
    nr = nside_;
    n_before = ncap_ + (jr-nside_)*nl4;
    kshift = (jr-nside_)&1;
    }

  int64 jp = (jpll[face_num]*nr + ix - iy + 1 + kshift) / 2;
  if (jp>nl4)
    jp-=nl4;
  else
    if (jp<1) jp+=nl4;

  return n_before + jp - 1;
  }

Healpix_Base2::Healpix_Base2 ()
  : order_(-1), nside_(0), npface_(0), ncap_(0), npix_(0),
    fact1_(0), fact2_(0), scheme_(RING) {}

void Healpix_Base2::Set (int order, Healpix_Ordering_Scheme scheme)
  {
  planck_assert ((order>=0)&&(order<=order_max), "bad order");
  order_  = order;
  nside_  = int64(1)<<order;
  npface_ = nside_<<order_;
  ncap_   = (npface_-nside_)<<1;
  npix_   = 12*npface_;
  fact2_  = 4./npix_;
  fact1_  = (nside_<<1)*fact2_;
  scheme_ = scheme;
  }
void Healpix_Base2::SetNside (int64 nside, Healpix_Ordering_Scheme scheme)
  {
  order_  = nside2order(nside);
  planck_assert ((scheme!=NEST) || (order_>=0),
    "SetNside: nside must be power of 2 for nested maps");
  nside_  = nside;
  npface_ = nside_*nside_;
  ncap_   = (npface_-nside_)<<1;
  npix_   = 12*npface_;
  fact2_  = 4./npix_;
  fact1_  = (nside_<<1)*fact2_;
  scheme_ = scheme;
  }

double Healpix_Base2::ring2z (int64 ring) const
  {
  if (ring<nside_)
    return 1 - ring*ring*fact2_;
  if (ring <=3*nside_)
    return (2*nside_-ring)*fact1_;
  ring=4*nside_ - ring;
  return ring*ring*fact2_ - 1;
  }

int64 Healpix_Base2::pix2ring (int64 pix) const
  {
  if (scheme_==RING)
    {
    if (pix<ncap_) // North Polar cap
      return (1+isqrt(1+2*pix))>>1; //counted from North pole
    else if (pix<(npix_-ncap_)) // Equatorial region
      return (pix-ncap_)/(4*nside_) + nside_; // counted from North pole
    else // South Polar cap
      return 4*nside_-((1+isqrt(2*(npix_-pix)-1))>>1); //counted from South pole
    }
  else
    {
    int face_num, ix, iy;
    nest2xyf(pix,ix,iy,face_num);
    return (int64(jrll[face_num])<<order_) - ix - iy - 1;
    }
  }

int64 Healpix_Base2::nest2ring (int64 pix) const
  {
  planck_assert(order_>=0, "nest2ring: need hierarchical map");
  int ix, iy, face_num;
  nest2xyf (pix, ix, iy, face_num);
  return xyf2ring (ix, iy, face_num);
  }

int64 Healpix_Base2::ring2nest (int64 pix) const
  {
  planck_assert(order_>=0, "ring2nest: need hierarchical map");
  int ix, iy, face_num;
  ring2xyf (pix, ix, iy, face_num);
  return xyf2nest (ix, iy, face_num);
  }

int64 Healpix_Base2::nest_peano_helper (int64 pix, int dir) const
  {
  int face = pix>>(2*order_);
  uint8 path = peano_face2path[dir][face];
  int64 result=0;

  for (int shift=2*order_-2; shift>=0; shift-=2)
    {
    uint8 spix = uint8((pix>>shift) & 0x3);
    result <<= 2;
    result |= peano_subpix[dir][path][spix];
    path=peano_subpath[dir][path][spix];
    }

  return result + (int64(peano_face2face[dir][face])<<(2*order_));
  }

int64 Healpix_Base2::nest2peano (int64 pix) const
  { return nest_peano_helper (pix,0); }

int64 Healpix_Base2::peano2nest (int64 pix) const
  { return nest_peano_helper (pix,1); }

int64 Healpix_Base2::zphi2pix (double z, double phi) const
  {
  double za = abs(z);
  double tt = fmodulo(phi*inv_halfpi,4.0); // in [0,4)

  if (scheme_==RING)
    {
    if (za<=twothird) // Equatorial region
      {
      uint64 nl4 = 4*nside_;
      double temp1 = nside_*(0.5+tt);
      double temp2 = nside_*z*0.75;
      int64 jp = int64(temp1-temp2); // index of  ascending edge line
      int64 jm = int64(temp1+temp2); // index of descending edge line

      // ring number counted from z=2/3
      int64 ir = nside_ + 1 + jp - jm; // in {1,2n+1}
      int kshift = 1-(ir&1); // kshift=1 if ir even, 0 otherwise

      uint64 t1 = jp+jm-nside_+kshift+1+nl4+nl4;
      int64 ip = (order_>0) ?
        (t1>>1)&(nl4-1) : ((t1>>1)%nl4); // in {0,4n-1}

      return ncap_ + (ir-1)*nl4 + ip;
      }
    else  // North & South polar caps
      {
      double tp = tt-int(tt);
      double tmp = nside_*sqrt(3*(1-za));

      int64 jp = int64(tp*tmp); // increasing edge line index
      int64 jm = int64((1.0-tp)*tmp); // decreasing edge line index

      int64 ir = jp+jm+1; // ring number counted from the closest pole
      int64 ip = int64(tt*ir); // in {0,4*ir-1}
      ip %= 4*ir;

      return (z>0)  ?  2*ir*(ir-1) + ip  :  npix_ - 2*ir*(ir+1) + ip;
      }
    }
  else // scheme_ == NEST
    {
    if (za<=twothird) // Equatorial region
      {
      int face_num, ix, iy;
      double temp1 = nside_*(0.5+tt);
      double temp2 = nside_*(z*0.75);
      int64 jp = int64(temp1-temp2); // index of  ascending edge line
      int64 jm = int64(temp1+temp2); // index of descending edge line
      int64 ifp = jp >> order_;  // in {0,4}
      int64 ifm = jm >> order_;
      if (ifp == ifm)           // faces 4 to 7
        face_num = (ifp&3)+4;
      else if (ifp < ifm)       // (half-)faces 0 to 3
        face_num = ifp;
      else                      // (half-)faces 8 to 11
        face_num = ifm + 8;

      ix = jm & (nside_-1);
      iy = nside_ - (jp & (nside_-1)) - 1;
      return xyf2nest(ix,iy,face_num);
      }
    else // polar region, za > 2/3
      {
      int ntt = min(3,int(tt));
      double tp = tt-ntt;
      double tmp = nside_*sqrt(3*(1-za));

      int64 jp = int64(tp*tmp); // increasing edge line index
      int64 jm = int64((1.0-tp)*tmp); // decreasing edge line index
      if (jp>=nside_) jp = nside_-1; // for points too close to the boundary
      if (jm>=nside_) jm = nside_-1;
      return (z>=0) ?
        xyf2nest(nside_-jm -1,nside_-jp-1,ntt) :
        xyf2nest(jp,jm,ntt+8);
      }
    }
  }

void Healpix_Base2::pix2zphi (int64 pix, double &z, double &phi) const
  {
  if (scheme_==RING)
    {
    if (pix<ncap_) // North Polar cap
      {
      int64 iring = (1+isqrt(1+2*pix))>>1; //counted from North pole
      int64 iphi  = (pix+1) - 2*iring*(iring-1);

      z = 1.0 - (iring*iring)*fact2_;
      phi = (iphi-0.5) * halfpi/iring;
      }
    else if (pix<(npix_-ncap_)) // Equatorial region
      {
      int64 nl4 = 4*nside_;
      int64 ip  = pix - ncap_;
      int64 tmp = (order_>=0) ? ip>>(order_+2) : ip/nl4;
      int64 iring = tmp + nside_,
            iphi = ip-nl4*tmp+1;;
      // 1 if iring+nside is odd, 1/2 otherwise
      double fodd = ((iring+nside_)&1) ? 1 : 0.5;

      z = (2*nside_-iring)*fact1_;
      phi = (iphi-fodd) * pi*0.75*fact1_;
      }
    else // South Polar cap
      {
      int64 ip = npix_ - pix;
      int64 iring = (1+isqrt(2*ip-1))>>1; //counted from South pole
      int64 iphi  = 4*iring + 1 - (ip - 2*iring*(iring-1));

      z = -1.0 + (iring*iring)*fact2_;
      phi = (iphi-0.5) * halfpi/iring;
      }
    }
  else
    {
    int face_num, ix, iy;
    nest2xyf(pix,ix,iy,face_num);

    int64 jr = (int64(jrll[face_num])<<order_) - ix - iy - 1;

    int64 nr;
    if (jr<nside_)
      {
      nr = jr;
      z = 1 - nr*nr*fact2_;
      }
    else if (jr > 3*nside_)
      {
      nr = nside_*4-jr;
      z = nr*nr*fact2_ - 1;
      }
    else
      {
      nr = nside_;
      z = (2*nside_-jr)*fact1_;
      }

    int64 tmp=jpll[face_num]*nr+ix-iy;
    if (tmp<0) tmp+=8*nr;
    else if (tmp>=8*nr) tmp -=8*nr;
    phi = (nr==nside_) ? 0.75*halfpi*tmp*fact1_ :
                         (0.5*halfpi*tmp)/nr;
    }
  }

void Healpix_Base2::get_ring_info (int64 ring, int64 &startpix, int64 &ringpix,
  double &costheta, double &sintheta, bool &shifted) const
  {
  int64 northring = (ring>2*nside_) ? 4*nside_-ring : ring;
  if (northring < nside_)
    {
    double tmp = northring*northring*fact2_;
    costheta = 1 - tmp;
    sintheta = sqrt(tmp*(2-tmp));
    ringpix = 4*northring;
    shifted = true;
    startpix = 2*northring*(northring-1);
    }
  else
    {
    costheta = (2*nside_-northring)*fact1_;
    sintheta = sqrt((1+costheta)*(1-costheta));
    ringpix = 4*nside_;
    shifted = ((northring-nside_) & 1) == 0;
    startpix = ncap_ + (northring-nside_)*ringpix;
    }
  if (northring != ring) // southern hemisphere
    {
    costheta = -costheta;
    startpix = npix_ - startpix - ringpix;
    }
  }
void Healpix_Base2::get_ring_info2 (int64 ring, int64 &startpix, int64 &ringpix,
  double &theta, bool &shifted) const
  {
  int64 northring = (ring>2*nside_) ? 4*nside_-ring : ring;
  if (northring < nside_)
    {
    double tmp = northring*northring*fact2_;
    double costheta = 1 - tmp;
    double sintheta = sqrt(tmp*(2-tmp));
    theta = atan2(sintheta,costheta);
    ringpix = 4*northring;
    shifted = true;
    startpix = 2*northring*(northring-1);
    }
  else
    {
    theta = acos((2*nside_-northring)*fact1_);
    ringpix = 4*nside_;
    shifted = ((northring-nside_) & 1) == 0;
    startpix = ncap_ + (northring-nside_)*ringpix;
    }
  if (northring != ring) // southern hemisphere
    {
    theta = pi-theta;
    startpix = npix_ - startpix - ringpix;
    }
  }

void Healpix_Base2::neighbors (int64 pix, fix_arr<int64,8> &result) const
  {
  int ix, iy, face_num;
  (scheme_==RING) ?
    ring2xyf(pix,ix,iy,face_num) : nest2xyf(pix,ix,iy,face_num);

  const int64 nsm1 = nside_-1;
  if ((ix>0)&&(ix<nsm1)&&(iy>0)&&(iy<nsm1))
    {
    if (scheme_==RING)
      for (int m=0; m<8; ++m)
        result[m] = xyf2ring(ix+nb_xoffset[m],iy+nb_yoffset[m],face_num);
    else
      {
      int64 fpix = int64(face_num)<<(2*order_),
            px0=spread_bits(ix  ), py0=spread_bits(iy  )<<1,
            pxp=spread_bits(ix+1), pyp=spread_bits(iy+1)<<1,
            pxm=spread_bits(ix-1), pym=spread_bits(iy-1)<<1;

      result[0] = fpix+pxm+py0; result[1] = fpix+pxm+pyp;
      result[2] = fpix+px0+pyp; result[3] = fpix+pxp+pyp;
      result[4] = fpix+pxp+py0; result[5] = fpix+pxp+pym;
      result[6] = fpix+px0+pym; result[7] = fpix+pxm+pym;
      }
    }
  else
    {
    for (int i=0; i<8; ++i)
      {
      int x=ix+nb_xoffset[i], y=iy+nb_yoffset[i];
      int nbnum=4;
      if (x<0)
        { x+=nside_; nbnum-=1; }
      else if (x>=nside_)
        { x-=nside_; nbnum+=1; }
      if (y<0)
        { y+=nside_; nbnum-=3; }
      else if (y>=nside_)
        { y-=nside_; nbnum+=3; }

      int f = nb_facearray[nbnum][face_num];
      if (f>=0)
        {
        int bits = nb_swaparray[nbnum][face_num>>2];
        if (bits&1) x=nside_-x-1;
        if (bits&2) y=nside_-y-1;
        if (bits&4) std::swap(x,y);
        result[i] = (scheme_==RING) ? xyf2ring(x,y,f) : xyf2nest(x,y,f);
        }
      else
        result[i] = -1;
      }
    }
  }

void Healpix_Base2::get_interpol (const pointing &ptg, fix_arr<int64,4> &pix,
  fix_arr<double,4> &wgt) const
  {
  double z = cos (ptg.theta);
  int64 ir1 = ring_above(z);
  int64 ir2 = ir1+1;
  double theta1, theta2, w1, tmp, dphi;
  int64 sp,nr;
  bool shift;
  int64 i1,i2;
  if (ir1>0)
    {
    get_ring_info2 (ir1, sp, nr, theta1, shift);
    dphi = twopi/nr;
    tmp = (ptg.phi/dphi - .5*shift);
    i1 = (tmp<0) ? int64(tmp)-1 : int64(tmp);
    w1 = (ptg.phi-(i1+.5*shift)*dphi)/dphi;
    i2 = i1+1;
    if (i1<0) i1 +=nr;
    if (i2>=nr) i2 -=nr;
    pix[0] = sp+i1; pix[1] = sp+i2;
    wgt[0] = 1-w1; wgt[1] = w1;
    }
  if (ir2<(4*nside_))
    {
    get_ring_info2 (ir2, sp, nr, theta2, shift);
    dphi = twopi/nr;
    tmp = (ptg.phi/dphi - .5*shift);
    i1 = (tmp<0) ? int64(tmp)-1 : int64(tmp);
    w1 = (ptg.phi-(i1+.5*shift)*dphi)/dphi;
    i2 = i1+1;
    if (i1<0) i1 +=nr;
    if (i2>=nr) i2 -=nr;
    pix[2] = sp+i1; pix[3] = sp+i2;
    wgt[2] = 1-w1; wgt[3] = w1;
    }

  if (ir1==0)
    {
    double wtheta = ptg.theta/theta2;
    wgt[2] *= wtheta; wgt[3] *= wtheta;
    double fac = (1-wtheta)*0.25;
    wgt[0] = fac; wgt[1] = fac; wgt[2] += fac; wgt[3] +=fac;
    pix[0] = (pix[2]+2)%4;
    pix[1] = (pix[3]+2)%4;
    }
  else if (ir2==4*nside_)
    {
    double wtheta = (ptg.theta-theta1)/(pi-theta1);
    wgt[0] *= (1-wtheta); wgt[1] *= (1-wtheta);
    double fac = wtheta*0.25;
    wgt[0] += fac; wgt[1] += fac; wgt[2] = fac; wgt[3] =fac;
    pix[2] = ((pix[0]+2)&3)+npix_-4;
    pix[3] = ((pix[1]+2)&3)+npix_-4;
    }
  else
    {
    double wtheta = (ptg.theta-theta1)/(theta2-theta1);
    wgt[0] *= (1-wtheta); wgt[1] *= (1-wtheta);
    wgt[2] *= wtheta; wgt[3] *= wtheta;
    }

  if (scheme_==NEST)
    for (tsize m=0; m<pix.size(); ++m)
      pix[m] = ring2nest(pix[m]);
  }

void Healpix_Base2::swap (Healpix_Base2 &other)
  {
  std::swap(order_,other.order_);
  std::swap(nside_,other.nside_);
  std::swap(npface_,other.npface_);
  std::swap(ncap_,other.ncap_);
  std::swap(npix_,other.npix_);
  std::swap(fact1_,other.fact1_);
  std::swap(fact2_,other.fact2_);
  std::swap(scheme_,other.scheme_);
  }

double Healpix_Base2::max_pixrad() const
  {
  vec3 va,vb;
  va.set_z_phi (2./3., pi/(4*nside_));
  double t1 = 1.-1./nside_;
  t1*=t1;
  vb.set_z_phi (1-t1/3, 0);
  return v_angle(va,vb);
  }
