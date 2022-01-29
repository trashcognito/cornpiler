cmake_policy(SET CMP0076 NEW)
file(GLOB cornpiler_SRC
     "*.cpp"
)

target_sources(cornpiler 
PUBLIC
${cornpiler_SRC}
)
