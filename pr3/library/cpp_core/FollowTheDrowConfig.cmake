add_library(FollowTheDrow STATIC IMPORTED)
find_library(LIBRARY_PATH FollowTheDrow HINTS ${CMAKE_CURRENT_LIST_DIR}/../lib)
set_target_properties(FollowTheDrow PROPERTIES IMPORTED_LOCATION ${LIBRARY_PATH})
