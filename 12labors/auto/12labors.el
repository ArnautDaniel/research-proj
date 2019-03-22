(TeX-add-style-hook
 "12labors"
 (lambda ()
   (TeX-add-to-alist 'LaTeX-provided-class-options
                     '(("article" "11pt" "twocolumn")))
   (TeX-run-style-hooks
    "latex2e"
    "article"
    "art11"
    "libertineotf"
    "csquotes"
    "mathtools"
    "amssymb")
   (TeX-add-symbols
    "oldemptyset"
    "emptyset"))
 :latex)

