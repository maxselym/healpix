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

;=============================================================================

PRO orthcursor, cursor_type=cursor_type
;+
;=-----------------------------------------------------------------------------
; NAME:   
;     ORTHCURSOR
; PURPOSE:
;     get sky position (long, lat), number and value of a pixel
;     selected on a Healpix or QuadCube map in Orthview projection 
; CALLING SEQUENCE:
;       ORTHCURSOR, [CURSOR_TYPE=]
;
; for more details see MOLLCURSOR
;=-----------------------------------------------------------------------------
;-

mollcursor, cursor_type = cursor_type

return
end

