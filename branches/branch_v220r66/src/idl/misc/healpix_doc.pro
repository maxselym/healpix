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
pro healpix_doc, help=help, html=html, pdf=pdf, whole=whole
;+
; NAME:
;         Healpix_doc
;
;
; PURPOSE:
;         displays html or pdf IDL-Healpix documentation
;
;
; CALLING SEQUENCE:
;         healpix_doc, [HTML= | PDF=] [, HELP=,  WHOLE=]
;
;
; KEYWORD PARAMETERS:
;
;         HELP= shows this header and exits
;
;         HTML=  if set, the HTML documentation is shown using
;            the web browser found by the $IDL_DIR/bin/online_help_html script
;            If the browser is already in use, a new tab is open.
;
;
;         PDF=   if set, the PDF documentation is shown using
;            the pdf viewer found by the $IDL_DIR/bin/online_help_pdf script
;
;            Either HTML or PDF must be set.
;
;
;         WHOLE=  if set, the whole Healpix documentation is accessible,
;              not just the IDL related part.
;
; SIDE EFFECTS:
;      
;       will start (or open a new tab in) a pdf viewer or html browser
;
;
; PROCEDURE:
;        
;       call ONLINE_HELP
;
;
; EXAMPLE:
;
;       healpix_doc, /html
;
;
; MODIFICATION HISTORY:
;        created 2009-10-07 by EH.
;        2010-03-12: opens main.htm instead of index.htm when used with /html,/whole
;-

routine = 'healpix_doc'
syntax = routine+', [HELP=, HTML=, PDF=, WHOLE=]'
do_whole = keyword_set(whole)
do_pdf = keyword_set(pdf)
do_html = keyword_set(html)

if keyword_set(help) then begin
    doc_library,routine
    return
endif

if (n_params() ne 0 || do_pdf eq do_html) then begin
    print,syntax
    print, ' '
    print,'Choose either /html or /pdf'
    print, ' '
    print,'Type '+routine+',/help'
    print,'   for more help.'
    return
endif


init_healpix

found = where( tag_names(!healpix.path) eq 'DOC', nfound)
nfound2 = (nfound eq 1) ? N_tags(!healpix.path.doc) : 0

if (nfound2 ge 2) then begin
    ; use !healpix.path.doc.(*) if they are defined
    path_pdf  = !healpix.path.doc.pdf
    path_html = !healpix.path.doc.html
endif else begin
    ; otherwise, build path with !healpix.directory
    sep = path_sep()
    path_main  = !healpix.directory+sep+'doc'+sep
    path_pdf   = path_main + 'pdf'+sep
    path_html  = path_main + 'html'+sep
endelse

if keyword_set(pdf) then begin

    if (do_whole) then begin
;;        print,'WARNING: ',routine,': keyword WHOLE ignored in pdf mode'
        doc_path = path_pdf + 'pdf_index.pdf'
    endif else begin
        doc_path = path_pdf + 'idl.pdf'
    endelse

endif else begin

    if (do_whole) then begin
;;        doc_path = path_html + 'index.htm'
        doc_path = path_html + 'main.htm'  ; corrected 2010-03-12
    endif else begin
        doc_path = path_html + 'idl.htm'
    endelse

endelse

if (~file_test(doc_path)) then begin
    print,'ERROR: documentation file ('+doc_path+') not found.'
endif else begin
    online_help, /full_path, book=doc_path
endelse


return
end




