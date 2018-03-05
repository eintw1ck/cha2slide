set(LATEX_COMPILER_FLAGS "-interaction=nonstopmode -shell-escape")
set(PDFLATEX_COMPILER_FLAGS "-interaction=nonstopmode -shell-escape")

file(COPY prototype.c DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_latex_document(SOURCE writeup.tex)
