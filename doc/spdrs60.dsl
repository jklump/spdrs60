<!DOCTYPE style-sheet PUBLIC
          "-//James Clark//DTD DSSSL Style Sheet//EN" [
<!ENTITY % html "IGNORE">
<![%html;[
<!ENTITY % print "IGNORE">
<!ENTITY docbook.dsl PUBLIC
         "-//Norman Walsh//DOCUMENT DocBook HTML Stylesheet//EN"
         CDATA dsssl>
]]>
<!ENTITY % print "INCLUDE">
<![%print;[
<!ENTITY docbook.dsl PUBLIC
         "-//Norman Walsh//DOCUMENT DocBook Print Stylesheet//EN"
         CDATA dsssl>
]]>
]>

<style-sheet>

<!--      PRINT       -->
<style-specification id="print" use="docbook">
<style-specification-body> 

;;; TeX backend can go to PS (where EPS is needed)
;;; or to PDF (where PNG is needed).  So, just
;;; omit the file extension altogether and let
;;; tex/pdfjadetex sort it out on its own.
(define (graphic-file filename)
 (let ((ext (file-extension filename)))
  (if (or (equal? 'backend 'tex) ;; Leave off the extension for TeX
          (not filename)
          (not %graphic-default-extension%)
          (member ext %graphic-extensions%))
      filename
      (string-append filename "." %graphic-default-extension%))))

;;; Full justification.
(define %default-quadding%
 'justify)

;;; To make URLs line wrap we use the TeX 'url' package.
;;; See also: jadetex.cfg
;; First we need to declare the 'formatting-instruction' flow class.
(declare-flow-object-class formatting-instruction
"UNREGISTERED::James Clark//Flow Object Class::formatting-instruction")
;; Then redefine ulink to use it.
(element ulink
  (make sequence
    (if (node-list-empty? (children (current-node)))
        ; ulink url="...", /ulink
        (make formatting-instruction
          data: (string-append "\\url{"
                               (attribute-string (normalize "url"))
                               "}"))
        (if (equal? (attribute-string (normalize "url"))
                    (data-of (current-node)))
        ; ulink url="http://...", http://..., /ulink
            (make formatting-instruction data:
                  (string-append "\\url{"
                                 (attribute-string (normalize "url"))
                                 "}"))
        ; ulink url="http://...", some text, /ulink
            (make sequence
              ($charseq$)
              (literal " (")
              (make formatting-instruction data:
                    (string-append "\\url{"
                                   (attribute-string (normalize "url"))
                                   "}"))
              (literal ")"))))))
;;; And redefine filename to use it too.
(element filename
  (make formatting-instruction
    data: (string-append "\\path{" (data-of (current-node)) "}")))

;; customize the print stylesheet
(define %paper-type%
  ;; Name of paper type
  "A4")

(define %two-side% #t)

;; center images
(element imagedata
  (if (have-ancestor? (normalize "mediaobject"))
    ($img$ (current-node) #t)                 
    ($img$ (current-node) #f)))

;; set font-size to 11pt
(define %bf-size%
  (case %visual-acuity%
        (("normal") 11pt)
        (("presbyopic") 13pt)
        (("large-type") 24pt)))
(define-unit em %bf-size%)

</style-specification-body>
</style-specification>

<!--      HTML       -->
<style-specification id="html" use="docbook">
<style-specification-body> 

(define %stylesheet%
 "../spdrs60.css")

(define %css-decoration%
 #t)

(define %generate-article-toc%
 #t)

(define %use-id-as-filename%
 #t)

(define %shade-verbatim%
 #t)

(define %html-ext%
 ".html")

(define %root-filename%
 "index")

(define (chunk-skip-first-element-list)
 '())

(define (list-element-list)
 '())

(define (toc-depth nd)
 2)

(define %section-autolabel%
 #t)

</style-specification-body>
</style-specification>

<external-specification id="docbook" document="docbook.dsl">

</style-sheet>

