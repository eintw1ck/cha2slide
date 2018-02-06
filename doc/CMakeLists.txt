set(LATEX_COMPILER_FLAGS "-interaction=nonstopmode -shell-escape")
set(PDFLATEX_COMPILER_FLAGS "-interaction=nonstopmode -shell-escape")

#latex_get_output_path(output_dir)
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/prototype.c ${output_dir}/prototype.c COPYONLY)

add_latex_document(SOURCE writeup.tex)
