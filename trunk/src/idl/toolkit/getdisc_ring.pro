; -----------------------------------------------------------------------------
;
;  Copyright (C) 1997-2010  Krzysztof M. Gorski, Eric Hivon, Anthony J. Banday
;
;
;
;
;
;  This file is part of HEALPix.
;
;  HEALPix is free software; you can redistribute it and/or modify
;  it under the terms of the GNU General Public License as published by
;  the Free Software Foundation; either version 2 of the License, or
;  (at your option) any later version.
;
;  HEALPix is distributed in the hope that it will be useful,
;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;  GNU General Public License for more details.
;
;  You should have received a copy of the GNU General Public License
;  along with HEALPix; if not, write to the Free Software
;  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
;
;  For more information about HEALPix see http://healpix.jpl.nasa.gov
;
; -----------------------------------------------------------------------------
;+
; NAME:
;  getdisc_ring
;
;  superseded by query_disc
;
;-

pro getdisc_ring, nside, vector0, radius_in, listpix, nlist, deg = deg

print,'***************************************************************'
print,'WARNING : GETDISC_RING is obsolete'
print,'WARNING : use query_disc instead (same syntax) which offers extra keywords (Nested, Inclusive)'
print,'***************************************************************'


query_disc, nside, vector0, radius_in, listpix, nlist, deg = deg

return
end

